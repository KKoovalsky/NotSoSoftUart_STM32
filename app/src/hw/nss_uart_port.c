#include "nss_uart_port.h"
#include "nss_uart.h"
#include "tim.h"
#include "gpio.h"

#define TIM_RX_INST		TIM2
#define TIM_TX_INST		TIM3
#define COUNTER_MARGIN_PERCENTAGE      80
#define COUNTER_BIT_DUR_THRESH         ( 0xFFFFFFFF ) * ( 100 ) / ( COUNTER_MARGIN_PERCENTAGE )

static int bits_rcvd = 0;


void nssu_init()
{
	MX_TIM2_Init();
	MX_TIM3_Init();
}

void nssu_rx_timer_start()
{
	// Clear the counter's value
	LL_TIM_SetCounter(TIM_RX_INST, 0);
	LL_TIM_EnableIT_UPDATE(TIM_RX_INST);
	LL_TIM_EnableCounter(TIM_RX_INST);
}

void nssu_rx_timer_stop()
{
	LL_TIM_DisableIT_UPDATE(TIM_RX_INST);
	LL_TIM_DisableCounter(TIM_RX_INST);
}

void nssu_rx_timer_restart()
{
	stop_nssu_rx_timer();
	start_nssu_rx_timer();
}

int nssu_get_rx_pin_state()
{
	return LL_GPIO_IsInputPinSet(D3_SOFTUART_IN_GPIO_Port, D3_SOFTUART_IN_Pin);
}

int nssu_get_num_bits_rcvd()
{
	int res = bits_rcvd + (LL_TIM_GetCounter(TIM_RX_INST) > COUNTER_BIT_DUR_THRESH ? 1 : 0);
	bits_rcvd = 0;
	return res;
}

void nssu_set_tx_pin_state(int state)
{
	if(state)
		LL_GPIO_SetOutputPin(D5_SOFTUART_OUT_GPIO_Port, D5_SOFTUART_OUT_Pin);
	else
		LL_GPIO_ResetOutputPin(D5_SOFTUART_OUT_GPIO_Port, D5_SOFTUART_OUT_Pin);
}

void nssu_tx_tim_isr_enable()
{
	// Clear the counter's value
	LL_TIM_SetCounter(TIM_TX_INST, 0);
	LL_TIM_EnableIT_UPDATE(TIM_TX_INST);
	LL_TIM_EnableCounter(TIM_TX_INST);
}

void nssu_tx_tim_isr_disable()
{
	LL_TIM_DisableIT_UPDATE(TIM_TX_INST);
	LL_TIM_DisableCounter(TIM_TX_INST);
}

void handle_nssu_rx_tim_overflow()
{
	bits_rcvd++;
}
