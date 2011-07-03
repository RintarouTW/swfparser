/*
 *  debug.h
 *  swfparser
 *
 *  Created by Rintarou on 2010/6/3.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */
//test

#ifndef _DEBUG_H
#define _DEBUG_H

#undef DEBUG 

#ifdef DEBUG
#define DEBUGMSG    debugMsg
#else
#define DEBUGMSG    {}
#endif


#define NOT_IMPLMENETED {fprintf (stderr, "%s : %s # %d is not implemented\n", __FILE__, __func__, __LINE__);}

#define ASSERT(m)   {   if (m) { \
fprintf(stderr, "\n[ASSERT] %s:%s()#%d\n", __FILE__, __func__, __LINE__); \
abort(); \
} \
}

#endif // _DEBUG_H