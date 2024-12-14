#ifndef MYLIB_DEVICE_DHT11_H
#define MYLIB_DEVICE_DHT11_H

#include "main.h"

void dht11_init(TIM_HandleTypeDef *htim, GPIO_TypeDef *port, uint16_t pin);
int8_t dht11_read(float *temperature, float *humidity);

#endif
