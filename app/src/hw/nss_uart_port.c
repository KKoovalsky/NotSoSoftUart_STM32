#include "nss_uart.h"
#include "tim.h"
#include "gpio.h"

#define TIM_RX_INST			TIM2
#define TIM_TX_INST			TIM3
#define EXTI_RX_LINE_INST	LL_EXTI_LINE_3

#define CPU_CLK_FREQ			96000000
#define NSSU_BAUD_RATE			9600
#define NSSU_PRESCALING			( (CPU_CLK_FREQ) / (NSSU_BAUD_RATE) )
#define NSSU_AUTO_RELOAD		( (NSSU_PRESCALING) - 1 )

#define COUNTER_TICKS_BIT_DUR			10000
#define COUNTER_MARGIN_PERCENTAGE		80
#define COUNTER_BIT_DUR_THRESH			( ( COUNTER_TICKS_BIT_DUR ) * ( COUNTER_MARGIN_PERCENTAGE ) / 100 )
#define COUNTER_FILL_TO_ROUND			( ( COUNTER_TICKS_BIT_DUR ) - ( COUNTER_BIT_DUR_THRESH ) ) 

void nssu_init()
{
	// Before enabling EXTI interrupt set pin state of TX pin to '1' (default)
	LL_GPIO_SetOutputPin(D5_SOFTUART_OUT_GPIO_Port, D5_SOFTUART_OUT_Pin);

	// Enable timers for RX and TX
	MX_TIM2_Init();
	MX_TIM3_Init();

	LL_TIM_SetAutoReload(TIM_RX_INST, 0xFFFFFFFF);
	LL_TIM_SetAutoReload(TIM_TX_INST, NSSU_AUTO_RELOAD);

	// Enable EXTI interrupt for handling on slope event
	LL_EXTI_EnableIT_0_31(EXTI_RX_LINE_INST);
	LL_EXTI_EnableRisingTrig_0_31(EXTI_RX_LINE_INST);
	LL_EXTI_EnableFallingTrig_0_31(EXTI_RX_LINE_INST);
}

void nssu_rx_timer_start()
{
	LL_TIM_SetCounter(TIM_RX_INST, 0);
	LL_TIM_EnableCounter(TIM_RX_INST);
}

void nssu_rx_timer_stop()
{
	LL_TIM_DisableCounter(TIM_RX_INST);
}

void nssu_rx_timer_restart()
{
	nssu_rx_timer_stop();
	nssu_rx_timer_start();
}

int nssu_get_rx_pin_state()
{
	return LL_GPIO_IsInputPinSet(D3_SOFTUART_IN_GPIO_Port, D3_SOFTUART_IN_Pin);
}

int nssu_get_num_bits_rcvd()
{
	int ticks = LL_TIM_GetCounter(TIM_RX_INST) + COUNTER_FILL_TO_ROUND;
	LL_TIM_SetCounter(TIM_RX_INST, 0);
	return ticks / COUNTER_TICKS_BIT_DUR;
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
	if(!LL_TIM_IsEnabledIT_UPDATE(TIM_TX_INST))
	{
		LL_TIM_SetCounter(TIM_TX_INST, 0);
		LL_TIM_EnableCounter(TIM_TX_INST);
		LL_TIM_EnableIT_UPDATE(TIM_TX_INST);
	}
}

void nssu_tx_tim_isr_disable()
{
	LL_TIM_DisableIT_UPDATE(TIM_TX_INST);
	LL_TIM_DisableCounter(TIM_TX_INST);
}

