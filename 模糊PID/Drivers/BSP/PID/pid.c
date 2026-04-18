#include "./BSP/PID/pid.h"

/*参数限幅*/
static float pid_limit(float value,float min,float max)
{
    if(value > max) return max;
    if(value < min) return min;
    return value;
}

/*清空pid控制器状态*/
void pid_reset(PID_t *pid)
{
    pid->target = 0.0f;
    pid->current = 0.0f;
    pid->out = 0.0f;

    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
}

/*加载params参数到pid*/
void pid_set_params(PID_t *pid,const Pidparams_t *params)
{
    pid->kp = params->kp;
    pid->ki = params->ki;
    pid->kd = params->kd;

    pid->out_min = params->out_min;
    pid->out_max = params->out_max;

    pid->integral_min = params->integral_min;
    pid->integral_max = params->integral_max;
}

/*加载params参数到pid，并重置pid控制器*/
void pid_init(PID_t *pid,const Pidparams_t *params)
{
    pid_set_params(pid,params);
    pid_reset(pid);
}

void pid_set_target(PID_t *pid,float target)
{
    pid->target = target;
}

float pid_calculate(PID_t *pid,float current)
{
    pid->current = current;
    pid->error = pid->target - pid->current;

    pid->integral = pid->integral + pid->error;
    pid->integral = pid_limit(pid->integral,pid->integral_min,pid->integral_max);

    pid->out = pid->kp * pid->error
             + pid->ki * pid->integral
             + pid->kd * (pid->error - pid->last_error);
    pid->out = pid_limit(pid->out,pid->out_min,pid->out_max);

    pid->last_error = pid->error;
    return pid->out;
}
