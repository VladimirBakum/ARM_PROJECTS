// Увеличение значения стека pulStack в startup_ewarm.c до значения 256 спасло ситуацию
// с нормальным запуском программы!!! Значение по умолчанию - 64
// Т.к. я однажды спалил порт PB0, и чтобы оживить один канал звука, поменял местами
// таймеры 2 и 3. Соответственно теперь звук будет на PB2 & PB3 :o)
// Проверялось с microSD Kingston 2Gb with FAT32 - полет нормальный.
// Складываем файлы в каталог mods
// МОДы с большим кол-вом каналов (~14) подтормаживают.
// ====
// Ewarm Stellaris LM4F120H5QR realization by (R)soft 17-04-2014
//---------------------------------------------------------------------------------

#include "inc/hw_ints.h" // for interrupts
#include "inc/hw_memmap.h"
//#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "inc/hw_timer.h"
//#include "driverlib/debug.h"
//#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h" // for interrupts
//#include "driverlib/pin_map.h"
//#include "driverlib/pwm.h"
#include "driverlib/rom.h" // for ROM_ procedures
#include "driverlib/sysctl.h"
#include "driverlib/systick.h" // for systick
#include "driverlib/timer.h"
#include "utils/uartstdio.h"    // for uart

#include "global.h" // for global definitions
#include "drivers/buttons.h"
#include "fatfs/ff.h"   // for fat system
#include "fatfs/diskio.h"

//*****************************************************************************
//
// Define pin to LED color mapping.
//
//*****************************************************************************

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

//TODO: I think this is some button input
#define ENCODERPORT 0

unsigned long g_ulFlags;
unsigned long g_ulTest;

char uButton = 0;
char uButtonPrev = 0;

//#define CHECKMEMORY {if(file.size!=19367){ROM_IntMasterDisable(); UARTprintf("uhOh (%s) size: %d at:%d\n",__FILE__,file.size,__LINE__); while(1){}}}

void main() 
{
  WORD i = 0;
  BYTE mode = 0;
  BYTE oldEncoderBtn = 0;

  unsigned long ulPeriod;

  //
  // Enable lazy stacking for interrupt handlers.  This allows floating-point
  // instructions to be used within interrupt handlers, but at the expense of
  // extra stack usage.
  //
  ROM_FPULazyStackingEnable();


  //
  // Setup the system clock to run at 80 Mhz from PLL with crystal reference
  //
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ	| SYSCTL_OSC_MAIN);

  //Enable uart
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioInit(0);
  UARTprintf("\033[2J*** Stellaris Launchpad S3M&MOD Player ***\n");
  UARTprintf("Ewarm version by (R)soft 2014\n");

  // Turn off LEDs
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

  //RED setting
  TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
  TimerLoadSet(TIMER0_BASE, TIMER_B,  0xFFFF);
  TimerMatchSet(TIMER0_BASE, TIMER_B, 0); // PWM

  //Blue
  TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
  TimerLoadSet(TIMER1_BASE, TIMER_A,  0xFFFF);
  TimerMatchSet(TIMER1_BASE, TIMER_A, 0x0); // PWM

  //Green
  TimerLoadSet(TIMER1_BASE, TIMER_B,  0xFFFF);
  TimerMatchSet(TIMER1_BASE, TIMER_B, 0); // PWM

  //Invert input
  HWREG(TIMER0_BASE + TIMER_O_CTL)   |= 0x4000;
  HWREG(TIMER1_BASE + TIMER_O_CTL)   |= 0x40;
  HWREG(TIMER1_BASE + TIMER_O_CTL)   |= 0x4000;

  TimerEnable(TIMER0_BASE, TIMER_BOTH);
  TimerEnable(TIMER1_BASE, TIMER_BOTH);

  GPIOPinConfigure(GPIO_PF3_T1CCP1);
  GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_3);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

  GPIOPinConfigure(GPIO_PF2_T1CCP0);
  GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

  GPIOPinConfigure(GPIO_PF1_T0CCP1);
  GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_1);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
  //GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
  //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);

  //GPIOPinConfigure(GPIO_PF2_T1CCP0);
  //GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2);

  //Left stereo channel Timer3
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  GPIOPinConfigure(GPIO_PB3_T3CCP1); //GPIOPinConfigure(GPIO_PB4_T1CCP0);
  GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_3);

  //right stereo channel
  //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  GPIOPinConfigure(GPIO_PB2_T3CCP0); // 
  GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_2);

  // Configure timer left Timer3
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
  TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
  TimerLoadSet(TIMER3_BASE, TIMER_A,  1 << BITDEPTH);
  TimerMatchSet(TIMER3_BASE, TIMER_A, 0); // PWM
  //TimerEnable(TIMER2_BASE, TIMER_A);

  // Configure timer right
  //SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  //TimerConfigure(TIMER2_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
  TimerLoadSet(TIMER3_BASE, TIMER_B,  1 << BITDEPTH);
  TimerMatchSet(TIMER3_BASE, TIMER_B, 0); // PWM
  TimerEnable(TIMER3_BASE, TIMER_BOTH);

  //
  // Sampler timer Timer2
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
  TimerConfigure(TIMER2_BASE, TIMER_CFG_32_BIT_PER);
  ulPeriod = SYSCLK / SAMPLERATE;
  TimerLoadSet(TIMER2_BASE, TIMER_A, ulPeriod+1);

  TimerEnable(TIMER2_BASE, TIMER_A);


  //
  // Initialize the buttons
  //
  ButtonsInit();

  // Initialize the SysTick interrupt to process colors and buttons.
  //
  SysTickPeriodSet(SysCtlClockGet() / 16);
  SysTickEnable();


  //set priorities
  IntPrioritySet(INT_TIMER2A,0x00);
  IntPrioritySet(FAULT_SYSTICK,0x80);

  //
  // Enable interrupts to the processor.
  //
  ROM_IntMasterEnable();

  IntEnable(INT_TIMER2A);
  TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
  SysTickIntEnable();


  //TODO: uncomment this when we work with an SD card
  ROM_IntMasterDisable();
  UARTprintf("Init SD\n");
  while(disk_initialize(0));
  UARTprintf("Mounting FS\n");
  f_mount(0, &fso);
  UARTprintf("\n");
  f_chdir(PATH);
  UARTprintf("Opening %s\n",PATH);
  f_opendir(&dir, ".");

  loadNextFile();
  ROM_IntMasterEnable();

  while(1)
  {
    while((SoundBuffer.writePos + 1 & SOUNDBUFFERSIZE - 1) != SoundBuffer.readPos)
     {
       if(!i)
        {
          //encoderDifference = encoderPosition - oldEncoderPosition;
          //if(encoderDifference)
          if(uButton!=uButtonPrev)
           {
             uButtonPrev=uButton;
             if(!mode)
              {
                if(uButton & LEFT_BUTTON)
                 {
                    ROM_IntMasterDisable();
                    UARTprintf("Load previous module\n");
                    ROM_IntMasterEnable();
                    loadPreviousFile();
                  }
                if(uButton & RIGHT_BUTTON)
                 {
                    ROM_IntMasterDisable();
                    UARTprintf("Load Next module\n");
                    ROM_IntMasterEnable();
                    loadNextFile();
                  }
               } 
            }

          if(ENCODERBTNPIN && !oldEncoderBtn)
          if(++mode == 3)  mode = 0;
          oldEncoderBtn = ENCODERBTNPIN;

          player();
          i = getSamplesPerTick();
         }

         mixer();
         i--;
      }
   }

}

//*****************************************************************************
//
// The interrupt handler for the first timer interrupt.
// Timer2
//
//*****************************************************************************
void Timer2IntHandler(void)
{
  //
  // Clear the timer interrupt.
  //
  ROM_TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

  //
  // Toggle the flag for the first timer.
  //
  HWREGBITW(&g_ulFlags, 0) ^= 1;

  //
  // Use the flags to Toggle the LED for this timer
  //
  /*if(++g_ulTest>=9999){
          g_ulTest = 0;
          GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, g_ulFlags << 1);
  }*/

  if(SoundBuffer.writePos != SoundBuffer.readPos) 
  {
    //Sound output Timer3
    TimerMatchSet(TIMER3_BASE, TIMER_B, SoundBuffer.left[SoundBuffer.readPos]); // PWM
    TimerMatchSet(TIMER3_BASE, TIMER_A, SoundBuffer.right[SoundBuffer.readPos]); // PWM

    //Visualizer
    //RED led
    TimerMatchSet(TIMER0_BASE, TIMER_B, (SoundBuffer.left[SoundBuffer.readPos]-850)<<5);
    //Blue led
    TimerMatchSet(TIMER1_BASE, TIMER_A, (SoundBuffer.right[SoundBuffer.readPos]-850)<<5);
    SoundBuffer.readPos++;
    SoundBuffer.readPos &= SOUNDBUFFERSIZE - 1;
  }
}

void AppButtonHandler(unsigned long ulButtons)
{
  switch(ulButtons & ALL_BUTTONS)
  {
    case LEFT_BUTTON:
      uButton = LEFT_BUTTON;
      //ROM_IntMasterDisable();
      //UARTprintf("Left Button pressed\n");
      //ROM_IntMasterEnable();
      break;
    case RIGHT_BUTTON:
      uButton = RIGHT_BUTTON;
      //ROM_IntMasterDisable();
      //UARTprintf("Right Button pressed\n");
      //ROM_IntMasterEnable();
      break;
    case ALL_BUTTONS:
      uButton = ALL_BUTTONS;
      //ROM_IntMasterDisable();
      //UARTprintf("Both Button pressed\n");
      //ROM_IntMasterEnable();
      break;
    default:
      uButton = 0;
      break;
  }
}

void SysTickIntHandler(void)
{
  unsigned long ulButtons;

  ulButtons = ButtonsPoll(0,0);
  AppButtonHandler(ulButtons);
}

//*****************************************************************************
//
// The interrupt handler for the quardature encoder.
//
//*****************************************************************************
/*void
Timer1IntHandler(void)
{
	static BYTE AB;
	static BYTE oldAB = 0;
	static const char encoderStates[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0,
			1, 0, 1, -1, 0 };
	//
	// Clear the timer interrupt.
	//
	ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

	//
	// Toggle the flag for the second timer.
	//
	HWREGBITW(&g_ulFlags, 1) ^= 1;

	//
	// Use the flags to Toggle the LED for this timer
	//
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, g_ulFlags << 1);

	AB = ENCODERPORT & 3;

	encoderSubPosition += encoderStates[(oldAB << 2) + AB];
	oldAB = AB;

	if (encoderSubPosition > 3) {
		encoderPosition++;
		encoderSubPosition = 0;
	} else if (encoderSubPosition < -3) {
		encoderPosition--;
		encoderSubPosition = 0;
	}

}*/
