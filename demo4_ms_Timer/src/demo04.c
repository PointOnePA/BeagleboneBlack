//$ ./a.out 1 100
#include <stdlib.h>
#include <stdio.h>		//perror
#include <unistd.h>		//NULL
#include <sys/types.h>
#include <signal.h>
#include <time.h>



timer_t timerid;
sigset_t mask;
struct itimerspec its;

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); } while (0)
sig_atomic_t mscount = 0;
timer_t timerid;
sigset_t mask;
struct itimerspec its;

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
}

//-----------------------------------------------------------------------------
// Creates a 1 millisecond timer to replace the HCS12 interrupt
//   MSTimerCreate() ----------------------------------------------------------
void MSTimerCreate()
{  struct sigevent sev;

   struct sigaction sa;

   sa.sa_flags = SA_SIGINFO;
   sa.sa_sigaction = MSTimerHandler;				// pointer to timer signal handler
   sigemptyset(&sa.sa_mask);
   if (sigaction(SIG, &sa, NULL) == -1)
   {	errExit("sigaction");						//TODO: error handler
   }

   sigemptyset(&mask);								// Block timer signal temporarily
   sigaddset(&mask, SIG);							// while setting up the timer
   if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
   {   errExit("sigprocmask");						//TODO: error handler
   }

   sev.sigev_notify = SIGEV_SIGNAL;
   sev.sigev_signo = SIG;							//SIG SIGRTMIN
   sev.sigev_value.sival_ptr = &timerid;
   if (timer_create(CLOCKID, &sev, &timerid) == -1)		// CLOCK_REALTIME Create the timer
   {	errExit("timer_create");
   }

   its.it_value.tv_sec = 0;
   its.it_value.tv_nsec = 1e6;							//nonzero it_value
   its.it_interval.tv_sec = its.it_value.tv_sec;
   its.it_interval.tv_nsec = its.it_value.tv_nsec;
   //flags=TIMER_ABSTIME  (absolute time of CLOCK_REALTIME, else relative time for interval timer)
   if (timer_settime(timerid, 0, &its, NULL) == -1)		//ARM the timer: Start
   {	errExit("timer_settime");
   }

   /* Unlock the timer signal, so that timer notification can be delivered */
   if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
   {   errExit("sigprocmask");
   }
}


//---------------------------------------------------
MSTimerDestroy()
{	if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
	{	errExit("sigprocmask");
	}
	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = 0;							//zero it_value
	its.it_interval = its.it_value;
	if (timer_settime(timerid, 0, &its, NULL) == -1)	//DISARM the timer: Stop
	{	errExit("timer_settime");
	}
	timer_delete(timerid);								//kill the timer
}


//----------------------------------------------------
int main(int argc, char *argv[])
{	int i,j,k=0,M=0;

   MSTimerCreate();
	   for(k=0;k<10;k++)
	   {   for(i=0;i<1000;i++)
		   {   for(j=0;j<94000;j++)
			   {	M=i+j;
			   }
		   }
		   printf("%d %d\n",k, mscount);
	   }
   MSTimerDestroy();
   exit(EXIT_SUCCESS);
}
