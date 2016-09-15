//-----------------------------------------------------
// ��������������� ��������� ��� Stellaris Launchpad
// Version 1.0 by (R)soft 2013-2014
// SYSCTL_SYSDIV_2, SYSCTL_USE_PLL, SYSCTL_XTAL_12MHZ -> 88 MHz Clock -> UART0 Work OK
// SYSCTL_SYSDIV_2, SYSCTL_USE_PLL, SYSCTL_XTAL_10MHZ -> 109 MHz Clock -> UART0 Work OK



#define KEY1            GPIO_PIN_0      // 0x01
#define RED_LED         GPIO_PIN_1 // ���� ����� ��� 0x02
#define BLUE_LED        GPIO_PIN_2 // ���� ����� ��� 0x04
#define GREEN_LED       GPIO_PIN_3
#define KEY2            GPIO_PIN_4      // 0x10

#define RED_LED_1   HWREG(GPIO_PORTF_AHB_BASE | (RED_LED << 2)) = RED_LED;
#define RED_LED_0   HWREG(GPIO_PORTF_AHB_BASE | (RED_LED << 2)) = 0;
#define BLUE_LED_1  HWREG(GPIO_PORTF_AHB_BASE | (BLUE_LED << 2)) = BLUE_LED;
#define BLUE_LED_0  HWREG(GPIO_PORTF_AHB_BASE | (BLUE_LED << 2)) = 0;
#define GREEN_LED_1  HWREG(GPIO_PORTF_AHB_BASE | (GREEN_LED << 2)) = GREEN_LED;
#define GREEN_LED_0  HWREG(GPIO_PORTF_AHB_BASE | (GRENN_LED << 2)) = 0;
#define KEY1_1  HWREG(GPIO_PORTF_AHB_BASE | (KEY1 << 2)) = KEY1;
#define KEY1_0  HWREG(GPIO_PORTF_AHB_BASE | (KEY1 << 2)) = 0;
#define KEY2_1  HWREG(GPIO_PORTF_AHB_BASE | (KEY2 << 2)) = KEY2;
#define KEY2_0  HWREG(GPIO_PORTF_AHB_BASE | (KEY2 << 2)) = 0;



const unsigned int LEDS_ALL = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;

const unsigned int SW1 = GPIO_PIN_0;
const unsigned int SW2 = GPIO_PIN_4;

//-------------------------------------------------------------
// �������� � �� ��������� �������� ��� ��������� 1...20 ��
void delay_ms (unsigned int value)
{
  ROM_SysCtlDelay( ((ROM_SysCtlClockGet()/1000)/3) *value ) ;  // more accurate
}

//-------------------------------------------------------------
// �������� � ��� ��������� �������� ��� ��������� 1...40 ���
void delay_us (unsigned int value)
{
  if (value>10)
  {
    ROM_SysCtlDelay( ((ROM_SysCtlClockGet()/1000000)/6)*(value*2) ) ;  // more accurate
  }
  else
  {
    ROM_SysCtlDelay( 16 * value ) ;
  }
}

//---------------------------------------------
// ������������ UART5

void configure_uart5(void) //485 
{ 

  // Enable PortE 
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); 

  // Enable  UART5 
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5); 
  
  // Configure the pin multiplexing 
  GPIOPinConfigure(GPIO_PE4_U5RX); 
  GPIOPinConfigure(GPIO_PE5_U5TX); 

  // Configure the type of the pins for UART Tx/Rx 
  GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4|GPIO_PIN_5); 

//  UARTStdioConfig(UART5_BASE, 9600, MAP_SysCtlClockGet());

//  IntEnable(INT_UART5);

}




//-----------------------------------------------------
// ������ ��������������� ���� PF0
// ������ ������� �� ����������� ����������� ����� Reset:
// PA[1:0] UART0 GPIOPCTL=0x01 (����� �� �������)
// PA[5:2] SSI0  GPIOPCTL=0x02 (����� �� �������)
// PB[3:2] I2C0  GPIOPCTL=0x03 (����� �� �������)
// PC[3:0] JTAG/SWD GPIOCTL=0x01 (����� ������ �� �������)
// PD7 NMI (����� ��������������)
// PF0 NMI (����� ��������������)
void unlock_PF0 (void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  HWREG(GPIO_PORTF_BASE | GPIO_O_LOCK) = GPIO_LOCK_KEY_DD; // ������������� (������ DD!!!)
  HWREG(GPIO_PORTF_BASE | GPIO_O_CR) |= GPIO_PIN_0; // ��������� ������ � �������� AFSEL,PUR,PDR � DEN
                                                     // �����. ��������� ����
  HWREG(GPIO_PORTF_BASE | GPIO_O_AFSEL) &= ~GPIO_PIN_0; // ���� ����� 1 �� ������ �� ��������� (����� ������)
                                                // ���� 0 - ������������.(Alternate Function Select)
  HWREG(GPIO_PORTF_BASE | GPIO_O_DEN) &= ~GPIO_PIN_0; // Digital Enable
  HWREG(GPIO_PORTF_BASE | GPIO_O_PCTL) = 0x00; // Port Control - ������������� ���� ���� F ��� GPIO
  
  HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x00; // �������� ����������
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
}


void unlock_PD7 (void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  HWREG(GPIO_PORTD_BASE | GPIO_O_LOCK) = GPIO_LOCK_KEY_DD; // ������������� (������ DD!!!)
  HWREG(GPIO_PORTD_BASE | GPIO_O_CR) |= GPIO_PIN_7; // ��������� ������ � �������� AFSEL,PUR,PDR � DEN
                                                     // �����. ��������� ����
  HWREG(GPIO_PORTD_BASE | GPIO_O_AFSEL) &= ~GPIO_PIN_7; // ���� ����� 1 �� ������ �� ��������� (����� ������)
                                                // ���� 0 - ������������.(Alternate Function Select)
  HWREG(GPIO_PORTD_BASE | GPIO_O_DEN) &= ~GPIO_PIN_7; // Digital Enable
  HWREG(GPIO_PORTD_BASE | GPIO_O_PCTL) = 0x00; // Port Control - ������������� ���� ���� F ��� GPIO
  
  HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0x00; // �������� ����������
  HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
}



//--------------------------------------------------------
// ��������� ������������� ������������ � ��������� ���

void system_init (void)
{
   // ��������� ������������ 80 ��� �� PLL � ������� �����������.
  // ������ ������������ ������� �������� ������� ������� 200 ���
  // �� �������� SYSCTL_SYSDIV_x
  //  ROM_SysCtlClockSet(SYSCTL_SYSDIV_2 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |SYSCTL_XTAL_12_2MHZ);
  //  ROM_SysCtlClockSet(SYSCTL_SYSDIV_2 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |SYSCTL_XTAL_13_5MHZ); // 25 MHz OSC
  // ��������!!! ��� ������� 100 ��� UART �������� �����������, ���������� ����� :(

  SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

  unlock_PF0();
  unlock_PD7();
    
  // Port F
  //  GPIOPinConfigure(GPIO_PF0_U1RTS);
  // ��������� � ������������ ����� F GPIO ��� ������ � LED
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  
  SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOF); // Advanced BUS Enable
  GPIOPinTypeGPIOOutput(GPIO_PORTF_AHB_BASE, 0xFF );
  GPIOPadConfigSet(GPIO_PORTF_AHB_BASE, 0xFF, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
  
  // Port A
  // (��� SD �������� ���� ���� ��������� �.�. SD �������� �� � AHB!)
  // � ����� ���� ���� ��������� ����� � �������������
  /*SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOA); // Advanced BUS Enable
  GPIOPinTypeGPIOOutput(GPIO_PORTA_AHB_BASE, 0xFF );
  GPIOPadConfigSet(GPIO_PORTA_AHB_BASE, 0xFF, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
  */
  
  // ������� ���� ����� A
  /*SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, 0xFF );
  GPIOPadConfigSet(GPIO_PORTA_BASE, 0xFF, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
  */
  
  // Port D
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOD); // Advanced BUS Enable
  GPIOPinTypeGPIOOutput(GPIO_PORTD_AHB_BASE, 0xFF );
  GPIOPadConfigSet(GPIO_PORTD_AHB_BASE, 0xFF, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

  
  // Port E
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOE); // Advanced BUS Enable
  GPIOPinTypeGPIOOutput(GPIO_PORTE_AHB_BASE, 0xFF );
  GPIOPadConfigSet(GPIO_PORTE_AHB_BASE, 0xFF, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

  
  
//  GPIO_PORTD_DIR_R = 0xFF;
//  GPIO_PORTD_DEN_R = 0xFF;

}



/*
void pins_dropped (void)
{
    HWREG(GPIO_PORTF_AHB_BASE | (0xFF << 2)) = 0xFF;
    HWREG(GPIO_PORTA_AHB_BASE | (0xFF << 2)) = 0xFF;
    HWREG(GPIO_PORTD_AHB_BASE | (0xFF << 2)) = 0xFF;
    HWREG(GPIO_PORTE_AHB_BASE | (0xFF << 2)) = 0xFF;    
    delay_us(10);
    HWREG(GPIO_PORTF_AHB_BASE | (0xFF << 2)) = 0;
    HWREG(GPIO_PORTA_AHB_BASE | (0xFF << 2)) = 0;
    HWREG(GPIO_PORTD_AHB_BASE | (0xFF << 2)) = 0;
    HWREG(GPIO_PORTE_AHB_BASE | (0xFF << 2)) = 0;    
    delay_us(10);
}
*/


    /*    
    unsigned int state = ~GPIOPinRead(GPIO_PORTF_BASE, SW1 | SW2);
    led = ((state & SW1) << 1) | ((state & SW2) >> 1);
    GPIOPinWrite(GPIO_PORTF_BASE, LEDS_ALL, led);
    */


//--------------------------------------------
// �������� ������� ����� ��� ������ ������.
// ���������� ��� ������� ������.
//  Usage: key=wait_key();
unsigned char  wait_key(void)
{
  unsigned char tmp;
  while(1)
  {
  KEY1_1; // ��������� ���� ������ � 1
  KEY2_1;

  // ���� ������ �� ����
  GPIODirModeSet(GPIO_PORTF_AHB_BASE, 0x11, GPIO_DIR_MODE_IN);
  
  tmp = GPIOPinRead(GPIO_PORTF_AHB_BASE, 0x11);
  tmp = tmp & 0x11;
  if (tmp==KEY1 || tmp==KEY2) break;
    
  GPIODirModeSet(GPIO_PORTF_AHB_BASE, 0x11, GPIO_DIR_MODE_OUT);
  
  };

//  UARTprintf("\nKEY=0x%02x\n", tmp);
  return (tmp);
}

//-----------------------------------------
// ������ ����� ���������� ��������� �����
// ����� �������� ���������, �� ����� �������������� ������ ��������� �����
#ifndef MICROSD
void
SysTickHandler(void)
{
 // Nothing   
}
#endif



//---------------------------------------------------
// ��������� ������ ������, ������� �� ��������

/*
char g_ulMode; //char uButton = 0;

void AppButtonHandler(void)
{

  unsigned char ucButtons;
  unsigned char ucButtonsChanged;

    //
    // Grab the current, debounced state of the buttons.
    //
    ucButtons = ButtonsPoll(&ucButtonsChanged, 0);

    //
    // If the left button has been pressed, and was previously not pressed,
    // start the process of changing the behavior of the JTAG pins.
    //
    if(BUTTON_PRESSED(LEFT_BUTTON, ucButtons, ucButtonsChanged))
    {
        //
        // Toggle the pin mode.
        //
        g_ulMode ^= 1;

        //
        // See if the pins should be in JTAG or GPIO mode.
        //
        if(g_ulMode == 0)
        {
            //
            // Change PC0-3 into hardware (i.e. JTAG) pins.
            //

            //
            // Turn on the LED to indicate that the pins are in JTAG mode.
            //
            ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_1, 
                             GPIO_PIN_3);
        }
        else
        {
            //
            // Change PC0-3 into GPIO inputs.
            //
            ROM_GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, (GPIO_PIN_0 | GPIO_PIN_1 |
                                                       GPIO_PIN_2 | GPIO_PIN_3));

            //
            // Turn off the LED to indicate that the pins are in GPIO mode.
            //
            ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_1,
                             GPIO_PIN_1);
        }
    }
}
*/

/*
void crSet(unsigned int base, unsigned char value)
{
    unsigned long v = (HWREG(GPIO_PORTF_BASE + GPIO_O_CR) & 0xFFFFFF00) | value;
    HWREG(base + GPIO_O_CR) = v;
}
*/

/*
void myproc (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH2_GPIOF);
    SysCtlDelay(2);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LEDS_ALL);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, SW1 | SW2);

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
    crSet(GPIO_PORTF_BASE, 1);
    GPIOPadConfigSet(GPIO_PORTF_BASE, SW1 | SW2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    crSet(GPIO_PORTF_BASE, 0);
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
}
*/



