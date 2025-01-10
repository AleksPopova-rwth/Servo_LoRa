/*
 * Degree90.c
 *
 *  Created on: Dec 4, 2024
 *      Author: alexp
 */
#include "Degree90.h"
#include <stdio.h>
#include <string.h>

void Degree90_Init(Degree90* controller, ADC_HandleTypeDef* adc1, ADC_HandleTypeDef* adc2, TIM_HandleTypeDef* tim, UART_HandleTypeDef* uart) {
    controller->hadc1 = adc1;
    controller->hadc2 = adc2;
    controller->htim = tim;
    controller->huart = uart;
}

void Degree90_ReadADCAndControlServo(Degree90* controller) {
    uint32_t raw1 = 0, raw2 = 0;
    char msg[50];

    // Read from ADC1
    HAL_ADC_Start(controller->hadc1);
    if (HAL_ADC_PollForConversion(controller->hadc1, HAL_MAX_DELAY) == HAL_OK) {
        raw1 = HAL_ADC_GetValue(controller->hadc1);
        sprintf(msg, "ADC1: %lu\r\n", raw1);
        HAL_UART_Transmit(controller->huart, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, (raw1 > 2048) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }

    HAL_Delay(100);

    // Read from ADC2
    HAL_ADC_Start(controller->hadc2);
    if (HAL_ADC_PollForConversion(controller->hadc2, HAL_MAX_DELAY) == HAL_OK) {
        raw2 = HAL_ADC_GetValue(controller->hadc2);
        sprintf(msg, "ADC2: %lu\r\n", raw2);
        HAL_UART_Transmit(controller->huart, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, (raw2 > 2048) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }

    HAL_Delay(100);

    // Control servo based on ADC values
    if (raw1 > raw2) {
        controller->htim->Instance->CCR1 = 250;
    } else if (raw1 < raw2) {
        controller->htim->Instance->CCR1 = 1250;
    } else {
        controller->htim->Instance->CCR1 = 750;
    }

    HAL_Delay(1000);
}
