//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Once

#pragma once

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void ClearPageFrame(UInt64 PageFrame);
void ClearPageFrames(UInt64 PageFrameFrom, UInt64 Count);
void ClearPageFrameRange(UInt64 PageFrameFrom, UInt64 PageFrameTo);

void CopyPageFrame(UInt64 PageFrameFrom, UInt64 PageFrameTo);
void CopyPageFrameRange(UInt64 PageFrameFrom, UInt64 PageFrameTo, UInt64 PageFrameCount);

void InitializeMmUtil();
void DeinitializeMmUtil();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
