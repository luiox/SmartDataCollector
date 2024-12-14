#include <app.h>
#include <dht11.h>
#include <debug.h>
#include <stdio.h>
#include <string.h>
#include <tim.h>
#include <usart.h>
#include <esp8266.h>

void app_init(void)
{
    debug_init(&huart1);
   dht11_init(&htim4,dht11_GPIO_Port, dht11_Pin);
    HAL_Delay(1000);

  
  //char buf[256];
    esp8266_init(&huart2);
    
    esp8266_disconnect_server();
    HAL_Delay(2000);
    esp8266_connect_wifi("fst_hot", "123456fst");
    HAL_Delay(2000);
    
    esp8266_set_ntp_server("ntp1.aliyun.com");
    HAL_Delay(2000);
    esp8266_set_user_config("dev1&k1o1r3JfU8U","eb21a540414de3e2c2073507ce7c39bfd0c3df1c79f820a7b6069ac63c0a82ed");
    HAL_Delay(2000);
    esp8266_set_client_id("k1o1r3JfU8U.dev1|securemode=2\\,signmethod=hmacsha256\\,timestamp=1733623875310|");
    HAL_Delay(2000);
    esp8266_connect_server("iot-06z00ddifrvd8u5.mqtt.iothub.aliyuncs.com", 1883);
    HAL_Delay(2000);
    
}

void app_loop(void)
{
    float temp, humi;
    if(dht11_read(&temp,&humi) == 0) {
        esp8266_public_float("temperature", temp);
        HAL_Delay(2000);
        esp8266_public_float("humidity", humi);
        HAL_Delay(2000);
        DEBUG_INFO("temp:%f,humi:%f", temp, humi);
        
        HAL_Delay(5000);
    }
    HAL_Delay(1500);
}
