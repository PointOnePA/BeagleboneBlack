#ifndef MSTIME_H
#define MSTIME_H

#include <signal.h>

extern sig_atomic_t mscount;	//global, one step processing

int MSTimerCreate(void );
void MSTimerDestroy(void);

#endif
