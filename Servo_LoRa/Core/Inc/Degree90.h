/*
 * Degree90.h
 *
 *  Created on: Dec 4, 2024
 *      Author: alexp
 */
#ifndef DEGREE90_H_
#define DEGREE90_H_

#include "stm32f4xx_hal.h"

typedef struct {
    ADC_HandleTypeDef* hadc1;
    ADC_HandleTypeDef* hadc2;
    TIM_HandleTypeDef* htim;
    UART_HandleTypeDef* huart;
} Degree90;

void Degree90_Init(Degree90* controller, ADC_HandleTypeDef* adc1, ADC_HandleTypeDef* adc2, TIM_HandleTypeDef* tim, UART_HandleTypeDef* uart);
void Degree90_ReadADCAndControlServo(Degree90* controller);

#endif /* DEGREE90_H_ */

