#ifndef NSS_UART_PORT_H
#define NSS_UART_PORT_H

// Should be called within ISR which handles bit counting for the receiver
void handle_nssu_rx_tim_overflow();

#endif /* NSS_UART_PORT_H */
