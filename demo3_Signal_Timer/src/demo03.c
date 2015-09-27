/*
Example 8-1 Controlling Timer Interrupts
----------------------------------------------------------------------------*/
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>

#define TIMERCNT 20

int	 timercnt=0;
struct timeval alarmtimes[TIMERCNT];

//------------------------------------------------------ TIMER HANDLER/CALLBACK
void timerhandler (int sig, siginfo_t *siginfo, void *context)
{	gettimeofday (&alarmtimes[timercnt], NULL);
	timercnt++;
	printf ("%d ", timercnt);
}

//------------------------------------------------------ EXIT Record of times
printtimes ()
{	int	i;
	long sec, usec, sec0, usec0;
	double delta;

	printf ("\n");

	sec0 = alarmtimes[0].tv_sec;
	usec0 = alarmtimes[0].tv_usec;
	for (i = 1; i < TIMERCNT; i++)
	{
		sec = alarmtimes[i].tv_sec;
		usec = alarmtimes[i].tv_usec;
		delta  = (double)(sec-sec0);
		delta += (double)(usec-usec0)/100000.0/(double)i;
		printf("%ld.%6ld   %12.6f\n", sec,usec, delta);
	}
}

main()
{
	struct itimerval times;
	sigset_t sigset;
	int		i, ret;
	struct sigaction act;
	siginfo_t	si;

	/* block SIGALRM */
	sigemptyset (&sigset);
	sigaddset (&sigset, SIGALRM);
	sigprocmask (SIG_BLOCK, &sigset, NULL);

	/* set up handler for SIGALRM */
	act.sa_sigaction = &timerhandler;
	sigemptyset (&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction (SIGALRM, &act, NULL);


	/*  set up interval timer, start after 1 sec, then every 32 millisecond */
	times.it_value.tv_sec = 1;
	times.it_value.tv_usec = 0;
	times.it_interval.tv_sec = 0;
	times.it_interval.tv_usec = 32000;		//32.000
	ret = setitimer (ITIMER_REAL, &times, NULL);
	printf ("timerhandler:start\n");
	//	printf ("main:setitimer ret = %d\n", ret);

	/* now wait for the alarms */
	sigemptyset (&sigset);
	while (timercnt < TIMERCNT)
	{	ret = sigsuspend (&sigset);
	}

	printtimes();
}
