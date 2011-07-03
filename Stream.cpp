/*
 *  Stream.cpp
 *  swfparser
 *
 *  Created by Rintarou on 2010/5/20.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "Stream.h"
#include "stdlib.h"
#include "string.h"

FileStream::FileStream(FILE *fstream)
{
    _fstream = fstream;
}

int FileStream::read(unsigned char *dst, unsigned int numBytes)
{
    if (!_fstream)
        return -1; // error

    if (numBytes == 0) // numBytes should always > 0
        return 0;

    return fread (dst, 1, numBytes, _fstream);
}

// attach only
MemoryStream::MemoryStream(unsigned char *streamStart, unsigned int streamLength, int isOwner)
{
    _stream_start = streamStart;
    _stream_length = streamLength;
    _stream_pos = 0;
    _isOwner = isOwner;
}

int MemoryStream::read(unsigned char *dst, unsigned int numBytes)
{
    if (!_stream_start)
        return -1; // error

    if (numBytes == 0) // numBytes should always > 0
        return 0;
    
    unsigned int copySize = 0;
    
    if (_stream_pos + numBytes > _stream_length) {
        copySize = _stream_length - _stream_pos;
    } else {
        copySize = numBytes;
    }
    
    memcpy (dst, (_stream_start + _stream_pos), copySize);
    
    _stream_pos += copySize;
    
    return copySize;
}

MemoryStream::~MemoryStream()
{
    if (_isOwner) {
        free((void *)_stream_start);
    }
}
