
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

    //停用看门狗
    MAP_WDT_A_holdTimer();

    //使能浮点运算的单元FPU,提高计算效率
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
    //---------以上是系统时钟初始化-------



    /*p1.0设为输出，LED用来指示程序正常运行*/
    //    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    //p5.3接发射led,设置为输出.　输出低电平(默认关闭)
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN3);

    //p7.0-p7.7全都设置为输入
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
    while (1)//判断小车的状态是什么样的
    {
        //        Delay_Nx1us(10);
        Delay_Nx1us(2000); //延时0.048s

        //延时系数2000, 单位us,　即2ms
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



//读取传感器的数据
uint8_t LineSensor_Read(uint32_t time)
{
    uint8_t result;

    //P5.3输出高电平,打开红外发射管
    GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN3);
    //P7.0-7.7方向设置为输出
    GPIO_setAsOutputPin(GPIO_PORT_P7,PIN_ALL8);
    //设置P7.0-7.7输出高电平
    GPIO_setOutputHighOnPin(GPIO_PORT_P7,PIN_ALL8);
    //延时10us
    Delay_Nx1us(10);
    //P7.0-7.7方向设置为输入
    GPIO_setAsInputPin(GPIO_PORT_P7,PIN_ALL8);
    //延时特定时间
    Delay_Nx1us(1000);
    //读取P7.0-7.7的输入值
    result = P7->IN;

    //P5.3输出低电平,关闭红外发射管
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN3);
    //返回端口读取值
    return result;
}

//DCO=24MHz
//
void Delay_Nx1us(uint32_t time)
{
    while(time>0)
    {
        time--;//空位操作还需要在判断时间的多少
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
