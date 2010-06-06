/*-------------------------------------------------------------------------*/
/* Devices.cpp                                                             */
/*                                                                         */
/*  Author:   Wolfgang Buescher (DL4YHF)                                   */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*                                                                         */
/*  Revision: 2005-07-01  (YYYY-MM-DD)                                     */
/*                                                                         */
/*  ToDo :  put i32CalibAddr[] + i32CalibMask[] to work !                  */
/*          ( required for some new chips where they scattered             */
/*            calibration bits all over the chip ... grrrr )               */
/*                                                                         */
/*  2009-06-04:  Ported to wxWidget by Philippe Chevrier                   */
/*-------------------------------------------------------------------------*/

// Database of programmable PIC devices.
// Contains everything which the programmer must know about a particular device.
//
// Once part of PIC_PRG.CPP .

#ifndef _DEVICES_H_
#define _DEVICES_H_

//#include <tchar.h>
#include <stdint.h>

 // Possible values for T_PicConfigBitInfo.iFunction :
#define CFGBIT_CUSTOM  0   // user-defined/"custom", which means none of the following...
#define CFGBIT_0_or_1  1   // bit with unknown function, no idea if H- or L-active, displayed as "0" or "1"
#define CFGBIT_H_ON    2   // any "high-active bit", H displayed as "ON", L displayed as "off"
#define CFGBIT_L_ON    3   // any "low-active bit",  L displayed as "ON", H displayed as "off"
#define CFGBIT_H_ENABLED 4 // any "high-active enable bit", H displayed as "ENABLED", L displayed as "disabled"
#define CFGBIT_L_ENABLED 5 // any "low-active enable bit",  L displayed as "ENABLED", H displayed as "disabled"
#define CFGBIT_OSC_SEL 8   // Oscillator selection bits (various bitmasks and schemes!)
#define CFGBIT_CP     10   // Code protection, general
#define CFGBIT_CPD    11   // Code protection for "data"
#define CFGBIT_PWRTEN 12   // power-up timer ENable
#define CFGBIT_WDTEN  13   // watchdog timer ENable
#define CFGBIT_LVPEN  14   // low voltage programming ENable
#define CFGBIT_BOREN  15   // brown-out reset ENable
#define CFGBIT_BORV   16   // brown-out Voltage select
#define CFGBIT_MCLREN 17   // memory clear ENable  ("Reset-input enable")
#define CFGBIT_DDEBUG 18   // debugging enable
#define CFGBIT_WRCODE 19   // code write enable
#define CFGBIT_BGCAL0 20   // bandgap calibration bit #0 (TWO bits in PIC12F629)
#define CFGBIT_BGCAL1 21   // bandgap calibration bit #1
#define CFGBIT_CCPMX0 30   // Capture/Compare pin selection (used for PIC16F818)
#define CFGBIT_GROUP  40   // A group of other "parallel" bits, treat them all the same

#define PICDEV_MAX_CONFIG_BIT_INFOS    100
#define PICDEV_MAX_CONFIG_BIT_SETTINGS 400

/*------------- Data Types ------------------------------------------------*/

typedef struct // T_PicDeviceInfo ..
{
  // general information about a particular device...
  char sz40DeviceName[44]; // like PIC16F628 (complete name, used in combo and ini file)
  int  iCodeMemType;     // 0=PIC_MT_EPROM   or   1=PIC_MT_FLASH
  int  iBitsPerInstruction;    // usually 14 bits per CPU INSTRUCTION, 16 for PIC18F, or 24 for dsPIC's
  long lCodeMemSize;           // like 2048 WORDs (a 12,14,16, or 24bit), negative = unknown
                               // very important for certain programming algorithms (PIC10F !)
  long lDataEEPROMSizeInByte;  // like 128  BYTEs,              must be defined !
  long lInternalRAMSize; // like 224  BYTEs for a 16F628, negative = unknown

  // Name of an optional "configuration register description file" .
  //    Only required for PIC18F and dsPIC30F ,  not for PIC16Fxxx .
  //    This may be the name of one of Microchip's "DEVICE"-files too,
  //    copied from C:\Programme\Microchip\MPLAB IDE\Device or similar.
  wxString sz80ConfigRegisterInfoFile;  // example: "dsPIC30F2010.dev"

  // device-specific info, required for programming:
  long lCodeMemWriteLatchSize; // number of *BYTES* (not "words") programmed at a time - only for PIC18F
  long lCodeMemEraseLatchSize; // number of *BYTES* (not "words") ERASED at a time - only for PIC18F
  long lConfMemBase;     // configuration memory base address,  0x2000 for 16Fxxx, 0xF80000 for dsPIC
  long lConfWordAdr;     // address of configuration WORD,      0x2007 for 16Fxxx, 0xF80000 for dsPIC
       // (once this was the LAST LOCATION of the config memory, but that's no longer true ! ! )
  // Which locations in the config memory are REALLY PROGRAMMABLE ("implemented") ?
  //    Here is one BIT for every "programmable location" in the config memory area :
  uint32_t dwConfMemUsedLocations0;  // address range 0x2000(->bit0) ... 0x201F(->bit31)
  uint32_t dwConfMemUsedLocations2;  // address range 0x2020(->bit0) ... 0x203F(->bit31)
  long lDataMemBase;     // data memory base address,           0x2100 for 16Fxxx, 0x7FF000 for dsPIC
  long lAddressOscCal;   // address of the (main) oscillator calibration word, <0 = none, 0x0000FF for PIC10F200, 0x0001FF for PIC10F206
  long lIdMemBase;       // ID memory base address,             0x2000 for 16Fxxx, 0xFF0000 for dsPIC,
  long lIdMemSize;       // size of ID memory IN WORDS,         8 for 16Fxxx, 8 for PIC18F, 8 for dsPIC
       // Note: sometimes the "ID Locations" contain the DEVICE ID, sometimes not !
  long lDeviceIdAddr;    // address of Device ID word,          0x2006 for 16Fxxx, 0xFF0000 for dsPIC
  long lDeviceIdMask;    // AND-mask to let only PIC ID pass,   0x3FE0 for 16Fxxx
  long lDeviceIdValue;   // device ID value (without revision)  like 0x0FC0 for 12F675

  // PLANNED(!) 2005-06 : A short list of "CALIBRATION LOCATIONS" which must be read out
  //  prior to erasing, and restored afterwards. Note that there is a terrible multitude
  //  of locations where "calibration bits and pieces" may be stored; not only CODE MEMORY
  //  but also individual BITS in the configuration memory, or whole WORDS in the config memory.
  //  To get all this "under one hat", the following principle is used :
  long  i32CalibAddr[8];   // A list of ADDRESSES which contain calibration stuff
  long  i32CalibMask[8];   // Bitmasks (32 bit, not all of them used) for each of the above locations
      // Example: PIC12F683: i32CalibAddr[0] = 0x00002008, i32CalibMask[0] = 0x00001FDF
      //          (why 32 bit each ? Who knows which config memory they may use next..)
      //          i32CalibAddr[x]=-1  means "unused entry" (ZERO is a valid address! ).
      // Note:    There is an extra address for the (old) OSCCAL value in the CODE MEMORY
      //          (lAddressOscCal), which this may be contained in i32CalibAddr[] now !


  // Options for the programming algorithm.
  //  Note: Some device-specific algo's are "hard-coded" and don't care for the following properties !
  uint16_t wCanRead;         // 1 = can read while programming,   0 = can NOT read in between
  uint16_t wEraseAlgo;       // erase & unprotect algorithm: PIC_ALGO_16FXX, PIC_ALGO_12FXX etc.
  uint16_t wCodeProgAlgo;    // programming algorithm for CODE MEMORY    (")
  uint16_t wConfigProgAlgo;  // programming algorithm for CONFIG MEMORY  (")
  uint16_t wDataProgAlgo;    // programming algorithm for DATA EEPROM    (")
  uint16_t wVppVddSequence; // ex: PROGMODE_VDD_THEN_VPP (BOTH MUST BE POSSIBLE to support new+old chips)
          // Now under full user control, using a checkbox in the main window.

  // timing requirements. Unit is MICROSECONDS.
  long lTi_Clock_us;  // minimum delay between two serial clock pulses, mostly 1 usec
  long lTi_Prog_us;   // usually 10000= 10 ms after "begin programming" for old devices
  long lTi_Erase_us;  // erase time, usually 10000= 10ms

  // Serial mode commands. Originally only used for PIC16C84 + 16F84,
  // but seemed to be valid for 16F628, 12F629/675 also .
  //        -1 means "not supported by this device" .
  int  iCmd_LoadConfig;  // once 0
  int  iCmd_LoadProg;    // once 2
  int  iCmd_ReadProg;    // once 4 (for CODE- and CONFIG memory)
  int  iCmd_IncrAddr;    // once 6
  int  iCmd_BeginProg;   // once 8
     // The following commands only apply to PICs with EPROM code memory,
     //   like the 16C71, 16C711 etc
  int  iCmd_EndProg;     //  14  for 16C71, 16C711 etc

     // The following commands only apply to PICs with DATA EEPROM Memory,
     //  (so not the 16C71, 16C711..) :
  int  iCmd_LoadDataDM;  //  3
  int  iCmd_ReadDataDM;  //  5
     // The following commands only apply to PICs with FLASH CODE Memory,
     //  (which is all 16Fxxx-types, but no 16Cxx(x)'s except for the 16C84):
  int  iCmd_EraseProg;        // 0x09, formerly known as "bulk erase" but doesn't erase CP on certain devices!
  int  iCmd_EraseData;        // 0x0B = 11
  int  iCmd_EraseChip;        // 0x1F only for 16F87xA and a few others
  int  iCmd_BeginProgNoErase; // 0x18 only for 16F87xA

  // bitmasks for the 1st configuration word and related parameters
  uint16_t wCfgmask_unused;   // all unused bits are set to "1" here
  uint16_t wCfgmask_unknown;  // programmable bits with unknown function
  uint16_t wCfgmask_cpbits;   // code protection bits, 0x3FF0 for PIC16F84
  uint16_t wCfgmask_pwrte;    // power-up timer enable,    0x0008 for 16F84
  uint16_t wCfgmask_inv_pwrte;// inversion mask for PWRTE, 0x0008 for 16F84
  uint16_t wCfgmask_wdte;     // watchdog enable config,   0x0004 for 16F84
  uint16_t wCfgmask_oscillator;// mask to isolate osc. bits,0x0003 for 16x84
  uint16_t wCfgmask_osc_rc;   //  bits for RC - oscillator,0x0003 for 16x84
  uint16_t wCfgmask_osc_hs;   //  High Speed - oscillator, 0x0002 for 16x84
  uint16_t wCfgmask_osc_xt;   //  crystal oscillator,      0x0001 for 16x84
  uint16_t wCfgmask_osc_lp;   //  low power oscillator,    0x0000 for 16x84
  uint16_t wCfgmask_osc_extclk;         // external clock to CLKIN, I/O on RA6
  uint16_t wCfgmask_osc_intrc_noclkout; // internal RC, I/O on RA6 and RA7
  uint16_t wCfgmask_osc_intrc_clkout;   // internal RC, CLKOUT on RA6, I/O on RA7
  uint16_t wCfgmask_osc_extrc_noclkout; // external RC, CLKOUT on RA6, resistor on RA7

  // new components for newer devices, since support for the PIC16F628...
  uint16_t wCfgmask_cpd   ;   //  Data EEPROM Protection,   0x0100 for 16F628
  uint16_t wCfgmask_lvp   ;   //  Low Voltage Prog. Enable, 0x0080 for 16F628
  uint16_t wCfgmask_boden ;   //  Brown-out Detect Enable,  0x0040 for 16F628
  uint16_t wCfgmask_mclre ;   //  RA5/MCLR function select, 0x0020 for 16F628

  // more new components, for the PIC16F87x family ...
  uint16_t wCfgmask_ddebug;   //  in-circuit debugger mode (16F87x, 1=DISABLE)
  uint16_t wCfgmask_wrcode;   //  writing to CODE MEMORY(!) enabled

  // .. and the story goes on, here for PIC12F620 + PIC12F675 (August 2002)..
  uint16_t wCfgmask_bandgap;  // bandgap calibration bits, 0x3000 for 12F675

  // .. and still they keep adding new ingredients for the cfg word (here: may 2003)..
  uint16_t wCfgmask_ccpmx;    // capture/compare output pin control bit (PIC16F818 et al)


  // .. and the endless story of new, incompatible configuration words continues.....
  uint16_t wCfgmask_borv;     // 2004-07, added for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable

  // .. and they keep making the life of the hobyist programmer tough,
  //    by adding a SECOND config word in the PIC16F88 (here since 2003-12-10) ..
  uint16_t wCfgmask2_used;    // all used bits in the 2nd config word must be SET(!) here
     // For the foreseeable future,
     // there will be NO DECODER for the 2nd config word
     //  (since WinPic now parses Microchip's DEV-files, which is more flexible)



  int iCheck73;  // used to check integrity of BuiltInPicDeviceInfo[], must contain '73'

} T_PicDeviceInfo;

typedef struct _T_PicCfgBitSetting
{ // = one lookup table entry for a BITCOMBINATION <-> DISPLAY COMBO
  const wxChar *szComboText;
  uint32_t  dwCombiValue;   // note: BITS RIGHT ALIGNED !  (unlike *.dev file)
  uint32_t  dwExtraMask;    // same here: Bit ZERO should be SET !
  struct _T_PicCfgBitSetting * pNext; // NULL indicates end of list
} T_PicConfigBitSetting, *P_PicConfigBitSetting;

typedef struct // T_PicConfigBitInfo : info about a bit or a group of bits in the config memory
{
  const wxChar *szText; // strings like "Code Protection", "Power-Up timer", ..
                      // szText = NULL indicates end of list
  int   iFunction;    // indicates a predefined "standard function" of this bit
  uint32_t dwAddress;    // target address (like 0x002007 for PIC16Fxxx)
  uint32_t dwBitmask;    // bitmask (usually only ONE bit set in this value)
  T_PicConfigBitSetting * pBitCombinations; // Pointer to a chain of "Bit Combination Combo Strings".
      // (if this pointer is NULL, the value will be displayed in numeric form)
  bool  fIsBuiltIn;   // Indicates that the value is built-in and shall not be freed!!!
} T_PicConfigBitInfo; // -> PicDev_ConfigBitInfo[PICDEV_MAX_CONFIG_BIT_INFOS]


/*------------- Variables  ------------------------------------------------*/
#undef EXTERN
#ifdef _I_AM_DEVICES_
 #define EXTERN
#else
 #define EXTERN extern
#endif

EXTERN T_PicDeviceInfo PIC_DeviceInfo;  // current PIC device-type and related parameters
       // More info, somehow "derived" from PIC_DeviceInfo, set in PIC_PRG_SetDeviceType() etc..
EXTERN long PIC_DeviceInfo_iConfMemSize; // total size of the "configuration memory"
EXTERN long PicDevInfo_i32ConfMemAddressFactor; // 2 for PIC18F ("words", but each BYTE has its own address)
EXTERN T_PicConfigBitInfo PicDev_ConfigBitInfo[PICDEV_MAX_CONFIG_BIT_INFOS];


/*------------- Prototypes ------------------------------------------------*/

  // Many of these routines are called from "C" (not CPP), so:
#ifdef __cplusplus      /* to call these routines from CPP modules : */
 #define CPROT extern "C"
 // Note: It's 'extern "C"' , not 'extern "c"' as stated in
 //       Borland's bugged help system ! (at least for BCB4)
#else
 #define CPROT          /* to call these routines from ordinary "C": */
#endif  /* nicht "cplusplus" */


CPROT int   PicDev_GetDeviceInfoByIndex(int iTableIndex, T_PicDeviceInfo *pDstDeviceInfo );
CPROT int   PicDev_GetDeviceInfoByName(const char *pszDeviceName, T_PicDeviceInfo *pDstDeviceInfo );
CPROT bool  PicDev_FillConfigBitInfoTable( T_PicDeviceInfo *psrcPicDeviceInfo );
CPROT const char *PicDev_GetDeviceNameByIdWord( uint16_t wDeviceIdWord, int iBitsPerInstruction );
CPROT const char *PicDev_AlgorithmCodeToString(int iAlgorithm);
CPROT int   PicDev_StringToAlgorithmCode(const char *pszAlgorithmName);
CPROT int   PicDev_StringToVppVddSequenceCode(const char *pszVppVddSequence);
CPROT const char *PicDev_VppVddSequenceCodeToString(int iVppVddSequence);
CPROT int   PicDev_GetNrOfBitsPerIdLocation(void);
CPROT int   PicDev_GetConfMemSize(void);   // better use PIC_DeviceInfo_iConfMemSize for speed reasons !
CPROT int   PicDev_IsConfigMemLocationValid(uint32_t dwCfgMemAddress);
CPROT uint32_t PicDev_GetVerifyMaskForAddress(uint32_t dwTargetAddress);
CPROT const wxChar *PicDev_GetInfoOnConfigMemory(uint32_t dwCfgMemAddress);
CPROT const wxChar *PicDev_ConfigBitValueToString( T_PicConfigBitInfo *pConfigBitInfo, uint32_t dwConfigRegisterContent );
CPROT bool  PicDev_ConfigBitStringToValue( T_PicConfigBitInfo *pConfigBitInfo, const wxChar *pszComboText, uint32_t *pdwDest );
CPROT void  PicDev_FillDefaultDeviceInfo(T_PicDeviceInfo *pDeviceInfo);
CPROT void  PicDev_DumpDeviceListToFile( const wxChar *pszDumpFileName );


#endif // _DEVICES_H_
