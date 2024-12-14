#include "debug.h"
#include <stdio.h>
#include <stdio.h>    // 包含 vsprintf 和 vprintf
#include <stdarg.h>   // 包含 va_list, va_start, va_end

// 定义一个缓冲区大小
#define DEBUG_BUFFER_SIZE 256

#if PLATFORM_STM32

static UART_HandleTypeDef* g_huart = NULL;

static void usart_send_string(const char* str)
{
    unsigned int k = 0;
    do {
        HAL_UART_Transmit(g_huart, (uint8_t*)(str + k), 1, 1000);
        k++;
    } while (*(str + k) != '\0');
}

void debug_init(UART_HandleTypeDef* huart)
{
    g_huart = huart;
}

#elif PLATFORM_MSPM0
static UART_Regs* g_huart = NULL;

void usart_send_string(const char* str)
{
    unsigned int k = 0;
    do {
        // 阻塞式写入，会先判断FIFO是否为慢，如果慢，需要等待
        // 这里是*(str + k)，先指针偏移，解引用以后强制类型转换，不是乘法
        DL_UART_Main_transmitDataBlocking(g_huart, (uint8_t) * (str + k));
        k++;
    } while (*(str + k) != '\0');
}

void debug_init(UART_Regs* huart)
{
    g_huart = huart;
}
#else
#    error unsupported microcontroller type
#endif

// 平台无关，无需修改
void debug_print(const char* fmt, ...)
{
    va_list args;                     // 定义一个 va_list 类型的变量，用来存放参数
    char buffer[DEBUG_BUFFER_SIZE];   // 定义一个字符数组，用来存放格式化后的字符串

    va_start(args, fmt);   // 初始化 args，fmt 是最后一个固定参数

    // 使用 vsprintf 将参数格式化到 buffer 中
    vsprintf(buffer, fmt, args);

    va_end(args);   // 清理 args

    // 输出格式化后的字符串
    // 这里是发送到串口
    usart_send_string(buffer);
}
