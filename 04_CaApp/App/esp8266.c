#include "esp8266.h"
#include <debug.h>
#include <string.h>

UART_HandleTypeDef *g_huart;
void esp8266_init(UART_HandleTypeDef *huart) { g_huart = huart; }
void esp8266_irq_handler(UART_HandleTypeDef *huart) {}

void esp8266_send_str(const char *str) {
  HAL_UART_Transmit(g_huart, str, strlen(str), 1000);
}

char buf[128];
int esp8266_connect_wifi(const char *ssid, const char *password) {
  esp8266_send_str("AT+CWMODE=3\r\n");
  HAL_Delay(2000);
  sprintf(buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
  esp8266_send_str(buf);

  return 0;
}
int esp8266_set_ntp_server(const char *server) {
  sprintf(buf, "AT+CIPSNTPCFG=1,8,\"%s\"\r\n", server);
  esp8266_send_str(buf);
  return 0;
}
int esp8266_set_user_config(const char *username, const char *password) {
  sprintf(buf, "AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n",
          username, password);
  esp8266_send_str(buf);
  return 0;
}
int esp8266_set_client_id(const char *client_id) {
  sprintf(buf, "AT+MQTTCLIENTID=0,\"%s\"\r\n", client_id);
  esp8266_send_str(buf);
  return 0;
}
int esp8266_connect_server(const char *server, uint16_t port) {
  sprintf(buf, "AT+MQTTCONN=0,\"%s\",%u,1\r\n", server, port);
  esp8266_send_str(buf);
  return 0;
}
int esp8266_public_float(const char *name, const float val) {
  sprintf(buf,
          "AT+MQTTPUB=0,\"/sys/k1o1r3JfU8U/dev1/thing/event/property/"
          "post\",\"{params:{\\\"%s\\\":%.3f}}\",0,0\r\n",
          name, val);
  esp8266_send_str(buf);
  return 0;
}
int esp8266_disconnect_server(void) {
  esp8266_send_str("AT+MQTTCLEAN=0\r\n");
  return 0;
}
int esp8266_disconnect_wifi(void) {
  esp8266_send_str("AT+CWQAP\r\n");
  return 0;
}
int esp8266_restart(void) {
  esp8266_send_str("AT+RST\r\n");
  return 0;
}
