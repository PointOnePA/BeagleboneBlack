#include <stdio.h>			//printf
#include "mstime.h"

//----------------------------------------------------
void main(int argc, char *argv[])
{	int i,j,k=0,M=0;
	int iret=0;

	printf("Creating Timer\n");
	if(0>MSTimerCreate())
	{	printf("Timer Creation Failed\n");
	}

	   for(k=0;k<10;k++)
	   {   for(i=0;i<1000;i++)
		   {   for(j=0;j<94000;j++)
			   {	M=i+j;
			   }
		   }
		   printf("%d %d\n",k, mscount);	//global to avoid function call
	   }

	printf("Destroying timer\n");
	MSTimerDestroy();

}
