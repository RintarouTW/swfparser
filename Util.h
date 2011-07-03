/*
 *  Util.h
 *  
 *
 *  Created by Rintarou on 2010/5/15.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "Stream.h"

#ifndef _UTIL_H
#define _UTIL_H

int inflate2Memory (Stream *stream, unsigned char **output_ptr);

char *Color2String(unsigned int rgba, int hasAlpha);
                 
#endif // _UTIL_H