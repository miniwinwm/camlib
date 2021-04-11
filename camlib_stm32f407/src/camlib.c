/*
*   3V3		-	3V		;		GND		-	GND
*   SCL		-	PB6		;		SDA		-	PB7
*   VS 		-	PC0		;		HS		-	PC3
*   PLK		-	PC1		;		XLK		-	PB8
*   D7		-	PA7    	;		D6		-	PA6
*   D5		-	PA5		;		D4		-	PA4
*   D3		-	PA3		;		D2		-	PA2
*   D1		-	PA1    	;		D0		-	PA0
*   RESET	-	NC  	;		PWDN	-	NC
*/

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"

#define OV7670_WRITE_ADDR 		0x42

const uint8_t OV7670_register_setup[][2] =
{
	{0x12, 0x80},		//Reset registers
	
	// Image format
	{0x12, 0x14},		//QVGA size, RGB mode

	{0x40, 0xd0},		//RGB565
	{0xb0, 0x84},		//Color mode
	
	// Hardware window
	{0x11, 0x01},		//PCLK settings, 15fps
	{0x32, 0x80},		//HREF
	{0x17, 0x17},		//HSTART
	{0x18, 0x05},		//HSTOP
	{0x03, 0x0a},		//VREF
	{0x19, 0x02},		//VSTART
	{0x1a, 0x7a},		//VSTOP
	
	// Scaling numbers
	{0x70, 0x3a},		//X_SCALING
	{0x71, 0x35},		//Y_SCALING
	{0x72, 0x11},		//DCW_SCALING
	{0x73, 0xf0},		//PCLK_DIV_SCALING
	{0xa2, 0x02},		//PCLK_DELAY_SCALING
	
	// Matrix coefficients
	{0x4f, 0x80},		{0x50, 0x80},
	{0x51, 0x00},		{0x52, 0x22},
	{0x53, 0x5e},		{0x54, 0x80},
	{0x58, 0x9e},
	
	// Gamma curve values
	{0x7a, 0x20},		{0x7b, 0x10},
	{0x7c, 0x1e},		{0x7d, 0x35},
	{0x7e, 0x5a},		{0x7f, 0x69},
	{0x80, 0x76},		{0x81, 0x80},
	{0x82, 0x88},		{0x83, 0x8f},
	{0x84, 0x96},		{0x85, 0xa3},
	{0x86, 0xaf},		{0x87, 0xc4},
	{0x88, 0xd7},		{0x89, 0xe8},
	
	// AGC and AEC parameters
	{0xa5, 0x05},		{0xab, 0x07},
	{0x24, 0x95},		{0x25, 0x33},
	{0x26, 0xe3},		{0x9f, 0x78},
	{0xa0, 0x68},		{0xa1, 0x03},
	{0xa6, 0xd8},		{0xa7, 0xd8},
	{0xa8, 0xf0},		{0xa9, 0x90},
	{0xaa, 0x94},		{0x10, 0x00},
	
	// AWB parameters
	{0x43, 0x0a},		{0x44, 0xf0},
	{0x45, 0x34},		{0x46, 0x58},
	{0x47, 0x28},		{0x48, 0x3a},
	{0x59, 0x88},		{0x5a, 0x88},
	{0x5b, 0x44},		{0x5c, 0x67},
	{0x5d, 0x49},		{0x5e, 0x0e},
	{0x6c, 0x0a},		{0x6d, 0x55},
	{0x6e, 0x11},		{0x6f, 0x9f},
	{0x6a, 0x40},		{0x01, 0x40},
	{0x02, 0x60},		{0x13, 0xe7},
	
	// Additional parameters
	{0x34, 0x11},		{0x3f, 0x00},
	{0x75, 0x05},		{0x76, 0xe1},
	{0x4c, 0x00},		{0x77, 0x01},
	{0xb8, 0x0a},		{0x41, 0x18},
	{0x3b, 0x12},		{0xa4, 0x88},
	{0x96, 0x00},		{0x97, 0x30},
	{0x98, 0x20},		{0x99, 0x30},
	{0x9a, 0x84},		{0x9b, 0x29},
	{0x9c, 0x03},		{0x9d, 0x4c},
	{0x9e, 0x3f},		{0x78, 0x04},
	{0x0e, 0x61},		{0x0f, 0x4b},
	{0x16, 0x02},		{0x1e, 0x00},
	{0x21, 0x02},		{0x22, 0x91},
	{0x29, 0x07},		{0x33, 0x0b},
	{0x35, 0x0b},		{0x37, 0x1d},
	{0x38, 0x71},		{0x39, 0x2a},
	{0x3c, 0x78},		{0x4d, 0x40},
	{0x4e, 0x20},		{0x69, 0x00},
	{0x6b, 0x3a},		{0x74, 0x10},
	{0x8d, 0x4f},		{0x8e, 0x00},
	{0x8f, 0x00},		{0x90, 0x00},
	{0x91, 0x00},		{0x96, 0x00},
	{0x9a, 0x00},		{0xb1, 0x0c},
	{0xb2, 0x0e},		{0xb3, 0x82},
	{0x4b, 0x01},
};

uint8_t frame_buffer[160 * 120 * 2];

static void xclk_init(void);
static void sccb_init(void);
static bool ov7670_init(void);
static void cam_paralell_io_init(void);
static void delay(volatile uint16_t count);
static bool sccb_write_reg(uint8_t reg_addr, uint8_t data);

void camlib_init(void)
{
	xclk_init();
	sccb_init();
	cam_paralell_io_init();
	ov7670_init();
	I2C_DeInit(I2C1);
}

uint16_t *camlib_get_frame(void)
{
	return (uint16_t *)&frame_buffer[0];
}

void camlib_capture(void)
{
	uint_fast32_t i;

	__disable_irq();

	// wait for VS
	while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) == Bit_RESET)
	{
	}
	while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) == Bit_SET)
	{
	}
	i = 0UL;

	for (uint_fast8_t y = 0U; y < 120U; y++)
	{
		// wait for first HS
		while (!(GPIOC->IDR & GPIO_Pin_3))
		{
		}

		while ((GPIOC->IDR & GPIO_Pin_3))
		{
		}

		// wait for second HS
		while (!(GPIOC->IDR & GPIO_Pin_3))
		{
		}

		for (uint_fast16_t x = 0U; x < 160U; x++)
		{
			// wait for PLK
			while (!(GPIOC->IDR & GPIO_Pin_1))
			{
			}

			frame_buffer[i] = GPIOA->IDR;
			i++;

			// wait for ~PLK
			while ((GPIOC->IDR & GPIO_Pin_1))
			{
			}

			// wait for PLK
			while (!(GPIOC->IDR & GPIO_Pin_1))
			{
			}

			frame_buffer[i] = GPIOA->IDR;
			i++;

			// wait for ~PLK
			while ((GPIOC->IDR & GPIO_Pin_1))
			{
			}

			// repeat doing dummy read
			while (!(GPIOC->IDR & GPIO_Pin_1))
			{
			}

			while ((GPIOC->IDR & GPIO_Pin_1))
			{
			}

			while (!(GPIOC->IDR & GPIO_Pin_1))
			{
			}

			while ((GPIOC->IDR & GPIO_Pin_1))
			{
			}
		}
	}

	__enable_irq();
}

static void delay(volatile uint16_t count)
{
	while (count--)
	{
	}
}

static void xclk_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_BaseStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM10);

	TIM_BaseStruct.TIM_Period = 1;
	TIM_BaseStruct.TIM_Prescaler = 0;
	TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM10, &TIM_BaseStruct);

	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM10, &TIM_OCInitStruct);

	TIM_Cmd(TIM10, ENABLE);
}

static void sccb_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_Init(I2C1,&I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
}

static bool sccb_write_reg(uint8_t reg_addr, uint8_t data)
{
	uint32_t timeout = 0x7FFFFF;
	
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
	{
		if ((timeout--) == 0)
		{
			return false;
		}			
	}
	
	// Send start bit
	I2C_GenerateSTART(I2C1, ENABLE);
	
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if ((timeout--) == 0)
		{
			return false;
		}
	}
	
	// Send slave address (camera write address)
	I2C_Send7bitAddress(I2C1, OV7670_WRITE_ADDR, I2C_Direction_Transmitter);
  
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if ((timeout--) == 0)
		{
			return false;
		}
	}

	// Send register address
	I2C_SendData(I2C1, reg_addr);
	
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if ((timeout--) == 0)
		{
			return false;
		}
	}

	// Send new register value
	I2C_SendData(I2C1, data);
	
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if ((timeout--) == 0)
		{
			return false;
		}
	}

	// Send stop bit
	I2C_GenerateSTOP(I2C1, ENABLE);

	return true;
}

static bool ov7670_init(void)
{
	uint8_t i;
	bool result;

	for (i = 0U; i < sizeof(OV7670_register_setup) / 2; i++)
	{
		result = sccb_write_reg(OV7670_register_setup[i][0], OV7670_register_setup[i][1]);

		if (result == false)
		{
			break;
		}
			
		delay(0xFFFF);
	}
	
	return result;
}

static void cam_paralell_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
