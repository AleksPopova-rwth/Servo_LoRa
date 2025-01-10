/*
 * Degree5.h
 *
 *  Created on: Dec 4, 2024
 *      Author: alexp
 */

#ifndef DEGREE5_H
#define DEGREE5_H

#include "stm32f4xx_hal.h"


typedef struct {
    ADC_HandleTypeDef* hadc1;     // Pointer to ADC1 handle
    ADC_HandleTypeDef* hadc2;     // Pointer to ADC2 handle
    TIM_HandleTypeDef* htim;      // Pointer to Timer handle
    UART_HandleTypeDef* huart;    // Pointer to UART handle
    uint16_t current_pwm;         // Current PWM value for servo position
} Degree5;

/**
 * @brief Initialize the Degree5 controller.
 * @param controller Pointer to the Degree5 structure.
 * @param adc1 Pointer to the ADC1 handle.
 * @param adc2 Pointer to the ADC2 handle.
 * @param tim Pointer to the Timer handle.
 * @param uart Pointer to the UART handle.
 */
void Degree5_Init(Degree5* controller, ADC_HandleTypeDef* adc1, TIM_HandleTypeDef* tim, UART_HandleTypeDef* uart);

/**
 * @brief Gradually set the servo position to the target PWM value, checking ADC values every 20 steps.
 * @param controller Pointer to the Degree5 structure.
 * @param target_pwm Target PWM value for the servo.
 */
void Degree5_SetServoPosition(Degree5* controller, uint16_t target_pwm);

/**
 * @brief Read ADC values and control the servo based on the values.
 * @param controller Pointer to the Degree5 structure.
 */
void Degree5_ReadADCAndControlServo(Degree5* controller);

/**
 * @brief Read a single value from the specified ADC.
 * @param adc Pointer to the ADC handle.
 * @return The raw ADC value.
 */
uint32_t Degree5_ReadADC(ADC_HandleTypeDef* adc);
void Degree5_SetServoAngle(Degree5* controller, uint8_t angle);

#endif // DEGREE5_H
