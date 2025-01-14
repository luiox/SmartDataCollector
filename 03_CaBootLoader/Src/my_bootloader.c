#include "my_bootloader.h"
#include "main.h"
#include <string.h>
#include <stdbool.h>
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include <core_cm3.h>
#include "my_shell.h"


#define BUFFER_SIZE 2*1024 // 定义缓冲区大小

uint32_t g_jump_init_flag __attribute__((at(0x20000000), zero_init)); 

void soft_reset_system(void)
{
    NVIC_SystemReset();
}


// 初始化环形缓冲区
void BufferInit(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    memset(rb->buffer, 0, BUFFER_SIZE);
}

// 检查缓冲区是否已满
bool BufferIsFull(RingBuffer *rb) {
    return ((rb->head + 1) % BUFFER_SIZE) == rb->tail;
}

// 检查缓冲区是否为空
bool BufferIsEmpty(RingBuffer *rb) {
    return rb->head == rb->tail;
}

// 向缓冲区写入一个字节
bool BufferWrite(RingBuffer *rb, uint8_t data) {
    if (BufferIsFull(rb)) {
        // 如果缓冲区已满，则写入失败
        return false;
    }
    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % BUFFER_SIZE;
    return true;
}

// 从缓冲区弹出一个字节
bool BufferRead(RingBuffer *rb, uint8_t *data) {
    if (BufferIsEmpty(rb)) {
        // 如果缓冲区为空，则读取失败
        return false;
    }
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % BUFFER_SIZE;
    return true;
}

extern UART_HandleTypeDef huart1;

UART_HandleTypeDef* g_uart;

void* app_address;

RingBuffer myBuffer;



uint8_t buf[16];
uint8_t len_buf[8];
int download_flag;

// 偏移20k作为APP的起始地址 
#define APP_START_ADDR  ((uint32_t)0x08000000+20*1024)

void jump_to_app(const void* app_addr)
{

}


// 初始化boot
void myboot_init(UART_HandleTypeDef* uart)
{
    g_uart = uart;
    
    BufferInit(&myBuffer);
    
    
    my_shell_init(uart);

     __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

// boot循环
void myboot_loop(void)
{
    // shellTask(NULL);
    
    
    download_flag = 0;
    // 开始接收数据
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
    
    // 等待可以开始接收文件
    while(download_flag == 0);
    
    HAL_UART_Receive(&huart1, len_buf, 4, HAL_MAX_DELAY);
    
    uint32_t byte_size = (uint32_t)(
        (len_buf[0] << 0)  |
        (len_buf[1] << 8)  |
        (len_buf[2] << 16) |
        (len_buf[3] << 24));
    
    // 循环读取
    
    // 开始接收数据
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
    int addr = APP_START_ADDR; 
    FLASH_EraseInitTypeDef EraseInitStruct;
  // 解锁
	HAL_FLASH_Unlock();

    while(byte_size != 0){
        // 设置擦除的页
        EraseInitStruct.TypeErase     = FLASH_TYPEERASE_PAGES;
        EraseInitStruct.NbPages       = 1;
        EraseInitStruct.PageAddress   = addr;
        uint32_t err;
        HAL_FLASHEx_Erase(&EraseInitStruct, &err);
        
        // 写入一页
        for(int i = 0; i<FLASH_PAGE_SIZE;i++){
            uint8_t dataToWrite;
            while(BufferRead(&myBuffer, &dataToWrite));
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr+i, dataToWrite);
        }
        
        // 往后加一页
        addr += FLASH_PAGE_SIZE;
    }

  HAL_FLASH_Lock();
    
    
    // 跳转到APP程序

	__IO  uint32_t* app_addr = (__IO  uint32_t*) (APP_START_ADDR);

	void (*app_start)(void) = (void (*)(void)) app_addr;
	
	__set_MSP( *(__IO  uint32_t*) (APP_START_ADDR));

	app_start();

}

