#ifndef MYLIB_DEVICE_ESP8266_H
#define MYLIB_DEVICE_ESP8266_H

#include "main.h"

void esp8266_init(UART_HandleTypeDef* huart);
void esp8266_irq_handler(UART_HandleTypeDef* huart);

int esp8266_connect_wifi(const char* ssid, const char* password);
int esp8266_set_ntp_server(const char* server);
int esp8266_set_user_config(const char* username, const char* password);
int esp8266_set_client_id(const char* client_id);
int esp8266_connect_server(const char* server, uint16_t port);
int esp8266_public_float(const char* name, const float val);
int esp8266_disconnect_server(void);
int esp8266_disconnect_wifi(void);
int esp8266_restart(void);

#endif
