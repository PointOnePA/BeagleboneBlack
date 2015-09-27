#include "thermistor.h"
#include <stdio.h>

//Thermistor Scaling                  1      2      3      4      5      6      7      8      9     10     11     12     13     14     15     16     17    18    19
static double adcCount[] = {503.,576.,659.,754.,863.,985.,1123.,1276.,1445.,1629.,1826.,2032.,2244.,2457.,2667.,2880.,3058.,3232.,3388.};  // 12-bit ADC counts with 1.8V reference.,
static double TempdegC[] = { 90., 85., 80., 75., 70., 65.,  60.,  55.,  50.,  45.,  40.,  35.,  30.,  25.,  20.,  15.,  10.,   5.,   0. };  // Output values, signed fixed point.

typedef  struct
{ unsigned char ncols;	// Number of elements in the table.  This must be at least 2.
  double *inputs;		// List of input values.
  double *outputs;		// Table data (output values).  The output values list must have the same length as the input list.
} Table1d;

Table1d THERMISTORTABLE = {	.ncols = 19, .inputs = adcCount,  .outputs = TempdegC };

double temperature(char *string, int ilen, int *counts)
{ double x=0.0, y=0.0;

  (*counts) = (int)strtol(string, &(string[ilen-1]), (int)10 );
  x = (double)(*counts);

  //printf("%d %f %f\n",THERMISTORTABLE.ncols, THERMISTORTABLE.inputs[1], THERMISTORTABLE.outputs[1]);

  bool ret1 = lookup1d ( &(THERMISTORTABLE), (double)x, &y);

  return(y);
}
