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

extern "C" int CheckCpuId();
extern "C" void EnableSse();
extern "C" void DisableNxe();

bool CpuidIsAvailable();

void CPUID(UInt32 Code, UInt32 *EAX, UInt32 *EBX, UInt32 *ECX, UInt32 *EDX);

UInt32 CpuidGetMaxType();

void CpuidGetVendor(char *Vendor);
UInt32 CpuidGetModel();

bool CpuidCheckFlagEdx(UInt32 Flag);
bool CpuidCheckFlagEcx(UInt32 Flag);

bool CpuidCheckApic();
bool CpuidHasMsr();
UInt8 CpuidGetLapicId();

void CpuidReadMsrRaw(UInt32 MSR, UInt32 *EAX, UInt32 *EDX);
void CpuidWriteMsrRaw(UInt32 MSR, UInt32 EAX, UInt32 EDX);
UInt64 CpuidReadMsr(UInt32 MSR);
void CpuidWriteMsr(UInt32 MSR, UInt64 Value);

UInt64 CpuReadCR0();
UInt64 CpuReadCR2();
UInt64 CpuReadCR3();
UInt64 CpuReadCR4();
UInt64 CpuReadCR8();

void CpuWriteCR0(UInt64 Value);
void CpuWriteCR2(UInt64 Value);
void CpuWriteCR3(UInt64 Value);
void CpuWriteCR4(UInt64 Value);
void CpuWriteCR8(UInt64 Value);

void FxSave(void *Buffer);
void FxLoad(void *Buffer);

UInt64 GetCpuTicks();

void InitializeCpuid();
void DeinitializeCpuid();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
