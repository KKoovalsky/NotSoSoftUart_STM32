#include "FreeRTOS.h"
#include "task.h"

extern void init_not_so_soft_uart(); 

void nssu_task(void *params)
{
	init_not_so_soft_uart();
	
}
