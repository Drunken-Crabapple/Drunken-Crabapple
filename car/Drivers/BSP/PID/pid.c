#include "./BSP/PID/pid.h"
#include "./BSP/ENCODER/encoder.h"

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
    pid->difout = 0.0f;
    pid->out = 0.0f;

    pid->error0 = 0.0f;     //当前误差
    pid->error1 = 0.0f;     //上一次误差
    pid->error2 = 0.0f;     //上两次误差
}

/*加载params参数到pid*/
void pid_set_params(PID_t *pid,const Pidparams_t *params)
{
    pid->kp = params->kp;
    pid->ki = params->ki;
    pid->kd = params->kd;

    pid->out_min = params->out_min;
    pid->out_max = params->out_max;

}

void pid_set_target(PID_t *pid,float target)
{
    pid->target = target;
}


/*加载params参数到pid，并重置pid控制器*/
void pid_init(PID_t *pid,const Pidparams_t *params)
{
    pid_set_params(pid,params);
    pid_reset(pid);
}

float pid_calculate(PID_t *pid,float current)
{
    float a = 0.8;
    pid->current = current;
    pid->error2 = pid->error1;
    pid->error1 = pid->error0;
    pid->error0 = pid->target - pid->current;

    pid->difout = (1 - a) * pid->kd * (pid->error0 - 2 * pid->error1 + pid->error2) + a * pid->difout;
    
    pid->out += pid->kp * (pid->error0 - pid->error1) + pid->ki * pid->error0 + pid->difout;
    
    pid->out = pid_limit(pid->out,pid->out_min,pid->out_max);
    return pid->out;
}
