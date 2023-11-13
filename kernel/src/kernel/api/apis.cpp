//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "apis.h"

#include "sysapi.h"

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeApis()
{
	InitializeSysApi();
}

void DeinitializeApis()
{
	//
}