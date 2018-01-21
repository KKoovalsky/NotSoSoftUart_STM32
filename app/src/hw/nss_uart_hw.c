#include "nss_uart.h"
#include "tim.h"
#include "gpio.h"

#define TIM_INST TIM2
#define COUNTER_MARGIN_PERCENTAGE	80
#define COUNTER_BIT_DUR_THRESH		( 0xFFFFFFFF ) * ( 100 ) / ( COUNTER_MARGIN_PERCENTAGE )

void init_not_so_soft_uart()
{
	MX_TIM2_Init();
}

void start_nssu_timer()
{
	nssu_bytes_rcvd = 0;

	// Clear the counter's value
	LL_TIM_SetCounter(TIM_INST, 0);
	LL_TIM_EnableIT_UPDATE(TIM_INST);
	LL_TIM_EnableCounter(TIM_INST);
}

void stop_nssu_timer()
{
	LL_TIM_DisableIT_UPDATE(TIM_INST);
	LL_TIM_DisableCounter(TIM_INST);
}

void reset_nssu_timer()
{
	stop_nssu_timer();
	start_nssu_timer();
}

int get_nssu_pin_state()
{
	return LL_GPIO_IsInputPinSet(D3_SOFTUART_IN_GPIO_Port, D3_SOFTUART_IN_Pin);
}

int get_nssu_bytes_rcvd()
{
	int n = nssu_bytes_rcvd;
	if(LL_TIM_GetCounter(TIM_INST) > COUNTER_BIT_DUR_THRESH)
		n++;
	return n;
}

