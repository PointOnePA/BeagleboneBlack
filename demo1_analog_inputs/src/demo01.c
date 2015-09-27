/*
 * demo01.c
 *
 *  Created on: Nov 4, 2014
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "demo01.h"

//const char AIN_DEV[] = "/sys/devices/ocp.3/helper.15/AIN1";  //UNITS millivolts
//                      /sys/bus/iio/devices/iio:device0 is a link to ../../../devices/ocp.3.44e0d000.tscadc/tiadc/iio:devices0
const char AIN_DEV[] = "/sys/bus/iio/devices/iio\:device0/in_voltage1_raw"; //UNITS integer counts 4095

double CtoF(double c)
{
  return (c * 9.0 / 5.0) + 32.0;
}


#define REPLY_MAX 1035

int main( int argc, char *argv[] )
{
  FILE *fp;
  char reply[REPLY_MAX];
  int counts=0;
  int fdai;

  //* Open the command for reading.
  //grep "cape-bone-iio" /sys/devices/bone_capemgr.9/slots
  fp = popen("grep cape-bone-iio /sys/devices/bone_capemgr.9/slots", "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    fp = popen("echo cape-bone-iio > /sys/devices/bone_capemgr.*/slots", "r");
    while(fgets(reply, REPLY_MAX,fp) != NULL){ }
    printf("%s", reply);
  }else{
	  printf("cape-bone-iio already installed\n");
  }
  pclose(fp);	// close
//echo cape-bone-iio > /sys/devices/bone_capemgr.*/slots



  fdai = open(AIN_DEV, O_RDONLY);
  if(-1 == fdai)
  {	printf("Can't open Analog Input device\n");
  }else
  {   printf("Analog Input Device opened\n");
	  while (1)
	  {		char buffer[1024];
			int ret = read(fdai, buffer, sizeof(buffer));
			if (ret != -1)
			{
			  buffer[ret] = '\0';

			  double celsius = temperature(buffer, 1024, &(counts));
			  double fahrenheit = CtoF(celsius);
			  printf("int: %5d  degC: %5.2f  degF: %5.2f\n", counts, celsius, fahrenheit);
			  lseek(fdai, 0, 0);
			}
			sleep(1);
	  }
	  close(fdai);
  }
  return 0;
}
