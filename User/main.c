/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/25
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/



#include "debug.h"
#include "ULWOS2.h"
#include "oled_display.h"
#include "fonts.h"


/* Global define */
ULWOS2_DEFINE_SIGNAL(SIGNAL_TIMER_INC);

typedef enum {
    TIMER_STATUS_PAUSED,
    TIMER_STATUS_RUNNING
} timer_status_t;

typedef enum {
    SETTING_STATUS_DISABLED,
    SETTING_STATUS_ENABLED
} settings_status_t;



/* Global Variable */
uint32_t min_timeout = 1;
uint8_t i = 0, sw_button = 0;
uint32_t shortPressMin = 5;
uint32_t longPressMin = 1500;
uint8_t seconds = 59, minutes;
uint8_t timer_status = TIMER_STATUS_PAUSED;
uint8_t setting_status = SETTING_STATUS_DISABLED;
char str[20];


// --------------- peripheral configuration ----------------------
// init I2C interface
void I2C_User_Init(uint32_t bound, uint16_t address) {
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };
    I2C_InitTypeDef I2C_InitTSturcture = { 0 };

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE);

    //SCL - PC2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    //SDA - PC1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure);

    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init( I2C1, &I2C_InitTSturcture);

    I2C_Cmd( I2C1, ENABLE);
    I2C_AcknowledgeConfig( I2C1, ENABLE);

}

// init switch button port D0 D3
void GPIO_SwButton_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

}

// init GPIO LED on port D4
void GPIO_Toggle_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

// --------------- END peripheral configuration ----------------------


// timer thread 1 second
void Timer_thread (void)
{
    ULWOS2_THREAD_START();
    while (1)
    {
        ULWOS2_THREAD_SLEEP_MS(1000);
        ULWOS2_THREAD_SEND_SIGNAL(SIGNAL_TIMER_INC);
    }
}

// Timer display thread
void Display_thread(void) {


    ULWOS2_THREAD_START();
    ssd1306_Fill(Black);
    ssd1306_SetCursor(20, 10);
    sprintf(str, "TIMER");
    ssd1306_WriteString(str, FONT_BIG, White);
    ssd1306_UpdateScreen();
    while (1)
    {
        ULWOS2_THREAD_WAIT_FOR_SIGNAL(SIGNAL_TIMER_INC);
        if (timer_status == TIMER_STATUS_RUNNING)
        {
            ssd1306_Fill(Black);
            ssd1306_SetCursor(0, 0);
            sprintf(str, "%d", minutes);
            ssd1306_WriteString(str, FONT_UGE, White);
            ssd1306_SetCursor(ssd1306_GetCurrentX()+5, 15);
            sprintf(str, "%d", seconds);
            ssd1306_WriteString(str, FONT_BIG, White);
            ssd1306_UpdateScreen();
            seconds --;
            if (minutes == 0 && seconds == 0)
            {
                ssd1306_Fill(Black);
                ssd1306_SetCursor(0, 10);
                sprintf(str, "TIMEOUT");
                ssd1306_WriteString(str, FONT_BIG, White);
                ssd1306_UpdateScreen();
                minutes = min_timeout;
                seconds = 59;
                timer_status = TIMER_STATUS_PAUSED;
            }
            if (seconds == 0)
            {
                minutes--;
                seconds = 59;
            }

        }

    }
}


// LED blinker thread
void ledBlinker_thread1(void)
{
    ULWOS2_THREAD_START();
    while (1)
    {
        ULWOS2_THREAD_SLEEP_MS(500);
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
    }
}


// switch button 1 thread: start, pause, reset countdown timer
void button1Checker_thread(void) {

    static uint8_t lastState = Bit_SET;
    static uint32_t timeCapture = 0;
    ULWOS2_THREAD_START();
    while (1)
    {
        ULWOS2_THREAD_SLEEP_MS(10);
        uint8_t currentState = (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0 ));
        if (lastState != currentState)
        {
            lastState = currentState;
            if (currentState == Bit_RESET)
                timeCapture = get_millis();
            // button short pressed
            else if (timeCapture != 0 && (get_millis() - timeCapture) > shortPressMin)
            {
                timeCapture = 0;
                if (timer_status == TIMER_STATUS_PAUSED)
                    timer_status = TIMER_STATUS_RUNNING;
                else if (timer_status == TIMER_STATUS_RUNNING)
                    timer_status = TIMER_STATUS_PAUSED;
            }
            else
                timeCapture = 0;
        }
        // button long pressed
        if (timeCapture != 0 && (get_millis() - timeCapture) > longPressMin)
        {
        	timer_status = TIMER_STATUS_PAUSED;
        	ssd1306_Fill(Black);
            ssd1306_SetCursor(20, 10);
            sprintf(str, "RESET");
            ssd1306_WriteString(str, FONT_BIG, White);
            ssd1306_UpdateScreen();
            timeCapture = 0;
            ULWOS2_THREAD_SLEEP_MS(3000);

            seconds = 59;
            minutes = min_timeout ;
            timer_status = TIMER_STATUS_RUNNING;
        }
    }
}


// switch button 2 thread. Set timeout minutes
void button2Checker_thread(void) {

    static uint8_t lastState = Bit_SET;
    static uint32_t timeCapture = 0;
    ULWOS2_THREAD_START();
    while (1)
    {
        ULWOS2_THREAD_SLEEP_MS(10);
        uint8_t currentState = (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3 ));
        if (lastState != currentState)
        {
            lastState = currentState;
            if (currentState == Bit_RESET)
                timeCapture = get_millis();
            // button short pressed
            else if (timeCapture != 0 && (get_millis() - timeCapture) > shortPressMin)
            {
                timeCapture = 0;
                if (setting_status == SETTING_STATUS_ENABLED)
                {
                	min_timeout ++;
                    if (min_timeout > 60)
                    	min_timeout = 1;
                    ssd1306_Fill(Black);
                    ssd1306_SetCursor(0, 0);
                    sprintf(str, "%d", min_timeout);
                    ssd1306_WriteString(str, FONT_UGE, White);
                    ssd1306_SetCursor(ssd1306_GetCurrentX() + 5, 15);
                    sprintf(str, "MIN");
                    ssd1306_WriteString(str, FONT_BIG, White);
                    ssd1306_UpdateScreen();
                }

            }
            else
                timeCapture = 0;
        }
        // button long pressed
        if (timeCapture != 0 && (get_millis() - timeCapture) > longPressMin)
        {
            if (setting_status == SETTING_STATUS_DISABLED) {
                timer_status = TIMER_STATUS_PAUSED;
                setting_status = SETTING_STATUS_ENABLED;
                ssd1306_Fill(Black);
                ssd1306_SetCursor(20, 10);
                sprintf(str, "SET");
                ssd1306_WriteString(str, FONT_BIG, White);
                ssd1306_UpdateScreen();
                ULWOS2_THREAD_SLEEP_MS(3000);

                ssd1306_Fill(Black);
                ssd1306_SetCursor(0, 0);
                ssd1306_WriteChar('1', FONT_UGE, White);
                min_timeout = 1;
                ssd1306_SetCursor(ssd1306_GetCurrentX() + 5, 15);
                sprintf(str, "MIN");
                ssd1306_WriteString(str, FONT_BIG, White);
                ssd1306_UpdateScreen();
            }
            else
            {
                // write timeout settings into flash
                FLASH_Unlock_Fast();
                FLASH_ErasePage_Fast(0x08003F80);
                FLASH_BufLoad(0x08003F80, min_timeout --);
                FLASH_ProgramPage_Fast(0x08003F80);
                FLASH_Lock_Fast();

                seconds = 59;
                minutes = min_timeout;
                setting_status = SETTING_STATUS_DISABLED;
                timer_status = TIMER_STATUS_RUNNING;
            }
            timeCapture = 0;
        }
    }
}


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    SysTick_INIT();

    I2C_User_Init(100000, 0x02);
    ssd1306_Init();
    GPIO_Toggle_INIT();
    GPIO_SwButton_INIT();

    // read stored minutes timeout data from flash
    FLASH_Lock_Fast();
    minutes = (uint8_t)(*(u32*)0x08003F80) -1;

#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();

#endif

    ULWOS2_INIT();
    ULWOS2_THREAD_CREATE(ledBlinker_thread1, 10);
    ULWOS2_THREAD_CREATE(button1Checker_thread, 10);
    ULWOS2_THREAD_CREATE(button2Checker_thread, 10);
    ULWOS2_THREAD_CREATE(Display_thread, 10);
    ULWOS2_THREAD_CREATE(Timer_thread, 10);
    ULWOS2_START_SCHEDULER();
}




