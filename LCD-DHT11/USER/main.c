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
#define CONG GPIO_Pin_3
#define TRU GPIO_Pin_4
#define OK GPIO_Pin_5
#define BACK GPIO_Pin_6

GPIO_InitTypeDef GPIO_Structure; 
int congtru_tong = 0;
uint8_t RHI = 0,RHD = 0 , TCI = 0, TCD = 0;
u8 RHI_set = 0, TCI_set = 0;
char Temp1[20], Temp2[20];
char Humi1[20], Humi2[20];
char Temp[20], Humi[20];

void EXTI0_Config(void);
void USART2_Config(void);
void USART2_SendString(uint8_t *str);
void gpio_Init(void);
void mannhietdo(void);
void mansetting1(void);
void mansetting2(void);
void Setting(void);

void hienthi_LCD(void *p); //hien thi nhiet do , do am len LCD
void GPIO_active(void *p); // hoat dong cua ngoai vi (den,quat)
void GT_esp32(void *p); // truyen du lieu tu stm32 sang esp32

int main(void){
	gpio_Init();
	DHT11_Init();
	xTaskCreate(GT_esp32, (const char*)"Gtiep_esp32",128 , NULL,1, NULL);
	xTaskCreate(hienthi_LCD, (const char*)"Display",128 , NULL,1, NULL);
	xTaskCreate(GPIO_active, (const char*)"GPIO",128 , NULL, 1, NULL);
	vTaskStartScheduler(); 
	while(1){
	}
}

void gpio_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_Structure.GPIO_Pin = DEN | QUAT; // cau hinh chan gpio den
	GPIO_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Structure);	
	
	
	GPIO_Structure.GPIO_Pin = CONG | TRU | OK | BACK ; // nut
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Structure);	
}

void mainmenu(void){
	while(1){
	systick_init();// initialize the delay function (Must initialize)
	lcd_i2c_init(1);
	if (GPIO_ReadInputDataBit(GPIO_PORT, OK) == 0){
		congtru_tong = congtru_tong + 1;			
			if(congtru_tong == 3){
				congtru_tong = 3;
			}
		}
	if (GPIO_ReadInputDataBit(GPIO_PORT,BACK) == 0){
			congtru_tong = congtru_tong - 1 ;
			if(congtru_tong == 0){
				congtru_tong = 0;
			}
	}		
	switch(congtru_tong){
		case 0:
			lcd_i2c_cmd(1,0x01); // Clear Display
			lcd_i2c_msg(1 ,1, 0, "MAN HINH CHINH");
			lcd_i2c_msg(1 ,2, 0, ">TIEP");
			DelayMs(50);
		break;
		case 1:
			mannhietdo();
		break;
		case 2:
			mansetting1();
			Setting();
		break;
		case 3:
			mansetting2();
			Setting();
		break;
		}
	}
}
void mannhietdo(void){
	lcd_i2c_cmd(1,0x01); // Clear Display
	while(1){
	DHT11_Read_Data(&RHI, &RHD, &TCI, &TCD);
	sprintf(Temp1 ,"T:%u *C",TCI);
	lcd_i2c_msg(1 ,1, 0, Temp1);
	sprintf(Humi1 ,"H:%u %%",RHI);
	lcd_i2c_msg(1 ,2, 0, Humi1);
	DelayMs(50);
}
	}
void mansetting1(void){
	while(1){
	sprintf(Temp1 ,"SET >T:%u *C", TCI_set);
	lcd_i2c_msg(1 ,1, 0, Temp2);
	sprintf(Humi1 ,"H:%u %%",RHI_set);
	lcd_i2c_msg(1 ,2, 5, Humi2);
	DelayMs(50);
	}
}

void mansetting2(void){
	while(1){
	sprintf(Temp1 ,"SET T:%u *C", TCI_set);
	lcd_i2c_msg(1 ,1, 0, Temp2);
	sprintf(Humi1 ,">H:%u %%",RHI_set);
	lcd_i2c_msg(1 ,2, 5, Humi2);
	DelayMs(50);
	}
}
void Setting(void){
	while(1){
	if (congtru_tong == 2){
		if (GPIO_ReadInputDataBit(GPIO_PORT, CONG) == 0){ // nut tang nhiet do 
				TCI_set = TCI_set + 1;
					if (TCI_set == 90){
							TCI_set = 90;
					}
			}
		if (GPIO_ReadInputDataBit(GPIO_PORT, TRU) == 0){ // nut giam nhiet do 
				TCI_set = TCI_set - 1;
					if (TCI_set == 0){
						TCI_set = 0;
					}
			}
		}
	if(congtru_tong == 3){
		if (GPIO_ReadInputDataBit(GPIO_PORT, CONG) == 0){ // nut tang do am
				RHI_set = RHI_set + 1;
					if (RHI_set == 100){
						RHI_set = 100;
					}
			}
		if (GPIO_ReadInputDataBit(GPIO_PORT, TRU) == 0){ // nut giam do am
				RHI_set = RHI_set - 1;
					if (RHI_set == 0){
						RHI_set = 0;
					}
			}
	  }
	}
}

//Dieu khien DEN , Quat 
void GPIO_active(void *p){
	while(1){
		if(TCI_set <= TCI){
			GPIO_SetBits( GPIO_PORT , DEN);	
			GPIO_ResetBits(GPIO_PORT , QUAT);
		}
		if(RHI_set > TCI){
			GPIO_SetBits( GPIO_PORT , QUAT);	
			GPIO_ResetBits(GPIO_PORT , DEN);
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
