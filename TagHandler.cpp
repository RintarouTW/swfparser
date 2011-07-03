/*
 *  TagHandler.cpp
 *  swfparser
 *
 *  Created by Rintarou on 2010/5/16.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "common.h"
#include "TagDefine.h"
#include "TagHandler.h"
#include "ActionHandler.h"

#include "Util.h"


//// TagCode : 0 - 91
//// 28 undefined tagcodes, 92 - 28 = 64 defined tagcodes
//// Lookup Table of Tag Name and Tag Handler Function
static TagHandlerInfo tagHandlerInfo[] = {
{ NULL,			"End"},                  // 0 - OK
{ NULL,			"ShowFrame"},            // 1 - OK
{ DefineShape,	"DefineShape"},          // 2 - OK
{ NULL,			"undefined-3"},          // 3 - OK
{ PlaceObject,	"PlaceObject"},          // 4 - OK
{ RemoveObject, "RemoveObject"},         // 5 - OK
{ DefineBits,	"DefineBits"},           // 6 - OK
{ DefineButton,	"DefineButton"},         // 7 - OK
{ JPEGTables,	"JPEGTables"},           // 8 - OK
{ SetBackgroundColor,	"SetBackgroundColor"},   // 9 - OK
{ DefineFont,	"DefineFont"},           // 10 - OK
{ DefineText,	"DefineText"},           // 11 - OK
{ DoAction,		"DoAction"},             // 12 - OK
{ DefineFontInfo,	"DefineFontInfo"},   // 13 - OK
{ DefineSound,	"DefineSound"},          // 14 - OK
{ StartSound,	"StartSound"},           // 15 - OK
{ NULL,			"undefined-16"},         // 16 - OK
{ DefineButtonSound, "DefineButtonSound"},    // 17 - OK
{ SoundStreamHead,	"SoundStreamHead"},      // 18 - OK
{ SoundStreamBlock,	"SoundStreamBlock"},     // 19 - OK
{ DefineBitsLossless,	"DefineBitsLossless"},   // 20 - OK
{ DefineBitsJPEG2,	"DefineBitsJPEG2"},  // 21 - OK
{ DefineShape,	"DefineShape2"},         // 22 - OK
{ DefineButtonCxform,	"DefineButtonCxform"},   // 23 - OK?
{ NULL,			"Protect"},              // 24 - OK
{ NULL,			"undefined-25"},         // 25 - OK
{ PlaceObject2, "PlaceObject2"},         // 26 - OK?
{ NULL,			"undefined-27"},         // 27 - OK
{ RemoveObject2,"RemoveObject2"},        // 28 - OK
{ NULL,			"undefined-29"},         // 29 - OK
{ NULL,			"undefined-30"},         // 30 - OK
{ NULL,			"undefined-31"},         // 31 - OK
{ DefineShape,	"DefineShape3"},         // 32 - OK
{ DefineText,	"DefineText2"},          // 33 - OK
{ DefineButton,	"DefineButton2"},        // 34 - OK?
{ DefineBitsJPEG2,	"DefineBitsJPEG3"},  // 35 - FIXME
{ DefineBitsLossless,	"DefineBitsLossless2"},  // 36 - FIXME
{ DefineEditText,	"DefineEditText"},   // 37 - OK
{ NULL,			"undefined-38"},         // 38 - OK
{ DefineSprite,	"DefineSprite"},         // 39 - OK
{ NULL,			"undefined-40"},         // 40 - OK
{ NULL,			"undefined-41"},         // 41 - OK
{ NULL,			"undefined-42"},         // 42 - OK
{ FrameLabel,	"FrameLabel"},           // 43 - OK
{ NULL,			"undefined-44"},         // 44 - OK
{ SoundStreamHead,	"SoundStreamHead2"}, // 45 - OK
{ DefineMorphShape,	"DefineMorphShape"}, // 46 - OK
{ NULL,			"undefined-47"},         // 47 - OK
{ DefineFont2,	"DefineFont2"},          // 48 - OK
{ NULL,			"undefined-49"},         // 49 - OK
{ NULL,			"undefined-50"},         // 50 - OK
{ NULL,			"undefined-51"},         // 51 - OK
{ NULL,			"undefined-52"},         // 52 - OK
{ NULL,			"undefined-53"},         // 53 - OK
{ NULL,			"undefined-54"},         // 54 - OK
{ NULL,			"undefined-55"},         // 55 - OK
{ ExportAssets,	"ExportAssets"},         // 56 - OK
{ ImportAssets,	"ImportAssets"},         // 57 - OK
{ EnableDebugger,	"EnableDebugger"},   // 58 - OK
{ DoInitAction,	"DoInitAction"},         // 59 - OK
{ DefineVideoStream, "DefineVideoStream"},    // 60 - OK
{ VideoFrame,	"VideoFrame"},           // 61 - OK
{ DefineFontInfo,	"DefineFontInfo2"},  // 62 - OK
{ NULL,			"undefined-63"},         // 63 - OK
{ EnableDebugger, "EnableDebugger2"},    // 64 - OK
{ ScriptLimits,	"ScriptLimits"},         // 65 - OK
{ SetTabIndex,	"SetTabIndex"},          // 66 - OK
{ NULL,			"undefined-67"},         // 67 - OK
{ NULL,			"undefined-68"},         // 68 - OK
{ FileAttributes,	"FileAttributes"},   // 69 - OK
{ PlaceObject2,	"PlaceObject3"},         // 70 - OK
{ ImportAssets,	"ImportAssets2"},        // 71 - OK
{ NULL,			"undefined-72"},         // 72 - OK
{ DefineFontAlignZones,	"DefineFontAlignZones"}, // 73 - OK
{ CSMTextSettings,	"CSMTextSettings"},  // 74 - OK
{ DefineFont2,	"DefineFont3"},          // 75 - OK
{ SymbolClass,	"SymbolClass"},          // 76 - OK
{ MetatData,	"Metadata"},             // 77 - OK
{ DefineScalingGrid, "DefineScalingGrid"},    // 78 - OK
{ NULL,			"undefined-79"},         // 79 - OK
{ NULL,			"undefined-80"},         // 80 - OK
{ NULL,			"undefined-81"},         // 81 - OK
{ DoABC,		"DoABC"},                // 82 - FIXME
{ DefineShape,	"DefineShape4"},         // 83 - OK
{ DefineMorphShape, "DefineMorphShape2"},  // 84 - OK
{ NULL,			"undefined-85"},         // 85 - OK
{ DefineSceneAndFrameLabelData,	"DefineSceneAndFrameLabelData"}, // 86 - OK
{ DefineBinaryData,	"DefineBinaryData"}, // 87 - OK
{ DefineFontName,	"DefineFontName"},   // 88 - OK
{ StartSound,	"StartSound2"},          // 89 - OK
{ DefineBitsJPEG2,	"DefineBitsJPEG4"},  // 90 - FIXME
{ NULL,			"DefineFont4"}           // 91 - FIXME
};


//////////////////////////////////////////
//// Tag Handler Functions
//////////////////////////////////////////

int EnableDebugger(Tag *tag, SWFParser *parser, VObject &tagObject)  // 58, EnableDebugger2 = 64
{
    if (tag->TagCode == TAG_ENABLEDEBUGGER2) // EnableDebugger2
        parser->getUI16();  // Reserved, always 0
    
    DEBUGMSG("Password : ");
    tagObject["Password"] = parser->getSTRING();    // MD5-encrypted password
    
    return TRUE;
}

/////////////////////////////////////////

int MetatData(Tag *tag, SWFParser *parser, VObject &tagObject) // 77
{
    
    DEBUGMSG("MetatData : ");    
    tagObject["MetatData"] = parser->getSTRING();
    
    
    //tagObject["MetatData"] = str.c_str(); //"FIXME";  // FIXME, need to be encoded to JSON string, where " => \"
    
    return TRUE;
}

int ScriptLimits(Tag *tag, SWFParser *parser, VObject &tagObject) // 65 = 0x41 (SWF7 or later)
{
    unsigned int MaxRecursionDepth, ScriptTimeoutSeconds;
    
    MaxRecursionDepth = parser->getUI16();
    ScriptTimeoutSeconds = parser->getUI16();

    tagObject["MaxRecursionDepth"]      = MaxRecursionDepth;
    tagObject["ScriptTimeoutSeconds"]   = ScriptTimeoutSeconds;
    DEBUGMSG("MaxRecursionDepth : %d,\nScriptTimeoutSeconds : %d", MaxRecursionDepth, ScriptTimeoutSeconds);
    
    return TRUE;
}

int SetTabIndex(Tag *tag, SWFParser *parser, VObject &tagObject) // 66 = 0x42 (SWF7 or later)
{
    unsigned int Depth, TabIndex;
    
    Depth = parser->getUI16();  // Depth of character
    TabIndex = parser->getUI16();

    tagObject["Depth"]      = Depth;
    tagObject["TabIndex"]   = TabIndex;
    DEBUGMSG("Depth : %d,\nTabIndex : %d", Depth, TabIndex);
    
    return TRUE;
}

int SetBackgroundColor(Tag *tag, SWFParser *parser, VObject &tagObject) // 9 = 0x09
{
	unsigned int rgb = 0;
    
	rgb = parser->getRGB();

	DEBUGMSG("BackgroundColor : \"%#03x\"", rgb);
	tagObject["BackgroundColor"] = Color2String(rgb, 0);
	return TRUE;
}

int FileAttributes(Tag *tag, SWFParser *parser, VObject &tagObject) // 69 = 0x45
{
	int i = 0;
    
	parser->getUBits(1); // Reserved must be 0
	
	i = parser->getUBits(1); // UseDirectBlit
    tagObject["UseBirectBlit"] = i;
	DEBUGMSG("UseBirectBlit : %d,\n", i);	
    
	i = parser->getUBits(1); // UseGPU
    tagObject["UseGPU"] = i;
	DEBUGMSG("UseGPU : %d,\n", i);
	
	i = parser->getUBits(1); // HasMetadata
    tagObject["HasMetadata"] = i;
	DEBUGMSG("HasMetadata : %d,\n", i);
	
	i = parser->getUBits(1); // ActionScript3
    tagObject["ActionScript3"] = i;
	DEBUGMSG("ActionScript3 : %d,\n", i);
	
	parser->getUBits(2); // Reserved	
	
	i = parser->getUBits(1); // UseNetwork
    tagObject["UseNetwork"] = i;
    DEBUGMSG("UseNetwork : %d", i);
	
	parser->getUBits(24); // Reserved must be 0
	    
	return TRUE;
}


int ExportAssets(Tag *tag, SWFParser *parser, VObject &tagObject) // 56 = 0x38 (SWF5 or later)
{
    unsigned int Count;
    
    Count = parser->getUI16(); // Number of assets to export
    
    tagObject["Count"] = Count;
    DEBUGMSG("Count : %d%c\n", Count, Count ? ',' : ' ');

    char tagIDString[30] = "";
    char nameIDString[30] = "";
    
    for (int i = 0; i < Count; i++) {
        snprintf(tagIDString, 30, "Tag%d", i+1);
        snprintf(nameIDString, 30, "Name%d", i+1);

        unsigned int TagID;
        
        TagID = parser->getUI16();
        tagObject[tagIDString] = TagID;
        
        DEBUGMSG("TagID : %d,\nName : ", TagID);
        tagObject[nameIDString] = parser->getSTRING();
        DEBUGMSG("%c\n", (i < (Count-1)) ? ',' : ' ');
    }
    return TRUE;
}

// ImportAssets  = 57 = 0x39, used between SWF5 to SWF7
// ImportAssets2 = 71 = 0x47, used in SWF8 or later
int ImportAssets(Tag *tag, SWFParser *parser, VObject &tagObject)
{
    
    if (tag->TagCode == TAG_IMPORTASSETS2) { // ImportAssets2 only
        DEBUGMSG("URL : ");
        
        tagObject["URL"] = parser->getSTRING(); // URL where the source SWF file can be found
        parser->getUI8(); // Reserved, must be 1
        parser->getUI8(); // Reserved, must be 0
    }
    
    unsigned int Count;
    
    Count = parser->getUI16(); // Number of assets to export
    
    tagObject["Count"] = Count;
    DEBUGMSG("Count : %d%c\n", Count, Count ? ',' : ' ');

    char tagIDString[30] = "";
    char nameIDString[30] = "";
    
    for (int i = 0; i < Count; i++) {
        snprintf(tagIDString, 30, "Tag%d", i+1);
        snprintf(nameIDString, 30, "Name%d", i+1);

        unsigned int TagID;

        TagID = parser->getUI16();
        tagObject[tagIDString] = TagID;
        
        DEBUGMSG("TagID : %d,\nName : ", TagID);
        tagObject[nameIDString] = parser->getSTRING();
        DEBUGMSG("%c\n", (i < (Count-1)) ? ',' : ' ');
    }
    return TRUE;
}


int SymbolClass(Tag *tag, SWFParser *parser, VObject &tagObject) // 76 = 0x4c (SWF9 or later)
{
    unsigned int NumSymbols;
    
    NumSymbols = parser->getUI16(); // Number of symbols that will be associtated by this tag.
    
    tagObject["NumSymbols"] = NumSymbols;
    DEBUGMSG("NumSymbols : %d%c\n", NumSymbols, NumSymbols ? ',' : ' ');

    char tagIDString[30] = "";
    char nameIDString[30] = "";
    
    for (int i = 0; i < NumSymbols; i++) {
        snprintf(tagIDString, 30, "Tag%d", i+1);
        snprintf(nameIDString, 30, "Name%d", i+1);

        unsigned int TagID;
        
        TagID = parser->getUI16();
        tagObject[tagIDString] = TagID;
        
        DEBUGMSG("TagID : %d,\nAS3 Class Name : ", TagID);
        tagObject[nameIDString] = parser->getSTRING();
        DEBUGMSG("%c\n", (i < (NumSymbols-1)) ? ',' : ' ');
    }
    return TRUE;
}




//////////////////////////////////////////
//// DefineShape  : 2  = 0x02
//// DefineShape2 : 22 = 0x16 (SWF2 or later)
//// DefineShape3 : 32 = 0x20 (SWF3 or later)
//// DefineShape4 : 83 = 0x53 (SWF8 or later)
//////////////////////////////////////////
int DefineShape(Tag *tag, SWFParser *parser, VObject &tagObject) // 2 = 0x02
{
	unsigned int ShapeId = 0;
    
	ShapeId = parser->getUI16();

    tagObject["ShapeId"] = ShapeId;
	DEBUGMSG("ShapeId : %d,\nShapeBounds : ", ShapeId);

	parser->getRECT(tagObject["ShapeBounds"]); // ShapeBounds

    if (tag->TagCode == TAG_DEFINESHAPE4) { // DefineShape4 only
        DEBUGMSG(",\nEdgeBounds : ");
        parser->getRECT(tagObject["EdgeBounds"]); // EdgeBounds
        
        parser->getUBits(5); // Reserved. Must be 0
        
        int UsesFillWindingRule, UsesNonScalingStrokes, UsesScalingStrokes;
        
        UsesFillWindingRule     = parser->getUBits(1);
        UsesNonScalingStrokes   = parser->getUBits(1);
        UsesScalingStrokes      = parser->getUBits(1);
        
        DEBUGMSG(",\nUsesFillWindingRule : %d", UsesFillWindingRule);
        DEBUGMSG(",\nUsesNonScalingStrokes : %d", UsesNonScalingStrokes);
        DEBUGMSG(",\nUsesScalingStrokes : %d", UsesScalingStrokes);
        
        tagObject["UsesFillWindingRule"]    = UsesFillWindingRule;
        tagObject["UsesNonScalingStrokes"]  = UsesNonScalingStrokes;
        tagObject["UsesScalingStrokes"]     = UsesScalingStrokes;
    }
    
	DEBUGMSG(",\nShapes : ");
	parser->getSHAPEWITHSTYLE(tag, tagObject["Shapes"]);
    
	return TRUE;
}


//// DefineButton = 7
//// DefineButton = 34
int DefineButton(Tag *tag, SWFParser *parser, VObject &tagObject)  // 7 = 0x07
{
    unsigned int ButtonId;

    ButtonId = parser->getUI16();

    tagObject["ButtonId"] = ButtonId;
    DEBUGMSG("ButtonId : %d", ButtonId);
    
    unsigned int TrackAsMenu, ActionOffset; // Used by DefineButton2
    
    if (tag->TagCode == TAG_DEFINEBUTTON2) { // DefineButton2 only
        TrackAsMenu = parser->getUI8() & 1;
        ActionOffset = parser->getUI16();
        
        tagObject["TrackAsMenu"]    = TrackAsMenu;
        tagObject["ActionOffset"]   = ActionOffset;
        DEBUGMSG(",\nTrackAsMenu : %d,\nActionOffset : %d", TrackAsMenu, ActionOffset);
    }
    
    DEBUGMSG(",\nCharacters : [\n");
    // Characters : BUTTONRECORD[]
    unsigned int recNo = 0;
    while (parser->getBUTTONRECORD(tag, tagObject["Chraracters"][recNo++]))
        DEBUGMSG(",\n");

    DEBUGMSG("\n]");
    
    if (tag->TagCode == TAG_DEFINEBUTTON) { // DefineButton
        DEBUGMSG(",\nActions : [\n");
        // Actions : ACTIONRECORD[]
        recNo = 0;
        while (parser->getACTIONRECORD(tagObject["Actions"][recNo++]))
            DEBUGMSG(",\n");
        
        DEBUGMSG("\n]");
    }
    
    if (tag->TagCode == TAG_DEFINEBUTTON2) { // DefineButton2
        // Actions : BUTTONCONDACTION[]
        if (ActionOffset) {
            DEBUGMSG(",\nActions : [\n");
            recNo = 0;
            while (parser->getBUTTONCONDACTION(tagObject["Actions"][recNo++]))
                DEBUGMSG(",\n");
            DEBUGMSG("\n]");
        }
    }
    
    return TRUE;
}


int DefineButtonCxform(Tag *tag, SWFParser *parser, VObject &tagObject) // 23 = 0x17 (SWF2)
{
    unsigned int ButtonId;
    
    ButtonId = parser->getUI16();
    
    tagObject["ButtonId"] = ButtonId;
    DEBUGMSG("ButtonId : %d\nButtonColorTransform : ", ButtonId);
    
    // ButtonColorTransform : CXFORM
    parser->getCXFORM(tagObject["ButtonColorTransform"]);
    
    return TRUE;
}



int DefineSprite(Tag *tag, SWFParser *parser, VObject &tagObject) // 39 = 0x27 (SWF3)
{
	unsigned int ShapeId = 0, FrameCount = 0;

	ShapeId = parser->getUI16();
	FrameCount = parser->getUI16();
    
    tagObject["ShapeId"] = ShapeId;
    tagObject["FrameCount"] = FrameCount;
	DEBUGMSG("ShapeId : %d,\nFrameCount : %d,\n", ShapeId, FrameCount);
    
    // FIXME for better design
    // ControlTags : Tag[]

    unsigned int TagCode = 0, TagLength = 0, tagNo = 0;
    TagHandler tagHandler;
    DEBUGMSG("ControlTags : [\n");
    
    VObject &controlTags = tagObject["ControlTags"];
    do {
        Tag tag2;
        
        parser->getTagCodeAndLength(&tag2);
        
        TagCode		= tag2.TagCode;
        TagLength	= tag2.TagLength;
        
        controlTags[tagNo]["TagCode"]   = TagCode;
        controlTags[tagNo]["TagLength"] = TagLength;
        
        DEBUGMSG("{\n");
        tagHandler.parse(&tag2, parser, controlTags[tagNo]);
        DEBUGMSG("}%c\n", TagCode ? ',' : ' '); // End of Tag
        tagNo++;
    } while ( TagCode != 0x0); /* Parse untile END Tag(0x0) */
    DEBUGMSG("]\n");
        
    return TRUE;
}


int DefineScalingGrid(Tag *tag, SWFParser *parser, VObject &tagObject) // 78 = 0x4e (SWF8)
{
    unsigned int CharacterId;
    
    CharacterId = parser->getUI16();
    
    tagObject["CharacterId"] = CharacterId;
    DEBUGMSG("CharacterId : %d\nSplitter : ", CharacterId);
    
    parser->getRECT(tagObject["Splitter"]);
    
    return TRUE;
}

//////////////////////////////////////////
//// DefineMorphShape  = 46 = 0x2e (SWF3)
//// DefineMorphShape2 = 84 = 0x54 (SWF8)
//////////////////////////////////////////
int DefineMorphShape(Tag *tag, SWFParser *parser, VObject &tagObject) // 2 = 0x02
{
	unsigned int CharacterID = 0, Offset, UsesNonScalingStrokes, UsesScalingStrokes;
    
    CharacterID = parser->getUI16();
    
    tagObject["CharacterID"] = CharacterID;
	DEBUGMSG("CharacterID : %d,\nStartBounds : ", CharacterID);
    
	parser->getRECT(tagObject["StartBounds"]); // StartBounds
    
    DEBUGMSG(",\nEndBounds : ");
    parser->getRECT(tagObject["EndBounds"]); // EndBounds

    if (tag->TagCode == TAG_DEFINEMORPHSHAPE2) { // DefineMorphShape2
        DEBUGMSG(",\nStartEdgeBounds : ");
        parser->getRECT(tagObject["StartEdgeBounds"]); // StartEdgeBounds
        
        DEBUGMSG(",\nEndEdgeBounds : ");
        parser->getRECT(tagObject["EndEdgeBounds"]); // EndEdgeBounds
        
        parser->getUBits(6); // Reserved
        
        UsesNonScalingStrokes = parser->getUBits(1);
        
        tagObject["UsesNonScalingStrokes"] = UsesNonScalingStrokes;
        DEBUGMSG(",\nUsesNonScalingStrokes : %d", UsesNonScalingStrokes);

        UsesScalingStrokes = parser->getUBits(1);
        
        tagObject["UsesScalingStrokes"] = UsesScalingStrokes;
        DEBUGMSG(",\nUsesScalingStrokes : %d", UsesScalingStrokes);
    }

    Offset = parser->getUI32(); // Offset to EndEdges

    tagObject["Offset"] = Offset;
    DEBUGMSG(",\nOffset : %d", Offset);

    DEBUGMSG(",\nMorphFillStyles : ");
    parser->getMORPHFILLSTYLEARRAY(tag, tagObject["MorphFillStyles"]);

    DEBUGMSG(",\nMorphLineStyles : ");
    parser->getMORPHLINESTYLEARRAY(tag, tagObject["MorphLineStyles"]);

    DEBUGMSG(",\nStartEdges : ");
    parser->getSHAPE(tag, tagObject["StartEdges"]);
    
    DEBUGMSG(",\nEndEdges : ");
    parser->getSHAPE(tag, tagObject["EndEdges"]);

    return TRUE;
}


//////////////////////////////////////////
//// PlaceObject SWF1/2/3
//////////////////////////////////////////
int PlaceObject(Tag *tag, SWFParser *parser, VObject &tagObject) // 4 = 0x04
{
	unsigned int CharacterId, Depth, TagEndPos;
    
	TagEndPos = parser->getStreamPos() + tag->TagLength;
    
	CharacterId = parser->getUI16();
	Depth = parser->getUI16();
    
    tagObject["CharacterId"] = CharacterId;
    tagObject["Depth"] = Depth;
    DEBUGMSG("CharacterId = %d\nDepth = %d\n", CharacterId, Depth);
	
    parser->getMATRIX(tagObject["Matrix"]); // Transform matrix data
	if (parser->getStreamPos() < TagEndPos)
		parser->getCXFORM(tagObject["ColorTransform"]);
	return TRUE;
}


//// PlaceObject2 SWF3 or later = 70
//// PlaceObject3 SWF8 or later = 26
int PlaceObject2(Tag *tag, SWFParser *parser, VObject &tagObject) // 26 = 0x1a
{
	unsigned int PlaceFlagHasClipActions, PlaceFlagHasClipDepth, PlaceFlagHasName, PlaceFlagHasRatio, PlaceFlagHasColorTransform, PlaceFlagHasMatrix, PlaceFlagHasCharacter, PlaceFlagHasMove;
    unsigned int PlaceFlagHasImage, PlaceFlagHasClassName, PlaceFlagHasCacheAsBitmap, PlaceFlagHasBlendMode, PlaceFlagHasFilterList;
	
	unsigned int Depth, CharacterId, Ratio, ClipDepth;
	
	parser->setByteAlignment();
	
	PlaceFlagHasClipActions		= parser->getUBits(1); // SWF5 and later (sprite characters only)
	PlaceFlagHasClipDepth		= parser->getUBits(1);
	PlaceFlagHasName			= parser->getUBits(1);
	PlaceFlagHasRatio			= parser->getUBits(1);
	PlaceFlagHasColorTransform	= parser->getUBits(1);
	PlaceFlagHasMatrix			= parser->getUBits(1);
	PlaceFlagHasCharacter		= parser->getUBits(1);
	PlaceFlagHasMove			= parser->getUBits(1);
    
    DEBUGMSG("PlaceFlagHasClipActions : %d,\n", PlaceFlagHasClipActions);
    DEBUGMSG("PlaceFlagHasClipDepth : %d,\n", PlaceFlagHasClipDepth);
    DEBUGMSG("PlaceFlagHasName : %d,\n", PlaceFlagHasName);
    DEBUGMSG("PlaceFlagHasRatio : %d,\n", PlaceFlagHasRatio);
    DEBUGMSG("PlaceFlagHasColorTransform : %d,\n", PlaceFlagHasColorTransform);
    DEBUGMSG("PlaceFlagHasMatrix : %d,\n", PlaceFlagHasMatrix);
    DEBUGMSG("PlaceFlagHasCharacter : %d,\n", PlaceFlagHasCharacter);
    DEBUGMSG("PlaceFlagHasMove : %d", PlaceFlagHasMove);
    
    tagObject["PlaceFlagHasClipActions"] = PlaceFlagHasClipActions;
    tagObject["PlaceFlagHasClipDepth"] = PlaceFlagHasClipDepth;
    tagObject["PlaceFlagHasName"] = PlaceFlagHasName;
    tagObject["PlaceFlagHasRatio"] = PlaceFlagHasRatio;
    tagObject["PlaceFlagHasColorTransform"] = PlaceFlagHasColorTransform;
    tagObject["PlaceFlagHasMatrix"] = PlaceFlagHasMatrix;
    tagObject["PlaceFlagHasCharacter"] = PlaceFlagHasCharacter;
    tagObject["PlaceFlagHasMove"] = PlaceFlagHasMove;
    
    if (tag->TagCode == TAG_PLACEOBJECT3) {   // PlaceObject3 only
        parser->getUBits(3);    // Reserved, must be 0
        
        PlaceFlagHasImage = parser->getUBits(1);        
        PlaceFlagHasClassName = parser->getUBits(1);
        PlaceFlagHasCacheAsBitmap = parser->getUBits(1);
        PlaceFlagHasBlendMode = parser->getUBits(1);
        PlaceFlagHasFilterList = parser->getUBits(1);
        
        DEBUGMSG(",\nPlaceFlagHasImage : %d", PlaceFlagHasImage);
        DEBUGMSG(",\nPlaceFlagHasClassName : %d", PlaceFlagHasClassName);
        DEBUGMSG(",\nPlaceFlagHasCacheAsBitmap : %d", PlaceFlagHasCacheAsBitmap);
        DEBUGMSG(",\nPlaceFlagHasBlendMode : %d", PlaceFlagHasBlendMode);
        DEBUGMSG(",\nPlaceFlagHasFilterList : %d", PlaceFlagHasFilterList);
        
        tagObject["PlaceFlagHasImage"] = PlaceFlagHasImage;
        tagObject["PlaceFlagHasClassName"] = PlaceFlagHasClassName;
        tagObject["PlaceFlagHasCacheAsBitmap"] = PlaceFlagHasCacheAsBitmap;
        tagObject["PlaceFlagHasBlendMode"] = PlaceFlagHasBlendMode;
        tagObject["PlaceFlagHasFilterList"] = PlaceFlagHasFilterList;
    }
	
	Depth = parser->getUI16();
    
    tagObject["Depth"] = Depth;
    DEBUGMSG(",\nDepth : %d", Depth);
    
    if (tag->TagCode == TAG_PLACEOBJECT3) {   // PlaceObject3 only
        // ClassName : String
        if (PlaceFlagHasClassName || (PlaceFlagHasImage & PlaceFlagHasCharacter)) {

            DEBUGMSG(",\nClassName : ");
            tagObject["ClassName"] = parser->getSTRING();
        }
    }
	
	if (PlaceFlagHasCharacter) {
		CharacterId = parser->getUI16();
        
        tagObject["CharacterId"] = CharacterId;
		DEBUGMSG(",\nCharacterId : %d", CharacterId);
	}
	
	if (PlaceFlagHasMatrix) {
        DEBUGMSG(",\nMatrix : ");
		parser->getMATRIX(tagObject["Matrix"]); // Transform matrix data
	}
	
	if (PlaceFlagHasColorTransform) {
        DEBUGMSG(",\nColorTransform : ");
		parser->getCXFORMWITHALPHA(tagObject["ColorTransform"]);
	}
	
	if (PlaceFlagHasRatio) {
		Ratio = parser->getUI16();
        
        tagObject["Ratio"] = Ratio;
		DEBUGMSG(",\nRatio : %d", Ratio);
	}
	
	if (PlaceFlagHasName) {
        DEBUGMSG(",\nName : ");
		tagObject["Name"] = parser->getSTRING();
	}
	
	if (PlaceFlagHasClipDepth) {
		ClipDepth = parser->getUI16();
        
        tagObject["ClipDepth"] = ClipDepth;
		DEBUGMSG(",\nClipDepth : %d", ClipDepth);
	}
    
    if (tag->TagCode == TAG_PLACEOBJECT3) {   // PlaceObject3 only
        if (PlaceFlagHasFilterList) {
            DEBUGMSG(",\nSurfaceFilterList : ");
            parser->getFILTERLIST(tagObject["SurfaceFilterList"]);
        }
        if (PlaceFlagHasBlendMode) {
            unsigned int BlendMode;
            
            BlendMode = parser->getUI8();
            
            tagObject["BlendMode"] = BlendMode;
            DEBUGMSG(",\nBlendMode : %d", BlendMode);
        }
        if (PlaceFlagHasCacheAsBitmap) {
            unsigned int BitmapCache;
            
            BitmapCache = parser->getUI8();
            
            tagObject["BitmapCache"] = BitmapCache;
            DEBUGMSG(",\nBitmapCache : %d", BitmapCache);
        }
    }
	
	if (PlaceFlagHasClipActions) {
        DEBUGMSG(",\nClipActions : ");
		parser->getCLIPACTIONS(tagObject["ClipActions"]);
	}
	
    
    if (tag->TagCode == TAG_PLACEOBJECT3) {
        if (tag->NextTagPos == (parser->getStreamPos() + 1))
            parser->getUI8(); // FIXME: wierd padding? not sure what's wrong.
    }
    
	return TRUE;
}


int RemoveObject(Tag *tag, SWFParser *parser, VObject &tagObject) // 5 = 0x05
{
	unsigned int CharacterId, Depth;
	
	CharacterId = parser->getUI16();
	Depth = parser->getUI16();
    
    tagObject["CharacterId"] = CharacterId;
    tagObject["Depth"] = Depth;
	DEBUGMSG("CharacterId : %d,\nDepth : %d\n", CharacterId, Depth);
	return TRUE;
}

int RemoveObject2(Tag *tag, SWFParser *parser, VObject &tagObject) // 28 = 0x1c
{
	unsigned int Depth;
	
	Depth = parser->getUI16();
    
    tagObject["Depth"] = Depth;
	DEBUGMSG("Depth : %d\n", Depth);
	
	return TRUE;
}

//////////////////////////////////////////
//// Bitmap (JPEG, PNG, GIF89a)
//////////////////////////////////////////

int DefineBits(Tag *tag, SWFParser *parser, VObject &tagObject) // 6 = 0x06
{
	unsigned int CharacterId;
	
	CharacterId = parser->getUI16();
    
    tagObject["CharacterId"] = CharacterId;
	DEBUGMSG("CharacterId : %d", CharacterId);
	

    tagObject["JPEGData"].setTypeInfo("JPEGData");	
    tagObject["JPEGData"] = new MemoryStream(parser->getStreamPosPtr(), tag->TagLength - 2, 0);   // readonly
    DEBUGMSG(",\nJPEGData : []\n");
	parser->dump(tag->TagLength - 2);
	parser->skip(tag->TagLength - 2); // JPEGData UI8[], CharacterId is UI16 = 2 bytes.
	
	return TRUE;
}

int JPEGTables(Tag *tag, SWFParser *parser, VObject &tagObject) // 8 = 0x08
{

    tagObject["JPEGData"].setTypeInfo("JPEGData");	
    tagObject["JPEGData"] = new MemoryStream(parser->getStreamPosPtr(), tag->TagLength, 0);   // readonly
    DEBUGMSG(",\nJPEGData : []\n");
    //parser->dump(tag->TagLength);
	parser->skip(tag->TagLength); // JPEGData UI8[]
	return TRUE;
}

int DefineBitsLossless(Tag *tag, SWFParser *parser, VObject &tagObject) // 20 = 0x14
{
	unsigned int CharacterId, BitmapFormat, BitmapWidth, BitmapHeight, BitmapColorTableSize;//, ColorTableRGB[256];
	
	CharacterId = parser->getUI16();
    
    tagObject["CharacterId"] = CharacterId;
	DEBUGMSG("CharacterId : %d,\n", CharacterId);
    
    BitmapFormat = parser->getUI8();    // 3 = 8-bit colormapped image (bytesPerPixel = 1)
                                        // 4 = 15-bit RGB image (bytesPerPixel = 2)
                                        // 5 = 24-bit RGB image (bytesPerPixel = 4)
    
    tagObject["BitmapFormat"] = BitmapFormat;
    DEBUGMSG("BitmapFormat : %d,\n", BitmapFormat);
    
    BitmapWidth = parser->getUI16();
    
    tagObject["BitmapWidth"] = BitmapWidth;
    DEBUGMSG("BitmapWidth : %d,\n", BitmapWidth);
    
    BitmapHeight = parser->getUI16();
    
    tagObject["BitmapHeight"] = BitmapHeight;
    DEBUGMSG("BitmapHeight : %d", BitmapHeight);
    
    
    
    if (BitmapFormat == 3) { // 8-bit colormapped image
        BitmapColorTableSize = parser->getUI8(); // This value is one less than the actual number of colors ins the color table for 256 colors.
        
        tagObject["BitmapColorTableSize"] = BitmapColorTableSize;
        DEBUGMSG(",\nBitmapColorTableSize : %d", BitmapColorTableSize);
    }
    
    
    tagObject["ZlibBitmapData"].setTypeInfo("ZlibBitmapData");
    unsigned int ZlibBitmapDataLength;
    
    ZlibBitmapDataLength = (tag->NextTagPos - parser->getStreamPos());
    
    MemoryStream ZlibBitmapData(parser->getStreamPosPtr(), ZlibBitmapDataLength, 0);   // this is the source zlib stream to be inflated. (Read Only)
    
#if 0    
    unsigned char *UncompressedZlibBitmapData = 0;
    unsigned int UncompressedZlibBitmapDataLength;
    
    //Util util;
    
    //inflate ZlibBitmapData
    // Consider to redesign Stream class and make VObject can hold a Stream instead of implement it's own streamValue;


    UncompressedZlibBitmapDataLength = inflate2Memory((Stream *)(&ZlibBitmapData), &UncompressedZlibBitmapData);
    
    tagObject["ZlibBitmapData"] = new MemoryStream(UncompressedZlibBitmapData, UncompressedZlibBitmapDataLength, 1);
#endif

    parser->seek(tag->NextTagPos);
    
    // FIXME
    /*
    if (BitmapFormat == 3) {    // ZlibBitmapData : COLORMAPDATA
        // ColorTableRGB : RGB[]
        // Todo: need to take care of different pixel format
        for (int i = 0; i <  (BitmapColorTableSize + 1); i++) {
            ColorTableRGB[i] = parser->getRGB();
            DEBUGMSG("ColorTableRGB[%d] = %03x\n", ColorTableRGB[%d]);
        }
        
        // ColormapPixelData : UI8[]
        // Row widths in ColormapPixelData[] are 32-bit word boundary
        
        
    } else { // Bitmap = 4 || 5 => ZlibBitmapData : BITMAPDATA
        // parser->getBITMAPDATA();        
    }
    */
    
    return TRUE;
}

// DefineBitsJPEG2
// DefineBitsJPEG3
// DefineBitsJPEG4
int DefineBitsJPEG2(Tag *tag, SWFParser *parser, VObject &tagObject) // 21 = 0x15, SWF2 or later
{
	unsigned int CharacterID, AlphaDataOffset, DeblockParam, alphaDataSize;
	
	CharacterID = parser->getUI16();
    
    tagObject["CharacterID"] = CharacterID;
	DEBUGMSG("CharacterID : %d,\n", CharacterID);
    
    if (tag->TagCode == TAG_DEFINEBITSJPEG2) {
        DEBUGMSG("ImageData : []\n");
        tagObject["ImageData"].setTypeInfo("ImageData");
        tagObject["ImageData"] = new MemoryStream(parser->getStreamPosPtr(), tag->TagLength - 2, 0);   // readonly
        parser->skip(tag->TagLength - 2);   // CharacterID = 2 bytes
    }
/*
#ifdef F2C
    parser->seek(tag->NextTagPos);
    return TRUE;
#endif
 */
    
    if (tag->TagCode == TAG_DEFINEBITSJPEG3) {
        AlphaDataOffset = parser->getUI32();
        tagObject["AlphaDataOffset"] = AlphaDataOffset;
        DEBUGMSG("AlphaDataOffset : %d\n", AlphaDataOffset);

        tagObject["ImageData"].setTypeInfo("ImageData");
        tagObject["ImageData"] = new MemoryStream(parser->getStreamPosPtr(), AlphaDataOffset, 0);   // readonly
        DEBUGMSG(",\nImageData : []\n");
        //parser->dump(AlphaDataOffset);
        parser->skip(AlphaDataOffset);
        
        //abort();
        
        
        // Zlib compressed alpha channel
        alphaDataSize = tag->TagLength - AlphaDataOffset - 6;    // CharacterID + AlphaDataOffset = 6 bytes
        
        tagObject["BitmapAlphaData"].setTypeInfo("BitmapAlphaData");
        tagObject["BitmapAlphaData"] = new MemoryStream(parser->getStreamPosPtr(), alphaDataSize, 0);   // readonly
        DEBUGMSG(",\nBitmapAlphaData : []\n");
/*        
        unsigned char *UncompressedZlibBitmapData;
        unsigned int UncompressedZlibBitmapDataLength;

        MemoryStream ZlibData(parser->getStreamPosPtr(), alphaDataSize, 0); // BitmapAlphaData is actually compressed by zlib.

        UncompressedZlibBitmapDataLength = inflate2Memory((Stream *)(&ZlibData), &UncompressedZlibBitmapData);
        fprintf(stderr, "JPEG3 - zlib length = %d\n", UncompressedZlibBitmapDataLength);
        tagObject["BitmapAlphaData"].setTypeInfo("BitmapAlphaData");
        tagObject["BitmapAlphaData"] = new MemoryStream(UncompressedZlibBitmapData, UncompressedZlibBitmapDataLength, 1);
        DEBUGMSG(",\nBitmapAlphaData : []\n");
 */
        //parser->dump(alphaDataSize);
        parser->skip(alphaDataSize);
    }
            
    if (tag->TagCode == TAG_DEFINEBITSJPEG4) {
            
        AlphaDataOffset = parser->getUI32();
        tagObject["AlphaDataOffset"] = AlphaDataOffset;
        DEBUGMSG("AlphaDataOffset : %d\n", AlphaDataOffset);
        
        DeblockParam = parser->getUI16();
        tagObject["DeblockParam"] = DeblockParam;
        DEBUGMSG("DeblockParam : %d", DeblockParam);
        
        tagObject["ImageData"].setTypeInfo("ImageData");
        tagObject["ImageData"] = new MemoryStream(parser->getStreamPosPtr(), AlphaDataOffset, 0);   // readonly
        DEBUGMSG(",\nImageData : []\n");
        //parser->dump(AlphaDataOffset);
        parser->skip(AlphaDataOffset);

        alphaDataSize = tag->TagLength - AlphaDataOffset - 8;    // CharacterID + AlphaDataOffset + DeblockParam = 8 bytes
        tagObject["BitmapAlphaData"].setTypeInfo("BitmapAlphaData");
        tagObject["BitmapAlphaData"] = new MemoryStream(parser->getStreamPosPtr(), alphaDataSize, 0);   // readonly
        DEBUGMSG(",\nBitmapAlphaData : []\n");
        //parser->dump(alphaDataSize);
        parser->skip(alphaDataSize);
    }
	
	return TRUE;
}

//////////////////////////////////////////
////// nGlyphs are set by DefineFont and also used by DefineFontInfo
//////////////////////////////////////////
static unsigned int nGlyphs = 0;

//// NumGlyphs are set by DefineFont2/3 and used by DefineFontAlignZones
//// nGlyphs are the same with NumGlyphs?????????? FIXME
static unsigned int NumGlyphs = 0;


int DefineFont(Tag *tag, SWFParser *parser, VObject &tagObject) // 10 = 0x10
{
	unsigned int FontID, firstEntry;
	
	FontID = parser->getUI16();
    
    tagObject["FontID"] = FontID;
    
	// get the first entry of OffsetTable
	// nGlyphs = first entry/2
	firstEntry = parser->getUI16();
	nGlyphs = firstEntry / 2;
	
	unsigned int OffsetTable[nGlyphs];
    
	OffsetTable[0] = firstEntry;
    
    tagObject["OffsetTable"][0] = OffsetTable[0];
	DEBUGMSG("FontID : %d,\nOffsetTable : [\n", FontID);
    
	// get the reset of entries of OffsetTable
	for (int i = 1; i < nGlyphs; i++) {
		OffsetTable[i] = parser->getUI16();
        
        tagObject["OffsetTable"][i] = OffsetTable[i];
		DEBUGMSG("%d", i, OffsetTable[i]);
        if (i < (nGlyphs - 1))
            DEBUGMSG(",\n");
	}
            
    DEBUGMSG("\n], GlyphShapeTable : [");
	// get GlyphShapeTable : SHAPE[nGlyph]
	for (int i = 0; i < nGlyphs; i++) {
		parser->getSHAPE(tag, tagObject["GlyphShapeTable"][i]);
        
        if (i < (nGlyphs - 1))
            DEBUGMSG(",\n");
	}
    DEBUGMSG("\n]");
	
	return TRUE;
}

// DefineFont2 = 48 = 0x30
// DefineFont3 = 75 = 0x4b
// The same with DefineFont2 except the 20 times resolution of GlyphShapeTable
int DefineFont2(Tag *tag, SWFParser *parser, VObject &tagObject) // 48 = 0x30
{
	unsigned int FontID, FontNameLen;
	
	FontID = parser->getUI16();
    
    tagObject["FontID"] = FontID;
	DEBUGMSG("FontID : %d", FontID);
	
	int FontFlagsHasLayout, FontFlagsSmallText, FontFlagsShiftJIS, FontFlagsANSI, FontFlagsItalic, FontFlagsBold, FontFlagsWideOffsets, FontFlagsWideCodes;
	
	FontFlagsHasLayout	= parser->getUBits(1); // FontFlagsHasLayout
	DEBUGMSG(",\nFontFlagsHasLayout : %d", FontFlagsHasLayout);
	
	FontFlagsShiftJIS	= parser->getUBits(1); // FontFlagsShiftJIS
	DEBUGMSG(",\nFontFlagsShiftJIS : %d", FontFlagsShiftJIS);
	
	FontFlagsSmallText	= parser->getUBits(1); // FontFlagsSmallText (SWF7 or later)
	DEBUGMSG(",\nFontFlagsSmallText : %d", FontFlagsSmallText);
	
	FontFlagsANSI		= parser->getUBits(1); // FontFlagsANSI
	DEBUGMSG(",\nFontFlagsANSI : %d", FontFlagsANSI);
	
	FontFlagsWideOffsets = parser->getUBits(1); // FontFlagsWideOffsets
	DEBUGMSG(",\nFontFlagsWideOffsets : %d", FontFlagsWideOffsets);
	
	FontFlagsWideCodes	= parser->getUBits(1); // FontFlagsWideCodes
	DEBUGMSG(",\nFontFlagsWideCodes : %d", FontFlagsWideCodes);
	
	FontFlagsItalic		= parser->getUBits(1); // FontFlagsItalic
	DEBUGMSG(",\nFontFlagsItalic : %d", FontFlagsItalic);
	
	FontFlagsBold		= parser->getUBits(1); // FontFlagsBold
	DEBUGMSG(",\nFontFlagsBold : %d", FontFlagsBold);
	
    tagObject["FontFlagsHasLayout"]     = FontFlagsHasLayout;
    tagObject["FontFlagsShiftJIS"]      = FontFlagsShiftJIS;
    tagObject["FontFlagsSmallText"]     = FontFlagsSmallText;
    tagObject["FontFlagsANSI"]          = FontFlagsANSI;
    tagObject["FontFlagsWideOffsets"]   = FontFlagsWideOffsets;
    tagObject["FontFlagsWideCodes"]     = FontFlagsWideCodes;
    tagObject["FontFlagsItalic"]        = FontFlagsItalic;
    tagObject["FontFlagsBold"]          = FontFlagsBold;
    
    DEBUGMSG(",\nLanguageCode : ");
	parser->getLANGUAGECODE();
	
	FontNameLen = parser->getUI8();
	tagObject["FontNameLen"]     = FontNameLen;
    
	char FontName[FontNameLen];
	
	DEBUGMSG(",\nFontNameLen : %d,\nFontName : ", FontNameLen);
	for (int i = 0; i < FontNameLen; i++) {
		FontName[i] = parser->getUI8();
	}
	DEBUGMSG("\"%s\"", FontName);
	tagObject["FontName"]     = FontName;
    
	NumGlyphs = parser->getUI16();
    tagObject["NumGlyphs"]     = NumGlyphs;
    
	DEBUGMSG(",\nNumGlyphs : %d,\nOffsetTable : [", NumGlyphs);
	
	//// OffsetTable : UI32[] or UI16[]
	unsigned int OffsetTable[NumGlyphs], CodeTable[NumGlyphs], CodeTableOffset;
	
	if (FontFlagsWideOffsets) {
		for (int i = 0; i < NumGlyphs; i++) {
			OffsetTable[i] = parser->getUI32();
            tagObject["OffsetTable"][i] = OffsetTable[i];
			DEBUGMSG("%d", i, OffsetTable[i]);
            if (i < (NumGlyphs - 1))
                DEBUGMSG(",");
		}
		CodeTableOffset = parser->getUI32();
	} else {
		for (int i = 0; i < NumGlyphs; i++) {
			OffsetTable[i] = parser->getUI16();
            
            tagObject["OffsetTable"][i] = OffsetTable[i];
			DEBUGMSG("%d", i, OffsetTable[i]);
            if (i < (NumGlyphs - 1))
                DEBUGMSG(",");
		}
		CodeTableOffset = parser->getUI16();
        
	}
    
	tagObject["CodeTableOffset"] = CodeTableOffset;
	DEBUGMSG("],\nCodeTableOffset : %d", CodeTableOffset); // can be used to check
	
	//// GlyphShapeTable : SHAPE[NumGlyphs]
    DEBUGMSG(",\nGlyphShapeTable : [\n");
	for (int i = 0; i < NumGlyphs; i++) {
        
		parser->getSHAPE(tag, tagObject["GlyphShapeTable"][i]);
        if (i < (NumGlyphs - 1))
            DEBUGMSG(",\n");
	}
    
    DEBUGMSG("\n],\nCodeTable : [");
	
	//// CodeTable : UI16[NumGlyphs] or UI8[NumGlyphs]
	if (FontFlagsWideCodes) {
		for (int i = 0; i < NumGlyphs; i++) {
			CodeTable[i] = parser->getUI16();
            
            tagObject["CodeTable"][i] = CodeTable[i];
			DEBUGMSG("%d", i, CodeTable[i]);
            if (i < (NumGlyphs - 1))
                DEBUGMSG(", ");
		}
	} else {
		for (int i = 0; i < NumGlyphs; i++) {
			CodeTable[i] = parser->getUI8();
            
            tagObject["CodeTable"][i] = CodeTable[i];
			DEBUGMSG("%d", i, CodeTable[i]);
            if (i < (NumGlyphs - 1))
                DEBUGMSG(", ");
		}
	}
    DEBUGMSG("]");
	
	signed int FontAscent, FontDescent, FontLeading, FontAdvanceTable[NumGlyphs];
	unsigned int KerningCount;
    
	if (FontFlagsHasLayout) {
		
		FontAscent	= parser->getSI16();
		FontDescent = parser->getSI16();
		FontLeading = parser->getSI16();
        
        tagObject["FontAscent"] = FontAscent;
        tagObject["FontDescent"] = FontDescent;
        tagObject["FontLeading"] = FontLeading;
		DEBUGMSG(",\nFontAscent : %d,\nFontDescent : %d,\nFontLeading : %d", FontAscent, FontDescent, FontLeading);
		
        VObject &fontAdvanceTable = tagObject["FontAdvanceTable"];
        DEBUGMSG(",\nFontAdvanceTable : [");

		//// FontAdvanceTable : SI16[NumGlyphs]
		for (int i = 0; i < NumGlyphs; i++){
			FontAdvanceTable[i] = parser->getSI16();
            
            fontAdvanceTable[i] = FontAdvanceTable[i];
			DEBUGMSG("%d", i, FontAdvanceTable[i]);
            if (i < (NumGlyphs - 1))
                DEBUGMSG(", ");
		}
        DEBUGMSG("]");
        
        
        VObject &fontBoundsTable = tagObject["FontBoundsTable"];
        DEBUGMSG(",\nFontBoundsTable : [\n");
        
		//// FontBoundsTable : RECT[NumGlyphs]
		for (int i = 0; i < NumGlyphs; i++){
			parser->getRECT(fontBoundsTable[i]);
            
            if (i < (NumGlyphs - 1))
                DEBUGMSG(",\n");
		}
        DEBUGMSG("\n]");
		
		KerningCount = parser->getUI16();
        
        tagObject["KerningCount"] = KerningCount;
        DEBUGMSG(",\nKerningCount : %d\n", KerningCount);

        VObject &fontKerningTable = tagObject["FontKerningTable"];
        DEBUGMSG(",\nFontKerningTable : [\n");
		
		//// FontKerningTable : KERNINGRECORD[KerningCount]
		for (int i = 0; i < KerningCount; i++) {
            
            VObject &kerningRecord = fontKerningTable[i];
            kerningRecord.setTypeInfo("KERNINGRECORD");
            
			//// KERNINGRECORD
			unsigned int FontKerningCode1, FontKerningCode2;
			signed int FontKerningAdjustment;
            
			if (FontFlagsWideCodes) {
				FontKerningCode1 = parser->getUI16();
				FontKerningCode2 = parser->getUI16();
			} else {
				FontKerningCode1 = parser->getUI8();
				FontKerningCode2 = parser->getUI8();
			}
			FontKerningAdjustment = parser->getSI16();
            
            kerningRecord["FontKerningCode1"] = FontKerningCode1;
            kerningRecord["FontKerningCode2"] = FontKerningCode2;
            kerningRecord["FontKerningAdjustment"] = FontKerningAdjustment;
            
			DEBUGMSG("{\n FontKerningCode1 : %d,\nFontKerningCode2 : %d,\nFontKerneingAdjustment : %d\n}", i, FontKerningCode1, FontKerningCode2, FontKerningAdjustment);
            if (i < (KerningCount - 1))
                DEBUGMSG(",\n");            
		}
        DEBUGMSG("\n]");
	} 
	
	return TRUE;
}

// DefineFont4 = 91 (SWF10)
// FIXME : FONTDATA
int DefineFont4(Tag *tag, SWFParser *parser, VObject &tagObject) // 91 = 0x5b
{
    unsigned int FontID, FontFlagsHasFontData, FontFlagsItalic, FontFlagsBold;
    
    FontID = parser->getUI16();
    
    tagObject["FontID"] = FontID;
    DEBUGMSG("FontID : %d", FontID);
    
    parser->getUBits(5);    // Reserved
    
    FontFlagsHasFontData = parser->getUBits(1);
    
    tagObject["FontFlagsHasFontData"] = FontFlagsHasFontData;
    DEBUGMSG(",\nFontFlagsHasFontData : %d", FontFlagsHasFontData);

    FontFlagsItalic      = parser->getUBits(1);
    
    tagObject["FontFlagsItalic"] = FontFlagsItalic;
    DEBUGMSG(",\nFontFlagsItalic : %d", FontFlagsItalic);

    FontFlagsBold        = parser->getUBits(1);
    
    tagObject["FontFlagsBold"] = FontFlagsBold;
    DEBUGMSG(",\nFontFlagsBold : %d", FontFlagsBold);
    
    DEBUGMSG(",\nFontName : ");
    tagObject["FontName"] = parser->getSTRING();
    
    // FontData : FONTDATA[0 or 1] // FIXME
    
    return TRUE;
}


// DefineFontInfo  = 13 = 0x0d
// DefineFontInfo2 = 62 = 0x3e (SWF6 or later)
int DefineFontInfo(Tag *tag, SWFParser *parser, VObject &tagObject) // 13 = 0x0d
{
	unsigned int FontID, FontNameLen;
	
	FontID = parser->getUI16();
    
    tagObject["FontID"] = FontID;
    
	FontNameLen = parser->getUI8();
	
	char FontName[FontNameLen];
	

	for (int i = 0; i < FontNameLen; i++) {
		FontName[i] = parser->getUI8();
	}
	
    tagObject["FontName"] = FontName;
    DEBUGMSG("FontName : \"%s\"", FontName);
	
	int FontFlagsReserved, FontFlagsSmallText, FontFlagsShiftJIS, FontFlagsANSI, FontFlagsItalic, FontFlagsBold, FontFlagsWideCodes;
	
	FontFlagsReserved	= parser->getUBits(2); // FontFlagsReserved Reserved bit fields
	
	FontFlagsSmallText	= parser->getUBits(1); // FontFlagsSmallText (SWF7 or later)
    
    tagObject["FontFlagsSmallText"] = FontFlagsSmallText;
	DEBUGMSG(",\nFontFlagsSmallText : %d", FontFlagsSmallText);
	
	FontFlagsShiftJIS	= parser->getUBits(1); // FontFlagsShiftJIS
    
    tagObject["FontFlagsShiftJIS"] = FontFlagsShiftJIS;
	DEBUGMSG(",\nFontFlagsShiftJIS : %d", FontFlagsShiftJIS);
	
	FontFlagsANSI		= parser->getUBits(1); // FontFlagsANSI
    
    tagObject["FontFlagsANSI"] = FontFlagsANSI;
	DEBUGMSG(",\nFontFlagsANSI : %d", FontFlagsANSI);
	
	FontFlagsItalic		= parser->getUBits(1); // FontFlagsItalic
    
    tagObject["FontFlagsItalic"] = FontFlagsItalic;
	DEBUGMSG(",\nFontFlagsItalic : %d", FontFlagsItalic);
	
	FontFlagsBold		= parser->getUBits(1); // FontFlagsBold
    
    tagObject["FontFlagsBold"] = FontFlagsBold;
	DEBUGMSG(",\nFontFlagsBold : %d", FontFlagsBold);
	
	FontFlagsWideCodes	= parser->getUBits(1); // FontFlagsWideCodes (DefineFontInfo2: Always 1)
    
    tagObject["FontFlagsWideCodes"] = FontFlagsWideCodes;
	DEBUGMSG(",\nFontFlagsWideCodes : %d", FontFlagsWideCodes);
	
    // DefineFontInfo2 = 62 (SWF6 or later)
    if (tag->TagCode == TAG_DEFINEFONTINFO2) {
        DEBUGMSG(",\nLanguageCode : ");
        tagObject["LanguageCode"] = parser->getLANGUAGECODE();
    }
    
	unsigned int CodeTable[nGlyphs];
	
    DEBUGMSG(",\nCodeTable : [");
	if (FontFlagsWideCodes) {
		for (int i = 0; i < nGlyphs; i++) {
			CodeTable[i] = parser->getUI16();
            
            tagObject["CodeTable"][i] = CodeTable[i];
			DEBUGMSG("%d", CodeTable[i]);
            if (i < (nGlyphs - 1))
                DEBUGMSG(",");
		}
	} else {
		for (int i = 0; i < nGlyphs; i++) {
			CodeTable[i] = parser->getUI8();
            
            tagObject["CodeTable"][i] = CodeTable[i];
			DEBUGMSG("%d", CodeTable[i]);
            if (i < (nGlyphs - 1))
                DEBUGMSG(",");
		}
	}
	
    DEBUGMSG("]");
    
	return TRUE;
}



int DefineFontAlignZones(Tag *tag, SWFParser *parser, VObject &tagObject) // 73 = 0x49 (SWF8)
{
    unsigned int FontID, CSMTableHint;
    
    FontID = parser->getUI16();
    CSMTableHint = parser->getUBits(2); // 0 = thin, 1 = medium, 2 = thick
    parser->getUBits(6); // Reserved
    
    tagObject["FontID"] = FontID;
    tagObject["CSMTableHint"] = CSMTableHint;
    DEBUGMSG("FontID : %d,\nCSMTableHint : %d", FontID, CSMTableHint);
    
    VObject &zoneTable = tagObject["ZoneTable"];
    DEBUGMSG(",\nZoneTable : [\n");
    
    // ZoneTable : ZONERECORD[NumGlyphs]
    for (int i = 0; i < NumGlyphs; i++) {
        
        VObject &zoneRecord = zoneTable[i];
        zoneRecord.setTypeInfo("ZONERECORD");
        
        // ZONERECORD
        DEBUGMSG("{ /* ZONERECORD */\n");
        
        unsigned int NumZoneData, ZoneMaskY, ZoneMaskX;
        float AlignmentCoordinate, Range;
        
        NumZoneData = parser->getUI8();
        
        zoneRecord["NumZoneData"] = NumZoneData;
        DEBUGMSG("NumZoneData : %d", NumZoneData);

        VObject &zoneData = zoneRecord["ZoneData"];
        DEBUGMSG(",\nZoneData : [\n");
        
        // ZoneData : ZONEDATA[NumZoneData]
        for (int j = 0; j < NumZoneData; j++) {
            
            // ZONEDATA
            AlignmentCoordinate = parser->getFLOAT16();
            Range = parser->getFLOAT16();
            
            zoneData[i].setTypeInfo("ZONEDATA");
            zoneData[i]["AlignmentCoordinate"] = AlignmentCoordinate;
            zoneData[i]["Range"] = Range;
            
            DEBUGMSG("{ /* ZONEDATA */\nAlignmentCoordinate : %f,\nRange : %f\n}", AlignmentCoordinate, Range); 
            if ( j < (NumZoneData - 1))
                DEBUGMSG(",\n");
        }
        
        DEBUGMSG("\n]");  // ZoneData
        
        parser->getUBits(6);    // Reserved, must be 0
        
        ZoneMaskY = parser->getUBits(1);
        
        zoneRecord["ZoneMaskY"] = ZoneMaskY;
        DEBUGMSG(",\nZoneMaskY : %d", ZoneMaskY);
        
        ZoneMaskX = parser->getUBits(1);
        
        zoneRecord["ZoneMaskX"] = ZoneMaskX;        
        DEBUGMSG(",\nZoneMaskX : %d", ZoneMaskX);
        
        DEBUGMSG("\n}");
        if (i < (NumGlyphs - 1))
            DEBUGMSG(",\n");
    }
    DEBUGMSG("\n]");     // ZoneTable
    return TRUE;
}

int DefineFontName(Tag *tag, SWFParser *parser, VObject &tagObject) // 88
{
    unsigned int FontID;
    
    FontID = parser->getUI16();
    
    tagObject["FontID"] = FontID;
    
    DEBUGMSG("FontID : %d,\nFontName : ", FontID);
    tagObject["FontName"] = parser->getSTRING();
    
    DEBUGMSG(",\nFontCopyright : ");
    tagObject["FontConpyright"] = parser->getSTRING();
    
    return TRUE;
}

//////////////////////////////////////////
//// Text
//////////////////////////////////////////
// DefineText  = 11
// DefineText2 = 33 (SWF3)
int DefineText(Tag *tag, SWFParser *parser, VObject &tagObject) // 11
{
    unsigned int CharacterID, GlyphBits, AdvanceBits;
    
    CharacterID = parser->getUI16();
    
    tagObject["CharacterID"] = CharacterID;
    DEBUGMSG("CharacterID : %d", CharacterID);
    
    DEBUGMSG(",\nTextBound : ");
    parser->getRECT(tagObject["TextBound"]);      // TextBounds
    
    DEBUGMSG(",\nTextMatrix : ");
    parser->getMATRIX(tagObject["TextMatrix"]);    // TextMatrix
    
    GlyphBits   = parser->getUI8();
    AdvanceBits = parser->getUI8();
    
    tagObject["GlyphBits"] = GlyphBits;
    tagObject["AdvanceBits"] = AdvanceBits;
    DEBUGMSG(",\nGlyphBits : %d,\nAdvanceBits : %d,\nTextRecords : [\n", GlyphBits, AdvanceBits);
    
    // TEXTRECORD[]
    while(parser->getTEXTRECORD(tag, GlyphBits, AdvanceBits))
        DEBUGMSG(",\n");
    
    DEBUGMSG("\n]"); // End of TextRecords
    
    return TRUE;
}


int DefineEditText(Tag *tag, SWFParser *parser, VObject &tagObject) // 37 (SWF4 or later)
{
    unsigned int CharacterID, HasText, WordWrap, Multiline, Password, ReadOnly, HasTextColor, HasMaxLength, HasFont, HasFontClass, AutoSize, HasLayout, NoSelect, Border, WasStatic, HTML, UseOutlines, FontID, FontHeight;
    unsigned int TextColor, MaxLength, Align, LeftMargin, RightMargin, Indent;
    signed int  Leading;
    
    CharacterID = parser->getUI16();
    
    tagObject["CharacterID"] = CharacterID;
    DEBUGMSG("CharacterID : %d,\nBounds : ", CharacterID);    
    
    parser->getRECT(tagObject["Bounds"]); // Bounds
    
    parser->setByteAlignment(); //??
    
    HasText = parser->getUBits(1);
    
    tagObject["HasText"] = HasText;
    DEBUGMSG(",\nHasText : %d", HasText);

    WordWrap = parser->getUBits(1);
    
    tagObject["WordWrap"] = WordWrap;
    DEBUGMSG(",\nWordWrap : %d", WordWrap);
    
    Multiline = parser->getUBits(1);
    
    tagObject["Multiline"] = Multiline;
    DEBUGMSG(",\nMultiline : %d", Multiline);
    
    Password = parser->getUBits(1);
    
    tagObject["Password"] = Password;
    DEBUGMSG(",\nPassword : %d", Password);
    
    ReadOnly = parser->getUBits(1);
    
    tagObject["ReadOnly"] = ReadOnly;
    DEBUGMSG(",\nReadOnly : %d", ReadOnly);
    
    HasTextColor = parser->getUBits(1);
    
    tagObject["HasTextColor"] = HasTextColor;
    DEBUGMSG(",\nHasTextColor : %d", HasTextColor);
    
    HasMaxLength = parser->getUBits(1);
    
    tagObject["HasMaxLength"] = HasMaxLength;
    DEBUGMSG(",\nHasMaxLength : %d", HasMaxLength);
    
    HasFont = parser->getUBits(1);
    
    tagObject["HasFont"] = HasFont;
    DEBUGMSG(",\nHasFont : %d", HasFont);
    
    HasFontClass = parser->getUBits(1);
    
    tagObject["HasFontClass"] = HasFontClass;
    DEBUGMSG(",\nHasFontClass : %d", HasFontClass);
    
    AutoSize = parser->getUBits(1);
    
    tagObject["AutoSize"] = AutoSize;
    DEBUGMSG(",\nAutoSize : %d", AutoSize);
    
    HasLayout = parser->getUBits(1);
    
    tagObject["HasLayout"] = HasLayout;
    DEBUGMSG(",\nHasLayout : %d", HasLayout);
    
    NoSelect = parser->getUBits(1);
    
    tagObject["NoSelect"] = NoSelect;
    DEBUGMSG(",\nNoSelect : %d", NoSelect);
    
    Border = parser->getUBits(1);
    
    tagObject["Border"] = Border;
    DEBUGMSG(",\nBorder : %d", Border);

    WasStatic = parser->getUBits(1);
    
    tagObject["WasStatic"] = WasStatic;
    DEBUGMSG(",\nWasStatic : %d", WasStatic);

    HTML = parser->getUBits(1);
    
    tagObject["HTML"] = HTML;
    DEBUGMSG(",\nHTML : %d", HTML);

    UseOutlines = parser->getUBits(1);
    
    tagObject["UseOutlines"] = UseOutlines;
    DEBUGMSG(",\nUseOutlines : %d", UseOutlines);

    if (HasFont) {
        FontID = parser->getUI16();
        tagObject["FontID"] = FontID;
        DEBUGMSG(",\nFontID : %d", FontID);
    }
    
    if (HasFontClass) {
        DEBUGMSG(",\nFontClass : ");
        tagObject["FontClass"] = parser->getSTRING();;
    }

    if (HasFont) {
        FontHeight = parser->getUI16();
        tagObject["FontHeight"] = FontHeight;
        DEBUGMSG(",\nFontHeight : %d", FontHeight);
    }
    
    if (HasTextColor) {
        TextColor = parser->getRGBA();
        tagObject["TextColor"] = Color2String(TextColor, 1);
        DEBUGMSG(",\nTextColor : \"%#04x\"", TextColor);
    }
    
    if (HasMaxLength) {
        MaxLength = parser->getUI16();
        tagObject["MaxLength"] = MaxLength;
        DEBUGMSG(",\nMaxLength : %d", MaxLength);        
    }
    
    if (HasLayout) {
        Align = parser->getUI8();
        tagObject["Align"] = Align;
        DEBUGMSG(",\nAlign : %d", Align);        

        LeftMargin = parser->getUI16();
        tagObject["LeftMargin"] = LeftMargin;
        DEBUGMSG(",\nLeftMargin : %d", LeftMargin);        

        RightMargin = parser->getUI16();
        tagObject["RightMargin"] = RightMargin;
        DEBUGMSG(",\nRightMargin : %d", RightMargin);        

        Indent = parser->getUI16();
        tagObject["Indent"] = Indent;
        DEBUGMSG(",\nIndent : %d", Indent);        

        Leading = parser->getSI16();
        tagObject["Leading"] = Leading;
        DEBUGMSG(",\nLeading : %d", Leading);        
    }
    
    DEBUGMSG(",\nVariableName : ");
    tagObject["VariableName"] = parser->getSTRING();
    
    if (HasText) {
        DEBUGMSG(",\nInitialText : ");
        tagObject["InitialText"] = parser->getSTRING();
    }
    
    
    return TRUE;
}

int CSMTextSettings(Tag *tag, SWFParser *parser, VObject &tagObject) // 74 = 0x4a (SWF8)
{
    unsigned int TextID, UseFlashType, GridFit;
    float   Thickness, Sharpness;
    
    TextID = parser->getUI16();
    
    tagObject["TextID"] = TextID;
    DEBUGMSG("TextID : %d", TextID);
    
    UseFlashType = parser->getUBits(2);
    
    tagObject["UseFlashType"] = UseFlashType;
    DEBUGMSG(",\nUseFlashType : %d", UseFlashType);

    GridFit = parser->getUBits(3);
    
    tagObject["GridFit"] = GridFit;
    DEBUGMSG(",\nGridFit : %d", GridFit);
    
    parser->getUBits(3); // Reserved, must be 0
    
    Thickness = parser->getFLOAT();
    
    tagObject["Thickness"] = Thickness;
    DEBUGMSG(",\nThickness : %f", Thickness);

    Sharpness = parser->getFLOAT();
    
    tagObject["Sharpness"] = Sharpness;
    DEBUGMSG(",\nSharpness : %f", Sharpness);

    parser->getUI8(); // Reserved, must be 0
    
    parser->seek(tag->NextTagPos); // Bug fix 
    return TRUE;
}


//////////////////////////////////////////
//// Frame Control
//////////////////////////////////////////

int FrameLabel(Tag *tag, SWFParser *parser, VObject &tagObject) // 43 = 0x2b (SWF3 or later)
{
    
    DEBUGMSG("LabelName : ");
    tagObject["LabelName"] = parser->getSTRING();
    
    return TRUE;
}

int DefineSceneAndFrameLabelData(Tag *tag, SWFParser *parser, VObject &tagObject) // 86 = 0x56
{
    unsigned int SceneCount;
    
    SceneCount = parser->getEncodedU32();
    
    tagObject["SceneCount"] = SceneCount;
    DEBUGMSG("SceneCount : %d", SceneCount);
    
    
    char offsetNo[20], nameNo[20];
    
    for (int i = 0; i < SceneCount; i++) {
        unsigned int offset;
        
        offset = parser->getEncodedU32();
        
        snprintf(offsetNo, 20, "Offset%d", i+1);
        tagObject[offsetNo] = offset;
        
        
        
        DEBUGMSG(",\nOffset%d : %d\nName%d : ", i+1, offset, i+1);
        snprintf(nameNo, 20, "Name%d", i+1);
        tagObject[nameNo] = parser->getSTRING();
    }
    
    
    
    unsigned int FrameLabelCount;
    
    FrameLabelCount = parser->getEncodedU32();
    
    tagObject["FrameLabelCount"] = FrameLabelCount;
    DEBUGMSG(",\nFrameLabelCount : %d", FrameLabelCount);
    
    char frameNumNo[20], frameLableNo[20];
    
    for (int i = 0; i < FrameLabelCount; i++) {
        unsigned int frameNum;
        
        frameNum = parser->getEncodedU32();
        
        snprintf(frameNumNo, 20, "FrameNum%d", i+1);
        tagObject[frameNumNo] = frameNum;

        DEBUGMSG(",\nFrameNum%d : %d\nFrameLabel%d : ", i+1, frameNum, i+1);
        snprintf(frameLableNo, 20, "nFrameLabel%d", i+1);
        tagObject[frameLableNo] = parser->getSTRING();
    }
    
    return TRUE;
}


//////////////////////////////////////////
//// Sound Tags
//////////////////////////////////////////

//////////////////////////////////////////
// Audio Coding Format Table
// 0 => Uncompressed, native-endian     => SWF1
// 1 => ADPCM                           => SWF1
// 2 => MP3                             => SWF4
// 3 => Uncompressed, little-enadian    => SWF4
// 6 => Nellymoser                      => SWF6
// 4 => Nellymoser 16khz                => SWF10
// 5 => Nellymoser 8khz                 => SWF10
// 11 => Speex                          => SWF10

int DefineSound(Tag *tag, SWFParser *parser, VObject &tagObject) // 14 = 0x0d
{
    unsigned int SoundId, SoundFormat, SoundRate, SoundSize, SoundType, SoundSampleCount;
    
    SoundId     = parser->getUI16();
    SoundFormat = parser->getUBits(4);      // 0 = uncompressed (big-endian), 1 = ADPCM, 2 = MP3, 3 = uncompressed (little-endian), 4 = Nelly 16kHz, 5 = Nelly 8kHz, 6 = Nelly, 11 = Speex
    SoundRate   = parser->getUBits(2);      // 0 = 5.5 kHz, 1 = 11 kHz, 2 = 22 kHz, 3 = 44 kHz.
    SoundSize   = parser->getUBits(1);      // Size of each sample. 0 = snd8Bit, 1 = snd16Bit
    SoundType   = parser->getUBits(1);      // 0 = sndMono, 1 = sndStereo
    SoundSampleCount = parser->getUI32();   // Number of samples
    
    DEBUGMSG("SoundId : %d,\nSoundFormat : %d,\nSoundRate : %d,\nSoundSize : %d,\nSoundType : %d,\nSoundSampleCount : %d,\nSoundData : [",
            SoundId, SoundFormat, SoundRate, SoundSize, SoundType, SoundSampleCount);
    
    //parser->dump(tag->TagLength - 7); // SoundData FIXME datail.
    
    DEBUGMSG("]");

    parser->skip(tag->TagLength - 7);
    return TRUE;
}

int DefineButtonSound(Tag *tag, SWFParser *parser, VObject &tagObject) // 17 = 0x11 (SWF2)
{
    unsigned int ButtonId, ButtonSoundChar0, ButtonSoundChar1, ButtonSoundChar2, ButtonSoundChar3;
    
    ButtonId = parser->getUI16();
    tagObject["ButtonId"] = ButtonId;
    DEBUGMSG("ButtonId : %d", ButtonId);
    
    ButtonSoundChar0 = parser->getUI16(); // Sound ID for OverUpToIdle
    tagObject["ButtonSoundChar0"] = ButtonSoundChar0;
    
    if (ButtonSoundChar0 != 0) {
        parser->getSOUNDINFO(tagObject["ButtonSoundInfo0"]);
    }
    
    ButtonSoundChar1 = parser->getUI16(); // Sound ID for IdleToOverUp
    tagObject["ButtonSoundChar1"] = ButtonSoundChar1;
    if (ButtonSoundChar1 != 0) {
        parser->getSOUNDINFO(tagObject["ButtonSoundInfo1"]);
    }
    
    ButtonSoundChar2 = parser->getUI16(); // Sound ID for OverUpToOverDown
    tagObject["ButtonSoundChar2"] = ButtonSoundChar2;
    if (ButtonSoundChar2 != 0) {
        parser->getSOUNDINFO(tagObject["ButtonSoundInfo2"]);
    }
    
    ButtonSoundChar3 = parser->getUI16(); // Sound ID for OverDownToOverUp
    tagObject["ButtonSoundChar3"] = ButtonSoundChar3;    
    if (ButtonSoundChar3 != 0) {
        parser->getSOUNDINFO(tagObject["ButtonSoundInfo3"]);
    }
    
    return TRUE;
}



//// StartSound  = 15
//// StartSound2 = 89
int StartSound(Tag *tag, SWFParser *parser, VObject &tagObject) // 15 = 0x0e
{
    unsigned int SoundId;
    char *SoundClassName;
    
    if (tag->TagCode == TAG_STARTSOUND){ // StartSound
        SoundId     = parser->getUI16();
        
        tagObject["SoundId"] = SoundId;
        DEBUGMSG("SoundId : %d", SoundId);
    } else {                            // StartSound2
        DEBUGMSG("SoundClassName : ");
        SoundClassName = parser->getSTRING();   // Name of the sound class to play
        tagObject["SoundClassName"] = SoundClassName;
    }
    
    // SOUNDINFO
    DEBUGMSG(",\nSoundInfo : ");
    parser->getSOUNDINFO(tagObject["SoundInfo"]);
    
    return TRUE;
}


//////////
// Internal used variable to store the StreamStoundCompression
/////////


// SoundStreamHead  = 18 = 0x12
// SoundStreamHead2 = 45 = 0x2c
//     StreamSoundCompression and StreamSoundSize allow different values
//
int SoundStreamHead(Tag *tag, SWFParser *parser, VObject &tagObject) // 18 = 0x12
{
    unsigned int PlaybackSoundRate, PlaybackSoundSize, PlaybackSoundType, StreamSoundCompression, StreamSoundRate, StreamSoundSize, StreamSoundType, StreamSoundSampleCount;
    signed int LatencySeek;
    
    parser->getUBits(4); // Reserved Always zero
    
    PlaybackSoundRate = parser->getUBits(2);    // 0 = 5.5kHz, 1 = 11kHz, 2 = 22khz, 3 = 44kHz    
    DEBUGMSG("PlaybackSoundRate : %d", PlaybackSoundRate);

    PlaybackSoundSize = parser->getUBits(1);    // Playbacksample size
                                                // SoundStreamHead => Always 1 (16bit)
                                                // SoundStreamHead2 => 0 = 8bit, 1 = 16bit
    DEBUGMSG(",\nPlaybackSoundSize : %d", PlaybackSoundSize);

    PlaybackSoundType = parser->getUBits(1);    // 0 = sndMono, 1 = sndSetreo
    DEBUGMSG(",\nPlaybackSoundType : %d", PlaybackSoundType);

    StreamSoundCompression = parser->getUBits(4);    // SoundStreamHead  => 1 = ADPCM, 2 = MP3 (SWF4 or later)
                                                     // SoundStreamHead2 => Audio Coding Formats
    DEBUGMSG(",\nStreamSoundCompression : %d", StreamSoundCompression);

    StreamSoundRate = parser->getUBits(2);    // 0 = 5.5kHz, 1 = 11kHz, 2 = 22khz, 3 = 44kHz
    DEBUGMSG(",\nStreamSoundRate : %d", StreamSoundRate);

    StreamSoundSize = parser->getUBits(1);    // streamming sound sample size
                                              // SoundStreamHead = > Always 1 (16bit)   
                                              // SoundStreamHead2
                                              // compressed format => Always 1 (16bit)
                                              // uncompressed format => 0 = 8bit, 1 = 16bit
    DEBUGMSG(",\nStreamSoundSize : %d", StreamSoundSize);

    StreamSoundType = parser->getUBits(1);    // 0 = sndMono, 1 = sndSetreo 
    DEBUGMSG(",\nStreamSoundType : %d", StreamSoundType);

    StreamSoundSampleCount = parser->getUI16();    // Average number of samples in each SoundStreamBlock
    DEBUGMSG(",\nStreamSoundSampleCount : %d", StreamSoundSampleCount);
    
    if (StreamSoundCompression == 2) {
        LatencySeek = parser->getSI16();
        DEBUGMSG(",\nLatencySeek : %d", LatencySeek);        
    }
    
    return TRUE;
}

int SoundStreamBlock(Tag *tag, SWFParser *parser, VObject &tagObject) // 19 = 0x13
{
    // StreamSoundData : UI8[size of compressed data]
    // FIXME, need to know the detail of the audio data is compressed or not.
    DEBUGMSG("StreamSoundData : [] /* FIXME */");
    parser->seek(tag->NextTagPos);
    return TRUE;
}

//////////////////////////////////////////
//// Video Tags
//////////////////////////////////////////



//// VideoFrame tag will need CodecID which is set by DefineVideoStream tag.
//// FIXME
//// What if two different Videos? 
//// CodecID should follow to the Video chracter. VideoFrame tag handler should get this info from the character in the dictionary.
static int CodecID = 0;


int DefineVideoStream(Tag *tag, SWFParser *parser, VObject &tagObject) // 60 = 0x32
{
    unsigned int CharacterID, NumFrames, Width, Height, VideoFlagsDeblocking, VideoFlagsSmoothing;
    
    CharacterID = parser->getUI16();
    DEBUGMSG("CharacterID : %d", CharacterID);
    
    NumFrames = parser->getUI16(); // Number of VideoFrame tags
    DEBUGMSG(",\nNumFrames : %d", NumFrames);
    
    Width = parser->getUI16();  // in pixels
    DEBUGMSG(",\nWidth : %d", Width);
    
    Height = parser->getUI16(); // in pixels
    DEBUGMSG(",\nHeight : %d", Height);
    
    parser->getUBits(4); // Reserved, must be 0
    
    VideoFlagsDeblocking = parser->getUBits(3); // 000 = use VIDEOPACKET value
                                                // 001 = off
                                                // 010 = Level1 (Fast deblocking filter)
                                                // 011 = Level2 (VP6 only, better deblocking filter)
                                                // 100 = Level3 (VP6 only, better deblocking plus fast deringing filter)
                                                // 101 = Level4 (VP6 only, better deblocking plus better deringing filter)
                                                // 110 = Reserved
                                                // 111 = Reserved
    DEBUGMSG(",\nVideoFlagsDeblocking : %d", VideoFlagsDeblocking);

    VideoFlagsSmoothing = parser->getUBits(1);  // 0 = smoothing off (faster), 1 = smoothing on (higher quality)
    DEBUGMSG(",\nVideoFlagsSmoothing : %d", VideoFlagsSmoothing);

    CodecID = parser->getUI8(); // 2 = Sorenson H.263
                                // 3 = Screen video (SWF7)
                                // 4 = VP6 (SWF8)
                                // 5 = VP6 video with alpha channel (SWF8)
    DEBUGMSG(",\nCodecID : %d", CodecID);

    return TRUE;
}


int VideoFrame(Tag *tag, SWFParser *parser, VObject &tagObject) // 61 = 0x4d
{
    unsigned int StreamID, FrameNum;
    
    StreamID = parser->getUI16();   // ID of video stream character
    DEBUGMSG("StreamID : %d,\n", StreamID);

    FrameNum = parser->getUI16();   // Sequence number of this frame within it's video stream
    DEBUGMSG("FrameNum : %d,\n", FrameNum);
    
    // VideoData
    // CodecID = 2 => H263VIDEOPACKET
    // CodecID = 3 => SCREENVIDEOPACKET
    // CodecID = 4 => VP6SWFVIDEOPACKET
    // CodecID = 5 => VP6SWFALPHAVIDEOPACKET
    // CodecID = 6 => SCREENV2VIDEOPACKET
    switch (CodecID) {
        case 2:
            DEBUGMSG("VideoData : /* H263VIDEOPACKET */ {\n");
            break;
        case 3:
            DEBUGMSG("VideoData : /* SCREENVIDEOPACKET */ {\n");
            break;
        case 4:
            DEBUGMSG("VideoData : /* VP6SWFVIDEOPACKET */ {\n");
            break;
        case 5:
            DEBUGMSG("VideoData : /* VP6SWFALPHAVIDEOPACKET */ {\n");
            break;
        case 6:
            DEBUGMSG("VideoData : /* SCREENV2VIDEOPACKET */ {\n");
            break;
        default:
            DEBUGMSG("VideoData : /* Undefined Video Packet */ {\n");
            ASSERT(1);
            break;
    }
    
    // FIXME
    parser->dump(tag->NextTagPos - parser->getStreamPos());
    DEBUGMSG("} /* End of VideoData */\n");

    parser->skip(tag->NextTagPos - parser->getStreamPos());
    return TRUE;
}


//////////////////////////////////////////
//// Action Tags
//////////////////////////////////////////

int DoAction(Tag *tag, SWFParser *parser, VObject &tagObject) // 12 = 0x0c
{
    DEBUGMSG("Actions : [\n");
    // ACTIONRECORD[]
    unsigned int recNo = 0;
	//while(parser->getACTIONRECORD(tagObject["Actions"][recNo++])); // read until we get ActionEndFlg = 0
    
    unsigned int ActionCode = 0, Length = 0;
    Action action;
    ActionHandler actionHandler;
    VObject &actionObject = tagObject["Actions"];
    
    do {
        parser->getActionCodeAndLength(&action);
        ActionCode = action.ActionCode;
        Length = action.Length;
        
        actionObject[recNo]["ActionCode"]   = ActionCode;
        
        if (ActionCode & 0x80) {
            actionObject[recNo]["Length"]   = Length;
        }
        
        actionHandler.parse(&action, parser, actionObject[recNo]);
        recNo++;
    } while (ActionCode != 0);
    
    DEBUGMSG("\n]");
    return TRUE;
}

// Excuted once.
// SWF6 or later (AS1/2 only)
// after SWF9, if AS3 is set in FileAttributes tag, DoInitAction tag will be ignored.
int DoInitAction(Tag *tag, SWFParser *parser, VObject &tagObject) // 59 = 0x4b 
{
    unsigned int SpriteID;
    SpriteID = parser->getUI16();
    tagObject["SpriteID"] = SpriteID;
    DEBUGMSG("SpriteID : %d,\n Actions : [\n", SpriteID);
    // Actions : ACTIONRECORD[]
    unsigned int recNo = 0;
	//while(parser->getACTIONRECORD(tagObject["Actions"][recNo++])); // read until we get ActionEndFlg = 0
    
    unsigned int ActionCode = 0, Length = 0;
    Action action;
    ActionHandler actionHandler;
    VObject &actionObject = tagObject["Actions"];
    
    do {
        parser->getActionCodeAndLength(&action);
        ActionCode = action.ActionCode;
        Length = action.Length;
        
        actionObject[recNo]["ActionCode"]   = ActionCode;

        if (ActionCode & 0x80) {
            actionObject[recNo]["Length"]   = Length;
        }
        
        actionHandler.parse(&action, parser, actionObject[recNo]);
        recNo++;
    } while (ActionCode != 0);
    
    DEBUGMSG("\n]");
    return TRUE;
}


int DoABC(Tag *tag, SWFParser *parser, VObject &tagObject) // 82 = 0x52 (SWF9)
{
    unsigned int Flags;
    
    Flags = parser->getUI32();
    tagObject["Flags"] = Flags;
    DEBUGMSG("Flags : %d,\nName :", Flags);
    // Name : STRING (The name assigned to the bytecode)
    tagObject["Name"] = parser->getSTRING();
    tagObject["ABCData"].setTypeInfo("FIXME : Binary Array");
    
    DEBUGMSG(",\nABCData : [");
    parser->dump(tag->NextTagPos - parser->getStreamPos());
    DEBUGMSG("] /* .abc byte code, FIXME */");

    parser->skip(tag->NextTagPos - parser->getStreamPos());
    return TRUE;
}

// for AS3 and with SymbolClass
int DefineBinaryData(Tag *tag, SWFParser *parser, VObject &tagObject) // 87 = 0x57
{
    unsigned int CharacterID;
    
    CharacterID = parser->getUI16();
    DEBUGMSG("CharacterID : %d,\n", CharacterID);
    
    parser->getUI32(); // Reserved, must be 0
    
    tagObject["CharacterID"] = CharacterID;
    tagObject["BinaryData"].setTypeInfo("FIXME : Binary Array");

    DEBUGMSG("BinaryData : [\n");
    parser->dump(tag->TagLength - 6);
    DEBUGMSG("]");
    
    parser->skip(tag->TagLength - 6);
    return TRUE;
}




// This would be called recursively by ControlTags
/////////////////////////////////////////////////////////////////
//// Call the real tag handler function according to the TagCode
////////////////////////////////////////////////////////////////
int TagHandler::parse(Tag *tag, SWFParser *parser, VObject &tagObject)
{
	unsigned int TagCode, TagLength;
	
	TagCode		= tag->TagCode;
	TagLength	= tag->TagLength;

    //parser->dump(16);
    
	if (TagCode > TAGCODE_MAX) {
		DEBUGMSG("Obfuscated TagCode = %d\n", TagCode);
        tagObject.setTypeInfo("Unknown Tag");
        parser->skip(TagLength);
        return FALSE;
	}
        
    tagObject.setTypeInfo(tagHandlerInfo[TagCode].name);
    
    DEBUGMSG("TagCode : %d, /* %s */\nTagLength : %d%c\n", TagCode, tagHandlerInfo[TagCode].name  ,TagLength, TagLength ? ',' : ' ');	
	

	if(tagHandlerInfo[TagCode].handler) {
		tagHandlerInfo[TagCode].handler(tag, parser, tagObject);
		DEBUGMSG("\n");
        if (parser->getStreamPos() != tag->NextTagPos) {
#ifdef DEBUG
            DEBUGMSG("parser pos = %d, NextTagPos = %d\n", parser->getStreamPos(), tag->NextTagPos);
            parser->seek(tag->NextTagPos - tag->TagLength);
            parser->dump(tag->TagLength);
            ASSERT (1);
#else
            parser->seek(tag->NextTagPos);
#endif
        }
		return TRUE;	// Tag is handled.
	}
	
	parser->skip(TagLength);
	return FALSE; // Tag is not handled.
}

////////////////// - End - /////////////////////
