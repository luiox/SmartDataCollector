#ifndef MYBOOTLOADER_H
#define MYBOOTLOADER_H

#include "main.h"


typedef struct {
    uint8_t buffer[2*1024];
    volatile uint32_t head;
    volatile uint32_t tail;
} RingBuffer;

// 初始化boot
void myboot_init(UART_HandleTypeDef* uart);
// boot循环
void myboot_loop(void);


#endif // !MYBOOTLOADER_H
