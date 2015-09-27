//ican.c
#include <stdio.h>			//printf
#include <stdlib.h>
#include <string.h>			//strncpy
#include <net/if.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <linux/can.h>
#include <errno.h>
#include <unistd.h>
#include "ican.h"

static int sfd, N=0;
static int nbytes;
static struct sockaddr_can addr;
static struct can_frame frame;
static struct can_frame framewrite;
static struct ifreq ifr;

//-----------------------------------------------------------------------------
int CAN_Destroy()
{	int iret=0;
	if(sfd)
	{	iret = close(sfd);
	}
	return(iret);
}
//-----------------------------------------------------------------------------
int CAN_Init()
{	int iMode=1;
	char *ifname = "can0";

	//Establish socket ProtocolFamily, Type_is_RAW, Protocol is CAN_RAW
	if((sfd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
	{	perror("Error while opening socket");
		return -1;
	}

	//ifconfig - convert inteface string can0
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) );
	ioctl(sfd, SIOCGIFINDEX, &ifr);			// into interface index ifr
	ioctl(sfd, FIONBIO, &iMode);				// non blocking mode enabled

	addr.can_ifindex = ifr.ifr_ifindex;		//setup address for binding
 	addr.can_family  = PF_CAN;				//select protocol family
	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);
	if(bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)		//bind socket to address
	{	perror("Error in socket bind");
		return -2;
	}
	return(0);
}
//-----------------------------------------------------------------------------
int CAN_RX()
{	int i;

	nbytes = read(sfd ,&frame, sizeof(struct can_frame));
	if(nbytes>0)
	{	printf("RX %3x ",frame.can_id);
		for(i=0; i<frame.can_dlc; i++)
		{	printf("%2X ",frame.data[i]);
			framewrite.data[i] = frame.data[i];
		}
		framewrite.can_dlc = frame.can_dlc;
		framewrite.can_id = 55;
		printf("\n");
		N=0;
	}else
	{	N++;
	}
return(nbytes);
}

static unsigned char calc_csum (unsigned char *buf, unsigned char len)
{
  unsigned char *p = buf + len - 1;
  unsigned char csum = 0;

  for (; len; --len)
    csum += *(p--);

  return csum & 0xFF;
}

//-----------------------------------------------------------------------------
int iCAN_TX( )
{	int i, errsv=0, n=0;

	if(sfd)
	{	framewrite.can_id = 55;
		framewrite.can_dlc = 8;
		framewrite.data[1] = 6;		// src _this->node
		framewrite.data[2] = 9;		// dest
		framewrite.data[3] = 1;		// CMD write-word
		framewrite.data[4] = 0x10;	// address high
		framewrite.data[5] = 0x02;  // address low
		framewrite.data[6] = 00;  	// data high
		framewrite.data[7] = 00;  	// data low
		framewrite.data[0] = calc_csum(&framewrite.data[1],framewrite.can_dlc-1 );

		n = write(sfd, &framewrite, sizeof(struct can_frame) );
		if(n <0)				// 6 ENXIO No such device or address
		{	errsv = errno;		//22=EINVAL Invalid argument
			printf("\t\t%d %d %d\n",errsv, framewrite.can_dlc, n );
			assert(n >0);
		}
	}
return(n);
}

//-----------------------------------------------------------------------------
int iCAN_TX2(  )
{	int errsv=0, n=0;

	if(sfd)
	{	n = write(sfd, &framewrite, sizeof(struct can_frame) );
		if(n <0)				// 6 ENXIO No such device or address
		{	errsv = errno;		//22=EINVAL Invalid argument
			printf("\t\t%d %d %d\n",errsv, framewrite.can_dlc, n );
			assert(n >0);
		}
	}
return(n);
}
