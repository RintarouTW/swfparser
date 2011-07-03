/*
 *  ActionHandler.cpp
 *  swfparser
 *
 *  Created by Rintarou on 2010/8/6.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */

#include "common.h"
#include "ActionDefine.h"
#include "ActionHandler.h"

static ActionHandlerInfo actionHandlerInfo[] = {
    { NULL,			"ActionEndFlag"},           // 00   0x00    SWF3
    { NULL,			"0x01"},                    // 01   0x01    NONE
    { NULL,         "0x02"},                    // 02   0x02    NONE
    { NULL,			"0x03"},                    // 03   0x03    NONE
    { NULL,			"ActionNextFrame"},         // 04   0x04    SWF3
    { NULL,         "ActionPreviousFrame"},     // 05   0x05    SWF3
    { NULL,			"ActionPlay"},              // 06   0x06    SWF3
    { NULL,			"ActionStop"},              // 07   0x07    SWF3
    { NULL,         "ActionToggleQuality"},     // 08   0x08    SWF3
    { NULL,			"ActionStopSounds"},        // 09   0x09    SWF3
    { NULL,			"ActionAdd"},               // 10   0x0A    SWF4
    { NULL,         "ActionSubtract"},          // 11   0x0B    SWF4
    { NULL,			"ActionMultiply"},          // 12   0x0C    SWF4
    { NULL,			"ActionDivide"},            // 13   0x0D    SWF4
    { NULL,         "ActionEquals"},            // 14   0x0E    SWF4
    { NULL,			"ActionLess"},              // 15   0x0F    SWF4
    { NULL,			"ActionAnd"},               // 16   0x10    SWF4
    { NULL,         "ActionOr"},                // 17   0x11    SWF4
    { NULL,			"ActionNot"},               // 18   0x12    SWF4
    { NULL,			"ActionStringEquals"},      // 19   0x13    SWF4
    { NULL,         "ActionStringLength"},      // 20   0x14    SWF4
    { NULL,			"ActionStringExtract"},     // 21   0x15    SWF4
    { NULL,			"0x16"},                    // 22   0x16    NONE
    { NULL,         "ActionPop"},               // 23   0x17    SWF4
    { NULL,			"ActionToInteger"},         // 24   0x18    SWF4
    { NULL,			"0x19"},                    // 25   0x19    NONE
    { NULL,         "0x1A"},                    // 26   0x1A    NONE
    { NULL,			"0x1B"},                    // 27   0x1B    NONE
    { NULL,			"ActionGetVariable"},       // 28   0x1C    SWF4
    { NULL,         "ActionSetVariable"},       // 29   0x1D    SWF4
    { NULL,			"0x1E"},                    // 30   0x1E    NONE
    { NULL,			"0x1F"},                    // 31   0x1F    NONE
    { NULL,			"ActionSetTarget2"},        // 32   0x20    SWF4
    { NULL,         "ActionStringAdd"},         // 33   0x21    SWF4
    { NULL,			"ActionGetProperty"},       // 34   0x22    SWF4
    { NULL,			"ActionSetProperty"},       // 35   0x23    SWF4
    { NULL,         "ActionCloneSprite"},       // 36   0x24    SWF4
    { NULL,			"ActionRemoveSprite"},      // 37   0x25    SWF4
    { NULL,			"ActionTrace"},             // 38   0x26    SWF4
    { NULL,         "ActionStartDrag"},         // 39   0x27    SWF4
    { NULL,			"ActionEndDrag"},           // 40   0x28    SWF4
    { NULL,			"ActionStringLess"},        // 41   0x29    SWF4
    { NULL,         "ActionThrow"},             // 42   0x2A    SWF7
    { NULL,			"ActionCastOp"},            // 43   0x2B    SWF7
    { NULL,			"ActionImplementsOp"},      // 44   0x2C    SWF7
    { NULL,         "0x2D"},                    // 45   0x2D    NONE
    { NULL,			"0x2E"},                    // 46   0x2E    NONE
    { NULL,			"0x2F"},                    // 47   0x2F    NONE
    { NULL,			"ActionRandomNumber"},      // 48   0x30    SWF4
    { NULL,         "ActionMBStringLength"},    // 49   0x31    SWF4
    { NULL,			"ActionCharToAscii"},       // 50   0x32    SWF4
    { NULL,			"ActionAsciiToChar"},       // 51   0x33    SWF4
    { NULL,         "ActionGetTime"},           // 52   0x34    SWF4
    { NULL,			"ActionMBStringExtract"},   // 53   0x35    SWF4
    { NULL,			"ActionMBCharToAscii"},     // 54   0x36    SWF4
    { NULL,         "ActionMBAsciiToChar"},     // 55   0x37    SWF4
    { NULL,			"0x38"},                    // 56   0x38    NONE
    { NULL,			"0x39"},                    // 57   0x39    NONE
    { NULL,         "ActionDelete"},            // 58   0x3A    SWF5
    { NULL,			"ActionDelete2"},           // 59   0x3B    SWF5
    { NULL,			"ActionDefineLocal"},       // 60   0x3C    SWF5
    { NULL,         "ActionCallFunction"},      // 61   0x3D    SWF5
    { NULL,			"ActionReturn"},            // 62   0x3E    SWF5
    { NULL,			"ActionModulo"},            // 63   0x3F    SWF5
    { NULL,			"ActionNewObject"},         // 64   0x40    SWF5
    { NULL,         "ActionDefineLocal2"},      // 65   0x41    SWF5
    { NULL,			"ActionInitArray"},         // 66   0x42    SWF5
    { NULL,			"ActionInitObject"},        // 67   0x43    SWF5
    { NULL,         "ActionTypeOf"},            // 68   0x44    SWF5
    { NULL,			"ActionTargetPath"},        // 69   0x45    SWF5
    { NULL,			"ActionEnumerate"},         // 70   0x46    SWF5
    { NULL,         "ActionAdd2"},              // 71   0x47    SWF5
    { NULL,			"ActionLess2"},             // 72   0x48    SWF5
    { NULL,			"ActionEquals2"},           // 73   0x49    SWF5
    { NULL,         "ActionToNumber"},          // 74   0x4A    SWF5
    { NULL,			"ActionToString"},          // 75   0x4B    SWF5
    { NULL,			"ActionPushDuplicate"},     // 76   0x4C    SWF5
    { NULL,         "ActionStackSwap"},         // 77   0x4D    SWF5
    { NULL,			"ActionGetMember"},         // 78   0x4E    SWF5
    { NULL,			"ActionSetMember"},         // 79   0x4F    SWF5
    { NULL,			"ActionIncrement"},         // 80   0x50    SWF5
    { NULL,         "ActionDecrement"},         // 81   0x51    SWF5
    { NULL,			"ActionCallMethod"},        // 82   0x52    SWF5
    { NULL,			"ActionNewMethod"},         // 83   0x53    SWF5
    { NULL,         "ActionInstanceOf"},        // 84   0x54    SWF6
    { NULL,			"ActionEnumerate2"},        // 85   0x55    SWF6
    { NULL,			"0x56"},                    // 86   0x56    NONE
    { NULL,         "0x57"},                    // 87   0x57    NONE
    { NULL,			"0x58"},                    // 88   0x58    NONE
    { NULL,			"0x59"},                    // 89   0x59    NONE
    { NULL,         "0x5A"},                    // 90   0x5A    NONE
    { NULL,			"0x5B"},                    // 91   0x5B    NONE
    { NULL,			"0x5C"},                    // 92   0x5C    NONE
    { NULL,         "0x5D"},                    // 93   0x5D    NONE
    { NULL,			"0x5E"},                    // 94   0x5E    NONE
    { NULL,			"0x5F"},                    // 95   0x5F    NONE
    { NULL,			"ActionBitAnd"},            // 96   0x60    SWF5
    { NULL,         "ActionBitOr"},             // 97   0x61    SWF5
    { NULL,			"ActionBitXor"},            // 98   0x62    SWF5
    { NULL,			"ActionBitLShift"},         // 99   0x63    SWF5
    { NULL,         "ActionBitRShift"},         // 100  0x64    SWF5
    { NULL,			"ActionBitURShift"},        // 101  0x65    SWF5
    { NULL,			"ActionStrictEquals"},      // 102  0x66    SWF6
    { NULL,         "ActionGreater"},           // 103  0x67    SWF6
    { NULL,			"ActionStringGreater"},     // 104  0x68    SWF6
    { NULL,			"ActionExtends"},           // 105  0x69    SWF7
    { NULL,         "0x6A"},                    // 106  0x6A    NONE
    { NULL,			"0x6B"},                    // 107  0x6B    NONE
    { NULL,			"0x6C"},                    // 108  0x6C    NONE
    { NULL,         "0x6D"},                    // 109  0x6D    NONE
    { NULL,			"0x6E"},                    // 110  0x6E    NONE
    { NULL,			"0x6F"},                    // 111  0x6F    NONE
    { NULL,			"0x70"},                    // 112  0x70    NONE
    { NULL,         "0x71"},                    // 113  0x71    NONE
    { NULL,			"0x72"},                    // 114  0x72    NONE
    { NULL,			"0x73"},                    // 115  0x73    NONE
    { NULL,         "0x74"},                    // 116  0x74    NONE
    { NULL,			"0x75"},                    // 117  0x75    NONE
    { NULL,			"0x76"},                    // 118  0x76    NONE
    { NULL,         "0x77"},                    // 119  0x77    NONE
    { NULL,			"0x78"},                    // 120  0x78    NONE
    { NULL,			"0x79"},                    // 121  0x79    NONE
    { NULL,         "0x7A"},                    // 122  0x7A    NONE
    { NULL,			"0x7B"},                    // 123  0x7B    NONE
    { NULL,			"0x7C"},                    // 124  0x7C    NONE
    { NULL,         "0x7D"},                    // 125  0x7D    NONE
    { NULL,			"0x7E"},                    // 126  0x7E    NONE
    { NULL,			"0x7F"},                    // 127  0x7F    NONE
    
/////////////////////////// >= 0x80 //////////////////////////////////
    
    { NULL,			"0x80"},                            // 128  0x80    NONE
    { ActionGotoFrame,      "ActionGotoFrame"},         // 129  0x81    SWF3
    { NULL,			"0x82"},                            // 130  0x82    NONE
    { ActionGetURL,         "ActionGetURL"},            // 131  0x83    SWF3
    { NULL,         "0x84"},                            // 132  0x84    NONE
    { NULL,			"0x85"},                            // 133  0x85    NONE
    { NULL,			"0x86"},                            // 134  0x86    NONE
    { ActionStoreRegister,  "ActionStoreRegister"},     // 135  0x87    SWF5
    { ActionConstantPool,	"ActionConstantPool"},      // 136  0x88    SWF5
    { NULL,			"0x89"},                            // 137  0x89    NONE
    { ActionWaitForFrame,   "ActionWaitForFrame"},      // 138  0x8A    SWF3
    { ActionSetTarget,		"ActionSetTarget"},         // 139  0x8B    SWF3
    { ActionGoToLabel,		"ActionGoToLabel"},         // 140  0x8C    SWF3
    { ActionWaitForFrame2,  "ActionWaitForFrame2"},     // 141  0x8D    SWF4
    { ActionDefineFunction2,"ActionDefineFunction2"},   // 142  0x8E    SWF7
    { ActionTry,			"ActionTry"},               // 143  0x8F    SWF7
    { NULL,			"0x90"},                            // 144  0x90    NONE
    { NULL,         "0x91"},                            // 145  0x91    NONE
    { NULL,			"0x92"},                            // 146  0x92    NONE
    { NULL,			"0x93"},                            // 147  0x93    NONE
    { ActionWith,           "ActionWith"},              // 148  0x94    SWF5
    { NULL,			"0x95"},                            // 149  0x95    NONE
    { ActionPush,           "ActionPush"},              // 150  0x96    SWF4
    { NULL,         "0x97"},                            // 151  0x97    NONE
    { NULL,			"0x98"},                            // 152  0x98    NONE
    { ActionJump,           "ActionJump"},              // 153  0x99    SWF4
    { ActionGetURL2,        "ActionGetURL2"},           // 154  0x9A    SWF4
    { ActionDefineFunction,	"ActionDefineFunction"},    // 155  0x9B    SWF5
    { NULL,			"0x9C"},                            // 156  0x9C    NONE
    { ActionIf,             "ActionIf"},                // 157  0x9D    SWF4
    { NULL,                 "ActionCall"},              // 158  0x9E    SWF4 (Special case Length = 0 ??)
    { ActionGotoFrame2,		"ActionGotoFrame2"}         // 159  0x9F    SWF4
};

int ActionGotoFrame(Action *action, SWFParser *parser, VObject &actionObject)	// 129 = 0x81
{
    actionObject["Frame"] = parser->getUI16();
    return TRUE;
}

int ActionGetURL(Action *action, SWFParser *parser, VObject &actionObject)	// 131 = 0x83
{
    actionObject["UrlString"] = parser->getSTRING();
    actionObject["TargetString"] = parser->getSTRING();
    return TRUE;
}

int ActionStoreRegister(Action *action, SWFParser *parser, VObject &actionObject)	// 134 = 0x87
{
    actionObject["RegisterNumber"] = parser->getUI8();
    return TRUE;
}

int ActionConstantPool(Action *action, SWFParser *parser, VObject &actionObject)	// 136 = 0x88
{   
    unsigned int Count = 0;
    Count = parser->getUI16();
    actionObject["Count"] = Count;
    
    for (unsigned int i = 0; i < Count; i++) {
        actionObject["ConstantPool"][i] = parser->getSTRING();
    }
    return TRUE;
}

int ActionWaitForFrame(Action *action, SWFParser *parser, VObject &actionObject)	// 138 = 0x8A
{    
    actionObject["Frame"] = parser->getUI16();
    actionObject["SkipCount"] = parser->getUI8();
    return TRUE;
}



int ActionSetTarget(Action *action, SWFParser *parser, VObject &actionObject)	// 139 = 0x8B
{    
    actionObject["TargetName"] = parser->getSTRING();
    return TRUE;
}

int ActionGoToLabel(Action *action, SWFParser *parser, VObject &actionObject)	// 140 = 0x8C
{    
    actionObject["Label"] = parser->getSTRING();
    return TRUE;
}

int ActionWaitForFrame2(Action *action, SWFParser *parser, VObject &actionObject)	// 141 = 0x8D
{
    actionObject["SkipCount"] = parser->getUI8();
    return TRUE;
}

int ActionDefineFunction2(Action *action, SWFParser *parser, VObject &actionObject)	// 142 = 0x8E
{   
    unsigned int NumParams = 0;
    actionObject["FunctionName"] = parser->getSTRING();
    NumParams = parser->getUI16();
    actionObject["NumParams"] = NumParams;
    actionObject["RegisterCount"] = parser->getUI8();
    actionObject["PreloadParentFlag"] = parser->getUBits(1);
    actionObject["PreloadRootFlag"] = parser->getUBits(1);
    actionObject["SuppressSuperFlag"] = parser->getUBits(1);
    actionObject["PreloadSuperFlag"] = parser->getUBits(1);
    actionObject["SuppressArgumentsFlag"] = parser->getUBits(1);
    actionObject["PreloadArgumentsFlag"] = parser->getUBits(1);
    actionObject["SuppressThisFlag"] = parser->getUBits(1);
    actionObject["PreloadThisFlag"] = parser->getUBits(1);
    actionObject["Reserved"] = parser->getUBits(7);
    actionObject["PreloadGlobalFlag"] = parser->getUBits(1);

    VObject &param = actionObject["Parameters"];
    
    for (unsigned int i = 0; i < NumParams; i++) {
        param[i].setTypeInfo("REGISTERPARAM");
        param[i]["Register"]  = parser->getUI8();
        param[i]["ParamName"] = parser->getSTRING();
    }
    
    actionObject["codeSize"] = parser->getUI16();
    return TRUE;
}

int ActionTry(Action *action, SWFParser *parser, VObject &actionObject)	// 143 = 0x8F
{   
    unsigned int CatchInRegisterFlag = 0, TrySize = 0, CatchSize = 0, FinallySize = 0;
    
    actionObject["Reserved"] = parser->getUBits(5);
    
    CatchInRegisterFlag = parser->getUBits(1);
    actionObject["CatchInRegisterFlag"] = CatchInRegisterFlag;
    
    actionObject["FinallyBlockFlag"]    = parser->getUBits(1);
    actionObject["CatchBlockFlag"]      = parser->getUBits(1);
    
    TrySize     = parser->getUI16();
    CatchSize   = parser->getUI16();
    FinallySize = parser->getUI16();
    
    actionObject["TrySize"]     = TrySize;
    actionObject["CatchSize"]   = CatchSize;
    actionObject["FinallySize"] = FinallySize;
    
    if (CatchInRegisterFlag) {
        actionObject["CatchRegister"] = parser->getUI8();
    } else {
        actionObject["CatchName"] = parser->getSTRING();
    }
    
    for (unsigned int i = 0; i < TrySize; i++) {
        actionObject["TryBody"][i] = parser->getUI8();
    }
    
    for (unsigned int i = 0; i < CatchSize; i++) {
        actionObject["CatchBody"][i] = parser->getUI8();
    }
    
    for (unsigned int i = 0; i < FinallySize; i++) {
        actionObject["FinallyBody"][i] = parser->getUI8();
    }
    
    return TRUE;
}

int ActionWith(Action *action, SWFParser *parser, VObject &actionObject)	// 148 = 0x94
{
    actionObject["Size"] = parser->getUI16();
    return TRUE;
}

int ActionPush(Action *action, SWFParser *parser, VObject &actionObject) // 150 = 0x96
{
    unsigned int Type = 0, valueIdx = 0;
    VObject &valueObject = actionObject["value"];
    do {
        Type = parser->getUI8();
        valueObject[valueIdx]["Type"] = Type;
        switch (Type) {
            case 0: // String
                valueObject[valueIdx]["String"] = parser->getSTRING();
                break;
            case 1: // Float
                valueObject[valueIdx]["Float"] = parser->getFLOAT();
                break;
            case 2: // null
                break;
            case 3: // undefined
                break;
            case 4: // register
                valueObject[valueIdx]["RegisterNumber"] = parser->getUI8();
                break;
            case 5: // Boolean
                valueObject[valueIdx]["Boolean"] = parser->getUI8();
                break;
            case 6: // double
                valueObject[valueIdx]["Double"] = parser->getDOUBLE();
                break;
            case 7: // integer
                valueObject[valueIdx]["Integer"] = parser->getUI32();
                break;
            case 8: // constant8
                valueObject[valueIdx]["Constant8"] = parser->getUI8();
                break;
            case 9: // constant16
                valueObject[valueIdx]["Constant16"] = parser->getUI16();
                break;
            default:
                ASSERT(1);
        }
        valueIdx++;
    } while (parser->getStreamPos() < action->NextActionPos);
    
    return TRUE;
}

int ActionJump(Action *action, SWFParser *parser, VObject &actionObject) // 153 = 0x99
{
    actionObject["BranchOffset"] = parser->getSI16();
    return TRUE;
}

int ActionGetURL2(Action *action, SWFParser *parser, VObject &actionObject)	// 154 = 0x9A
{
    actionObject["SendVarsMethod"]      = parser->getUBits(2);
    actionObject["Reserved"]            = parser->getUBits(4);
    actionObject["LoadTargetFlag"]      = parser->getUBits(1);
    actionObject["LoadVariablesFlag"]   = parser->getUBits(1);
    return TRUE;
}

int ActionDefineFunction(Action *action, SWFParser *parser, VObject &actionObject)	// 155 = 0x9B
{   
    unsigned int NumParams = 0;
    actionObject["FunctionName"] = parser->getSTRING();
    NumParams = parser->getUI16();
    actionObject["NumParams"] = NumParams;
    
    for (unsigned int i = 0; i < NumParams; i++) {
        actionObject["param"][i] = parser->getSTRING();
    }
    actionObject["codeSize"] = parser->getUI16();
    return TRUE;
}

int ActionIf(Action *action, SWFParser *parser, VObject &actionObject) // 157 = 0x9D
{
    actionObject["BranchOffset"] = parser->getSI16();
    return TRUE;
}


int ActionGotoFrame2(Action *action, SWFParser *parser, VObject &actionObject)	// 159 = 0x9F
{
    unsigned int SceneBiasFlag = 0;
    actionObject["Reserved"]        = parser->getUBits(6);
    SceneBiasFlag = parser->getUBits(1);
    actionObject["SceneBiasFlag"]   = SceneBiasFlag;
    actionObject["PlayFlag"]        = parser->getUBits(1);
    if (SceneBiasFlag) {
        actionObject["SceneBias"] = parser->getUI16();
    }
    return TRUE;
}

// This would be called recursively by DoAction Tag
/////////////////////////////////////////////////////////////////
//// Call the real action handler function according to the ActionCode
////////////////////////////////////////////////////////////////
int ActionHandler::parse(Action *action, SWFParser *parser, VObject &actionObject)
{
	unsigned int ActionCode, Length;
	
	ActionCode	= action->ActionCode;
	Length      = action->Length;
    
    //parser->dump(16);
    
	if (ActionCode > ACTIONCODE_MAX) {
		DEBUGMSG("Obfuscated ActionCode = %d\n", ActionCode);
        actionObject.setTypeInfo("Unknown ActionCode");
        parser->skip(Length);
        return FALSE;
	}
    
    actionObject.setTypeInfo(actionHandlerInfo[ActionCode].name);
    
    DEBUGMSG("ActionCode : %d, /* %s */\nLength : %d%c\n", ActionCode, actionHandlerInfo[ActionCode].name  ,Length, Length ? ',' : ' ');	
	
    
	if(actionHandlerInfo[ActionCode].handler) {
		actionHandlerInfo[ActionCode].handler(action, parser, actionObject);
		DEBUGMSG("\n");
        if (parser->getStreamPos() != action->NextActionPos) {
#ifdef DEBUG
            DEBUGMSG("ActionHeaderOffset = %d, parser pos = %d, NextActionPos = %d\n", action->ActionHeaderOffset, parser->getStreamPos(), action->NextActionPos);
            parser->seek(action->ActionHeaderOffset);
            parser->dump(action->ActionHeaderLength + action->Length);
            ASSERT (1);
#else
            parser->seek(action->NextActionPos);
#endif
        }
		return TRUE;	// Action is handled.
	}
	
    if (Length)
        ASSERT(1);

	parser->skip(Length);
	return FALSE; // Action is not handled.
}

//////////////// END //////////////////