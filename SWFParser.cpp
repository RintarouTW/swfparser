/*
 *  SWFParser.cpp
 *
 *
 *  Created by Rintarou on 2010/5/16.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "common.h"
#include "SWFParser.h"
#include "TagDefine.h"
#include "TagHandler.h"
#include "Util.h"

SWFParser::SWFParser()
{
    root.resetCounters();
}


SWFParser::~SWFParser()
{
    root.resetCounters();
}

VObject *SWFParser::parse(const char *filename)
{
    return parseWithCallback(filename, NULL);
}

VObject *SWFParser::parseWithCallback(const char *filename, ProgressUpdateFunctionPtr progressUpdate)
{
    if (!filename) {
        return NULL;
    }
    if (!open(filename)) {
        return NULL;
    }
        
    root.setTypeInfo(".swf");
        
    // SWF Header
    VObject &swfHeader = root["Header"];
    swfHeader.setTypeInfo("SWFHeader");
    swfHeader["Signature"]   = Signature;
    swfHeader["Version"]     = SWFVersion;
    swfHeader["FileLength"]  = FileLength;
    getRECT(swfHeader["FrameSize"]);
    
    float FrameRate = 0.0;
    
    FrameRate = getUI16();
    FrameRate = FIXED8TOFLOAT(FrameRate); // Fixed 8.8 ignore the part
    DEBUGMSG(",\nFrameRate : %f", FrameRate);
    
    unsigned int FrameCount = 0;
    
    FrameCount = getUI16();
    DEBUGMSG(",\nFrameCount : %d", FrameCount);
    
    swfHeader["FrameRate"]   = FrameRate;
    swfHeader["FrameCount"]  = FrameCount;
    
    // Parse TagCodeAndLength (RECORDHEADER)
    unsigned int TagCode = 0, TagLength = 0;
    
    TagHandler tagHandler;
    unsigned int tagNo = 0;
    DEBUGMSG(",\nTags : [\n");
    
    VObject &tags = root["Tags"];
    do {
        Tag tag;
        
        getTagCodeAndLength(&tag);
        TagCode		= tag.TagCode;
        TagLength	= tag.TagLength;
        
        tags[tagNo]["TagCode"]      = TagCode;
        tags[tagNo]["TagLength"]    = TagLength;
        
        if (progressUpdate) {            
            if (!progressUpdate((tag.TagBodyOffset * 100) / FileLength)) {
                // the caller want to stop the parsing.
                break;
            }
        }
        
#ifndef F2C
        DEBUGMSG("{\n");
        tagHandler.parse(&tag, this, tags[tagNo]);
        
        DEBUGMSG("}%c\n", TagCode ? ',' : ' '); // End of Tag
        tagNo++;
#else // F2C parse DefineShape/2/3/4 Tags only
        if ((TagCode == TAG_DEFINESHAPE) || 
            (TagCode == TAG_DEFINESHAPE2) || 
            (TagCode == TAG_DEFINESHAPE3) || 
            (TagCode == TAG_DEFINESHAPE4) ) 
        {
            tagHandler.parse(&tag, this, tags[tagNo]);
        } else {
            seek(tag.NextTagPos);
        }
        
        tagNo++;
#endif // F2C
        
    } while ( TagCode != 0x0); /* Parse untile END Tag(0x0) */
    
    DEBUGMSG("]\n}\n"); // End of SWFStream
    
	if (getStreamPos() != getFileLength()) {
		DEBUGMSG("Fatal Error, not complete parsing, pos = %d, file length = %d\n", getStreamPos(), getFileLength());
        return NULL;
    }
    return &root;
}

///////////////////////////////////////
//// Color Operations
///////////////////////////////////////
unsigned int SWFParser::getRGB()
{
    unsigned int value = 0;

    setByteAlignment();
	
    value = getUI8();                   // R
    value = value | (getUI8() << 8);    // G
    value = value | (getUI8() << 16);   // B
    
    // value = 0xBBGGRR
    return value;
}

///////////////////////////////////////
//// Gradient Operation
///////////////////////////////////////
int SWFParser::getGRADIENT(Tag *tag, VObject &gradientObject)
{   
	unsigned int SpreadMode, InterpolationMode, NumGradients, i;

    gradientObject.setTypeInfo("GRADIENT");
    DEBUGMSG("{ /* GRADIENT */\n");
    
	setByteAlignment();
    
	SpreadMode = getUBits(2);           // 0 = Pad mode, 1 = Reflect mode, 2 = Repeat mode, 3 = Reserved
	InterpolationMode = getUBits(2);    // 0 = Nomral RGB mode, 1 = Linear RGB mode, 2 and 3 = Reserved
	NumGradients = getUBits(4); // 1 to 15
    
    gradientObject["SpreadMode"] = SpreadMode;
    gradientObject["InterpolationMode"] = InterpolationMode;
    gradientObject["NumGradients"] = NumGradients;
    DEBUGMSG("SpreadMode : %d,\nInterpolationMode : %d,\nNumGradients : %d", SpreadMode, InterpolationMode, NumGradients);

    if (NumGradients) {
        DEBUGMSG(",\nGradientRecords : [\n");
        
        for (i = 0; i < NumGradients; i++) {
            // GRADRECORD
            VObject &gradientRecord = gradientObject["GradientRecords"][i];
            gradientRecord.setTypeInfo("GRADRECORD");
            
            unsigned int Ratio, Color;
            
            Ratio = getUI8();
            gradientRecord["Ratio"] = Ratio;

            if ((tag->TagCode == TAG_DEFINESHAPE) || (tag->TagCode == TAG_DEFINESHAPE2)) {
                Color = getRGB(); // for DefineShape and DefineShape2
                DEBUGMSG("{ Ratio : %d, Color : \"%#03x\"}", Ratio, Color);
                gradientRecord["Color"] = Color2String(Color, 0);
            } else {
                Color = getRGBA(); // for DefineShape3 or later?
                DEBUGMSG("{ Ratio : %d, Color : \"%#04x\"}", Ratio, Color);
                gradientRecord["Color"] = Color2String(Color, 1);
            }
            
            
            if (i < (NumGradients - 1))
                DEBUGMSG(",\n");
        }
        DEBUGMSG("\n]");
    }
    DEBUGMSG("\n}");
	return TRUE;
}

int SWFParser::getFOCALGRADIENT(Tag *tag, VObject &focalObject)
{
    DEBUGMSG("{ /* FOCALGRADIENT */\n");
    focalObject.setTypeInfo("FOCALGRADIENT");
    
    getGRADIENT(tag, focalObject);
    
    float FocalPoint;
    
    FocalPoint = getFIXED8();    // Focal point location
    
    focalObject["FocalPoint"] = FocalPoint;
    DEBUGMSG(",\nFocalPoint : %f\n}", FocalPoint);
	return TRUE;
}

///////////////////////////////////////
//// Rectangle Operations
///////////////////////////////////////
int SWFParser::getRECT(VObject &rectObject)
{
    rectObject.setTypeInfo("RECT");
    
	unsigned int Nbits = 0;
	signed int Xmin, Xmax, Ymin, Ymax;

	setByteAlignment();
    
	Nbits	= getUBits(5);
	Xmin	= getSBits(Nbits);
	Xmax	= getSBits(Nbits);
	Ymin	= getSBits(Nbits);
	Ymax	= getSBits(Nbits);
    
    rectObject["Nbits"] = Nbits;
	rectObject["Xmin"] = Xmin;
	rectObject["Xmax"] = Xmax;
	rectObject["Ymin"] = Ymin;
	rectObject["Ymax"] = Ymax;
	DEBUGMSG("{\nXmin : %d,\nXmax : %d,\nYmin : %d,\nYmax : %d\n}", Xmin, Xmax, Ymin, Ymax);
	
	return TRUE;
}

///////////////////////////////////////
//// Matrix Operation
///////////////////////////////////////
int SWFParser::getMATRIX(VObject &matrixObject)
{
    matrixObject.setTypeInfo("MATRIX");
    
	unsigned int HasScale, NScaleBits, HasRotate, NRotateBits, NTranslateBits;
	float ScaleX, ScaleY, RotateSkew0, RotateSkew1; // FIXME : These should be FIXED values
	signed int TranslateX, TranslateY;
	
	setByteAlignment(); // MATRIX Record must be byte aligned.
    
	DEBUGMSG("{ /* MATRIX */\n");
	
    
	HasScale = getUBits(1);
    matrixObject["HasScale"] = HasScale;
    DEBUGMSG("HasScale : %d", HasScale);
	if (HasScale) {
		NScaleBits = getUBits(5);
		ScaleX = FIXED2FLOAT(getSBits(NScaleBits));
		ScaleY = FIXED2FLOAT(getSBits(NScaleBits));
        matrixObject["NScaleBits"] = NScaleBits;
        matrixObject["ScaleX"] = ScaleX;
        matrixObject["ScaleY"] = ScaleY;
		DEBUGMSG(",\nNScaleBits : %d,\nScaleX : %f,\nScaleY : %f", NScaleBits, ScaleX, ScaleY);
	}
	
	HasRotate = getUBits(1);
    matrixObject["HasRotate"] = HasRotate;
    DEBUGMSG(",\nHasRotate : %d", HasRotate);
	if (HasRotate) {
		NRotateBits = getUBits(5);
		RotateSkew0 = FIXED2FLOAT(getSBits(NRotateBits));
		RotateSkew1 = FIXED2FLOAT(getSBits(NRotateBits));
        matrixObject["NRotateBits"] = NRotateBits;
        matrixObject["RotateSkew0"] = RotateSkew0;
        matrixObject["RotateSkew1"] = RotateSkew1;
		DEBUGMSG(",\nNRotateBits : %d,\nRotateSkew0 : %d,\nRotateSkew1 : %d", NRotateBits, RotateSkew0, RotateSkew1);
	}
	
	NTranslateBits = getUBits(5);
	TranslateX = getSBits(NTranslateBits);
	TranslateY = getSBits(NTranslateBits);
    matrixObject["NTranslateBits"] = NTranslateBits;
    matrixObject["TranslateX"] = TranslateX;
    matrixObject["TranslateY"] = TranslateY;
	DEBUGMSG(",\nNTranslateBits : %d,\nTranslateX : %d,\nTranslateY : %d", NTranslateBits, TranslateX, TranslateY);
	
    DEBUGMSG("\n}");
	return TRUE;
}

///////////////////////////////////////
//// Color Transformation
///////////////////////////////////////
int SWFParser::getCXFORM(VObject &cxObject)
{
	unsigned int HasAddTerms, HasMultTerms, Nbits;
	signed int RedMultTerm, GreenMultTerm, BlueMultTerm, RedAddTerm, GreenAddTerm, BlueAddTerm;
	
	setByteAlignment(); // CXFORM Record must be byte aligned.
	
	DEBUGMSG("{\n");
	cxObject.setTypeInfo("CXFORM");
    
	HasAddTerms		= getUBits(1);
	HasMultTerms	= getUBits(1);
	Nbits			= getUBits(4);
    
    cxObject["HasAddTerms"]     = HasAddTerms;
    cxObject["HasMultTerms"]    = HasMultTerms;
    cxObject["Nbits"]           = Nbits;
	DEBUGMSG("HasAddTerms : %d,\nHasMultTerms : %d,\nNbits : %d", Nbits, HasAddTerms, HasMultTerms);
	
	if (HasMultTerms) {
		RedMultTerm		= getSBits(Nbits);  // in swf file, it's still a SB
        GreenMultTerm	= getSBits(Nbits);
		BlueMultTerm	= getSBits(Nbits);
        cxObject["RedMultTerm"]     = FIXED8TOFLOAT(RedMultTerm);   // convert it to FIXED8 to be meaningful for the user.
        cxObject["GreenMultTerm"]   = FIXED8TOFLOAT(GreenMultTerm);
        cxObject["BlueMultTerm"]    = FIXED8TOFLOAT(BlueMultTerm);
		DEBUGMSG(",\nRedMultTerm : %f,\nGreenMultTerm : %f,\nBlueMultTerm : %f", RedMultTerm, GreenMultTerm, BlueMultTerm);
	}
	
	if (HasAddTerms) {
		RedAddTerm		= getSBits(Nbits);
        GreenAddTerm	= getSBits(Nbits);
		BlueAddTerm		= getSBits(Nbits);
        cxObject["RedAddTerm"] = RedAddTerm;
        cxObject["GreenAddTerm"] = GreenAddTerm;
        cxObject["BlueAddTerm"] = BlueAddTerm;
		DEBUGMSG(",\nRedAddTerm : %d,\nGreenAddTerm : %d,\nBlueAddTerm : %d", RedAddTerm, GreenAddTerm, BlueAddTerm);
	}
	DEBUGMSG("\n}");
	
	return TRUE;
}

int SWFParser::getCXFORMWITHALPHA(VObject &cxObject)
{
	unsigned int HasAddTerms, HasMultTerms, Nbits;
	signed int RedMultTerm, GreenMultTerm, BlueMultTerm, AlphaMultTerm, RedAddTerm, GreenAddTerm, BlueAddTerm, AlphaAddTerm;
	
	setByteAlignment(); // CXFORM Record must be byte aligned.

	DEBUGMSG("{\n");
	cxObject.setTypeInfo("CXFORMWITHALPHA");
    
	HasAddTerms		= getUBits(1);
	HasMultTerms	= getUBits(1);
	Nbits = getUBits(4);

    cxObject["HasAddTerms"]     = HasAddTerms;
    cxObject["HasMultTerms"]    = HasMultTerms;
    cxObject["Nbits"]           = Nbits;
    
	DEBUGMSG("HasAddTerms : %d,\nHasMultTerms : %d,\nNbits : %d", HasAddTerms, HasMultTerms, Nbits);
	
	if (HasMultTerms) {
		RedMultTerm		= getSBits(Nbits);
        GreenMultTerm	= getSBits(Nbits);
		BlueMultTerm	= getSBits(Nbits);
		AlphaMultTerm	= getSBits(Nbits);
        
        cxObject["RedMultTerm"]     = FIXED8TOFLOAT(RedMultTerm);
        cxObject["GreenMultTerm"]   = FIXED8TOFLOAT(GreenMultTerm);
        cxObject["BlueMultTerm"]    = FIXED8TOFLOAT(BlueMultTerm);
        cxObject["AlphaMultTerm"]   = FIXED8TOFLOAT(AlphaMultTerm);
        
        DEBUGMSG(",\nRedMultTerm : %d,\nGreenMultTerm : %d,\nBlueMultTerm : %d,\nAlphaMultTerm : %d", RedMultTerm, GreenMultTerm, BlueMultTerm, AlphaMultTerm);
	}
	
	if (HasAddTerms) {
		RedAddTerm		= getSBits(Nbits);
        GreenAddTerm	= getSBits(Nbits);
		BlueAddTerm		= getSBits(Nbits);
		AlphaAddTerm	= getSBits(Nbits);

        cxObject["RedAddTerm"] = RedAddTerm;
        cxObject["GreenAddTerm"] = GreenAddTerm;
        cxObject["BlueAddTerm"] = BlueAddTerm;
        cxObject["AlphaAddTerm"] = AlphaAddTerm;
        
        DEBUGMSG(",\nRedAddTerm : %d,\nGreenAddTerm : %d,\nBlueAddTerm : %d,\nAlphaAddTerm : %d", RedAddTerm, GreenAddTerm, BlueAddTerm, AlphaAddTerm);
	}
	
    DEBUGMSG("\n}");
	
	return TRUE;
}


///////////////////////////////////////
//// Fill Style & Line Style
///////////////////////////////////////
//// FillStyleType :
//// 0x00 = solid fill
//// 0x10 = linear gradient fill
//// 0x12 = radial gradient fill
//// 0x13 = focal radial gradient fill (SWF8 or later)
//// 0x40 = repeating bitmap fill
//// 0x41 = clipped bitmap fill
//// 0x42 = non-smoothed repeating bitmap
//// 0x43 = non-smoothed clipped bitmap
int SWFParser::getFILLSTYLE(Tag *tag, VObject &fillStyleObject)
{
    // FILLSTYLE
    unsigned int FillStyleType, Color;
    
    FillStyleType = getUI8();
    fillStyleObject["FillStyleType"] = FillStyleType;
    DEBUGMSG("{\nFillStyleType : %d ", FillStyleType);

    /*
    ASSERT ( !((FillStyleType == 0x00) ||
               (FillStyleType == 0x10) ||
               (FillStyleType == 0x11) ||
               (FillStyleType == 0x12) ||
               (FillStyleType == 0x13) ||
               (FillStyleType == 0x40) ||
               (FillStyleType == 0x41) ||
               (FillStyleType == 0x42) ||
               (FillStyleType == 0x43) ) );

    if (FillStyleType & 0x10) { // Gradient Fill
        DEBUGMSG("Gradient Fill {\n");
        getMATRIX(); // GradientMatrix
        if ((FillStyleType == 0x10) ||
            (FillStyleType == 0x12)) {
            getGRADIENT(tag);
        } else { // 0x13
            getFOCALGRADIENT(tag); // SWF8 or later
        }
        DEBUGMSG("} // Gradient Fill\n");
        
    } else if ( FillStyleType & 0x40 ) { // Bitmap Fill
        
        unsigned int BitmapId;
        
        BitmapId = getUI16();
        DEBUGMSG("Bitmap Fill {\nBitmapId = %d\n", BitmapId);
        
        getMATRIX(); // Matrix for bitmap fill
        DEBUGMSG("} // Bitmap Fill\n");
        
    } else  {
        
        if ((tag->TagCode == 2) || (tag->TagCode == 22))
            Color = getRGB();   // DefineShape/DefineShape2
        else
            Color = getRGBA();  // DefineShape3 or 4?            
        
        DEBUGMSG("Solid Fill, Color = %x\n", Color);        
    }
    */
    
    switch (FillStyleType) {
        case 0x00:
            fillStyleObject.setTypeInfo("FILLSTYLE (Solid Fill)");
            break;
        case 0x10:
            fillStyleObject.setTypeInfo("FILLSTYLE (Linear Gradient Fill)");
            break;
        case 0x12:
            fillStyleObject.setTypeInfo("FILLSTYLE (Radial Gradient Fill)");
            break;
        case 0x13:
            fillStyleObject.setTypeInfo("FILLSTYLE (Focal Gradient Fill)");
            break;
        case 0x40:
            fillStyleObject.setTypeInfo("FILLSTYLE (Repeating Bitmap Fill)");
            break;
        case 0x41:
            fillStyleObject.setTypeInfo("FILLSTYLE (Clipped Bitmap Fill)");
            break;
        case 0x42:
            fillStyleObject.setTypeInfo("FILLSTYLE (Non-smoothed Repeating Bitmap)");
            break;
        case 0x43:
            fillStyleObject.setTypeInfo("FILLSTYLE (Non-smoothed Clipped Bitmap)");
            break;
        default:
            ASSERT(1);  // undefined FILLSTYLE type
            break;
    }

    
    if (FillStyleType == 0x00) { // Solid Color Fill
        
        //fillStyleObject["FillStyleType"].setComment("Solid Fill");
        
        
        if ((tag->TagCode == TAG_DEFINESHAPE) || (tag->TagCode == TAG_DEFINESHAPE2)) {
            Color = getRGB();   // DefineShape/DefineShape2
            fillStyleObject["Color"] = Color2String(Color, 0);
        } else {
            Color = getRGBA();  // DefineShape3 or 4? 
            fillStyleObject["Color"] = Color2String(Color, 1);
        }
        
        DEBUGMSG(", /* Solid Fill */\nColor : \"%#04x\"", Color);
    }
    
    if ((FillStyleType == 0x10) ||
        (FillStyleType == 0x12) ||
        (FillStyleType == 0x13)) { // Gradient Fill
                
        DEBUGMSG(", /* Gradient Fill */");
        //fillStyleObject["FillStyleType"].setComment("Gradient Fill");
        
        
        DEBUGMSG("\nGradientMatrix : ");
        getMATRIX(fillStyleObject["GradientMatrix"]); // GradientMatrix
        
        DEBUGMSG(",\nGradient : ");
        if ((FillStyleType == 0x10) ||
            (FillStyleType == 0x12)) {
            getGRADIENT(tag, fillStyleObject["Gradient"]);
        } else { // 0x13
            getFOCALGRADIENT(tag, fillStyleObject["Gradient"]); // SWF8 or later
        }
        //DEBUGMSG("} // Gradient Fill\n");
    }
    
    if ((FillStyleType == 0x40) ||
        (FillStyleType == 0x41) ||
        (FillStyleType == 0x42) ||
        (FillStyleType == 0x43)) { // Bitmap Fill
        
        //fillStyleObject["FillStyleType"].setComment("Bitmap Fill");
        
        unsigned int BitmapId;
        
        BitmapId = getUI16();
        fillStyleObject["BitmapID"] = BitmapId;
        DEBUGMSG(", /* Bitmap Fill */\nBitmapId : %d,\nBitmapMatrix : ", BitmapId);
        getMATRIX(fillStyleObject["BitmapMatrix"]); // Matrix for bitmap fill
        //DEBUGMSG("} // Bitmap Fill\n");
    }

    DEBUGMSG("\n}");
    return TRUE;
}

int SWFParser::getFILLSTYLEARRAY(Tag *tag, VObject &fillStyleArrayObject)
{
    fillStyleArrayObject.setTypeInfo("FILLSTYLEARRAY");
	unsigned int FillStyleCount, i;

	FillStyleCount = getUI8();
	if (FillStyleCount == 0xff)
		FillStyleCount = getUI16();
    
    fillStyleArrayObject["FillStyleCount"] = FillStyleCount;
	DEBUGMSG("{ /* FILLSTYLEARRAY */\nFillStyleCount : %d", FillStyleCount);
	
    if (FillStyleCount) {
        DEBUGMSG(",\nFillStyles : [\n");
        // read FillStyles : FILLSTYLE[]
        for (i = 0; i < FillStyleCount; i++) {
            getFILLSTYLE(tag, fillStyleArrayObject["FillStyles"][i]);
            if (i < (FillStyleCount - 1))
                DEBUGMSG(",\n");
        }
    
        DEBUGMSG("\n]");
    }
    
    DEBUGMSG("\n}");
	
	return TRUE;
}



int SWFParser::getLINESTYLEARRAY(Tag *tag, VObject &lineStyleArrayObject)
{
	unsigned int LineStyleCount, i;
	
    lineStyleArrayObject.setTypeInfo("LINESTYLEARRAY");
    
	LineStyleCount = getUI8();
	if (LineStyleCount == 0xff)
		LineStyleCount = getUI16();
	
    lineStyleArrayObject["LineStyleCount"] = LineStyleCount;
    DEBUGMSG("{ /* LINESTYLEARRAY */\nLineStyleCount : %d", LineStyleCount);
    
    if (!LineStyleCount) {
        DEBUGMSG("\n}");
        return TRUE;
    }
	
    DEBUGMSG(",\nLineStyles : [\n");
    VObject &lineStyles = lineStyleArrayObject["LineStyles"];
	// LineStyles
    // DefineShape1/2/3 => LINESTYLE[]
    // DefineShape4     => LINESTYLE2[]
    if (tag->TagCode == TAG_DEFINESHAPE4) {   // DefineShape4 => LINESTYLE2[]

        for (i = 0; i < LineStyleCount; i++) { // Index start from 1
            // LINESTYLE2
            lineStyles[i].setTypeInfo("LINESTYLE2");
            unsigned int Width, StartCapStyle, JoinStyle, HasFillFlag, NoHScaleFlag, NoVScaleFlag, PixelHintingFlag, NoClose, EndCapStyle, MiterLimitFactor, Color;
            
            
            Width           = getUI16();
            lineStyles[i]["Width"] = Width;
            DEBUGMSG("{\nWidth : %d", Width);
            
            StartCapStyle   = getUBits(2); // 0 = Round cap, 1 = No cap, 2 = Square cap
            lineStyles[i]["StartCapStyle"] = StartCapStyle;
            DEBUGMSG(",\nStartCapStyle : %d", StartCapStyle);
            
            JoinStyle       = getUBits(2); // 0 = Round join, 1 = Bevel join, 2 = Miter join
            lineStyles[i]["JoinStyle"] = JoinStyle;
            DEBUGMSG(",\nJoinStyle : %d", JoinStyle);
            
            HasFillFlag     = getUBits(1);
            lineStyles[i]["HasFillFlag"] = HasFillFlag;
            DEBUGMSG(",\nHasFillFlag : %d", HasFillFlag);
            
            NoHScaleFlag    = getUBits(1);
            lineStyles[i]["NoHScaleFlag"] = NoHScaleFlag;
            DEBUGMSG(",\nNoHScaleFlag : %d", NoHScaleFlag);
            
            NoVScaleFlag    = getUBits(1);
            lineStyles[i]["NoVScaleFlag"] = NoVScaleFlag;
            DEBUGMSG(",\nNoVScaleFlag : %d", NoVScaleFlag);
            
            PixelHintingFlag = getUBits(1);
            lineStyles[i]["PixelHintingFlag"] = PixelHintingFlag;
            DEBUGMSG(",\nPixelHintingFlag : %d", PixelHintingFlag);
            
            getUBits(5); // Reserved must be 0
            
            NoClose         = getUBits(1);
            lineStyles[i]["NoClose"] = NoClose;
            DEBUGMSG(",\nNoClose : %d", NoClose);
            
            EndCapStyle     = getUBits(2);
            lineStyles[i]["EndCapStyle"] = EndCapStyle;
            DEBUGMSG(",\nEndCapStyle : %d", EndCapStyle);
                        
            if (JoinStyle == 2) {
                MiterLimitFactor = getUI16();   // Miter limit factor is an 8.8 fixed-point value.
                lineStyles[i]["MiterLimitFactor"] = FIXED8TOFLOAT(MiterLimitFactor);    // convert it to be meaningful to the user.
                DEBUGMSG(",\nMiterLimitFactor : %f", MiterLimitFactor);
            }
            
            if (HasFillFlag == 0) {
                Color = getRGBA();          // Color
                lineStyles[i]["Color"] = Color2String(Color, 1);
                DEBUGMSG(",\nColor : \"%#04x\"", Color);
            } else {
                DEBUGMSG(",\nFillType : ");
                getFILLSTYLE(tag, lineStyles[i]["FillTyle"]);  // FillType : FILLSTYLE
            }
            
            DEBUGMSG("\n}");
            if (i < (LineStyleCount - 1))
                DEBUGMSG(",\n");
        }
        
    } else { // DefineShape1/2/3 => LINESTYLE[]
        
        
        for (i = 0; i < LineStyleCount; i++) {
            
            lineStyles[i].setTypeInfo("LINESTYLE");
            
            unsigned int Width, Color = 0;
            
            Width = getUI16();
            lineStyles[i]["Width"] = Width;
            
            if (tag->TagCode == TAG_DEFINESHAPE3) { // DefineShape3
                Color = getRGBA();
                DEBUGMSG("{ Width : %d, Color : \"%#04x\" }", Width, Color);
                lineStyles[i]["Color"] = Color2String(Color, 1);
            } else {
                Color = getRGB();   // DefineShape1/2
                DEBUGMSG("{ Width : %d, Color : \"%#03x\" }", Width, Color);
                lineStyles[i]["Color"] = Color2String(Color, 0);
            }
            
            if (i < (LineStyleCount - 1))
                DEBUGMSG(",\n");
        }
	}
    
	DEBUGMSG("\n]\n}");
	
	return TRUE;
}


///////////////////////////////////////
//// Shape Operations
///////////////////////////////////////

int SWFParser::getSHAPE(Tag *tag, VObject &shapeObject)
{
	unsigned int NumFillBits = 0, NumLineBits = 0;// ShapeRecordNo = 0;
	
	setByteAlignment(); // reset bit buffer for byte-alignment
	
	NumFillBits = getUBits(4); // NumFillBits
	NumLineBits = getUBits(4); // NumLineBits
    
    shapeObject.setTypeInfo("SHAPE");
    shapeObject["NumFillBits"] = NumFillBits;
    shapeObject["NumLineBits"] = NumLineBits;
	DEBUGMSG("{ /* SHAPE */\nNumFillBits : %d,\nNumLineBits : %d", NumFillBits, NumLineBits);
	
    DEBUGMSG(",\nShapeRecords : [\n");
	//// ShapeRecords : SHAPERECORD[]
    
    VObject &shapeRecordsObject = shapeObject["ShapeRecords"];
    unsigned int recNo = 0;
	while(1) {
        //DEBUGMSG("curr pos = %d, next tag = %d\n", getStreamPos(), tag->NextTagPos);
        ASSERT (getStreamPos() > tag->NextTagPos);
        
    	//setByteAlignment(); // reset bit buffer for byte-alignment
        VObject &shapeRecord = shapeRecordsObject[recNo];
        
		unsigned int TypeFlag = getUBits(1); // TypeFlag => 0 : Non-edge, 1: Edge Reocrds
        shapeRecord["TypeFlag"]   = TypeFlag;
        
	    if (!TypeFlag) { // Non-edge Records where TypeFlag == 0
			
			unsigned int Flags = getUBits(5);            
			
			if (Flags == 0) { // ENDSHAPERECORD
                shapeRecord.setTypeInfo ("ENDSHAPERECORD");                
                shapeRecord["EndOfShape"] = 0;
				DEBUGMSG("{ /* EndShapeRecord */\nTypeFlag : %d,\nEndOfShape : 0\n}\n]\n}", TypeFlag);
				return TRUE;
			} else { // STYLECHANGERECORD
				shapeRecord.setTypeInfo ("STYLECHANGERECORD");
                
				unsigned int StateNewStyles, StateLineStyle, StateFillStyle1, StateFillStyle0, StateMoveTo;
				signed int MoveDeltaX, MoveDeltaY;
				unsigned int FillStyle0, FillStyle1, LineStyle; // Selector of FillStyles and LineStyles. Arrays begin at index 1.
				
				StateNewStyles  = (Flags >> 4) & 1;
				StateLineStyle  = (Flags >> 3) & 1;
				StateFillStyle1 = (Flags >> 2) & 1;
				StateFillStyle0 = (Flags >> 1) & 1;
				StateMoveTo     = Flags & 1;
                
                shapeRecord["StateNewStyles"]   = StateNewStyles;
                shapeRecord["StateLineStyle"]   = StateLineStyle;
                shapeRecord["StateFillStyle1"]  = StateFillStyle1;
                shapeRecord["StateFillStyle0"]  = StateFillStyle0;
                shapeRecord["StateMoveTo"]      = StateMoveTo;
                DEBUGMSG("{ /* StyleChangeRecord */\nTypeFlag : %d,\nStateNewStyles : %d,\nStateLineStyle : %d,\nStateFillStyle1 : %d,\nStateFillStyle0 : %d,\nStateMoveTo : %d", 
                         TypeFlag, StateNewStyles, StateLineStyle, StateFillStyle1, StateFillStyle0, StateMoveTo);
				
				if (StateMoveTo) {
					unsigned int MoveBits = getUBits(5);
					MoveDeltaX = getSBits(MoveBits);
					MoveDeltaY = getSBits(MoveBits);
                    
                    shapeRecord["MoveBits"]      = MoveBits;
                    shapeRecord["MoveDeltaX"]    = MoveDeltaX;
                    shapeRecord["MoveDeltaY"]    = MoveDeltaY;
					DEBUGMSG(",\nMoveBits : %d,\nMoveDeltaX : %d,\nMoveDeltaY : %d", MoveBits, MoveDeltaX, MoveDeltaY);
				}
				
				if (StateFillStyle0) {
                    //ASSERT (NumFillBits == 0); // weird, it seems allow to get 0 bit          
					FillStyle0 = getUBits(NumFillBits);
                    
                    shapeRecord["FillStyle0"]      = FillStyle0;
					DEBUGMSG(",\nFillStyle0 : %d", FillStyle0);
				}
				
				if (StateFillStyle1) {
                    //ASSERT (NumFillBits == 0); // weird, it seems allow to get 0 bit
					FillStyle1 = getUBits(NumFillBits);
                    
                    shapeRecord["FillStyle1"]      = FillStyle1;
					DEBUGMSG(",\nFillStyle1 : %d", FillStyle1);
				}
				
				if (StateLineStyle) {
                    //ASSERT (NumLineBits == 0); // weird, it seems allow to get 0 bit
					LineStyle = getUBits(NumLineBits);
                    
                    shapeRecord["LineStyle"]      = LineStyle;
					DEBUGMSG(",\nLineStyle : %d", LineStyle);
				}
				
				if (StateNewStyles) {
					DEBUGMSG(",\nFillStyles : ");
                    getFILLSTYLEARRAY(tag, shapeRecordsObject[recNo]["FillStyles"]);
					DEBUGMSG(",\nLineStyles : ");
					getLINESTYLEARRAY(tag, shapeRecordsObject[recNo]["LineStyles"]);
                    
					//unsigned int NewNumFillBits, NewNumLineBits;
					// Bug Fixed : NumFillBits and NumLineBits should be changed if new styles
                    NumFillBits = getUBits(4);
					NumLineBits = getUBits(4);
                    
                    shapeRecord["NumFillBits"]      = NumFillBits;
                    shapeRecord["NumLineBits"]      = NumLineBits;
					DEBUGMSG(",\nNumFillBits : %d,\nNumLineBits : %d", NumFillBits, NumLineBits);
				}
				
				DEBUGMSG("\n},\n");
			}		
		} else { // Edge Records where TypeFlag == 1
			
			unsigned int StraightFlag, NumBits;
			
			StraightFlag	= getUBits(1);
			NumBits			= getUBits(4);

            shapeRecord["StraightFlag"] = StraightFlag;
            shapeRecord["NumBits"]      = NumBits;
                        
			
			if (StraightFlag) { // STRAIGHTEDGERECORD
                
				shapeRecord.setTypeInfo ("STRAIGHTEDGERECORD");
                DEBUGMSG("{ /* StraightEdgeRecord */\nTypeFlag : %d,\nStraightFlag : %d,\nNumBits : %d", TypeFlag, StraightFlag, NumBits);
                
				unsigned int GeneralLineFlag = getUBits(1);
				signed int VertLineFlag, DeltaX, DeltaY;
                
                shapeRecord["GeneralLineFlag"] = GeneralLineFlag;
                DEBUGMSG(",\nGeneralLineFlag : %d", GeneralLineFlag);
                
				if (GeneralLineFlag) { // General Line
                    shapeRecord["GeneralLineFlag"].setTypeInfo ("General Line");
                    
					DeltaX = getSBits(NumBits + 2);
					DeltaY = getSBits(NumBits + 2);
                    shapeRecord["DeltaX"] = DeltaX;
                    shapeRecord["DeltaY"] = DeltaY;
					DEBUGMSG(", /* General Line */\nDeltaX : %d,\nDeltaY : %d", DeltaX, DeltaY);
                    
				} else { // Vert/Horz Line
                    shapeRecord["GeneralLineFlag"].setTypeInfo ("Vert/Horz Line");
                    
					VertLineFlag = getUBits(1);
                    
                    shapeRecord["VertLineFlag"] = VertLineFlag;
                    DEBUGMSG(", /* Vert/Horz Line */\nVertLineFlag : %d", VertLineFlag);
                    
					if (VertLineFlag) {
						DeltaY = getSBits(NumBits + 2);
                        
                        shapeRecord["DeltaY"] = DeltaY;
						DEBUGMSG(",\nDeltaY : %d", DeltaY);
					} else {
						DeltaX = getSBits(NumBits + 2);
                        shapeRecord["DeltaX"] = DeltaX;
                        
						DEBUGMSG(",\nDeltaX : %d", DeltaX);
					}
				}
				DEBUGMSG("\n},\n");
                
			} else { // CURVEDEDGERECORD
                shapeRecord.setTypeInfo ("CURVEDEDGERECORD");
                
				signed int ControlDeltaX, ControlDeltaY, AnchorDeltaX, AnchorDeltaY;
                
                DEBUGMSG("{ /* CurvedEdgeRecord */\nTypeFlag : %d,\nStraightFlag : %d,\nNumBits : %d", TypeFlag, StraightFlag, NumBits);
                
				ControlDeltaX	= getSBits(NumBits + 2);
				ControlDeltaY	= getSBits(NumBits + 2);
				AnchorDeltaX	= getSBits(NumBits + 2);
				AnchorDeltaY	= getSBits(NumBits + 2);
                
				shapeRecord["ControlDeltaX"] = ControlDeltaX;
                shapeRecord["ControlDeltaY"] = ControlDeltaY;
                shapeRecord["AnchorDeltaX"] = AnchorDeltaX;
                shapeRecord["AnchorDeltaY"] = AnchorDeltaY;
				DEBUGMSG(",\nControlDeltaX : %d,\nControlDeltaY : %d,\nAnchorDeltaX : %d,\nAnchorDeltaY : %d",
						 ControlDeltaX, ControlDeltaY, AnchorDeltaX, AnchorDeltaY);
				DEBUGMSG("\n},\n");
			}

		}
        recNo++;
	} // while
}

int SWFParser::getSHAPEWITHSTYLE(Tag *tag, VObject &styleObject)
{
    styleObject.setTypeInfo("SHAPEWITHSTYLE");
	DEBUGMSG("{ /* SHAPEWITHSTYLE */\n");
    
    DEBUGMSG("FillStyles : ");
	getFILLSTYLEARRAY(tag, styleObject["FillStyles"]);
    
    DEBUGMSG(",\nLineStyles : ");
	getLINESTYLEARRAY(tag, styleObject["LineStyles"]);
    
    DEBUGMSG(",\nShape : ");
	getSHAPE(tag, styleObject["Shape"]);
	
	DEBUGMSG("\n}");
	
	return TRUE;
}


///////////////////////////////////////
//// Morph Fill Style
///////////////////////////////////////

int SWFParser::getMORPHGRADIENT(Tag *tag, VObject &gradientObject)
{
    gradientObject.setTypeInfo("MORPHGRADIENT");
    DEBUGMSG("{ /* MORPHGRADIENT */\n");
    
    unsigned int NumGradients;
    
    NumGradients = getUI8();
    
    gradientObject["NumGradients"] = NumGradients;
    DEBUGMSG("NumGradient : %d", NumGradients);
    
    DEBUGMSG(",\nGradientRecords : [\n");
    
    // GradientRecords : MORPHGRADRECORD[]
    
    for (int i = 0; i < NumGradients; i++) {
        VObject &gradRecord = gradientObject["GradientRecords"][i];
        
        // MORPHGRADRECORD
        gradRecord.setTypeInfo("MORPHGRADRECORD");
        
        unsigned int StartRatio, StartColor, EndRatio, EndColor;
        
        StartRatio = getUI8();
        
        gradRecord["StartRatio"] = StartRatio;
        DEBUGMSG("{ /* MORPHGRADRECORD */\nStartRatio : %d", StartRatio);
        
        StartColor = getRGBA();
        
        gradRecord["StartColor"] = StartColor;
        DEBUGMSG(",\nStartColor : \"%#04x\"", StartColor);
        
        EndRatio = getUI8();
        
        gradRecord["EndRatio"] = EndRatio;
        DEBUGMSG(",\nEndRatio : %d", EndRatio);
        
        EndColor = getRGBA();
        
        gradRecord["EndColor"] = EndColor;
        DEBUGMSG(",\nEndColor : \"%#04x\"\n}", EndColor);
        if (i < (NumGradients - 1))
            DEBUGMSG(",\n");
    }
    
    DEBUGMSG("\n]");
    DEBUGMSG("\n}");
    return TRUE;
}

//// FillStyleType :
//// 0x00 = solid fill
//// 0x10 = linear gradient fill
//// 0x12 = radial gradient fill
//// 0x13 = focal radial gradient fill (SWF8 or later)
//// 0x40 = repeating bitmap fill
//// 0x41 = clipped bitmap fill
//// 0x42 = non-smoothed repeating bitmap
//// 0x43 = non-smoothed clipped bitmap
int SWFParser::getMORPHFILLSTYLE(Tag *tag, VObject &fillStyleObject)
{
    
    DEBUGMSG("{ /* MORPHFILLSTYLE */\n");
    
    unsigned int FillStyleType, StartColor, EndColor;
    
    FillStyleType = getUI8();    
    fillStyleObject["FillStyleType"] = FillStyleType;
    DEBUGMSG("FillStyleType : %d", FillStyleType);
    
    switch (FillStyleType) {
        case 0x00:
            fillStyleObject.setTypeInfo("MORPHFILLSTYLE (Solid Fill)");
            break;
        case 0x10:
            fillStyleObject.setTypeInfo("MORPHFILLSTYLE (Linear Gradient Fill)");
            break;
        case 0x12:
            fillStyleObject.setTypeInfo("MORPHFILLSTYLE (Radial Gradient Fill)");
            break;
        case 0x40:
            fillStyleObject.setTypeInfo("MORPHFILLSTYLE (Repeating Bitmap Fill)");
            break;
        case 0x41:
            fillStyleObject.setTypeInfo("MORPHFILLSTYLE (Clipped Bitmap Fill)");
            break;
        case 0x42:
            fillStyleObject.setTypeInfo("MORPHFILLSTYLE (Non-smoothed Repeating Bitmap)");
            break;
        case 0x43:
            fillStyleObject.setTypeInfo("MORPHFILLSTYLE (Non-smoothed Clipped Bitmap)");
            break;
        default:
            ASSERT(1);  // undefined MORPHFILLSTYLE type
            break;
    }
    
    if (FillStyleType == 0x00) { // Solid Color Fill
        //fillStyleObject["FillStyleType"].setTypeInfo("Solid Fill");
        
        StartColor = getRGBA();
        EndColor = getRGBA();
        
        fillStyleObject["StartColor"] = Color2String(StartColor, 1);
        fillStyleObject["EndColor"] = Color2String(EndColor, 1);
        DEBUGMSG(", /* Solid Fill */\nStartColor : \"%#04x\",\nEndColor : \"%#04x\"", StartColor, EndColor);
    }
    
    // FIXME, how about 0x13 = focal radial gradient fill??
    if ((FillStyleType == 0x10) ||
        (FillStyleType == 0x12)) { // Gradient Fill
        //fillStyleObject["FillStyleType"].setTypeInfo("Gradient Fill");
        fillStyleObject.setTypeInfo("MORPHFILLSTYLE (Gradient Fill)");
        
        DEBUGMSG(", /* Gradient Fill */\nStartGradientMatrix : ");
        getMATRIX(fillStyleObject["StartGradientMatrix"]); // StartGradientMatrix
        
        DEBUGMSG(",\nEndGradientMatrix : ");
        getMATRIX(fillStyleObject["EndGradientMatrix"]); // EndGradientMatrix
        
        DEBUGMSG(",\nGradient : ");
        getMORPHGRADIENT(tag, fillStyleObject["Gradient"]);
    }
    
    if ((FillStyleType == 0x40) ||
        (FillStyleType == 0x41) ||
        (FillStyleType == 0x42) ||
        (FillStyleType == 0x43)) { // Bitmap Fill
        
        //fillStyleObject["FillStyleType"].setTypeInfo("Bitmap Fill");
        fillStyleObject.setTypeInfo("MORPHFILLSTYLE (Bitmap Fill)");
        
        unsigned int BitmapId;
        
        BitmapId = getUI16();
        
        fillStyleObject["BitmapId"] = BitmapId;
        DEBUGMSG(", /* Bitmap Fill */\nBitmapId : %d", BitmapId);
        
        DEBUGMSG(",\nStartBitmpMatrix : ");
        getMATRIX(fillStyleObject["StartBitmpMatrix"]); // StartBitmpMatrix

        DEBUGMSG(",\nEndBitmpMatrix : ");
        getMATRIX(fillStyleObject["EndBitmpMatrix"]); // EndBitmpMatrix
    }
    DEBUGMSG("\n}");
        
    return TRUE;
}

int SWFParser::getMORPHFILLSTYLEARRAY(Tag *tag, VObject &fillStyleArrayObject)
{
    fillStyleArrayObject.setTypeInfo("MORPHFILLSTYLEARRAY");
    DEBUGMSG("{ /* MORPHFILLSTYLEARRAY */\n");
    
    unsigned int FillStyleCount = 0;

    FillStyleCount = getUI8();
    
    if (FillStyleCount == 0xff)
        FillStyleCount = getUI16();
    
    fillStyleArrayObject["FillStyleCount"] = FillStyleCount;
    DEBUGMSG("FillStyleCount : %d", FillStyleCount);
    
    DEBUGMSG(",\nFillStyles : [\n");
    
    // FillStyles : MORPHFILLSTYLE[FillStyleCount]
    for (int i = 0; i < FillStyleCount; i++) {
        
        getMORPHFILLSTYLE(tag, fillStyleArrayObject["FillStyles"][i]);
        if (i < (FillStyleCount - 1))
            DEBUGMSG(",\n");
    }
    
    DEBUGMSG("\n]");
    DEBUGMSG("\n}");
    
    return TRUE;
}


///////////////////////////////////////
//// Morph Line Style
///////////////////////////////////////


// DefineMorphShape  = 46
// DefineMorphShape2 = 84
int SWFParser::getMORPHLINESTYLE(Tag *tag, VObject &lineStyleObject)
{
    lineStyleObject.setTypeInfo("MORPHLINESTYLE");
    DEBUGMSG("{ /* MORPHLINESTYLE */\n");
    
    unsigned int StartWidth, EndWidth, StartColor, EndColor;
    unsigned int StartCapStyle, JoinStyle, HasFillFlag, NoHScaleFlag, NoVScaleFlag, PixelHintingFlag, NoClose, EndCapStyle, MiterLimitFactor;

    StartWidth = getUI16();
    
    lineStyleObject["StartWidth"] = StartWidth;
    DEBUGMSG("StartWidth : %d", StartWidth);
    
    EndWidth = getUI16();
    
    lineStyleObject["EndWidth"] = EndWidth;
    DEBUGMSG(",\nEndWidth : %d", EndWidth);
    
    if (tag->TagCode == TAG_DEFINEMORPHSHAPE2) { // DefineMorphShape2
        
        StartCapStyle = getUBits(2); // 0 = Round cap, 1 = No cap, 2 = Square cap
        
        lineStyleObject["StartCapStyle"] = StartCapStyle;
        DEBUGMSG(",\nStartCapStyle : %d", StartCapStyle);
        
        JoinStyle = getUBits(2);    // 0 = Round join, 1 = Bevel join, 2 = Miter join
        
        lineStyleObject["JoinStyle"] = JoinStyle;
        DEBUGMSG(",\nJoinStyle : %d", JoinStyle);
        
        HasFillFlag = getUBits(1);  // 0 = use StartColor and End Color, 1 = use FillType
        
        lineStyleObject["HasFillFlag"] = HasFillFlag;
        DEBUGMSG(",\nHasFillFlag : %d", HasFillFlag);
        
        NoHScaleFlag = getUBits(1);
        
        lineStyleObject["NoHScaleFlag"] = NoHScaleFlag;
        DEBUGMSG(",\nNoHScaleFlag : %d", NoHScaleFlag);
        
        NoVScaleFlag = getUBits(1);
        
        lineStyleObject["NoVScaleFlag"] = NoVScaleFlag;
        DEBUGMSG(",\nNoVScaleFlag : %d", NoVScaleFlag);
        
        PixelHintingFlag = getUBits(1);
        
        lineStyleObject["PixelHintingFlag"] = PixelHintingFlag;
        DEBUGMSG(",\nPixelHintingFlag : %d", PixelHintingFlag);

        getUBits(5); // Reserved, Must be 0
        
        NoClose = getUBits(1);
        
        lineStyleObject["NoClose"] = NoClose;
        DEBUGMSG(",\nNoClose : %d", NoClose);
        
        EndCapStyle = getUBits(2); // 0 = Round cap, 1 = No cap, 2 = Square cap
        
        lineStyleObject["EndCapStyle"] = EndCapStyle;
        DEBUGMSG(",\nEndCapStyle : %d", EndCapStyle);
        
        
        if (JoinStyle == 2) {
            MiterLimitFactor = getUI16(); // Miter limit factor as an 8.8 fixed-point value.
            
            lineStyleObject["MiterLimitFactor"] = FIXED8TOFLOAT(MiterLimitFactor);
            DEBUGMSG(",\nMiterLimitFactor : %f", MiterLimitFactor);
        }
        
        if (HasFillFlag) {
            DEBUGMSG(",\nFillType : ");
            getMORPHFILLSTYLE(tag, lineStyleObject["FillType"]);
            
            DEBUGMSG("\n}"); // MORPHLINESTYLE end
            return TRUE;
        }
    }
    
    StartColor = getRGBA();
    
    lineStyleObject["StartColor"] = Color2String(StartColor, 1);
    DEBUGMSG(",\nStartColor : \"%#04x\"", StartColor);
        
    EndColor = getRGBA();
    
    lineStyleObject["EndColor"] = Color2String(EndColor, 1);
    DEBUGMSG(",\nEndColor : \"%#04x\"", EndColor);
    
    DEBUGMSG("\n}"); // MORPHLINESTYLE end
    return TRUE;
}


int SWFParser::getMORPHLINESTYLEARRAY(Tag *tag, VObject &lineStyleArrayObject)
{
    lineStyleArrayObject.setTypeInfo("MORPHLINESTYLEARRAY");
    DEBUGMSG("{ /* MORPHLINESTYLEARRAY */\n");
    
    unsigned int LineStyleCount = 0;
    
    LineStyleCount = getUI8();
    if (LineStyleCount == 0xff)
        LineStyleCount = getUI16();
    
    lineStyleArrayObject["LineStyleCount"] = LineStyleCount;
    DEBUGMSG("LineStyleCount : %d", LineStyleCount);
    
    DEBUGMSG(",\nLineStyles : [\n");
    
    // LinesStyles : MORPHLILNESTYLE[LineStyleCount] || MORPHLINESTYLE2[LineStyleCount]
    for (int i = 0; i < LineStyleCount; i++) {
        getMORPHLINESTYLE(tag, lineStyleArrayObject["LinesStyles"][i]);
        if (i < (LineStyleCount - 1))
            DEBUGMSG(",\n");
    }
    
    DEBUGMSG("\n]");
    DEBUGMSG("\n}");
    
    return TRUE;
}






///////////////////////////////////////
//// TEXTRECORD
///////////////////////////////////////
int SWFParser::getTEXTRECORD(Tag *tag, unsigned int GlyphBits, unsigned int AdvanceBits)
{
    unsigned int Flags, TextRecordType, StyleFlagsHasFont, StyleFlagsHasColor, StyleFlagsHasYOffset, StyleFlagsHasXOffset, FontID, TextColor, TextHeight, GlyphCount;
    signed int XOffset = 0, YOffset = 0;
    
    
    Flags = getUI8();
    
    TextRecordType = (Flags >> 7) & 1;
    if (!TextRecordType) {
        DEBUGMSG("{ /* EndOfRecordsFlag */\nEndOfRecords : 0\n}");
        return FALSE;
    }
    
    DEBUGMSG("{ /* TEXTRECORD */\n");
    
    StyleFlagsHasFont       = (Flags >> 3) & 1;
    DEBUGMSG("StyleFlagsHasFont : %d", StyleFlagsHasFont);

    StyleFlagsHasColor      = (Flags >> 2) & 1;
    DEBUGMSG(",\nStyleFlagsHasColor : %d", StyleFlagsHasColor);

    StyleFlagsHasYOffset    = (Flags >> 1) & 1;
    DEBUGMSG(",\nStyleFlagsHasYOffset : %d", StyleFlagsHasYOffset);

    StyleFlagsHasXOffset    = Flags & 1;
    DEBUGMSG(",\nStyleFlagsHasXOffset : %d", StyleFlagsHasXOffset);
    
    if (StyleFlagsHasFont) {
        FontID = getUI16();
        DEBUGMSG(",\nFontID : %d", FontID);
    }
    
    if (StyleFlagsHasColor) {
        if (tag->TagCode == TAG_DEFINETEXT) {
            TextColor = getRGB();
            DEBUGMSG(",\nTextColor : \"%#03x\"", TextColor);
        }
        if (tag->TagCode == TAG_DEFINETEXT2) {
            TextColor = getRGBA();
            DEBUGMSG(",\nTextColor : \"%#04x\"", TextColor);
        }
    }
    
    if (StyleFlagsHasXOffset) {
        XOffset = getSI16();
        DEBUGMSG(",\nXOffset : %d", XOffset);
    }
    
    if (StyleFlagsHasYOffset) {
        YOffset = getSI16();
        DEBUGMSG(",\nYOffset : %d", YOffset);
    }
    
    if (StyleFlagsHasFont) {
        TextHeight = getUI16();
        DEBUGMSG(",\nTextHeight : %d", TextHeight);
    }
    
    GlyphCount = getUI8();
    DEBUGMSG(",\nGlyphCount : %d", GlyphCount);
    
    DEBUGMSG(",\nGlyphEntries : [\n");
    
    for (int i = 0; i < GlyphCount; i++) {
        // GLYPHENTRY
        unsigned int GlyphIndex;
        signed GlyphAdvance;
        
        GlyphIndex   = getUBits(GlyphBits);
        GlyphAdvance = getSBits(AdvanceBits);
        DEBUGMSG("{ GlyphIndex : %d, GlyphAdvance : %d }", GlyphIndex, GlyphAdvance);
        if (i < (GlyphCount - 1))
            DEBUGMSG(",\n");
    }
    
    DEBUGMSG("\n]");    
    DEBUGMSG("\n}");
    
    return TRUE;
}


///////////////////////////////////////
//// BUTTONRECORD
///////////////////////////////////////
int SWFParser::getBUTTONRECORD(Tag *tag, VObject &btnRecordObject)
{
    unsigned int firstByte, ButtonHasBlendMode, ButtonHasFilterList, ButtonStateHitTest, ButtonStateDown, ButtonStateOver, ButtonStateUp, CharacterID, PlaceDepth, BlendMode;
    
    firstByte = getUI8();
    
    if (firstByte == 0) {
        btnRecordObject.setTypeInfo("CharacterEndFlag");
        btnRecordObject = 0;
        
        DEBUGMSG("{ /* CharacterEndFlag */\nChraracterEndFlag : 0\n}");
        return FALSE; // This should be CharacterEndFlag? FIXME
    }
    
    DEBUGMSG("{ /* BUTTONRECORD */\n");
    btnRecordObject.setTypeInfo("BUTTONRECORD");
    
    ButtonHasBlendMode  = (firstByte >> 5) & 1;
    
    btnRecordObject["ButtonHasBlendMode"] = ButtonHasBlendMode;
    DEBUGMSG("ButtonHasBlendMode : %d", ButtonHasBlendMode);
    
    ButtonHasFilterList = (firstByte >> 4) & 1;
    
    btnRecordObject["ButtonHasFilterList"] = ButtonHasFilterList;
    DEBUGMSG(",\nButtonHasFilterList : %d", ButtonHasFilterList);
    
    ButtonStateHitTest  = (firstByte >> 3) & 1;
    
    btnRecordObject["ButtonStateHitTest"] = ButtonStateHitTest;
    DEBUGMSG(",\nButtonStateHitTest : %d", ButtonStateHitTest);
    
    ButtonStateDown     = (firstByte >> 2) & 1;
    
    btnRecordObject["ButtonStateDown"] = ButtonStateDown;
    DEBUGMSG(",\nButtonStateDown : %d", ButtonStateDown);
    
    ButtonStateOver     = (firstByte >> 1) & 1;
    
    btnRecordObject["ButtonStateOver"] = ButtonStateOver;
    DEBUGMSG(",\nButtonStateOver : %d", ButtonStateOver);
    
    ButtonStateUp       = firstByte & 1;
    
    btnRecordObject["ButtonStateUp"] = ButtonStateUp;
    DEBUGMSG(",\nButtonStateUp : %d", ButtonStateUp);
    
    CharacterID = getUI16();
    PlaceDepth  = getUI16();
    
    btnRecordObject["CharacterID"] = CharacterID;
    btnRecordObject["PlaceDepth"] = PlaceDepth;
    DEBUGMSG(",\nChracterId : %d,\nPlaceDepth : %d", CharacterID, PlaceDepth);
    
    DEBUGMSG(",\nPlaceMatrix : ");
    getMATRIX(btnRecordObject["PlaceMatrix"]);    // PlaceMatrix
    
    if (tag->TagCode == TAG_DEFINEBUTTON2) { // DefineButton2

        DEBUGMSG(",\nColorTransform : ");
        getCXFORMWITHALPHA(btnRecordObject["ColorTransform"]);   // ColorTransform
        
        if (ButtonHasFilterList) {
            DEBUGMSG(",\nFilterList : ");
            getFILTERLIST(btnRecordObject["FilterList"]);    // FilterList
        }
        
        if (ButtonHasBlendMode) {
            BlendMode = getUI8();
            
            btnRecordObject["BlendMode"] = BlendMode;
            DEBUGMSG(",\nBlendMode : %d", BlendMode);            
        }
    }
    
    DEBUGMSG("\n}");
    return TRUE;
}


int SWFParser::getBUTTONCONDACTION(VObject &btnCondObject)
{
    btnCondObject.setTypeInfo("BUTTONCONDACTION");
    DEBUGMSG("{ /* BUTTONCONDACTION */\n");
    
    unsigned int CondActionSize, CondIdleToOverDown, CondOutDownToIdle, CondOutDownToOverDown, CondOverDownToOutDown, CondOverDownToOverUp, CondOverUpToOverDown, CondOverUpToIdle, CondIdleToOverUp, CondKeyPress, CondOverDownToIdle;
    
    CondActionSize = getUI16();
    
    btnCondObject["CondActionSize"] = CondActionSize;
    DEBUGMSG("CondActionSize : %d", CondActionSize);
    
    CondIdleToOverDown      = getUBits(1);
    
    btnCondObject["CondIdleToOverDown"] = CondIdleToOverDown;
    DEBUGMSG(",\nCondIdleToOverDown : %d", CondIdleToOverDown);
    
    CondOutDownToIdle       = getUBits(1);
    
    btnCondObject["CondOutDownToIdle"] = CondOutDownToIdle;
    DEBUGMSG(",\nCondOutDownToIdle : %d", CondOutDownToIdle);
    
    CondOutDownToOverDown   = getUBits(1);
    
    btnCondObject["CondOutDownToOverDown"] = CondOutDownToOverDown;
    DEBUGMSG(",\nCondOutDownToOverDown : %d", CondOutDownToOverDown);
    
    CondOverDownToOutDown   = getUBits(1);
    
    btnCondObject["CondOverDownToOutDown"] = CondOverDownToOutDown;
    DEBUGMSG(",\nCondOverDownToOutDown : %d", CondOverDownToOutDown);
    
    CondOverDownToOverUp    = getUBits(1);
    
    btnCondObject["CondOverDownToOverUp"] = CondOverDownToOverUp;
    DEBUGMSG(",\nCondOverDownToOverUp : %d", CondOverDownToOverUp);
    
    CondOverUpToOverDown    = getUBits(1);
    
    btnCondObject["CondOverUpToOverDown"] = CondOverUpToOverDown;
    
    DEBUGMSG(",\nCondOverUpToOverDown : %d", CondOverUpToOverDown);
    CondOverUpToIdle        = getUBits(1);
    
    btnCondObject["CondOverUpToIdle"] = CondOverUpToIdle;
    
    DEBUGMSG(",\nCondOverUpToIdle : %d", CondOverUpToIdle);
    CondIdleToOverUp        = getUBits(1);
    
    btnCondObject["CondIdleToOverUp"] = CondIdleToOverUp;
    DEBUGMSG(",\nCondIdleToOverUp : %d", CondIdleToOverUp);
    
    CondKeyPress            = getUBits(7);  // SWF4 or later : Key code
    
    btnCondObject["CondKeyPress"] = CondKeyPress;
    DEBUGMSG(",\nCondKeyPress : %d", CondKeyPress);
    
    CondOverDownToIdle      = getUBits(1);
    
    btnCondObject["CondOverDownToIdle"] = CondOverDownToIdle;
    DEBUGMSG(",\nCondOverDownToIdle : %d", CondOverDownToIdle);
    
    DEBUGMSG(",\nActions : [\n");
    
    unsigned int recNo = 0;
    while(getACTIONRECORD(btnCondObject["Actions"][recNo++]));
    
    DEBUGMSG("\n]");

    DEBUGMSG("\n}");
    // FIXEME : need to check the tag end and CondActionSize
    
    if (CondActionSize == 0)
        return FALSE;   // last action
    else
        return TRUE;   // has next
}



// FIXME, need to go into details
unsigned int SWFParser::getCLIPEVENTFLAGS()
{
	if (SWFVersion <= 5)
		return getUI16();
	else
		return getUI32();
}

unsigned int SWFParser::getCLIPACTIONRECORD(VObject &clipActionRecordObject)
{
    clipActionRecordObject.setTypeInfo("CLIPACTIONRECORD");
    
	unsigned int EventFlags, ActionRecordSize, KeyCode;
	
	EventFlags = getCLIPEVENTFLAGS();
	
    clipActionRecordObject["EventFlags"] = EventFlags;
    
	if (EventFlags == 0) // This is ClipActionEndFlg = 0
		return FALSE; 
	
	ActionRecordSize = getUI32();
	
    clipActionRecordObject["ActionRecordSize"] = ActionRecordSize;
    
	if (EventFlags & CLIPEVENT_KEYPRESS_MASK) {
		KeyCode = getUI8();
        clipActionRecordObject["KeyCode"] = KeyCode;
    }

	// ACTIONRECORD[]
    unsigned int recNo = 0;
	while(getACTIONRECORD(clipActionRecordObject["Actions"][recNo++])); // read until we get ActionEndFlg = 0
	
	return EventFlags;
}

int SWFParser::getCLIPACTIONS(VObject &clipActionsObject)
{	
    clipActionsObject.setTypeInfo("CLIPACTIONS");
    
	unsigned int AllEventFlags;
	getUI16(); // Reserved must be 0
	
	// CLIPEVENTFLAGS
	AllEventFlags = getCLIPEVENTFLAGS();
    clipActionsObject["AllEventFlags"] = AllEventFlags; // FIXME
    
	unsigned int recNo = 0;
	while(getCLIPACTIONRECORD(clipActionsObject["ClipActionRecords"][recNo++])); // read until we get ClipActionEndFlag = 0
	
    return TRUE;
}


///////////////////////////////////////
//// Filter Operation (used internally)
///////////////////////////////////////
int SWFParser::getCOLORMATRIXFILTER(VObject &filterObject)
{
    filterObject.setTypeInfo("COLORMATRIXFILTER");
    DEBUGMSG("{ /* COLORMATRIXFILTER */\n");
    
    float Matrix[20];
    
    DEBUGMSG("Matrix : [ ");
    for (int i = 0; i < 20; i++) {
        Matrix[i] = getFLOAT();
        
        filterObject["Matrix"][i] = Matrix[i];
        DEBUGMSG("%4f ", Matrix[i]);
        /*
        if ( (i+1) % 5 == 0)
            DEBUGMSG("\n");
         */
        if (i < 19)
            DEBUGMSG(", ");
    }
    
    DEBUGMSG("]");
    DEBUGMSG("\n}");
    
    return TRUE;
}

int SWFParser::getCONVOLUTIONFILTER(VObject &filterObject)
{
    filterObject.setTypeInfo("CONVOLUTIONFILTER");
    DEBUGMSG("{ /* CONVOLUTIONFILTER */\n");
    
    unsigned int MatrixX, MatrixY;
    MatrixX = getUI8();
    MatrixY = getUI8();
    
    filterObject["MatrixX"] = MatrixX;
    filterObject["MatrixY"] = MatrixY;
    DEBUGMSG("MatrixX : %d,\nMatrixY : %d", MatrixX, MatrixY);
    
    float Divisor, Bias, Matrix[MatrixX * MatrixY];
    
    Divisor = getFLOAT();
    Bias    = getFLOAT();

    filterObject["Divisor"] = Divisor;
    filterObject["Bias"]    = Bias;
    DEBUGMSG(",\nDivisor : %f\nBias : %f", Divisor, Bias);
    
    DEBUGMSG(",\nMatrix : [\n");
    for (int i = 0; i < (MatrixX * MatrixY); i++) {
        
        Matrix[i] = getFLOAT();
        
        filterObject["Matrix"][i] = Matrix[i];
        DEBUGMSG("%4f", Matrix[i]);

        if (i < (MatrixX * MatrixY - 1))
            DEBUGMSG(", ");

        if ( (i+1) % MatrixX == 0)
            DEBUGMSG("\n");
    }
    
    DEBUGMSG("\n]");
    
    unsigned int DefaultColor, Clamp, PreserveAlpha;
    
    DefaultColor = getRGBA();
    
    getUBits(6);    // Reserved
    Clamp = getUBits(1);
    PreserveAlpha = getUBits(1);
    
    filterObject["DefaultColor"]    = Color2String(DefaultColor, 1);
    filterObject["Clamp"]           = Clamp;
    filterObject["PreserveAlpha"]   = PreserveAlpha;
    DEBUGMSG(",\nDefaultColor : \"%#04x\",\nClamp : %d,\nPreserveAlpha : %d", DefaultColor, Clamp, PreserveAlpha);
    
    DEBUGMSG("\n}");
    
    return TRUE;
}

int SWFParser::getBLURFILTER(VObject &filterObject)
{
    filterObject.setTypeInfo("BLURFILTER");
    DEBUGMSG("{ /* BLURFILTER */\n");
    
    float BlurX, BlurY;
    
    BlurX = getFIXED();
    BlurY = getFIXED();
    
    unsigned int Passes;
    
    Passes = getUBits(5);
    
    filterObject["BlurX"]   = BlurX;
    filterObject["BlurY"]   = BlurY;
    filterObject["Passes"]  = Passes;
    DEBUGMSG("BlurX : %f,\nBlurY : %f,\nPasses : %d", BlurX, BlurY, Passes);
    ASSERT(getUBits(3));    // Reserved, must be 0
    
    DEBUGMSG("\n}");
    
    return TRUE;
}

int SWFParser::getDROPSHADOWFILTER(VObject &filterObject)
{
    filterObject.setTypeInfo("DROPSHADOWFILTER");
    DEBUGMSG("{ /* DROPSHADOWFILTER */\n");
    
    unsigned int DropShadowColor, InnerShadow, Knockout, CompositeSource, Passes;
    
    float BlurX, BlurY, Angle, Distance, Strength;

    DropShadowColor = getRGBA();
    BlurX = getFIXED();
    BlurY = getFIXED();
    Angle = getFIXED();
    Distance = getFIXED();
    Strength = getFIXED8();
    
    InnerShadow = getUBits(1);
    Knockout    = getUBits(1);
    CompositeSource = getUBits(1);
    Passes = getUBits(5);
    
    filterObject["DropShadowColor"] = Color2String(DropShadowColor, 1);
    filterObject["BlurX"]           = BlurX;
    filterObject["BlurY"]           = BlurY;
    filterObject["Angle"]           = Angle;
    filterObject["Distance"]        = Distance;
    filterObject["Strength"]        = Strength;
    filterObject["InnerShadow"]     = InnerShadow;
    filterObject["Knockout"]        = Knockout;
    filterObject["CompositeSource"] = CompositeSource;
    filterObject["Passes"]          = Passes;
    
    DEBUGMSG("DropShadowColor : \"%#04x\",\nBlurX : %f,\nBlurY : %f,\nAngle : %f,\nDistance : %f,\nStrength : %f", DropShadowColor, BlurX, BlurY, Angle, Distance, Strength);
    DEBUGMSG(",\nInnerShadow : %d,\nKnockout : %d,\nCompositeSource : %d,\nPasses : %d", InnerShadow, Knockout, CompositeSource, Passes);
    
    DEBUGMSG("\n}");
    return TRUE;
}


int SWFParser::getGLOWFILTER(VObject &filterObject)
{
    filterObject.setTypeInfo("GLOWFILTER");
    DEBUGMSG("{ /* GLOWFILTER */\n");

    unsigned int GlowColor, InnerShadow, Knockout, CompositeSource, Passes;
    
    float BlurX, BlurY, Strength;
    
    GlowColor = getRGBA();
    BlurX = getFIXED();
    BlurY = getFIXED();
    Strength = getFIXED8();
    
    InnerShadow = getUBits(1);
    Knockout    = getUBits(1);
    CompositeSource = getUBits(1);
    Passes = getUBits(5);

    filterObject["GlowColor"]       = Color2String(GlowColor, 1);
    filterObject["BlurX"]           = BlurX;
    filterObject["BlurY"]           = BlurY;
    filterObject["Strength"]        = Strength;
    filterObject["InnerShadow"]     = InnerShadow;
    filterObject["Knockout"]        = Knockout;
    filterObject["CompositeSource"] = CompositeSource;
    filterObject["Passes"]          = Passes;
    
    DEBUGMSG("GlowColor : \"%#04x\",\nBlurX : %f,\nBlurY : %f,\nStrength : %f", GlowColor, BlurX, BlurY, Strength);
    DEBUGMSG(",\nInnerShadow : %d,\nKnockout : %d,\nCompositeSource : %d,\nPasses : %d", InnerShadow, Knockout, CompositeSource, Passes);
    
    DEBUGMSG("\n}");
    return TRUE;
}


int SWFParser::getBEVELFILTER(VObject &filterObject)
{
    filterObject.setTypeInfo("BEVELFILTER");
    DEBUGMSG("{ /* BEVELFILTER */\n");

    unsigned int ShadowColor, HighlightColor, InnerShadow, Knockout, CompositeSource, OnTop, Passes;
    
    float BlurX, BlurY, Angle, Distance, Strength;
    
    ShadowColor     = getRGBA();
    HighlightColor  = getRGBA();
    BlurX           = getFIXED();
    BlurY           = getFIXED();
    Angle           = getFIXED();
    Distance        = getFIXED();
    Strength        = getFIXED8();
    
    InnerShadow     = getUBits(1);
    Knockout        = getUBits(1);
    CompositeSource = getUBits(1);
    OnTop           = getUBits(1);
    Passes          = getUBits(4);
    
    filterObject["ShadowColor"]     = Color2String(ShadowColor, 1);
    filterObject["HighlightColor"]  = Color2String(HighlightColor, 1);
    filterObject["BlurX"]           = BlurX;
    filterObject["BlurY"]           = BlurY;
    filterObject["Angle"]           = Angle;
    filterObject["Distance"]        = Distance;
    filterObject["Strength"]        = Strength;
    filterObject["InnerShadow"]     = InnerShadow;
    filterObject["Knockout"]        = Knockout;
    filterObject["CompositeSource"] = CompositeSource;
    filterObject["OnTop"]           = OnTop;
    filterObject["Passes"]          = Passes;
    
    DEBUGMSG("ShadowColor : \"%#04x\",\nHighlightColor : \"%#04x\",\nBlurX : %f,\nBlurY : %f,\nAngle : %f,\nDistance : %f,\nStrength : %f", ShadowColor, HighlightColor, BlurX, BlurY, Angle, Distance, Strength);
    DEBUGMSG(",\nInnerShadow : %d,\nKnockout : %d,\nCompositeSource : %d,\nOnTop : %d,\nPasses : %d", InnerShadow, Knockout, CompositeSource, OnTop, Passes);
    
    DEBUGMSG("\n}");
    return TRUE;
}

int SWFParser::getGRADIENTGLOWFILTER(VObject &filterObject)
{
    filterObject.setTypeInfo("GRADIENTGLOWFILTER");
    DEBUGMSG("{ /* GRADIENTGLOWFILTER */\n");
    
    unsigned int NumColors, InnerShadow, Knockout, CompositeSource, OnTop, Passes;
    
    float BlurX, BlurY, Angle, Distance, Strength;
    
    NumColors = getUI8();
    
    filterObject["NumColors"] = NumColors;
    DEBUGMSG("NumColors : %d", NumColors);
    
    unsigned int GradientColors[NumColors], GradientRatio[NumColors];
    
    DEBUGMSG(",\nGradientColors : [\n");
    
    for (int i = 0; i < NumColors; i++) {
        GradientColors[i] = getRGBA();
        
        filterObject["GradientColors"][i] = Color2String(GradientColors[i], 1);
        DEBUGMSG("\"%#04x\"", GradientColors[i]);
        if (i < (NumColors - 1))
            DEBUGMSG(", ");
    }
    
    DEBUGMSG("\n]");

    DEBUGMSG(",\nGradientRatio : [\n");
    for (int i = 0; i < NumColors; i++) {
        GradientRatio[i] = getUI8();
        
        filterObject["GradientRatio"][i] = GradientRatio[i];
        DEBUGMSG("%d", GradientRatio[i]);
        if (i < (NumColors - 1))
            DEBUGMSG(", ");
    }
    DEBUGMSG("\n]");    
    
    BlurX           = getFIXED();
    BlurY           = getFIXED();
    Angle           = getFIXED();
    Distance        = getFIXED();
    Strength        = getFIXED8();
    
    InnerShadow     = getUBits(1);
    Knockout        = getUBits(1);
    CompositeSource = getUBits(1);
    OnTop           = getUBits(1);
    Passes          = getUBits(4);
    
    filterObject["BlurX"]           = BlurX;
    filterObject["BlurY"]           = BlurY;
    filterObject["Angle"]           = Angle;
    filterObject["Distance"]        = Distance;
    filterObject["Strength"]        = Strength;
    filterObject["InnerShadow"]     = InnerShadow;
    filterObject["Knockout"]        = Knockout;
    filterObject["CompositeSource"] = CompositeSource;
    filterObject["OnTop"]           = OnTop;
    filterObject["Passes"]          = Passes;
    
    DEBUGMSG(",\nBlurX : %f,\nBlurY : %f,\nAngle : %f,\nDistance : %f,\nStrength : %f", BlurX, BlurY, Angle, Distance, Strength);
    DEBUGMSG(",\nInnerShadow : %d,\nKnockout : %d,\nCompositeSource : %d,\nOnTop : %d,\nPasses : %d", InnerShadow, Knockout, CompositeSource, OnTop, Passes);
    
    DEBUGMSG("\n}");
    return TRUE;
}

int SWFParser::getGRADIENTBEVELFILTER(VObject &filterObject)
{
    filterObject.setTypeInfo("GRADIENTBEVELFILTER");
    DEBUGMSG("{ /* GRADIENTBEVELFILTER */\n");
    
    unsigned int NumColors, InnerShadow, Knockout, CompositeSource, OnTop, Passes;
    
    float BlurX, BlurY, Angle, Distance, Strength;
    
    NumColors = getUI8();
    
    filterObject["NumColors"] = NumColors;
    DEBUGMSG("NumColors : %d", NumColors);
    
    unsigned int GradientColors[NumColors], GradientRatio[NumColors];
    
    DEBUGMSG(",\nGradientColors : [\n");
    for (int i = 0; i < NumColors; i++) {
        GradientColors[i] = getRGBA();
        
        filterObject["GradientColors"][i] = Color2String(GradientColors[i], 1);
        DEBUGMSG("\"%#04x\"", GradientColors[i]);
        if (i < (NumColors - 1))
            DEBUGMSG(", ");
    }

    DEBUGMSG("\n]");
    
    DEBUGMSG(",\nGradientRatio : [\n");
    
    for (int i = 0; i < NumColors; i++) {
        GradientRatio[i] = getUI8();
        
        filterObject["GradientRatio"][i] = GradientRatio[i];
        DEBUGMSG("%d", GradientRatio[i]);
        if (i < (NumColors - 1))
            DEBUGMSG(", ");
    }
    DEBUGMSG("\n]");
    
    BlurX       = getFIXED();
    BlurY       = getFIXED();
    Angle       = getFIXED();
    Distance    = getFIXED();
    Strength    = getFIXED8();
    
    InnerShadow     = getUBits(1);
    Knockout        = getUBits(1);
    CompositeSource = getUBits(1);
    OnTop           = getUBits(1);
    Passes          = getUBits(4);

    filterObject["BlurX"]           = BlurX;
    filterObject["BlurY"]           = BlurY;
    filterObject["Angle"]           = Angle;
    filterObject["Distance"]        = Distance;
    filterObject["Strength"]        = Strength;
    filterObject["InnerShadow"]     = InnerShadow;
    filterObject["Knockout"]        = Knockout;
    filterObject["CompositeSource"] = CompositeSource;
    filterObject["OnTop"]           = OnTop;
    filterObject["Passes"]          = Passes;
    
    DEBUGMSG(",\nBlurX : %f,\nBlurY : %f,\nAngle : %f,\nDistance : %f,\nStrength : %f", BlurX, BlurY, Angle, Distance, Strength);
    DEBUGMSG(",\nInnerShadow : %d,\nKnockout : %d,\nCompositeSource : %d,\nOnTop : %d,\nPasses : %d", InnerShadow, Knockout, CompositeSource, OnTop, Passes);
    
    DEBUGMSG("\n}");
    return TRUE;
}

///////////////////////////////////////////////////////////
// filter operations above is used by getFILTERLIST() only.
///////////////////////////////////////////////////////////
int SWFParser::getFILTERLIST(VObject &filterlistObject)    // SWF8 or later
{
    filterlistObject.setTypeInfo("FILTERLIST");
    DEBUGMSG("{ /* FILTERLIST */\n");
    
    // FILTERLIST FIXME
    unsigned int NumberOfFilters;
    
    NumberOfFilters = getUI8();
    
    filterlistObject["NumberOfFilters"] = NumberOfFilters;    
    DEBUGMSG("NumberOfFilters : %d", NumberOfFilters);
    
    if (NumberOfFilters > 0) {
        DEBUGMSG(",\nFilter : [\n");
        VObject &filters = filterlistObject["Filter"];

        // FILTER[]
        for (int i = 0; i < NumberOfFilters; i++) {

            VObject &filter = filters[i];
            
            unsigned int FilterID;
            
            FilterID = getUI8();
            
            filter["FilterID"] = FilterID;
            DEBUGMSG("{ /* FILTER */\nFilterID : %d,\n", FilterID);
            
            switch (FilterID) {
                case 0: // DropShadowFilter
                    DEBUGMSG("DropShadowFilter : ");
                    getDROPSHADOWFILTER(filter["DropShadowFilter"]);
                    break;
                case 1: // BlurFilter
                    DEBUGMSG("BlurFilter : ");
                    getBLURFILTER(filter["BlurFilter"]);
                    break;
                case 2: // GlowFilter
                    DEBUGMSG("GlowFilter : ");
                    getGLOWFILTER(filter["GlowFilter"]);
                    break;
                case 3: // BevelFilter
                    DEBUGMSG("BevelFilter : ");
                    getBEVELFILTER(filter["BevelFilter"]);
                    break;
                case 4: // GradientGlowFilter
                    DEBUGMSG("GradientGlowFilter : ");
                    getGRADIENTGLOWFILTER(filter["GradientGlowFilter"]);
                    break;
                case 5: // ConvolutionFilter
                    DEBUGMSG("ConvolutionFilter : ");
                    getCONVOLUTIONFILTER(filter["ConvolutionFilter"]);
                    break;
                case 6: // ColorMatrixFilter
                    DEBUGMSG("ColorMatrixFilter : ");
                    getCOLORMATRIXFILTER(filter["ColorMatrixFilter"]);
                    break;
                case 7: // GradientBevelFilter
                    DEBUGMSG("GradientBevelFilter : ");
                    getGRADIENTBEVELFILTER(filter["GradientBevelFilter"]);
                    break;
                default:
                    DEBUGMSG("Undefine FilterID(%d) - Fatal Error", FilterID); // Assert
                    ASSERT(1);
                    return FALSE;
            } // switch
        
            DEBUGMSG("\n}");
        
            if ( i < (NumberOfFilters - 1))
                DEBUGMSG(",\n");
        } // for
        DEBUGMSG("\n]");
    }
    
    DEBUGMSG("\n}"); // End of FILTERLIST
    
    return TRUE;
}


///////////////////////////////////////
//// Sound Operation
///////////////////////////////////////
int SWFParser::getSOUNDINFO(VObject &soundInfoObject)
{
    soundInfoObject.setTypeInfo("SOUNDINFO");
    DEBUGMSG("{ /* SOUNDINFO */\n");
    
    unsigned int SyncStop, SyncNoMultiple, HasEnvelope, HasLoops, HasOutPoint, HasInPoint;
    
    // SOUNDINFO
    getUBits(2);    // Reserved 0
    SyncStop        = getUBits(1);  // Stop the sound now
    SyncNoMultiple  = getUBits(1);  // Don't start the sound if already playing
    HasEnvelope     = getUBits(1);
    HasLoops        = getUBits(1);
    HasOutPoint     = getUBits(1);
    HasInPoint      = getUBits(1);

    DEBUGMSG("SyncStop : %d", SyncStop);
    DEBUGMSG(",\nSyncNoMultiple : %d", SyncNoMultiple);
    DEBUGMSG(",\nHasEnvelope : %d", HasEnvelope);
    DEBUGMSG(",\nHasLoops : %d", HasLoops);
    DEBUGMSG(",\nHasOutPoint : %d", HasOutPoint);
    DEBUGMSG(",\nHasInPoint : %d", HasInPoint);
    
    soundInfoObject["SyncStop"] = SyncStop;
    soundInfoObject["SyncNoMultiple"] = SyncNoMultiple;
    soundInfoObject["HasEnvelope"] = HasEnvelope;
    soundInfoObject["HasLoops"] = HasLoops;
    soundInfoObject["HasOutPoint"] = HasOutPoint;
    soundInfoObject["HasInPoint"] = HasInPoint;
    
    unsigned int InPoint, OutPoint, LoopCount, EnvPoints;
    
    if (HasInPoint) {
        InPoint = getUI32();
        
        soundInfoObject["InPoint"] = InPoint;
        DEBUGMSG(",\nInPoint : %d", InPoint);
    }
    if (HasOutPoint) {
        OutPoint = getUI32();
        
        soundInfoObject["OutPoint"] = OutPoint;
        DEBUGMSG(",\nOutPoint : %d", OutPoint);
    }
    if (HasLoops) {
        LoopCount = getUI16();
        
        soundInfoObject["LoopCount"] = LoopCount;
        DEBUGMSG(",\nLoopCount = %d", LoopCount);
    }
    if (HasEnvelope) {
        EnvPoints = getUI8();   // Sound Envelope point count
        
        soundInfoObject["EnvPoints"] = EnvPoints;
        DEBUGMSG(",\nEnvPoints : %d", EnvPoints);

        DEBUGMSG(",\nEnvelopeRecords : [\n");

        VObject &envelopeRecords = soundInfoObject["EnvelopeRecords"];
        // SOUNDENVELOPE[EnvPoints]
        for (int i = 0; i < EnvPoints; i++) {
            unsigned int Pos44, LeftLevel, RightLevel;

            VObject &envRecord = envelopeRecords[i];
            envRecord.setTypeInfo("SOUNDENVELOPE");

            Pos44       = getUI32();
            LeftLevel   = getUI16();
            RightLevel  = getUI16();

            envRecord["Pos44"]      = Pos44;
            envRecord["LeftLevel"]  = LeftLevel;
            envRecord["RightLevel"] = RightLevel;

            DEBUGMSG("{ Pos44 : %d, LeftLevel : %d, RightLevel : %d }", i, Pos44, LeftLevel, RightLevel);
            if (i < (EnvPoints - 1))
                DEBUGMSG(",\n");
        }
        
        DEBUGMSG("\n]");
    }
    
    DEBUGMSG("\n}");
    
    return TRUE;
}
///////////////////////////////////////
//// ACTIONRECORD
///////////////////////////////////////
int SWFParser::getACTIONRECORD(VObject &actRecordObject)
{
	unsigned int ActionCode = 0, Length = 0;
	
    actRecordObject.setTypeInfo("ACTIONRECORD");
    
	ActionCode = getUI8();
	
	if (ActionCode & 0x80)
		Length = getUI16();
	
    actRecordObject["ActionCode"] = ActionCode;
    actRecordObject["Length"] = Length;
	
    skip(Length); // FIXME, need more effort to aciton models
	return ActionCode;
}

///////////////////////////////////////
//// Action Operation
///////////////////////////////////////
int SWFParser::getActionCodeAndLength(Action *action)
{
	unsigned int ActionCode = 0, Length = 0;
	
    action->ActionHeaderOffset = getStreamPos();
    
    ActionCode = getUI8();
    
    if (ActionCode & 0x80) {
        Length = getUI16();
    }
    
    //DEBUGMSG("ActionCode : %d, \nLength : %d\n", ActionCode, Length);
    
    action->ActionBodyOffset = getStreamPos();
    action->ActionHeaderLength = action->ActionBodyOffset - action->ActionHeaderOffset;
    
	action->ActionCode      = ActionCode;
	action->Length          = Length;
    action->NextActionPos   = action->ActionBodyOffset + Length; // Actually it point to the next Action record
	
	return TRUE;
}


///////////////////////////////////////
//// Tag Operation
///////////////////////////////////////
int SWFParser::getTagCodeAndLength(Tag *tag)
{
	unsigned int TagCode, TagLength;
    
    tag->TagHeaderOffset = getStreamPos();
    
	TagCode = getUI16();
	TagLength = TagCode & 0x3f; /* Lower 6 bits */
	
	if (TagLength == 0x3f) { /* RECORDHEADER(long) */
		TagLength = getUI32();
	}
	
	TagCode = TagCode >> 6; /* Upper 10 bits */
    
    //DEBUGMSG("TagCode : %d, \nTagLength : %d\n", TagCode, TagLength);
	tag->TagCode    = TagCode;
	tag->TagLength  = TagLength;
    
    tag->TagBodyOffset = getStreamPos();
    tag->TagHeaderLength = tag->TagBodyOffset - tag->TagHeaderOffset;
    tag->NextTagPos = tag->TagBodyOffset + TagLength; // Actually it points to the next tag
	
	return TRUE;
}
