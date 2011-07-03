/*
 *  common.h
 *  
 *
 *  Created by Rintarou on 2010/5/15.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */


#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

#include "debug.h"
#include "VObject.h"

#ifndef _COMMON_H
#define _COMMON_H

///////////////// Types //////////////////
#define TRUE    1
#define FALSE   0
//////////////////////////////////////////

// see http://en.wikipedia.org/wiki/Q_(number_format)
#define FIXED2FLOAT(a)      (((float)(a)) / 65536);   // FIXED is a 16.16 fixed-point number 65536 = (2 << 16)
#define FIXED8TOFLOAT(a)    (((float)(a)) / 256);     // FIXED is a 8.8 fixed-point number     256 = (2 << 8)

void debugMsg( const char* fmt, ... );  // implemented in Util.cpp


struct Tag {	// RECORDHEADER
	unsigned int TagCode;
	unsigned int TagLength;
    unsigned int TagHeaderOffset;
    unsigned int TagHeaderLength;
    unsigned int TagBodyOffset;
    unsigned int NextTagPos;  // Internal used only, for debugging
};

struct Action { // ACTIONRECORD
    unsigned int ActionCode;
    unsigned int Length;
    unsigned int ActionHeaderOffset;
    unsigned int ActionHeaderLength;
    unsigned int ActionBodyOffset;
    unsigned int NextActionPos; // Internal used only, for debugging
};

#endif // _COMMON_H