#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/**************************************全局变量*******************************/
#define TIMER_PERIOD 127

#define DUTY_CYCLE1 70
#define DUTY_CYCLE2 70

uint8_t LineSensor_Read(uint32_t time);
void Delay_Nx1us(uint32_t time);

static volatile uint8_t val;

/* Timer_A UpDown Configuration Parameter */
const Timer_A_UpDownModeConfig upDownConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock SOurce
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          // SMCLK/1 = 3MHz
        TIMER_PERIOD,                           // 127 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value

};

/* Timer_A Compare Configuration Parameter  (PWM1) */
Timer_A_CompareModeConfig compareConfig_PWM_L =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_4,          // Use CCR4
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        DUTY_CYCLE1                                 // 32 Duty Cycle
};

/* Timer_A Compare Configuration Parameter (PWM2) */
Timer_A_CompareModeConfig compareConfig_PWM_R =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_3,          // Use CCR3
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        DUTY_CYCLE2                                 // 96 Duty Cycle
};

/*********************************函数声明**************************************/
    void motor_gpio_init(void);
    void motor_poweron(void);
    void motor_poweroff(void);
    void motor_L_forward(void);
    void motor_L_reverse(void);
    void motor_L_break(void);
    void motor_R_forward(void);
    void motor_R_reverse(void);
    void motor_R_break(void);
    void robot_init_status(void);
    void robot_forward(void);
    void robot_back(void);
    void robot_break(void);
    void robot_forward_left(void);
    void robot_forward_right(void);
    void touch_init(void);

/***********************************函数功能定义************************************/
    void motor_gpio_init()//初始化端口配置
    {
        MAP_GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7);
        MAP_GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);
    //    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN7);
    //    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN6);
        MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN7);
        MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
    }

    void motor_poweron()//运动函数编写
    {
        GPIO_setOutputHighOnPin (GPIO_PORT_P3, GPIO_PIN7);
        GPIO_setOutputHighOnPin (GPIO_PORT_P3, GPIO_PIN6);
    }

    void motor_poweroff()
    {
        GPIO_setOutputLowOnPin (GPIO_PORT_P3, GPIO_PIN7);
        GPIO_setOutputLowOnPin (GPIO_PORT_P3, GPIO_PIN6);
    }

    void motor_L_forward()
    {
        GPIO_setOutputLowOnPin (GPIO_PORT_P1, GPIO_PIN7);
        GPIO_setOutputHighOnPin (GPIO_PORT_P2, GPIO_PIN7);
    }

    void motor_L_reverse()
    {
        GPIO_setOutputHighOnPin (GPIO_PORT_P1, GPIO_PIN7);
        GPIO_setOutputHighOnPin (GPIO_PORT_P2, GPIO_PIN7);
    }

    void motor_L_break()
    {
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN7);
    }

    void motor_R_forward()
    {
        GPIO_setOutputLowOnPin (GPIO_PORT_P1, GPIO_PIN6);
        GPIO_setOutputHighOnPin (GPIO_PORT_P2, GPIO_PIN6);
    }

    void motor_R_reverse()
    {
        GPIO_setOutputHighOnPin (GPIO_PORT_P1, GPIO_PIN6);
        GPIO_setOutputHighOnPin (GPIO_PORT_P2, GPIO_PIN6);
    }

    void motor_R_break()
    {
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN6);
    }

    void robot_init_status()
    {
        GPIO_setOutputLowOnPin (GPIO_PORT_P3, GPIO_PIN7);
        GPIO_setOutputLowOnPin (GPIO_PORT_P3, GPIO_PIN6);
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN7);
        GPIO_setOutputLowOnPin (GPIO_PORT_P2, GPIO_PIN6);
        GPIO_setOutputLowOnPin (GPIO_PORT_P1, GPIO_PIN7);
        GPIO_setOutputLowOnPin (GPIO_PORT_P1, GPIO_PIN6);
    }

    void robot_forward()
    {
        motor_L_forward();
        motor_R_forward();
    }

    void robot_back()
    {
        motor_L_reverse();
        motor_R_reverse();
    }

    void robot_break()
    {
        motor_L_break();
        motor_R_break();
    }

    void robot_forward_left()
    {
        motor_L_reverse();
        motor_R_forward();
    }

    void robot_forward_right()
    {
        motor_R_reverse();
        motor_L_forward();
    }


int main()
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
    //以上是系统时钟初始化

    //GPIO端口初始化

    /*init*/
    motor_gpio_init();
    robot_init_status();

    /*P1.0设为输出，LED用来指示程序正常运行*/
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    //P5.3控制LED,设置为输出.　输出低电平(默认关闭)
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN3);

    //P7.0-P7.7全都设置为输入
    GPIO_setAsInputPin(GPIO_PORT_P7, PIN_ALL8);

    /* Configuring P1.0 as output */
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);//前面已经定义过P1.0的知识状态，不用管

    /* Setting P2.7 and P2.6 and peripheral outputs for CCR */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,
            GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring Timer_A1 for UpDown Mode and starting */
    MAP_Timer_A_configureUpDownMode(TIMER_A0_BASE, &upDownConfig);
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UPDOWN_MODE);

    /* Initialize compare registers to generate PWM1 */
//    compareConfig_PWM_L.compareValue=90;
    MAP_Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM_L);

    /* Initialize compare registers to generate PWM2 */
    MAP_Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM_R);

    /* GPIO interrupts */
    MAP_Interrupt_enableInterrupt(INT_PORT1);

    /* Enabling SRAM Bank Retention */
    MAP_SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK1);

    /* Enabling MASTER interrupts */
    MAP_Interrupt_enableMaster();

    printf("line sensor test start!\n");

    motor_poweron();
//    robot_forward();

    debug_time = 1000;
    while (1)//判断小车的状态是什么样的
    {
//        Delay_Nx1us(10);
        Delay_Nx1us(2000); //延时0.048s

        //延时系数2000, 单位us,　即2ms
//        val=LineSensor_Read(2000);
        val=LineSensor_Read(debug_time);

        if(val==0x01)
        {
            robot_forward_left();
            Delay_Nx1us(2000);
            robot_forward();//superh
        }
        if(val==0x02)
        {
            robot_forward_left();
            Delay_Nx1us(2000);
            robot_forward();//high
        }
        if(val==0x03)
        {
            robot_forward_left();
            Delay_Nx1us(2000);
            robot_forward();//high
        }
        if(val==0x04)
        {
            robot_forward_left();
            Delay_Nx1us(2000);
            robot_forward();//low
        }
        if(val==0x08)
        {
            robot_forward_left();
            Delay_Nx1us(2000);
            robot_forward();//low
        }
        if(val==0x07)
        {
            robot_forward_left();
            Delay_Nx1us(2000);
            robot_forward();//superh
        }
        if(val==0x06)
        {
            robot_forward_left();
            Delay_Nx1us(2000);
            robot_forward();//h
        }
        if(val==0x0c)
        {
            robot_forward_left();
            Delay_Nx1us(2000);//h
            robot_forward();
        }
        if(val==0x0e)
        {
            robot_forward_left();
            Delay_Nx1us(2000);
            robot_forward();//superh
        }
        if(val==0x0f)
        {
            robot_forward_left();
            Delay_Nx1us(2000);
            robot_forward();//superh
        }
        if(val==0x1c)
        {
            robot_forward_left();
            Delay_Nx1us(2000);
            robot_forward();//low
        }
        if(val==0x1e)
        {
            robot_forward_left();
            Delay_Nx1us(2000);//h
            robot_forward();
        }
        if(val==0x1f)
        {
            robot_forward_left();
            Delay_Nx1us(320000);//h
            robot_forward();
        }
        if(val==0x3f)
        {
            robot_forward_left();
            Delay_Nx1us(320000);//h
            robot_forward();
        }
        if(val==0x7f)
        {
            robot_forward_left();
            Delay_Nx1us(320000);//h
            robot_forward();
        }
        if(val==0x18)
        {
            robot_forward();
        }
        if(val==0x30)
        {
            robot_forward();
        }
        if(val==0x3c)
        {
            robot_forward();
        }
        if(val==0x10)
        {
            robot_forward_right();
            Delay_Nx1us(2000);
            robot_forward();//low
        }
        if(val==0x20)
        {
            robot_forward_right();
            Delay_Nx1us(2000);
            robot_forward();//low
        }
        if(val==0x40)
        {
            robot_forward_right();
            Delay_Nx1us(2000);
            robot_forward();//h
        }
        if(val==0x78)
        {
            robot_forward_right();
            Delay_Nx1us(2000);
            robot_forward();//h
        }
        if(val==0x38)
        {
            robot_forward_right();
            Delay_Nx1us(2000);
            robot_forward();//low
        }
        if(val==0x60)
        {
            robot_forward_right();
            Delay_Nx1us(2000);
            robot_forward();//h
        }
        if(val==0xc0)
        {
            robot_forward_right();
            Delay_Nx1us(2000);
            robot_forward();//h
        }
        if(val==0xf0)
        {
            robot_forward_right();
            Delay_Nx1us(2000);
            robot_forward();//superh
        }
        if(val==0x80)
        {
            robot_forward_right();
            Delay_Nx1us(2000);
            robot_forward();//superh
        }
        if(val==0x70)
        {
            robot_forward_right();
            Delay_Nx1us(2000);//superh
            robot_forward();
        }

        if(val==0xe0)
        {
            robot_forward_right();
            Delay_Nx1us(2000);//superh
            robot_forward();
        }
        if(val==0xf8)
        {
            robot_forward_right();
            Delay_Nx1us(2000);//superh
            robot_forward();
        }
        if(val==0xfc)
        {
            robot_forward_right();
            Delay_Nx1us(2000);//superh
            robot_forward();
        }
        if(val==0xfe)
        {
            robot_forward_right();
            Delay_Nx1us(2000);//superh
            robot_forward();
        }
        if(val==0x3e)
        {
            robot_forward_right();
            Delay_Nx1us(2000);
            robot_forward();//h
        }
        if(val==0x3f)
        {
            robot_forward_right();
            Delay_Nx1us(2000);
            robot_forward();//h
        }
        if(val==0x7f)
        {
            robot_forward_right();
            Delay_Nx1us(2000);
            robot_forward();//h
        }

        printf("Line Sensor State:%#x\n",val);

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
