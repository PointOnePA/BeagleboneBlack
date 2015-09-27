/*
 * demo01.c
 *
 *  Created on: Nov 4, 2014
 */

#include "../include/demo02.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define REPLY_MAX 1035

#define SYSBUS
#ifdef SYSBUS
const char AIN0_DEV[] = "/sys/bus/iio/devices/iio\:device0/in_voltage0_raw"; //UNITS
const char AIN1_DEV[] = "/sys/bus/iio/devices/iio\:device0/in_voltage1_raw"; //integer counts 4095

double CtoF(double c){	  return (c * 9.0 / 5.0) + 32.0;	}


int main()
{	char buffer0[1024], buffer1[1024];
	int ret=0, counts0=0,counts1=0, errsv;
	int fd0 = open(AIN0_DEV, O_RDONLY);
	int fd1 = open(AIN1_DEV, O_RDONLY);
	double TdegC0=0.0, TdegF0=0.0, TdegC1=0.0, TdegF1=0.0;

	  FILE *fp;
	  char reply[REPLY_MAX];

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


	if(-1 == fd0 || -1 == fd1)
	{	if(-1 == fd0)
		{	printf("Can't open AIN0: %s\n",AIN0_DEV);
		}
		if(-1 == fd1)
		{	printf("Can't open AIN1: %s\n",AIN1_DEV);
		}
	}else
	{   printf("Analog Input Devices opened %d %d\n",fd0, fd1);
		while (1)
		{
     		ret = read(fd0, buffer0, sizeof(buffer0));
			if (ret != -1)
			{	buffer0[ret] = '\0';
				TdegC0 = temperature(buffer0, 1024, &(counts0));
				TdegF0 = CtoF(TdegC0);
			}else
			{	errsv = errno;
				printf("Read Error fd0 %d\t",errsv);
			}
			lseek(fd0, 0, 0);

			ret = read(fd1, buffer1, sizeof(buffer1));
			if (ret != -1)
			{	buffer1[ret] = '\0';
				TdegC1 = temperature(buffer1, 1024, &(counts1));
				TdegF1 = CtoF(TdegC1);
			}else
			{	errsv = errno;
				printf("Read Error fd1 %d\t",errsv );
			}
			printf("#0%5d degC0:%5.2f degF0:%5.2f   #1%5d degC1:%5.2f degF1:%5.2f\n",counts0, TdegC0, TdegF0, counts1, TdegC1, TdegF1);
			lseek(fd1, 0, 0);
			sleep(1);
		}
	}
	if( -1 != fd0)close(fd0);
	if( -1 != fd1)close(fd1);
    return 0;
}

#else

const char AIN0_DEV[] = "/sys/devices/ocp.3/helper.15/AIN0"; //UNITS
const char AIN1_DEV[] = "/sys/devices/ocp.3/helper.15/AIN1"; //mV 1800 => 1.8V max

double CtoF(double c){	  return (c * 9.0 / 5.0) + 32.0;	}


int main()
{	char buffer0[1024], buffer1[1024];
	int ret=0, counts0=0,counts1=0, errsv;
	int fd0 = open(AIN0_DEV, O_RDONLY);
	int fd1 = open(AIN1_DEV, O_RDONLY);
	double TdegC0=0.0, TdegF0=0.0, TdegC1=0.0, TdegF1=0.0;

	if(-1 == fd0 || -1 == fd1)
	{	if(-1 == fd0)
		{	printf("Can't open AIN0: %s\n",AIN0_DEV);
		}
		if(-1 == fd1)
		{	printf("Can't open AIN1: %s\n",AIN1_DEV);
		}
	}else
	{   printf("Analog Input Devices opened %d %d\n",fd0, fd1);
		while (1)
		{
     		ret = read(fd0, buffer0, sizeof(buffer0));
			if (ret != -1)
			{	buffer0[ret] = '\0';
				TdegC0 = temperature(buffer0, 1024, &(counts0));
				TdegF0 = CtoF(TdegC0);
			}else
			{	errsv = errno;
				printf("Read Error fd0 %d\t",errsv);
			}
			lseek(fd0, 0, 0);

			ret = read(fd1, buffer1, sizeof(buffer1));
			if (ret != -1)
			{	buffer1[ret] = '\0';
				TdegC1 = temperature(buffer1, 1024, &(counts1));
				TdegF1 = CtoF(TdegC1);
			}else
			{	errsv = errno;
				printf("Read Error fd1 %d\t",errsv );
			}
			printf("#0%5d degC0:%5.2f degF0:%5.2f   #1%5d degC1:%5.2f degF1:%5.2f\n",counts0, TdegC0, TdegF0, counts1, TdegC1, TdegF1);
			lseek(fd1, 0, 0);
			sleep(1);
		}
	}
	if( -1 != fd0)close(fd0);
	if( -1 != fd1)close(fd1);
    return 0;
}

#endif
