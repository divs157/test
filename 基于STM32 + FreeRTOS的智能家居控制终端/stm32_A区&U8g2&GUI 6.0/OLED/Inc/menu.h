#ifndef __MENU_H__
#define __MENU_H__

#include "stm32f1xx_hal.h" 
#include "FreeRTOS.h"		  //包含需要的头文件
#include "task.h"			  //包含需要的头文件
#include "cmsis_os.h"		  //包含需要的头文件
#include "queue.h"

#define		WORD_H 		16


/*	窗口结构体	*/
typedef struct Menu{
	const char* Name;			/*	菜单名														*/
	uint8_t len;
	void (*Funsion)(void);		/*	菜单功能，用于处理各种操作	*/
	TaskHandle_t TaskHandle;
	struct Menu* Father;		/*	父菜单指针，用于查找上一级菜单，为NULL表示该窗口无父菜单	*/
	struct Menu* Child;			/*	子菜单指针，用于查找下一级菜单，为NULL表示该窗口无子菜单	*/
	struct Menu* Last;			/*	前菜单指针，用于查找同级上一个菜单							*/
	struct Menu* Next;			/*	后菜单指针，用于查找同级下一个菜单							*/
}Menu;

/*函数声明*********************/

/*初始化函数*/
void Menu_Init(Menu* MU);

/*配置函数*/
void Menu_SetLast( Menu* MU, Menu* LastMenu);
void Menu_SetNext( Menu* MU, Menu* NextMenu);
void Menu_SetFunsion( Menu* MU, void (*Funsion)(void));
void Menu_SetFather( Menu* MU, Menu* FatherMenu);
void Menu_SetChild( Menu* MU, Menu* ChildMenu);
void Menu_SetIcon( Menu* MU, const uint8_t* Icon);
void Menu_SetName( Menu* MU, const char* Name);
void Menu_SetCurrent(Menu* MU);

/*功能函数*/
uint8_t Menu_ToLast(void);
uint8_t Menu_ToNext(void);
uint8_t Menu_ToFather(void);
uint8_t Menu_ToChild(void);
uint8_t Menu_ToFunsion(void);
Menu* Menu_GetCurrent(void);
Menu* Menu_GetLast(void);
Menu* Menu_GetNext(void);
Menu* Menu_GetFather(void);
Menu* Menu_GetChild(void);
const char* Menu_GetName(void);
const uint8_t* Menu_GetIcon(void);

/*********************函数声明*/

extern Menu GameMenu;	

void MainMenu_Init(void);

void run_option_class(Menu Page);
void run_play_option_class(Menu Page);	//添加动画

#endif
