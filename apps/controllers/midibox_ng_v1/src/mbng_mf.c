// $Id$
/*
 * Motorfader access functions for MIDIbox NG
 *
 * ==========================================================================
 *
 *  Copyright (C) 2012 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <mios32.h>

#include "app.h"
#include "tasks.h"
#include "mbng_mf.h"
#include "mbng_lcd.h"
#include "mbng_patch.h"
#include "mbng_event.h"


/////////////////////////////////////////////////////////////////////////////
// local variables
/////////////////////////////////////////////////////////////////////////////

static u8  mf_value_msb[MBNG_PATCH_NUM_MF_MODULES][8];
static u8  mf_touchsensor_selected[MBNG_PATCH_NUM_MF_MODULES];

/////////////////////////////////////////////////////////////////////////////
// This function initializes the MF handler
/////////////////////////////////////////////////////////////////////////////
s32 MBNG_MF_Init(u32 mode)
{
  if( mode != 0 )
    return -1; // only mode 0 supported

  int i;
  for(i=0; i<MBNG_PATCH_NUM_MF_MODULES; ++i) {
    int j;
    for(j=0; j<8; ++j)
      mf_value_msb[i][j] = 0;
    mf_touchsensor_selected[i] = 0;
  }

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// This function should be called from APP_MIDI_NotifyPackage whenver a new
// MIDI event has been received
/////////////////////////////////////////////////////////////////////////////
s32 MBNG_MF_MIDI_NotifyPackage(mios32_midi_port_t port, mios32_midi_package_t midi_package)
{
  // motorfaders will send/receive the Motormix protocol!

  if( midi_package.event != CC )
    return 0; // not for interest

  u8 is_fader_msb       = (midi_package.cc_number & 0xf8) == 0x00;
  u8 is_fader_lsb       = (midi_package.cc_number & 0xf8) == 0x20;
  u8 is_touchsensor_sel = midi_package.cc_number == 0x0f;
  u8 is_touchsensor_val = midi_package.cc_number == 0x2f;

  int module;
  mbng_patch_mf_entry_t *mf = (mbng_patch_mf_entry_t *)&mbng_patch_mf[0];
  for(module=0; module<MBNG_PATCH_NUM_MF_MODULES; ++module, ++mf) {
    if( mf->flags.enabled && midi_package.chn == mf->flags.chn &&
	(port == mf->midi_in_port || port == mf->config_port) ) {

      // forward to config or MIDI OUT port
      mios32_midi_port_t out_port = (port == mf->midi_in_port) ? mf->config_port : mf->midi_out_port;
      MUTEX_MIDIOUT_TAKE;
      MIOS32_MIDI_SendPackage(out_port, midi_package);
      MUTEX_MIDIOUT_GIVE;

      if( is_touchsensor_sel ) {
	mf_touchsensor_selected[module] = midi_package.value;
      } else if( is_touchsensor_val ) {
	u8 is_pressed = midi_package.value >= 0x40;
	u8 ts_sel = mf_touchsensor_selected[module];
	if( ts_sel < 8 && mf->ts_first_button_id ) {

	  // get ID
	  mbng_event_item_id_t button_id = mf->ts_first_button_id + ts_sel;
	  mbng_event_item_t item;
	  if( MBNG_EVENT_ItemSearchById(button_id, &item) < 0 ) {
	    if( debug_verbose_level >= DEBUG_VERBOSE_LEVEL_INFO ) {
	      DEBUG_MSG("No event assigned to BUTTON id=%d for touchsensor\n", button_id & 0xfff);
	    }
	    return -2; // no event assigned
	  }

	  if( debug_verbose_level >= DEBUG_VERBOSE_LEVEL_INFO ) {
	    MBNG_EVENT_ItemPrint(&item);
	  }

	  // take over new value
	  item.value = is_pressed ? item.max : item.min;

	  // send MIDI event
	  MBNG_EVENT_NotifySendValue(&item);
	}
      } else if( is_fader_msb ) {
	u8 fader_sel = midi_package.cc_number & 0x7;
	mf_value_msb[module][fader_sel] = midi_package.value;
      } else if( is_fader_lsb ) {
	u8 fader_sel = midi_package.cc_number & 0x7;
	u16 value = midi_package.value | ((u16)mf_value_msb[module][fader_sel] << 7);
	u16 fader = module*8 + fader_sel;

	// get ID
	u16 hw_id = fader + 1;
	mbng_event_item_t item;
	if( MBNG_EVENT_ItemSearchByHwId(MBNG_EVENT_CONTROLLER_MF, hw_id, &item) < 0 ) {
	  if( debug_verbose_level >= DEBUG_VERBOSE_LEVEL_INFO ) {
	    DEBUG_MSG("No event assigned to MF hw_id=%d\n", hw_id);
	  }
	  return -2; // no event assigned
	}

	if( debug_verbose_level >= DEBUG_VERBOSE_LEVEL_INFO ) {
	  MBNG_EVENT_ItemPrint(&item);
	}

	// scale value from 14bit
	s16 min = item.min;
	s16 max = item.max;
	u8 *map_values;
	int map_len = MBNG_EVENT_MapGet(item.map, &map_values);
	if( map_len > 0 ) {
	  min = 0;
	  max = map_len-1;
	}

	u32 value_scaled;
	if( min <= max ) {
	  int range = max - min + 1;
	  value_scaled = min + (value / (16384 / range));
	} else {
	  int range = min - max + 1;
	  value_scaled = max + (value / (16384 / range));
	}

	if( map_len > 0 ) {
	  value_scaled = map_values[value_scaled];
	}

	if( item.value != value_scaled ) {
	  // take over new value
	  item.value = value_scaled;

	  // send MIDI event
	  MBNG_EVENT_NotifySendValue(&item);
	}
      }
    }
  }

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// This function should be called from APP_SYSEX_Parser on incoming SysEx data
/////////////////////////////////////////////////////////////////////////////
s32 MBNG_MF_ReceiveSysEx(mios32_midi_port_t port, u8 midi_in)
{
  int module;
  mbng_patch_mf_entry_t *mf = (mbng_patch_mf_entry_t *)&mbng_patch_mf[0];
  for(module=0; module<MBNG_PATCH_NUM_MF_MODULES; ++module, ++mf) {
    if( mf->flags.enabled &&
	(port == mf->midi_in_port || port == mf->config_port) ) {

      mios32_midi_port_t out_port = (port == mf->midi_in_port) ? mf->config_port : mf->midi_out_port;

      // forward as single byte
      // TODO: optimize this by collecting up to 3 data words and put it into package
      MUTEX_MIDIOUT_TAKE;
      mios32_midi_package_t midi_package;
      midi_package.ALL = 0x00000000;
      midi_package.type = 0xf; // single byte
      midi_package.evnt0 = midi_in;
      MIOS32_MIDI_SendPackage(out_port, midi_package);
      MUTEX_MIDIOUT_GIVE;
    }
  }

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
// This function is called by MBNG_EVENT_ItemReceive when a matching value
// has been received
/////////////////////////////////////////////////////////////////////////////
s32 MBNG_MF_NotifyReceivedValue(mbng_event_item_t *item)
{
  u16 hw_id = item->hw_id;

  if( debug_verbose_level >= DEBUG_VERBOSE_LEVEL_INFO ) {
    DEBUG_MSG("MBNG_MF_NotifyReceivedValue(%d, %d)\n", hw_id, item->value);
  }

  // forward to MF
  if( hw_id && hw_id <= MBNG_PATCH_NUM_MF_MODULES*8 ) {
    int module = (hw_id-1) / 8;
    int fader = (hw_id-1) % 8;
    mbng_patch_mf_entry_t *mf = (mbng_patch_mf_entry_t *)&mbng_patch_mf[module];
    if( mf->flags.enabled ) {
      // scale value to 14bit
      int value14;
      u8 *map_values;
      int map_len = MBNG_EVENT_MapGet(item->map, &map_values);
      if( map_len > 0 ) {
	value14 = MBNG_EVENT_MapIxGet(map_values, map_len, item->value) * (16384 / map_len);
      } else if( item->min <= item->max ) {
	int range = item->max - item->min + 1;
	value14 = (item->value - item->min) * (16384 / range);
      } else {
	int range = item->min - item->max + 1;
	value14 = (item->value - item->max)* (16384 / range);
      }

      if( value14 < 0 )
	value14 = 0;
      else if( value14 > 16383 )
	value14 = 16383;

      MUTEX_MIDIOUT_TAKE;
      MIOS32_MIDI_SendCC(mf->midi_out_port, mf->flags.chn, 0x00 + fader, (value14 >> 7) & 0x7f);
      MIOS32_MIDI_SendCC(mf->midi_out_port, mf->flags.chn, 0x20 + fader, value14 & 0x7f);
      MUTEX_MIDIOUT_GIVE;    
    }
  }

  return 0; // no error
}
