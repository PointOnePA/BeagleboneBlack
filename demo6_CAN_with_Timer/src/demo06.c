#include <stdio.h>			//printf
#include "mstime.h"
#include "ican.h"

//----------------------------------------------------
int main(int argc, char *argv[])
{	int i,j=0,k=0,M=0, N=0;

	printf("Creating Timer\n");
	if(0>MSTimerCreate())
	{	printf("Timer Creation Failed\n");
	}

	CAN_Init();


	   for(k=0;k<10;k++)
	   {   for(i=0;i<1000;i++)
		   {   for(j=0;j<94000;j++)
			   {	M=i+j;
			   }
		   }
		   printf("%d %d\n",k, mscount);	//global to avoid function call
	   }


/*
	while( M < 50)
	{	N=CAN_Get();
		if(!N)
		{	++M;
		}
	}
*/

	printf("Destroying timer\n");
	MSTimerDestroy();
	printf("Destroying CAN\n");
	CAN_Destroy();

	return 0;
}
