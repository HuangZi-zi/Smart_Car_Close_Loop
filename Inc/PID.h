#ifndef __PID_H_
#define __PID_H_

#include "motor.h"

//控制器结构体
typedef struct {
	float u_k;
	float u_k1;
	float e_k1;
	float e_k;	
}controller_t;

void controller_init(controller_t* controller);
float controller_output(controller_t* controller,float e);
void close_loop_run(int position);
#endif
