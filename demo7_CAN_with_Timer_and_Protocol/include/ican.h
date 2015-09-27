#ifndef ICAN_H
#define ICAN_H

//#include <stdio.h>			//printf
//#include <stdlib.h>
#//include <string.h>			//strncpy
#include <net/if.h>				//needed for sa_family type
//#include <sys/ioctl.h>
#include <linux/can.h>

int CAN_Destroy();
int CAN_Init();
int CAN_RX();

int iCAN_TX(  );
int iCAN_TX2(  );

#endif  //ICAN_H
