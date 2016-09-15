#include  "systemInit.h"   
#include  <stdio.h>   
//定义A B   
#define  A_PERIPH            SYSCTL_PERIPH_GPIOF   
#define  A_PORT              GPIO_PORTF_BASE   
#define  A_PIN               GPIO_PIN_1   
   
#define  B_PERIPH            SYSCTL_PERIPH_GPIOF   
#define  B_PORT              GPIO_PORTF_BASE   
#define  B_PIN               GPIO_PIN_2   
   
//  定义LED1和LED2   
#define  LED1_PERIPH            SYSCTL_PERIPH_GPIOD   
#define  LED1_PORT              GPIO_PORTD_BASE   
#define  LED1_PIN               GPIO_PIN_0   
   
#define  LED2_PERIPH            SYSCTL_PERIPH_GPIOG   
#define  LED2_PORT              GPIO_PORTG_BASE   
#define  LED2_PIN               GPIO_PIN_2   
   
   
void A_Init(void)                                            //编码开关A口输入中断初始化   
{   
    SysCtlPeriEnable(A_PERIPH);                              //  使能A所在的GPIO端口   
    GPIOPinTypeIn(A_PORT, A_PIN);                            //  设置A所在管脚为输入   
    //GPIOIntTypeSet(A_PORT, A_PIN, GPIO_LOW_LEVEL);          //  设置A的中断类型   
    //IntPrioritySet(INT_GPIOF, 1 << 5);                      //  设置A中断优先级为1   
    //GPIOPinIntEnable(A_PORT, A_PIN);                        //  使能A所在管脚的中断   
    //IntEnable(INT_GPIOF);                                   //  使能A端口中断   
}   
   
void B_Init(void)                                       //编码开关B口输入初始化   
{   
    SysCtlPeriEnable(B_PERIPH);                              //  使能A所在的GPIO端口   
    GPIOPinTypeIn(B_PORT, B_PIN);                            //  设置A所在管脚为输入   
}   
   
   
//  主函数（程序入口）   
int main(void)   
{   
    jtagWait();                                             //  防止JTAG失效，重要！   
    clockInit();                                            //  时钟初始化：晶振，6MHz   
    A_Init();   
    B_Init();   
   
    SysCtlPeriEnable(LED1_PERIPH);                          //  使能LED1所在的GPIO端口   
    GPIOPinTypeOut(LED1_PORT, LED1_PIN);                    //  设置LED1所在的管脚为输出   
    GPIOPinWrite(LED1_PORT, LED1_PIN, 0x01);                //  熄灭LED1   
   
    SysCtlPeriEnable(LED2_PERIPH);                          //  使能LED2所在的GPIO端口   
    GPIOPinTypeOut(LED2_PORT, LED2_PIN);                    //  设置LED2所在的管脚为输出   
    GPIOPinWrite(LED2_PORT, LED2_PIN, 1 << 2);              //  熄灭LED2   
   
    //IntMasterEnable();   
    static unsigned char Aold,Bold;   
    static unsigned char st;   
    unsigned long tmp=0;   
    for (;;)   
    {   
      if(GPIOPinRead(A_PORT,A_PIN)&&GPIOPinRead(B_PORT,B_PIN))   
      {   
        SysCtlDelay(2*(TheSysClock/3000));   
        if(GPIOPinRead(A_PORT,A_PIN)&&GPIOPinRead(B_PORT,B_PIN))   
        st=1;   
      }   
      if(st)   
      {   
        if(GPIOPinRead(A_PORT,A_PIN)==0&&GPIOPinRead(B_PORT,B_PIN)==0)   
        {   
          SysCtlDelay(2*(TheSysClock/3000));   
          if(GPIOPinRead(A_PORT,A_PIN)==0&&GPIOPinRead(B_PORT,B_PIN)==0)   
          {   
            if(Bold)   
            {   
              st=0;   
              tmp++;   
              GPIOPinWrite(LED2_PORT,LED2_PIN,1 << 2);   
              GPIOPinWrite(LED1_PORT,LED1_PIN,0x00);   
            }   
            if(Aold)   
            {   
              st=0;   
              tmp--;   
              GPIOPinWrite(LED1_PORT,LED1_PIN,0x01);   
              GPIOPinWrite(LED2_PORT,LED2_PIN,0x00);   
            }   
          }   
        }   
      }   
   
      Aold=GPIOPinRead(A_PORT,A_PIN);   
      Bold=GPIOPinRead(B_PORT,B_PIN);   
    }   
}   
   
/*void GPIO_Port_F_ISR(void)  
{  
    unsigned long ulStatus;  
  
    ulStatus = GPIOPinIntStatus(A_PORT, true);           //  读取中断状态  
    GPIOPinIntClear(A_PORT, ulStatus);                   //  清除中断状态，重要  
  
    if (ulStatus & A_PIN)                                //  如果KEY1的中断状态有效  
    {  
        SysCtlDelay(50*(TheSysClock/3000));  
        if ((GPIOPinRead(B_PORT,B_PIN))==0x00)                      // 顺时针LED1亮  
        { AA++;  
          GPIOPinWrite(LED2_PORT,LED2_PIN,1 << 2);  
          GPIOPinWrite(LED1_PORT,LED1_PIN,0x00);  
        }  
        else                                                        //逆时针LED2亮  
        { BB++;  
          GPIOPinWrite(LED1_PORT,LED1_PIN,0x01);  
          GPIOPinWrite(LED2_PORT,LED2_PIN,0x00);  
        }  
  
    }  
    for(;;)  
    {  
  
    }  
}*/   
   
