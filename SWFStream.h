/*
 *  SWFStream.h
 *  
 *
 *  Created by Rintarou on 2010/5/15.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "common.h"

#ifndef _SWFSTREAM_H
#define _SWFSTREAM_H

class SWFStream {
public:
	
	SWFStream();
	~SWFStream();
	
	//// File Operation
	int				open(const char *filename);
	
	//// TODO: 
	//Attach();
	//Detach();
	
	//// Debug Functions
	void			dump(unsigned int numBytes);
	void			skip(unsigned int numBytes) { _stream_pos += numBytes; }
    void            seek(unsigned int streamPos) { _stream_pos = streamPos; }

	//// Bit Operations
	unsigned int	getUBits(unsigned int numBits);
	signed int		getSBits(unsigned int numBits);
	void			setByteAlignment() { _bitOpFlag = 0; }
	
	//// Integer Operations
	unsigned int	getUI32();
	unsigned int	getUI16();
	unsigned int	getUI8();
	
	signed   int	getSI16();
    signed   int	getSI32();
	
	//// Fixed Number Operation
	float			getFIXED8();	// 16-bit 8.8 fixed-point number
	float			getFIXED();		// 32-bit 16.16 fixed-point number
	
	//// Floating-point Number Operation
	float			getFLOAT16();	
	float			getFLOAT();
	double			getDOUBLE();
	unsigned int	getEncodedU32();
	
	//// String Operation
	char*           getSTRING();
	unsigned int	getLANGUAGECODE();
	
	//// Info
	int				isOpened() { return _streamBuffer ? 1 : 0 ; }
	unsigned int	getStreamPos() { return _stream_pos; }
	unsigned int	getFileLength() { return FileLength; }
    
    //// Ugly usage to get the memory pointer FIXME later
    //// used by internal zipped data handlers (DefinebitsLosses Tag Handler)
    unsigned char   *getStreamPosPtr() { return _streamBuffer + _stream_pos; }

//protected:	
	//// File Info
    char	Signature[4];
	unsigned int	SWFVersion;
	unsigned int	FileLength;	
	
private:
	
	//// Bit Buffer Handlers
	unsigned int	_bitBufferLen;
	unsigned char	_bitBuffer;
	int				_bitOpFlag;
	
	//// Stream Handlers
	unsigned char	*_streamBuffer;
	unsigned int	_stream_pos;
	unsigned int	_stream_size;

};

#endif