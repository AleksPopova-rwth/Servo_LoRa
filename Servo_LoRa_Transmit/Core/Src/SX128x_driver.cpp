#include <main.h>
#include "SX128x_OBJ.h"

//---------------------------------------------------------------------------------------------
// low level driver
//---------------------------------------------------------------------------------------------
#if defined SOPHIA_V1
extern I2C_HandleTypeDef hi2c2;
extern SPI_HandleTypeDef hspi2;
uint8_t SX128x_OBJ::HalGpioRead(GpioPinFunction_t func)
{
	uint8_t rx = HAL_OK;
	HAL_StatusTypeDef rv;
	switch (func)
	{
		case GPIO_PIN_RESET:
			rv = HAL_I2C_Mem_Read (&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			if (rv != HAL_OK)
			{
				Error_Handler();
			}
			return (rx & 0x08) != 0;
			break;
		case GPIO_PIN_BUSY:
			rv = HAL_I2C_Mem_Read (&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			if (rv != HAL_OK)
			{
				Error_Handler();
			}
			return (rx & 0x04) != 0;
			break;
		case GPIO_PIN_DIO1:
			rv = HAL_I2C_Mem_Read (&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			if (rv != HAL_OK)
			{
				Error_Handler();
			}
			return (rx & 0x02) != 0;
			break;
		case GPIO_PIN_DIO2:
			rv = HAL_I2C_Mem_Read (&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			if (rv != HAL_OK)
			{
				Error_Handler();
			}
			return (rx & 0x01) != 0;
			break;
		case GPIO_PIN_DIO3:
			return 0;
			break;
		
		default:
			break;
	}

}

void SX128x_OBJ::HalGpioWrite(GpioPinFunction_t func, uint8_t value)
{
	uint8_t rx;
	HAL_StatusTypeDef rv;
	if(value == 0)
	{
		switch (func)
		{
		case GPIO_PIN_RESET:
			rv = HAL_I2C_Mem_Read (&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			rx &= 0xF7;
			rv = HAL_I2C_Mem_Write(&hi2c2, 0x78, 0x01, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			if (rv != HAL_OK)
			{
				Error_Handler();
			}
			break;
		case GPIO_PIN_BUSY:
			rv = HAL_I2C_Mem_Read (&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			rx &= 0xFB;
			rv = HAL_I2C_Mem_Write(&hi2c2, 0x78, 0x01, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			if (rv != HAL_OK)
			{
				Error_Handler();
			}
			break;
		case GPIO_PIN_DIO1:
			rv = HAL_I2C_Mem_Read (&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			rx &= 0xFD;
			rv = HAL_I2C_Mem_Write(&hi2c2, 0x78, 0x01, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			if (rv != HAL_OK)
			{
				Error_Handler();
			}
			break;
		case GPIO_PIN_DIO2:
			rv = HAL_I2C_Mem_Read (&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			rx &= 0xFD;
			rv = HAL_I2C_Mem_Write(&hi2c2, 0x78, 0x01, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			if (rv != HAL_OK)
			{
				Error_Handler();
			}
			break;
		case GPIO_PIN_DIO3:
			return;
			break;
		default:
			break;
		}
	}
	else
	{
		switch (func)
		{
		case GPIO_PIN_RESET:
			rv = HAL_I2C_Mem_Read (&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			rx |= 0x08;
			rv = HAL_I2C_Mem_Write(&hi2c2, 0x78, 0x01, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			if (rv != HAL_OK)
			{
				Error_Handler();
			}
			break;
		case GPIO_PIN_BUSY:
			rv = HAL_I2C_Mem_Read (&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			rx |= 0x04;
			rv = HAL_I2C_Mem_Write(&hi2c2, 0x78, 0x01, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			if (rv != HAL_OK)
			{
				Error_Handler();
			}
			break;
		case GPIO_PIN_DIO1:
			rv = HAL_I2C_Mem_Read (&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			rx |= 0x02;
			rv = HAL_I2C_Mem_Write(&hi2c2, 0x78, 0x01, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			if (rv != HAL_OK)
			{
				Error_Handler();
			}
			break;
		case GPIO_PIN_DIO2:
			rv = HAL_I2C_Mem_Read (&hi2c2, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			rx |= 0x01;
			rv = HAL_I2C_Mem_Write(&hi2c2, 0x78, 0x01, I2C_MEMADD_SIZE_8BIT, &rx, 1, 10);
			if (rv != HAL_OK)
			{
				Error_Handler();
			}
			break;
		case GPIO_PIN_DIO3:
			return;
			break;
		default:
			break;
		}
	}
}

void SX128x_OBJ::HalSpiTransfer(uint8_t *buffer_in, const uint8_t *buffer_out, uint16_t size)
{
	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)buffer_out, buffer_in, size, 1);
	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)GPIO_PIN_SET);
}

void SX128x_OBJ::HalSpiTransferDelay(uint8_t *buffer_in, const uint8_t *buffer_out, uint16_t size)
{
	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)buffer_out, buffer_in, size, 1);
	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)GPIO_PIN_SET);
}

#elif defined SOPHIA_V2
extern SPI_HandleTypeDef hspi2;

uint8_t SX128x_OBJ::HalGpioRead(GpioPinFunction_t func)
{
	switch (func)
	{
		case GPIO_PIN_RESET:
			return HAL_GPIO_ReadPin(SX1280_RST_GPIO_Port, SX1280_RST_Pin);
		case GPIO_PIN_BUSY:
			return HAL_GPIO_ReadPin(SX1280_BUSY_GPIO_Port, SX1280_BUSY_Pin);
		case GPIO_PIN_DIO1:
			return 0;
		case GPIO_PIN_DIO2:
			return 0;
		case GPIO_PIN_DIO3:
			return 0;
		default:
			return 0;
	}
}


void SX128x_OBJ::HalGpioWrite(GpioPinFunction_t func, register uint8_t value)
{
	switch (func)
	{
	case GPIO_PIN_RESET:
		HAL_GPIO_WritePin(SX1280_RST_GPIO_Port, SX1280_RST_Pin, (GPIO_PinState)value);
		break;
	case GPIO_PIN_BUSY:
		HAL_GPIO_WritePin(SX1280_BUSY_GPIO_Port, SX1280_BUSY_Pin, (GPIO_PinState)value);
		break;
	case GPIO_PIN_DIO1:
		break;
	case GPIO_PIN_DIO2:
		break;
	case GPIO_PIN_DIO3:
		break;
	default:
		break;
	}
}

void SX128x_OBJ::HalSpiTransfer(uint8_t *buffer_in, const uint8_t *buffer_out, uint16_t size)
{
	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)0);
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)buffer_out, buffer_in, size, 1);
	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)1);
}

void SX128x_OBJ::HalSpiTransferDelay(uint8_t *buffer_in, const uint8_t *buffer_out, uint16_t size)
{
	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)0);
	HAL_Delay(10);
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)buffer_out, buffer_in, size, 1);
	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)1);
}

#elif defined NUCLEO_L476
extern SPI_HandleTypeDef hspi2;

uint8_t SX128x_OBJ::HalGpioRead(GpioPinFunction_t func)
{
	switch (func)
	{
		case GPIO_PIN_RESET:
			return HAL_GPIO_ReadPin(SX1280_RST_GPIO_Port, SX1280_RST_Pin);
		case GPIO_PIN_BUSY:
			return HAL_GPIO_ReadPin(SX1280_BUSY_GPIO_Port, SX1280_BUSY_Pin);
		case GPIO_PIN_DIO1:
			return 0;
		case GPIO_PIN_DIO2:
			return 0;
		case GPIO_PIN_DIO3:
			return 0;
		default:
			return 0;
	}
}


void SX128x_OBJ::HalGpioWrite(GpioPinFunction_t func, register uint8_t value)
{
	switch (func)
	{
	case GPIO_PIN_RESET:
		HAL_GPIO_WritePin(SX1280_RST_GPIO_Port, SX1280_RST_Pin, (GPIO_PinState)value);
		break;
	case GPIO_PIN_BUSY:
		HAL_GPIO_WritePin(SX1280_BUSY_GPIO_Port, SX1280_BUSY_Pin, (GPIO_PinState)value);
		break;
	case GPIO_PIN_DIO1:
		break;
	case GPIO_PIN_DIO2:
		break;
	case GPIO_PIN_DIO3:
		break;
	default:
		break;
	}
}

void SX128x_OBJ::HalSpiTransfer(uint8_t *buffer_in, const uint8_t *buffer_out, uint16_t size)
{
//	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)0);
//	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)buffer_out, buffer_in, size, 1);
//	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)1);
	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)buffer_out, buffer_in, size, 10);
	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)GPIO_PIN_SET);
}

void SX128x_OBJ::HalSpiTransferDelay(uint8_t *buffer_in, const uint8_t *buffer_out, uint16_t size)
{
//	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)0);
//	HAL_Delay(10);
//	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)buffer_out, buffer_in, size, 1);
//	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)1);
	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)buffer_out, buffer_in, size, 10);
	HAL_GPIO_WritePin(SX1280_NSS_GPIO_Port, SX1280_NSS_Pin, (GPIO_PinState)GPIO_PIN_SET);
}

#endif
