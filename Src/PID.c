#include "PID.h"

void controller_init(controller_t* controller)
{
	controller->u_k	=0.0;
	controller->u_k1 =0.0;
	controller->e_k1 =0.0;
	controller->e_k  =0.0;
}

float controller_output(controller_t* controller,float e)
{
	controller->u_k1=controller->u_k;
	controller->e_k1=controller->e_k;
	controller->e_k=e;
	controller->u_k=(1/1.870)*(-0.130*controller->u_k1 + 12.586*controller->e_k - 10.586*controller->e_k1);
	return controller->u_k;
}

void close_loop_run(int position)
{
	int r;
	r=position-180;
	//r>0, срв╙
	//r<0ё╛вСв╙
	
	
	
}
