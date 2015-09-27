//ican.c
#include <stdio.h>			//printf
#include <stdlib.h>
#include <string.h>			//strncpy
#include <net/if.h>
#include <sys/ioctl.h>
//#include <linux/can.h>

//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <linux/can/raw.h>

#include "ican.h"

static int s, N=0;
static int nbytes;
static struct sockaddr_can addr;
static struct can_frame frame;
static struct ifreq ifr;

//-----------------------------------------------------------------------------
int CAN_Destroy()
{	int iret=0;
	if(s)
	{	iret = close(s);
	}
	return(iret);
}
//-----------------------------------------------------------------------------
int CAN_Init()
{	int iMode=1;
	char *ifname = "can0";

	//Establish socket ProtocolFamily, Type_is_RAW, Protocol is CAN_RAW
	if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
	{	perror("Error while opening socket");
		return -1;
	}

	//ifconfig - convert inteface string can0
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) );
	ioctl(s, SIOCGIFINDEX, &ifr);			// into interface index ifr
	ioctl(s, FIONBIO, &iMode);				// non blocking mode enabled

	addr.can_ifindex = ifr.ifr_ifindex;		//setup address for binding
 	addr.can_family  = PF_CAN;				//select protocol family
	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);
	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)		//bind socket to address
	{	perror("Error in socket bind");
		return -2;
	}
	return(0);
}
//-----------------------------------------------------------------------------
int CAN_Get()
{	int i;

	nbytes = read(s,&frame, sizeof(struct can_frame));
	if(nbytes>0)
	{	printf("%3x ",frame.can_id);
		for(i=0; i<frame.can_dlc; i++)
		{	printf("%2X ",frame.data[i]);
		}
		printf("\n");
		N=0;
	}else
	{	N++;
	}
return(N);
}
