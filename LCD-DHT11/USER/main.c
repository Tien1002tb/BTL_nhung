#include "stm32f10x.h"                  // Device header
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdio.h"

#include "systick_time.h"
#include "lcd_1602_drive.h"
#include "dht11.h"

#define GPIO_PORT GPIOA
#define DEN  GPIO_Pin_0
#define QUAT GPIO_Pin_1
#define BUTTON_1 GPIO_Pin_3
#define BUTTON_2 GPIO_Pin_4
#define BUTTON_3 GPIO_Pin_5
#define BUTTON_4 GPIO_Pin_6

GPIO_InitTypeDef GPIO_Structure;
char  Temp[20], Humi[20];
u8 tempset = 30, humiset = 20;
uint8_t RHI = 0,RHD = 0 , TCI = 0, TCD = 0;
char line1[20], line2[20];
void EXTI0_Config(void);
void USART2_Config(void);
void USART2_SendString(uint8_t *str);
void gpio_Init(void);


void hienthi_LCD(void *p); //hien thi nhiet do , do am len LCD
void GPIO_active(void *p); // hoat dong cua ngoai vi (den,quat)
void Setting(void *p); // cai dat nhiet do, do am theo yeu cau 
void GT_esp32(void *p); // truyen du lieu tu stm32 sang esp32

int main(void){
	xTaskCreate(GT_esp32, (const char*)"Gtiep_esp32",128 , NULL,1, NULL);
	xTaskCreate(hienthi_LCD, (const char*)"Display",128 , NULL,1, NULL);
	xTaskCreate(GPIO_active, (const char*)"GPIO",128 , NULL, 1, NULL);
	xTaskCreate(Setting, (const char*)"Setting",128 , NULL, tskIDLE_PRIORITY, NULL);
	vTaskStartScheduler(); 
	while(1){
	}
}


// Phan hien thi
void hienthi_LCD(void *p){
	systick_init();// initialize the delay function (Must initialize)
	lcd_i2c_init(1);
	DHT11_Init();
	while(1)	{	
		DHT11_Read_Data(&RHI, &RHD, &TCI, &TCD);
		sprintf(line1 ,"T:%u *C | H:%u %%", TCI, RHI);
		sprintf(line2 ,"SET:%u*C | %u%%", tempset, humiset);	
		lcd_i2c_msg(1 ,1, 0, line1);
		lcd_i2c_msg(1 ,2, 0, line2);
		DelayMs(500);
	}
	}
//Dieu khien DEN , Quat 
void GPIO_active(void *p){
	while(1){
		if(tempset <= TCI){
			GPIO_SetBits( GPIO_PORT , DEN);	
			GPIO_ResetBits(GPIO_PORT , QUAT);
		}
		if(tempset > TCI){
			GPIO_SetBits( GPIO_PORT , QUAT);	
			GPIO_ResetBits(GPIO_PORT , DEN);
}
	}
}
// nut nhan set nhiet do 

void gpio_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_Structure.GPIO_Pin = DEN | QUAT; // cau hinh chan gpio den
	GPIO_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Structure);	
	
	GPIO_Structure.GPIO_Pin = BUTTON_1 | BUTTON_2 | BUTTON_3 |BUTTON_4; // nut nhan 1
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Structure);	
}

void Setting(void *p){
	gpio_Init();
	
	while(1){
		if (GPIO_ReadInputDataBit(GPIO_PORT, BUTTON_1) == 0){ // nut tang nhiet do 
				tempset = tempset + 1;
					if (tempset == 60){
						tempset = 0;
					}
			}
		}
	while(1){	
		if (GPIO_ReadInputDataBit(GPIO_PORT, BUTTON_2) == 0){ // nut giam nhiet do 
				tempset = tempset - 1;
					if (tempset == 0){
						tempset = 60;
					}
			}
		}
	while(1){
		if (GPIO_ReadInputDataBit(GPIO_PORT, BUTTON_3) == 0){ // nut tang do am
				humiset = humiset + 1;
					if (humiset == 100){
						humiset = 0;
					}
			}
		}		
	while(1){	
		if (GPIO_ReadInputDataBit(GPIO_PORT, BUTTON_2) == 0){ // nut giam do am
				humiset = humiset - 1;
					if (humiset == 0){
						humiset = 100;
					}
			}
	}
}


// truyen UART
void EXTI0_Config(void){
	RCC->APB2ENR |= 0x01;
	AFIO->EXTICR[0] |= 0x01;
	EXTI->PR = 0x01;
	EXTI->RTSR = 0x00;
	EXTI->FTSR |= 0x01;
	EXTI->IMR |= 0x01;
	EXTI->EMR = 0x00;
	EXTI->SWIER |= 0x00;
	NVIC->ISER[0] = 0x40;
}

void GT_esp32(void *p){
	EXTI0_Config();
	USART2_Config();
	SystemInit();
	SystemCoreClockUpdate();	
while(1){
	sprintf(Temp, "Temp: %u *C", TCI);
	sprintf(Humi, "Humi: %u %%", RHI);
	USART2_SendString(Temp);
	DelayMs(500);
	USART2_SendString(Humi);
	DelayMs(500);
}

}
