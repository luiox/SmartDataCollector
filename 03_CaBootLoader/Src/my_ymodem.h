// 适用于单片机的Ymodem-1k协议实现

#ifndef MY_YMODEM_H
#define MY_YMODEM_H

#include <stdint.h>


#define YMODEM_SOH 0x01
#define YMODEM_STX 0x02
#define YMODEM_EOT 0x04
#define YMODEM_ACK 0x06
#define YMODEM_NAK 0x15
#define YMODEM_CAN 0x18
#define YMODEM_C 0x43

typedef void (*ymodem_write_bytes_func_t)(uint8_t* data,uint32_t len);
typedef void (*ymodem_error_handler_t)(int code);

typedef void* ymodem_handle_t;

// 这个函数需要在中断里调用，利用中断读取数据
void ymodem_read_byte(ymodem_handle_t ymodem, uint8_t byte);

// 初始化ymodem
void ymodem_init(ymodem_handle_t ymodem, ymodem_write_bytes_func_t write_bytes,
    ymodem_error_handler_t error_handler);

// 发送文件，此函数暂时不实现，因为BootLoader阶段是没有文件系统的
// 之后支持文件系统后，再实现
void ymodem_send(ymodem_handle_t ymodem, char* filename, uint32_t file_size);

// 接收文件，这个无callback先不实现，因为没有文件系统
void ymodem_recv(ymodem_handle_t ymodem, char* filename, uint32_t file_size);

// 带callback的接收文件
// 每次接收到1k的数据就会调用callback函数
// bytes_received是已经接收到的字节数
// data是接收到的数据
void ymodem_recv_with_callback(ymodem_handle_t ymodem, 
    char* filename, uint32_t* file_size, 
    void (*callback)(uint32_t bytes_received, uint8_t* data));

#define YMODEM_ERROR_PACKET_SIZE 1
#define YMODEM_ERROR_CRC 2

#endif // !MY_YMODEM_H
