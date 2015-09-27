#include "unistd.h"					// for sleep
#include "stdio.h"
#include "../c_wrapper/pruio.h" 	// include header

#include <signal.h>
#include <time.h>			//timer_t, timer_create is extern
#include <syscall.h>		//timer_create
#include <errno.h>			//errno


//global definition shared through include file
sig_atomic_t mscount = 0;

//local private data
static timer_t timerid;
static sigset_t mask;
static struct itimerspec its;
static int data[1201][9];
pruIo *io; 	//! create new driver structure

//-----------------------------------------------------------------------------
//This replaced the HSC12 interrupt_200us function for the purpose of ticking
//the millisecond time counter mstime.  Note that this variable is global and
//sig_atomic_t, so that it is updated in one clock cycle.  get_time64 used for
//the time in the logger function has been switched to an absolute time and
//no longer depends upon this tick.
//-----------------------------------------------------------------------------
static void MSTimerHandler(int sig, siginfo_t *si, void *uc)
{	static sig_atomic_t ticktock=0;
	int i,j;

	++ticktock;
	mscount = ticktock;
	if(mscount < 1201)
	{	   data[mscount][8] = io->DRam[0];
		j=(data[mscount][8] < 8)?0:8;
		for(i=0;i<8;i++)                               //0,1, 2, 3, 4, 5, 6, 7
		{	data[mscount][i] = io->Adc->Value[i+j];	  // 8,9,10,11,12,13,14,15
		}
	}
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



//----------------------------------------------------
int main(int argc, char *argv[])
{	int i,j,k,m,n,M=0;
	char buffer[256], *msg = buffer;

	                     // OpD units are steps 0 to 0x3FFFF. SaD units re stops 0 to 255
                         //  Act  , avg, OpD, SaD;   //open delay is settling time. sample delay is integration time.
	io = pruio_new(PRUIO_DEF_ACTIVE, 2, 0x98, 0); 	//! create new driver structure
                      //Samp=1 = IO_mode, immediate start
	                  //Samp>1 - RB_mode, Pruio::rb_start();
	                     //Mask=1-1111-1110 for AIN7....AIN0, charge
								   //Tmr=0 except for MMmode with Samp>1
	               //Io, Samp, Mask, Tmr, Modus 0for12bits
	if (pruio_config(io, 2, 0x1FE, 1e6, 0)) 					// upload (default) settings, start IO mode
	{	printf("config failed (%s)\n", io->Errr);
	}else
	{	printf("Creating Timer\n");
		if(0>MSTimerCreate())
		{	printf("Timer Creation Failed\n");
		}

		msg=pruio_rb_start(io);
		if( msg )
		{	printf("RB_START Failed to start %s\n",msg);
		}

		printf("Samples: %d\n", io->Adc->Samples);
		printf("TimerVal: %d ns\n",io->Adc->TimerVal);
		printf("Sleep 10 seconds\n");
		   for(k=0;k<10;k++)
		   {   for(i=0;i<1000;i++)
			   {   for(j=0;j<94000;j++)
				   {	M=i+j;
				   }
			   }
			   printf("%d %d\n",k, mscount);	//global to avoid function call
		   }
		printf("Dram[0] = %d\n",io->DRam[0]);

		// now current ADC samples are available for AIN-0 to AIN-7 in array io->Adc->Value[]
		printf("Samples: %d\n", io->Adc->Samples);
		printf("TimerVal: %d ns\n",io->Adc->TimerVal);

		printf("Destroying timer %d\n",M);
		MSTimerDestroy();
		pruio_destroy(io);							// we're done, destroy driver structure
	}


	//Data dump
	for(n=0; n<1201; n++)
	{	printf("%d ",n);
			for(m=0;m<9;m++)
			{	printf("%4d ",data[n][m]);
			}
		printf("\n");
	}


return(0);
}
