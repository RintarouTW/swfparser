/*
 *  ActionDefine.h
 *  swfparser
 *
 *  Created by Rintarou on 2010/8/7.
 *  Copyright 2010 http://rintarou.dyndns.org. All rights reserved.
 *
 */


#ifndef _ACTIONDEFINE_H
#define _ACTIONDEFINE_H

/*
 ActionCode	Name
 --------------------------------------------
 SWF3 (DoAction Tag)
 --------------------------------------------
 0x04		ActionNextFrame
 0x05		ActionPreviousFrame
 0x06		ActionPlay
 0x07		ActionStop
 0x08		ActionToggleQuality
 0x09		ActionStopSounds
 
 0x81		ActionGotoFrame             X
 0x83		ActionGetURL                X
 0x8A		ActionWaitForFrame          X
 0x8B		ActionSetTarget             X
 0x8C		ActionGoToLabel             X
 --------------------------------------------
 SWF4 (Stack based VM)
 --------------------------------------------
 0x0A		ActionAdd
 0x0B		ActionSubtract
 0x0C		ActionMultiply
 0x0D		ActionDivide
 0x0E		ActionEquals
 0x0F		ActionLess
 
 0x10		ActionAnd
 0x11		ActionOr
 0x12		ActionNot
 0x13		ActionStringEquals
 0x14		ActionStringLength
 0x15		ActionStringExtract
 0x17		ActionPop
 0x18		ActionToInteger
 0x1C		ActionGetVariable
 0x1D		ActionSetVariable
 
 0x20		ActionSetTarget2
 0x21		ActionStringAdd
 0x22		ActionGetProperty
 0x23		ActionSetProperty
 0x24		ActionCloneSprite
 0x25		ActionRemoveSprite
 0x26		ActionTrace
 0x27		ActionStartDrag
 0x28		ActionEndDrag
 0x29		ActionStringLess
 
 0x30		ActionRandomNumber
 0x31		ActionMBStringLength
 0x32		ActionCharToAscii
 0x33		ActionAsciiToChar
 0x34		ActionGetTime
 0x35		ActionMBStringExtract
 0x36		ActionMBCharToAscii
 0x37		ActionMBAsciiToChar
 
 0x8D		ActionWaitForFrame2         X
 
 0x96		ActionPush                  X
 0x99		ActionJump                  X
 0x9A		ActionGetURL2               X
 0x9D		ActionIf
 0x9E		ActionCall
 0x9F		ActionGotoFrame2            X
 --------------------------------------------
 SWF5 (ScriptObject)
 --------------------------------------------
 0x3A		ActionDelete
 0x3B		ActionDelete2
 0x3C		ActionDefineLocal
 0x3D		ActionCallFunction 
 0x3E		ActionReturn
 0x3F		ActionModulo
 
 0x40		ActionNewObject
 0x41		ActionDefineLocal2
 0x42		ActionInitArray
 0x43		ActionInitObject
 0x44		ActionTypeOf
 0x45		ActionTargetPath
 0x46		ActionEnumerate
 0x47		ActionAdd2
 0x48		ActionLess2
 0x49		ActionEquals2
 0x4A		ActionToNumber
 0x4B		ActionToString
 0x4C		ActionPushDuplicate
 0x4D		ActionStackSwap
 0x4E		ActionGetMember
 0x4F		ActionSetMember
 
 0x50		ActionIncrement
 0x51		ActionDecrement 
 0x52		ActionCallMethod
 0x53		ActionNewMethod
 
 0x60		ActionBitAnd
 0x61		ActionBitOr
 0x62		ActionBitXor
 0x63		ActionBitLShift
 0x64		ActionBitRShift
 0x65		ActionBitURShift
 
 0x87		ActionStoreRegister         X
 0x88		ActionConstantPool          X
 
 0x94		ActionWith                  X
 0x9B		ActionDefineFunction        X
 --------------------------------------------
 SWF6 (DoInitAction Tag)
 --------------------------------------------
 0x54		ActionInstanceOf
 0x55		ActionEnumerate2
 
 0x66		ActionStrictEquals
 0x67		ActionGreater
 0x68		ActionStringGreater
 --------------------------------------------
 SWF7
 --------------------------------------------
 0x2A		ActionThrow
 0x2B		ActionCastOp
 0x2C		ActionImplementsOp
 
 0x69		ActionExtends
 
 0x8E		ActionDefineFunction2       X
 0x8F		ActionTry                   X
 --------------------------------------------
 SWF9 (DoABC Tag)
 --------------------------------------------
 --------------------------------------------
 SWF10 (No changes)
 */

#define ACTIONCODE_MAX                         0x9F

#endif /* _ACTIONDEFINE_H */