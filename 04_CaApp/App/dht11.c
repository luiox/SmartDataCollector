#include "dht11.h"
#include "main.h"
#include "tim.h"

TIM_HandleTypeDef *g_htim;
GPIO_TypeDef *g_port;
uint16_t g_pin;

inline static void dht_delay_us(uint32_t us) {
  HAL_TIM_Base_Start(g_htim); // 启动定时器
  while (g_htim->Instance->CNT < us)
    ;
  g_htim->Instance->CNT = 0;
  HAL_TIM_Base_Stop(g_htim); // 关闭定时器
}

inline static void dht_delay_ms(uint32_t ms) { HAL_Delay(ms); }

inline static void dht_set_pin_input() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = g_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(g_port, &GPIO_InitStruct);
}

inline static void dht_set_pin_output() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = g_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(g_port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(g_port, g_pin, GPIO_PIN_RESET);
}

inline static void dht_set_pin_high() {
  HAL_GPIO_WritePin(g_port, g_pin, GPIO_PIN_SET);
}

inline static void dht_set_pin_low() {
  HAL_GPIO_WritePin(g_port, g_pin, GPIO_PIN_RESET);
}

inline static uint8_t dht_get_pin_state() {
  return HAL_GPIO_ReadPin(g_port, g_pin) == GPIO_PIN_SET ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////

void dht11_init(TIM_HandleTypeDef *htim, GPIO_TypeDef *port, uint16_t pin)
{
    g_htim = htim;
    g_port = port;
    g_pin = pin;
}

static void dht11_start(void) {
  dht_set_pin_output(); // IO设置为输出模式。在传输的最开始，MCU要向DHT11发送信号
  dht_set_pin_low(); // IO->DHT11:先拉低电平18ms（应时序要求）
  dht_delay_ms(18);
  dht_set_pin_high(); // IO->DHT11:随后拉高电平20us
  dht_delay_us(20);
}

static int8_t dht11_check(void) {
  int retry = 0;
  // MCU通过IO向DHT11发送请求完毕。接下来DHT11向IO发送响应,IO转为输入模式。在这之后就开始信号的转译读取。
  dht_set_pin_input();

  dht_delay_us(20);
  // DHT11发回低电平响应（读取到低电平，说明DHT11有响应）
  if (dht_get_pin_state() != 0)
    return -1;

  //接下来，DHT11拉低电平一段时间后拉高电平一段时间
  while (dht_get_pin_state() == 0 && retry < 100) {
    dht_delay_us(1);
    retry++;
  }
  retry = 0;
  while (dht_get_pin_state() == 1 && retry < 100) {
    dht_delay_us(1);
    retry++;
  }
  retry = 0;
  return 0;
}

// 用于转译采集DHT11发送给IO口的电平信号（8位）。
uint8_t dht11_read_byte(void) //从DHT11读取一位（8字节）信号
{
  uint8_t ReadData = 0;
  uint8_t temp;      //临时存放信号电平（0或1）
  uint8_t retry = 0; // retry用于防止卡死
  uint8_t i;
  for (i = 0; i < 8; i++) //一次温湿度信号读取八位
  {
    //等待直到DHT11输出高电平：当PA5=1，上升沿，表示开始接受数据，可以判断0 or
    // 1，跳出循环，执行后续判断（若PA5=0，将一直循环等待）
    while (dht_get_pin_state() == 0 && retry < 100)

    {
      dht_delay_us(1);
      retry++; // retry防止PA5读取不到数据卡死在这一步，当经历100us后retry自增到100，跳出循环。
    }
    retry = 0;

    dht_delay_us(40); //延时30us
    //根据时序图，DHT传回高电平信号维持26us~28us表示0，	维持70us表示1
    //延时30us后，如果IO读取到仍是高电平，说明采集到1；如果IO读取到低电平，说明采集到0
    //读取电平信号暂存temp内，随后会压入ReadData中
    if (dht_get_pin_state() == 1)
      temp = 1;
    else
      temp = 0;
    //等待直到DHT11输出低电平，表示退出。本轮1bit信号接收完毕。
    while (dht_get_pin_state() == 1 && retry < 100)

    {
      dht_delay_us(1);
      retry++;
    }
    retry = 0;

    ReadData <<= 1; // ReadData内信号先全部左移一位，空出末尾位置
    ReadData |= temp; //将temp写入ReadData
  }

  return ReadData;
}

int8_t dht11_read(float *temperature, float *humidity) {
  uint8_t data[5] = {0x00}; // Data存储读取的温湿度信息

  uint8_t i;

  dht11_start();

  int ret = dht11_check();
  if (ret != 0) {
    return -1;
  }

  //一共传输40位，一次DHT_Read_Byte返回8位，共读取5次。存储在Data[]中。（Data[]定义为全局）
  for (i = 0; i < 5; i++) {
    data[i] = dht11_read_byte(); //每次读取一字节（8位）
  }

  //说明：Data[0]湿度，
  // Data[2]温度。Data[1]和Data[3]分别为0和2的小数位。Data[4]用于校验。

  // 校验
  if (data[0] + data[1] + data[2] + data[3] != data[4]) {
    return -1;
  }
  *humidity = data[0] + (float)data[1] / 256;
  *temperature = data[2] + (float)data[3] / 256;
  return 0;
}
