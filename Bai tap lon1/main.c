#include "systick_time.h"
#include "lcd_1602_drive.h"
#include "dht11.h"
#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stdio.h"

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

void gpio_Init(void);
void mainmenu(void);
void mannhietdo(void);
void mansetting1(void);
void mansetting2(void);
void Setting(void);

int main(){
	gpio_Init();
	systick_init();
	lcd_i2c_init(1);
	DHT11_Init();
	mainmenu();

}
void gpio_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_Structure.GPIO_Pin = DEN | QUAT; // cau hinh chan gpio den
	GPIO_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Structure);	
	
	GPIO_Structure.GPIO_Pin = CONG | TRU | OK ; // nut
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
