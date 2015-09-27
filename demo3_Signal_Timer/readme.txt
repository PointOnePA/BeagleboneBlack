Interval Timer Functions
Real-time applications often schedule actions using interval timers. 
Interval timers can be either of two types: a one-shot type or a periodic type.
A one-shot is an armed timer that is set to an expiration time relative to 
either current time or an absolute time. The timer expires once and is disarmed. 
Such a timer is useful for clearing buffers after the data has been transferred 
to storage, or to time-out an operation.

A periodic timer is armed with an initial expiration time (either absolute or relative) 
and a repetition interval. Each time the interval timer expires it is reloaded with 
the repetition interval and rearmed. This timer is useful for data logging or for 
servo-control. In calls to interval timer functions, time values smaller than the 
resolution of the system hardware periodic timer are rounded up to the next multiple 
of the hardware timer interval (typically 10 ms).

There are two sets of timer interfaces in SunOS 5.0 through 5.8. The setitimer(2) 
and getitimer(2) interfaces operate fixed set timers, called the BSD timers, 
using the timeval structure to specify time intervals. The POSIX timers, 
timer_create(3RT), operate the POSIX clock, CLOCK_REALTIME. POSIX timer operations 
are expressed in terms of the timespec structure.

The functions getitimer(2) and setitimer(2) retrieve and establish, respectively, 
the value of the specified BSD interval timer. There are three BSD interval timers 
available to a process, including a real-time timer designated ITIMER_REAL. If a 
BSD timer is armed and allowed to expire, the system sends a signal appropriate 
to the timer to the process that set the timer.

timer_create(3RT) can create up to TIMER_MAX POSIX timers. The caller can specify 
what signal and what associated value are sent to the process when the timer expires. 
timer_settime(3RT) and timer_gettime(3RT) retrieve and establish respectively the 
value of the specified POSIX interval timer. Expirations of POSIX timers while the 
required signal is pending delivery are counted, and timer_getoverrun(3RT) 
retrieves the count of such expirations. timer_delete(3RT) deallocates a POSIX timer.

Example 8-1 illustrates how to use setitimer(2) to generate a periodic interrupt, 
and how to control the arrival of timer interrupts.
