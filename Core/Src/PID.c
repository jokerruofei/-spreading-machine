/*
 * PID.C
 *
 *  Created on: May 19, 2024
 *      Author: Administrator
 */
#include "PID.h"
#include <stdint.h>

void PID_Init(PID_TypeDef *pid, float Kp, float Ki, float Kd, float setpoint, float maxOutput, float minOutput) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->setpoint = setpoint;
    pid->integral = 0.0f;
    pid->prevError = 0.0f;
    pid->output = 0.0f;
    pid->maxOutput = maxOutput;
    pid->minOutput = minOutput;
}

float PID_Compute(PID_TypeDef *pid, float input) {
    float error = pid->setpoint - input;
    pid->integral += error;
    float derivative = error - pid->prevError;
    pid->output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;

    if (pid->output > pid->maxOutput) {
        pid->output = pid->maxOutput;
    } else if (pid->output < pid->minOutput) {
        pid->output = pid->minOutput;
    }

    pid->prevError = error;
    return pid->output;
}
