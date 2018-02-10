#include "nssu_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "nss_uart.h"
#include "utils.h"
#include "log.h"

static TaskHandle_t nssu_task_handle;

extern void nssu_init(); 

static void nssu_task(void *params);

void create_nssu_task()
{
	xTaskCreate(nssu_task, "nssu", 256, NULL, 1, &nssu_task_handle); 
}

static void nssu_task(void *params)
{
	nssu_init();

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
}

