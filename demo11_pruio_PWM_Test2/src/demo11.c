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
//#define P_IN P9_28		//failed reading input @P_IN (pion not in CAP mode)


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
{   float_t f1 = 500.0F; 			//      Variable for calculated frequency.
    float_t d1 = 0.9F; 				//      Variable for calculated duty cycle.
    float_t f0 = 500.0F;   			//      The required frequency.
    float_t d0 = .9F;    			//      The required duty cycle.
    float_t MINFREQ = 2.0F;         //
    char buffer[256], *msg = buffer;
    pruIo *Io;
    int charX=0;

    Io = pruio_new(PRUIO_DEF_ACTIVE, 16, 0x98, 0); 		//pruIo* pruio_new(uint16 Act, uint8 Av, uint32 OpD, uint8 SaD);
    pruio_destroy(Io);       							//destroy driver structure
    Io = pruio_new(PRUIO_DEF_ACTIVE, 16, 0x98, 0); 		//pruIo* pruio_new(uint16 Act, uint8 Av, uint32 OpD, uint8 SaD);

    //output GpioUdt::setValue PwmMod::setValue
    //input  GpioUdt::config   CapMod::config

    msg= pruio_pwm_setValue(Io, P_FANA, f0, d0);
    if(msg)printf("1%s\n",msg);	//char* pruio_pwm_setValue(pruIo* Io, uint8 Ball, float_t Hz, float_t Du);
    msg= pruio_pwm_setValue(Io, P_FANB, f0, d0);
    if(msg)printf("2%s\n",msg);	//char* pruio_pwm_setValue(pruIo* Io, uint8 Ball, float_t Hz, float_t Du);
    msg= pruio_config(Io, 1, 0x1FE, 1e6, 0);
    if(msg)printf("3%s\n",msg);	//char* pruio_config(pruIo* Io, uint32 Samp, uint32 Mask, uint32 Tmr, uint16 Mds);
    msg= pruio_cap_config(Io, (uint8)P_IN, MINFREQ);			//NO NON-FLOAT with decimal, get interpreted at double
    if(msg)printf("4%s\n",msg);	//char* pruio_cap_config(pruIo* Io, uint8 Ball, float_t FLow);


// not using ring buffer mode. couldn't get 2 PWM outputs to work with SAMP>1
// So TMR value is ignored
//	msg= pruio_rb_start(Io);						if(msg)printf("5%s\n",msg);	//char* pruio_rb_start(pruIo* Io);

	printf("Samples: %d\n",    Io->Adc->Samples);
	printf("TimerVal: %d ns\n",Io->Adc->TimerVal);
//	printf("Dram[0]= %d\n",Io->DRam[0] );			//most recent write position

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
    {	if (1 == isleep(1000)) 				// got a char
    	{	charX = getchar();
    		switch (charX)					// evaluate keystroke
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
				default: {irun = 0; break;}				//anything other than above, quit
			};

    		if(irun)
    		{	msg = pruio_pwm_setValue(Io, P_FANA, f0, d0);
				if(msg)
				{	printf("6failed setting PWMA output (%s)\n", Io->Errr); irun=0;
				}else
				{	printf("6\n");
				}
				msg = pruio_pwm_setValue(Io, P_FANB, f0, d0);
				if(msg)
				{	printf("7failed setting PWMB output (%s)\n", Io->Errr); irun=0;
				}else
				{	printf("7\n");
				}
    		}
    	}

    	if(irun)
    	{	  if (pruio_cap_Value(Io, P_IN, &f1, &d1)) 		//    get current input
			  {		printf("8failed reading input @P_IN=%d (%s)\n", P_IN, Io->Errr); break;
			  }
			   printf("Freq %6.1f Duty: %6.1f Freq: %6.1f Duty: %6.1f ADC: %4d %4d %4d\n", f0, d0, f1, d1,Io->Adc->Value[1],Io->Adc->Value[2],Io->Adc->Value[3]); // info
			  fflush(STDIN_FILENO);
    	}
    } //end while


    tcsetattr( STDIN_FILENO, TCSANOW, &oldt ); 		//         reset terminal
    printf("Exiting\n");

    if (pruio_pwm_setValue(Io, P_FANA, f0, 1))	   //   set new output
	{   printf("failed setting PWMA output (%s)\n", Io->Errr);
	}
    if (pruio_pwm_setValue(Io, P_FANB, f0, 1))	   //   set new output
	{   printf("failed setting PWMB output (%s)\n", Io->Errr);
	}

    pruio_destroy(Io);       					//destroy driver structure
	return 0;
}
