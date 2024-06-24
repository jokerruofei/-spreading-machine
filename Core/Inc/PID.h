/*
 * PID.h
 *
 *  Created on: May 19, 2024
 *      Author: Administrator
 */

#ifndef INC_PID_H_
#define INC_PID_H_
#include <stdint.h>
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float setpoint;
    float integral;
    float prevError;
    float output;
    float maxOutput;
    float minOutput;
} PID_TypeDef;

void PID_Init(PID_TypeDef *pid, float Kp, float Ki, float Kd, float setpoint, float maxOutput, float minOutput);
float PID_Compute(PID_TypeDef *pid, float input);

#endif /* INC_PID_H_ */
