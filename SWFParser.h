/*
 *  SWFParser.h
 *
 *
 *  Created by Rintarou on 2010/5/16.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "common.h"
#include "SWFStream.h"


#ifndef _SWFPARSER_H
#define _SWFPARSER_H

// return false if want to stop parsing, give the caller a chance to stop the parsing loop.
typedef int (*ProgressUpdateFunctionPtr)(unsigned int progress);

class SWFParser	: public SWFStream {
public:
    SWFParser();
    ~SWFParser();
    
    //// User Operation
    VObject *parse(const char *filename);
    VObject *parseWithCallback(const char *filename, ProgressUpdateFunctionPtr progressUpdate);
    
	//// Color Operations
	unsigned int	getRGB();
	unsigned int	getARGB() { return getUI32(); }
	unsigned int	getRGBA() { return getUI32(); }
	
	//// Gradient Operation
    int             getGRADIENT(Tag *tag, VObject &gradientObject);
    int             getFOCALGRADIENT(Tag *tag, VObject &focalObject);
	
	//// Rectangle Operations	
	int             getRECT(VObject &rectObject);
	
	//// Matrix Operation
    int             getMATRIX(VObject &matrixObject);
	
	//// Transformation
	int             getCXFORM(VObject &cxObject);
	int             getCXFORMWITHALPHA(VObject &cxObject);
	
	//// Fill & Line Styles
    int             getFILLSTYLE(Tag *tag, VObject &fillStyleObject);
	int             getFILLSTYLEARRAY(Tag *tag, VObject &fillStyleArrayObject);
	int             getLINESTYLEARRAY(Tag *tag, VObject &lineStyleArrayObject);
    
    //// Morph Fill & Line Styles
    int             getMORPHGRADIENT(Tag *tag, VObject &gradientObject);
    int             getMORPHFILLSTYLE(Tag *tag, VObject &fillStyleObject);
	int             getMORPHFILLSTYLEARRAY(Tag *tag, VObject &fillStyleArrayObject);
    int             getMORPHLINESTYLE(Tag *tag, VObject &lineStyleObject);
	int             getMORPHLINESTYLEARRAY(Tag *tag, VObject &lineStyleArrayObject);

	//// Shape Operations
	int             getSHAPE(Tag *tag, VObject &shapeObject);
	int             getSHAPEWITHSTYLE(Tag *tag, VObject &styleObject);
	
    //// TEXTRECORD
    int             getTEXTRECORD(Tag *tag, unsigned int GlyphBits, unsigned int AdvanceBits);
    
    //// BUTTONRECORD
    int             getBUTTONRECORD(Tag *tag, VObject &btnRecordObject);
    int             getBUTTONCONDACTION(VObject &btnCondObject);
    
	//// ACTIONRECORD	
	int             getACTIONRECORD(VObject &actRecordObject);
    
    //// Action Operations
    int             getActionCodeAndLength(Action *action);
	
	//// CLIPACTIONS and CLIPACTIONRECORD
	#define CLIPEVENT_KEYPRESS_MASK (1<<22) // 0x00400000
	#define CLIPEVENT_DRAGOUT_MASK  (1<<23) // 0x00800000
		
	unsigned int    getCLIPEVENTFLAGS();	// FIXME
	unsigned int    getCLIPACTIONRECORD(VObject &clipActionRecordObject);
	int             getCLIPACTIONS(VObject &clipActionsObject);
    //// Filters
    int             getFILTERLIST(VObject &filterlistObject);    // SWF8 or later
	
    //// SOUNDINFO
    int             getSOUNDINFO(VObject &soundInfoObject);
    
    
	//// Tag Operations
	int             getTagCodeAndLength(Tag *tag);

private:
    
    //// Internally used Filter getters
    int             getCOLORMATRIXFILTER(VObject &filterObject);
    int             getCONVOLUTIONFILTER(VObject &filterObject);
    int             getBLURFILTER(VObject &filterObject);
    int             getDROPSHADOWFILTER(VObject &filterObject);
    int             getGLOWFILTER(VObject &filterObject);
    int             getBEVELFILTER(VObject &filterObject);
    int             getGRADIENTGLOWFILTER(VObject &filterObject);
    int             getGRADIENTBEVELFILTER(VObject &filterObject);
    
    VObject         root;
};

#endif