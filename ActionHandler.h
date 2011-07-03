/*
 *  ActionHandler.h
 *  swfparser
 *
 *  Created by Rintarou on 2010/8/6.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "common.h"
#include "SWFParser.h"

#ifndef _ACTIONHANDLER_H
#define _ACTIONHANDLER_H

class ActionHandler {
public:
	int		parse(Action *action, SWFParser *parser, VObject &actionObject);
};


//// Standard Tag Handler Function
typedef int (*ActionHandlerFunc)(Action *action, SWFParser *parser, VObject &actionObject);

struct ActionHandlerInfo {
	ActionHandlerFunc handler;
	const char        *name;
};


int ActionGotoFrame(Action *action, SWFParser *parser, VObject &actionObject);          // 128 = 0x81
int ActionGetURL(Action *action, SWFParser *parser, VObject &actionObject);             // 131 = 0x83
int ActionStoreRegister(Action *action, SWFParser *parser, VObject &actionObject);      // 134 = 0x87
int ActionConstantPool(Action *action, SWFParser *parser, VObject &actionObject);       // 136 = 0x88
int ActionWaitForFrame(Action *action, SWFParser *parser, VObject &actionObject);       // 138 = 0x8A
int ActionSetTarget(Action *action, SWFParser *parser, VObject &actionObject);          // 139 = 0x8B
int ActionGoToLabel(Action *action, SWFParser *parser, VObject &actionObject);          // 140 = 0x8C
int ActionWaitForFrame2(Action *action, SWFParser *parser, VObject &actionObject);      // 141 = 0x8D
int ActionDefineFunction2(Action *action, SWFParser *parser, VObject &actionObject);	// 142 = 0x8E
int ActionTry(Action *action, SWFParser *parser, VObject &actionObject);                // 143 = 0x8F
int ActionWith(Action *action, SWFParser *parser, VObject &actionObject);               // 148 = 0x94
int ActionPush(Action *action, SWFParser *parser, VObject &actionObject);               // 152 = 0x96
int ActionJump(Action *action, SWFParser *parser, VObject &actionObject);               // 153 = 0x99
int ActionGetURL2(Action *action, SWFParser *parser, VObject &actionObject);            // 154 = 0x9A
int ActionDefineFunction(Action *action, SWFParser *parser, VObject &actionObject);     // 155 = 0x9B
int ActionIf(Action *action, SWFParser *parser, VObject &actionObject);                 // 157 = 0x9D
int ActionGotoFrame2(Action *action, SWFParser *parser, VObject &actionObject);         // 159 = 0x9F
#endif