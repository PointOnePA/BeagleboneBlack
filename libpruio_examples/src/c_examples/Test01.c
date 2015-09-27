#define _GNU_SOURCE 1			//! Message for the compiler.

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include "../c_wrapper/pruio.h"

#include "thermistor.h"

/* ----------------------------------------------------------------------------
 * ! \brief Wait for keystroke or timeout.
\param mseconds Timeout value in milliseconds.
\returns 0 if timeout, 1 if input available, -1 on error.
Wait for a keystroke or timeout and return which of the events happened.
*/
int isleep(unsigned int mseconds)
{
  fd_set set;
  struct timeval timeout;

  /* Initialize the file descriptor set. */
  FD_ZERO(&set);
  FD_SET(STDIN_FILENO, &set);
  /* Initialize the timeout data structure. */
  timeout.tv_sec = 0;
  timeout.tv_usec = mseconds * 1000;
  return TEMP_FAILURE_RETRY(select(FD_SETSIZE, &set, NULL, NULL, &timeout));
}

double CtoF(double c){	  return (c * 9.0 / 5.0) + 32.0;	}


//! ---------------------------------------------------------The main function.
int main(int argc, char **argv)
{	double TdegC0=0.0, TdegF0=0.0, TdegC1=0.0, TdegF1=0.0;
	int counts0=0,counts1=0;


	//STEP1: Create Pruio struct     Act,Av,  opD, SaD
	pruIo *io = pruio_new(PRUIO_DEF_ACTIVE, 1, 0x98, 0);    //! create new driver structure

  do { //pseudo loop to avoid goto when "break" conditions executed
		if (io->Errr){   printf("initialisation failed (%s)\n", io->Errr); break;		}

		//STEP2: Upload configuration to subsystem registers and start operation
		if (pruio_config(io, 1, 0x1FE, 0, 4)){   printf("config failed (%s)\n", io->Errr); break;	}

		//setup terminal to stop the problem on keyboard entry
			struct termios oldt, newt; //             make terminal non-blocking
			tcgetattr( STDIN_FILENO, &oldt );
			newt = oldt;
			newt.c_lflag &= ~( ICANON | ECHO );
			newt.c_cc[VMIN] = 0;
			newt.c_cc[VTIME] = 0;
			tcsetattr(STDIN_FILENO, TCSANOW, &newt);

		while(!isleep(400)) 						//run loop until keystroke
		{	counts0 = (io->Adc->Value[1])/16;		//change 16bits to 12-bit 0-4095
			TdegC0 = temperature(counts0);			//convert counts to degC
			TdegF0 = CtoF(TdegC0);					//conversion to degF
			counts1 = (io->Adc->Value[2])/16;		//change 16bits to 12-bit 0-4095
			TdegC1 = temperature(counts1);			//convert counts to degC
			TdegF1 = CtoF(TdegC1);					//conversion to degF
			printf("#0%5d degC0:%5.2f degF0:%5.2f   #1%5d degC1:%5.2f degF1:%5.2f\n",counts0, TdegC0, TdegF0, counts1, TdegC1, TdegF1);
			fflush(STDIN_FILENO);
		}

		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);	//reset terminal back the way it was
		printf("\n");
	}while (0);

	//STEP3: resore original configuration and destroy the Pruio sturcture
	pruio_destroy(io);								// destroy driver structure
	return 0;
}



