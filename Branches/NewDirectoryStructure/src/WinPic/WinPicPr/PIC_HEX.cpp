/*-------------------------------------------------------------------------*/
/* PIC_HEX.cpp                                                             */
/*   Specialized HEX-file import/export for PIC microcontrollers.          */
/*                                                                         */
/*  Author:   Wolfgang Buescher (DL4YHF)                                   */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*  2009-06-04:  Ported to wxWidget by Philippe Chevrier                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*
 * Load/dump Microchip (Intel) format hex files.  Only INHX8M supported .
 *
 * Revision history:
 *
 * 10-Mar-1994: V-0.0; initial routines written for pp.c V-0.3
 * 11-Jul-1996: V-0.1; deals with integrated files, auto-identify format
 * 17-Sep-2000: Migrated to DL4YHF's Windooze - PIC-Programmer  (V1.0).
 *              File renamed from HEX.C to PIC_HEX.C
 *              REMOVED ALL TAB CHARACTERS. Keep it this way.. thanks !
 * 2006-04-13 : Added some mods by L.Lisovskiy required for PIC12F508 .
 *
 * HEX.C:  Copyright (C) 1996 David Tait.  All rights reserved.
 *    Permission is granted to use, modify, or redistribute this software
 *    so long as it is not sold or exploited for profit.
 *
 * PIC_HEX.C: Adaption for Windows(tm) & C++Builder.
 *   Copyright(C) 2000  by Wolfgang Buescher.  All rights reserved.

 *
 * THIS SOFTWARE IS PROVIDED AS IS AND WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED.
 *
 */

#include <wx/intl.h>
#include <wx/file.h>
#include <stdio.h>
#include <stdlib.h>

#include <Wx/Appl.h>       // APPL_ShowMsg(), etc
#include "PIC_PRG.h"   // some infos in PIC_dev_param are required here !
#include "LoadHex.h"   // newer, "universal" routine to load hex files (also for dsPIC)

#define _I_AM_PIC_HEX_ 1
#include "PIC_HEX.h"

#ifndef false
 #define false 0
#endif

#ifndef true
 #define true 1
#endif



/*------------- Implementation --------------------------------------------*/

/***************************************************************************/
int PIC_HEX_Init(void)
 /* Initializes all variables of the module.
  * Must be called before any other routine of this module.
  * Should *not* depend on PIC_DeviceInfo !
  */
{
  static int buffers_allocated = 0;
  static long i32BufferSizes[PIC_BUFFERS] = { // max buffer sizes for init loop
        PIC_BUF_CODE_SIZE,
        PIC_BUF_DATA_SIZE,
        PIC_BUF_CONFIG_SIZE,
        PIC_BUF_EXEC_SIZE ,
        PIC_BUF_ID_MEM_SIZE,
        PIC_BUF_DEV_ID_SIZE };

  int i;

  if( ! buffers_allocated )
   { buffers_allocated = 1;
     for(i=0; i<PIC_BUFFERS; ++i)
      {
        PicBuf[i].pdwData =(uint32_t*)malloc(sizeof(uint32_t) * i32BufferSizes[i] );
        if( PicBuf[i].pdwData == NULL )
           return 0;   // should NEVER EVER HAPPEN under windows, but who knows
        PicBuf[i].pdwCopy =(uint32_t*)malloc(sizeof(uint32_t) * i32BufferSizes[i] );
        if( PicBuf[i].pdwCopy == NULL )
           return 0;
        PicBuf[i].pwFlags = (uint16_t*)malloc(sizeof(uint16_t) * i32BufferSizes[i] );
        if( PicBuf[i].pwFlags == NULL )
           return 0;
        PicBuf[i].pwFlagCopy = (uint16_t*)malloc(sizeof(uint16_t) * i32BufferSizes[i] );
        if( PicBuf[i].pwFlags == NULL )
           return 0;
        PicBuf[i].dwMaxSize = i32BufferSizes[i]; // max USABLE "entries" in this buffer
      }
   } // end if( ! buffers_allocated )
  for(i=0; i<PIC_BUFFERS; ++i)
   { PicBuf[i].i32LastUsedArrayIndex = -1;
     PicBuf[i].dwAddressFactor = 2;  // assume there will be 16-bit WORDs in this buffer
     PicBuf[i].dwAddressOffset = 0;  // assume 1st location in this buffer is at address ZERO in target
     PicBuf[i].iBitsPerElement = 16; // just an inital guess. device-dependent !
   }

  return 1;

} // end PIC_HEX_Init()

//---------------------------------------------------------------------------
long PicHex_ParseInteger(const char *cp, long lDefaultValue)
  // parses an integer value from a text string, decimal or hexadecimal (with prefix)
{ long ret=0;
   int neg=0;  int hex=0;
   char c;
//   int  digit;
   const char *pszSource;

   while(*cp==' ')  // skip optional spaces
        ++cp;
   pszSource = cp;  // analysis begins here (to detect if anything parsed at all)

   if (*pszSource=='-') { neg=1; ++pszSource; }     /* negative Zahl */
   if (*pszSource=='$') { hex=1; ++pszSource; }     /* Hex-Zahl      */
     else
   if (pszSource[0]=='0' && (pszSource[1]=='x' || pszSource[1]=='X') )
                        { hex=1; pszSource+=2; }    /* Hex-Zahl im "C"-Stil */
     else
   if (*pszSource=='#') { hex=0; ++pszSource; }     /* Dez-Zahl      */
   if (hex)
    {                                    /* Einlesen einer HEX-Zahl : */
     do {
         c=*pszSource++;                       /* naechstes Zeichen holen */
         if ( (c>='0') && (c<='9') )
            ret = (ret*16) + (c-'0')    ;        /* 0 .. 9 */
         else
         if ( (c>='a') && (c<='f') )
            ret = (ret*16) + (c-'a'+10) ;       /* a .. f */
         else
         if ( (c>='A') && (c<='F') )
            ret = (ret*16) + (c-'A'+10) ;       /* A .. F */
         else break;
        } while(c);
     --pszSource;     /* das letzte Inkrementieren ging zu weit ! */
    }  /* if (hex) */
   else
    {                                    /* Einlesen einer DEZ-Zahl : */
     do {
         c=*pszSource++;                 /* naechstes Zeichen holen */
         if ( (c>='0') && (c<='9') )
            ret = (ret * 10) + (c-'0')   ;       /* 0 .. 9 */
         else break;
        } while(c);
     --pszSource;     /* das letzte Inkrementieren ging zu weit ! */
    }
   if (neg) ret = -ret;

   if(pszSource==cp)  // nothing parsed; return "default" value
      ret = lDefaultValue;

   return ret;
} /* end PicHex_ParseInteger() */


//---------------------------------------------------------------------------
bool PicHex_GetHexValueFromSource( const char **ppszSource, uint32_t *pdwDest)
   // Parses and skips a 32-bit unsigned integer, ALWAYS in hexadecimal format, no prefix
{
 const char *pszSource;
 char c;
 uint32_t dwValue  = 0;
 int  nDigits = 0;
 bool fResult = false;

  pszSource = *ppszSource;

  while(*pszSource==' ')
       ++pszSource;

  // At least TWO hex digits must be parsed.
  // Otherwise, the source data are considered ASCII characters.
  while( ((c=*pszSource)>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F') )
   { ++pszSource;
     ++nDigits;
     fResult = true;
     dwValue = dwValue<<4;
     if(c>='0' && c<='9') dwValue |= (long)(c-'0');
     else if(c>='a' && c<='f') dwValue |= (long)(c-'a'+10);
     else if(c>='A' && c<='F') dwValue |= (long)(c-'A'+10);
   }

  if(nDigits>=2)
   { if(pdwDest) *pdwDest = dwValue;
     *ppszSource = pszSource;
     return fResult;
   }
  else  // doesn't look like valid HEX values... guess it's ASCII :
   {
     return false;
   }
} // end PicHex_GetHexValueFromSource()


/***************************************************************************/
void PIC_HEX_ClearBuffers(void)
/* Initialize buffers to imitate the erased state.
 * NEEDS VALID INFORMATION in PIC_DeviceInfo !
 */
{
  long  i,j;
  uint32_t dw;

  PicBuf_SetConfigWord(0,  // set a DEFAULT config word for this type of PIC:
             PIC_DeviceInfo.wCfgmask_unused  // all unused bits HIGH="ERASED"
          |  PIC_DeviceInfo.wCfgmask_unknown // programmable bits with unknown function
          |  PIC_DeviceInfo.wCfgmask_cpbits  // all CP bits "high" means protection OFF
          | (PIC_DeviceInfo.wCfgmask_pwrte^PIC_DeviceInfo.wCfgmask_inv_pwrte)
          |  PIC_DeviceInfo.wCfgmask_osc_rc  // select RC oscillator configuration
          |  PIC_DeviceInfo.wCfgmask_cpd     //  Data EEPROM Protection off
          |  PIC_DeviceInfo.wCfgmask_lvp     //  Low Voltage Prog. Enabled
          |  PIC_DeviceInfo.wCfgmask_boden); //  Brown-out Detect & Reset Enabled

  PicBuf_SetConfigWord(1,   // set a DEFAULT value for the 2nd config word
             PIC_DeviceInfo.wCfgmask2_used); // all unimplemented bits read as '0'



    switch( PIC_DeviceInfo.iBitsPerInstruction )
     { case 12:  dw = 0x00FFF;    break;  // stoneage PICs  ("12 bit core")
       case 14:  dw = 0x03FFF;    break;  // 12Fxxx, 16Fxxx ("14 bit core")
       case 16:  dw = 0x0FFFF;    break;  // 18Fxxx         ("16 bit core")
       case 24:  dw = 0x00FFFFFF; break;  // dsPIC30F ("enhanced 16 bit core")
       default:  dw = 0x03FFF;    break;
     }
    for ( i=0; i<PIC_BUFFERS; ++i )
      {
        uint32_t ErasedValue = (i == PIC_BUF_DATA) ? 0xFF : dw;
        PicBuf[i].i32LastUsedArrayIndex = -1;     // niente
        if( PicBuf[i].pdwData != NULL )
           for( j=0; j<(long)PicBuf[i].dwMaxSize; ++j)
              PicBuf[i].pdwData[j] = ErasedValue;
        if( PicBuf[i].pwFlags != NULL )
           for( j=0; j<(long)PicBuf[i].dwMaxSize; ++j)
              PicBuf[i].pwFlags[j] = 0x0000; // no special marks.
      }

} // end PIC_HEX_ClearBuffers()

/***************************************************************************/
T_PicBufferInfo * PicBuf_TargetAddressToBufPtr(
      long i32TargetAddress,  // input:  "device address" as seen by target
      long * pi32ArrayIndex ) // optional result: array index into T_PicBufferInfo.pdwData + pwFlags
  // "Decides" which buffer must be used for the specified target address,
  //  and converts the target address into an array index for R+W access .
  // Returns a pointer to the buffer if successful,   otherwise NULL .
  //
  // Notes ....
  // 2005-05: PIC10F20x-specific stuff (like Config-Word at 0x0FFF instead of 0x03FF)
  //          is also done here. See PIC10F_PRG.CPP for details .

{
 int  iBufNr;
 T_PicBufferInfo *pBuf;
 long i32ArrayIndex;

  // Some device-specific cruelties ...
  if( PIC_DeviceInfo.wCodeProgAlgo==PIC_ALGO_PIC10F )
   {  // for PIC10F20x : Configuration word located at 0x0FFF in the hex file,
      //                 and unfortunately also in Microchip's PIC10F206.DEV ..
      if(   (i32TargetAddress == 0x0FFF)
#if(0) // old version by W.B.
         || (i32TargetAddress == 0x01FF && PIC_DeviceInfo.lCodeMemSize==256 )
         || (i32TargetAddress == 0x03FF && PIC_DeviceInfo.lCodeMemSize==512 )
#else  // more "universal" version by Leonid Lisovskiy (added here 2006-04-13) :
         || (i32TargetAddress == (PIC_DeviceInfo.lCodeMemSize<<1) - 1 )

#endif
        )
       { // all these "addresses" mean the same location inside a PIC10F20x !
         i32TargetAddress = PicBuf[PIC_BUF_CONFIG].dwAddressOffset; // FIRST location in config buffer !
       }
   }

  // iBufNr: PIC_BUF_CODE=0, PIC_BUF_DATA=1, PIC_BUF_CONFIG=2, .. (see PIC_HEX.H)
  for( iBufNr=0; iBufNr<PIC_BUFFERS; ++iBufNr )
   { pBuf = &PicBuf[iBufNr];
     // TargetAddress := AddressOffset + ArrayIndex * AddressFactor
     // ArrayIndex = (TargetAddress - AddressOffset) / AddressFactor
     i32ArrayIndex = i32TargetAddress - pBuf->dwAddressOffset;
     if( pBuf->dwAddressFactor != 0 )
         i32ArrayIndex /= pBuf->dwAddressFactor;  // divide by 1, 2, or 4; no ROUNDING but TRUNCATING
     if( (i32ArrayIndex>=0) && (i32ArrayIndex < (long)pBuf->dwMaxSize)
        && pBuf->dwAddressOffset != (uint32_t)0xFFFFFFFFL  )
      { // Ok, possibly found the "right" buffer for the specified target address,
        //  but some pitfalls must be avoided still:
        if( iBufNr==PIC_BUF_CODE && i32ArrayIndex>=PIC_DeviceInfo.lCodeMemSize)
         { // don't think 0x2100 in a PIC16Fxxx is code memory - it's DATA memory !
         }
        else
         { // bingo, found the buffer
           if( pi32ArrayIndex ) *pi32ArrayIndex = i32ArrayIndex;
           return pBuf;
         }
      }
   } // end for  <  all buffers >

  return NULL;   // bad luck :(

} // end PicBuf_TargetAddressToBufPtr()


/***************************************************************************/
uint32_t PicBuf_ArrayIndexToTargetAddress( int iBufMemType, uint32_t dwArrayIndex )
  // Converts an ARRAY INDEX for one of the memory-type dependent buffers
  // into a TARGET ADDRESS by Microchip's own gusto .
{
  return PicBuf[iBufMemType].ArrayIndexToTargetAddress(dwArrayIndex);
} // end PicHex_CodeMemArrayIndexToAddress()


//---------------------------------------------------------------------------
// Buffer access routines.  Return value > 0 means successfull,
//       everything else is an error code (zero or negative).
//       NOTE: THE RETURN CODE IS NOT THE "VALUE READ FROM BUFFER" !
// About the term "Target Address" :
//       This means a target(="PIC")-device-specific address,
//       not an address of something in the PC's memory !
//       (thats why i32TargetAddress is an integer, not a pointer) .
//---------------------------------------------------------------------------

/***************************************************************************/
int PicBuf_GetBufferWord(
        long i32TargetAddress, // input: "device address" as seen by target
        uint32_t *pdwDest )       // output: placed in this uint32_t, regardless of 8..24 bit !
  // Quite "universal" routine to read "something" from a buffer.
  //  Which buffer to read from ?  Automatically detected from "Target address".
  // Note 1: for dsPICs, an "instruction word" is 24 bit, so WinPic uses
  //         type uint32_t for the buffers.
  //         On a PC it doesn't hurt to waste a few bytes of RAM .
  // Note 2: Even if a memory range is buffered as 16-bit WORDs,
  //         i32TargetAddress may be an ODD value ! In that case,
  //         PicBuf_GetBufferWord() + PicBuf_SetBufferWord() only access
  //         the HIGH BYTE (bits 15..8) of the 16-bit buffer entry .
  //         This is important for PIC18F because Microchip's DEV-file
  //         uses odd addresses in the CONFIG MEMORY range . See Note 3.
  // Note 3: CONFIGURATION MEMORY in PIC18F ... warning, tricky !
  //         We access the config-memory-buffer WORD-WISE, though the PIC18F
  //         datasheet treats it like an 8-bit-array (with odd+even addresses).
  //         But for the config memory buffer, dwAddressFactor is TWO,
  //         so every array index in pdwData[] contains a 16-bit WORD !
  //  (too late to change this; it would affect too many places in the project)
{
 long i32ArrayIndex;
 T_PicBufferInfo *pBuf = PicBuf_TargetAddressToBufPtr( i32TargetAddress, &i32ArrayIndex );

  if( pBuf )
    { // Ok, found the buffer for this target address,
      // and the array index is ok too ...
      if( (i32TargetAddress & 1)!=0   &&  pBuf->dwAddressFactor>1 )
       {   // 2 address-steps per 16-bit array element,
           // but ODD addresses can be used to access the higher byte ONLY:
         *pdwDest = pBuf->pdwData[i32ArrayIndex] >> 8;
       }
      else // no odd address, or BYTE array:
       { *pdwDest = pBuf->pdwData[i32ArrayIndex];
       }
      return 1;
    }

  return 0;  // error, invalid target address ?!
} // end PicBuf_GetBufferWord()



/***************************************************************************/
int  PicBuf_SetBufferWord(long i32TargetAddress, uint32_t dwData)
{
 long i32ArrayIndex;
 T_PicBufferInfo *pBuf = PicBuf_TargetAddressToBufPtr( i32TargetAddress, &i32ArrayIndex );

  if( pBuf )
    { // Ok, buffer for the target address has been found, and array index is valid
      if( (i32TargetAddress & 1)!=0   &&  pBuf->dwAddressFactor>1 )
       {   // 2 address steps per 16-bit array element,
           // but ODD addresses can be used to access the higher byte ONLY:
         pBuf->pdwData[i32ArrayIndex] = (pBuf->pdwData[i32ArrayIndex] & 0x00FF) | (dwData << 8);
       }
      else  // no odd address, or BYTE array:
       { pBuf->pdwData[i32ArrayIndex] = dwData;
       }
      return 1;
    }

  return 0; // arrived here: ERROR !  None of the buffers wanted to accept the poor li'l word ;-)

} // end PicBuf_SetBufferWord()

/***************************************************************************/
int PicBuf_GetBufferByte(long i32TargetAddress, uint8_t *pbDest)
  // Similar like PicBuf_GetBufferWord() ,
  // but the result is always 8 bit, odd addresses are always allowed .
  // Return value :  true = ok, successfully read the byte from a buffer
  //                 false= error, most likely an invalid source address
  //                        for the currently selected device .
{
 long i32ArrayIndex;
 T_PicBufferInfo *pBuf = PicBuf_TargetAddressToBufPtr( i32TargetAddress, &i32ArrayIndex );

  if( pBuf )
    { // Ok, the right buffer for the target-address has been found,
      // and the array index is also ok ...
      *pbDest = pBuf->pdwData[i32ArrayIndex];
      return 1;
    }

  return 0;  // invalid target address ?!
} // end PicBuf_GetBufferByte()

/***************************************************************************/
int PicBuf_SetBufferByte(long i32TargetAddress, uint8_t bSource)
  // Similar like PicBuf_GetBufferWord() ,
  // but here only setting a single BYTE, regardless of the target memory type.
  // Return value :  1    = ok, successfully read the byte from a buffer
  //                 <=0  = error, most likely an invalid source address
  //                        for the currently selected device .
{
 long i32ArrayIndex;
 uint32_t dwTemp;
 uint32_t dwAddressFactor;
 uint32_t dwNBitShifts; // number of bit shifts =  8 * "address modulo X" (X depends on "bits per instruction" )
 uint32_t dwDataMask;

 T_PicBufferInfo *pBuf = PicBuf_TargetAddressToBufPtr( i32TargetAddress, &i32ArrayIndex );

  if( pBuf && (i32ArrayIndex>=0) )
    { // Ok, the right buffer for the target-address has been found,
      // the array index is also ok ... now the "WIDTH" must be observed,
      // to access a "HALF WORD" properly (etc) .
      dwAddressFactor = pBuf->dwAddressFactor;
      if(dwAddressFactor<1) dwAddressFactor=1;
      if( pBuf->iBitsPerElement <= 8 )
       {  // 1..8 bit per entry in pdwData[i32ArrayIndex], easy...
          pBuf->pdwData[i32ArrayIndex] = bSource;
          return 1;
       }
      if( pBuf->iBitsPerElement <= 16 )
       {  // 9..16 bit per entry in pdwData[i32ArrayIndex], fair...
          dwNBitShifts = 8 * (i32TargetAddress & 0x0001);
       }
      else
       { // 17..24 bit per entry in pdwData[i32ArrayIndex], sometimes impossible (!)
          dwNBitShifts = 8 * (i32TargetAddress & 0x0003);
       }
      dwDataMask   = 0x000000FF << dwNBitShifts;  //
      dwTemp =  (   pBuf->pdwData[i32ArrayIndex]  & (~dwDataMask) )
              | ( ((uint32_t)bSource << dwNBitShifts)&  dwDataMask   );
      pBuf->pdwData[i32ArrayIndex] = dwTemp;
      return 1;
    }

  return 0x0000;  // invalid target address ?!
} // end PicBuf_GetBufferByte()


/***************************************************************************/
uint16_t PicBuf_GetMemoryFlags(long i32TargetAddress )
{
 long i32ArrayIndex;
 T_PicBufferInfo *pBuf = PicBuf_TargetAddressToBufPtr( i32TargetAddress, &i32ArrayIndex );

  if( pBuf )
    { // Ok, buffer for the target address has been found, and array index is valid
      return pBuf->pwFlags[i32ArrayIndex];
    }
  return 0x0000;
} // end PicBuf_GetMemoryFlags()


/***************************************************************************/
void PicBuf_SetMemoryFlags(long i32TargetAddress, uint16_t wFlags )
{
 long i32ArrayIndex;
 T_PicBufferInfo *pBuf = PicBuf_TargetAddressToBufPtr( i32TargetAddress, &i32ArrayIndex );

  if( pBuf && (uint32_t)i32ArrayIndex<pBuf->dwMaxSize )
    { // Ok, buffer for the target address has been found, and array index is valid
      pBuf->pwFlags[i32ArrayIndex] = wFlags;
      if( wFlags & PIC_BUFFER_FLAG_USED )
       { if( pBuf->i32LastUsedArrayIndex < i32ArrayIndex )
             pBuf->i32LastUsedArrayIndex = i32ArrayIndex;
         // ODD addresses are something special if pBuf->dwAddressFactor=2 .
         // See the note on 'CONFIGURATION MEMORY in PIC18F' in PicBuf_GetBufferWord().
         if( (i32TargetAddress & 1) && (pBuf->dwAddressFactor>=2) )
          { if( pBuf->i32LastUsedArrayIndex <= i32ArrayIndex )
                pBuf->i32LastUsedArrayIndex =  i32ArrayIndex+1;
          }
       }
    }

} // end PicBuf_SetMemoryFlags()


/***************************************************************************/
void PicBuf_SetConfigWord(int iCfgWordIndex,
                         uint16_t wCfgWordValue)
 // Only exists to maintain compatibility with "older" prog'ing routines !
 // Caution: iCfgWordIndex counts from ZERO for "first config word"
 //          (which is usually at address 0x2007 in a 14-bit-core PIC)
{
  if( PIC_DeviceInfo.lConfWordAdr < 0 )
      return;  // Note: dsPIC30Fxxxx has no "classic config word" but a lot of "config registers".

  PicBuf_SetBufferWord(PIC_DeviceInfo.lConfWordAdr
          + iCfgWordIndex * PicBuf[PIC_BUF_CONFIG].dwAddressFactor,
          wCfgWordValue);
} // end PicBuf_SetConfigWord()



/***************************************************************************/
uint16_t PicBuf_GetConfigWord(int iCfgWordIndex )
 // Only exists to maintain compatibility with "older" prog'ing routines !
 // Caution: iCfgWordIndex is ZERO for "the first config word"
 //       (which is usually at address 0x2007 in a 14-bit-core PIC).
 //        and ONE for the "second config word" (@ 0x2008 for 14-bit core).
 // In a dsPIC, the config words are at 0xF80000, 0xF80002, etc (16 bit each).
 // In a PIC18F, the config words are at 0x300000, 0x300001, etc (8 bits each).
 //              WinPic displays them as 16-bit quantities !
{
  uint32_t dwDataFromBuffer;
  if( PicBuf_GetBufferWord(PIC_DeviceInfo.lConfWordAdr
          + iCfgWordIndex * PicBuf[PIC_BUF_CONFIG].dwAddressFactor,
                                &dwDataFromBuffer ) > 0 )
         return (uint16_t)dwDataFromBuffer;
   else  return 0x3FFF;  // best bet if there is no config word !
} // end PicBuf_GetConfigWord()



long PicHex_i32CountIgnoredLocations;
long PicHex_i32ErrorInLine;
long PicHex_i32FirstIgnoredAddress, PicHex_i32LastIgnoredAddress;

/***************************************************************************/
extern "C" void PIC_HEX_EnterByteInBuffer(   // callback for 'load file'..
                  uint32_t dwHexSourceLine,     // line number in source file (for error messages)
                  uint32_t dwLinearByteAddress, uint8_t bData)
  // PIC-specific callback function for loading HEX-files .
  // dwLinearByteAddress is the address found in the HEX file, *NOT* the target
  //                   address (see comments inside PIC_HEX_EnterByteInBuffer).
  // Caller:  LoadHexFile(), via pointer .
{
  uint32_t dwTargetAddress= dwLinearByteAddress / 2; // only true for MOST PIC devices
  long  i32ArrayIndex;
  uint32_t dwNBitShifts; // number of bit shifts =  8 * "address modulo X" (X depends on "bits per instruction" )
  uint32_t dwDataMask;
  uint32_t dwDataShifted;
  int   iTargetAddrStepsPerByteInEEPROM = 1; // default for PIC18F, but not for some others !
  int   iTargetAddrStepsPerConfigRegister = 1; // 1 for PIC16, 2 for PIC18F + dsPIC30F
  int   ignored = 1;

  // Where to put these data ? How to interpret the goddamned addresses ?
  //  Why is the address in the hex file not the "target" address ?
  // CAUTION, MANY PITFALLS !  Microchip decided to play a bizarre game with the
  //    addresses, because for most PIC's the target address steps by one
  //    for every 14-bit location. But for 16-bit core it steps by two.
  //    And for 24-bit core (dsPIC) it steps by two, too !
  //  The addresses in an "EIGHT-BIT" Intel Hex file must be divided by two
  //   in many cases (even where this is unnecessary, like for the 8-bit
  //   DATA EEPROM).
  //   This principle leads to some strange interpretation for the
  //   CODE MEMORY addresses in a dsPIC, as mentioned in the Microchip forum,
  //   locally saved as
  //        C:\datasheets\pic\dsPIC_HEX_File_misinterpretations.txt :
  //
  // > Hex files for the dsPIC can be hard to understand. The file format is
  // > byte-oriented, where each byte has a valid address. But program memory
  // > on the dsPIC is word-oriented, where each word is 24 bits long. The
  // > address counter in program memory increments by 2 for each word (3
  // > bytes). Therefore each byte in program memory does not have an
  // > individual address. Hmmm...
  // >
  // > For this reason, any representation of dsPIC program memory in hex files
  // > is going to be strange. The designers at Microchip decided to introduce
  // > a so-called "phantom" byte so that each 24 bit word can be represented
  // > as a 32 bit quantity. Each byte (including the phantom) has an address
  // > in the file, so there is a 2-to-1 relationship between hex file
  // > addresses and program memory addresses. For example, address 0x200 in
  // > the hex file corresponds to 0x100 in program memory. Its not pretty, but
  // > it works.
  // Similar crude tricks seem to apply to the EEPROM DATA MEMORY too !
  //  For example, PIC16Fxxx : The EEPROM DATA should reside at 0x2100 + x,
  //  but in fact, the addresses parsed from the HEX file are   0x4200 + 2x
  switch( PIC_DeviceInfo.iBitsPerInstruction )
   { case 24:   // every "instruction" (in CODE MEMORY + EXECUTIVE MEMORY)
                // occupies 24 BIT,  but 32 BIT in the hex file,
                // as quoted above for dsPICs .
                dwTargetAddress= dwLinearByteAddress / 2;  // assumed "target address" due to Microchip's crude "trick"..
                i32ArrayIndex = dwLinearByteAddress / 4;  // div by 4, not by 2 !
                dwNBitShifts = 8 * (dwLinearByteAddress & 0x0003L); // 0, 8, 16, or 24(!) "left"-shifts
                dwDataShifted= (uint32_t)bData << dwNBitShifts;
                dwDataMask   = ~(0x000000FF << dwNBitShifts);
                iTargetAddrStepsPerByteInEEPROM = 2; // .. anyway SPECIAL CASE(!)
                iTargetAddrStepsPerConfigRegister = 2;
          break;
     case 16:   // 16 bit per "core instruction" (in CODE MEMORY),
                // but no divide-by-two game for this PIC18F family  ?!
                // For DATA MEMORY, copy data BYTE-WISE, no word-fiddling
                iTargetAddrStepsPerByteInEEPROM  = 1; // '1' for PIC18F, '2' for others !
                iTargetAddrStepsPerConfigRegister= 2;
                dwTargetAddress = dwLinearByteAddress; // not as "funny" as a dsPIC ;-)
                if ( (long)dwLinearByteAddress >= PIC_DeviceInfo.lDataMemBase
                  && (long)dwLinearByteAddress < (PIC_DeviceInfo.lDataMemBase+PIC_DeviceInfo.lDataEEPROMSizeInByte) )
                 {
                   dwNBitShifts = 0;   // .. because WinPic treats DATA MEMORY as BYTES !
                   dwDataShifted= bData;
                   dwDataMask   = 0x000000FF;
                 }
                else // for all other locations (including CONFIG REGS ? ? ),
                 {   // pack the data from the INHX8M file into WORDs...
                   dwNBitShifts = 8 * (dwLinearByteAddress & 0x0001L); // 0 or 8 "left"-shifts
                   dwDataShifted= (uint32_t)bData << dwNBitShifts;
                   dwDataMask   = ~(0x000000FF << dwNBitShifts);
                 }
          break;
     case 14:   // 14 bit per "core instruction", like PIC12F, PIC16,..
                dwNBitShifts = 8 * (dwLinearByteAddress & 0x0001L); // 0 or 8 "left"-shifts
                dwDataShifted= (uint32_t)bData << dwNBitShifts;
                dwDataMask   = ~(0x000000FF << dwNBitShifts);
                dwTargetAddress = dwLinearByteAddress / 2;
                iTargetAddrStepsPerByteInEEPROM  = 1 ; // ..but beware of "phantoms", see "db0ubi" example
                iTargetAddrStepsPerConfigRegister= 1;
          break;
     case 12:   // 12-bit core only tested with PIC10F206 (anno 2005) !
                dwNBitShifts = 8 * (dwLinearByteAddress & 0x0001L); // 0 or 8 "left"-shifts
                dwDataShifted= (uint32_t)bData << dwNBitShifts;
                dwDataMask   = ~(0x000000FF << dwNBitShifts);
                dwTargetAddress = dwLinearByteAddress / 2;
                iTargetAddrStepsPerByteInEEPROM  = 1 ; // there is no DATA EEPROM in PIC10F206 but who knows
                iTargetAddrStepsPerConfigRegister= 1;
                // Some more ugly "special situations", this time for the
                // PIC10F20x family . Quoted from DS41228D :
                // > By convention the Configuration Word register is stored
                // > at the logical address 0x0FF within the hex file
                // > generated for the PIC10F200/202/204/206.
                // > This logical (UNLOGICAL!) address may not (WILL NEVER!)
                // > reflect the actual physical address for the part itself.
                // > It is the responsibility of blablabla to retrieve the
                // > Configuration Word Register (..) and translate the adddress
                // > to the proper physical location when programming.
                // The problem is, the stupid "0xFFF" as address for the config word
                // is even contained in the *.DEV file .
                // In WinPic, the stupid "translation" is hard-coded in PIC10F_PRG.C !
          break;
     default:   // most likely NOT a PIC, so...
                dwNBitShifts = 0;
                dwDataShifted= (uint32_t)bData;
                dwDataMask   = 0x00000000;
                dwTargetAddress = dwLinearByteAddress;
          break;
   } // end switch( PIC_DeviceInfo.iBitsPerInstruction )

  if ( (long)dwTargetAddress < (PIC_DeviceInfo.lCodeMemSize/*instr.words*/
                               * (long)PicBuf[PIC_BUF_CODE].dwAddressFactor ) )
   {  // For CODE MEMORY BUFFER : beware of "BYTE ADDRESS" vs "TARGET ADDRESS" !
      //  For PIC12Fxxx, PIC16Fxxx, the (target-) address steps BY ONE
      //     though every location occupies more than one byte - STRANGE !
      // For dsPIC30F (24 bit core) with N "instruction words" code memory,
      //     the max. valid "target address" is 2 * (N-1) ,
      //     because the "address" steps by TWO for every 24-bit location !
      if( PIC_DeviceInfo.iBitsPerInstruction == 24 )
           i32ArrayIndex = dwLinearByteAddress / 4;
      else i32ArrayIndex = dwLinearByteAddress / 2;
     if( i32ArrayIndex>=0 && i32ArrayIndex<(long)PicBuf[PIC_BUF_CODE].dwMaxSize )
      { ignored = 0;
        PicBuf[PIC_BUF_CODE].pdwData[i32ArrayIndex] =
           ( PicBuf[PIC_BUF_CODE].pdwData[i32ArrayIndex] & dwDataMask ) | dwDataShifted;
        PicBuf[PIC_BUF_CODE].pwFlags[i32ArrayIndex] = PIC_BUFFER_FLAG_USED;  // mark location as "used"
        if ( PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex < i32ArrayIndex )
             PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex = i32ArrayIndex;
      }
   }
  else if ( (long)dwTargetAddress >= PIC_DeviceInfo.lDataMemBase
         && (long)dwTargetAddress < (PIC_DeviceInfo.lDataMemBase
              + PIC_DeviceInfo.lDataEEPROMSizeInByte * iTargetAddrStepsPerByteInEEPROM ) )
   { // Put data from hex file in the DATA(~EEPROM) buffer .
     // Beware of totally different address interpretation for different devices !
     // Examples for DATA MEMORY for various PICs families (14, 16, and 24-bit "core") :
     //  :104200002000640062003000750062006900200038     (" db0ubi ", for PIC16F628)
     //  :1000000054657374204461746100010203040500A7     ("Test Data" for PIC18F252)
     //  :10f80000647300005049000043330000304600009c     ("dsPIC30F " for dsPIC30F2010)
     //  :LLAAAATTDDddDDddDDddDDddDDddDDddDDddDDddDD , where :
     //   LL = Record Length,
     //   AAAA = Address (PIC16F628: 0x4200 instead of 0x2100),
     //   TT = Type (here 00=data record),
     //   DD = true data at even addresses, commonly treated as "LOWER BYTE"
     //   dd = dummy data at odd addresses, would be "HIGHER BYTE" if it existed .
     // 2005-02-23: To be prepared for future surprises, dd (the "dummy data")
     //   are placed in bits 15..8 of the uint32_t array.
     //   Who knows what else Microchip may come up with... 16-bit-wide EEPROM ?
     //
     // 2005-02-28: Tried to load different hex files from subdirectory \test .
     //   - the 16F628 DATA MEMORY sample contains PHANTOM BYTES
     //      (see " db0ubi "-example, only every 2nd byte location is used) .
     //      Cure: pack all ODD bytes into bit 15..8 of every location !
     //   - the 18F252 DATA MEMORY sample has NO GAPS in "Test Data",
     //     and there was no damned address-divided-by-two game ?!
     //     ( dwLinearByteAddress, decoded from the hex file, was in fact
     //       0xF000000 when loading EEPROM from 'PIC18F252test.hex' ) .
     //   - the dsPIC30F DATA MEMORY contain strange "phantom WORDS":
     //        in assembly sourcefile  : .byte  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77
     //        in MPLAB's EEPROM memory: 1100 3322 5544 7766 (!)
     //        in HEX file             : :10f80000001100002233000044550000667700001c
     //        from HEX file, readable : 00 11 00 00 22 33 00 00 44 55 00 00 66 77 00 00
     //        (these PHANTOM WORDS are related with the ugly address trick...)
     //  'iTargetAddrStepsPerByteInEEPROM' now caters for this annoying 'special case'.
     i32ArrayIndex = (dwTargetAddress - PIC_DeviceInfo.lDataMemBase) / iTargetAddrStepsPerByteInEEPROM;
     if( i32ArrayIndex>=0 && i32ArrayIndex<(long)PicBuf[PIC_BUF_DATA].dwMaxSize )
         ignored = 0;

     if( PIC_DeviceInfo.iBitsPerInstruction == 24 )
      { // due to Microchip's "address trick", the dsPIC is a special case:
        // Half of the bytes from the hex file contain "dummies",
        //  therefore  iTargetAddrStepsPerByteInEEPROM := 2 for dsPIC30F .
        // But, the first TWO bytes contain valid data, the next TWO bytes don't, etc.
        i32ArrayIndex = dwLinearByteAddress/2 - PIC_DeviceInfo.lDataMemBase;
        switch( dwLinearByteAddress & 0x0003 )
             { case 0: // byte [0] = valid data, first byte, address is ok
                    break;
               case 1: // byte [1] = valid data, second byte in a word, increment address
                     ++i32ArrayIndex;
                     break;
               default: // byte [2,3] = rubbish, discard, but don't consider as "ignored"
                     i32ArrayIndex = -1;
                     break;
             }
      } // end if( PIC_DeviceInfo.iBitsPerInstruction == 24 )
     else if( PIC_DeviceInfo.iBitsPerInstruction <= 14 )
      { // For 14- or 12-bit core: get rid of the phantom bytes in DATA area of hex-file
        //  (ignore every 2nd byte, such a rubbish.. )
        if( dwLinearByteAddress & 0x0001 )
           i32ArrayIndex = -1; // ignore every 2nd "DATA"-byte for PIC16Fxxx !
      }
     if( i32ArrayIndex>=0 && i32ArrayIndex<(long)PicBuf[PIC_BUF_DATA].dwMaxSize )
      { PicBuf[PIC_BUF_DATA].pdwData[i32ArrayIndex] = bData;
        PicBuf[PIC_BUF_DATA].pwFlags[i32ArrayIndex] = PIC_BUFFER_FLAG_USED;
        if ( PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex < i32ArrayIndex )
             PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex = i32ArrayIndex;
      }
   }
  else if ( (long)dwTargetAddress >= PIC_DeviceInfo.lConfMemBase
         && (long)dwTargetAddress < (PIC_DeviceInfo.lConfMemBase+2*PIC_BUF_CONFIG_SIZE) )
   { // Put data from hex file in the CONFIG MEMORY buffer :
     //     destination buffer contains one 16-bit-WORD per array index.
     // (for dsPIC :  the same address trick here as explained for DATA memory)
     dwNBitShifts  = 8 * (dwLinearByteAddress & 0x0001L); // 0 for EVEN, 8 for ODD address (with "odd" content :)
     i32ArrayIndex = ( dwTargetAddress - PIC_DeviceInfo.lConfMemBase ) / iTargetAddrStepsPerConfigRegister;
     if( PIC_DeviceInfo.iBitsPerInstruction == 24 )
      { // Same crude address trick as for DATA memory , requiring special treatment:
        // The first TWO bytes contain valid data, the next TWO bytes don't, etc.
        switch( dwLinearByteAddress & 0x0003 )
         { case 0: // byte [0] = valid data, first byte, address is ok
           case 1: // byte [1] = valid data, second byte, increment address
                   break;
           case 2: // byte [2,3] = rubbish, discard, but consider as "handled" !
           case 3: i32ArrayIndex = -1;
                   ignored = 0;
                   break;
         }
      }
     if( i32ArrayIndex>=0 && i32ArrayIndex < (long)PicBuf[PIC_BUF_CONFIG].dwMaxSize )
      { ignored = 0;
        PicBuf[PIC_BUF_CONFIG].pdwData[i32ArrayIndex]= (PicBuf[PIC_BUF_CONFIG].pdwData[i32ArrayIndex]
                & (~(0x000000FFL << dwNBitShifts)) )
                 | ((uint32_t)bData << dwNBitShifts);
        PicBuf[PIC_BUF_CONFIG].pwFlags[i32ArrayIndex] = PIC_BUFFER_FLAG_USED;
        if ( PicBuf[PIC_BUF_CONFIG].i32LastUsedArrayIndex < i32ArrayIndex )
             PicBuf[PIC_BUF_CONFIG].i32LastUsedArrayIndex = i32ArrayIndex;
      }
   }
  else if ( (long)dwTargetAddress >= PIC_DeviceInfo.lIdMemBase
         && (long)dwTargetAddress < (PIC_DeviceInfo.lIdMemBase+2*PIC_BUF_ID_MEM_SIZE) )
   { // Only for certain devices: Put data from hex file in the "ID"-MEMORY buffer :
     //                 destination buffer contains one 16-bit-WORD per array index.
     // Note: for some chips the ID MEMORY is part of the CONFIG MEMORY, for others not . . .
     dwNBitShifts  = 8 * (dwLinearByteAddress & 0x0001L); // 0 for EVEN, 8 for ODD address (with "odd" content :)
     i32ArrayIndex = ( dwTargetAddress - PIC_DeviceInfo.lIdMemBase ) / iTargetAddrStepsPerConfigRegister/*here too*/;
     if( PIC_DeviceInfo.iBitsPerInstruction == 24 )
      { // Same crude address trick as for DATA memory , requiring special treatment:
        // The first TWO bytes contain valid data, the next TWO bytes don't, etc.
        switch( dwLinearByteAddress & 0x0003 )
         { case 0: // byte [0] = valid data, first byte, address is ok
           case 1: // byte [1] = valid data, second byte, increment address
                   break;
           case 2: // byte [2,3] = rubbish, discard, but consider as "handled" !
           case 3: i32ArrayIndex = -1;
                   ignored = 0;
                   break;
         }
      }
     if( i32ArrayIndex>=0 && i32ArrayIndex < (long)PicBuf[PIC_BUF_ID_LOCATIONS].dwMaxSize )
      { ignored = 0;
        PicBuf[PIC_BUF_ID_LOCATIONS].pdwData[i32ArrayIndex]= (PicBuf[PIC_BUF_ID_LOCATIONS].pdwData[i32ArrayIndex]
                & (~(0x000000FFL << dwNBitShifts)) )
                 | ((uint32_t)bData << dwNBitShifts);
        PicBuf[PIC_BUF_ID_LOCATIONS].pwFlags[i32ArrayIndex] = PIC_BUFFER_FLAG_USED;
        if ( PicBuf[PIC_BUF_ID_LOCATIONS].i32LastUsedArrayIndex < i32ArrayIndex )
             PicBuf[PIC_BUF_ID_LOCATIONS].i32LastUsedArrayIndex = i32ArrayIndex;
      }
   }
  else if ( (long)dwTargetAddress>=0x800000L && (long)dwTargetAddress<(0x800000L+PIC_BUF_EXEC_SIZE) )
   { // this MAY belong to the dsPIC's "executive code memory" ...
     i32ArrayIndex = (dwTargetAddress - 0x800000L) / 2;
     if( i32ArrayIndex>=0 && i32ArrayIndex<(long)PicBuf[PIC_BUF_EXEC].dwMaxSize )
      { ignored = 0;
        PicBuf[PIC_BUF_EXEC].pdwData[i32ArrayIndex] =
           ( PicBuf[PIC_BUF_EXEC].pdwData[i32ArrayIndex] & dwDataMask ) | dwDataShifted;
        PicBuf[PIC_BUF_EXEC].pwFlags[i32ArrayIndex] = PIC_BUFFER_FLAG_USED;  // mark location as "used"
        if ( PicBuf[PIC_BUF_EXEC].i32LastUsedArrayIndex < i32ArrayIndex )
             PicBuf[PIC_BUF_EXEC].i32LastUsedArrayIndex = i32ArrayIndex;
      }
   }

 if( ignored )
  {
    // 2006-03-26 : got here with PIC16F628-file containing dwTargetAddress=0x0F9C (?!)
    ++PicHex_i32CountIgnoredLocations;
    PicHex_i32ErrorInLine = dwHexSourceLine;
    if((PicHex_i32FirstIgnoredAddress == -1) || ( dwTargetAddress<(uint32_t)PicHex_i32FirstIgnoredAddress) )
        PicHex_i32FirstIgnoredAddress = dwTargetAddress;
    if((PicHex_i32LastIgnoredAddress == -1) || ( dwTargetAddress>(uint32_t)PicHex_i32LastIgnoredAddress) )
        PicHex_i32LastIgnoredAddress = dwTargetAddress;

  }

} // end PIC_HEX_LoadCallback()

/***************************************************************************/
int PIC_HEX_LoadFile(const wxChar *fname )  // Loads a Microchip hex file
{
 wxChar szErrorMsg[256];
 int  i32ParserErrorInLine;
  PicHex_i32ErrorInLine = 0;
  PicHex_i32FirstIgnoredAddress = -1;
  PicHex_i32LastIgnoredAddress = -1;
  PicHex_i32CountIgnoredLocations = 0;
  szErrorMsg[0]=0;
  i32ParserErrorInLine = LoadHexFile( fname,
                  (T_HexLoadCallback)PIC_HEX_EnterByteInBuffer,
                   szErrorMsg );
  if( PicHex_i32CountIgnoredLocations>0 )
   {
    _stprintf(szErrorMsg, _("LoadHex: Ignored %ld location%c at addr 0x%06lX...0x%06lX ."),
             (long)PicHex_i32CountIgnoredLocations,
             (char)(PicHex_i32CountIgnoredLocations==1)?' ':'s',
             (long)PicHex_i32FirstIgnoredAddress,
             (long)PicHex_i32LastIgnoredAddress );
    APPL_ShowMsg( APPL_CALLER_PIC_HEX, 127/*important error*/, szErrorMsg);

   }
  else
   { if( i32ParserErrorInLine > 0 ) // here, counting lines from ONE to N !
      {  APPL_ShowMsg( APPL_CALLER_PIC_HEX, 127/*important error*/, szErrorMsg );
         PicHex_i32ErrorInLine = i32ParserErrorInLine;
      }
     else // no ERROR, but there may be funny stuff in the HEX file
      {   //  (for example, placed there by the SCC-compiler) ...
        if( szErrorMsg[0]>32 )
         { APPL_ShowMsg( APPL_CALLER_PIC_HEX,0, szErrorMsg );
         }
      }
   }
  return PicHex_i32ErrorInLine;
} // end ..LoadHexFile()


static void PIC_HEX_DumpByteToFile(FILE *fp, unsigned b)
{
  b &= 0x00FF;
  fprintf(fp,"%02X",b);
  PIC_HEX_check += b;
}

static void PIC_HEX_DumpAddrToFile(FILE *fp, unsigned w)
{
  PIC_HEX_DumpByteToFile(fp, (w>>8) & 0x00FF);  // address bits 15..8 first,
  PIC_HEX_DumpByteToFile(fp, w & 0x00FF);       // address bits  7..0 next
}

/* d_hexrec(fp, buf, address, nw)
 *
 * Dumps nw words from buf in INHX8M format.
 *
 */

static void PIC_HEX_DumpHexRecords(FILE *fp, uint32_t *buf, int address, int nw)
{
 int i, n;
 uint16_t w;

  while ( nw > 0 )
   {
     // Format of INHX8M "DATA RECORD" :
     //  :LLAAAATTDDddDDddDDddDDddDDddDDddDDddDDddDD , where :
     //   LL = Record Length,
     //   AAAA = Address (PIC16F628: 0x4200 instead of 0x2100),
     //   TT = Type (here 00=data record),
     //   DD = true data at even addresses, commonly treated as "LOWER BYTE"
     //   dd = dummy data at odd addresses, would be "HIGHER BYTE" if it existed .
    PIC_HEX_check = 0;
    fprintf(fp,":");
    n = (nw > PIC_HEX_WORDS_PER_HEX_DUMP)? PIC_HEX_WORDS_PER_HEX_DUMP: nw;
    PIC_HEX_DumpByteToFile(fp,2*n);          // LL
    PIC_HEX_DumpAddrToFile(fp,2*address);    // AAAA
    PIC_HEX_DumpByteToFile(fp,0);            // TT
    for ( i=0; i < n; ++i )
      {
        w = *buf++;
        // swap byte order ?! NONSENSE !
        //  Low-byte first, for EVEN addresses,
        //  followed by high-byte, for ODD addresses :
        PIC_HEX_DumpByteToFile(fp, w & 0x00FF);
        PIC_HEX_DumpByteToFile(fp, w >> 8);

      }
    PIC_HEX_DumpByteToFile(fp,(-PIC_HEX_check)&0xFF);
    fprintf(fp,"\n");
    nw      -= PIC_HEX_WORDS_PER_HEX_DUMP;
    address += PIC_HEX_WORDS_PER_HEX_DUMP;
   } // end while
} // end ..DumpHexRecords()


/***************************************************************************/
int PIC_HEX_DumpHexFile(const wxChar *fname )
  // Dumps buffers in Microchip HEX format.
{
 FILE *fp;
// int  iBufNr;// TODO (death#1#): Could we change this with wxFILE thing? because of unicode on Linux
  if ( (fp = fopen(wxString(fname).ToAscii(), "w")) == NULL )
   {
       return -1;
   }

  // dump code memory, data memory, configuration memory,
  //   and possibly the EXECUTIVE MEMORY (if there is something in it)
  for(int iBufNr=0; iBufNr<PIC_BUFFERS; ++iBufNr )
   {
    if( PicBuf[iBufNr].i32LastUsedArrayIndex >= 0 )
      {
          PIC_HEX_DumpHexRecords(fp,
            PicBuf[iBufNr].pdwData,
            PicBuf[iBufNr].dwAddressOffset,
            PicBuf[iBufNr].i32LastUsedArrayIndex+1 );
      }
   }

  fprintf(fp,":00000001FF\n");
//  fclose(fp);
  return 1;
} // end ..DumpHexFile()


/* EOF <pic_hex.cpp> */


