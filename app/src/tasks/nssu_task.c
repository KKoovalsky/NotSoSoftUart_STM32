#include "nssu_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "nss_uart.h"
#include "utils.h"
#include "log.h"

static TaskHandle_t nssu_task_handle;

extern void init_not_so_soft_uart(); 

static void nssu_task(void *params);

void create_nssu_task()
{
	xTaskCreate(nssu_task, "nssu", 256, NULL, 1, &nssu_task_handle); 
}

void byte_rcvd_callback()
{
	BaseType_t higher_prior_task_woken;
	vTaskNotifyGiveFromISR(nssu_task_handle, &higher_prior_task_woken);
	portEND_SWITCHING_ISR(higher_prior_task_woken);
}

static void nssu_task(void *params)
{
	init_not_so_soft_uart();
	
	register_nssu_byte_received_callback(byte_rcvd_callback);

	// Try send some bytes
	char some_data[] = "MAKAPAKA";
	transmit_data((uint8_t *) some_data, array_len(some_data));

	for(;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		char byte = pop_byte_from_rx_buf();
		log_byte(byte);
	}
}
