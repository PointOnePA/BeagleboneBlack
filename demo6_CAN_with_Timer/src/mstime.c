#include <time.h>			//timer_t
#include <errno.h>			//errno
#include <stdio.h>			//printf
#include "mstime.h"
#include "ican.h"			//CAN_Get()

//global definition shared through include file
sig_atomic_t mscount = 0;

//local private data
static timer_t timerid;
static sigset_t mask;
static struct itimerspec its;

//-----------------------------------------------------------------------------
//This replaced the HSC12 interrupt_200us function for the purpose of ticking
//the millisecond time counter mstime.  Note that this variable is global and
//sig_atomic_t, so that it is updated in one clock cycle.  get_time64 used for
//the time in the logger function has been switched to an absolute time and
//no longer depends upon this tick.
//-----------------------------------------------------------------------------
static void MSTimerHandler(int sig, siginfo_t *si, void *uc)
{	static sig_atomic_t ticktock=0;
	++ticktock;
	mscount = ticktock;
	CAN_Get();
}

//-----------------------------------------------------------------------------
// Creates a 1 millisecond timer to replace the HCS12 interrupt
//   MSTimerCreate() ----------------------------------------------------------
int MSTimerCreate(void)
{	struct sigevent sev;							//structure for notification
	struct sigaction sa;							//examine, change signal action
	int errsv = errno;
	int SIG=SIGRTMIN;								//define the signal

	sa.sa_flags = SA_SIGINFO;						// using sa_sigaction instead of sa_handler
	sa.sa_sigaction = MSTimerHandler;				// pointer to timer signal handler
	sigemptyset(&sa.sa_mask);						// initialize mask as null
	if (sigaction(SIG, &sa, NULL) == -1)
	{	printf("MSTimerCreate:sigaction error#: %d\n",errsv);	//TODO: error handler
	}

   sigemptyset(&mask);								// Block timer signal temporarily
   sigaddset(&mask, SIG);							// while setting up the timer
   if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
	{	printf("MSTimerCreate:sigprocmask error#: %d\n",errsv);	//TODO: error handler
		return(-1);
	}

   sev.sigev_notify = SIGEV_SIGNAL;					// Notify by sending a signal
   sev.sigev_signo = SIG;							// SIG SIGRTMIN
   sev.sigev_value.sival_ptr = &timerid;
   if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)			// CLOCK_REALTIME Create the timer
	{	printf("MSTimerCreate:timer_create error#: %d\n",errsv);	//TODO: error handler
		return(-1);
	}


   its.it_value.tv_sec = 0;								// 1 millisecond = 1e6 nanosec
   its.it_value.tv_nsec = 1e6;							//nonzero it_value
   its.it_interval.tv_sec = its.it_value.tv_sec;
   its.it_interval.tv_nsec = its.it_value.tv_nsec;
   //flags=0 for relative time for interval timer, otherwise use TIMER_ABSTIME
   if (timer_settime(timerid, 0, &its, NULL) == -1)		//ARM the timer: Start
   {	printf("MSTImerCreate: timer_settime error# %d\n",errsv);
	    return(-1);
   }

   /* Unblock the timer signal, so that timer notification can be delivered */
   if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
   {	printf("MSTImerCreate: sigprocmask error# %d\n",errsv);
	    return(-1);
   }
return(0);
}


//---------------------------------------------------
void MSTimerDestroy(void)
{	int errsv = errno;

	if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
	{	printf("MSTimerDestroy:sigprocmask error#: %d\n",errsv);
	}
	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = 0;							//zero it_value
	its.it_interval = its.it_value;
	if (timer_settime(timerid, 0, &its, NULL) == -1)	//DISARM the timer: Stop
	{	printf("MSTimerDestroy:timer_settime error#: %d\n",errsv);
	}
	timer_delete(timerid);								//kill the timer
}
