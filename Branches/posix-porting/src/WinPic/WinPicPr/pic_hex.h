/*-------------------------------------------------------------------------*/
/* pic_hex.h                                                               */
/*     Part of Wolfgang Buescher's  simple PIC-Programmer for Windows.     */
/*-------------------------------------------------------------------------*/

/*
 * Header file for hex load/dump routines.
 *
 * Revision history:
 *
 * 11-Jul-1996: V-0.0; wrote definitions
 * 14-Jul-1996: V-0.1; created separate header file
 * 17-Sep-2000: Migrated to DL4YHF's Windooze - PIC-Programmer  (V1.0).
 *              File renamed from HEX.C to PIC_HEX.C
 *              REMOVED ALL TAB CHARACTERS. Keep it this way.. thanks !
 * (now using ISO8601 int'l date format which is YYYY-MM-DD) ...
 * 2003-12-10 : Added support for the 2nd config word in a PIC16F88 .
 *
 * Copyright (C) 1996 David Tait.  All rights reserved.
 * Permission is granted to use, modify, or redistribute this software
 * so long as it is not sold or exploited for profit.
 *
 * Copyright (C) 2003 Wolfgang Buescher.
 * THIS SOFTWARE IS PROVIDED AS IS AND WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED.
 *
 */


#define PIC_HEX_WORDS_PER_HEX_DUMP 8 /* dump this many words per hex record */

  // Identifiers and max.sizes for all 'buffers'. Don't change the sequence :
  // Meanwhile, there are A LOT OF BUFFERS because Microchip decided to split
  // everything up in the PIC18F (and use funny names for them... see below)
  // - CODE :  Code memory. Up to now, begins at address zero
  // - DATA :  Data EEPROM. Start address is family-specific, for dsPIC even device-specific.
  // - CONFIG: One or two 14-bit CONFIGURATION WORDS in old devices,
  //           but a lot of 16-bit CONFIGURATION REGISTERS .
  //      PIC16F628: "CONFIG MEM" is address range 0x2000 to 0x2007,
  //                 "CONFIG WORD" at 0x2007, NO(!) Device ID,
  //               "ID Locations" are part of the CONFIG MEMORY RANGE !
  //      PIC18F: This buffer does NOT contain the "ID Space",
  //              though Microchip calls this "Configuration and ID Space",
  //              ranging from 0x200000(!) to 0x3FFFFF(!) but only contains
  //              8(ID) + 14(Config) + 2(Device ID) locations.
  //              Allocating a 2-MByte-buffer for this would be overkill,
  //               so neither "ID Locations" nor "Device ID word" are part
  //               of the "CONFIG" buffer in this ugly case !
  //      dsPIC:  Similar ugly game as for PIC18F, but totally different addresses
  //              of course (CONFIG: 0xF80000..0xF8000F, Device ID: 0xFF0000,
  //              "ID Locations" seem to be missing completely)
  // - EXEC:    Special code memory for "bootloader", "debugger" or similar.
  //              Only exists for dsPIC .
  // - ID LOCATIONS: See notes on "CONFIG"... sometimes part of config mem,
  //              sometimes extra buffer, sometimes not existing at all (yucc).
  // - DEVICE ID   : See notes above... sometimes part of ID LOCATIONS,
  //              sometimes an extra buffer, sometimes not existing at all (eek).
  //
#define PIC_BUF_CODE          0 /* buffer nr for the CODE MEMORY buffer      */
#define PIC_BUF_CODE_SIZE 65536 /* max size of code memory IN INSTRUCTION WORDS */
#define PIC_BUF_DATA          1 /* buffer nr for the DATA MEMORY buffer      */
#define PIC_BUF_DATA_SIZE  4096 /* max size of EEDATA memory IN BYTES (!)    */
#define PIC_BUF_CONFIG        2 /* buffer nr for the CONFIG MEMORY buffer    */
#define PIC_BUF_CONFIG_SIZE  64 /* max size of CONFIG memory (SOMETIMES incl. ID memory) */
#define PIC_BUF_EXEC          3 /* buffer nr for the EXECUTIVE MEMORY buffer     */
#define PIC_BUF_EXEC_SIZE  1024 /* max size of dsPIC's "executive code memory" IN INSTRUCTION WORDS */
#define PIC_BUF_ID_LOCATIONS  4 /* buffer nr for the ID locations (incl. DEVICE-ID) */
#define PIC_BUF_ID_MEM_SIZE   8 /* max size of the "identification memory" in 'WORDS' */
   /* why different buffers for "config memory" and "ID locations" ? see dsPIC .. :-( */
#define PIC_BUF_DEVICE_ID     5 /* buffer nr for the DEVICE-ID(s)  ... if needed */
#define PIC_BUF_DEV_ID_SIZE   8 /* max size of the "DEVICE ID memory" in 'WORDS' */
#define PIC_BUFFERS           6 /* number of different buffer types for PIC data */

  // ex: #define PIC_HEX_NR_CONFIG_WORDS  2 /* max count of configuration words */
  // (TWO words since 2003-12 because of PIC16F88;  removed 2004, now part of CONFIG MEMORY)
#define PIC_HEX_MAX_DEVICE_ID_LOCATIONS 2 /* dsPICs: 2 device ID words @ 0xFF0000 - 0xFF0002 */

#define PIC_HEX_FORMAT_UNKNOWN -1      /* hex formats */
#define PIC_HEX_FORMAT_INHX8M  0
#define PIC_HEX_FORMAT_INHX16  1

#define PIC_HEX_ERROR_EOF  -1      /* unexpected EOF */
#define PIC_HEX_ERROR_DEX  -2      /* hex digit required */
#define PIC_HEX_ERROR_CEX  -3      /* missing ':' */
#define PIC_HEX_ERROR_CHK  -4      /* checksum error */
#define PIC_HEX_ERROR_FILE_NOT_FOUND -5
#define PIC_HEX_ERROR_IGN  1       /* warning that some records were ignored */

    // FLAGS for each word in the program buffer
#define PIC_BUFFER_FLAG_UNUSED    0
#define PIC_BUFFER_FLAG_USED      1  // LOADED or READ<>3FFF
#define PIC_BUFFER_FLAG_PRG_ERROR 2  // PROGRAM or VERIFY error
#define PIC_BUFFER_FLAG_VFY_ERROR 4
#define PIC_BUFFER_FLAG_DEAD      8  // Failed repeatedly (R.I.P.)
#define PIC_BUFFER_FLAG_SPECIAL  16  // special location, like osc calib word

/*---------------- types     ----------------------------------------------*/
class T_PicBufferInfo
{
public:
  uint32_t  *pdwData;  // points to buffer (somewhere in memory, max 32 bit per location)
  uint32_t  *pdwCopy;  // points to buffer that is the copy of pdwData for editing
  uint16_t   *pwFlags;  // points to "flags" (combination of PIC_BUFFER_FLAG_xxx)
  uint16_t   *pwFlagCopy; // points to copy of pwFlags for editing
  uint32_t  dwMaxSize; // allocated size of this buffer
  long   i32LastUsedArrayIndex;  // last used array index (-1 = "nothing in it")
  long   i32LastUsedIndexCopy;   // Copy of last used array index for editing
  // ( Notice the difference between "last used array index" and
  //   "count of used locations".
  //   The latter is pretty useless, because the "used locations"
  //   may be spread all over the available space.
  //   We assume the "first used array index" is zero, in fact it usually is,
  //   and it has got absolutely nothing to do with an "address" . )
  uint32_t  dwAddressOffset; // device-dependent address of first buffer location
  uint32_t  dwAddressFactor; // device-dependent relationship between buffer index and display address
     // displayed_device_address := dwAddressOffset + buffer_index * dwAddressFactor
     // Examples: PIC16F628, code memory : dwAddressOffset=0, dwAddressFactor=1
     //           dsPIC30F , code memory : dwAddressOffset=0, dwAddressFactor=2
     // ( dsPIC30F is bizarre!! TWO address steps per instruction, despite THREE bytes per instruction ! )

  int    iBitsPerElement; // how many meaningful bits in one pdwData[]-array element ?
     // Examples: 8 = one BYTE per array element (used for DATA EEPROM)
     //          16 = one WORD per array element (used for most others, except..)
     //          24 = one 24-BIT "instruction word" for a dsPIC30F

  uint32_t ArrayIndexToTargetAddress( uint32_t dwArrayIndex ) { return dwAddressOffset + dwAddressFactor * dwArrayIndex; }

};


/*---------------- variables ----------------------------------------------*/
#undef EXTERN
#ifdef _I_AM_PIC_HEX_
 #define EXTERN
#else
 #define EXTERN extern
#endif

// Buffers for all kinds of memories...
EXTERN T_PicBufferInfo PicBuf[PIC_BUFFERS];

  // ex:  WORD PIC_HEX_config_word[PIC_HEX_NR_CONFIG_WORDS]; /* code protect, watchdog, oscillator.. */
  // Since 2004-08-02, the config WORDS are part of the CONFIG MEMORY :
  // Sometimes the thingy at 0x2008 is called "Calibration Word 1", sometimes not .
  // To cope with this ugly situation, there are Get.. and Set.. - routines to access
  // the "configuration words" and "calibration words", depending on the chip type !

EXTERN int  PIC_HEX_check;
EXTERN int  PIC_HEX_fail;


///*---------------- prototypes ---------------------------------------------*/
//
//  // Many of these routines are called from "C" (not CPP), so:
//#ifdef __cplusplus      /* to call these routines from CPP modules : */
// #define extern "C"
// // Note: It's 'extern "C"' , not 'extern "c"' as stated in
// //       Borland's bugged help system ! (at least for BCB4)
//#else
// #define CPROT          /* to call these routines from ordinary "C": */
//#endif  /* nicht "cplusplus" */


/***************************************************************************/
long PicHex_ParseInteger(const char *cp, long lDefaultValue);
bool PicHex_GetHexValueFromSource( const char **ppszSource, uint32_t *pdwDest);

/***************************************************************************/
int  PIC_HEX_Init(void);
 /* Initializes all variables of the module.
  * Must be called before any other routine of this module.
  */


/***************************************************************************/
void PIC_HEX_ClearBuffers(void);
/* Initialize buffers to imitate the erased state.
 *  Depends on PIC_DeviceInfo !
 */


/***************************************************************************/
int PIC_HEX_LoadFile( const wxString fname );    // Loads a Microchip hex file
int PIC_HEX_DumpHexFile( const wxString fname ); // Dumps buffers in Microchip HEX format


/***************************************************************************/
  // Buffer access routines.  Return value > 0 means successfull,
  //       everything else is an error code (zero or negative).
  //       NOTE: THE RETURN CODE IS NOT THE "VALUE READ FROM BUFFER" !
  // About the term "Target Address" :
  //       This means a target(="PIC")-device-specific address,
  //       not an address of something in the PC's memory !
  //       (thats why i32TargetAddress is an integer, but not a pointer) .
T_PicBufferInfo * PicBuf_TargetAddressToBufPtr( long i32TargetAddress, long * pi32ArrayIndex );
int PicBuf_GetBufferWord(long i32TargetAddress, uint32_t *pdwDest); // may be 8..24 bit !
int PicBuf_SetBufferWord(long i32TargetAddress, uint32_t dwData);
int PicBuf_GetBufferByte(long i32TargetAddress, uint8_t *pbDest); // always 8 bit, odd address allowed
int PicBuf_SetBufferByte(long i32TargetAddress, uint8_t bSource);


/***************************************************************************/
uint16_t  PicBuf_GetMemoryFlags(long i32TargetAddress);
void  PicBuf_SetMemoryFlags(long i32TargetAddress,  uint16_t wFlags);

/***************************************************************************/
void  PicBuf_SetConfigWord(int iCfgWordIndex, uint16_t wCfgWordValue);
uint16_t  PicBuf_GetConfigWord(int iCfgWordIndex );

/***************************************************************************/
uint32_t PicBuf_ArrayIndexToTargetAddress( int iBufMemType, uint32_t dwArrayIndex );


/* EOF <pic_hex.h> */



