#include <shell.h>
#include <stdio.h>
#include <string.h>
#include "my_shell.h"
#include "main.h"

char shellBuffer[512];
void* g_huart;
Shell shell;


/**
 * @brief shell读取数据函数原型
 *
 * @param data shell读取的字符
 * @param len 请求读取的字符数量
 *
 * @return unsigned short 实际读取到的字符数量
 */
short shellRead(char *data, unsigned short len)
{
    HAL_UART_Receive(g_huart, (uint8_t *)data, len, HAL_MAX_DELAY);
    return len;
}
/**
 * @brief shell写数据函数原型
 *
 * @param data 需写的字符数据
 * @param len 需要写入的字符数
 *
 * @return unsigned short 实际写入的字符数量
 */
short shellWrite(char *data, unsigned short len)
{
    HAL_UART_Transmit(g_huart, (uint8_t *)data, len, HAL_MAX_DELAY);

    return len;
}

void my_shell_init(void* huart)
{
    // 保存串口句柄
    g_huart = huart;
    // 初始化Shell
    shell.read = shellRead;
    shell.write = shellWrite;
    shellInit(&shell, shellBuffer, 512);
    
}

// 电脑通过zmodem命令发送文件到单片机
// 此函数负责参数解析和对应操作
int shell_sz_cmd_proc(int argc, char *argv[])
{
    char buf[32];
    sprintf(buf,"argc=%d\n",argc);
    
    int len = strlen(buf);
    shellWrite(buf, len);
    
    if(argc < 2){
        shellWrite("Usage: sz file.bin\n", strlen("Usage: sz file.bin\n"));
        return -1;
    }
    
    // 验证参数
    
    
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), sz, shell_sz_cmd_proc, receive file via ymodem);
