/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

// TO DO: (Version Plan)
// 1) The step function can be any type, no need always be "LL_Thread__step_func".
// 2) Besides the loop of whole thread, parts (some steps) of the thread can also be looped.

#include "LL_Platform.h"
#include "LL_Common.h"



void LL_Thread__start( T_LL_Thread__var *p_var, const LL_Thread__step_func *steps, unsigned long loop_countdown) { p_var->step_now = 1; p_var->timer = gulTimerCnt1ms;
    p_var->p_steps        = steps;
    p_var->loop_countdown = loop_countdown;
}

void LL_Thread__stop(  T_LL_Thread__var *p_var) { p_var->step_now = 0; }



void LL_Thread(T_LL_Thread__var *p_var) { if(0 == p_var->step_now) return; unsigned long step_index = p_var->step_now - 1;
    LL_Thread__step_func step_func = p_var->p_steps[step_index];
    if(LL_Thread__step__DELAY_ms == step_func) { unsigned long ms = (unsigned long)(p_var->p_steps[step_index + 1]);
        if(ms <= LL_Timer_Elapsed_ms(p_var->timer)) { // if timeout (then can go to next step)
            p_var->step_now += 2; p_var->timer = gulTimerCnt1ms;
        } else { // else, still timing (then stay in this step)
            // nothing here
        }
    } else if(LL_Thread__step__START_A_SUB_THREAD == step_func) {
        {
            T_LL_Thread__var     *p_thread_var  = (T_LL_Thread__var     *)p_var->p_steps[step_index + 1];
            LL_Thread__step_func *steps         = (LL_Thread__step_func *)p_var->p_steps[step_index + 2];
            unsigned long        loop_countdown = (unsigned long         )p_var->p_steps[step_index + 3];
            LL_Thread__start(p_thread_var, steps, loop_countdown);
        }
        p_var->step_now += 4; p_var->timer = gulTimerCnt1ms;        
    } else if(LL_Thread__step__WAIT_FOR_A_SUB_THREAD_DONE == step_func) {
        T_LL_Thread__var *p_thread_var = (T_LL_Thread__var *)p_var->p_steps[step_index + 1];
        if(LL_Thread__is_stopped(p_thread_var)) { // if that thread is stopped (done)
            p_var->step_now += 2; p_var->timer = gulTimerCnt1ms;
        } else {
            // nothing, keep waiting
        }
    } else if(LL_Thread__step__UPDATE_THE_CURRENT_THREAD == step_func) {
        {
            LL_Thread__step_func *steps         = (LL_Thread__step_func *)p_var->p_steps[step_index + 1];
            unsigned long        loop_countdown = (unsigned long         )p_var->p_steps[step_index + 2];
            LL_Thread__start(p_var, steps, loop_countdown);
        }
    } else if(LL_Thread__step__END_OR_LOOP_THIS_THREAD == step_func) { 
        if(p_var->loop_countdown == LL_Thread__LOOP_FOREVER) {
            p_var->step_now = 1; p_var->timer = gulTimerCnt1ms;
        } else if(p_var->loop_countdown > 0) { p_var->loop_countdown--;
            p_var->step_now = 1; p_var->timer = gulTimerCnt1ms;//LL_Thread__start(p_var);
        } else {
            LL_Thread__stop(p_var);            
        }
//    } else if(LL_Thread__step__STOP_THIS_THREAD == step_func) { 
//        LL_Thread__stop(p_var);
    } else { // else, must be a real function
        unsigned long return_value = step_func();
        if(LL_Thread__step_RETURN__Done == return_value) { // if can go to next step (then just go)
            p_var->step_now += 1; p_var->timer = gulTimerCnt1ms;
        } else { // else, must be "LL_Thread__step_RETURN__NotDone" or "LL_Thread__step_RETURN__Done_andThisThreadHasBeenRestartedOrUpdated" or other exception value (then just stay in this step)
            // nothing here
        }
    }
}


