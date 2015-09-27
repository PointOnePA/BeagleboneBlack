/*! \file pwm_cap.c
\brief Example: PWM output and CAP input.

This file contains an example on how to measure the frequency and duty
cycle of a pulse train with a eCAP module input. The program sets
another pin as eHRPWM output to generate a pulse width modulated signal
as source for the measurement. The output can be changed by some keys,
the frequency and duty cycle of the input is shown continously in the
terminal output.

Licence: GPLv3

Copyright 2014 by Thomas{ dOt ]Freiherr[ At ]gmx[ DoT }net


Compile by: `gcc -Wall -o pwm_cap pwm_cap.c -lpruio`

*/

//! Message for the compiler.
#define _GNU_SOURCE 1
#include <sys/select.h>	//POSIX.1-2001, FD_SET
#include <unistd.h>		//for STDOUT_FILENO
//#include <stdio.h>			//STDIN_FILENO
#include <stdio.h>
#include <termios.h>
#include <errno.h>
#include "pruio.h"
#include "pruio_pins.h"

//! The pin for PWM output.
#define P_FANA P9_14
#define P_FANB P9_16
//! The pin for CAP input.
#define P_IN P9_42
//#define P_IN P9_28


/* ----------------------------------------------------------------------------
 * ! \brief Wait for keystroke or timeout.
	\param mseconds Timeout value in milliseconds.
	\returns 0 if timeout, 1 if input available, -1 on error.
	Wait for a keystroke or timeout and return which of the events happened.
*/
int isleep(unsigned int mseconds)
{ fd_set set;
  struct timeval timeout;

  /* Initialize the file descriptor set. */
  FD_ZERO(&set);
  FD_SET( STDIN_FILENO , &set);

  /* Initialize the timeout data structure. */
  timeout.tv_sec = 0;
  timeout.tv_usec = mseconds * 1000;
  return TEMP_FAILURE_RETRY(select(FD_SETSIZE, &set, NULL, NULL, &timeout));
}


//-------------------------------! The main function.
int main(int argc, char **argv)
{   float_t f1 = 500; 		//      Variable for calculated frequency.
    float_t d1 = 0.9; 		//      Variable for calculated duty cycle.
    float_t f0 = 500;   //      The required frequency.
    float_t d0 = .9;    //      The required duty cycle.
    char buffer[256], *msg = buffer;

    pruIo *Io = pruio_new(PRUIO_DEF_ACTIVE, 1, 0x98, 0); //! create new driver structure

    do {
    if (Io->Errr)
    {	printf("initialisation failed (%s)\n", Io->Errr); break;
    }
    if (pruio_cap_config(Io, P_IN, 2.)) //         configure input pin
    {   printf("failed setting input @P_IN (%s)\n", Io->Errr); break;
    }

    if (pruio_pwm_setValue(Io, P_FANA, f0, d0))
    {   printf("failed setting output @P_FANA (%s)\n", Io->Errr); break;  //failed setting output @P_FANA (no pin control)
    }
    //pin config OK, transfer local settings to PRU and start
//  if (pruio_config(Io, 1, 0x1FE, 0,   4) )
    if( pruio_config(Io, 2, 0x1FE, 1e6, 0) )
    {   printf("config failed (%s)\n", Io->Errr); break;
    }else
	{	msg=pruio_rb_start(Io);
		if( msg )
		{	printf("RB_START Failed to start %s\n",msg);
		}
		printf("Samples: %d\n",    Io->Adc->Samples);
		printf("TimerVal: %d ns\n",Io->Adc->TimerVal);
	}



    struct termios oldt, newt; //             make terminal non-blocking
    int localerr=0;
    if( 0>tcgetattr( STDIN_FILENO, &oldt ) )
   	{	localerr = errno;
    	printf("tcgetattr failed, error = %d\n",localerr);
   	}
    newt = oldt;
    newt.c_lflag &= ~( ICANON );
    newt.c_cc[VMIN] = 0;
    newt.c_cc[VTIME] = 1;
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );

    int irun=1;
    while(irun)								// Terminate on ESC, else run endless loop
    {	if (1 == isleep(500)) 				// got a char
    	{	switch (getchar())					// evaluate keystroke
			{ 	case '0' : d0 = 0.0; break;
				case '1' : d0 = 0.1; break;
				case '2' : d0 = 0.2; break;
				case '3' : d0 = 0.3; break;
				case '4' : d0 = 0.4; break;
				case '5' : d0 = 0.5; break;
				case '6' : d0 = 0.6; break;
				case '7' : d0 = 0.7; break;
				case '8' : d0 = 0.8; break;
				case '9' : d0 = 0.9; break;
				case ',' : d0 = 1.0; break;
				case 'm' : f0 = (f0 > 5.5 ? f0 - 5. : .5); break;
				case 'p' : f0 = (f0 < 999995. ? f0 + 5. : 1000000.); break;
				case '*' : f0 = (f0 < 1000000 ? f0 * 2 : 1000000.); break;
				case '/' : f0 = (f0 > .5 ? f0 / 2 : .5); break;
				case '+' : f0 = 1000000; break;
				case '-' : f0 = .5; break;
				default: goto finish;			//anything other than above, quit
			};
			if (pruio_pwm_setValue(Io, P_FANA, f0, d0))	//   set new output
			{   printf("failed setting PWM output (%s)\n", Io->Errr); break;			//failed setting PWM output (no pin control)
			}
			printf("\n--> Frequency: %6.1f , Duty: %6.1f\n", f0, d0);   //  info
    	}

      if (pruio_cap_Value(Io, P_IN, &f1, &d1)) 		//    get current input
      {		printf("failed reading input @P_IN (%s)\n", Io->Errr); break;
      }
       printf("\r    CapFreq: %6.1f CapDuty: %6.1f Nrpm: %6.1f ADC0-3: %4X %4X %4X", f1, d1,f1*30, Io->Adc->Value[1],Io->Adc->Value[2],Io->Adc->Value[3]); // info
      fflush(STDIN_FILENO);
    } //end while



finish:
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt ); 		//         reset terminal
    printf("Exiting\n");
 } while (0);

	if (pruio_pwm_setValue(Io, P_FANA, 0, 1))	   //   set new output
	{   printf("failed setting PWM output (%s)\n", Io->Errr);
	}
    pruio_destroy(Io);       /* destroy driver structure */
	return 0;
}
