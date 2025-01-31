/*
 * Degree5.c
 *
 *  Created on: Dec 4, 2024
 *      Author: alexp
 */
#include "Degree5.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>

jmp_buf env;

void Degree5_Init(Degree5* controller, ADC_HandleTypeDef* adc1, TIM_HandleTypeDef* tim, UART_HandleTypeDef* uart) {
    controller->hadc1 = adc1;
    controller->htim = tim;
    controller->huart = uart;
    controller->current_pwm = 750; // Initial position
}

void Degree5_ReadADC(Degree5* controller, uint32_t* raw1, uint32_t* raw2) {
    ADC_ChannelConfTypeDef sConfig = {0};

    // Lesen von Kanal 0
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1; // Erster Kanal
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(controller->hadc1, &sConfig);

    HAL_ADC_Start(controller->hadc1);
    if (HAL_ADC_PollForConversion(controller->hadc1, HAL_MAX_DELAY) == HAL_OK) {
        *raw1 = HAL_ADC_GetValue(controller->hadc1);
    }

    // Lesen von Kanal 1
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = 1; // Wieder erster Kanal
    HAL_ADC_ConfigChannel(controller->hadc1, &sConfig);

    HAL_ADC_Start(controller->hadc1);
    if (HAL_ADC_PollForConversion(controller->hadc1, HAL_MAX_DELAY) == HAL_OK) {
        *raw2 = HAL_ADC_GetValue(controller->hadc1);
    }
}

void Degree5_SetServoPosition(Degree5* controller, uint16_t target_pwm) {
    uint32_t step_count = 0;
    const uint16_t tolerance = 20; // Toleranz für ADC-Werte

    if (controller->current_pwm < target_pwm) {
        while (controller->current_pwm < target_pwm) {
            controller->current_pwm++;
            controller->htim->Instance->CCR1 = controller->current_pwm;
            HAL_Delay(1); // Geschwindigkeit des Übergangs anpassen
            step_count++;

            // ADC-Werte nach 10 Schritten prüfen
            if (step_count >= 10) {
                uint32_t raw1, raw2;
                Degree5_ReadADC(controller, &raw1, &raw2);

                if ((raw1 > 2048 && raw2 <= 2048) || (raw2 > 2048 && raw1 <= 2048)) {
                    break;
                } else if (abs((int32_t)(raw1 - raw2)) <= tolerance) {
                    break;
                }
                step_count = 0; // Schrittzähler zurücksetzen
            }
        }
    } else if (controller->current_pwm > target_pwm) {
        while (controller->current_pwm > target_pwm) {
            controller->current_pwm--;
            controller->htim->Instance->CCR1 = controller->current_pwm;
            HAL_Delay(1); // Geschwindigkeit des Übergangs anpassen
            step_count++;

            // ADC-Werte nach 10 Schritten prüfen
            if (step_count >= 10) {
                uint32_t raw1, raw2;
                Degree5_ReadADC(controller, &raw1, &raw2);

                if ((raw1 > 2048 && raw2 <= 2048) || (raw2 > 2048 && raw1 <= 2048)) {
                    break;
                } else if (abs((int32_t)(raw1 - raw2)) <= tolerance) {
                    break;
                }
                step_count = 0; // Schrittzähler zurücksetzen
            }
        }
    }
}

void Degree5_ReadADCAndControlServo(Degree5* controller) {
    uint32_t raw1 = 0, raw2 = 0;
    char msg[50];
    const uint16_t tolerance = 20;

    Degree5_ReadADC(controller, &raw1, &raw2);

    // Debug-Nachricht für ADC1
    sprintf(msg, "ADC1: %lu\r\n", raw1);
    HAL_UART_Transmit(controller->huart, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, (raw1 > 2048) ? GPIO_PIN_RESET : GPIO_PIN_SET);

    HAL_Delay(100);

    // Debug-Nachricht für ADC2
    sprintf(msg, "ADC2: %lu\r\n", raw2);
    HAL_UART_Transmit(controller->huart, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, (raw2 > 2048) ? GPIO_PIN_RESET : GPIO_PIN_SET);

    HAL_Delay(100);

    uint16_t target_pwm = 750;
    if (raw1 > raw2) {
        target_pwm = 250;
    } else if (raw1 < raw2) {
        target_pwm = 1250;
    }

    if (abs((int32_t)(raw1 - raw2)) >= tolerance) {
        Degree5_SetServoPosition(controller, target_pwm);
    }
    HAL_Delay(100);
}

void Degree5_SetServoAngle(Degree5* controller, uint8_t angle) {
    if (angle > 180) {
        angle = 180;
    }

    uint16_t pwm_min = 250;
    uint16_t pwm_max = 1250;
    uint16_t target_pwm = pwm_min + (angle * (pwm_max - pwm_min) / 180);

    char msg[50];
    sprintf(msg, "Winkel: %u Grad, PWM: %u\r\n", angle, target_pwm);
    HAL_UART_Transmit(controller->huart, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    if (controller->current_pwm < target_pwm) {
            while (controller->current_pwm < target_pwm) {
                controller->current_pwm++;
                controller->htim->Instance->CCR1 = controller->current_pwm;
                HAL_Delay(10); // Geschwindigkeit des Übergangs anpassen
            }
        } else if (controller->current_pwm > target_pwm) {
            while (controller->current_pwm > target_pwm) {
                controller->current_pwm--;
                controller->htim->Instance->CCR1 = controller->current_pwm;
                HAL_Delay(10); // Geschwindigkeit des Übergangs anpassen
            }
        }
    }
    //Degree5_SetServoPosition_withRadio(controller, target_pwm);

