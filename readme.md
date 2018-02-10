# Not-so-soft-UART port for STM32F411CE

This is an example port of [NSSU](https://github.com/KKoovalsky/NotSoSoftUart). FreeRTOS and LL are used in this 
project. 

## Downloading

As multiple submodules are used one must clone the repository with `recursive` flag:
```
git clone --recursive https://github.com/KKoovalsky/NotSoSoftUart.git
```

## Running

To build tests:

```
cd build
cmake .. -DUNIT_TEST_LOCAL
make
```

The tests run on local computer (not on target device).

To build the application run:
```
cd build
cmake ..
make
```

## Implementation description

The functions used within NSSU are defined inside `app/src/hw/nss_uart_port.c`. To provide some information how to
implement a port for other hardware, here I describe how external NSSU functions are programmed with this port.

#### `void nssu_init()`

Firstly the TX pin is set (default UART bus state):
```
LL_GPIO_SetOutputPin(D5_SOFTUART_OUT_GPIO_Port, D5_SOFTUART_OUT_Pin);
```
and then the timers, one for RX and one for TX side, are initialized:
```
MX_TIM2_Init();
MX_TIM3_Init();
```
Those functions were generated automatically using CubeMX.
Then the autoreload registers are set:
```
LL_TIM_SetAutoReload(TIM_RX_INST, 0xFFFFFFFF);
LL_TIM_SetAutoReload(TIM_TX_INST, NSSU_AUTO_RELOAD);
```
For the RX side we don't want the timer to overflow because later number of bits received is calculated and it's done
basing on the counter value which is cleared when it will reach the autoreload register's value.

For the TX side the autoreload register value is set to such a value to generate interrupt periodically, with a 
frequency equal to baud rate. `NSSU_AUTO_RELOAD` is computed using this equation:

```
NSSU_AUTO_RELOAD = CPU_CLK_FREQ / NSSU_BAUD_RATE - 1 
```

Where `CPU_CLK_FREQ` and `NSSU_BAUD_RATE` are `#define`s provided by the user.

Then the interrupt invoked on falling/raising edge is enabled:

```
LL_EXTI_EnableIT_0_31(EXTI_RX_LINE_INST);
LL_EXTI_EnableRisingTrig_0_31(EXTI_RX_LINE_INST);
LL_EXTI_EnableFallingTrig_0_31(EXTI_RX_LINE_INST);
``` 

Functions which initialize `HAL` library and `GPIOs` are called earlier, in `main` function.

#### `void nssu_rx_timer_start()`

Here the counter value is cleared and the counter is enabled:
```
LL_TIM_SetCounter(TIM_RX_INST, 0);
LL_TIM_EnableCounter(TIM_RX_INST);
```

#### `void nssu_rx_timer_stop()`

This function simply stops the counter:
```
LL_TIM_DisableCounter(TIM_RX_INST);
```

#### `void nssu_rx_timer_restart()`

Calls `nssu_rx_timer_stop` and then `nssu_rx_timer_start` in that order.

#### `nssu_get_rx_pin_state()`

Uses LL library to get the pin state:

```
return LL_GPIO_IsInputPinSet(D3_SOFTUART_IN_GPIO_Port, D3_SOFTUART_IN_Pin);
```

#### `int nssu_get_num_bits_rcvd()`

This function calculates number of bits received between two next edges. Firstly value of the counter is obtained:
```
int ticks = LL_TIM_GetCounter(TIM_RX_INST) + COUNTER_FILL_TO_ROUND;
```
`COUNTER_FILL_TO_ROUND` is used to take into account a margin that bit can last shorter than it is assumed. This
constant is computed like that:
```
COUNTER_FILL_TO_ROUND =  COUNTER_TICKS_BIT_DUR - COUNTER_BIT_DUR_THRESH
```
where `COUNTER_TICKS_BIT_DUR` is the value of the counter which should correspond to duration of one bit for such a
baud rate. `COUNTER_BIT_DUR_THRESH` is calculated in such a way:
```
COUNTER_BIT_DUR_THRESH = COUNTER_TICKS_BIT_DUR * COUNTER_MARGIN_PERCENTAGE / 100
```
This constant defines a threshold for the counter value above which we decide that a bit was sent.
`COUNTER_MARGIN_PERCENTAGE` and `COUNTER_TICKS_BIT_DUR` are defined by the user.

When `COUNTER_FILL_TO_ROUND` is added to the value of the counter then when the result of this operation is divided
by the duration of one bit:
```
ticks / COUNTER_TICKS_BIT_DUR
```
we get the number of bits received between slopes including the margin.

This simple example explains it clearer: if one wants the value `0.8` rounded to `1` and having always floor rounding
provided, then adding `0.2` to the initial value will round `0.8` to `1`:

```
round(0.8 + 0.2) = 1;
round(0.7 + 0.2) = 0
round(0.9 + 0.2) = 1;
```
Now when e.g. the counter value is equal to `2850` and `COUNTER_TICKS_BIT_DUR = 1000` and 
`COUNTER_MARGIN_PERCENTAGE=80` then (assuming integer-like rounding):
```
number of bytes received = ( 2850 + ((100 - 80) / 100) * 1000 ) / 1000 = (2850 + 200) / 1000 = 3
```

#### `void nssu_set_tx_pin_state(int state)`

Sets the TX pin basing on `state` argument:
```
if(state)
	LL_GPIO_SetOutputPin(D5_SOFTUART_OUT_GPIO_Port, D5_SOFTUART_OUT_Pin);
else
	LL_GPIO_ResetOutputPin(D5_SOFTUART_OUT_GPIO_Port, D5_SOFTUART_OUT_Pin);
```

#### `void nssu_tx_tim_isr_enable()`

Does the same thing like `nssu_rx_timer_start`, but additionally enables the interrupt by:
```
LL_TIM_EnableIT_UPDATE(TIM_TX_INST);
```

#### `void nssu_tx_tim_isr_disable()`

Does the same thing like `nssu_rx_timer_stop`, but additionally disables the interrupt by:
```
LL_TIM_DisableIT_UPDATE(TIM_TX_INST);
```

## Application

In this example FreeRTOS task was created which implements simple echo:

```
for(;;)
{
	// Check each ms whether there are some new bytes received 
	vTaskDelay(pdMS_TO_TICKS(1));
	int n = nssu_get_num_bytes_rcvd();
	if(n)
	{
		// When there are some bytes received then get them
		char rcv_buf[n];
		for(int i = 0; i < n ; ++i)
			rcv_buf[i] = nssu_get_rcvd_byte();

		// And make an echo
		nssu_transmit_data((uint8_t *) rcv_buf, n);
	}
}
```
