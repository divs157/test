#include "stm32f1xx_hal.h"
#include "Subhead_1.h"
#include "Subhead_2.h"
#include "Subhead_4.h"
#include "Subhead_5.h"
#include "usart.h"
#include "menu.h"
#include "GUI.h"
#include "oled.h"				//包含需要的头文件
#include "key.h"
#include "main.h"

extern QueueHandle_t g_xQueueMenu;

extern void Control_RGB(u8g2_t *u8g2);


/*	Subhead_5名字	*/
const char* Subhead_5_Name = "Control RGB";

/*	Subhead_5菜单结构体	*/
Menu Subhead_5;	

void Subhead_5_Funsion(void)
{
	Control_RGB(&u8g2);
	//	为空	
}

void Subhead_5_Init(void)
{
	Menu_Init(&Subhead_5);
	Subhead_5.Funsion = Subhead_5_Funsion;
	Subhead_5.Name = Subhead_5_Name;
}



void Control_RGB(u8g2_t *u8g2)
{
	g_xQueueMenu = xQueueCreate(1,sizeof(struct Key));
	struct Key	key_data;
	static uint8_t R,G,B,brightness,num;
	static char brightness_str[3],R_str[3],G_str[3],B_str[3],num_str[3];
	static uint8_t Flag = 0;
	static int8_t options = 0;
	uint8_t draw_param[6][4] = {30,15,17,2,
								30,15,17,18,
								30,15,17,33,
								30,15,17,47,
								30,15,82,2,
	};
	
	u8g2_SetFontPosTop(u8g2);
	
    while(1)
    {
        u8g2_ClearBuffer(u8g2);
		u8g2_SetFontPosTop(u8g2); 
		u8g2_DrawRFrame(u8g2, 0, 0, 128, 64, 7);
		u8g2_DrawStr(u8g2, 3, 3, "L");
		u8g2_DrawStr(u8g2, 3, 19, "R");
		u8g2_DrawStr(u8g2, 3, 34, "G");
		u8g2_DrawStr(u8g2, 3, 48, "B");
		u8g2_DrawStr(u8g2, 50, 3, "num");
		
		sprintf(brightness_str, "%d", brightness);
		u8g2_DrawStr(u8g2, 20, 4, brightness_str);
		sprintf(R_str, "%d", R);
		u8g2_DrawStr(u8g2, 20, 20, R_str);
		sprintf(G_str, "%d", G);
		u8g2_DrawStr(u8g2, 20, 35, G_str);
		sprintf(B_str, "%d", B);
		u8g2_DrawStr(u8g2, 20, 50, B_str);
		sprintf(num_str, "%d", num);
		u8g2_DrawStr(u8g2, 85, 4, num_str);
//		u8g2_DrawRBox(u8g2, 20, 7, 255 * (100 / brightness), 4, (float)0.5);
//		u8g2_DrawRBox(u8g2, 20, 23, 255 * (100  / R), 4, (float)0.5);
//		u8g2_DrawRBox(u8g2, 20, 39, 255 * (100 / G), 4, (float)0.5);
//		u8g2_DrawRBox(u8g2, 20, 53, 255 * (100 / B), 4, (float)0.5);


		if(Flag == 0)
		{
			BlinkDraw(u8g2, EasingDrawBox, draw_param[options][2],draw_param[options][3], draw_param[options], 500,0);	
		}
		else
		{
			BlinkDraw(u8g2, EasingDrawBox, draw_param[options][2],draw_param[options][3], draw_param[options], 500,1);	

		}



		u8g2_SendBuffer(u8g2);

		
		if(pdPASS ==  xQueueReceive(g_xQueueMenu,&key_data,0))
		{
			if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_CLICK)
			{
				if(Flag == 0)
				{
					Flag = 1;
				}
				else
				{
					Flag = 0;
				}
				
			}
			else if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_DOUBLE)
			{
				Flag = 0;
                LinearClearScreen(u8g2, CLEAR_LEFT, 10, 10);
                return;
			}
			else if(key_data.Key_Data == CM_KEY && key_data.Key_Param == VK_LONG)
			{
				Flag = 0;
                LinearClearScreen(u8g2, CLEAR_LEFT, 10, 10);
                return;
			}
			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_RIGHT)
			{
				if(Flag == 0)
				{
					options++;
					if(options>4)
					{
						options=4;
					}
				}
				else
				{
					switch(options)
					{
						case 0:
							brightness-=1;
//							if(brightness<0)
//							{
//								brightness=0;
//							}
						break;
						case 1:
							R--;
//							if(R<0)
//							{
//								R=0;
//							}
						break;
						case 2:
							G--;
//							if(G<0)
//							{
//								G=0;
//							}
						break;
						case 3:
							B--;
//							if(B<0)
//							{
//								B=0;
//							}
						break;
						case 4:
							num=0;
							RGB_Close(255);
						break;
					}
				}
				
			}
			else if(key_data.Key_Data == CM_ROATATING && key_data.Key_Param == EK_LEFT)
			{
				if(Flag == 0)
				{
					options--;
					if(options<0)
					{
						options=0;
					}
				}
				else
				{
					switch(options)
					{
						case 0:
							brightness+=1;
							if(brightness>255)
							{
								brightness=255;
							}
						break;
						case 1:
							R++;
							if(R>255)
							{
								R=255;
							}
						break;
						case 2:
							G++;
							if(G>255)
							{
								G=255;
							}
						break;
						case 3:
							B++;
							if(B>255)
							{
								B=255;
							}
						break;
						case 4:
							num++;
//							if(num>4)
//							{
//								num=4;
//							}
						break;
						
					}
				}
				
			}
		}
		
		RGB_SetRGB(R,G,B,brightness,num);	
        u8g2_SendBuffer(u8g2);
    }
}



