#include "menu.h"
#include "stm32f1xx_hal.h" 
#include <string.h>


/*全局变量*********************/

/* 当前菜单指针，指向当前聚焦的菜单 */
static Menu* Menu_Current;

/*********************全局变量*/


void Menu_Init(Menu* MU)
{
	MU->Name = NULL;
	MU->Funsion = NULL;
	MU->len = 0;
	MU->Child = NULL;
	MU->Father = NULL;
	MU->Last = NULL;
	MU->Next = NULL;
}


/*配置函数*********************/


void Menu_SetLast( Menu* MU, Menu* LastMenu)
{
	MU->Last = LastMenu;
	LastMenu->Next = MU;
}


void Menu_SetNext( Menu* MU, Menu* NextMenu)
{
	MU->Next = NextMenu;
	NextMenu->Last = MU;
}


void Menu_SetFunsion( Menu* MU, void (*Funsion)(void))
{
	MU->Funsion = Funsion;
}

void Menu_SetTaskHandle( Menu* MU, TaskHandle_t TaskHandle)
{
	MU->TaskHandle = TaskHandle;
}


void Menu_SetFather( Menu* MU, Menu* FatherMenu)
{
	MU->Father = FatherMenu;
	
	if(FatherMenu->Child == NULL)
	{
		FatherMenu->Child = MU;
	}
	else
	{
		Menu* BrotherMenu = FatherMenu->Child;
		while( BrotherMenu->Next != NULL )
			BrotherMenu = BrotherMenu->Next;
		Menu_SetNext( BrotherMenu, MU);
	}
	FatherMenu->len++;
}


void Menu_SetChild( Menu* MU, Menu* ChildMenu)
{
	ChildMenu->Father = MU;
	
	if(MU->Child == NULL)
	{
		MU->Child = ChildMenu;
	}
	else
	{
		Menu* tmp = ChildMenu;
		
		while(tmp->Last != NULL)
		{
			tmp = tmp->Last;
			if(MU->Child == tmp)
			{
				MU->Child = ChildMenu;
				return;
			}
		}
		while(tmp->Next != NULL)
		{
			tmp = tmp->Next;
			if(MU->Child == tmp)
			{
				MU->Child = ChildMenu;
				return;
			}
		}
	}
}


void Menu_SetName( Menu* MU, const char* Name)
{
	MU->Name = Name;
}


void Menu_SetCurrent(Menu* MU)
{
	Menu_Current = MU;
}

/*********************配置函数*/





/*功能函数*********************/

uint8_t Menu_ToLast(void)
{
	if(Menu_Current->Last != NULL)
	{
		Menu_Current = Menu_Current->Last;
		return 1;
	}
	else
	{
		return 0;
	}
}


uint8_t Menu_ToNext(void)
{
	if(Menu_Current->Next != NULL)
	{
		Menu_Current = Menu_Current->Next;
		return 1;
	}
	else
	{
		return 0;
	}
}


uint8_t Menu_ToFather(void)
{
	if(Menu_Current->Father != NULL)
	{
		Menu_SetChild( Menu_Current->Father, Menu_Current);
		Menu_Current = Menu_Current->Father;
		return 1;
	}
	else
	{
		return 0;
	}
}


uint8_t Menu_ToChild(void)
{
	if(Menu_Current->Child != NULL)
	{
		Menu_Current = Menu_Current->Child;
		return 1;
	}
	else
	{
		return 0;
	}
}


uint8_t Menu_ToFunsion(void)
{
	if(Menu_Current->Funsion != NULL)
	{
		Menu_Current->Funsion();
		return 1;
	}
	else
	{
		return 0;
	}
}


Menu* Menu_GetCurrent(void)
{
	return Menu_Current;
}


Menu* Menu_GetLast(void)
{
	return Menu_Current->Last;
}


Menu* Menu_GetNext(void)
{
	return Menu_Current->Next;
}


Menu* Menu_GetFather(void)
{
	return Menu_Current->Father;
}


Menu* Menu_GetChild(void)
{
	return Menu_Current->Child;
}


const char* Menu_GetName(void)
{
	return Menu_Current->Name;
}



/*功能函数*********************/






