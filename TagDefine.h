/*
 *  TagDefine.h
 *  swfparser
 *
 *  Created by Rintarou on 2010/5/27.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#ifndef _TAGDEFINE_H
#define _TAGDEFINE_H

#define TAG_END             0
#define TAG_SHOWFRAME       1
#define TAG_DEFINESHAPE     2

#define TAG_PLACEOBJECT     4
#define TAG_REMOVEOBJECT    5
#define TAG_DEFINEBITS      6
#define TAG_DEFINEBUTTON    7
#define TAG_JPEGTABLES      8
#define TAG_SETBACKGROUND   9
#define TAG_DEFINEFONT      10
#define TAG_DEFINETEXT      11
#define TAG_DOACTION        12
#define TAG_DEFINEFONTINFO  13
#define TAG_DEFINESOUND     14
#define TAG_STARTSOUND      15

#define TAG_DEFINEBUTTONSOUND   17
#define TAG_SOUNDSTREAMHEAD     18
#define TAG_SOUNDSTREAMBLOCK    19
#define TAG_DEFINEBITSLOSSLESS  20
#define TAG_DEFINEBITSJPEG2     21
#define TAG_DEFINESHAPE2        22
#define TAG_DEFINEBUTTONCXFORM  23
#define TAG_PROTECT             24

#define TAG_PLACEOBJECT2        26

#define TAG_REMOVEOBJECT2       28

#define TAG_DEFINESHAPE3        32
#define TAG_DEFINETEXT2         33
#define TAG_DEFINEBUTTON2       34
#define TAG_DEFINEBITSJPEG3     35
#define TAG_DEFINEBITSLOSSLESS2 36
#define TAG_DEFINEEDITTEXT      37

#define TAG_DEFINESPRITE        39

#define TAG_FRAMELABEL          43

#define TAG_SOUNDSTREAMHEAD2    45
#define TAG_DEFINEMORPHSHAPE    46

#define TAG_DEFINEFONT2         48

#define TAG_EXPORTASSETS        56
#define TAG_IMPORTASSETS        57
#define TAG_ENABLEDEBUGGER      58
#define TAG_DOINITACTION        59
#define TAG_DEFINEVIDEOSTREAM   60
#define TAG_VIDEOFRAME          61
#define TAG_DEFINEFONTINFO2     62

#define TAG_ENABLEDEBUGGER2     64
#define TAG_SCRIPTLIMITS        65
#define TAG_SETTABINDEX         66

#define TAG_FILEATTRIBUTES      69
#define TAG_PLACEOBJECT3        70
#define TAG_IMPORTASSETS2       71

#define TAG_DEFINEFONTALIGNZONES    73
#define TAG_CSMTEXTSETTINGS         74
#define TAG_DEFINEFONT3             75
#define TAG_SYMBOLCLASS             76
#define TAG_METADATA                77
#define TAG_DEFINESCALINGGRID       78

#define TAG_DOABC                   82
#define TAG_DEFINESHAPE4            83
#define TAG_DEFINEMORPHSHAPE2       84

#define TAG_DEFINESCENEANDFRAMELABELDATA    86
#define TAG_DEFINEBINARYDATA                87
#define TAG_DEFINEFONTNAME                  88
#define TAG_STARTSOUND2                     89
#define TAG_DEFINEBITSJPEG4                 90
#define TAG_DEFINEFONT4                     91

#define TAGCODE_MAX                         91  // not a swf tag, used for range checking

#endif /* _TAGDEFINE_H */
