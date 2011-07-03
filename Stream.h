/*
 *  Stream.h
 *  swfparser
 *
 *  Created by Rintarou on 2010/5/20.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

//#include "common.h"

#include "stdio.h"

#ifndef _STREAM_H_
#define _STREAM_H_

// As a simple interface for stream read so far.
// Todo : Need to extend for more general usage.
class Stream {
public:
    // return number of bytes read or -1 if error, exactly like fread()
    virtual int read(unsigned char *dst, unsigned int numBytes) = 0;
};

class FileStream : Stream {

public:
    FileStream(FILE *fstream);
    int read(unsigned char *dst, unsigned int numBytes);
private:
    FILE *_fstream;
};


class MemoryStream : Stream {
public:
    MemoryStream(unsigned char *streamStart, unsigned int streamLength, int isOwner);    // attach
    ~MemoryStream();
    int read(unsigned char *dst, unsigned int numBytes);
    unsigned int    length() { return _stream_length; };
    void *          getStartPtr() { return (void *)_stream_start; };
    
private:
    unsigned int    _stream_length;
    unsigned char   *_stream_start;
    unsigned int    _stream_pos; // current pos
    
    int             _isOwner;   // if isOwner, need to free the memory in destructor.
};

#endif // _STREAM_H_