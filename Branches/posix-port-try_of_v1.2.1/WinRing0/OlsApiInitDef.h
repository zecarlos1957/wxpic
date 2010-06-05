//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                     Copyright 2007-2009 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------
// for WinRing0 1.3.x

#pragma once

//-----------------------------------------------------------------------------
//
// Type Defines
//
//-----------------------------------------------------------------------------

// DLL
typedef uint32_t (WINAPI *_GetDllStatus) ();
typedef uint32_t (WINAPI *_GetDllVersion) (PBYTE major, PBYTE minor, PBYTE revision, PBYTE release);
typedef uint32_t (WINAPI *_GetDriverVersion) (PBYTE major, PBYTE minor, PBYTE revision, PBYTE release);
typedef uint32_t (WINAPI *_GetDriverType) ();

typedef BOOL (WINAPI *_InitializeOls) ();
typedef VOID (WINAPI *_DeinitializeOls) ();

// CPU
typedef BOOL (WINAPI *_IsCpuid) ();
typedef BOOL (WINAPI *_IsMsr) ();
typedef BOOL (WINAPI *_IsTsc) ();

typedef BOOL  (WINAPI *_Hlt) ();
typedef uint32_t (WINAPI *_Rdmsr) (uint32_t index, uint32_t* eax, uint32_t* edx);
typedef uint32_t (WINAPI *_Wrmsr) (uint32_t index, uint32_t eax, uint32_t edx);
typedef uint32_t (WINAPI *_Rdpmc) (uint32_t index, uint32_t* eax, uint32_t* edx);
typedef uint32_t (WINAPI *_Cpuid) (uint32_t index, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);
typedef uint32_t (WINAPI *_Rdtsc) (uint32_t* eax, uint32_t* edx);

typedef BOOL  (WINAPI *_HltTx) (uint32_t_PTR threadAffinityMask);
typedef uint32_t (WINAPI *_RdmsrTx) (uint32_t index, uint32_t* eax, uint32_t* edx, uint32_t_PTR threadAffinityMask);
typedef uint32_t (WINAPI *_WrmsrTx) (uint32_t index, uint32_t eax, uint32_t edx, uint32_t_PTR threadAffinityMask);
typedef uint32_t (WINAPI *_RdpmcTx) (uint32_t index, uint32_t* eax, uint32_t* edx, uint32_t_PTR threadAffinityMask);
typedef uint32_t (WINAPI *_CpuidTx) (uint32_t index, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx, uint32_t_PTR threadAffinityMask);
typedef uint32_t (WINAPI *_RdtscTx) (uint32_t* eax, uint32_t* edx, uint32_t_PTR threadAffinityMask);

typedef BOOL  (WINAPI *_HltPx)   (uint32_t_PTR processAffinityMask);
typedef uint32_t (WINAPI *_RdmsrPx) (uint32_t index, uint32_t* eax, uint32_t* edx, uint32_t_PTR processAffinityMask);
typedef uint32_t (WINAPI *_WrmsrPx) (uint32_t index, uint32_t eax, uint32_t edx, uint32_t_PTR processAffinityMask);
typedef uint32_t (WINAPI *_RdpmcPx) (uint32_t index, uint32_t* eax, uint32_t* edx, uint32_t_PTR processAffinityMask);
typedef uint32_t (WINAPI *_CpuidPx) (uint32_t index, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx, uint32_t_PTR processAffinityMask);
typedef uint32_t (WINAPI *_RdtscPx) (uint32_t* eax, uint32_t* edx, uint32_t_PTR processAffinityMask);

// I/O
typedef BYTE  (WINAPI *_ReadIoPortByte) (WORD address);
typedef WORD  (WINAPI *_ReadIoPortWord) (WORD address);
typedef uint32_t (WINAPI *_ReadIoPortDword) (WORD address);

typedef BOOL (WINAPI *_ReadIoPortByteEx) (WORD address, PBYTE value);
typedef BOOL (WINAPI *_ReadIoPortWordEx) (WORD address, PWORD value);
typedef BOOL (WINAPI *_ReadIoPortDwordEx) (WORD address, uint32_t* value);

typedef VOID (WINAPI *_WriteIoPortByte) (WORD address, BYTE value);
typedef VOID (WINAPI *_WriteIoPortWord) (WORD address, WORD value);
typedef VOID (WINAPI *_WriteIoPortDword) (WORD address, uint32_t value);

typedef BOOL (WINAPI *_WriteIoPortByteEx) (WORD address, BYTE value);
typedef BOOL (WINAPI *_WriteIoPortWordEx) (WORD address, WORD value);
typedef BOOL (WINAPI *_WriteIoPortDwordEx) (WORD address, uint32_t value);

// PCI
typedef VOID (WINAPI *_SetPciMaxBusIndex) (BYTE max);

typedef BYTE  (WINAPI *_ReadPciConfigByte) (uint32_t pciAddress, BYTE regAddress);
typedef WORD  (WINAPI *_ReadPciConfigWord) (uint32_t pciAddress, BYTE regAddress);
typedef uint32_t (WINAPI *_ReadPciConfigDword) (uint32_t pciAddress, BYTE regAddress);

typedef BOOL (WINAPI *_ReadPciConfigByteEx) (uint32_t pciAddress, uint32_t regAddress, PBYTE value);
typedef BOOL (WINAPI *_ReadPciConfigWordEx) (uint32_t pciAddress, uint32_t regAddress, PWORD value);
typedef BOOL (WINAPI *_ReadPciConfigDwordEx) (uint32_t pciAddress, uint32_t regAddress, uint32_t* value);

typedef VOID (WINAPI *_WritePciConfigByte) (uint32_t pciAddress, BYTE regAddress, BYTE value);
typedef VOID (WINAPI *_WritePciConfigWord) (uint32_t pciAddress, BYTE regAddress, WORD value);
typedef VOID (WINAPI *_WritePciConfigDword) (uint32_t pciAddress, BYTE regAddress, uint32_t value);

typedef BOOL (WINAPI *_WritePciConfigByteEx) (uint32_t pciAddress, uint32_t regAddress, BYTE value);
typedef BOOL (WINAPI *_WritePciConfigWordEx) (uint32_t pciAddress, uint32_t regAddress, WORD value);
typedef BOOL (WINAPI *_WritePciConfigDwordEx) (uint32_t pciAddress, uint32_t regAddress, uint32_t value);

typedef uint32_t (WINAPI *_FindPciDeviceById) (WORD vendorId, WORD deviceId, BYTE index);
typedef uint32_t (WINAPI *_FindPciDeviceByClass) (BYTE baseClass, BYTE subClass, BYTE programIf, BYTE index);

// Memory
#ifdef _PHYSICAL_MEMORY_SUPPORT
typedef uint32_t (WINAPI *_ReadDmiMemory) (PBYTE buffer, uint32_t count, uint32_t unitSize);
typedef uint32_t (WINAPI *_ReadPhysicalMemory) (uint32_t_PTR address, PBYTE buffer, uint32_t count, uint32_t unitSize);
typedef uint32_t (WINAPI *_WritePhysicalMemory) (uint32_t_PTR address, PBYTE buffer, uint32_t count, uint32_t unitSize);
#endif
