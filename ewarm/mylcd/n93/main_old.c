#include  "systemInit.h"   
#include  <stdio.h>   
//����A B   
#define  A_PERIPH            SYSCTL_PERIPH_GPIOF   
#define  A_PORT              GPIO_PORTF_BASE   
#define  A_PIN               GPIO_PIN_1   
   
#define  B_PERIPH            SYSCTL_PERIPH_GPIOF   
#define  B_PORT              GPIO_PORTF_BASE   
#define  B_PIN               GPIO_PIN_2   
   
//  ����LED1��LED2   
#define  LED1_PERIPH            SYSCTL_PERIPH_GPIOD   
#define  LED1_PORT              GPIO_PORTD_BASE   
#define  LED1_PIN               GPIO_PIN_0   
   
#define  LED2_PERIPH            SYSCTL_PERIPH_GPIOG   
#define  LED2_PORT              GPIO_PORTG_BASE   
#define  LED2_PIN               GPIO_PIN_2   
   
   
void A_Init(void)                                            //���뿪��A�������жϳ�ʼ��   
{   
    SysCtlPeriEnable(A_PERIPH);                              //  ʹ��A���ڵ�GPIO�˿�   
    GPIOPinTypeIn(A_PORT, A_PIN);                            //  ����A���ڹܽ�Ϊ����   
    //GPIOIntTypeSet(A_PORT, A_PIN, GPIO_LOW_LEVEL);          //  ����A���ж�����   
    //IntPrioritySet(INT_GPIOF, 1 << 5);                      //  ����A�ж����ȼ�Ϊ1   
    //GPIOPinIntEnable(A_PORT, A_PIN);                        //  ʹ��A���ڹܽŵ��ж�   
    //IntEnable(INT_GPIOF);                                   //  ʹ��A�˿��ж�   
}   
   
void B_Init(void)                                       //���뿪��B�������ʼ��   
{   
    SysCtlPeriEnable(B_PERIPH);                              //  ʹ��A���ڵ�GPIO�˿�   
    GPIOPinTypeIn(B_PORT, B_PIN);                            //  ����A���ڹܽ�Ϊ����   
}   
   
   
//  ��������������ڣ�   
int main(void)   
{   
    jtagWait();                                             //  ��ֹJTAGʧЧ����Ҫ��   
    clockInit();                                            //  ʱ�ӳ�ʼ��������6MHz   
    A_Init();   
    B_Init();   
   
    SysCtlPeriEnable(LED1_PERIPH);                          //  ʹ��LED1���ڵ�GPIO�˿�   
    GPIOPinTypeOut(LED1_PORT, LED1_PIN);                    //  ����LED1���ڵĹܽ�Ϊ���   
    GPIOPinWrite(LED1_PORT, LED1_PIN, 0x01);                //  Ϩ��LED1   
   
    SysCtlPeriEnable(LED2_PERIPH);                          //  ʹ��LED2���ڵ�GPIO�˿�   
    GPIOPinTypeOut(LED2_PORT, LED2_PIN);                    //  ����LED2���ڵĹܽ�Ϊ���   
    GPIOPinWrite(LED2_PORT, LED2_PIN, 1 << 2);              //  Ϩ��LED2   
   
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
  
    ulStatus = GPIOPinIntStatus(A_PORT, true);           //  ��ȡ�ж�״̬  
    GPIOPinIntClear(A_PORT, ulStatus);                   //  ����ж�״̬����Ҫ  
  
    if (ulStatus & A_PIN)                                //  ���KEY1���ж�״̬��Ч  
    {  
        SysCtlDelay(50*(TheSysClock/3000));  
        if ((GPIOPinRead(B_PORT,B_PIN))==0x00)                      // ˳ʱ��LED1��  
        { AA++;  
          GPIOPinWrite(LED2_PORT,LED2_PIN,1 << 2);  
          GPIOPinWrite(LED1_PORT,LED1_PIN,0x00);  
        }  
        else                                                        //��ʱ��LED2��  
        { BB++;  
          GPIOPinWrite(LED1_PORT,LED1_PIN,0x01);  
          GPIOPinWrite(LED2_PORT,LED2_PIN,0x00);  
        }  
  
    }  
    for(;;)  
    {  
  
    }  
}*/   
   
