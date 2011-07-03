/*
 *  Util.cpp
 *  
 *
 *  Created by Rintarou on 2010/5/15.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "Util.h"
#include "zlib.h"
#include "common.h"

void debugMsg( const char* fmt, ... )
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

char *Color2String(unsigned int rgba, int hasAlpha)
{
    static char strBuffer[30];
    if (hasAlpha)
        snprintf(strBuffer, 30, "0x%08x", rgba);
    else
        snprintf(strBuffer, 30, "0x%06x", rgba);
    return strBuffer;
}


#define CHUNK 4096

// It's caller's job to return the allocated memory.
//int Util::inflate2Memory (Stream *stream, unsigned char **output_ptr)
int inflate2Memory (Stream *stream, unsigned char **output_ptr)
{
    unsigned char *inflated_stream = 0;
    unsigned int inflated_stream_size = 0;
    int ret;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
    
    strm.zalloc     = Z_NULL;
    strm.zfree      = Z_NULL;
    strm.opaque     = Z_NULL;
    strm.avail_in   = 0;        // number of bytes available at next_in 
    strm.next_in    = Z_NULL;   // pointer to the next input byte
	
    ret = inflateInit(&strm);
    if (ret != Z_OK) {
        DEBUGMSG("failed to inflateInit()\n");
        return -1; 
    }
    
	
    for(;;) {
		
        int numBytes = 0;
		
        if (strm.avail_in == 0) { // No data for zlib, read from source.
            numBytes = stream->read(in, CHUNK);
            
            ASSERT(numBytes < 0);
            
            if (numBytes == 0) {
                DEBUGMSG("end of stream\n");
                break;
            }
            //DEBUGMSG("numBytes = %d\n", numBytes);
			
            strm.next_in  = in;
            strm.avail_in = numBytes;
        }
		
		
        strm.avail_out  = CHUNK;     // remaining free space at next_out
        strm.next_out   = out;       // where the next output byte should be put
		
        ret = inflate(&strm, Z_SYNC_FLUSH);
		/*
		 DEBUGMSG("strm.avail_in = %d\n", strm.avail_in);
		 DEBUGMSG("strm.avail_out = %d\n", strm.avail_out);
		 */
		
		if (ret == Z_BUF_ERROR) {
            DEBUGMSG("Z_BUF_ERROR\n");
            break;
        }
        if (ret == Z_DATA_ERROR) {
            DEBUGMSG("Z_DATA_ERROR msg=%s\n", strm.msg);
            break;
        }
        if (ret == Z_MEM_ERROR) {
            DEBUGMSG("Z_MEM_ERROR\n");
            break;
        }
		
        unsigned int expended_size = inflated_stream_size + (CHUNK - strm.avail_out);
        
        //fprintf(stderr, "expended_size = %d\n", expended_size);
        void *newPtr = realloc((void *)inflated_stream, expended_size);
		inflated_stream = (unsigned char *)newPtr;
        if (!inflated_stream) {
            DEBUGMSG("fatal error to realloc()\n");
            inflateEnd(&strm);
            return -1; 
        }
		
        memcpy (inflated_stream + inflated_stream_size, out, CHUNK - strm.avail_out);
        //inflated_stream_size += (CHUNK - strm.avail_out);
        inflated_stream_size = expended_size;
		
        if (ret == Z_STREAM_END) {
            //DEBUGMSG("Z_STREAM_END\n");
            inflateEnd(&strm);
            (*output_ptr) = inflated_stream;
            
            //fprintf(stderr, "Uncompressed Size = %d Bytes\n", inflated_stream_size);
            return inflated_stream_size;
        }
    }
    inflateEnd(&strm);
    return -1;
}

