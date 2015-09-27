#if !defined(LOOKUP_H)
#define LOOKUP_H

/**
 * @file
 * Table lookup with interpolation (1-D and 2-D) header file.
 */

#include <stdbool.h>

//=============================================================================
// One dimensional lookup table
typedef struct
{ unsigned int ncols;	// Number of elements in the table.  This must be at least 2.
  double  *inputs;		// List of input values.
  double *outputs;		// Table data (output values).  The output values list must have the same length as the input list.
} Table1d;





#ifdef TWODTABLE
/** Two dimensional lookup table. */
typedef const struct
{
  /** Number of columns (X values) in the table.  Must be at least 2. */
  unsigned char ncols;
  /** Number of rows (Y values) in the table.  Must be at least 2. */
  unsigned char nrows;
  /** X-axis input values list. */
  int *columns;
  /** Y-axis input values list. */
  int *rows;
  /** Table data.  This is an array of <code>columns</code>X<code>rows</code>,
      arranged in rows.  For example, <code>table[1]</code> is the second 
      column in the first row. */
  int *table;
} Table2d;

/******************************************************************************/
/* Prototypes */
bool lookup2d (const Table2d *t, int ix, int iy, int *o);

#endif


bool lookup1d (Table1d *t, double x, double *y);


#endif
