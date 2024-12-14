//
// 用于调试时，串口打印有问题的代码
// 解决重定向fputc后，直接调用printf导致这些调试代码不容易删除的问题
// 使用方法：1. 初始化串口相关内容，（这通常是使用HAL库或者是手动调用标准固件库实现）
//           2. 调用debug_init来初始化调试模块，（这用于确定信息输出到哪个串口）
 //           3. 把DEBUG_INFO宏当做printf来打印信息，\
               （注意，这需要定义USE_DEBUG_MODE宏，这样的目的是，在非DEBUG模式下，可以把相关调试代码替换为空）
//
//

#ifndef MYLIB_BASE_DEBUG_H
#define MYLIB_BASE_DEBUG_H

#include "main.h"

#define PLATFORM_STM32 1
#define USE_DEBUG_MODE 1
#if PLATFORM_STM32

#    ifdef USE_DEBUG_MODE
#        define DEBUG_INFO(fmt, ...) \
            debug_print("[info][%s][%d]:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#    else
#        define DEBUG_INFO(fmt, ...)
#    endif   // USE_DEBUG_MODE

void debug_init(UART_HandleTypeDef* huart);

#elif PLATFORM_MSPM0

#    ifdef USE_DEBUG_MODE

#        include <stdio.h>

// 为了解决mspm0下载不进去的奇怪bug只能这样绕过去
#        define DEBUG_INFO(fmt, ...)                                                             \
            {                                                                                    \
                char __s_buf[128];                                                               \
                sprintf(__s_buf, "[info][%s][%d]:" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
                usart_send_string(__s_buf);                                                      \
            }
#    else
#        define DEBUG_INFO(fmt, ...)
#    endif   // USE_DEBUG_MODE

void debug_init(UART_Regs* huart);
void usart_send_string(const char* str);
#else
#    error unsupported microcontroller type
#endif

void debug_print(const char* fmt, ...);


#if USE_DEBUG_ASSERT
#    define MYLIB_DEBUG_ASSERT(expr)                                  \
        if (!(expr)) {                                                \
            debug_print("assert failed: %s:%d\n", __FILE__, __LINE__); \
        }
#else
#    define MYLIB_DEBUG_ASSERT(expr)
#endif

#endif   // !MYLIB_BASE_DEBUG_H
