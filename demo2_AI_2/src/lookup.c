/**
 * @file
 * Table lookup with interpolation (1-D and 2-D).
 */

#include "lookup.h"

/**
 * 1-D table lookup.
 *
 * This function performs a 1-D table lookup with interpolation.  The output
 * value is clamped to either of the table end values when the input value is
 * out of bounds.
 *
 * @param[in]   t      table data structure
 * @param[in]   x      input (X-axis) value
 * @param[out]  y      output data
 * @retval      true   if the lookup result is suspect due to clipping
 * @retval      false  on successful lookup
 */
bool lookup1d ( Table1d *t, double x, double *y)
{ unsigned char i;
  unsigned int TableMAX = (t->ncols)-1;
  
  if (x > t->inputs [TableMAX])
  {  *y = t->outputs[TableMAX];		 // MAXIMUM: Off the end of the table
     return true;
  }
  else if (x < t->inputs [0])
  {       *y = t->outputs[0];				 // MINIMUM: Off beginning of the table
    	  return true;
  }
  for (i = 0; i < t->ncols - 1; ++i)	// Within the bounds of the table
  { if (   x >= t->inputs[i] && x <= t->inputs[i + 1])
    {
      double y_low   = t->outputs[i];			// Output (outputs) low value
      double x_low   = t->inputs[i];		// Input (X-axis) low value
      double x_delta = t->inputs[i + 1]  - t->inputs[i];	// Spead between the two adjacent input values
      double y_delta = t->outputs[i + 1] - t->outputs[i];	// Spread between the two adjacent table output values
      
      // Prevent division by zero.
      // We could get here if two consecutive input values in the table are the same.
      if (x_delta == 0)
      {  *y = y_low;
          return true;
      }
      *y = y_low + ((x - x_low) * (long)y_delta) / x_delta;
      return false;
    }
  }
  return true;		// ASSET() Didn't find it (we shouldn't ever get here).
}



#ifdef TWODTABLE

/**
 * 2-D table lookup.
 *
 * This function performs a 2-D table lookup with interpolation.  The output
 * value is clamped to either of the table end values when the input value is
 * out of bounds.
 *
 * @param[in]   t      table data structure
 * @param[in]   ix     input (X-axis) value
 * @param[in]   iy     input (Y-axis) value
 * @param[out]  o      output value
 *
 * @retval      true   if the lookup result is suspect due to clipping
 * @retval      false  on successful lookup
 */

bool lookup2d (Table2d *t, int ix, int iy, int *o)
{
  /* The lower X and Y coordinates of the interpolation box */
  unsigned char i, j;
  /* Set whenever one of the lookups goes off the end of the table */
  bool is_fault = false;
  
  /* ======================================================================== */
  /* X axis coordinate lookup */  
  /* ======================================================================== */
  /* ------------------------------------------------------------------------ */
  /* Off the end of the table */
  if (ix > t->outputs[t->ncols - 1])
  {
    /* Pretend the input value is right at the table edge so that interpolation
       works as expected */
    ix = t->columns[t->ncols - 1];
    i = t->ncols - 1;
    is_fault = true;
  }
  /* ------------------------------------------------------------------------ */
  /* Off beginning of the table */
  else if (ix < t->columns[0])
  {
    ix = t->columns[0];
    i = 0;
    is_fault = true;
  }
  /* ------------------------------------------------------------------------ */
  /* Within the bounds of the table */
  else
  {
    for (i = 0; i < t->ncols - 1; ++i)
    {
      if (   ix >= t->columns[i]
          && ix <= t->columns[i + 1])
      {
        break;
      }
    }
  }

  /* ======================================================================== */
  /* Y axis coordinate lookup */
  /* ======================================================================== */
  /* ------------------------------------------------------------------------ */
  /* Off the bottom of the table */
  if (iy > t->rows[t->nrows - 1])
  {
    iy = t->rows[t->nrows - 1];
    j = t->nrows - 1;
    is_fault = true;
  }
  /* ------------------------------------------------------------------------ */
  /* Off the top of the table */
  else if (iy < t->rows[0])
  {
    iy = t->rows[0];
    j = 0;
    is_fault = true;
  }
  /* ------------------------------------------------------------------------ */
  /* Within the bounds of the table */
  else
  {
    for (j = 0; j < t->nrows - 1; ++j)
    {
      if (   iy >= t->rows[j]
          && iy <= t->rows[j + 1])
      {
        break;
      }
    }
  }

  /* ======================================================================== */
  /* 2-D interpolation */
  /* ======================================================================== */
  /* At this point we know that the input X value is between
     column[i] and column[i+1] and that the input Y value is between
     row[j] and row[j+1].  Therefore we have a rectangle in which we need
     to interpolate. 
     
     To do the interpolation, we first interpolate between column i and
     column i+1 on the upper row j.  Then, we interpolate between the same
     columns on row j+1.  Finally, we interpolate vertically between the two
     rows based on the input Y value.
     
     row0 is the upper row data and row1 is the lower (higher subscript) row
     data. */
  {
    const int *row0 = &t->table[j * t->ncols];
    const int *row1 = &row0[t->ncols];
    /* Difference between the two adjacent column values */
    int i_delta = t->columns[i + 1] - t->columns[i];
    /* Difference between the two adjacent row values */
    int j_delta = t->rows[j + 1] - t->rows[j];
    /* Low column value */
    int i_low = t->columns[i];
    /* Low row value */
    int j_low = t->rows[j];
    /* Interpolation results for the upper and lower rows */
    int o0, o1;
    
    /* ---------------------------------------------------------------------- */
    /* Prevent division by zero if the input values aren't increasing.
       If no division by zero, interpolate between columns in the upper and
       lower row. */
    if (i_delta == 0)
    {
      o0 = row0[i];
      o1 = row1[i];
      is_fault = true;
    }
    else
    {    
      /* -------------------------------------------------------------------- */
      /* Interpolate the upper row */
      {
        int o_low   = row0[i];                 /* Row value at low column # */
        int o_delta = row0[i + 1] - row0[i];   /* Difference from next column */
  
        o0 = o_low + ((ix - i_low) * (long)o_delta) / i_delta;
      }
      /* -------------------------------------------------------------------- */
      /* Interpolate the lower (higher subscript) row */
      {
        int o_low   = row1[i];                 /* Row value at low column # */
        int o_delta = row1[i + 1] - row1[i];   /* Difference from next column */
  
        o1 = o_low + ((ix - i_low) * (long)o_delta) / i_delta;
      }
    }

    /* ---------------------------------------------------------------------- */
    /* Guard against division by zero in the row axis.  If all is well,
       interpolate between the two row interpolation results from earlier. */
    if (j_delta == 0)
    {
      *o = o0;
      is_fault = true;
    }
    else
    {
      *o = o0 + ((iy - j_low) * (long)(o1 - o0)) / j_delta;
    }
  }
 
  return is_fault;
}
#endif

