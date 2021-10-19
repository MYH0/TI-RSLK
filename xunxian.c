
//#include <line.h>
//#include <SystemTick.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* Application Defines */
#define TIMER_PERIOD 127

#define DUTY_CYCLE1 70
#define DUTY_CYCLE2 70

uint8_t LineSensor_Read(uint32_t time);
void Delay_Nx1us(uint32_t time);

static volatile uint8_t val;

int main(void)
{
    volatile uint32_t debug_time;

    //ͣ�ÿ��Ź�
    MAP_WDT_A_holdTimer();

    //ʹ�ܸ�������ĵ�ԪFPU,��߼���Ч��
    MAP_FPU_enableModule();

    //DCO 24MHz, REFO=32kHz
    MAP_CS_setDCOFrequency(24000000);
    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_32KHZ);

    //ACLK = REFO/1 =32kHz
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    //MCLK = DCO = 24MHz
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    //HSMCLK = DCO/2 = 12MHz,
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_2);

    //SMCLK = DCO/8 = 3MHz,
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_8);
    //---------������ϵͳʱ�ӳ�ʼ��-------



    /*p1.0��Ϊ�����LED����ָʾ������������*/
    //    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    //p5.3�ӷ���led,����Ϊ���.������͵�ƽ(Ĭ�Ϲر�)
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN3);

    //p7.0-p7.7ȫ������Ϊ����
    GPIO_setAsInputPin(GPIO_PORT_P7, PIN_ALL8);

    /* Configuring P1.0 as output */
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0+GPIO_PIN1+GPIO_PIN2);


    /* GPIO interrupts */
    MAP_Interrupt_enableInterrupt(INT_PORT1);

    /* Enabling SRAM Bank Retention */
    MAP_SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK1);

    /* Enabling MASTER interrupts */
    MAP_Interrupt_enableMaster();

    debug_time = 1000;
    while (1)//�ж�С����״̬��ʲô����
    {
        //        Delay_Nx1us(10);
        Delay_Nx1us(2000); //��ʱ0.048s

        //��ʱϵ��2000, ��λus,����2ms
        //        val=LineSensor_Read(2000);
        val=LineSensor_Read(debug_time);

        if(val==0x7f)
        {
            GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
        }
        if(val==0xbf)
        {
            GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
        }
        if(val==0xdf)
        {
            GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
        }
        if(val==0xef)
        {
            GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
        }

        if(val==0xf7)
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
        }
        if(val==0xfb)
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
        }
        if(val==0xfd)
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
        }
        if(val==0xfe)
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
        }

    }

}



//��ȡ������������
uint8_t LineSensor_Read(uint32_t time)
{
    uint8_t result;

    //P5.3����ߵ�ƽ,�򿪺��ⷢ���
    GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN3);
    //P7.0-7.7��������Ϊ���
    GPIO_setAsOutputPin(GPIO_PORT_P7,PIN_ALL8);
    //����P7.0-7.7����ߵ�ƽ
    GPIO_setOutputHighOnPin(GPIO_PORT_P7,PIN_ALL8);
    //��ʱ10us
    Delay_Nx1us(10);
    //P7.0-7.7��������Ϊ����
    GPIO_setAsInputPin(GPIO_PORT_P7,PIN_ALL8);
    //��ʱ�ض�ʱ��
    Delay_Nx1us(1000);
    //��ȡP7.0-7.7������ֵ
    result = P7->IN;

    //P5.3����͵�ƽ,�رպ��ⷢ���
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN3);
    //���ض˿ڶ�ȡֵ
    return result;
}

//DCO=24MHz
//
void Delay_Nx1us(uint32_t time)
{
    while(time>0)
    {
        time--;//��λ��������Ҫ���ж�ʱ��Ķ���
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();



    }
}
