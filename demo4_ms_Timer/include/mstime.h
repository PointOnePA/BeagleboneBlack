#if !defined(MSTIME_H)
#define MSTIME_H

#include <signal.h>
#include <time.h>


extern sig_atomic_t mscount;
timer_t timerid;
sigset_t mask;
struct itimerspec its;

void MSTimerCreate();
void MSTimerDestroy();

#endif
