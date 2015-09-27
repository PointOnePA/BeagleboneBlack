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
uint8 P_FANA = P9_14;
uint8 P_FANB = P9_16;

//-------------------------------! The main function.
int main(int argc, char **argv)
{	char *ret;
	uint32 Samp, Mask, Tmr;
	uint16 Mds;
    pruIo *Io = pruio_new(PRUIO_DEF_ACTIVE, 0x98, 0, 1); 				// create new driver structure

    if (Io->Errr)
    {	printf("initialisation failed (%s)\n", Io->Errr);
    }

    //pin config OK, transfer local settings to PRU and start
    //char* pruio_config(pruIo* Io, uint32 Samp, uint32 Mask, uint32 Tmr, uint16 Mds);
    Samp = 1;
    Mask = 0x1FE;
    Tmr = 0;
    Mds = 4;
    ret = pruio_config(Io, Samp, Mask, Tmr, Mds);
    if (ret)	//pruio_config(Io, 1, 0x1FE, 0, 4))
    {   printf("config failed (%s)\n", Io->Errr);
    }
    /*=============================================
//  char* pruio_gpio_setValue(pruIo* Io, uint8 Ball, uint8 Modus);
    ret = pruio_gpio_setValue(Io, P_FANA, 1);	//
	if (ret)	   // set HIGH to turn off fan
	{   printf("failed setting PWMA output (%s)\n", Io->Errr);
	}
    ret = pruio_gpio_setValue(Io, P_FANB, 1);
	if (ret)	   // set HIGH to turn off fan
	{   printf("failed setting PWMB output (%s)\n", Io->Errr);
	}
============================================*/

    ret = pruio_gpio_setValue(Io, P_FANA, 1);	//
	if (ret)	   // set HIGH to turn off fan
	{   printf("failed setting PWMA output (%s)\n", Io->Errr);
	}
    ret = pruio_gpio_setValue(Io, P_FANB, 1);
	if (ret)	   // set HIGH to turn off fan
	{   printf("failed setting PWMB output (%s)\n", Io->Errr);
	}

	sleep(5);
    pruio_destroy(Io);       /* destroy driver structure */
	return 0;
}
