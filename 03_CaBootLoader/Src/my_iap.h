#ifndef MY_IAP_H
#define MY_IAP_H

#include <stdint.h>

void iap_init(void);
void iap_write(uint8_t* data, uint32_t size, uint32_t address);

void ym_write_bytes(uint8_t* data,uint32_t len);
void ym_error_handler(int code);


#endif // !MY_IAP_H
