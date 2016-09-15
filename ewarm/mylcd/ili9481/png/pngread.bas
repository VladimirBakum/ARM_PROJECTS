DECLARE FUNCTION bin$ (BYVAL num&)
'
' For Microsoft BASIC PDS 7.1
'    QBX /L to load QBX.QLB
'
' Need /L for use of CALL INTERRUPT for VESA graphics.

'$INCLUDE: 'qbx.bi'  ' BASIC PDS 7.1

' VESA graphics functions (very basic)
DECLARE SUB iPSET24 (BYVAL xx%, BYVAL yy%, BYVAL red%, BYVAL grn%, BYVAL blu%, BYVAL alpha%)
DECLARE SUB iPSET16 (BYVAL xx%, BYVAL yy%, BYVAL lowbyte%, BYVAL highbyte%, BYVAL alpha%)
DECLARE SUB VESAmode (BYVAL mode%)
DECLARE SUB VESAslice (BYVAL bank%)

DECLARE SUB Bswap (num&)
DECLARE SUB FillDstream ()
DECLARE SUB HuffmanFromLen (Tree() AS ANY, BitLen() AS INTEGER, BYVAL NumCodes%, BYVAL MaxBitLen%)
DECLARE SUB InflateHuffman (BYVAL Btype%)
DECLARE SUB InflateStore ()
DECLARE SUB TreeInflateDyn (TreeLL() AS ANY, TreeD() AS ANY)
DECLARE SUB TreeInflateFixed (TreeLL() AS ANY, TreeD() AS ANY)
DECLARE SUB unFilter (BYVAL inSeg%, BYVAL inPtr&, BYVAL inBytes&)

DECLARE FUNCTION PaethPredictor% (BYVAL a%, BYVAL b%, BYVAL c%)
DECLARE FUNCTION HuffmanDecodeSym% (Tree() AS ANY, BYVAL NumCodes%)
DECLARE FUNCTION ReadBit% ()
DECLARE FUNCTION ReadBits% (BYVAL bits%)
DECLARE FUNCTION zLibDecompress% ()

DIM Pal(0 TO 255) AS LONG  ' palette in 32 bit color
COMMON SHARED Pal() AS LONG

COMMON SHARED FileName$
COMMON SHARED FileNum%                 ' input file number
'COMMON SHARED OutFileNum%              ' output file number
COMMON SHARED FileSize&                ' LOF(FileNum%)
COMMON SHARED iEND%                    ' set to 1 when iEND is read by FillDstream
COMMON SHARED PNGer%

TYPE adam7

	Xinit AS INTEGER
	Yinit AS INTEGER
	Xinc AS INTEGER
	Yinc AS INTEGER
END TYPE

TYPE PNGheader ' starts @ offset 12 in file

	HeaderName      AS STRING * 4 ' "IHDR"
	wd              AS LONG       ' big endian
	ht              AS LONG       ' big endian
	BitDepth        AS STRING * 1
	ColorType       AS STRING * 1 ' 0=grey, 2=RGB, 3=palette, 4=grey+alpha, 6=RGBa
	CompressMethod  AS STRING * 1 ' 0
	FilterMethod    AS STRING * 1 ' 0
	InterlaceMethod AS STRING * 1 ' 0 or 1

END TYPE

TYPE PNGchunk

	'chCRC  AS LONG
	chLen  AS LONG
	chName AS STRING * 4

END TYPE

TYPE HuffEntry

	'Lengths AS INTEGER
	'Tree1d  AS LONG

	oz(0 TO 1) AS INTEGER
END TYPE

CONST NoLenCodes = 19
CONST NoDeflateCodeSymbols = 288
CONST NoDistanceSymbols = 32
CONST FirstLengthCodeIndex = 257
CONST LastLengthCodeIndex = 285

CONST Xmax = 1023
CONST Ymax = 767

COMMON SHARED info AS PNGheader
COMMON SHARED ColorType%, ColChan%, bpp%, interlace%
COMMON SHARED chunk AS PNGchunk

'
' incomming data stream (8kb at a time) and variables to access it.
'
COMMON SHARED Dstream AS STRING * 8192
COMMON SHARED DstreamPtr& ' offset from VARPTR(dstream) for unprocessed data
COMMON SHARED DstreamByt& ' bytes of unprocessed data @ dstreamPtr&
COMMON SHARED BitMask%    ' for reading a bit from stream

'
' outgoing data stream (32kb circular buffer / "sliding window" used by inflate)
'
' NOTE: OutStream() array is dimensioned below since it is dynamic
'
COMMON SHARED OutStream() AS INTEGER  ' 32k sliding window buffer
COMMON SHARED OutStreamLen& '    No. bytes read from file
			    '    AND with 32767 to get offset into OutStream()

' used by unFilter
COMMON SHARED PrevLine() AS INTEGER
COMMON SHARED CurLine() AS INTEGER
COMMON SHARED xpos%, ypos%, bytePP%, FilterType%, linebytes%

' arrays filled with constants from DATA statements, below
DIM BitVal(0 TO 15) AS INTEGER
DIM LengthBase(0 TO 28) AS INTEGER
DIM LengthExtra(0 TO 28) AS INTEGER
DIM DistanceBase(0 TO 29) AS INTEGER
DIM DistanceExtra(0 TO 29) AS INTEGER
DIM CLCLorder(0 TO 18) AS INTEGER
DIM adam7(1 TO 7) AS adam7

COMMON SHARED BitVal() AS INTEGER
COMMON SHARED LengthBase() AS INTEGER
COMMON SHARED LengthExtra() AS INTEGER
COMMON SHARED DistanceBase() AS INTEGER
COMMON SHARED DistanceExtra() AS INTEGER
COMMON SHARED CLCLorder() AS INTEGER
COMMON SHARED adam7() AS adam7

	RESTORE BitValues:
	FOR ck% = 0 TO 15
		READ BitVal(ck%)
	NEXT

	RESTORE Lbase:
	FOR ck% = 0 TO 28
		READ LengthBase(ck%)
	NEXT

	RESTORE Lextra:
	FOR ck% = 0 TO 28
		READ LengthExtra(ck%)
	NEXT

	RESTORE Dbase:
	FOR ck% = 0 TO 29
		READ DistanceBase(ck%)
	NEXT

	RESTORE Dextra:
	FOR ck% = 0 TO 29
		READ DistanceExtra(ck%)
	NEXT

	RESTORE Corder:
	FOR ck% = 0 TO 18
		READ CLCLorder(ck%)
	NEXT

	RESTORE adam7:
	FOR ck% = 1 TO 7

		READ adam7(ck%).Xinit
		READ adam7(ck%).Yinit
		READ adam7(ck%).Xinc
		READ adam7(ck%).Yinc
	NEXT

' The following array is our 32k sliding window.
' This is a dynamic array -- BASIC places it in its own segment @ offset 0

'$DYNAMIC
DIM OutStream(0 TO 16383) AS INTEGER
'$STATIC

SCREEN 0, 0, 0
VESAmode &H3

FILES "*.PNG"                      ' list files
INPUT "FileName -> "; FileName$    ' get user-input

IF DIR$(FileName$) = "" THEN

	PRINT "File not found."
	END
END IF

VESAmode &H117             ' 1024 x 768 x 16bpp
iPSET24 0, 0, 0, 0, 0, 255 ' ensure topmost slice is active

PRtop:

' open file/init file vars
FileNum% = FREEFILE
OPEN FileName$ FOR BINARY AS FileNum%

'' to dump raw bitmap data (after decoding) to disk
'OutFileNum% = FREEFILE
'OPEN "test.raw" FOR BINARY AS OutFileNum%

	FileSize& = LOF(FileNum%)

	sig$ = SPACE$(4)     ' read signature @ offset 0
	GET #1, , sig$

		IF sig$ <> "‰PNG" THEN

			PRINT "Signature mismatch"
			GOTO PNGexit:
		END IF

	GET #1, 12 + 1, info ' read header    @ offset 12

		IF info.HeaderName <> "IHDR" THEN

			PRINT "Signature mismatch"
			GOTO PNGexit
		END IF

		Bswap info.wd
		Bswap info.ht

		ColorType% = ASC(info.ColorType)
		SELECT CASE ColorType%

			CASE 0: ColChan% = 1  ' grey
			CASE 2: ColChan% = 3  ' RGB
			CASE 3: ColChan% = 1  ' palette
			CASE 4: ColChan% = 2  ' grey+alpha
			CASE 6: ColChan% = 4  ' RGB +alpha

			CASE ELSE

				PRINT "Invalid ColorType (" + STR$(ASC(info.ColorType)); ")"
				SLEEP
				END

		END SELECT

		interlace% = ASC(info.InterlaceMethod)

		IF ColChan% = 3 AND info.BitDepth = CHR$(5) THEN

			bpp% = 16
		ELSE
			bpp% = ColChan% * ASC(info.BitDepth)
		END IF

		' initialize variables used by unFilter()
		'-----------------------------------------------------------
		FilterType% = -1                  ' means "begin next line"
		bytePP% = (bpp% + 7) \ 8
		linebytes% = ((1& * info.wd * bpp%) + 7) \ 8&
		
		'$DYNAMIC
		DIM PrevLine(0 TO linebytes%) AS INTEGER
		DIM CurLine(0 TO linebytes%) AS INTEGER
		'$STATIC
		'-----------------------------------------------------------
		
	CriticalPOS% = 1  ' 1 after IHDR, 2 after PLTE, 3 after IDAT

	' loop through the chunks, ignorning unknown chunks and stopping at IEND
	'--------------------------------------------------------------------------
	SEEK #FileNum%, 33 + 1 ' -> first byte of first chunk after header
	DO
		'IF FileOffset& > (LOF(FileNum%) - 4) THEN
		'
		'        PRINT "Unexpected EOF"
		'        GOTO PNGexit
		'END IF

		GET #FileNum%, , chunk
		Bswap chunk.chLen  ' convert len from big -> little endian

		SELECT CASE chunk.chName

		CASE "PLTE"

			palSize% = chunk.chLen \ 3
			rgb$ = SPACE$(3)
			chr0$ = CHR$(0)

			FOR ck% = 0 TO palSize% - 1

				GET #FileNum%, , rgb$
				Pal(ck%) = CVL(rgb$ + chr0$)
			NEXT

			GET #FileNum%, , crc&

		CASE "IDAT"

			CriticalPOS% = 3

			'--------------------------------------------------
			' A summary of the "call stack" of functions here:
			'--------------------------------------------------
			'
			' zLibDecompress
			'   InflateHuffman
			'      InflateHuffmanDyn
			'          TreeInflateDyn ' builds TreeLL & TreeD
			'
			'          - inflates data according to TreeLL/TreeD
			'          - calls FillDstream as needed for new data
			'          - calls unFilter when an even 32k of data
			'                exists in OutStream.
			'
			'   unFilter (called with remaining data in OutStream)
			'
			r% = zLibDecompress

			' any errors?
			IF PNGer% THEN

				PRINT "PNG error "; PNGer%
				EXIT DO
			END IF

			IF iEND% THEN EXIT DO


		CASE "IEND": EXIT DO

		CASE ELSE

			' does file contain at least as many bytes as
			' this chunk claims to be?
			offset& = SEEK(FileNum%) + chunk.chLen + 4
			IF offset& >= FileSize& THEN

				PRINT "Bad file"
				GOTO PNGexit
			END IF

			' skip rest of this chunk
			SEEK #FileNum%, offset&

		END SELECT
	LOOP

PNGexit:

CLOSE FileNum%', OutFileNum%

BEEP
SLEEP

VESAmode &H3

END


BitValues:
DATA 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,-32768

Lbase:
' LengthBase(29) = base code lengths represented by codes 257 - 285
DATA 3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59
DATA 67,83,99,115,131,163,195,227,258

Lextra:
' LengthExtra(29) = the extra bits used by codes 257-285 (added to base length)
DATA 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0

Dbase:
' DistanceBase(30) = base backwards distances (distance codes are after length)
DATA 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537
DATA 2049,3073,4097,6145,8193,12289,16385,24577

Dextra:
' DistanceExtra(30) = the extra bits of backwards distances (added to base)
DATA 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13

Corder:
' CLCLorder(19) = Order in which "code length alphabet lengths" are stored.
'       Used to build the huffman tree of the dynamic huffman tree lengths.
DATA 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15


adam7:
' 7 passes
'    xinit,yinit, xinc, yinc
DATA     0,    0,    8,    8
DATA     4,    0,    8,    8
DATA     0,    4,    4,    8
DATA     2,    0,    4,    4
DATA     0,    2,    2,    4
DATA     1,    0,    2,    2
DATA     0,    1,    1,    2

FUNCTION bin$ (BYVAL num&)
'
' This function is for debugging...
'

IF num& AND &H80000000 THEN r$ = "1" ELSE r$ = "0"

FOR ck% = 30 TO 0 STEP -1

	IF num& AND (2 ^ ck%) THEN r$ = r$ + "1" ELSE r$ = r$ + "0"
NEXT

bin$ = r$

END FUNCTION

SUB Bswap (num&)
'
' Reverses bytes in NUM&
'

DEF SEG = VARSEG(num&)
r% = VARPTR(num&)

r1% = PEEK(r%)
r2% = PEEK(r% + 1)
r3% = PEEK(r% + 2)
r4% = PEEK(r% + 3)

POKE r%, r4%
POKE r% + 1, r3%
POKE r% + 2, r2%
POKE r% + 3, r1%

END SUB

SUB FillDstream
'
' Reads IDAT chunk(s) to fill dstream()
' Assumes chunk.chLen = bytes remaining in current iDAT chunk
'
' Adjusts DstreamByt& and DstreamPtr& by how much data was read.
' Sets iEND% = 1 if "IEND" chunk is found           (expected end of image)
' Sets iEND% = 2 if no more "IDAT" chunks are found (unexpected end of image)
'

DstreamPtr& = VARPTR(Dstream)

IF chunk.chLen >= LEN(Dstream) THEN
	'
	' Chunk contains at least 8k -- read 8k and we're done
	'
	GET #FileNum%, , Dstream

	DstreamByt& = LEN(Dstream)
	chunk.chLen = chunk.chLen - LEN(Dstream)
ELSE
	'
	' Chunk contains < 8k, so try to read more chunk(s) to fill
	'   the 8k.  If we can't, set iEND%
	'

	' read remainder of this chunk
	temp$ = SPACE$(chunk.chLen)
	GET #FileNum%, , temp$

	' find bytes we'd like to make 8k
	maxbytes% = LEN(Dstream) - chunk.chLen
	DO
		' read CRC?
		GET #FileNum%, , crc&

		' read header for next chunk
		GET #FileNum%, , chunk
		Bswap chunk.chLen

		IF chunk.chName = "IDAT" THEN
			'
			' This is a chunk of data for zlib data-stream
			'
			IF chunk.chLen >= maxbytes% THEN

				part$ = SPACE$(maxbytes%)
				chunk.chLen = chunk.chLen - maxbytes%
			ELSE
				part$ = SPACE$(chunk.chLen)
				chunk.chLen = 0
			END IF

			GET #FileNum%, , part$

			' add to our partial 8k, above
			temp$ = temp$ + part$

			' clear string to save string space
			part$ = ""

			' find remaining bytes needed to fill 8k
			maxbytes% = LEN(Dstream) - LEN(temp$)
		ELSE
			'
			' Some other chunk
			'
			SEEK #FileNum%, SEEK(FileNum%) - LEN(chunk)
			iEND% = 1
		END IF

	LOOP UNTIL (maxbytes% = 0) OR iEND%

	Dstream = temp$
	DstreamByt& = DstreamByt& + LEN(temp$)
END IF


END SUB

FUNCTION HuffmanDecodeSym% (Tree() AS HuffEntry, BYVAL NumCodes%)
'
'
'

TreePos% = 0
DO
	' decode the symbol from the Tree()
	' ReadBit() returns one bit from the data stream
	ct% = Tree(TreePos%).oz(ReadBit)

	IF ct% < NumCodes% THEN

		HuffmanDecodeSym% = ct%
		EXIT FUNCTION
	ELSE
		TreePos% = ct% - NumCodes%

		IF TreePos% >= NumCodes% THEN

			HuffmanDecodeSym% = -1
			EXIT FUNCTION
		END IF
	END IF
LOOP

END FUNCTION

SUB HuffmanFromLen (Tree() AS HuffEntry, BitLen() AS INTEGER, BYVAL NumCodes%, BYVAL MaxBitLen%)
'
' BitLen() - array of bit lengths   (0 to NumCodes%)
'
' Builds a "two dimensional" Huffman tree given only BitLen() and NumCodes%
'
' Returns:
'   0 - OK
'  55 - oversubscribed - can't build tree

REDIM BLcount&(0 TO MaxBitLen%)
REDIM NextCode&(0 TO MaxBitLen%)
REDIM Tree1d(0 TO NumCodes% - 1)  AS LONG

' Tree1d() stores a 1d representation of the huffman tree.
' We only need it to build the 2d version, which is what the function returns.

' 1. Count instances of each code length
FOR bits% = 0 TO NumCodes% - 1

	BLcount&(BitLen(bits%)) = BLcount&(BitLen(bits%)) + 1
NEXT

' 2. Generate the "nextcode" values
FOR bits% = 1 TO MaxBitLen%

	NextCode&(bits%) = (NextCode&(bits% - 1) + BLcount&(bits% - 1)) * 2&
NEXT

' 3. Generate all the codes
FOR ck% = 0 TO NumCodes% - 1

	IF BitLen(ck%) THEN
	
		'Tree(ck%).Tree1d = NextCode&(BitLen(ck%))
		Tree1d(ck%) = NextCode&(BitLen(ck%))
		NextCode&(BitLen(ck%)) = NextCode&(BitLen(ck%)) + 1
	END IF

	' initialize for next step..
	Tree(ck%).oz(0) = 32767
	Tree(ck%).oz(1) = 32767
NEXT

' Create 2d version of the tree:
' ------------------------------
' There are 2 rows, one for each bit value: .oz(0)
'                                           .oz(1)
' There are as many columns as codes - 1    Tree.(column%)
'
' Value of 32767 means "uninitialized"
' Value >= NumCodes is an address to another bit
' Value <  NumCodes is a code
'

' 4. Convert to "2d"
NodeFilled% = 0
TreePos% = 0
FOR n% = 0 TO NumCodes% - 1 ' N% is the "code"

	FOR i% = 0 TO BitLen(n%) - 1

		' Bit% = 0 or 1 ?
		bit% = 0
		'IF BitVal(BitLen(n%) - i% - 1) AND Tree(n%).Tree1d THEN bit% = 1
		IF BitVal(BitLen(n%) - i% - 1) AND Tree1d(n%) THEN bit% = 1

		' error?
		IF TreePos% > NumCodes% - 1 THEN PNGer% = 55: EXIT SUB

		IF Tree(TreePos%).oz(bit%) = 32767 THEN
			'
			' Not filled in yet..
			'
			IF (i% + 1) = BitLen(n%) THEN

				' last bit: Put the code in it
				Tree(TreePos%).oz(bit%) = n%
				TreePos% = 0
			ELSE
				' store address of next step (address = NodeFilled% + 1)
				' NOTE: addresses are encoded with NumCodes% added to them..
				NodeFilled% = NodeFilled% + 1
				Tree(TreePos%).oz(bit%) = NodeFilled% + NumCodes%

				TreePos% = NodeFilled%
			END IF
		ELSE
			TreePos% = Tree(TreePos%).oz(bit%) - NumCodes%
		END IF
	NEXT
NEXT

FOR n% = 0 TO NumCodes% - 1

	IF Tree(n%).oz(0) = 32767 THEN Tree(n%).oz(0) = 0
	IF Tree(n%).oz(1) = 32767 THEN Tree(n%).oz(1) = 0
NEXT

END SUB

SUB InflateHuffman (BYVAL Btype%)
'
'
'

REDIM TreeLL(0 TO NoDeflateCodeSymbols - 1) AS HuffEntry' "lit,len"  codes
REDIM TreeD(0 TO NoDistanceSymbols - 1)     AS HuffEntry' "distance" codes

IF Btype% = 1 THEN

	TreeInflateFixed TreeLL(), TreeD()

ELSEIF Btype% = 2 THEN

	TreeInflateDyn TreeLL(), TreeD()
END IF

' OutStream() is a 32k data buffer of data to be unfiltered/rendered to screen
'   The buffer is circular -- when we fill the last byte, we begin again at
'   byte 0.
'
' We incriment OutStreamLen& with each new byte
'   By ANDing with 127, we get our current offset into OutStream 32k buffer.
'   When OutStreamLen AND 32767 = 32767, we unfilter the buffer to the screen.
'
'   We must keep at least 32k of output in memory because the "sliding
'   window" used by Deflate is up to 32k.
'

' decode all symbols until end is reached
DO
	CodeLL% = HuffmanDecodeSym(TreeLL(), NoDeflateCodeSymbols)

	SELECT CASE CodeLL%

	CASE IS <= 255
		'
		' literal symbol (byte)
		'

		' find offset in OutStream: it is wrapped @ 32k
		r% = OutStreamLen& AND 32767

		' store in outstream
		DEF SEG = VARSEG(OutStream(0))
		POKE r%, CodeLL%
		
		' if we just filled the last byte of the 32k window,
		' write the entire window to the screen
		IF r% = 32767 THEN

			unFilter VARSEG(OutStream(0)), 0, 32768
		END IF

		OutStreamLen& = OutStreamLen& + 1
		
	CASE FirstLengthCodeIndex TO LastLengthCodeIndex
		'
		' repeated symbol
		'

		' get "base length"
		Length% = LengthBase(CodeLL% - FirstLengthCodeIndex)

		' no. extra bits?
		r% = LengthExtra(CodeLL% - FirstLengthCodeIndex)

		' read extra bits / add value to Length%
		Length% = Length% + ReadBits(r%)

		' get distance code
		CodeD% = HuffmanDecodeSym(TreeD(), NoDistanceSymbols)

		' error? (invalid distance code)
		IF CodeD% > 29 THEN PNGer% = 18: EXIT DO

		' get "base distance"
		distance% = DistanceBase(CodeD%)

		' no. extra bits?
		r% = DistanceExtra(CodeD%)

		' read extra bits / add value to Distance%
		distance% = distance% + ReadBits(r%)
		
		' error? (can't go back further than 32k or output so far)
		IF distance% > OutStreamLen& THEN PNGer% = 52: EXIT DO

		' fill in OutStream() based on Length% and Distance%
		OutPos& = OutStreamLen&
		Start& = OutPos&
		Backward& = Start& - distance%
		
		FOR ck% = 1 TO Length%

			DEF SEG = VARSEG(OutStream(0))
			POKE OutPos& AND 32767, PEEK(Backward& AND 32767)

			OutPos& = OutPos& + 1
			Backward& = Backward& + 1

			' did we just write last byte of 32k window?
			' if so, write it to the screen.
			IF (OutStreamLen& AND 32767) = 32767 THEN

				unFilter VARSEG(OutStream(0)), 0, 32768
			END IF

			OutStreamLen& = OutStreamLen& + 1

			IF Backward& >= Start& THEN Backward& = Start& - distance%
		NEXT

	CASE 256
		'
		' End Code: exit loop
		'
		EXIT DO

	CASE ELSE
		'
		' Error
		'
		IF iEND% THEN

			PNGer% = 10 ' no endcode (if EOF reached..)
		ELSE
			PNGer% = 11 ' wrong jump outside of tree?
		END IF

		EXIT DO

	END SELECT
LOOP

END SUB

SUB InflateStore
'
' Not done yet... no example images were found..
'



END SUB

SUB iPSET16 (BYVAL xx%, BYVAL yy%, BYVAL lowbyte%, BYVAL highbyte%, BYVAL alpha%)
'
' Sets a pixel using a 16bit rgb input value.
'

STATIC prevslice%

offset& = ((yy% AND 31) * 2048&) + (xx% + xx%) ' offset into slice
slice% = yy% \ 32                              ' slice

IF slice% <> prevslice% THEN

	VESAslice slice%
	prevslice% = slice%
END IF

' poke into video memory
DEF SEG = &HA000
POKE offset&, lowbyte%
POKE offset& + 1, highbyte%

END SUB

SUB iPSET24 (BYVAL xx%, BYVAL yy%, BYVAL red%, BYVAL grn%, BYVAL blu%, BYVAL alpha%)
'
' Sets a pixel using a 24bit input value.
'

STATIC prevslice%

offset& = ((yy% AND 31) * 2048&) + (xx% + xx%) ' offset into slice
slice% = yy% \ 32                              ' slice

IF slice% <> prevslice% THEN

	VESAslice slice%
	prevslice% = slice%
END IF

' convert RGB to 16 bits
'---------------------------------
'  (blue  is bits   4, 3, 2, 1, 0)
'  (green is bits  10, 9, 8, 7, 6, 5)
'  ( red  is bits  15,14,13,12,11)

a& = alpha% \ 16

red% = red% AND (128 + 64 + 32 + 16 + 8)    ' AND red%,11111000b
grn% = grn% AND (128 + 64 + 32 + 16 + 8 + 4)' AND grn%,11111100b

blu% = blu% \ 8                             ' SHR blu%,3
grn% = grn% * 8                             ' SHL grn%,3

DEF SEG = &HA000

iLo% = blu% OR grn%        ' =         gggBBBBB
iHi% = red% OR (grn% \ 256)' = RRRRRggg

IF a& <> (1 + 2 + 4 + 8) THEN

	d1& = PEEK(offset&) + 256& * PEEK(offset& + 1) ' d1& = background
	s1& = (iLo% AND 255) + (256& * (iHi% AND 255)) ' s1& = source pixel

	' d2& / s2& is RED and BLUE bits
	d2& = d1& AND (32768 + 16384 + 8192 + 4096 + 2048 + 16 + 8 + 4 + 2 + 1)
	s2& = s1& AND (32768 + 16384 + 8192 + 4096 + 2048 + 16 + 8 + 4 + 2 + 1)

	' d1& / s1& is GREEN bits
	d1& = d1& AND (1024 + 512 + 256 + 128 + 64 + 32)
	s1& = s1& AND (1024 + 512 + 256 + 128 + 64 + 32)

	' apply our formula:
	''' result = [a * (s + 64 - d)] / 16 + [d - (a * 4)]
	'
	' result = [a * (s - d)] / 16 + d

	result1& = (a& * (s1& - d1&)) \ 16 + d1&
	result2& = (a& * (s2& - d2&)) \ 16 + d2&

	result1& = result1& AND (1024 + 512 + 256 + 128 + 64 + 32)
	result2& = result2& AND (32768 + 16384 + 8192 + 4096 + 2048 + 16 + 8 + 4 + 2 + 1)

	result1& = result1& OR result2&

	iLo% = result1& AND 255
	iHi% = result1& \ 256
END IF

' poke into video memory
POKE offset&, iLo%
POKE offset& + 1, iHi%

END SUB

FUNCTION PaethPredictor% (BYVAL a%, BYVAL b%, BYVAL c%)

pa% = ABS(b% - c%)
pb% = ABS(a% - c%)
pc% = ABS(a% + b% - c% - c%)

IF (pc% < pa%) AND (pc% < pb%) THEN

	PaethPredictor% = c%

ELSEIF pb% < pa% THEN

	PaethPredictor% = b%

ELSE
	PaethPredictor% = a%
END IF

END FUNCTION

FUNCTION ReadBit%
'
' Reads one bit from Dstream()
' Adjusts BitMask%, DstreamPtr&, DstreamByt& as necesary
' Calls FillDstream if necesary

IF DstreamByt& < 1 THEN

	FillDstream
	IF DstreamByt& < 1 THEN EXIT FUNCTION
END IF

' return a one if bit is set
DEF SEG = VARSEG(Dstream)
IF PEEK(DstreamPtr&) AND BitMask% THEN ReadBit% = 1

BitMask% = BitMask% * 2

' 1 2 4 8 16 32 64 128
IF BitMask% > 128 THEN

	BitMask% = 1
	DstreamByt& = DstreamByt& - 1
	DstreamPtr& = DstreamPtr& + 1
END IF

END FUNCTION

FUNCTION ReadBits% (BYVAL bits%)
'
' Calls ReadBit to get BITS% bits.
' Fÿÿst b
'

bitvalue% = 1

FOR ck% = 1 TO bits%

	IF ReadBit THEN r% = r% + bitvalue%
	bitvalue% = bitvalue% * 2
NEXT

ReadBits% = r%

END FUNCTION

SUB TreeInflateDyn (TreeLL() AS HuffEntry, TreeD() AS HuffEntry)
'
' Get the tree of a deflated block with dynamic tree.
' The tree itself is also compressed with a known tree (called TreeCL)
'

hlit% = ReadBits(5) + 257 ' No. literal/length codes + 257
hdist% = ReadBits(5) + 1  ' No. distance codes
hclen% = ReadBits(4) + 4  ' No.ÿengt

REDIM BitLenCL%(0 TO NoLenCodes - 1)
REDIM TreeCL(0 TO NoLenCodes - 1) AS HuffEntry

' read bit lengths for TreeCL.
' they are stored in a weird order... re-order based on CLCLorder() constants
FOR ck% = 0 TO NoLenCodes - 1

	IF ck% < hclen% THEN BitLenCL%(CLCLorder(ck%)) = ReadBits(3)
NEXT

' build TreeCL (the tree used to read TreeLL and TreeD)
HuffmanFromLen TreeCL(), BitLenCL%(), NoLenCodes, 7

ERASE BitLenCL%

' use TreeCL() to read lengths for trees this function returns
REDIM BitLenLL%(0 TO NoDeflateCodeSymbols - 1)
REDIM BitLenD%(0 TO NoDistanceSymbols - 1)

' i% is the current symbol we're reading in the part that contains
'    the code lengths of lit/len and dist codes?
i% = 0
DO WHILE i% < (hlit% + hdist%)

	code% = HuffmanDecodeSym(TreeCL(), NoLenCodes)
	' 0-15: code lengths
	'   16: copy previous 3-6 times
	'   17: 3-10 zeros
	'   18: 11-138 zeros

	SELECT CASE code%

	' Length code
	CASE IS <= 15

		IF i% < hlit% THEN

			BitLenLL%(i%) = code%
		ELSE
			BitLenD%(i% - hlit%) = code%
		END IF
		i% = i% + 1

	' Repeat previous
	CASE 16
		' can't repeat previous if there is no previous..
		IF i% = 0 THEN PNGer% = 54: EXIT SUB
		
		' how many repetitions?
		r% = 3 + ReadBits(2)

		' value to repeat?
		IF i% <= hlit% THEN

			value% = BitLenLL%(i% - 1)
		ELSE
			value% = BitLenD%(i% - hlit% - 1)
		END IF

		' repeat (value%) r% times
		FOR ck% = 1 TO r%

			' i% cannot exceed amount of codes..
			IF i% >= (hlit% + hdist%) THEN PNGer% = 13: EXIT SUB

			IF i% < hlit% THEN

				BitLenLL%(i%) = value%
			ELSE
				BitLenD%(i% - hlit%) = value%
			END IF

			i% = i% + 1
		NEXT

	' Repeat "0" 3-10 times
	CASE 17
		r% = 3 + ReadBits(3)

		' repeat (0) r% times
		FOR ck% = 1 TO r%

			' i% cannot exceed amount of codes
			IF i% >= (hlit% + hdist%) THEN PNGer% = 14: EXIT SUB

			IF i% < hlit% THEN

				BitLenLL%(i%) = 0
			ELSE
				BitLenD%(i% - hlit%) = 0
			END IF

			i% = i% + 1
		NEXT

	' Repeat "0" 11-138 times
	CASE 18
		r% = 11 + ReadBits(7)

		' repeat (0) r% times
		FOR ck% = 1 TO r%

			' i% cannot exceed amount of codes
			IF i% >= (hlit% + hdist%) THEN PNGer% = 14: EXIT SUB

			IF i% < hlit% THEN

				BitLenLL%(i%) = 0
			ELSE
				BitLenD%(i% - hlit%) = 0
			END IF

			i% = i% + 1
		NEXT

	' Error..
	CASE ELSE
		'IF code% = -1 THEN
		'        '
		'        ' -1 returned by HuffmanDecodeSym()
		'        '                indicates error
		'        '
		'
		'ELSE
			PNGer% = 16
			EXIT SUB
		'END IF

	END SELECT
LOOP

' length of the end code 256 must be > 0
IF BitLenLL%(256) = 0 THEN PNGer% = 64: EXIT SUB

ERASE TreeCL

' we have HLIT% and HDIST%
' now we generate code trees
HuffmanFromLen TreeLL(), BitLenLL%(), NoDeflateCodeSymbols, 15
HuffmanFromLen TreeD(), BitLenD%(), NoDistanceSymbols, 15

ERASE BitLenLL%, BitLenD%

END SUB

SUB TreeInflateFixed (TreeLL() AS HuffEntry, TreeD() AS HuffEntry)
'
'
'
'

' build TreeLL()
REDIM BitLenLL%(0 TO NoDeflateCodeSymbols - 1)

FOR ck% = 0 TO 143: BitLenLL%(ck%) = 8: NEXT
FOR ck% = 144 TO 255: BitLenLL%(ck%) = 9: NEXT
FOR ck% = 256 TO 279: BitLenLL%(ck%) = 7: NEXT
FOR ck% = 280 TO 287: BitLenLL%(ck%) = 8: NEXT

HuffmanFromLen TreeLL(), BitLenLL%(), NoDeflateCodeSymbols, 15


' build TreeD()
REDIM BitLenD%(0 TO NoDistanceSymbols - 1)

FOR ck% = 0 TO NoDistanceSymbols - 1
	BitLenD%(ck%) = 5
NEXT

HuffmanFromLen TreeD(), BitLenD%(), NoDistanceSymbols, 15

END SUB

SUB unFilter (BYVAL inSeg%, BYVAL inPtr&, BYVAL inBytes&)
'
' Called by InflateHuffman when an even 32k is in OutStream
'   or called by zLibDecompress after all data has been inflated.
'
' This function reads bytes from inSeg% : inPtr& and processes inBytes& bytes.
'
' The data stream passed to this function may end at any point.
'    This function will return and pick up where it left off on next call.
'
' The data stream passed to this function is 1 byte of "filter type"
'    followed by SLbytes% of image data:
'
'    Non-interlaced images: SLbytes% = LineBytes% (calculated from header)
'    Interlaced images:     SLbytes% is calculated below according to which
'        stage of the "Adam7" interlace scheme we are in.
'
' The filter type is remembered, and the remaining data for a line is copied
'    into CurLine().
'
' CurLine() is then "unfiltered" according to the filter type byte.
'    The unfiltered data is stored back into CurLine() and a copy is also
'    kept in PrevLine() since some filters refer to the "previous line".
'
' When a complete line of data is processed, it is written to the screen
'    using iPSET24 or iPSET16.
'
'
' NOTE: When there was no "previous line", PrevLine() is initialized to all 0.
'
' Uses shared variables:
'       xpos% - x possition on screen (-1 means read FilterType)
'       ypos% - y possition on screen (starts @ 0)
'  linebytes% - width of image, in bytes
'     bytePP% - bytes per pixel
'  colortype% - input color format (always outputs in 16-bit color)
'        bpp% - input bits per pixel
'  interlace% - 0 for normal display
'               1 - 7 for Adam7 interlace
'

STATIC totalbytes&
totalbytes& = totalbytes& + inBytes&

STATIC i%
STATIC SLbytes%     ' bytes per line, this line
		    ' (same as LineBytes% if not using Adam7 interlace)

STATIC Xinc%
STATIC Xinit%
STATIC Yinc%

DO WHILE inBytes&

	' read FilterType (means "start new row")
	IF FilterType% = -1 THEN
	
		DEF SEG = inSeg%
		FilterType% = PEEK(inPtr&)

		inPtr& = inPtr& + 1
		inBytes& = inBytes& - 1

		IF interlace% THEN
			'
			' interlaced
			'
			IF Yinc% = 0 THEN
				'
				' First row: initialize local static vars
				'
				ypos% = adam7(1).Yinit
				Yinc% = adam7(1).Yinc

			ELSEIF ypos% >= info.ht THEN
				'
				' Reached bottom of image: next interlace pass
				'
				
				interlace% = interlace% + 1

				ypos% = adam7(interlace%).Yinit
				Yinc% = adam7(interlace%).Yinc
			END IF

			Xinc% = adam7(interlace%).Xinc
			Xinit% = adam7(interlace%).Xinit

			SLbytes% = bytePP% * ((info.wd - Xinit% + Xinc% - 1) \ Xinc%)
		ELSE
			'
			' non-interlaced
			'
			Xinc% = 1
			Xinit% = 0
			Yinc% = 1

			SLbytes% = linebytes%
		END IF

		'IF ypos% >= info.ht THEN
		'
		'        interlace% = interlace% + 1
		'        ypos% = adam7(interlace%).Yinit
		'END IF

		i% = 0
	END IF

	' read more into CurLine ?
	IF i% < SLbytes% THEN

		DEF SEG = inSeg%
		FOR i% = i% TO SLbytes% - 1
	
			' if we exit here, we have only a partial line
			' so we wait to be called again to fill it before
			' writing it to screen.
			IF inBytes& = 0 THEN

				'IF ypos% >= info.ht - 1 THEN STOP
				EXIT DO
			END IF

			CurLine(i%) = PEEK(inPtr&)

			inPtr& = inPtr& + 1
			inBytes& = inBytes& - 1
		NEXT
	END IF

	' apply the filter
	SELECT CASE FilterType%

		CASE 0  ' "none"

			' (no action required here... data in CurLine is output without filtering)
			
		CASE 1  ' "sub" (add to left pixel)

			FOR ck% = bytePP% TO SLbytes% - 1

				CurLine(ck%) = (CurLine(ck%) + CurLine(ck% - bytePP%)) AND 255
			NEXT

		CASE 2  ' "up"  (add to pixel above)
			FOR ck% = 0 TO SLbytes% - 1

				CurLine(ck%) = (CurLine(ck%) + PrevLine(ck%)) AND 255
			NEXT
			
		CASE 3  ' "average"

			' first bytePP% bytes
			FOR ck% = 0 TO bytePP% - 1

				CurLine(ck%) = (CurLine(ck%) + PrevLine(ck%) \ 2) AND 255
			NEXT

			' remaining bytes
			FOR ck% = bytePP% TO SLbytes% - 1

				CurLine(ck%) = (CurLine(ck%) + (CurLine(ck% - bytePP%) + PrevLine(ck%)) \ 2) AND 255
			NEXT
			
		CASE 4 ' average

			' first bytePP% bytes
			FOR ck% = 0 TO bytePP% - 1

				CurLine(ck%) = (CurLine(ck%) + PrevLine(ck%)) AND 255' PaethPredictor(0, PrevLine(ck%), 0) is always PrevLine(ck%)
			NEXT

			' remaining bytes
			FOR ck% = bytePP% TO SLbytes% - 1

				CurLine(ck%) = (CurLine(ck%) + PaethPredictor(CurLine(ck% - bytePP%), PrevLine(ck%), PrevLine(ck% - bytePP%))) AND 255
			NEXT

		CASE ELSE

			' invalid filter type!
			STOP

	END SELECT

	' cause us to begin next line, above
	FilterType% = -1

	' render this row to screen (check keyboard for abort..)
	IF ypos% <= Ymax THEN

		IF LEN(INKEY$) THEN
		 
			SLEEP
			VESAmode &H3
			SCREEN 0, 0, 0
			END
		END IF

		ck% = 0
		SELECT CASE bpp%

		CASE 1: mask% = 128                ' high bit set
		CASE 2: mask% = 128 + 64           ' high 2 bits set
		CASE 4: mask% = 128 + 64 + 32 + 16 ' high 4 bits set
		CASE 8: mask% = 255

		END SELECT

		FOR xpos% = Xinit% TO info.wd - 1 STEP Xinc%

			IF xpos% > Xmax THEN EXIT FOR

			SELECT CASE ColorType%

			CASE 0 ' greyscale

				SELECT CASE bpp%

				CASE 1
					r% = 0
					IF CurLine(ck%) AND mask% THEN r% = 255

					mask% = mask% \ 2
					IF mask% = 0 THEN

						mask% = 128
						ck% = ck% + 1
					END IF

				CASE 2
					SELECT CASE CurLine(ck%) AND mask%

					' no bits set (black)
					CASE 0: r% = 0

					' all bits set (white)
					CASE (128 + 64), (32 + 16), (8 + 4), (2 + 1): r% = 255

					' high bit set
					CASE 128, 32, 8, 2: r% = 128

					' low bit set
					CASE ELSE: r% = 64

					END SELECT

					mask% = mask% \ 4
					IF mask% = 0 THEN

						mask% = 128 + 64
						ck% = ck% + 1
					END IF

				CASE 4
					IF mask% = 128 + 64 + 32 + 16 THEN

						r% = (CurLine(ck%) AND (128 + 64 + 32 + 16)) \ 16

						mask% = 1 + 2 + 4 + 8
						ck% = ck% + 1
					ELSE
						r% = CurLine(ck%) AND (1 + 2 + 4 + 8)

						mask% = 128 + 64 + 32 + 16
					END IF


				CASE 8
					r% = CurLine(ck%)
					

				END SELECT

				iPSET24 xpos%, ypos%, r%, r%, r%, 255

			CASE 3 ' palette

				SELECT CASE bpp%

				CASE 1
					r& = VARPTR(Pal((CurLine(ck%) AND mask%) \ mask%))

					mask% = mask% \ 2
					IF mask% = 0 THEN
						mask% = 128
						ck% = ck% + 1
					END IF

				CASE 2

					SELECT CASE CurLine(ck%) AND mask%

					' no bits set (black)
					CASE 0: r% = 0

					' all bits set
					CASE (128 + 64), (32 + 16), (8 + 4), (2 + 1): r% = 3

					' high bit set
					CASE 128, 32, 8, 2: r% = 2

					' low bit set
					CASE ELSE: r% = 1

					END SELECT

					r& = VARPTR(Pal(r%))

					mask% = mask% \ 4
					IF mask% = 0 THEN

						mask% = 128 + 64
						ck% = ck% + 1
					END IF

				CASE 4

					IF mask% = (128 + 64 + 32 + 16) THEN

						r% = (CurLine(ck%) AND mask%) \ 16
						mask% = 1 + 2 + 4 + 8
					ELSE
						r% = CurLine(ck%) AND mask%
						mask% = 128 + 64 + 32 + 16
						ck% = ck% + 1
					END IF
					r& = VARPTR(Pal(r%))


				CASE 8
					r& = VARPTR(Pal(CurLine(ck%)))
					ck% = ck% + bytePP%

				END SELECT

				DEF SEG = VARSEG(Pal(0))
				iPSET24 xpos%, ypos%, PEEK(r&), PEEK(r& + 1), PEEK(r& + 2), 255

			CASE 2, 6'RGB / RGBa

				SELECT CASE bytePP%

				CASE 2
					iPSET16 xpos%, ypos%, CurLine(ck%), CurLine(ck% + 1), 255

				CASE 3

					iPSET24 xpos%, ypos%, CurLine(ck%), CurLine(ck% + 1), CurLine(ck% + 2), 255

				CASE 4
					iPSET24 xpos%, ypos%, CurLine(ck%), CurLine(ck% + 1), CurLine(ck% + 2), CurLine(ck% + 3)

					'' for file dump
					'r$ = CHR$(CurLine(ck%)) + CHR$(CurLine(ck% + 1)) + CHR$(CurLine(ck% + 2))
					'PUT #OutFileNum%, , r$

				CASE IS > 4

					iPSET24 xpos%, ypos%, CurLine(ck%), CurLine(ck% + 2), CurLine(ck% + 4), 255

				END SELECT


				'IF bytePP% > 4 THEN
				'
				'        iPSET24 xpos%, ypos%, CurLine(ck%), CurLine(ck% + 2), CurLine(ck% + 4)
				'ELSE
				'
				'        iPSET24 xpos%, ypos%, CurLine(ck%), CurLine(ck% + 1), CurLine(ck% + 2)
				'
				'        ' for file dump
				'        r$ = CHR$(CurLine(ck%)) + CHR$(CurLine(ck% + 1)) + CHR$(CurLine(ck% + 2))
				'        PUT #OutFileNum%, , r$
				'END IF

				ck% = ck% + bytePP%
				
			CASE 4 ' 8 bit greyscale / 8 bit alpha

				iPSET24 xpos%, ypos%, CurLine(ck%), CurLine(ck%), CurLine(ck%), 255
				ck% = ck% + bytePP%

			END SELECT
		NEXT
		
		ypos% = ypos% + Yinc%
	END IF

	' copy current row to "previous row"
	FOR ck% = 0 TO SLbytes% - 1
	
		PrevLine(ck%) = CurLine(ck%)
	NEXT
LOOP

END SUB

SUB VESAmode (BYVAL mode%)

DIM reg AS RegType

reg.ax = &H4F02
reg.bx = mode%
Interrupt &H10, reg, reg


END SUB

SUB VESAslice (BYVAL bank%)

DIM reg AS RegType

reg.ax = &H4F05
reg.bx = 0
reg.cx = 0
reg.dx = bank%

Interrupt &H10, reg, reg

END SUB

FUNCTION zLibDecompress%
'
' Reads zLib header from Dstream
' Calls InflateHuffman / InflateStore on each Deflated block.
'
' Returns:

DIM zLibEr AS INTEGER ' error

DIM cm AS INTEGER
DIM cinfo  AS INTEGER
DIM fdict  AS INTEGER

FillDstream              ' get some data from disk (reads up to 8k each call)
IF DstreamByt& < 2 THEN  ' did we get at least 2 bytes?

	zLibEr% = 53
	GOTO ZLDend:
END IF

DEF SEG = VARSEG(Dstream)

cm = PEEK(DstreamPtr&) AND 15
cinfo = (PEEK(DstreamPtr&) \ 16) AND 15
fdict = (PEEK(DstreamPtr& + 1) \ 32) AND 1

' check stuff
'------------------------------------
IF (cm <> 8) OR (cinfo > 7) THEN

	zLibEr% = 25 ' PNG spec only uses compression method 8
	GOTO ZLDend: ' "inflate with sliding window of 32k"

ELSEIF fdict <> 0 THEN

	zLibEr% = 26 ' PNG spec: "additional flags shall not specify
	GOTO ZLDend: '              a preset dictionary"
END IF

' adjust variables to remove zlib header
DstreamPtr& = DstreamPtr& + 2  ' used by ReadBit / ReadBits / FillDstream
DstreamByt& = DstreamByt& - 2  ' used by ReadBit / ReadBits / FillDstream
BitMask% = 1                   ' used by ReadBit / ReadBits

'-----------------------
' INFLATE
'-----------------------

DO
	
	Bfinal% = ReadBit
	Btype% = ReadBit + (2 * ReadBit)' NOTE: BASIC processes left to right

	' Bfinal% = 1 when this is the last block
	' Btype%      indicates type of block
	
	SELECT CASE Btype%

	CASE 0: InflateStore

	CASE 3
		zLibEr% = 20 ' invalid BTYPE
		GOTO ZLDend:

	CASE ELSE: InflateHuffman (Btype%)

	END SELECT

LOOP UNTIL Bfinal%

' commit remaining OutStream() to screen
unFilter VARSEG(OutStream(0)), 0, OutStreamLen& AND 32767

ZLDend:




END FUNCTION

