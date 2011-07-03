/*
 *  SWFStream.cpp
 *  
 *
 *  Created by Rintarou on 2010/5/15.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "common.h"

#include "SWFStream.h"
#include "Util.h"


SWFStream::SWFStream()
{
	_bitBuffer		= 0;
	_bitBufferLen	= 0;
	_bitOpFlag		= 0;
	_streamBuffer	= 0;
	_stream_pos		= 0;
	_stream_size	= 0;
	SWFVersion		= 0;
	bzero (Signature, 4);
	FileLength		= 0;
}

SWFStream::~SWFStream()
{
	if (_streamBuffer) {
		free(_streamBuffer);
	}
}

int SWFStream::open(const char *filename)
{
	FILE *file_stream = fopen (filename, "r");
    
    if (!file_stream) {
        DEBUGMSG("Error: No such file.\n");
        return FALSE;
    }
    
    //// Get SWF Header
    DEBUGMSG("{\n");
        
    fread (Signature, 1, 3, file_stream);
    DEBUGMSG("Signature : \"%s\"", Signature);
        
    if (((Signature[0] != 'C') && 
        (Signature[0] != 'F')) || 
        (Signature[1] != 'W') ||
        (Signature[2] != 'S')) {
        fprintf(stderr, "\"Not SWF File!!\"\n");
        fclose(file_stream);
        return FALSE;
    }
    
    fread (&SWFVersion, 1, 1, file_stream);
    DEBUGMSG(",\nVersion : %d", SWFVersion);
    
    fread (&FileLength, 1, 4, file_stream);
    DEBUGMSG(",\nFileLength : %d", FileLength);
    
    /*
    */
    
    if (Signature[0] != 'C') {
        
        _streamBuffer = (unsigned char *)(void *) malloc (FileLength);
        if (!_streamBuffer) {
            DEBUGMSG("failed to malloc()\n");
            fclose (file_stream);
            return FALSE;
        }
        
        rewind(file_stream);
        fread(_streamBuffer, 1, FileLength, file_stream);    // read whole file stream to memory stream.
        fclose(file_stream);
    } else {
        //Util util;
        unsigned int inflated_stream_size = 0;
        unsigned char *inflated_stream = 0;
        FileStream theFileStream(file_stream);
        
        // we can only know the real stream size after infalted.
        inflated_stream_size = inflate2Memory((Stream *)(&theFileStream), &inflated_stream);
        if (inflated_stream_size < 0) {
            DEBUGMSG("failed to inflate\n");
            fclose (file_stream);
            return FALSE;
        }
        
        _streamBuffer = (unsigned char *)(void *) malloc (inflated_stream_size + 8);
        rewind(file_stream);
        fread(_streamBuffer, 1, 8, file_stream); // read back the header
        memcpy((_streamBuffer + 8), inflated_stream, inflated_stream_size);
        free(inflated_stream);
        fclose(file_stream);
    }
    
    _stream_pos = 8; // continue to parse after header.
	return TRUE;
}

void SWFStream::dump(unsigned int numBytes)
{
	int i = 0;
	for  (i = 0; i < numBytes; i++) {
		DEBUGMSG("%02x ", _streamBuffer[_stream_pos+i]);
		if ((i+1) % 10 == 0)
			DEBUGMSG("\t");
		if ((i+1) % 20 == 0)
			DEBUGMSG("\n");
	}
	DEBUGMSG("\n\n");
}

//// Bit Operations

unsigned int SWFStream::getUBits(unsigned int numBits)
{
    int i = 0;
    unsigned int value = 0;
	
	// FIXME, numBits must <= 32
	
    if (!_bitOpFlag) {
        _bitOpFlag = 1;
        _bitBufferLen = 0; // reset _bitBuffer
    }
	
    // TODO: Not Optimized, I know one bit per round is stupid.
    while (i < numBits) {
		
        if (_bitBufferLen == 0) {
            _bitBuffer = _streamBuffer[_stream_pos];
            _bitBufferLen = 8;
            _stream_pos++;
        }
		
        value = (value << 1) | ((_bitBuffer >> --_bitBufferLen) & 0x01); // get MSb
        i++;
    }
    
    return value;
}

int SWFStream::getSBits(unsigned int numBits)
{
	int value = 0;
	
	value = getUBits(numBits);
	
	if (value & (1 << (numBits - 1)))
		value |=  (-1 << numBits); // sign extension
	
    return value;
}


//// Integer Operations

unsigned int SWFStream::getUI32()
{
    unsigned int value = 0;
	
    setByteAlignment();
	
    value = _streamBuffer[_stream_pos++];
    value = value | (_streamBuffer[_stream_pos++] << 8);
    value = value | (_streamBuffer[_stream_pos++] << 16);
    value = value | (_streamBuffer[_stream_pos++] << 24);
    return value;
}


unsigned int SWFStream::getUI16()
{
    unsigned int value = 0;
	
    setByteAlignment();
	
    value = _streamBuffer[_stream_pos++];
    value = value | (_streamBuffer[_stream_pos++] << 8);
    return value;
}

unsigned int SWFStream::getUI8()
{
    unsigned int value = 0;
	
    setByteAlignment();
	
    value = _streamBuffer[_stream_pos++];
    return value;
}


signed int SWFStream::getSI16()
{
	signed int value = 0;
	
	value = getUI16();
	
	if (value & (1 << 15))
		value |=  (-1 << 16); // sign extension
	
    return value;
}

signed int SWFStream::getSI32()
{
	signed int value = 0;
	
	value = *((signed int *)(_streamBuffer + _stream_pos));
    _stream_pos += 4;
	
    return value;
}

//// Fixed Number Operation

// check http://www.digitalsignallabs.com/fp.pdf (Fixed-Point Arithmetic)
float SWFStream::getFIXED8() // 16-bit 8.8 fixed-point number => A(7, 8)
{
	float value = 0;
    /*
	value = getUI8() / 0x100;
	value += getUI8();
    */
    value = getSI16() / (1 << 8);
	return value;
}

float SWFStream::getFIXED() // 32-bit 16.16 fixed-point number => A (15, 16)
{
	float value = 0;
    /*
	value = getUI16() / 0x10000;
	value += getUI16();
    */
    value = getSI32() / (1 << 16);
	return value;
}

//// Floating-point Number Operation
float SWFStream::getFLOAT16()   // Half-precision 16-bit floating-point
{
    union {
        float f;
        unsigned int i;
    } value;
    
    unsigned int tmp = getUI16();
        
    int sign        = (tmp >> 15) & 0x00000001; // bit16
    int exponent    = (tmp >> 10) & 0x0000001f; // bit15..bit11
    int mantissa    =  tmp        & 0x000003ff; // bit10..bit1
    
    if (exponent == 0)
    {
        if (mantissa == 0) { // Plus or minus zero
            return sign << 31;
        } else { // Denormalized number -- renormalize it
            while (!(mantissa & 0x00000400)) {
                mantissa <<= 1;
                exponent -=  1;
            }
            exponent += 1;
            mantissa &= ~0x00000400;
        }
    } else if (exponent == 31) {
        if (mantissa == 0) { // Inf
            return (sign << 31) | 0x7f800000;
        } else { // NaN
            return (sign << 31) | 0x7f800000 | (mantissa << 13);
        }
    }
    exponent = exponent + (127 - 15);
    mantissa = mantissa << 13;

    value.i = (sign << 31) | (exponent << 23) | mantissa;
    
	return value.f;
}

float SWFStream::getFLOAT()     // Single-pricesion 32-bit floating-point
{
    union {
        float f;
        unsigned int i;
    } value;
    
    value.i = getUI32();
	return value.f;
}

double SWFStream::getDOUBLE()   // Double-pricesion 64-bit floating-point
{
    union {
        double d;
        unsigned int i[2];
    } value;
    
    value.i[0] = getUI32();
    value.i[1] = getUI32();
	return value.d;
}

unsigned int SWFStream::getEncodedU32()
{
	unsigned int value = 0;
	value = getUI8();
	if (!(value & 0x80)) {
		return value;
	}
	value = (value & 0x7f) | (getUI8() << 7);
	if (!(value & 0x4000)) {
		return value;
	}
	value = (value & 0x3fff) | (getUI8() << 14);
	if (!(value & 0x200000)) {
		return value;
	}
	value = (value & 0x1fffff) | (getUI8() << 21);
	if (!(value & 0x10000000)) {
		return value;
	}
	value = (value & 0xfffffff) | (getUI8() << 28);
	return value;
}

//// String Operations

/// FIXME, SWF5 or earlier : ANSI/shift-JIS. Player should decode according the locale of the platform.
/// SWF6 or later : UTF8
char* SWFStream::getSTRING()
{
    char *str = (char *)(_streamBuffer + _stream_pos);
	DEBUGMSG("\"%s\"", _streamBuffer + _stream_pos);
	while (_streamBuffer[_stream_pos++]); // until get null
	return str;
}

unsigned int SWFStream::getLANGUAGECODE()
{
	// LANGCODE : UI8
	unsigned int LanguageCode;
	LanguageCode = getUI8();
	
	const char *langName[] = {
		"No Language depened on the platform locale",
		"Latin",
		"Japanese",
		"Korean",
		"Simplified Chinese",
		"Traditional Chinese"
	};
	
	DEBUGMSG("\"%s\"", langName[LanguageCode]);	
	return LanguageCode;
}
