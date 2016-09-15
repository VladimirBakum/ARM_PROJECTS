#include <QtGui>

unsigned char array[0x1b00];
const unsigned char mask[8]={128,64,32,16,8,4,2,1};

const int blu[16]={0x00,0x80,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff};
const int red[16]={0x00,0x00,0x80,0x80,0x00,0x00,0x80,0x80,0x00,0x00,0xff,0xff,0x00,0x00,0xff,0xff};
const int grn[16]={0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff};

QPixmap view(char*);

int main(int narg, char* varg[])
{
	QApplication app(narg,varg);
	QDialog window;
	if (narg>1)
	{
		QLabel label;
		QPixmap viewer;
		char* name=varg[1];
		viewer=view(name).scaled(512,384);
		label.setPixmap(viewer);
		label.show();
		return app.exec();
	} else {
		qDebug() << "need filename as parameter\n";
		return 1;
	}
}

QPixmap view(char* name)
{
	FILE* filename;

	filename=fopen(name,"r");
	if (filename!=NULL)
	{
		QPixmap pixmap(256,192);
		int i,xcrd,ycrd,scradr,atradr,delta, col;
		QPainter painter;
		QColor qcolor;
		for (i=0; i<0x1b00; i++) {
			array[i]=fgetc(filename);
		}
		fclose(filename);

		painter.begin(&pixmap);
			for (ycrd=0; ycrd<192; ycrd++) {
				for (xcrd=0; xcrd<256; xcrd++) {
					scradr=(ycrd&0xc0)*32+(ycrd&0x38)*4+(ycrd&7)*256+(xcrd&0xf8)/8;
					atradr=0x1800+(ycrd&0xf8)*4+(xcrd&0xf8)/8;
					delta=xcrd&0x07;
					if ((array[scradr]&mask[delta])!=0) {
						col=array[atradr]&0x07+(array[atradr]&0x40)/8;
					} else {
						col=(array[atradr]&0x78)/8;
					}
					qcolor.setRgb(red[col],grn[col],blu[col]);
					painter.setPen(qcolor);
					painter.drawPoint(xcrd,ycrd);
				}
			}

		painter.end();
		return pixmap;
	} else {
		qDebug() << "no such file :" << name << "\n";
		return NULL;
	}
}
