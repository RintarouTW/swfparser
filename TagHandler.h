/*
 *  TagHandler.h
 *  swfparser
 *
 *  Created by Rintarou on 2010/5/16.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "common.h"
#include "SWFParser.h"

#ifndef _TAGHANDLER_H
#define _TAGHANDLER_H

class TagHandler {
public:
	int		parse(Tag *tag, SWFParser *parser, VObject &tagObject);
};


//// Standard Tag Handler Function
typedef int (*TagHandlerFunc)(Tag *tag, SWFParser *parser, VObject &tagObject);

struct TagHandlerInfo {
	TagHandlerFunc handler;
	const char     *name;
};

//// Debug
int EnableDebugger(Tag *tag, SWFParser *parser, VObject &tagObject);        // 58, EnableDebugger2 = 64


//// File Scope
int SetBackgroundColor(Tag *tag, SWFParser *parser, VObject &tagObject);	// 9 = 0x09
int FileAttributes(Tag *tag, SWFParser *parser, VObject &tagObject);		// 69 = 0x45
int MetatData(Tag *tag, SWFParser *parser, VObject &tagObject);             // 44 = 0x2c
int ScriptLimits(Tag *tag, SWFParser *parser, VObject &tagObject);          // 65 = 0x41 (SWF7)
int SetTabIndex(Tag *tag, SWFParser *parser, VObject &tagObject);           // 66 = 0x42 (SWF7)

//// Assets
int ExportAssets(Tag *tag, SWFParser *parser, VObject &tagObject);          // 56 = 0x38 (SWF5)
int ImportAssets(Tag *tag, SWFParser *parser, VObject &tagObject);          // 57 = 0x39 (SWF5 - SWF7), ImportAssets2 = 71 = 0x47 (SWF8 or later)
int SymbolClass(Tag *tag, SWFParser *parser, VObject &tagObject);           // 76 = 0x4c (SWF9 or later)

//// Shapes
int DefineShape(Tag *tag, SWFParser *parser, VObject &tagObject);			// 2 = DefineShape, 22 = DefineShape2, 32 = DefineShape3, 83 = DefineShape4
int DefineButton(Tag *tag, SWFParser *parser, VObject &tagObject);			// 7 = 0x07
int DefineButtonCxform(Tag *tag, SWFParser *parser, VObject &tagObject);    // 23 = 0x17 (SWF2)
int DefineScalingGrid(Tag *tag, SWFParser *parser, VObject &tagObject);     // 78 = 0x4e

int DefineMorphShape(Tag *tag, SWFParser *parser, VObject &tagObject);		// 46 = 0x3e (SWF3), DefineMorphShape2 = 84 = 0x54

//// Sprites
int DefineSprite(Tag *tag, SWFParser *parser, VObject &tagObject);          // 39 = 0x27 (SWF3)


//// DisplayList Control
int PlaceObject(Tag *tag, SWFParser *parser, VObject &tagObject);			// 4 = 0x04
int PlaceObject2(Tag *tag, SWFParser *parser, VObject &tagObject);			// 26 = 0x1a
int RemoveObject(Tag *tag, SWFParser *parser, VObject &tagObject);			// 5 = 0x05
int RemoveObject2(Tag *tag, SWFParser *parser, VObject &tagObject);			// 28 = 0x1c

//// Bits
int DefineBits(Tag *tag, SWFParser *parser, VObject &tagObject);			// 6 = 0x06
int DefineBitsLossless(Tag *tag, SWFParser *parser, VObject &tagObject);    // 20 = 0x14 (SWF2), DefineBitsLossless2 = 36 = 0x20 (SWF3)

int JPEGTables(Tag *tag, SWFParser *parser, VObject &tagObject);			// 8 = 0x08
int DefineBitsJPEG2(Tag *tag, SWFParser *parser, VObject &tagObject);		// 21 = 0x15, SWF2 or later

//// Fonts
int DefineFont(Tag *tag, SWFParser *parser, VObject &tagObject);			// 10 = 0x10
int DefineFont2(Tag *tag, SWFParser *parser, VObject &tagObject);			// 48 = 0x30, DefineFont3 = 75 = 0x4b (SWF8)
int DefineFontInfo(Tag *tag, SWFParser *parser, VObject &tagObject);		// 13 = 0x0d, DefineFontInfo2 = 62 = 0x3e (SWF6)
int DefineFontAlignZones(Tag *tag, SWFParser *parser, VObject &tagObject);  // 73 = 0x49
int DefineFontName(Tag *tag, SWFParser *parser, VObject &tagObject);        // 88 = 0x58 (SWF9)

//// Texts
int DefineText(Tag *tag, SWFParser *parser, VObject &tagObject);            // 11 = 0x42, DefineText2 = 33 = 0x33 (SWF3)
int DefineEditText(Tag *tag, SWFParser *parser, VObject &tagObject);        // 37 = 0x25 (SWF4)
int CSMTextSettings(Tag *tag, SWFParser *parser, VObject &tagObject);       // 74 = 0x4a (SWF8)

//// Frame Control
int FrameLabel(Tag *tag, SWFParser *parser, VObject &tagObject);            // 43 = 0x2b (SWF3)
int DefineSceneAndFrameLabelData(Tag *tag, SWFParser *parser, VObject &tagObject); // 86 = 0x56

//// Sounds
int DefineSound(Tag *tag, SWFParser *parser, VObject &tagObject);           // 14 = 0x0d
int DefineButtonSound(Tag *tag, SWFParser *parser, VObject &tagObject);     // 17 = 0x11 (SWF2)
int StartSound(Tag *tag, SWFParser *parser, VObject &tagObject);            // 15 = StartSound, 89 = StartSound2
int SoundStreamHead(Tag *tag, SWFParser *parser, VObject &tagObject);       // 18 = 0x12
int SoundStreamBlock(Tag *tag, SWFParser *parser, VObject &tagObject);      // 19 = 0x13

//// Video
int DefineVideoStream(Tag *tag, SWFParser *parser, VObject &tagObject);     // 60 = 0x32
int VideoFrame(Tag *tag, SWFParser *parser, VObject &tagObject);            // 61 = 0x4d

//// Actions
int DoAction(Tag *tag, SWFParser *parser, VObject &tagObject);              // 12 = 0x0c
int DoInitAction(Tag *tag, SWFParser *parser, VObject &tagObject);          // 59 = 0x4b (SWF6)
int DefineBinaryData(Tag *tag, SWFParser *parser, VObject &tagObject);      // 87 = 0x57 (SWF9)
int DoABC(Tag *tag, SWFParser *parser, VObject &tagObject);                 // 82 = 0x52 (SWF9)


#endif