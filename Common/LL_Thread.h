/*******************************************************************************
 Copyright (c) 2016 Lumen Labs (HK) Limited. All Rights Reserved.
*******************************************************************************/

#ifndef _LL_Thread_H
#define _LL_Thread_H



// Implement your own step function: (return Done or Not)
        #define LL_Thread__step_RETURN__NotDone                                         0
        #define LL_Thread__step_RETURN__Done                                            1
      //#define LL_Thread__step_RETURN__Done_andThisThreadHasBeenRestartedOrUpdated     2
typedef unsigned long (*LL_Thread__step_func)(void);



// Some common steps you can use directly:
#define LL_Thread__step__DELAY_ms                   ((LL_Thread__step_func)0xFFFFFFFB)
#define LL_Thread__step__START_A_SUB_THREAD         ((LL_Thread__step_func)0xFFFFFFFC)
#define LL_Thread__step__WAIT_FOR_A_SUB_THREAD_DONE ((LL_Thread__step_func)0xFFFFFFFD)
#define LL_Thread__step__UPDATE_THE_CURRENT_THREAD  ((LL_Thread__step_func)0xFFFFFFFE)
#define LL_Thread__step__END_OR_LOOP_THIS_THREAD    ((LL_Thread__step_func)0xFFFFFFFF)
// Macro for better reading when using the "common steps" above:
#define LL_Thread__delay_ms(xxx)                                                      LL_Thread__step__DELAY_ms,                   (LL_Thread__step_func)(xxx)
#define LL_Thread__start_a_sub_thread(         p_thread_var, steps, loop_countdown)   LL_Thread__step__START_A_SUB_THREAD,         (LL_Thread__step_func)p_thread_var, (LL_Thread__step_func)steps, (LL_Thread__step_func)loop_countdown
#define LL_Thread__wait_for_a_sub_thread_done( p_thread_var                       )   LL_Thread__step__WAIT_FOR_A_SUB_THREAD_DONE, (LL_Thread__step_func)p_thread_var
#define LL_Thread__update_the_current_thread(                steps, loop_countdown)   LL_Thread__step__UPDATE_THE_CURRENT_THREAD,                                      (LL_Thread__step_func)steps, (LL_Thread__step_func)loop_countdown



// Example:
//
// Step 1) Define a val for a "thread":
//    T_LL_Thread__var thread_var__beep; // no need any init now, init when LL_Thread__start
//
// Step 2) Prepare the steps:
//    LL_Thread__step_func steps[] = {
//        step_func1,
//        LL_Thread__delay_ms(200),
//        step_func2,
//        LL_Thread__delay_ms(200),
//        LL_Thread__step__END_OR_LOOP_THIS_THREAD // can always loop here, then use "loop_countdown" when start a thread to control the times that you actually need
//    };
// 
// Step 3) Prepare the routine: (in main loop or other loop)
//    LL_Thread(&thread_var__beep);
//
// Step 4) Start the thread:
//    LL_Thread__start(&thread_var__beep, p_steps, loop_countdown);



typedef struct {
    unsigned long step_now; const LL_Thread__step_func *p_steps; unsigned long loop_countdown;
    unsigned long timer;
} T_LL_Thread__var;

#define LL_Thread__is_started(val)    ((val)->step_now != 0)
#define LL_Thread__is_stopped(val)    ((val)->step_now == 0)

#define LL_Thread__LOOP_FOREVER 0xFFFFFFFF  // can be used as the "loop_countdown" of "LL_Thread__start"
void LL_Thread__start( T_LL_Thread__var *p_var, const LL_Thread__step_func *steps, unsigned long loop_countdown);
void LL_Thread__stop(  T_LL_Thread__var *p_var);
void LL_Thread(        T_LL_Thread__var *p_var);



#endif
