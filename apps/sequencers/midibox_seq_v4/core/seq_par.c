// $Id$
/*
 * Parameter Layer Routines
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <mios32.h>

#include "seq_par.h"
#include "seq_core.h"


/////////////////////////////////////////////////////////////////////////////
// Local types
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Local prototypes
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Global variables
/////////////////////////////////////////////////////////////////////////////

u8 par_layer_value[SEQ_CORE_NUM_TRACKS][3][SEQ_CORE_NUM_STEPS];


/////////////////////////////////////////////////////////////////////////////
// Local variables
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Initialisation
/////////////////////////////////////////////////////////////////////////////
s32 SEQ_PAR_Init(u32 mode)
{
  int track, step;

  // init parameter layer values
  for(track=0; track<SEQ_CORE_NUM_TRACKS; ++track) {
    for(step=0; step<SEQ_CORE_NUM_STEPS; ++step) {
      par_layer_value[track][0][step] = 0x3c; // note C-3
      par_layer_value[track][1][step] = 100;  // velocity
      par_layer_value[track][2][step] = 17;   // gatelength 75%
    }
  }

  return 0; // no error
}