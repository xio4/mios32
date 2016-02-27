/*
 * MIDIbox Genesis Tracker: VGM Source Stream from SD Card Class
 *
 * ==========================================================================
 *
 *  Copyright (C) 2016 Sauraen (sauraen@gmail.com)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _VGMSOURCESTREAM_H
#define _VGMSOURCESTREAM_H

//#include "vgmsource.h"
#include <file.h>

#ifndef VGMSOURCESTREAM_BUFSIZE
#define VGMSOURCESTREAM_BUFSIZE 512
#endif


class VgmSourceStream {
public:
    VgmSourceStream();
    ~VgmSourceStream();
    
    u8 getByte(u32 addr);
    inline u32 getSize() { return datalen; }
    
    inline u8 getBlockByte(u32 blockaddr) { return ((blockaddr < blocklen) ? (block[blockaddr]) : 0); }
    inline u32 getBlockSize() { return blocklen; }
    void loadBlock(u32 startaddr, u32 len);
    
    s32 startStream(char* filename);
    
    void bg_streamBuffer();
    
    void readHeader();
    u32 vgmdatastartaddr;
    u32 opn2clock;
    u32 psgclock;
    u32 loopaddr;
    u32 loopsamples;
    
private:
    file_t file;
    u32 datalen;
    
    u8* buffer1;
    u8* buffer2;
    u32 buffer1addr;
    u32 buffer2addr;
    
    u8 wantbuffer;
    u8 wantbufferaddr;

    u8* block;
    u32 blocklen;
    u32 blockorigaddr;
};


#endif /* _VGMSOURCE_H */