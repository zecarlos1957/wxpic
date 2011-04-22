/*-------------------------------------------------------------------------*/
/*! \file PIC_PRG.cpp                                                      */
/*                                                                         */
/*  \brief  PIC Programming algorithms for 12Fxxx and 16Fxxx               */
/*  \author Wolfgang Buescher (DL4YHF)                                     */
/*  \date   2005-08-21                                                     */
/*  \bug    Crashes a lot and requires huge amounts of memory.             */
/*  \warning Ugly structured, needs to be turned into clean C++.           */
/*                                                                         */
/*! Purpose: Serial PIC Programming algorithms for 12Fxxx and 16Fxxx .     */
/*!           ( PIC10Fxxx, PIC18Fxxxx, dsPIC30Fxxxx in other modules ! )   */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*
 * A few old parts based on "Pp.c" Copyright (C) 1994-1998 David Tait :
 *   > All rights reserved. Permission is granted to use, modify,
 *   > or redistribute this software so long as it is not sold
 *   > or exploited for profit.
 *
 * Adaption for Windows(tm) & C++Builder, Module "PIC_PRG.cpp" :
 *   Copyright(C) 2000-2004  by Wolfgang Buescher.
 *   Lots of modifications (almost none of David's code left).
 *   All rights reserved.
 *
 * Permission is granted to use, modify, or redistribute this software
 * so long as it is not sold or exploited for profit.
 *
 *
 * THIS SOFTWARE IS PROVIDED AS IS AND WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED.
 *
 * BY MICROCHIP'S QUALITY REQUIREMENTS, THIS IS **NOT**
 *   A PRODUCTION-GRADE PROGRAMMER !!!
 *
 * Revision history:
 *
 * ??-Feb-1994: V-0.0; started as a few routines to debug hardware.
 * 07-Mar-1994: V-0.1; first code to successfully program a 16C84.
 * 09-Mar-1994: V-0.2; fuse switches; 7407 support; H/W test.
 * 10-Mar-1994: V-0.3; LPT2, LPT3 and INHX8M support; cosmetic changes.
 * 30-Aug-1996: V-0.4; major re-write (10-Sep-96: added config warning).
 * 03-Apr-1998: V-0.5; 16F8x now default; added -osng switches.
 * 03-Apr-1998: V-0.5; identity and hardware header defined in config.h
 * 16-Sep-2000: V1.0,  Converted from Pp.c to PIC_PRG.cpp
 *                     for beta-release of DL4YHF's Windows-PIC-Programmer.
 *                     REMOVED ALL TAB CHARACTERS. Keep it this way.. thanks.
 * 04-Dec-2001: V1.1,  added support for PIC16F628 and PIC16C711 etc.
 * 2003-12-28:  V1.2,  Cured a bug caused by another LOUSY programming spec .
 *                     Search for label [lps1] in all sourcefiles
 * 2004-01-28:  V1.3,  Added support for 16F628A(!). Guess what..
 *                     They changed the programming specs again !
 * 2004-03-28:  V1.4,  Copied all string literals into TRANSLATION.C .
 *                     Looked for an algorithm suitable for the 16F630/676.
 *       (2004-03-30)  The lousy spec does not mention anything 'compatible'..
 *                     .. as always .. and instead of wasting hours comparing
 *                     the NEW PROGRAMMING SPECS FOR THE 16F630 (-baah-) with
 *       all other programming specs to find similarities / incompatibilities,
 *       WoBu decided to write three special algorithms following Microchips
 *       Specs from DS41191B !
 *    (Note: I now have dozens of different PIC programming specs
 *           on my harddisk, and they keep making new ones.
 *          The author is still waiting for the day when Microchip
 *          introduce a new chip and simply say "USE THE SAME PROGRAMMING
 *          ALGORITHM AS FOR THE PIC16Fxyz" -- which did not happen up to now.
 *          Instead they seem to be happily writing new programming specs
 *          over and over ! These folks should take some lessons at ATMEL..)
 *
 * 2005-02-18:  Tried to add support for a dsPIC30F201 after BUYING(!)
 *          a sample chip, since Microchip's sampling program didn't work.
 *          The algorithms are located in dsPIC_PRG.cpp, not here in PIC_PRG !
 *
 * 2005-04-XX:  Support for PIC18Fxxxx added by Martin van de Werff,
 *          located in a separate module.
 * 2005-05-01:  Began to add support for PIC10F20x ... totally different again
 * 2005-06-06:  Looked at the PIC16F7x programming specs because someone asked
 *          if it was possible to add support. Found out :
 *           - AGAIN a totally different method, this time TWO locations per write
 *           - some stupid person thought it was cool to write down binary numbers
 *             beginning with the LEAST(!) SIGNIFICANT BIT AT THE LEFT SIDE. Ouch !
 * 2005-06-06:  Looked at the PIC16F72 programming specs because the 16F72
 *             was not mentioned in the "PIC16F7X" programming specs. Found that..
 *           - They wrote another programming spec again (ONLY for the 16F72 - ARGH...),
 *             this time writing binary numbers (in the table of serial commands)
 *             with the MOST SIGNIFICANT BIT AT THE LEFT SIDE (like it should be).
 *             Is the PIC16F72 so much different from PIC16F73/74/76/77 ? ? ?
 *             At least the program flow charts looked the same, with
 *             two memory words at a time, so PIC_PRG_Program16F7X7() should
 *             theoretically work for PIC16F72 *AND* PIC16F73/74/76/77 .
 *             Conclusion: Author left everything as it was and went back
 *             to play with the ATmega48 with its BEAUTIFUL instruction set ;-)
 *
 * 2007-06-03:  A.Kibler added support for PIC12F609/615,16F610/616,
 *             which require the "End Programming" command .
 *             Modifications in PIC_PRG_Program16FXX() - thanks Andrew !
 */
/*  2009-06-04:  Ported to wxWidget by Philippe Chevrier                   */

#include <stdio.h>     // don't panic.. just required for sprintf !
#include <wx/intl.h>
#include <wx/utils.h>

//#pragma hdrstop  // don't try to precompile the following headers (->chaos!!)

#include "Config.h"    // permanently saved Config-structure
#include "Devices.h"   // device information structure
#include <Appl.h>       // APPL_ShowMsg(), etc
#include "PIC_HEX.h"   // HEX-file import, export  and buffers

#include "PIC_HW.h"    // Routines to drive the programmer hardware interface

#include "PIC16F716_PRG.h" // support for PIC16F716 since 2005-12 (what's different with that beast ?)
#include "PIC10F_PRG.h" // support for PIC10F20x by W.Buescher (2005-05)
#include "dsPIC_PRG.h"  // support dsPIC30 family by W.Buescher
#include "PIC18F_PRG.h" // support for PIC18F devices by Martin van der Werff

#define _I_AM_PIC_PRG_
#include "PIC_PRG.h"  // include file for THIS module with single-source VARs

/*----------- INTERNAL definitions -----------------------------------------*/

// PIC run modes
#define  PIC_RM_STOP 0
#define  PIC_RM_GO   1


/*----------- Tables -------------------------------------------------------*/




/*----------- Variables ----------------------------------------------------*/

// Current states for the three (?) memory types while "programming all"...
int PicPrg_iCodeMemErased   = 0;  // 0=no, 1=yes ...
int PicPrg_iCodeMemVerified = 0;
int PicPrg_iDataMemErased   = 0;
int PicPrg_iDataMemVerified = 0;
int PicPrg_iConfMemErased   = 0;
int PicPrg_iConfMemProgrammed=0;
int PicPrg_iConfMemVerified = 0;
int PicPrg_iConfWordProgrammed=0; // 1 = "classic" config word with readout protection has been set
int PicPrg_iConfWordVerified=0;

int PIC_run_mode = PIC_RM_STOP; /* let PIC run PIC_run_mode = PIC_RM_GO   */

int PIC_PRG_iSimulateOnly = 0;  // don't simulate, use "the real thing"

wxChar PIC_error_string[256];




/*------------- Implementation --------------------------------------------*/


/***************************************************************************/
/** Initializes all variables of the module.
 * Must be called ONCE(!) before any other routine of this module.
 */
void PIC_PRG_Init(void)
{
// ex: if(Config.pic_device_type == PIC_DEV_TYPE_UNKNOWN)
//        Config.pic_device_type =  PIC_DEV_TYPE_16F84;
//  Since December 2001, the "unknown" PIC type can be used to try
//  anything you like, max buffer sizes, but no Config word decoding.
//
    PIC_PRG_SetDeviceType( NULL );
    PIC_lBandgapCalibrationBits     = -1;    // < 0 means "unknown"
    PIC_lOscillatorCalibrationWord  = -1;    // < 0 means "unknown"
    PIC_iHaveErasedCalibration      = false; // may quit program without harm
    PIC_PRG_iBatchProgState = BATCH_PROG_OFF;

} // end ..Init()


/***************************************************************************/
/** Sets the PIC device type and initializes all type-dependent variables.
 * May also load some "additional info files" if necessary.
 */
bool PIC_PRG_SetDeviceType(T_PicDeviceInfo *pDeviceInfo )
{
    bool fResult = false;
    const char *cp;
    int  iSuffix;
    uint32_t dwFamily, dwWriteBufSize_bytes, dwEraseBufSize_bytes; // for PIC18Fxxx(x)

// Emergency brake for the code memory size...
    if (TSessionConfig::GetUnknownCodeMemorySize() > PIC_BUF_CODE_SIZE)
        TSessionConfig::SetUnknownCodeMemorySize(PIC_BUF_CODE_SIZE);

    if (pDeviceInfo)
        PIC_DeviceInfo = *pDeviceInfo;
    else
        PicDev_FillDefaultDeviceInfo( &PIC_DeviceInfo );

// AFTER the basic PIC-device-info is set valid, try to fill the config-bit info..
    fResult = PicDev_FillConfigBitInfoTable( &PIC_DeviceInfo ); // -> PicDev_ConfigBitInfo[]


    if (   PIC_DeviceInfo.iBitsPerInstruction!=12
            && PIC_DeviceInfo.iBitsPerInstruction!=14
            && PIC_DeviceInfo.iBitsPerInstruction!=16
            && PIC_DeviceInfo.iBitsPerInstruction!=24 )
    {
        // there were bugs in some old device definition tables, try to fix them here:
        PIC_DeviceInfo.iBitsPerInstruction = 14;   // default for PIC12Fxxx and PIC16Fxxx
        if ( PIC_DeviceInfo.wCodeProgAlgo==PIC_ALGO_PIC10F )
            PIC_DeviceInfo.iBitsPerInstruction=12; // PIC10F   -> 12 bit per "instruction word"
        if ( PIC_DeviceInfo.wCodeProgAlgo==PIC_ALGO_PIC18F )
            PIC_DeviceInfo.iBitsPerInstruction=16; // PIC18F   -> 16 bit per "instruction word"
        if ( PIC_DeviceInfo.wCodeProgAlgo==PIC_ALGO_dsPIC30F )
            PIC_DeviceInfo.iBitsPerInstruction=24; // dsPIC30F -> 24 bit per "instruction word"
    } // end if < suspicious "bits per instruction word" >


    if ( strcmp(PIC_DeviceInfo.sz40DeviceName, "PIC??????")==0 )
    {
        // replace some device parameters if the device type is unknown :
        PIC_DeviceInfo.lCodeMemSize  = TSessionConfig::GetUnknownCodeMemorySize();
        PIC_DeviceInfo.lDataEEPROMSizeInByte = TSessionConfig::GetUnknownDataMemorySize();
        if (TSessionConfig::GetHasFlashMemory())
            PIC_DeviceInfo.iCodeMemType=PIC_MT_FLASH;   // 12Fxxx, 16Fxxx -> 14 bit per word
        else   PIC_DeviceInfo.iCodeMemType=PIC_MT_EPROM;
    }

// Set some "higher level", device-dependent parameters ..
    PIC_DeviceInfo_iConfMemSize = PicDev_GetConfMemSize();
    if ( PIC_DeviceInfo.lCodeMemWriteLatchSize < 1 )
        PIC_DeviceInfo.lCodeMemWriteLatchSize = 1;
    PicBuf[PIC_BUF_CODE].iBitsPerElement = PIC_DeviceInfo.iBitsPerInstruction;
    PicBuf[PIC_BUF_DATA].dwAddressOffset = PIC_DeviceInfo.lDataMemBase;
    PicBuf[PIC_BUF_DATA].dwAddressFactor = 1;
    PicBuf[PIC_BUF_DATA].iBitsPerElement = 8; // display this buffer 8-bit-wise
    PicBuf[PIC_BUF_CONFIG].dwAddressOffset = PIC_DeviceInfo.lConfMemBase;
    PicBuf[PIC_BUF_CONFIG].dwAddressFactor = 2;  // assume 16-bit wide CONFIG memory.. (not for PIC16Fxxx !)
    PicBuf[PIC_BUF_CONFIG].iBitsPerElement = 16;
    PicBuf[PIC_BUF_EXEC].dwAddressOffset = 0x800000;
    PicBuf[PIC_BUF_EXEC].dwAddressFactor = 2;  // bizarre but true for dsPIC30F !
    PicBuf[PIC_BUF_EXEC].iBitsPerElement = PIC_DeviceInfo.iBitsPerInstruction;
    PicBuf[PIC_BUF_ID_LOCATIONS].dwAddressOffset = PIC_DeviceInfo.lIdMemBase;
    PicBuf[PIC_BUF_ID_LOCATIONS].dwAddressFactor = 2;  // assume 16-bit wide ID LOCATIONS
    PicBuf[PIC_BUF_ID_LOCATIONS].iBitsPerElement = 16;
// Why different buffers for "ID locations" and "Device ID" ? See PIC18F + dsPIC30F !
    PicBuf[PIC_BUF_DEVICE_ID].dwAddressOffset = PIC_DeviceInfo.lDeviceIdAddr;
    PicBuf[PIC_BUF_DEVICE_ID].dwAddressFactor = 2;  // assume 16-bit wide DEVICE ID word(s)
    PicBuf[PIC_BUF_DEVICE_ID].iBitsPerElement = 16;

    if ( PIC_DeviceInfo.iBitsPerInstruction>=24 )
    {
        // To imitate microchip's bizarre addressing scheme for dsPICs,
        // multiply the array index by two for the address offset, because:
        // Though a dsPIC instruction consists of THREE bytes,
        // the address steps by TWO for every instruction .
        // (stepping by THREE, or maybe FOUR would have been more logical)
        // As a result of Microchip's ugly adressing scheme,
        // NOT EVERY BYTE has a unique address !
        // This becomes terribly obvious in MPLAB's 'Program Memory' dump,
        // which shows 12(!)hex bytes per line, but 16(!!)ASCII characters,
        // while the address steps by 8(!!!) for every line.  Ummmmm .
        //  (Note: WinPic displays EIGHT INSTRUCTION LOCATIONS per line)
        // For the 18F family (with 16 bits per instruction), multiplying
        // the address by two is acceptable because every instruction
        // consists of two bytes .
        PicBuf[PIC_BUF_CODE].dwAddressOffset = 0;
        PicBuf[PIC_BUF_CODE].dwAddressFactor = 2;  // indeed, TWO address steps per "code word", though THREE bytes each !
        PicBuf[PIC_BUF_EXEC].dwAddressOffset = 0x800000;
        PicBuf[PIC_BUF_EXEC].dwAddressFactor = 2;  // ..bizarre but true for dsPIC30F !
    }
    else if ( PIC_DeviceInfo.iBitsPerInstruction>=16 )
    {
        // here, most likely for PIC18Fxxxx :
        PicBuf[PIC_BUF_CODE].dwAddressOffset = 0;
        PicBuf[PIC_BUF_CODE].dwAddressFactor = 2;   // here TWO address steps per code word, sounds reasonable for 16-bit :)
        // PicBuf[PIC_BUF_EXEC].dwAddressOffset = ?; // no such thing here ?
        // PicBuf[PIC_BUF_EXEC].dwAddressFactor = 2;
    }
    else // PIC_DeviceInfo.iBitsPerInstruction < 16 :
    {
        // here for PIC12Fxxx, PIC16Fxxx :
        PicBuf[PIC_BUF_CODE].dwAddressOffset = 0;
        PicBuf[PIC_BUF_CODE].dwAddressFactor = 1;   // here ONE address step per code word, ummmm
        PicBuf[PIC_BUF_DATA].dwAddressFactor = 1;    // 8-bit wide DATA memory, only pseudo-address range for HEX file (not mapped to CPU adress range)
        PicBuf[PIC_BUF_CONFIG].dwAddressFactor = 1;  // address increments BY ONE for every 14-bit "register" !
        PicBuf[PIC_BUF_ID_LOCATIONS].dwAddressFactor = 1;
        PicBuf[PIC_BUF_DEVICE_ID].dwAddressFactor = 1;
        // PicBuf[PIC_BUF_EXEC].dwAddressOffset = ?; // no such thing here too
        // PicBuf[PIC_BUF_EXEC].dwAddressFactor = 1;
    }
    PicDevInfo_i32ConfMemAddressFactor = PicBuf[PIC_BUF_CONFIG].dwAddressFactor; // "address steps per CONFIG REGISTER"


// Since 2005-05-19, Martin's PIC18F module must know the PIC18F DEVICE FAMILY.
//  Here, in WinPic, retrieve the "family ID" from the DEVICE NAME (!),
//  which may be something like "PIC18F2550" (used in Microchip's DEV-files,
//  but also in WinPic's own DEVICES.INI file ).
    if (  PIC_DeviceInfo.sz40DeviceName[3]=='1'
            && PIC_DeviceInfo.sz40DeviceName[4]=='8' )
    {
        // Looks like a PIC18-something .
        // Now find the first DIGIT of the "suffix"...
        cp = PIC_DeviceInfo.sz40DeviceName;
        cp += strlen(cp)-1;
        while ( cp>PIC_DeviceInfo.sz40DeviceName && cp[-1]>='0' && cp[-1]<='9' )
        {
            --cp;  // skip next DIGIT
        }
        iSuffix = atoi( cp );

        // As a default, try to retrieve the PIC18F "family" from the write buffer size
        // which has been a part of the PIC DEVICE INFO. Unfortunately this may be
        // not enough to tell -for example- the 458 family from the 4580 family .
        // Note: PIC_DeviceInfo.lCodeMemWriteLatchSize may be ZERO if "unknown" !
        if ( iSuffix<=999 )
        {
            // Includes PIC18F242, 252, 442, 452, 248, 258, 448, 458 .
            dwFamily = PIC18_FAMILY_458;
            dwWriteBufSize_bytes = 8;
            dwEraseBufSize_bytes = 64;
        }
        else switch (  PIC_DeviceInfo.lCodeMemWriteLatchSize/*BYTES*/ )
            {
            case 64:  // PIC18F2525, 2620, 4525, 4620, 2585, 2680, 4585, 4680 :
                dwFamily = PIC18_FAMILY_4680;
                dwWriteBufSize_bytes = 64;
                dwEraseBufSize_bytes = 64;
                break;
            case 32:  // PIC18F2420, 2520, 4420, 4520, 2455, 2550, 4455, 4550 :
                dwFamily = PIC18_FAMILY_4550;
                dwWriteBufSize_bytes = 32;
                dwEraseBufSize_bytes = 64;
                break;
            default:  // PIC18F2220, 2320, 4220, 4320, 2331, 2431, 4331, 4431, 2439, 2539, 4439, 4539
                // (?)   1220, 1320, 2480, 2580, 4480, 4580 .
                dwFamily = PIC18_FAMILY_4580;
                dwWriteBufSize_bytes = 8;
                dwEraseBufSize_bytes = 64;
                break;
            }
        // Additionally, for certains devices we are SURE about the family !
        // From PIC18F_PRG.cpp : The size of the write buffer differs between devices
        //
        //  8 bytes  PIC18F2410, PIC18F2510, PIC18F4410, PIC18F4510
        // (4 WORDS) PIC18F2420, PIC18F2520, PIC18F4420, PIC18F4520
        //           PIC18F2480, PIC18F2580, PIC18F4480, PIC18F4580 << family
        //
        // 32 bytes  PIC18F2455, PIC18F2550, PIC18F4455, PIC18F4550 << family
        //
        // 64 bytes   PIC18F2515, PIC18F2610, PIC18F4515, PIC18F4610
        // (32 WORDS) PIC18F2525, PIC18F2620, PIC18F4585, PIC18F4280
        //            PIC18F2585, PIC18F2680, PIC18F4585, PIC18F4680 << family
        if ( iSuffix==2410 || iSuffix==2510 || iSuffix==4410 || iSuffix==4510
                || iSuffix==2420 || iSuffix==2520 || iSuffix==4420 || iSuffix==4520
                || iSuffix==2480 || iSuffix==2580 || iSuffix==4480 || iSuffix==4580 )
        {
            dwFamily = PIC18_FAMILY_4580;
            dwWriteBufSize_bytes = 8;
            dwEraseBufSize_bytes = 64;
        }
        if ( iSuffix==2455 || iSuffix==2550 || iSuffix==4455 || iSuffix==4550 )
        {
            dwFamily = PIC18_FAMILY_4550;
            dwWriteBufSize_bytes = 32;
            dwEraseBufSize_bytes = 64;
        }
        if ( iSuffix==2515 || iSuffix==2610 || iSuffix==4515 || iSuffix==4610
                || iSuffix==2525 || iSuffix==2620 || iSuffix==4585 || iSuffix==4280
                || iSuffix==2585 || iSuffix==2680 || iSuffix==4585 || iSuffix==4680 )
        {
            dwFamily = PIC18_FAMILY_4680;
            dwWriteBufSize_bytes = 64;
            dwEraseBufSize_bytes = 64;
        }
        // If the buffer sizes (write+erase) specified in the device file are VALID,
        // use them regardless of the above..
        if ( PIC_DeviceInfo.lCodeMemWriteLatchSize/*BYTES*/ >= 8 )
            dwWriteBufSize_bytes = PIC_DeviceInfo.lCodeMemWriteLatchSize;
        if ( PIC_DeviceInfo.lCodeMemEraseLatchSize/*BYTES*/ >= 8 )
            dwEraseBufSize_bytes = PIC_DeviceInfo.lCodeMemEraseLatchSize;
        PIC18F_SetDeviceFamily( dwFamily, dwWriteBufSize_bytes/2/*->words*/, dwEraseBufSize_bytes/2 );
        // Since the command pattern for "Bulk Erase" was changed by Microchip,
        // the algorithm may need to know if "old" or "new" chip shall be erased.
        // See details in PIC18F_EraseChip_4550() .  Modified by WoBu 2007-01-25 .
        PIC18F_fUseOldBulkEraseCommands = ( PIC_DeviceInfo.wEraseAlgo == PIC_ALGO_PIC18F_OLD );
        switch ( dwFamily )
        {
        case PIC18_FAMILY_458 :
            cp="458";
            break;
        case PIC18_FAMILY_4580:
            cp="4580";
            break;
        case PIC18_FAMILY_4550:
            cp="4550";
            break;
        case PIC18_FAMILY_4680:
            cp="4680";
            break;
        default:
            cp="???";
            break;
        }
        APPL_ShowMsg( 0, _("PIC18F: family='%ld'=%s, WriteLatch=%ld bytes, EraseLatch=%ld bytes ."),
                      dwFamily, cp, dwWriteBufSize_bytes, dwEraseBufSize_bytes );
    } // end if < 4-digit "suffix" for PIC18 (?)

//    // Since 2008-05 : Update some info in the "Programmer Hardware Info" .
//    //  Some "smart" PIC-programmers (in the form of plugin-DLLs) may need this one day.
//    strncpy( PHWInfo.sz63ProgrammedDeviceName, PIC_DeviceInfo.sz40DeviceName, 63 );
//    PHWInfo.iCodeMemorySize_words = PIC_DeviceInfo.lCodeMemSize;
//    PHWInfo.iBitsPerCodeMemoryWord= PIC_DeviceInfo.iBitsPerInstruction;
//    PHWInfo.iDataEEPROMSize_bytes = PIC_DeviceInfo.lDataEEPROMSizeInByte;
//    PHWInfo.iCodeMemWriteLatchSize= PIC_DeviceInfo.lCodeMemWriteLatchSize;
//    PHWInfo.iCodeMemEraseLatchSize= PIC_DeviceInfo.lCodeMemEraseLatchSize;
//    PHWInfo.iConfMemBase          = PIC_DeviceInfo.lConfMemBase;
//    PHWInfo.iConfWordAdr          = PIC_DeviceInfo.lConfWordAdr;

    return fResult;

} // end ..SetDeviceType()

/***************************************************************************/
void PicPrg_SetVerifyResult( long i32DeviceBaseAddress, int iVerifyResultCode )
{
    if (i32DeviceBaseAddress==0)
        PicPrg_iCodeMemVerified = iVerifyResultCode;
    else if (i32DeviceBaseAddress==PIC_DeviceInfo.lDataMemBase)
        PicPrg_iDataMemVerified = iVerifyResultCode;
    else if (i32DeviceBaseAddress==PIC_DeviceInfo.lConfMemBase)
        PicPrg_iConfMemVerified = iVerifyResultCode;

} // end PicPrg_SetVerifyResult()

/***************************************************************************/
bool PicPrg_ReadWriteDummy( uint32_t dwDummyAddress, uint32_t *pdwDummyData, uint32_t dwNrOfDummyLocations)
{
    dwDummyAddress= dwDummyAddress;   // assign dummy to dummy, to make compiler happy
    pdwDummyData  = pdwDummyData;
    dwNrOfDummyLocations = dwNrOfDummyLocations;
    return false;
} //

/***************************************************************************/
/** Returns a pointer to a device-specific READER function.
 * dwTargetBaseAddress tells us the memory type (code/data/config/...),
 * PIC_DeviceInfo.iBitsPerInstruction the basic chip type.
 */
bool PicPrg_GetReadWriteFunctionPtr(
    uint32_t dwTargetBaseAddress, //!< in: target address within PIC
    T_PicPrg_ReadWriteFunc ** ppvReader,  //!<out: address of READER function
    T_PicPrg_ReadWriteFunc ** ppvWriter ) //!<out: address of WRITER function
{
    bool fResult = true;

    // Set dummy vectors as default, if no special reader or writer is implemented:
    if (ppvReader) *ppvReader  = PicPrg_ReadWriteDummy;
    if (ppvWriter) *ppvWriter  = PicPrg_ReadWriteDummy;

    if ( dwTargetBaseAddress == 0 )
    {
        // get address of reader + writer for CODE MEMORY :
        if ( PIC_DeviceInfo.iBitsPerInstruction==24 )
        {
            // most likely a dsPIC30F (24 bits per instruction)
            if (ppvReader) *ppvReader  = dsPIC_ReadCodeMemory;
            if (ppvWriter) *ppvWriter  = dsPIC_WriteCodeMemory;
        }
//#ifndef PIC18F_INCOMPLETE
        else if ( PIC_DeviceInfo.iBitsPerInstruction==16 )
        {
            if (ppvReader) *ppvReader  = PIC18F_ReadCodeMemory;
            if (ppvWriter) *ppvWriter  = PIC18F_WriteCodeMemory;
        }
//#endif
        else
        {
            fResult = false;
        }
    }
    else if ( (long)dwTargetBaseAddress == PIC_DeviceInfo.lDataMemBase )
    {
        // get address of reader + writer for DATA MEMORY :
        if ( PIC_DeviceInfo.iBitsPerInstruction==24 )
        {
            // most likely a dsPIC30F (24 bits per instruction)
            if (ppvReader) *ppvReader  = dsPIC_ReadDataMemory;
            if (ppvWriter) *ppvWriter  = dsPIC_WriteDataMemory;
        }
//#ifndef PIC18F_INCOMPLETE
        else if ( PIC_DeviceInfo.iBitsPerInstruction==16 )
        {
            if (ppvReader) *ppvReader  = PIC18F_ReadDataMemory;
            if (ppvWriter) *ppvWriter  = PIC18F_WriteDataMemory;
        }
//#endif
        else
        {
            fResult = false;
        }
    }
    else if ( ( (long)dwTargetBaseAddress >= PIC_DeviceInfo.lConfMemBase )
              &&  ( (long)dwTargetBaseAddress <  PIC_DeviceInfo.lConfMemBase+64 ) )
    {
        // get address of reader + writer for CONFIG MEMORY :
        if ( PIC_DeviceInfo.iBitsPerInstruction==24 )
        {
            // most likely a dsPIC30F (24 bits per instruction)
            if (ppvReader) *ppvReader  = dsPIC_ReadConfigRegs;
            if (ppvWriter) *ppvWriter  = dsPIC_WriteConfigRegs;
        }
//#ifndef PIC18F_INCOMPLETE
        else if ( PIC_DeviceInfo.iBitsPerInstruction==16 )
        {
            if (ppvReader) *ppvReader  = PIC18F_ReadConfigRegs;
            if (ppvWriter) *ppvWriter  = PIC18F_WriteConfigRegs;
        }
//#endif
        else
        {
            fResult = false;
        }
    }
    else
    {
        fResult = false;
    }

    return fResult;

} // end PicPrg_GetReadWriteFunctionPtr()


/***************************************************************************/
/** Makes a 'mask' for verifying the old "config word".
 * Only good for PIC12Fxxx+PIC16Fxxx, where the config word
 * was located at 0x2007 for PIC16Fxx(x) .
 */
uint16_t PicPrg_GetConfigWordMask(void)
{
    return PicDev_GetVerifyMaskForAddress( PIC_DeviceInfo.lConfWordAdr );
} // end PicPrg_GetConfigWordMask()


/***************************************************************************/
/** Programming algorithm for early PIC16Fxx devices (14-bit core).
 *  Writes a single 14-bit location in the PIC's CODE memory.
 *  Data is not written when out of 14 bit range (0 - 0x3FFF) (0xFFFF for example)
 */
void PIC_PRG_Flash14ProgCycle(
    uint16_t wLoadCmd,   // 6-bit "load-"command immediately before the data
    uint16_t wData )      // 14-bit data to be written
{
    if (PIC_PRG_iSimulateOnly)
        return;

    PIC_HW_SerialOut_Command6(wLoadCmd, false);  // clock LOW, data HIGH after this
    if (wData < 0x4000)
        PIC_HW_SerialOut_14Bit(wData);        // clock LOW after this (important for JDM prog)
//    PicHw_FlushCommand( wLoadCmd,6,  wData,14 ); // added 2008-05
    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg, true);  // clock LOW, data HIGH after this
    PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // program delay (usually 10ms)
    PIC_HW_SetClockAndData(false, false);

    PicHw_FeedChargePump();  // required to produce Vpp with a charge pump (JDM)

} // end PIC_PRG_Flash14ProgCycle()


/***************************************************************************/
/** Disable the code protection
 *    For many of the 14 bit cores
 */
enum EEraseOper
{
    erasePROTECTION,
    eraseCODE,
    eraseDATA,
};
void PIC_PRG_EraseOper (EEraseOper pOper)
{
    uint16_t LoadCommand = 0;
    switch (pOper)
    {
    case erasePROTECTION:
        LoadCommand = PIC_DeviceInfo.iCmd_LoadConfig;
        PicPrg_iCodeMemErased = 1;
        PicPrg_iConfMemErased = 1;
        PicPrg_iDataMemErased = 1; //-- Usually but not for all
        break;
    case eraseCODE:
        LoadCommand = PIC_DeviceInfo.iCmd_LoadProg;
        PicPrg_iCodeMemErased = 1;
        PicPrg_iConfMemErased = 1;
        break;
    case eraseDATA:
        LoadCommand = PIC_DeviceInfo.iCmd_LoadDataDM;
        PicPrg_iDataMemErased = 1;
        break;
    }
    //-- Disable code protection
    //-- This also erases the configuration
    //-- This also erases the Program and (in most cases) the data memories
    PIC_HW_SerialOut_Command6(LoadCommand, false); /* defeat code protection */
    PIC_HW_SerialOut_14Bit(0x3FFF);

    for (int i=0; i<7; ++i )        // increment internal address counter
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true);

    PIC_HW_SerialOut_Command6(1, true); // ERASE_SETUP 1
    PIC_HW_SerialOut_Command6(7, true); // ERASE_SETUP 2
    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg, true);
    PIC_HW_Delay_us(10000);             // wait  10ms
    PIC_HW_SerialOut_Command6(1, true); // ERASE_SETUP 1
    PIC_HW_SerialOut_Command6(7, true); // ERASE_SETUP 2
    //-- End of Disable code protection sequence
}


/***************************************************************************/
/** Save the Oscillator Calibration word
 *    To be called only for device having such calibration word!
 */
void SaveOscCalWord (void)
{
    uint16_t w;
    // Read OSCCAL *from code memory* before bulk erase ?    ( only for PIC12Fxxx + PIC16Fxxx )
    //  This precious information must be READ and PRESERVED before bulk erase.
    //  (Saving the value is safe, regardless of TSessionConfig::GetDontCareForOsccal() here !)
    if (PIC_PRG_iSimulateOnly)
    {
        w = 0x3478;  // this may be a valid oscillator calibration word (RETLW 0x78)
    }
    else // don't simulate..
    {
        PIC_HW_ProgMode();   // first(?) Vdd on, then(?) Vpp on, program counter to 0
        // Unfortunately we cannot set the internal address directly to that
        // location. We must perform a number of dummy accesses to increment
        // the address up to the osc config word.
        for (int i=0; i<PIC_DeviceInfo.lAddressOscCal; ++i )
        {
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true);
        }
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_ReadProg, true/*flush!!*/);
        w = ( PIC_HW_SerialRead_14Bit() & 0x3FFF);
    } // end if <simulate or not>

    if (TSessionConfig::GetVerboseMessages())
        APPL_ShowMsg( 0, _( "Read osccal, result 0x%06lX" ), w );

    if ( (w!=0x3FFF) || (PIC_lOscillatorCalibrationWord<0) )
    {
        // if the osc calibration word is already present and we have read "3FFF",
        // don't overwrite the oscillator calibration word...
        PIC_lOscillatorCalibrationWord = w;
    }

    if ( (PIC_lOscillatorCalibrationWord & 0xFF00) != 0x3400/*RETLW*/ )
        APPL_ShowMsg( 0, _( "Oscillator calibration word looks bad (0x%06lX) !" ), PIC_lOscillatorCalibrationWord );
}


/***************************************************************************/
/** Save the BandGap Calibration bits
 *    To be called only for device having such calibration bits!
 */
void SaveBandgapCalBit (void)
{
    uint16_t w;
    uint32_t dwVerifyMask;
    // read configuration word, it may contain the bandgap calibration bits
    w = (PIC_PRG_iSimulateOnly) ? 0x11FF : PIC_PRG_ReadConf();

    //-- Avoid saving the bits if the config seems to be erased and we already have saved some bits
    dwVerifyMask = PicDev_GetVerifyMaskForAddress(PIC_DeviceInfo.lConfWordAdr);
    if (((w & dwVerifyMask) != (0x3FFF & dwVerifyMask)) || (PIC_lBandgapCalibrationBits < 0))
        PIC_lBandgapCalibrationBits = (w & PIC_DeviceInfo.wCfgmask_bandgap);

    if (TSessionConfig::GetVerboseMessages())
        APPL_ShowMsg( 0, _( "Read cfg with bandgap ref, result 0x%06lX" ), w );
} // end if(PIC_DeviceInfo.wCfgmask_bandgap != 0)



/***************************************************************************/
/** Erases a PIC.
 * Valid erase options:
 *  PIC_ERASE_CODE ,  PIC_ERASE_DATA , PIC_ERASE_CONFIG , PIC_ERASE_ALL
 *  plus the optional flag:  PIC_SAVE_CALIBRATION
 */
bool PIC_PRG_Erase(int iEraseOptions)
{
    int i;
    uint16_t w;
    wxChar sz80Temp[81];
    bool fIsProtected;
    bool fResult = true;

    if ( ! PicHw_iConnectedToTarget )
    {
        PIC_HW_ConnectToTarget();    // connect to target if not already done
    }
    if ( PIC_HW_CanSelectVdd() )       // since 2005-09-29 :
    {
        PIC_HW_SelectVdd( 1/*norm*/ );  // use the "normal" voltage (=5V) for erasing
    }


    if ( PIC_DeviceInfo.iBitsPerInstruction==12 )
    {
        fResult = PIC10F_ProgramAll( PIC_ACTION_ERASE );
        PicPrg_iCodeMemErased = 1;
        PicPrg_iDataMemErased = 1;
        PicPrg_iConfMemErased = 1;
    }
    else
    {
        // no 12-bit core..

        if ( (iEraseOptions & PIC_SAVE_CALIBRATION) && (PIC_iHaveErasedCalibration==0) )
        {
            //
            // Save certain "precious" values from the PIC before erasing all ?
            //

            if (PIC_DeviceInfo.lAddressOscCal >= 0
            &&  PIC_DeviceInfo.lAddressOscCal < PIC_DeviceInfo.lCodeMemSize )
                SaveOscCalWord();

            if (PIC_DeviceInfo.wCfgmask_bandgap != 0)
                SaveBandgapCalBit();

        } // end if(iEraseOptions & PIC_SAVE_CALIBRATION)


        if (PIC_PRG_iSimulateOnly)
        {
            PIC_iHaveErasedCalibration = 1;
            PicPrg_iCodeMemErased = 1;
            PicPrg_iDataMemErased = 1;
            PicPrg_iConfMemErased = 1;
        }
        else
        {
            // BULK ERASE or CHIP ERASE(newer devices) to unprotect and erase everything.
            if (TSessionConfig::GetVerboseMessages())
            {
                APPL_ShowMsg( 0, _( "Erasing chip using algorithm \"%hs\" ." ),
                              PicDev_AlgorithmCodeToString(PIC_DeviceInfo.wEraseAlgo) );
            }
            switch ( PIC_DeviceInfo.wEraseAlgo )
            {
            case PIC_ALGO_16FXX:
                //  This "bulk erase" procedure failed on a code-protected
                //    PIC12F675 (an early sample device from September 2002) .
                //  2004-01-28: NOTE: THIS DOES NOT SEEM TO WORK for the 16F628A !
                //              (bulk erase program memory has been changed significantly, see below)
                //  2004-03-30: This also does NOT seem to work for the 16F630 ... see 12FXX !
                PIC_HW_ProgMode();           // first(?) Vdd on, then(?) Vpp on  (16F628A: other way round)
                //-- Disable code protection
                //-- This also erases the Program and the data memories
                PIC_PRG_EraseOper(erasePROTECTION);

                //-- In case the Disable code protection would not erase anything when code is not protected
                //-- Erase again program and data
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadProg, false); //-- Start by loading prog data withh 0x3FFF
                PIC_HW_SerialOut_14Bit(0x3FFF);

                PIC_PRG_Flash14ProgCycle(PIC_DeviceInfo.iCmd_EraseProg, /* bulk erase program/config memory */
                                         0xFFFF); // no data
                PIC_HW_Delay_us(10000);  // another 10ms though not mentioned in the specs
                PicPrg_iCodeMemErased = PicPrg_iConfMemErased = 1;

                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadConfig, false); //-- Add that for the 16F631 and friends to erase User ID
                PIC_HW_SerialOut_14Bit(0x3FFF);                                   //-- Should not prevent others to work

                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadDataDM, false); //-- Start by loading data withh 0x3FFF
                PIC_HW_SerialOut_14Bit(0x3FFF);                                  //-- This is specified for 16F627 16F628 and 16F84

                PIC_PRG_Flash14ProgCycle(PIC_DeviceInfo.iCmd_EraseData, /* bulk erase data memory */
                                         0xFFFF); // no data
                PicPrg_iDataMemErased = 1;
                PIC_iHaveErasedCalibration = 1; // please don't crash after this ;-)
                PIC_HW_ProgModeOff();           // programming voltage off, clock off, data high-z
                PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged before returning
                fResult = true; // still need a way to check the success of this !!!
                break;  // end case PIC_ALGO_16FXX

            case PIC_ALGO_16FXX_OLD_ERASE:
                //  This "bulk erase" procedure failed on a code-protected
                //    PIC12F675 (an early sample device from September 2002) .
                //  2004-01-28: NOTE: THIS DOES NOT SEEM TO WORK for the 16F628A !
                //              (bulk erase program memory has been changed significantly, see below)
                //  2004-03-30: This also does NOT seem to work for the 16F630 ... see 12FXX !
                PIC_HW_ProgMode();           // first(?) Vdd on, then(?) Vpp on  (16F628A: other way round)
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadConfig, false); /* defeat code protection */
//                PHWInfo.iCurrProgAddress = PIC_DeviceInfo.lConfMemBase; // added 2008-05 for the plugin-DLL
                PIC_HW_SerialOut_14Bit(0x3FFF);
//                PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadConfig, 6, 0x3FFF, 14); // added 2008-05

                for ( i=0; i<7; ++i )        // increment internal address counter
                {
                    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true);
                }
                PIC_HW_SerialOut_Command6(1, true); // mysterious command with no name
                PIC_HW_SerialOut_Command6(7, true); //    "           "    "   "   "
                //  (found much later in the Programming Specification for the PIC16F627,628:
                //    0x01 = "ERASE_SETUP 1"
                //    0x07 = "ERASE_SETUP 2" ... but no real explanation )
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg, true);
                PIC_HW_Delay_us(10000);      // allow  10ms (minimum) to pass
                PIC_HW_SerialOut_Command6(1, true); // mysterious command with no name
                PIC_HW_SerialOut_Command6(7, true); //    "           "    "   "   "
                PIC_PRG_Flash14ProgCycle(PIC_DeviceInfo.iCmd_EraseProg, /* bulk erase program/config memory */
                                         0x3FFF); // dummy data (14 bit)
                PIC_HW_Delay_us(10000);  // another 10ms though not mentioned in the specs
                PicPrg_iCodeMemErased = PicPrg_iConfMemErased = 1;
                PIC_PRG_Flash14ProgCycle(PIC_DeviceInfo.iCmd_EraseData, /* bulk erase data memory */
                                         0x3FFF);
                PIC_HW_Delay_us(10000);      // another 10ms though not mentioned in the specs
                PicPrg_iDataMemErased = 1;
                PIC_iHaveErasedCalibration = 1; // please don't crash after this ;-)
                PIC_HW_ProgModeOff();           // programming voltage off, clock off, data high-z
                PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged before returning
                fResult = true; // still need a way to check the success of this !!!
                break;  // end case PIC_ALGO_16FXX_OLD_ERASE

            case PIC_ALGO_ERASE_16F62XA:  // new since 2004-01-28 (YUCC!!!)
                // From the PIC16F62xA programming spec:
                // > Bulk Erase Program Memory
                // >   The program memory can be erased with the Bulk
                // >   Erase Program Memory command.
                // >   To perform a bulk erase of the program memory, the
                // >   following sequence must be performed:
                // >   1. Execute a Load Data for Program Memory with
                // >   the data word set to all '1's (0x3FFF).
                // >   2. Execute a Bulk Erase Program Memory
                // >   command
                // >   3. Wait TERA for the erase cycle to complete.
                // >   If the address is pointing to the configuration memory
                // >   (0x2000-0x200F), then both user ID locations and
                // >   program memory will be erased.
                // What's the big difference to the good old 16F628 ?
                // The 16F628 required a "Begin Programming" command somewhere in between.
                // And, there are no more "mysterious commands with no name" .
                // The 16F628A seems to be irritated by this. Holy s...  !
                // So, here is the code to "Bulk Erase Program Memory" a la PIC16F628A :
                PIC_HW_ProgMode();           // 16F628A wants to have "Vpp high, the Vdd on" here !
                PIC_HW_SerialOut_Command6(0, false);  // "Load Config" (pointer to config memory) to  defeat code protection
                PIC_HW_SerialOut_14Bit(0x3FFF); // data word set to all '1's (0x3FFF)
//                PicHw_FlushCommand(0, 6, 0x3FFF, 14);
                PIC_HW_SerialOut_Command6(9, true);   // Bulk Erase Program Memory (maybe data too ?!)
                PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Erase_us);  // wait Tera (6 ms for the 16F628A, but 10ms won't hurt)
                // This should have erased the CODE memory. What about the DATA memory ?
                //   (  Remember, this routine is called PIC_PRG_Erase() ,
                //      and it's already complicated enough   ;)
                // We don't know if it has been erased, too, because this depends on the state
                // of the data protection bit (CPD in the config word). So, to make sure :
                PIC_HW_SerialOut_Command6(11, true); // bulk erase data memory
                PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Erase_us);  // wait Tera ... THATS ALL FOR THE 16F628A !
                PIC_HW_ProgModeOff();           // programming voltage off, clock off, data high-z
                PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged before returning
                PIC_iHaveErasedCalibration = 1; // ..if any
                PicPrg_iCodeMemErased = PicPrg_iDataMemErased = PicPrg_iConfMemErased = 1;
                fResult = true; // still need a way to check the success of this !!!
                break;  // end case PIC_ALGO_ERASE_16F62XA

            case PIC_ALGO_PIC16F716:      // PIC16F716 should be erased this way :
                PIC16F716_EraseAll();   // implemented in PIC16F716_PRG.C
                PicPrg_iCodeMemErased = PicPrg_iDataMemErased = PicPrg_iConfMemErased = 1;
                break;

            case PIC_ALGO_16F87X:
            case PIC_ALGO_16F87XA:
            case PIC_ALGO_ERASE_16F87X:   // 16F87X, but not 16F87xA !
                // Erasing the 16F87x is ridiculous . Two different algorithms,
                // depending on the state of code and/or data-protection bits.
                // NOTE: THIS SOUNDS SIMILAR TO THE 16F81X(!)
                //       BUT IT IS NOT ! (spot the difference yourself)
                // So here is the "erase everything" procedure for PIC16F87X :
                w = PIC_PRG_ReadConf(); // read configuration word, to check if we used protection ;-)
                fIsProtected = false;
                if ( (w & PIC_DeviceInfo.wCfgmask_cpbits) != PIC_DeviceInfo.wCfgmask_cpbits)
                {
                    // any ZERO-bit amongst the "code protection bits" means we're protected
                    fIsProtected = true;
                }
                if ( (w & PIC_DeviceInfo.wCfgmask_cpd) != PIC_DeviceInfo.wCfgmask_cpd)
                {
                    // any ZERO-bit amongst the "data protection bits" means we're protected
                    //  (usally there is just a single data protection bit -if any- with mask 0x0100)
                    fIsProtected = true;
                }

                PIC_HW_ProgMode();            // first(?) Vdd on, then(?) Vpp on
                if (fIsProtected)
                {
                    // PIC is protected, but there IS NO "CHIP ERASE" command for the 16F87X. From spec:
                    // > When a Chip Erase command is issued and the PC points to (2000h-2007h), all of the
                    // > configuration memory, program memory and data memory will be erased .
                    APPL_ShowMsg( 0, _("EraseAll: Device is protected, using method 2") );
                    PIC_PRG_EraseOper(erasePROTECTION);
                }
                else // PIC is *not* protected (neither CODE nor DATA), must use "BULK ERASE" in another flavour...
                {
                    APPL_ShowMsg( 0, _("EraseAll: Device is not protected, using method 1") );
                    PIC_PRG_EraseOper(eraseCODE);
                    PIC_PRG_EraseOper(eraseDATA);
                } // end < erase an UNPROTECTED PIC16F87X >
                PIC_HW_ProgModeOff();           // programming voltage off, clock off, data high-z
                PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged before returning
                PicPrg_iCodeMemErased = PicPrg_iDataMemErased = PicPrg_iConfMemErased = 1;
                fResult = true; // still need a way to check the success of this !!!
                break;  // end < ridiculous erase algorithm for PIC16F87X  >


            case PIC_ALGO_ERASE_12F6XX:  // added 2006-03-01
                // (due to problems with  PIC12F683, reported by Edward Schlunder)
                // "Bulk erase" procedure for:
                // PIC12F683, PIC12F635
                // PIC16F685, PIC16F687, PIC16F636, PIC16F688,
                // PIC16F639, PIC16F689, PIC16F684, PIC16F690.
                //
                // PIC12F6XX/16F6XX Memory Programming Specification:
                // http://ww1.microchip.com/downloads/en/DeviceDoc/41204F.pdf
                PIC_HW_ProgMode();     // first(?) Vdd on, then(?) Vpp on

                // Erase Program Memory, User ID words, and Config Word
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadConfig, false); /* reset PC to 0x2000, allows erasing User ID words */
//                PHWInfo.iCurrProgAddress = PIC_DeviceInfo.lConfMemBase; // added 2008-05 for the plugin-DLL
                PIC_HW_SerialOut_14Bit(0x3FFF);
//                PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadConfig, 6, 0x3FFF, 14 );


                // If the user checked "no special treatment for OSCCAL," then
                // we will turn on support for erasing the calibration words
                if (TSessionConfig::GetDontCareForOsccal())
                {
                    // increment internal address counter
                    for ( i=0; i<9; ++i )
                        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true);

                    PIC_iHaveErasedCalibration = 1;
                }

                PIC_HW_SerialOut_Command6(9, true); // 6-bit-cmd: bulk erase program memory
                PIC_HW_Delay_us(10000);       // allow  10ms (minimum) to pass (Tera is 6ms max in datasheet)

                // Erase EEPROM Data Memory
                PIC_HW_SerialOut_Command6(11, true); // 6-bit-cmd: bulk erase data memory
                PIC_HW_Delay_us(10000);       // allow  10ms (minimum) to pass (Tera is 6ms max in datasheet)

                PIC_HW_ProgModeOff();      // programming voltage off, clock off, data high-z
                PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged before returning

                PicPrg_iCodeMemErased = PicPrg_iDataMemErased = PicPrg_iConfMemErased = 1;
                fResult = true; // still need a way to check the success of this !!!
                break;  // end case PIC_ALGO_ERASE_12F6XX


            case PIC_ALGO_12FXX:
            case PIC_ALGO_16F630: // ERASE 16F630, added 2004-03-30 (should work for 12F675+16F630)
                // "Bulk erase" procedure for PIC12F629/675
                PIC_HW_ProgMode();            // first(?) Vdd on, then(?) Vpp on


                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadConfig, false); //-- Start by loading data with 0x3FFF
                PIC_HW_SerialOut_14Bit(0x3FFF);                                  //-- This is specified for 16F631 & friends to erase ID
                                                                                //-- Hopefully this should not affect others

                PIC_HW_SerialOut_Command6(9, true); // 6-bit-cmd: bulk erase program memory
                PIC_HW_Delay_us(10000);       // allow  10ms (minimum) to pass

                PIC_HW_SerialOut_Command6(11, true); // 6-bit-cmd: bulk erase data memory
                PIC_HW_Delay_us(10000);       // allow  10ms (minimum) to pass

                PIC_HW_ProgModeOff();      // programming voltage off, clock off, data high-z
                PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged before returning
                PIC_iHaveErasedCalibration = 1;
                PicPrg_iCodeMemErased = PicPrg_iDataMemErased = PicPrg_iConfMemErased = 1;
                fResult = true; // still need a way to check the success of this !!!
                break;  // end case PIC_ALGO_12FXX


            case PIC_ALGO_16F81X:
            case PIC_ALGO_ERASE_16F81X:
            case PIC_ALGO_ERASE_16F87XA:
                // Either the 16F818 + 16F87xA or its programming specification is ridiculous .
                // The easy way to just "chip-erase everything" should not work if the chip
                // is *NOT* write protected !  The spec suggests to check the protection bits
                // to decide which of the various erase algorithms should be used ! ! Holy Shit .
                // NOTE: THIS SOUNDS SIMILAR TO THE 16F87X(!)
                //       BUT IT IS NOT ! ! ! ! (spot the difference yourself) ...
                // So here is the "erase everything" procedure for PIC16F818/819 :
                w = PIC_PRG_ReadConf(); // read configuration word, to check if we used protection ;-)
                if (TSessionConfig::GetVerboseMessages())
                {
                    _stprintf(sz80Temp, _( "EraseAll: Read config word 0x%06lX" ), w);
                    APPL_ShowMsg( 0, sz80Temp );
                }
                fIsProtected = false;
                if ( (w & PIC_DeviceInfo.wCfgmask_cpbits) != PIC_DeviceInfo.wCfgmask_cpbits)
                {
                    // any ZERO-bit amongst the "code protection bits" means we're protected
                    fIsProtected = true;
                }
                if ( (w & PIC_DeviceInfo.wCfgmask_cpd) != PIC_DeviceInfo.wCfgmask_cpd)
                {
                    // any ZERO-bit amongst the "data protection bits" means we're protected
                    //  (usally there is just a single data protection bit -if any- with mask 0x0100)
                    fIsProtected = true;
                }

                PIC_HW_ProgMode();            // first(?) Vdd on, then(?) Vpp on
                if (fIsProtected)
                {
                    // PIC protected, one must(?) use the "CHIP ERASE" command. From spec:
                    // > When a Chip Erase command is issued and the PC points to (2000h-2007h), all of the
                    // > configuration memory, program memory and data memory will be erased .
                    APPL_ShowMsg( 0, _( "EraseAll: Device is protected, using CHIP erase" ) );
                    //           PIC_HW_ProgMode();           // first(?) Vdd on, then(?) Vpp on
                    PIC_HW_SerialOut_Command6(0x00, false); // "Load Config" = set program counter to 0x2000
                    PIC_HW_SerialOut_14Bit(0x3FFF);  // why this is/was necessary, remains a mystery !
//                    PicHw_FlushCommand( 0x00, 6, 0x3FFF, 14 );
                    for ( i=0; i<7; ++i )            // increment internal address counter to 0x2007
                        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true );
                    PIC_HW_SerialOut_Command6(0x1F, true);  // "Chip Erase" (does this work if NOT protected?)
                    PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Erase_us);  // wait for erase
                    PIC_HW_SerialOut_Command6(0x17, true);  // "End Programming" (ends "external" timing ?)
                }
                else // PIC is *not* protected (neither CODE nor DATA), must use "BULK ERASE" (? !?)
                {
                    APPL_ShowMsg( 0, _( "EraseAll: Device is not protected, using BULK erase" ) );
                    PIC_HW_SerialOut_Command6(0x09, true);  // "Bulk Erase Program Memory" (externally timed)
                    PIC_HW_SerialOut_Command6(0x08, true);  // "Begin Erase" (externally timed)
                    PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Erase_us);  // wait for erase
                    PIC_HW_SerialOut_Command6(0x17, true);  // "End Programming" (ends "external" timing ?)

                    PIC_HW_SerialOut_Command6(0x0B, true);  // "Bulk Erase Data Memory" (externally timed)
                    PIC_HW_SerialOut_Command6(0x08, true);  // "Begin Erase" (externally timed)
                    PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Erase_us);  // wait for erase
                    PIC_HW_SerialOut_Command6(0x17, true);  // "End Programming" (ends "external" timing ?)
                }

                PIC_HW_ProgModeOff();      // programming voltage off, clock off, data high-z
                PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged before returning
                PicPrg_iCodeMemErased = PicPrg_iDataMemErased = PicPrg_iConfMemErased = 1;
                fResult = true; // still need a way to check the success of this !!!
                break;  // end <ridiculous erase algorithm for 16F818/819>


            case PIC_ALGO_PIC16F7X:  // Erase a PIC16F7x (PIC16F73/74/76/77 but NOT PIC16F72 - ummm) */
            case PIC_ALGO_16F7X7:    // Erase a PIC16F7x7 :
                // Microchip doesn't seem to be sure whether to call this
                //  "Bulk erase" or "Chip erase" in DS30492A - both terms are used in that document,
                //  and it SEEMS TO MEAN THE SAME THING in the 16F7X7 (UNLIKE IN OTHER PICS - BAAAH again).
                //  The PIC16F737 has no data EEPROM (what a shame - too expensive?),
                //      so no-one seemed to care about the difference .
                //  DS30492A, Chapter 2.3.1.6 "Chip Erase (Program Memory)"
                PIC_HW_ProgMode();            // first(?) Vdd on, then(?) Vpp on
                PIC_HW_SerialOut_Command6(9, true); // 6-bit-cmd: For PIC16F737, 0x09="CHIP erase"
                // (BULK erase program memory in other docs)
                PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Erase_us); // 16F737 requires 30(!!) ms,  thanks Bob

                PIC_HW_ProgModeOff();      // programming voltage off, clock off, data high-z
                PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged before returning
                PIC_iHaveErasedCalibration = 1;
                PicPrg_iCodeMemErased = PicPrg_iDataMemErased = PicPrg_iConfMemErased = 1;
                fResult = true; // still need a way to check the success of this !!!
                break;  // end case PIC_ALGO_16F7X7 (erase)

            case PIC_ALGO_dsPIC30F:  /* erase dsPIC30Fxxx .. */
                // ... is TOTALLY different, so algorithm is in a separate module :
                fResult = dsPIC_EraseAll();  // "Erases all and clear config registers"
                if ( fResult==true )
                {
                    PicPrg_iCodeMemErased = PicPrg_iDataMemErased = PicPrg_iConfMemErased = 1;
                }
                break;  // end case PIC_ALGO_dsPIC30F (erase)

            case PIC_ALGO_PIC18F:     // erase PIC18Fxxxx, separate module by M.v.d. Werff
//#ifdef SUPPORT_PIC18F
                PIC18F_EraseChip();
                PicPrg_iCodeMemErased = PicPrg_iDataMemErased = PicPrg_iConfMemErased = 1;
                fResult = true; // cannot check if "successful", assume it's ok..
//#endif // SUPPORT_PIC18F
                break;  // end case PIC_ALGO_PIC18F (erase)

            case PIC_ALGO_MULTI_WORD: // not an erase algo
            default:    // this PIC does not seem to support BULK ERASE
                APPL_ShowMsg( 0, _( "EraseAll: missing bulk erase algorithm !" ) );
                fResult = false;
                break;
            } // end switch
        } // end if(! PIC_PRG_iSimulateOnly)
    } // end else <  no 12-bit core >

    return fResult;  // still need a way to check the success of this !!!
} // end PIC_PRG_Erase()


/***************************************************************************/
/** Loads the (first) configuration word.
 * Increments the PIC's internal address up to the address dwDeviceAddress
 * (passed as argument "dwDeviceAddress")
 */
void PIC_PRG_LoadConf(
    uint32_t dwDeviceAddress,   //!< usually 0x2000 for the 1st config word
    uint16_t wDataAfterLoadCmd ) //!< 14-bit data to be loaded
{
    int i, n;
    if (PIC_PRG_iSimulateOnly)
        return;

    PIC_HW_SerialOut_Command6( PIC_DeviceInfo.iCmd_LoadConfig/*0x00*/,  false );
    // The name "LOAD CONFIGURATION" may be misleading !
    // All this command does is set an internal address counter to 0x2000.
//    PHWInfo.iCurrProgAddress = PIC_DeviceInfo.lConfMemBase; // added 2008-05 for the plugin-DLL
    PIC_HW_SerialOut_14Bit( wDataAfterLoadCmd/*ex:PicBuf_GetConfigWord(0)*/ );
//    PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadConfig, 6, wDataAfterLoadCmd, 14 );
    // This "loads" some 14 bit word somewhere but does not program it (yet).
    // In fact, this is rarely the Configuration word, because
    // at address 0x2000 most PICs will have the first ID LOCATION !
    // Writing the CONFIG WORD to that address would be foolish .
    // Caution, the programming specs for various PICs are different here.
    // Some require a "dummy word" containing 0x3FFF sent after "LoadConfig",
    // others require sending the contents of the 1st "user-ID"-location,
    // others really require sending the contents of the "config word" here.
    // Baaah .
    // So the best bet is not to use this old subroutine, but use only
    // simple serial in/out commands instead ( see PIC16F716_prg.C for example).

    n = dwDeviceAddress - PIC_DeviceInfo.lConfMemBase; // PIC16F84: lConfMemBase=0x2000
    for ( i=0; i<n; ++i )
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true );
    PIC_HW_SetClockAndData(false, false);

    // Arrived here, the PIC's internal program pointer points
    // to 'dwDeviceAddress' ,  usually 0x2007 for the config word .
} // end PIC_PRG_LoadConf()


/***************************************************************************/
/** Read the PIC's configuration (first) word.
 * Also called from some erase subroutines !
 * Doesn't make much sense in PIC18F and dsPIC,
 * which have many different configuration REGISTERS.
 */
uint16_t PIC_PRG_ReadConf(void)
{
    int i;
    uint16_t w;

    if ( PIC_DeviceInfo.iBitsPerInstruction==24 )
    {
        // 24 bit per "instruction word" - must be a dsPIC or similar .
        // Things are totally different here !
        // There are 8(!) Configuration Registers with 16 bit each in this case,
        // located at addresses 0xF80000 to 0xF8000E .
        // HERE, for compatibility, return only the "most important config register"...
        return dsPIC_STDP_Read16BitLocation( PIC_DeviceInfo.lConfWordAdr );
    }

    // Arrived here: NOT a dsPIC, NOT an 18Fxxx, so must be a 12Fxxx or a 16Fxxx .
    // Read ID Locations (0x2000..0x2003),  Device ID Word (0x2006),
    //   and Config Word (0x2007)
    PIC_HW_ProgMode();        // first(?) Vdd on, then(?) Vpp on
    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadConfig, false);  // internal address to 0x2000
//    PHWInfo.iCurrProgAddress = PIC_DeviceInfo.lConfMemBase; // added 2008-05 for the plugin-DLL
    PIC_HW_SerialOut_14Bit(0x3FFF);
//    PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadConfig, 6, 0x3FFF, 14 );
    for ( i=0; i<4; ++i )
    {
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_ReadProg, true );
        w = (PIC_HW_SerialRead_14Bit() & 0x3FFF);
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true );
    }
    for ( i=0; i<2; ++i )  // skip locations 0x2004 and 0x2005
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true );
    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_ReadProg, true ); // read location 0x2006
    w  = PIC_HW_SerialRead_14Bit() & 0x3FFF;  // -> device ID
    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true );
    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_ReadProg, true ); // read location 0x2007
    w  = PIC_HW_SerialRead_14Bit() & 0x3FFF;  // -> 1st config word
    PIC_HW_ProgModeOff();      // programming voltage off, clock off, data high-z
    PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged before returning

    return w;
}


/***************************************************************************/
/** Programms some older PIC16Fxx and PIC12F6xx chips .
 * Good for CODE, CONFIG, and DATA EEPROM in a 16F84 + many other "old" PIC devices ,
 * it also seemed to work for the PIC16F628,
 * but seemed to fail with a newer sample
 * of PIC16F628A in 2005 (reported by Anand Dhuru).
 * See case-list in PIC_PRG_Program() !
 */
bool PIC_PRG_Program16FXX(   // Also used for 12FXX (but can't change the name to ..ProgramXXFXX ;)
    uint32_t *pdwSourceData,  //!< pointer to source buffer
    int n,     //!< count of WORDS to be programmed
    uint32_t dwMask, //!< only "1"-bits in the mask may be PROGRAMMED TO ONE and verified
    int ldcmd,  //!< command word to "LOAD" the data, for example PIC_DeviceInfo.iCmd_LoadProg
    int rdcmd,  //!< ICSP instruction             like PIC_DeviceInfo.iCmd_ReadProg
    uint32_t dwDeviceAddress ) //!< device specific dwDeviceAddress address like PIC_DeviceInfo.conf_word_adr
{
    int i;
    uint16_t r, w;
    uint16_t  wFlags;
    uint32_t dwDeviceAddr2;
    uint32_t dwVerifyMask;  // when programming CONFIG REGS, individual masks for verify may be required !
// bool fIsConfigMem;
    int  nGoodConfigMemLocs = 0;
    int  n_errors = 0;
    bool fOk = true;


    if ( Appl_CheckUserBreak() )
        return false;

    if (! PIC_PRG_iSimulateOnly)
        PIC_HW_ProgMode();          // first(?) Vdd on, then(?) Vpp on

    if (   ( dwDeviceAddress >= (uint32_t)PIC_DeviceInfo.lConfMemBase)
            && ( dwDeviceAddress <= (uint32_t)PIC_DeviceInfo.lConfWordAdr) )
    {
        PIC_PRG_LoadConf( dwDeviceAddress,  // load the 1st config word and set the PIC's internal address counter
                          pdwSourceData[0]/* or better PicBuf_GetConfigWord(0), or always 0x3FFF ?*/  );
    }

    for ( i=0; i<n; ++i )
    {
        dwDeviceAddr2 = dwDeviceAddress + i; // Note: for 16FXX, the device address steps by ONE for each config mem location
//        PHWInfo.iCurrProgAddress = dwDeviceAddr2; // added 2008-05 for the plugin-DLL
        w = pdwSourceData[i] & dwMask;
        if ( PIC_DeviceInfo.wCanRead ) //  1 = can read while programming,   0 = can NOT read in between
        {
            if (PIC_PRG_iSimulateOnly)
            {
                r = 0x3FFF; // simulated cell is empty
            }
            else
            {
                PIC_HW_SerialOut_Command6(rdcmd, true);     // READ command to check old memory content
                r = PIC_HW_SerialRead_14Bit() & dwMask;
            }
            if ( w != r )  // this cell must be programmed..
            {
                if (PIC_PRG_iSimulateOnly)
                {
                    r = w & dwMask;
                }
                else
                {
                    PIC_PRG_Flash14ProgCycle( ldcmd, // 6-bit load command  for program OR data memory
                                              w);    // 14-bit data(w) + Begin Programming + Wait Tprog
                    if (PIC_DeviceInfo.iCmd_BeginProg == 24 )             //test for externally timed begin prog command and
                    {
                        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg, true); // send end prog.  12F60x/12F61x/16F61X
                        PIC_HW_Delay_us(100);                                         // externally timed programming!!  2007-06-01 Andrew Kibler
                    }
                    PIC_HW_SerialOut_Command6(rdcmd, true);  // read the programmed word back after progging..
                    r = PIC_HW_SerialRead_14Bit() & dwMask;
                }
                // Determine the "verify-mask". This is usually 0x3FFF for PIC16xxx, but:
                // When programming CONFIG REGS, individual masks for verify may be required..
                dwVerifyMask = PicDev_GetVerifyMaskForAddress(dwDeviceAddr2);
                if ( (w & dwVerifyMask) != (r & dwVerifyMask) )
                {
                    ++n_errors;
                    _stprintf(PIC_error_string, _( "Verify Error: %06lX: read %06lX, wanted %06lX" ), dwDeviceAddr2, r, w );
                    APPL_ShowMsg( 0, PIC_error_string);
                    fOk = false;
                    wFlags = PicBuf_GetMemoryFlags(dwDeviceAddr2);
                    if (wFlags & PIC_BUFFER_FLAG_PRG_ERROR)
                        wFlags |=PIC_BUFFER_FLAG_DEAD;
                    else
                        wFlags |=PIC_BUFFER_FLAG_PRG_ERROR;
                    PicBuf_SetMemoryFlags(dwDeviceAddr2,  wFlags);
                    if ( (long)dwDeviceAddr2 == PIC_DeviceInfo.lConfWordAdr )
                    {
                        PicPrg_iConfWordVerified = -1;  // "classic" config word has NOT passed verify-test
                    }
                    else if ( PicDev_IsConfigMemLocationValid(dwDeviceAddr2) )
                    {
                        // other stuff in the "config memory" (like ID locations for PIC16Fxxx)
                        nGoodConfigMemLocs = -9999;
                    }
                }
                else
                {
                    // this cell looks good, remove old error flag for memory display
                    PicBuf_SetMemoryFlags(dwDeviceAddr2,
                                          PicBuf_GetMemoryFlags(dwDeviceAddr2)
                                          & (~PIC_BUFFER_FLAG_PRG_ERROR)
                                          & (~PIC_BUFFER_FLAG_DEAD) ) ;
                    if ( (long)dwDeviceAddr2 == PIC_DeviceInfo.lConfWordAdr )
                    {
                        PicPrg_iConfWordVerified = 1;  // "classic" config word has been programmed AND VERIFIED
                    }
                    else if ( PicDev_IsConfigMemLocationValid(dwDeviceAddr2) )
                    {
                        // other stuff in the "config memory" (ID location for PIC16Fxxx)
                        if ( nGoodConfigMemLocs>=0 )
                            ++nGoodConfigMemLocs;
                    }
                }
            } // end if ( w != r) -> "must be programmed"
        } // end if (can_read)
        else // can NOT read, so cannot verify during "program"
        {
            //  (and cannot check if cell really needs to be programmed)
            if (! PIC_PRG_iSimulateOnly)
            {
                // send 6-bit load command  for program OR data memory, followed by 14 bit data,
            PIC_PRG_Flash14ProgCycle(ldcmd,w);  // + Begin Programming + Wait Tprog
                if (PIC_DeviceInfo.iCmd_BeginProg == 24 )                 //test for externally timed begin prog command and
                {
                    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg, true); // send end prog.  12F60x/12F61x/16F61X
                    PIC_HW_Delay_us(100);                                   // externally timed programming!! 2007-06-01 Andrew Kibler
                }
                // Note about PIC16F628A: The prog spec calls this the "Program Cycle"
                //  in DS41196E, figure 3-3, for Program Memory (ldcmd=2, begin_prog=8) :
                //  "Load Data for Program Memory" + "Begin Programming" + "Wait Tprog" .
                // Tprog for code memory is only 2.5 ms for the PIC16F628A,
                //       but 6.0 ms for the PIC16F628A's Data EEPROM !
                // So far, WinPic allows only one common Tprog for all memory ranges.
                // Could that ridiculous difference be the cause of the F628A trouble ?
            }
        } // end else < ! PIC_DeviceInfo.wCanRead >
        // If this was the "config word", set a flag, READING may be impossible now after setting some CP bits..
        if ( (long)dwDeviceAddr2 == PIC_DeviceInfo.lConfWordAdr )
        {
            PicPrg_iConfWordProgrammed = 1;  // "classic" config word has been programmed now
        }
        else if ( PicDev_IsConfigMemLocationValid(dwDeviceAddr2) )
        {
            // other stuff in the "config memory" was written but not necessarily verified:
            PicPrg_iConfMemProgrammed = 1;
        }
        if ( PIC_PRG_iSimulateOnly )
        {
            wxMilliSleep(10);
        }
        else
        {
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true);
        }
        APPL_ShowProgress( (100*i) / n );
        if (n_errors>16)
        {
            _stprintf(PIC_error_string, _( "Programming aborted after %d errors." ), n_errors);
            APPL_ShowMsg( 0, PIC_error_string);
            break;
        }
        if ( (i&15)==0 ) // added 2007-08-27, because a USB<->RS-232 adapter was SOOO TERRIBLY SLOW...
        {
            Appl_CheckUserBreak();
        }
        if (APPL_iUserBreakFlag)
        {
            fOk=false;
            break;
        }
    } // end for(i..)
    if ( ! PIC_PRG_iSimulateOnly )
    {
        PIC_HW_ProgModeOff();  // programming voltage off, clock off, data high-z
        PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged before continuing
    }

    if (nGoodConfigMemLocs>0)
        PicPrg_iConfMemVerified = 1; // other stuff in the "config memory" (ID location for PIC16Fxxx)
    else if (nGoodConfigMemLocs<0)
        PicPrg_iConfMemVerified = -1;
    /* if nGoodConfigMemLocs==0, DO NOT TOUCH PicPrg_iConfMemVerified here ! */

    if ( ! PIC_DeviceInfo.wCanRead ) //  1 = can read while programming,   0 = can NOT read in between
    {
        // if the PIC could not be 'read' while programming, VERIFY it now !
        if (! PicPrg_Verify( dwDeviceAddress, // device internal "base"-address
                             pdwSourceData,   // pointer to buffer
                             n,       // count of memory locations to be verified
                             dwMask,  // only look at the 14(?) bits we have written
                             rdcmd))  // read instruction (for verify, here: 0x04)
        {
            fOk = false;
        }
    } // end if( ! PIC_DeviceInfo.wCanRead )
    else
    {
        // already verified while programming, so set the verify-result here:
        PicPrg_SetVerifyResult( dwDeviceAddress, fOk ? +1 : -1 );
    }

    return fOk/*?*/ ;
} // end PIC_PRG_Program16FXX()


/***************************************************************************/
/** Programs a PIC16F630.
 * Good for CODE memory in a 16F630 only .   Added 2004-03-30 without testing !
 * The serial commands in the device into table are IGNORED to make things easier.
 */
bool PIC_PRG_ProgramCode16F630(
    uint32_t *pdwSourceData,  //!< pointer to source buffer
    int n,               //!< count of WORDS to be programmed
    uint32_t dwMask,          //!< only "1"-bits in the mask may be PROGRAMMED TO ONE and verified
    uint32_t dwDeviceAddress) //!< device specific "base"-address like PIC_DeviceInfo.conf_word_adr
{
    int i;
    uint16_t r, w;
    uint16_t wFlags;
    uint32_t dwDeviceAddr2;
    uint32_t dwVerifyMask;
    int  n_errors = 0;
    bool fOk = true;


    if ( Appl_CheckUserBreak() )
        return false;

    if (! PIC_PRG_iSimulateOnly)
        PIC_HW_ProgMode();          // first(?) Vdd on, then(?) Vpp on

    // Entry point is PIC16F630 memory programming specification, page 12, Figure 2-14,
    // *AFTER* "BULK ERASE". The OSCCAL value and BG calib bits have already been read.
    for ( i=0; i<n; ++i )
    {
        dwDeviceAddr2 = dwDeviceAddress + i; // Note: for 16FXX, the device address steps by ONE for each config mem location
//        PHWInfo.iCurrProgAddress = dwDeviceAddr2; // added 2008-05 for the plugin-DLL
        w = pdwSourceData[i] & dwMask;
        if (PIC_PRG_iSimulateOnly)
        {
            r = w & dwMask;
        }
        else
        {
            // "Program Cycle" ...
            PIC_HW_SerialOut_Command6(0x02, false); // "Load command  for program memory"
            PIC_HW_SerialOut_14Bit(w);       //   .. followed by 14 data bits (not shown in Fig 2-14)
//            PicHw_FlushCommand( 0x02, 6,  w, 14 );
            PIC_HW_SerialOut_Command6(0x08, true); // "begin programming" (internally timed ! ! !)
            PIC_HW_Delay_us(2500);           // "Wait Tprog1"  (2.5ms for CODE, INTERNALLY TIMED, no "end prog"!! )
            // Note: different "Tprog1" for DATA+CODE ... not supported in dev list,
            //       so using a 'fixed' value here !

            // "Read Data from Program Memory" ...
            PIC_HW_SerialOut_Command6(0x04, true);
            r = PIC_HW_SerialRead_14Bit() & dwMask;
            // 2004-03-30, when tested with a 16F628: HERE OK, LATER ALL ZERO .
            //           Reason: different location of CODE PROTECTION BITS .
            //           At this step, reading is ok because protection still off .
        }

        // Determine the "verify-mask". This is usually 0x3FFF for PIC16xxx, but:
        // When programming CONFIG REGS, individual masks for verify may be required..
        dwVerifyMask = PicDev_GetVerifyMaskForAddress(dwDeviceAddr2);
        if ( ( w & dwVerifyMask ) != ( r & dwVerifyMask ) )
        {
            // "If data not correct, report programming failure" ..
            ++n_errors;
            _stprintf(PIC_error_string, _( "Verify Error: %06lX: read %06lX, wanted %06lX" ), dwDeviceAddress+i, r, w );
            APPL_ShowMsg( 0, PIC_error_string);
            fOk = false;
            wFlags = PicBuf_GetMemoryFlags(dwDeviceAddress+i);
            if (wFlags & PIC_BUFFER_FLAG_PRG_ERROR)
                wFlags |=PIC_BUFFER_FLAG_DEAD;
            else
                wFlags |=PIC_BUFFER_FLAG_PRG_ERROR;
            PicBuf_SetMemoryFlags(dwDeviceAddress+i,  wFlags);
        }

        if ( w == r)
        {
            // this cell looks good, remove old error flag for memory display
            PicBuf_SetMemoryFlags(dwDeviceAddress+i,
                                  PicBuf_GetMemoryFlags(dwDeviceAddress+i)
                                  & (~PIC_BUFFER_FLAG_PRG_ERROR)
                                  & (~PIC_BUFFER_FLAG_DEAD) ) ;
        }
        if ( PIC_PRG_iSimulateOnly )
        {
            wxMilliSleep(10);
        }
        else
        {
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true );
        }
        APPL_ShowProgress( (100*i) / n );
        if (n_errors>16)
        {
            _stprintf(PIC_error_string, _( "Programming aborted after %d errors." ), n_errors);
            APPL_ShowMsg( 0, PIC_error_string);
            break;
        }
        if (APPL_iUserBreakFlag)
            break;
    } // end for(i..)
    if ( ! PIC_PRG_iSimulateOnly )
    {
        PIC_HW_ProgModeOff();  // programming voltage off, clock off, data high-z
        PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged before returning
    }

    // Because this programming subroutine has already VERIFIED, set "ok"/"error" -
    PicPrg_SetVerifyResult( dwDeviceAddress, fOk ? +1 : -1 );

    return fOk/*?*/ ;
} // end PIC_PRG_Program16F630()




/***************************************************************************/
/** Programs some PIC16F87x devices.
 * For CODE MEMORY in PIC16F87x only, (no-A, no "eight memory words at a time")
 * Difference to 16F84: here: first programm ALL, then verify ALL.
 * When tested with an OLD PIC16F870, the 16F84 algo also worked.
 */
bool PIC_PRG_Program16F87X(
    uint32_t *pdwSourceData, //!< pointer to source buffer
    int n,      //!< count of WORDS to be programmed
    uint32_t dwMask, //!< only "1"-bits in the mask may be PROGRAMMED TO ONE and verified
    int ldcmd,   //!< command word to "LOAD" the data, for example PIC_DeviceInfo.iCmd_LoadProg
    int rdcmd,   //!< ICSP instruction like PIC_DeviceInfo.iCmd_ReadProg
    uint32_t dwDeviceAddress) //!< device specific "base"-address like PIC_DeviceInfo.conf_word_adr
{
    int i;
// uint16_t r;
    uint16_t w;
// uint16_t wFlags;
    bool fOk = true;


    if ( Appl_CheckUserBreak() )
        return false;

    if (! PIC_PRG_iSimulateOnly)
        PIC_HW_ProgMode();          // first(?) Vdd on, then(?) Vpp on

    if (   ( dwDeviceAddress >= (uint32_t)PIC_DeviceInfo.lConfMemBase)
            && ( dwDeviceAddress <= (uint32_t)PIC_DeviceInfo.lConfWordAdr) )
    {
        PIC_PRG_LoadConf(dwDeviceAddress,  // load the 1st config word and set the PIC's internal address counter
                         pdwSourceData[0]/* or better PicBuf_GetConfigWord(0), or always 0x3FFF ?*/  );
    }

    for ( i=0; i<n; ++i )
    {
        w = pdwSourceData[i] & dwMask;
        if (! PIC_PRG_iSimulateOnly)
        {
            PIC_PRG_Flash14ProgCycle( ldcmd, w );
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true );
        }
        else // PIC_PRG_iSimulateOnly
        {
            wxMilliSleep(10);
        }
        APPL_ShowProgress( (100*i) / n );
        if (APPL_iUserBreakFlag)
            break;
    } // end for(i..)
    if ( ! PIC_PRG_iSimulateOnly )
    {
        PIC_HW_ProgModeOff();
        PIC_HW_LongDelay_ms(200); // make sure the programming voltage is discharged before entering again
    }

    if (APPL_iUserBreakFlag)
        return false;


    // Verify... belongs to the PIC16F87X's "programming algorithm",
    //           the spec seems to say we cannot read directly after writing (?!)
    //  though an old 16F870 could be programmed with the 16F84's algorithm
    //  which verifies IMMEDIATELY after programming a WORD .
    if (! PicPrg_Verify( dwDeviceAddress,  // device internal address
                         pdwSourceData,   // pointer to buffer
                         n,      // count of memory locations to be verified
                         dwMask, // only look at the 14(?) bits we have written
                         rdcmd)) // read instruction (for verify, here: 0x04)
    {
        fOk = false;
    }


    return fOk/*?*/ ;

} // end PIC_PRG_Program16F87X()


/***************************************************************************/
/** CODE MEMORY PROGRAMMING for PIC16F7x7 (with "TWO words at a time").
 * Expected to work with some other PICs too, but never tested.
 * See detailed info below function header.
 */
bool PIC_PRG_Program16F7X7(
    uint32_t *pdwSourceData,  //!< pointer to source buffer
    int n,     //!< count of WORDS to be programmed
    int nWordsPerProgCycle, //!< 2 "words per programming cycle" in the 16F737
    uint32_t dwMask,  //!< only "1"-bits in the mask may be PROGRAMMED TO ONE and verified
    int ldcmd,  //!< command word to "LOAD" the data, for example PIC_DeviceInfo.iCmd_LoadProg
    int rdcmd,  //!< ICSP instruction like PIC_DeviceInfo.iCmd_ReadProg (for verify)
    uint32_t dwDeviceAddress )  //!< device specific "base"-address like PIC_DeviceInfo.conf_word_adr
// Added by WoBu 2004-07-31 but never tested so far (lacking a sample) .
//
// Some angry notes of the author (grumbling again about INCOMPATIBLE PROGRAMMING SPECS..)
// - Some blockhead decided to write the binary command values ("Command mapping")
//   in the programming specs in LSB...MSB order (Least Significant Bit FIRST - BAAAAAH ! ),
//   while in older specs (16F628 for example) the used MSB...LSB (MSbit first) .
//   Don't be fooled by this, a closer look shows :
//    PIC16F7X7:  "Load Data for Memory",          LSB..MSB(!) 0 1 0 0 x x
//    PIC16F628:  "Load Data for PROGRAM Memory",  MSB..LSB(!) x x 0 0 1 0
//    which is the same, because these seem to be 6-bit-numbers though more bits are actually sent.
// - By passing the "count of WORDS per programming cycle" this may be future-compatible,
//    no matter if Microchip decide to use ONE, TWO, (THREE?), FOUR, EIGHT, SIXTEEN or whatever
//    "number words per programming row" ...
//
//
{
    int  iLocationCounter;
    uint32_t dwCurrentAddress;
// WORD r;
    uint16_t w;
// WORD wFlags;
// int  n_errors = 0;
    bool fOk = true;

    if ( Appl_CheckUserBreak() )
        return false;

    // Caution: The "chip" must have been erased already !
    if (! PIC_PRG_iSimulateOnly)
        PIC_HW_ProgMode();          // first(?) Vdd on, then(?) Vpp on ? ? ? ?
    //  Not clearly specified for the 16F7x7 !
    //   ( In contrast to the 16F62xA programming spec, which contains
    //     a nice timing diagram with the sequence
    //        first-raise-Vpp-then-raise-Vdd
    //      for "high-voltage programming mode",
    //      which is what what most interfaces use. )
    //  The 16F7x7 programming spec only mentions this :
    //   > The MCLR pin should be raised from
    //   > below VIL to above the minimum VIHH
    //   > (VPP) within 100 µs of VDD rise.
    //  So : For the 16F7x7, raise VDD, and IMMEDIATELY afterwards also VPP .

    // If this routine is called to write CONFIGURATION MEMORY (and/or ID locations):
    if (   ( dwDeviceAddress >= (uint32_t)PIC_DeviceInfo.lConfMemBase)
            && ( dwDeviceAddress <= (uint32_t)PIC_DeviceInfo.lConfWordAdr) )
    {
        PIC_PRG_LoadConf(dwDeviceAddress, // load the 1st config word and set the PIC's internal address counter
                         pdwSourceData[0]/* or better PicBuf_GetConfigWord(0), or always 0x3FFF ?*/  );
    }


    // Update 2004-12-16:  This algorithm didn't seem to work properly for the
    //                     16F737 - thanks Bob Wilton for this report:
    // > If the data to be loaded is:
    // >  Addr0 - Word0    Addr1 - Word1    Addr2 - Word2  Addr3 - Word3
    // >  Addr4 - Word4    Addr5 - Word5   ....etc
    // > The data read back is:
    // >  Addr0 - Blank    Addr1 - Word1    Addr2 - Word0  Addr3 - Word3
    // >  Addr4 - Word2    Addr5 - Word5    Addr6 - Word4   ....etc
    // Looked into the PIC16F7x7 programming spec (DS30492A) again,
    //  but found nothing obvious. Maybe the Vpp rise was too slow for
    //  "Enter programming mode", so the address counter was not zero
    //  when getting here ?

    for ( iLocationCounter=0; iLocationCounter<n; ++iLocationCounter )
    {
        w = pdwSourceData[iLocationCounter] & dwMask;
        if (! PIC_PRG_iSimulateOnly)
        {
            // Steps 5..8 from Microchip's programming spec for the 16F7x7:
            //      (which LOOKS exactly the same for PIC16F73/PIC16F74/76/77) :
            // > 1. Issue the 'Load Data' command to load a word at the current
            //      (even) program memory address.
            // > 2. Issue an 'Increment Address' command.
            // > 3. Load a word at the current (odd) program memory address
            //      using the 'Load Data' command.
            // > 4. Issue a 'Begin Programming' command.
            // > 5. Wait tprog (about 1 ms)
            // > 6. Issue an 'End Programming' command.
            // > 7. Increment to the next address
            // > 8. Repeat this sequence as required to write
            //      program and configuration memory .
            // The alternative sequence for programming one memory word at a time
            // is as follows (forget it, see "Strange !" below) :
            //       >  1. Set a word for the current memory location using the
            //             'Load Data' command.
            //       > 2. Issue a 'Begin Programming' command.
            //       > 3. Wait tprog (about 1 ms)
            //       > 4. Issue an 'End Programming' command.
            //       > 5. Increment to the next address.
            //       > 6. Repeat this alternative sequence as required
            //            to write program and configuration memory .
            // Strange ! If the "ALTERNATIVE SEQUENCE" really works
            //  programming the 16F7x7 should have worked with the
            //  "old algorithm" but -as reported by someone- it didn't.
            // So WoBu decided NOT to use the ALTERNATIVE SEQUENCE for the 16F737.
            // The actual sequence will be:
            //    LOAD(even) -INCREMENT - LOAD(odd) - BEGIN_PROG - WAIT - END_PROG - INCREMENT
            dwCurrentAddress = dwDeviceAddress + iLocationCounter;
//            PHWInfo.iCurrProgAddress = dwCurrentAddress; // added 2008-05 for the plugin-DLL

            PIC_HW_SerialOut_Command6(ldcmd, false);  // "Load Data Command" (00010bin for PROGRAM)
            PIC_HW_SerialOut_14Bit(w);                // .. immediately followed by data word
//            PicHw_FlushCommand( ldcmd,6,  w,14);
            PIC_HW_SetClockAndData(false, false);

            //  "Two Loads Done ?"  [ future-compatible, WHO KNOWS with how many
            //                        "Words per Programming Cycle" Microchip
            //                        will come up next ?! GRRR ]
            // This only happens at OFF memory addresses (for nWordsPerProgCycle=2).
            // Example: 16F737, "TWO words per programming cycle" -> nWordsPerProgCycle=2 ->
            //  -> if( ( dwCurrentAddress MODULO 2) == 1 )
            if (  ((dwCurrentAddress % nWordsPerProgCycle) == (uint32_t)(nWordsPerProgCycle-1) ) // "odd" memory location ?
                    ||((iLocationCounter+1)==n)  )  // or "last location" ?
            {
                // Send command "Begin Programming Only Command" (16F7x7: ??1000bin = 0x08) :
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg, true );
                PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // "Wait tprog1" (16F818: 1...2 ms)
                // Send command "End Programming Command" (16F7x7: ??1110bin = 0x0E = 14 decimal) :
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg, true );
            }

            // Send command "Increment Address Command" (16F7x7: ??0110bin = 0x06 ) :
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true );
        }
        if ( PIC_PRG_iSimulateOnly )
        {
            wxMilliSleep(10);
        }
        APPL_ShowProgress( (100*iLocationCounter) / n );
        if (APPL_iUserBreakFlag)
            break;
    } // end for(iLocationCounter..)

    if ( ! PIC_PRG_iSimulateOnly )
    {
        PIC_HW_ProgModeOff();   // programming voltage off, clock off, data high-z
        PIC_HW_LongDelay_ms(200); // make sure the programming voltage is discharged
    }

    if (APPL_iUserBreakFlag)
        return false;


    // Verify... belongs to this PIC's "programming algorithm"
    //           because we cannot read directly after writing a single WORD !
    if (! PicPrg_Verify( dwDeviceAddress, // device internal address
                         pdwSourceData,  // pointer to source buffer
                         n,      // count of memory locations to be verified
                         dwMask, // only look at the 14 bits we have written
                         rdcmd)) // read instruction (for verify, here: 0x04)
    {
        fOk = false;
    }

    return fOk/*?*/ ;
} // end PIC_PRG_Program16F7X7()




/***************************************************************************/
/** CODE MEMORY PROGRAMMING for PIC16F818/819 ("four words at a time").
 * Also for PIC16F88; but not sure if this applies to the DATA EEPROM too.
 */
bool PIC_PRG_Program16F81X(
    uint32_t *pdwSourceData,  // pointer to source buffer
    int n,     // count of WORDS to be programmed
    uint32_t dwMask,  // only "1"-bits in the mask may be PROGRAMMED TO ONE and verified
    int ldcmd,  // command word to "LOAD" the data, for example PIC_DeviceInfo.iCmd_LoadProg
    int rdcmd,  // ICSP instruction             like PIC_DeviceInfo.iCmd_ReadProg (for verify)
    uint32_t dwDeviceAddress )  // device specific "base"-address like PIC_DeviceInfo.conf_word_adr
// If this also applies to DATA EEPROM programming in a 16F818
//     is not very clear (as often in the lousy programming specs)
//  Theoretically, the DATA EEPROM block may be organized totally different than the CODE FLASH.
//  Practically,   programming the DATA EEPROM often failed using this method - hmmmm...
//  (take care of lines marked "[lps1]" = "lousy programming spec" !
//
{
    int iLocationCounter, iLoadCounter;
// WORD r;
    uint16_t w;
// WORD wFlags;
    uint16_t dwCurrentAddress;
// int  n_errors = 0;
    bool fOk = true;

    if ( Appl_CheckUserBreak() )
        return false;

    // Caution: The "chip" must have been erased already - which is tricky on a 16F818/819 !
    if (! PIC_PRG_iSimulateOnly)
        PIC_HW_ProgMode();          // first(?) Vdd on, then(?) Vpp on

    iLoadCounter = dwDeviceAddress & 0x000003L;  // -> not necessarily zero (but zero in most cases)
    for ( iLocationCounter=0; iLocationCounter<n; ++iLocationCounter )
    {
        dwCurrentAddress = dwDeviceAddress + iLocationCounter;
//        PHWInfo.iCurrProgAddress = dwCurrentAddress; // added 2008-05 for the plugin-DLL
        w = pdwSourceData[iLocationCounter] & dwMask;
        if (! PIC_PRG_iSimulateOnly)
        {
            // Steps 5..8 from Microchip's programming spec for the 16F818:
            // > 1. Set pointer to row location (YHF: something divideable by 32  ?? )
            // > 2. Issue a 'Begin Erase' command. (YHF: Not done here because device is already erased)
            // > 3. Wait tprog2.
            // > 4. Issue an 'End Programming' command.
            // > 5. Load a word at the current program memory address using the 'Load Data' command.
            // > 6. Issue an 'Increment Address' command.
            // > 7. Load a word at the current program memory address using the 'Load Data' command.
            // > 8. Repeat step 6 and step 7 two times.
            // YHF: The result would be
            //       5. LOAD - 6. INC - 7. LOAD - [8a] 6. INC - 7. LOAD- [8b] 6. INC - 7. LOAD
            //      (FOUR loads but only THREE INC's)
            // > 9. Issue a 'Begin Programming' command to begin programming.
            // > 10. Wait tprog1.
            // > 11. Issue an 'End Programming' command.
            // > 12. Increment to the next address.  ( YHF: A-ha... here's the missing INC)
            // > 13. Repeat steps 5 through 12 seven times (!) to program one row.
            //
            PIC_HW_SerialOut_Command6(ldcmd, false ); // "Load Data Command" (00010bin for PROGRAM, 00011bin for DATA memory)
            PIC_HW_SerialOut_14Bit(w);        // .. immediately followed by data word
//            PicHw_FlushCommand( ldcmd,6,  w,14);
            PIC_HW_SetClockAndData(false, false);

            ++iLoadCounter;
            if (  ((iLoadCounter>=4 || (iLocationCounter+1)==n) )   //  "Four Loads Done ?"
                    || (ldcmd==PIC_DeviceInfo.iCmd_LoadDataDM) // [lps1] NO "Four-Load" question when programming DATA-EEPROM but not CODE-FLASH
                    || (dwCurrentAddress >= PIC_DeviceInfo.lConfMemBase)  // also NO "Four-Loads" when programming CONFIG MEMORY !
                    // 2003-12-28: [lps1] A-HA ! Indeed another LOUSY PROGRAMMING SPEC - Congrats to Microchip...
                    // The "DATA MEMORY" programming may ***NOT*** use the "Four Loads per Programming Command" thingy !
                    // Without the above or-term for the if condition, the first two of three programmed bytes
                    // in the DATA MEMORY of a PIC16F818 *DID* fail programming .
                    // Later, in 2004, Microchip published a better programming specification
                    //    in DS41204C called "PIC12F6XX/16F6XX Memory Programming Specification",
                    //    clearly stating that "FOUR-WORD PROGRAMMING" only works for the CODE MEMORY,
                    //    but neither for DATA nor for CONFIG memory . The same seems to apply to other chips too.
               )
            {
                //  (not clear in Microchips spec: Really "four loads" or "after a 4-word-address-boundary" ?
                //    We may not always start programming at address zero, and there may be less than 4 words !
                //    Guess the important thing is sending the BeginProgOnly command
                //    BEFORE CROSSING the 4-word-boundary. Only a problem if (dwDeviceAddress & 0xFFFC) != 0 !
                //  It later turned out that the above assumption was correct. What counts is
                //   CROSSING THE ADDRESS BOUNDARY, not the "count of four loads.
                //   See PIC_PRG_Program16F7X7(), where the "load counter" was dropped .
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg,true); // "Begin Programming Only Command" (11000bin = 24dec)
                PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // "Wait tprog1" (16F818: 1...2 ms)
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg,true); // "End Programming Command" (10111bin=23dec)
                iLoadCounter = 0;  // wait for another "four loads" now
            }

            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr,true);  // "Increment Address Command"
        }
        if ( PIC_PRG_iSimulateOnly )
        {
            wxMilliSleep(10);
        }
        APPL_ShowProgress( (100*iLocationCounter) / n );
#if(0)
        if (n_errors>16)
        {
            sprintf(PIC_error_string,
                    TE( "Programming aborted after %d errors." ),(int)n_errors);
            APPL_ShowMsg( 0, PIC_error_string);
            break;
        }
#endif
        if (APPL_iUserBreakFlag)
            break;
    } // end for(iLocationCounter..)

    if ( ! PIC_PRG_iSimulateOnly )
    {
        PIC_HW_ProgModeOff();   // programming voltage off, clock off, data high-z
        PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged
    }

    if (APPL_iUserBreakFlag)
        return false;


    // Verify... belongs to the PIC16F818/819's "programming algorithm"
    //           because we cannot read directly after writing a single WORD !
    if (! PicPrg_Verify( dwDeviceAddress, // device internal address
                         pdwSourceData,  // pointer to source buffer
                         n,     // count of memory locations to be verified
                         dwMask,  // only look at the 14 bits we have written
                         rdcmd))  // read instruction (for verify, here: 0x04)
    {
        fOk = false;
    }

    return fOk/*?*/ ;
} // end PIC_PRG_Program16F81X()


/***************************************************************************/
/** CONFIGURATION MEMORY PROGRAMMING for PIC16F818/819 ("four words at a time")
 *  Also for PIC16F88 since 2003-12
 * Based on : "FLOW CHART - PIC16F818/819 Configuration Memory 4.5V <=Vdd < 5.5V"
 */
bool PIC_PRG_ProgramConfigMem_16F81X(
    uint32_t *pdwSourceData,  //!< pointer to source buffer
    int n,     //!< count of WORDS to be programmed
    uint32_t dwMask, //!< only "1"-bits in the mask may be PROGRAMMED TO ONE and verified
    int ldcmd,  //!< command word to "LOAD" the data, for example PIC_DeviceInfo.iCmd_LoadProg
    int rdcmd,  //!< ICSP instruction like PIC_DeviceInfo.iCmd_ReadProg (for verify)
    uint32_t dwDeviceAddress) //!< device specific "base"-address like PIC_DeviceInfo.conf_word_adr
{
    int iAddress = 0;
    int iLoadCounter;
    uint16_t r, w;
// uint16_t wFlags;
    uint32_t dwVerifyMask;
    int  n_errors = 0;
    bool fOk = true;


    if ( Appl_CheckUserBreak() )
        return false;


    if (   ( dwDeviceAddress < (uint32_t)PIC_DeviceInfo.lConfMemBase)
            || ( dwDeviceAddress > (uint32_t)PIC_DeviceInfo.lConfWordAdr) )
    {
        APPL_ShowMsg( 0, _T("Erroneously called ProgramConfigMem_16F81X !")); // not worth translating !
        return false;
    }

    if (! PIC_PRG_iSimulateOnly)
        PIC_HW_ProgMode();          // first(?) Vdd on, then(?) Vpp on


    PIC_HW_SerialOut_Command6( 0x00,false );  // "Load Configuration" = set PC to 0x2000
    PIC_HW_SerialOut_14Bit(0x3FFF);     // ("Load Config" must immediately be followed by something)
//    PicHw_FlushCommand( 0x00,6,  0x3FFF,14);
//    PHWInfo.iCurrProgAddress = iAddress = 0x2000;
    if (dwDeviceAddress == (uint32_t)PIC_DeviceInfo.lConfMemBase) // program ID Location ?
    {
        // Sequence to "program four locations" :
        PIC_HW_SerialOut_Command6( 0x08, true );             // "Begin Erase"
        PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Erase_us);        // "Wait tprog2"
        PIC_HW_SerialOut_Command6( 0x17, true );             // "End Programing"
        for (iLoadCounter=0; iLoadCounter<=3; ++iLoadCounter)
        {
//            PHWInfo.iCurrProgAddress = iAddress;
            PIC_HW_SerialOut_Command6( 0x02, false ); // "Load Data Command"... (guess for PROGRAM + CONFIG memory)
            PIC_HW_SerialOut_14Bit(pdwSourceData[iLoadCounter]);  // .. followed by data word
//            PicHw_FlushCommand( 0x02,6,  pdwSourceData[iLoadCounter],14 );
            PIC_HW_SerialOut_Command6( 0x06, true ); // "Increment Address Command"
            ++iAddress;
        }
        // now "four loads done"
//        PHWInfo.iCurrProgAddress = iAddress;
        PIC_HW_SerialOut_Command6( 0x18, true );     // "Begin Program Only Command" (baah)
        PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us); // "Wait tprog1"
        PIC_HW_SerialOut_Command6( 0x17, true );     // "End Programming Command"
        // now "program four locations" done . Read them back to verify !
        PIC_HW_SerialOut_Command6( 0x00, false );    // "Load Configuration" = set PC to 0x2000
        PIC_HW_SerialOut_14Bit(0x3FFF);              // ("Load Config" must be followed by something)
//        PicHw_FlushCommand( 0x00,6, 0x3FFF,14 );
        iAddress = 0x2000;
        for (iLoadCounter=0; iLoadCounter<=3; ++iLoadCounter)
        {
//            PHWInfo.iCurrProgAddress = iAddress;
            PIC_HW_SerialOut_Command6( 0x04, true );      // "Read Data Command"...
            w = pdwSourceData[iLoadCounter] & dwMask;
            r = PIC_HW_SerialRead_14Bit() & dwMask;       // .. followed by 14 bit (IN)

            // When programming CONFIG REGS, individual masks for verify ARE be required, so:
            dwVerifyMask = PicDev_GetVerifyMaskForAddress( iAddress );
            // (Note: for 16FXX, the device address steps by ONE for each config mem location)

            // If data not correct, report programming failure :
            if ( ( w & dwVerifyMask ) != ( r & dwVerifyMask ) )
            {
                ++n_errors;
                _stprintf(PIC_error_string, _( "Verify Error: %06lX: read %06lX, wanted %06lX" ), iAddress, r, w );
                APPL_ShowMsg( 0, PIC_error_string);
                fOk = false;
            }
            PIC_HW_SerialOut_Command6( PIC_DeviceInfo.iCmd_IncrAddr, true ); // "Increment Address Command"
            ++iAddress;
        }
    } // end if <program and verify ID location ?>

    while (iAddress < 0x2007) // advance PC to 0x2007 (location of 1st config word)
    {
//        PHWInfo.iCurrProgAddress = iAddress;
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true);  // "Increment Address Command"
        ++iAddress;
    }

    if ( (dwDeviceAddress+n) > 0x2007) // also program the configuration word(s) ?
    {
//        PHWInfo.iCurrProgAddress = iAddress;
        w = pdwSourceData[iAddress - dwDeviceAddress] & dwMask;
        if (! PIC_PRG_iSimulateOnly)
        {
            PIC_HW_SerialOut_Command6(ldcmd, false);   // "Load Data Command" (00010bin)
            PIC_HW_SerialOut_14Bit(w);           // .. followed by data word
//            PicHw_FlushCommand( ldcmd,6, w,14);
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg, true); // "Begin Programming Only Command" (11000bin !!)
            PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // "Wait tprog1" (16F818: 1...2 ms)
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg, true); // "End Programming Command" (10111bin !!)
            if ( (dwDeviceAddress+n) > 0x2008) // also program the SECOND configuration word(s) ?
            {
                PIC_HW_SerialOut_Command6( 0x06, true );  // "Increment Address Command"
                ++iAddress;                        // iAddress should be 0x2008 now
//                PHWInfo.iCurrProgAddress = iAddress;
                w = pdwSourceData[iAddress - dwDeviceAddress] & PIC_DeviceInfo.wCfgmask2_used;
                PIC_HW_SerialOut_Command6(ldcmd, false);   // "Load Data Command" (00010bin)
                PIC_HW_SerialOut_14Bit(w);         // .. immediately followed by data word
//                PicHw_FlushCommand( ldcmd,6, w,14 );
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg, true); // "Begin Programming Only Command" (11000bin !!)
                PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // "Wait tprog1" (16F818: 1...2 ms)
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg, true); // "End Programming Command" (10111bin !!)
            } // end if < program SECOND config word >
        }
        else //  PIC_PRG_iSimulateOnly
        {
            wxMilliSleep(10);
        }
        APPL_ShowProgress( 100 );
    } // end if <program CONFIG WORD ? >

    if ( ! PIC_PRG_iSimulateOnly )
    {
        PIC_HW_ProgModeOff();  // programming voltage off, clock off, data high-z
        PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged
    }


    // Verify... belongs to the PIC16F818/819's "programming algorithm"
    //           because we cannot read directly after writing a single WORD !
    if (! PicPrg_Verify( dwDeviceAddress,  // device internal address
                         pdwSourceData,   // pointer to buffer
                         n,       // count of memory locations to be verified
                         dwMask,  // only look at the 14 bits we have written
                         rdcmd))  // read instruction (for verify, here: 0x04)
    {
        fOk = false;
    }

    return fOk/*?*/ ;

} // end PIC_PRG_ProgramConfigMem_16F81X()


/***************************************************************************/
/** CODE MEMORY PROGRAMMING for PIC16F87xA (with "eight memory words at a time")
 * CAUTION: Based on an "Advance Information" document by Microchip
 * and never tested by WoBu, so it may not work at all ! !
 */
bool PIC_PRG_Program16F87XA(
    uint32_t *pdwSourceData,  //!< pointer to source buffer
    int n,     //!< count of WORDS to be programmed
    uint32_t dwMask,  //!< only "1"-bits in the mask may be PROGRAMMED TO ONE and verified
    int ldcmd,  //!< command word to "LOAD" the data, for example PIC_DeviceInfo.iCmd_LoadProg
    int rdcmd,  //!< ICSP instruction like PIC_DeviceInfo.iCmd_ReadProg
    uint32_t dwDeviceAddress) //!< device specific "base"-address like PIC_DeviceInfo.conf_word_adr
{
    int iLocationCounter, iLoadCounter;
// uint16_t r;
    uint16_t w;
// uint16_t wFlags;
//  int  n_errors = 0;
    bool fOk = true;

    if ( Appl_CheckUserBreak() )
        return false;


    // Caution: The "chip" must have been erased already - which is tricky on a 16F818/819 !
    if (! PIC_PRG_iSimulateOnly)
        PIC_HW_ProgMode();          // first(?) Vdd on, then(?) Vpp on

    iLoadCounter = dwDeviceAddress & 0x000003L;  // -> not necessarily zero (but zero in most cases)
    for ( iLocationCounter=0; iLocationCounter<n; ++iLocationCounter )
    {
        w = pdwSourceData[iLocationCounter] & dwMask;
        if (! PIC_PRG_iSimulateOnly)
        {
            // Eight locations must be loaded before every
            // ‘Begin Erase/Programming’ command. After this
            // command is received and decoded, eight words of
            // program memory will be erased and programmed with
            // the values contained in the program data latches. The
            // PC address will decode which eight words are programmed.
            // The lower three bits of the PC are ignored,
            // so if the PC points to address 003h, then all eight
            // locations from 000h to 007h are written.
            // An internal timing mechanism executes an erase
            // before write. The user must allow the combined time
            // for erase and programming, as specified in the electrical
            // specs, for programming to complete. No ‘End
            //  Programming’ command is required.
            PIC_HW_SerialOut_Command6(ldcmd,false);  // "Load Data Command" (00010bin for PROGRAM, 00011bin for DATA memory)
            PIC_HW_SerialOut_14Bit(w);         // .. immediately followed by data word
//            PicHw_FlushCommand( ldcmd,6, w,14 );
            PIC_HW_SetClockAndData(false, false);

            ++iLoadCounter;
            if (  ((iLoadCounter>=8 || (iLocationCounter+1)==n) )   //  "Eight Loads Done ?"
                    || (ldcmd==PIC_DeviceInfo.iCmd_LoadDataDM) // [lps1] NO "Four-Load" question when programming DATA-EEPROM but not CODE-FLASH
               )
            {
                //  (not clear in Microchips spec: Really "four loads" or "after a 4-word-address-boundary" ?
                //    We may not always start programming at address zero, and there may be less than 4 words !
                //    Guess the important thing is sending the BeginProgOnly command
                //    BEFORE CROSSING the 4-word-boundary. Only a problem if (dwDeviceAddress & 0xFFFC) != 0    !
                PIC_HW_SerialOut_Command6(0x08, true);        // "Begin Erase / Programming Cycle" (01000bin = 24dec)
                PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // "Wait tprog1"
                PIC_HW_SerialOut_Command6(0x17, true);        // "End Programming Command" (10111bin)
                iLoadCounter = 0;  // wait for another "eight loads" now
            }

            PIC_HW_SerialOut_Command6(0x06, true);  // "Increment Address Command"
        }
        if ( PIC_PRG_iSimulateOnly )
        {
            wxMilliSleep(10);
        }
        APPL_ShowProgress( (100*iLocationCounter) / n );
    } // end for(iLocationCounter..)

    if ( ! PIC_PRG_iSimulateOnly )
    {
        PIC_HW_ProgModeOff();   // programming voltage off, clock off, data high-z
        PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged
    }


    // Verify... belongs to the PIC16F87xAs "programming algorithm"
    //           because we cannot read directly after writing a single WORD !
    if (! PicPrg_Verify( dwDeviceAddress,  // device internal address
                         pdwSourceData,   // pointer to buffer
                         n,       // count of memory locations to be verified
                         dwMask,  // only look at the 14 bits we have written
                         rdcmd))  // read instruction (for verify, here: 0x04)
    {
        fOk = false;
    }

    return fOk/*?*/ ;
} // end PIC_PRG_Program16F87XA()


/***************************************************************************/
/** CONFIGURATION MEMORY PROGRAMMING for PIC16F87xA (with "four memory words at a time")
 * CAUTION: SLIGHTLY DIFFERENT FROM PIC16F818 !!! (  grrrrrrrrrrrrrr )
 * Based on : "FLOW CHART - PIC16F87XA Configuration Memory 2.0V <=Vdd < 5.5V"
 */
bool PIC_PRG_ProgramConfigMem_16F87XA(
    uint32_t *pdwSourceData,  //!< pointer to source buffer
    int n,     //!< count of WORDS to be programmed
    uint32_t dwMask,  //!< only "1"-bits in the mask may be PROGRAMMED TO ONE and verified
    int ldcmd,  //!< command word to "LOAD" the data, for example PIC_DeviceInfo.iCmd_LoadProg
    int rdcmd,  //!< ICSP instruction like PIC_DeviceInfo.iCmd_ReadProg (for verify)
    uint32_t dwDeviceAddress) //!< device specific "base"-address like PIC_DeviceInfo.conf_word_adr
{
    int iAddress, iLoadCounter;
    uint16_t r, w;
// uint16_t wFlags;
    uint32_t dwVerifyMask;
    int  n_errors = 0;
    bool fOk = true;

    if ( Appl_CheckUserBreak() )
        return false;


    if (   ( dwDeviceAddress < (uint32_t)PIC_DeviceInfo.lConfMemBase)
            || ( dwDeviceAddress > (uint32_t)PIC_DeviceInfo.lConfWordAdr) )
    {
        APPL_ShowMsg( 0, _("Erroneously called ProgramConfigMem_16F87XA !"));
        return false;
    }

    if (! PIC_PRG_iSimulateOnly)
        PIC_HW_ProgMode();          // first(?) Vdd on, then(?) Vpp on


    PIC_HW_SerialOut_Command6(0x00, false); // "Load Configuration" = set PC to 0x2000
    PIC_HW_SerialOut_14Bit(0x3FFF);  // ("Load Config" must be followed by something)
//    PicHw_FlushCommand( 0x00,6,  0x3FFF,14 ); // added 2008-05-19
    iAddress = 0x2000;
    if (dwDeviceAddress == (uint32_t)PIC_DeviceInfo.lConfMemBase) // program ID Location ?
    {
        // Sequence to "program four locations" (16F87xA, configuration memory):
        for (iLoadCounter=0; iLoadCounter<=3; ++iLoadCounter)
        {
//            PHWInfo.iCurrProgAddress = iAddress;
            PIC_HW_SerialOut_Command6(0x02, false);   // "Load Data Command"... (guess for PROGRAM + CONFIG memory)
            PIC_HW_SerialOut_14Bit(pdwSourceData[iLoadCounter]);  // .. followed by data word
//            PicHw_FlushCommand( 0x02,6, pdwSourceData[iLoadCounter],14 );
            PIC_HW_SerialOut_Command6(0x06, true);   // "Increment Address Command"
            ++iAddress;
        }
        // now "four loads done"
//        PHWInfo.iCurrProgAddress = iAddress;
        PIC_HW_SerialOut_Command6(0x08, true);       // "Begin Erase / Program Command" (baah)
        PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us); // "Wait tprog1"  (here: 8000 us)
        // no "End Programming Command" for the 16F87xA at this place ! ! ! !
        // now "program four locations" done . Read them back to verify !
        PIC_HW_SerialOut_Command6( 0x00, false ); // "Load Configuration" = set PC to 0x2000
        PIC_HW_SerialOut_14Bit(0x3FFF);           // ("Load Config" must be followed by something)
//        PicHw_FlushCommand( 0x00,6, 0x3FFF,14 );

        iAddress = 0x2000;
        for (iLoadCounter=0; iLoadCounter<=3; ++iLoadCounter)
        {
//            PHWInfo.iCurrProgAddress = iAddress;
            PIC_HW_SerialOut_Command6( 0x04, true );  // "Read Data Command"...
            w = pdwSourceData[iLoadCounter] & dwMask;
            r = PIC_HW_SerialRead_14Bit() & dwMask;   // .. followed by 14 bit (IN)

            // When programming CONFIG REGS, individual masks for verify MAY be required, so:
            dwVerifyMask = PicDev_GetVerifyMaskForAddress( iAddress );
            // (Note: for 16FXX, the device address steps by ONE for each config mem location)

            if ( ( w & dwVerifyMask ) != ( r & dwVerifyMask ) )
            {
                // if data not correct, report programming failure
                ++n_errors;
                _stprintf(PIC_error_string, _( "Verify Error: %06lX: read %06lX, wanted %06lX" ), iAddress, r, w );
                APPL_ShowMsg( 0, PIC_error_string);
                fOk = false;
            }
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true); // "Increment Address Command"
            ++iAddress;
        }
    } // end if <program and verify ID location ?>

    while (iAddress < 0x2007) // advance PC to 0x2007 (location of 1st config word)
    {
//        PHWInfo.iCurrProgAddress = iAddress;
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true);  // "Increment Address Command"
        ++iAddress;
    }

    if ( (dwDeviceAddress+n) > 0x2007L) // also program the configuration word(s) ?
    {
        w = pdwSourceData[iAddress - dwDeviceAddress] & dwMask;
        if (! PIC_PRG_iSimulateOnly)
        {
//            PHWInfo.iCurrProgAddress = iAddress;
            PIC_HW_SerialOut_Command6(ldcmd, false); // "Load Data Command" (00010bin)
            PIC_HW_SerialOut_14Bit(w);        // .. immediately followed by data word
//            PicHw_FlushCommand( ldcmd,6,  w,14 );
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg,true); // "Begin Programming Only Command" (11000bin !!)
            PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // "Wait tprog1" (16F818: 1...2 ms)
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg,true); // "End Programming Command" (10111bin !!)
            if ( (dwDeviceAddress+n) > 0x2008) // also program the SECOND configuration word(s) ?
            {
                PIC_HW_SerialOut_Command6(0x06,true);  // "Increment Address Command"
                ++iAddress;                       // iAddress should be 0x2008 now
//                PHWInfo.iCurrProgAddress = iAddress;
                w = pdwSourceData[iAddress - dwDeviceAddress] & PIC_DeviceInfo.wCfgmask2_used;
                PIC_HW_SerialOut_Command6(ldcmd, false); // "Load Data Command" (00010bin)
                PIC_HW_SerialOut_14Bit(w);        // .. immediately followed by data word
//                PicHw_FlushCommand( ldcmd,6,  w,14 );
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg,true); // "Begin Programming Only Command" (11000bin !!)
                PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // "Wait tprog1" (16F818: 1...2 ms)
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg,true); // "End Programming Command" (10111bin !!)
            } // end if < program SECOND config word >
        }
        else //  PIC_PRG_iSimulateOnly
        {
            wxMilliSleep(10);
        }
        APPL_ShowProgress( 100 );
    } // end if <program CONFIG WORD ? >

    if ( ! PIC_PRG_iSimulateOnly )
    {
        PIC_HW_ProgModeOff();  // programming voltage off, clock off, data high-z
        PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged
    }


    // Verify... belongs to the PIC16F818/819's "programming algorithm"
    //           because we cannot read directly after writing a single WORD !
    if (! PicPrg_Verify( dwDeviceAddress, // device internal address
                         pdwSourceData,   // pointer to buffer
                         n,      // count of memory locations to be verified
                         dwMask, // only look at the 14 bits we have written
                         rdcmd)) // read instruction (for verify, here: 0x04)
    {
        fOk = false;
    }

    return fOk/*?*/ ;

} // end PIC_PRG_ProgramConfigMem_16F87XA()


/***************************************************************************/
/** Program one row at a time
 *    The row size must be given in nWordsPerRow
 */
bool PIC_PRG_ProgramRow(
    uint32_t *pdwSourceData,  //!< pointer to source buffer
    int n,     //!< count of WORDS to be programmed
    int nWordsPerRow, //! number of words per row
    uint32_t dwMask,  //!< only "1"-bits in the mask may be PROGRAMMED TO ONE and verified
    int ldcmd,  //!< command word to "LOAD" the data, for example PIC_DeviceInfo.iCmd_LoadProg
    int rdcmd,  //!< ICSP instruction like PIC_DeviceInfo.iCmd_ReadProg (for verify)
    uint32_t dwDeviceAddress) //!< device specific "base"-address like PIC_DeviceInfo.conf_word_adr
{
    uint16_t w;
    bool fOk = true;

    if ( Appl_CheckUserBreak() )
        return false;


    if (   ( dwDeviceAddress >= (uint32_t)PIC_DeviceInfo.lConfMemBase)
            && ( dwDeviceAddress <= (uint32_t)PIC_DeviceInfo.lConfWordAdr) )
    {
        PIC_PRG_LoadConf(dwDeviceAddress, // load the 1st config word and set the PIC's internal address counter
                         pdwSourceData[0]/* or better PicBuf_GetConfigWord(0), or always 0x3FFF ?*/  );
    }
    else
        //-- When not writting config the number of word written must be multiple of the row size
        n = ((n-1)/nWordsPerRow + 1)*nWordsPerRow;

    if (! PIC_PRG_iSimulateOnly)
        PIC_HW_ProgMode();          // first(?) Vdd on, then(?) Vpp on

    for (int i=0; i<n; ++i )
    {
//        uint32_t Address = dwDeviceAddress + i;
        w = pdwSourceData[i] & dwMask;

        if (! PIC_PRG_iSimulateOnly)
        {
            PIC_HW_SerialOut_Command6(ldcmd, false); //-- Load command
            PIC_HW_SerialOut_14Bit(w);        //-- .. followed by data word

            if ((i+1) % nWordsPerRow == 0)
            {
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg, true );
                PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);
//                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg, true );
            }
            // Send command "Increment Address Command" (16F7x7: ??0110bin = 0x06 ) :
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true );

            if (APPL_iUserBreakFlag)
                break;
        }
        else
            PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);

        APPL_ShowProgress( (100*i) / n );
    }

    if ( ! PIC_PRG_iSimulateOnly )
    {
        PIC_HW_ProgModeOff();  // programming voltage off, clock off, data high-z
        PIC_HW_LongDelay_ms(200);  // make sure the programming voltage is discharged
    }

    // Verify... belongs to this PIC's "programming algorithm"
    //           because we cannot read directly after writing a single WORD !
    if (! PicPrg_Verify( dwDeviceAddress, // device internal address
                         pdwSourceData,  // pointer to source buffer
                         n,      // count of memory locations to be verified
                         dwMask, // only look at the 14 bits we have written
                         rdcmd)) // read instruction (for verify, here: 0x04)
        fOk = false;

    return fOk;

} // end PIC_PRG_ProgramRow()



/***************************************************************************/
void PIC_PRG_Eprom14ProgCycle(uint16_t w)
{
    if ( PIC_PRG_iSimulateOnly )
    {
        wxMilliSleep(5);
        return;
    }

    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadProg,false); // now included in "..ProgCycle"
    PIC_HW_SerialOut_14Bit(w);  // "load"-command immediately followed by data
//    PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadProg,6,  w,14 );
    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg,true); // time critical !
    PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // program delay (usually 0.1 ms)
    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg, true);  // time critical !
    PIC_HW_SetClockAndData(false, false);
} // end PIC_PRG_Eprom14ProgCycle()


/***************************************************************************/
/** Stoneage routine for programming EPROM-based PICs (no FLASH).
 * NEVER TESTED FOR YEARS !
 */
bool PIC_PRG_ProgramEpromMcu(
    uint32_t *pdwSourceData, //!< pointer to source buffer
    int iCountOfWords,  //!< count of WORDS to be programmed
    uint32_t dwMask,  //!< only "1"-bits in the mask may be PROGRAMMED TO ONE and verified
    uint32_t dwDeviceAddress) //!< device specific "base"-address like PIC_DeviceInfo.lConfWordAdr
{
    int  i,j,n;
    uint16_t r, w, wFlags;
    int  n_errors = 0;
    bool fOk = true;

    if ( Appl_CheckUserBreak() )
        return false;
    // Note: for an EEPROM MCU, we do *NOT* break out by user command after this,
    //    because an EEPROM / OTP - based PIC would go straight into the waste bin.

    if ( PIC_PRG_iSimulateOnly )
    {
        APPL_ShowMsg( 0, _("Simulated programming of PIC failed"));
        return false;
    }

    PIC_HW_ProgMode();          // first(?) Vdd on, then(?) Vpp on

    if (   ( dwDeviceAddress >= (uint32_t)PIC_DeviceInfo.lConfMemBase)
            && ( dwDeviceAddress <= (uint32_t)PIC_DeviceInfo.lConfWordAdr) )
    {
        PIC_PRG_LoadConf(dwDeviceAddress, // load the 1st config word and set the PIC's internal address counter
                         pdwSourceData[0]/* or better PicBuf_GetConfigWord(0), or always 0x3FFF ?*/  );
    }

    for ( i=0; i<iCountOfWords; ++i )
    {
        w = pdwSourceData[i] & dwMask;
        n = 0;
        do
        {
            PIC_PRG_Eprom14ProgCycle(w);
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_ReadProg, true);
            r = PIC_HW_SerialRead_14Bit() & dwMask;   // read back, data correct ?
            ++n;
        }
        while ( (r!=w) && (n<25) ); // try up to 25 times before giving up !
        if (r != w)
        {
            // this cell does not want to be programmed ?  grumble.. another chip please :-(
            ++n_errors;
            _stprintf(PIC_error_string, _( "Verify Error: %06lX: read %06lX, wanted %06lX" ), dwDeviceAddress+i, r, w );
            APPL_ShowMsg( 0, PIC_error_string);
            fOk = false;

            wFlags = PicBuf_GetMemoryFlags(dwDeviceAddress+i);
            if (wFlags & PIC_BUFFER_FLAG_PRG_ERROR)
                wFlags |=PIC_BUFFER_FLAG_DEAD;
            else
                wFlags |=PIC_BUFFER_FLAG_PRG_ERROR;
            PicBuf_SetMemoryFlags(dwDeviceAddress+i,  wFlags);

            // this is an EPROM-based, most likely OTP chip, so lets go on programming
            // maybe the chip can be used if we carry on
        } // end if (r != w) after 25 programming "pulses"
        else
        {
            // looks good, apply (3*n) additional program cycles
            for (j = 0; j<3*n; ++j)
                PIC_PRG_Eprom14ProgCycle(w);
        }

        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true);
        APPL_ShowProgress( (100*i) / iCountOfWords );
        if (n_errors>20)
        {
            _stprintf(PIC_error_string, _( "Programming aborted after %d errors." ), n_errors);
            APPL_ShowMsg( 0, PIC_error_string);
            break;
        }
    } // end for(i..)
    PIC_HW_ProgModeOff();           // programming voltage off, clock off, data high-z
    return fOk/*?*/ ;
} // end ..ProgramEpromMcu()


/***************************************************************************/
/** Universal PIC programming subroutine.
 * Decides which PIC type, which memory type, and which algorithm to use.
 */
bool PIC_PRG_Program(
    uint32_t *pdwSourceData,  //!< pointer to source data (array of DWORDs)
    int n,      //!< count of WORDS to be programmed
    int nWordsPerRow, //! number of words per row
    uint32_t dwMask, //!< only "1"-bits in the mask may be PROGRAMMED TO ONE and verified
    int ldcmd,  //!< command word to "LOAD" the data, for example PIC_DeviceInfo.iCmd_LoadProg
    int rdcmd,  //!< ICSP instruction like PIC_DeviceInfo.iCmd_ReadProg (for verify)
    uint32_t dwDeviceAddress) //!< device specific "base"-address like PIC_DeviceInfo.lConfWordAdr
{
// int  i;
    uint16_t wProgAlgo;
    bool fResult = true;
    enum { MT_CONFIG, MT_DATA, MT_CODE } eMemType;


    if ( Appl_CheckUserBreak() )
        return false;

    if ( ! PicHw_iConnectedToTarget )
    {
        PIC_HW_ConnectToTarget();    // connect to target if not already done
    }
    // call PIC_HW_DisconnectFromTarget() LATER when the USER user wants it !

    // What memory type are we going to program ? (DATA EEPROM, CONFIG MEMORY or CODE MEMORY)
    if (    (dwDeviceAddress >= (uint32_t)PIC_DeviceInfo.lConfMemBase)
            && (dwDeviceAddress <= (uint32_t)PIC_DeviceInfo.lConfWordAdr)  )
    {
        eMemType = MT_CONFIG;
        wProgAlgo = PIC_DeviceInfo.wConfigProgAlgo; // use programming algorithm for CONFIG MEMORY
    }
    else
        if (    (dwDeviceAddress >= (uint32_t)PIC_DeviceInfo.lDataMemBase)
                && (dwDeviceAddress <  (uint32_t)PIC_DeviceInfo.lDataMemBase + (uint32_t)PIC_DeviceInfo.lDataEEPROMSizeInByte )  )
        {
            eMemType = MT_DATA;
            wProgAlgo = PIC_DeviceInfo.wDataProgAlgo;    // programming algorithm for DATA EEPROM memory
        }
        else  // neither CONFIGURATION MEMORY  nor  DATA MEMORY,   must be   CODE MEMORY :
        {
            eMemType = MT_CODE;
            wProgAlgo = PIC_DeviceInfo.wCodeProgAlgo;    // programming algorithm for CODE MEMORY
        }

    if (TSessionConfig::GetVerboseMessages())
    {
        APPL_ShowMsg( 0, _("Programming 0x%06lX..0x%06lX, algo=\"%hs\", CanRead=%d ."),
                      dwDeviceAddress,
                      dwDeviceAddress+n-1,
                      PicDev_AlgorithmCodeToString(wProgAlgo),
                      PIC_DeviceInfo.wCanRead );
    }

    switch (wProgAlgo) // which algorithm must be used to program this memory type ?
    {
    case PIC_ALGO_UNKNOWN:
    case PIC_ALGO_16FXX  :  // 16F84, 16F628 (but not -A), and many others
    case PIC_ALGO_12FXX  :  // PIC12F629, PIC12F675
    default:
        return PIC_PRG_Program16FXX( pdwSourceData, n, dwMask, ldcmd, rdcmd, dwDeviceAddress );

    case PIC_ALGO_16FXX_OLD_ERASE  :  // 16F84, 16F628 (but not -A), and many others
        APPL_ShowMsg( 0, _("WARNING! PIC_ALGO_16FXXOLD_ERASE algo should not be used for programming!"));
        return PIC_PRG_Program16FXX( pdwSourceData, n, dwMask, ldcmd, rdcmd, dwDeviceAddress );

    case PIC_ALGO_16F630 :  // 16F630 (different from 16F628, never tested)
        switch (eMemType)
        {
        case MT_CONFIG:
            return PIC_PRG_Program16FXX( pdwSourceData, n, dwMask, ldcmd, rdcmd, dwDeviceAddress ); // ??
        case MT_DATA:
            return PIC_PRG_Program16FXX( pdwSourceData, n, dwMask, ldcmd, rdcmd, dwDeviceAddress ); // ???
        case MT_CODE:
            return PIC_PRG_ProgramCode16F630(pdwSourceData, n, dwMask, dwDeviceAddress );
        default:
            return false;
        }

    case PIC_ALGO_MULTI_WORD :
        switch (eMemType)
        {
        case MT_CONFIG:
            return PIC_PRG_ProgramRow( pdwSourceData, n, nWordsPerRow, dwMask, ldcmd, rdcmd, dwDeviceAddress ); // ??
        case MT_DATA:
            return PIC_PRG_ProgramRow( pdwSourceData, n, nWordsPerRow, dwMask, ldcmd, rdcmd, dwDeviceAddress ); // ???
        case MT_CODE:
            return PIC_PRG_ProgramRow( pdwSourceData, n, nWordsPerRow, dwMask, ldcmd, rdcmd, dwDeviceAddress );
        default:
            return false;
        }

    case PIC_ALGO_PIC16F7X: /* added 2005-08-21, because the PIC16F74 was too different.. */
        APPL_ShowMsg( 0, _("ERROR ! PIC_PRG_Program() must not be called for PIC16F73/74/76/77 !"));
        return false;

    case PIC_ALGO_PIC16F716: /* added 2005-12-03, ANOTHER "SPECIAL" CASE - grumble ;-) */
        switch (eMemType)
        {
        case MT_CONFIG:
            fResult = PIC16F716_WriteConfigMemory( pdwSourceData, n, dwDeviceAddress );
            if ( !fResult )
                return false;
            // Verify CONFIG MEMORY if not done yet :
            if (  (PicPrg_iConfMemProgrammed  && !PicPrg_iConfMemVerified)
                    ||(PicPrg_iConfWordProgrammed && !PicPrg_iConfWordVerified)
               )
            {
                fResult = PicPrg_Verify( dwDeviceAddress,
                                         pdwSourceData,  // pointer to source buffer
                                         n,       // count of memory locations to be verified
                                         0x3FFF,  // only look at 14 bits (or even less)
                                         rdcmd);  // read instruction (for verify, here: 0x04)
                if ( ! fResult )
                {
                    // VERIFYING (not PROGRAMMING) the config memory often failed
                    // when doing this *immediately after* writing the code memory.
                    // The ID LOCATIONS all read zero then, but when verifying *later*
                    // they turned out to be properly written ! !
                    APPL_ShowMsg( 0, _("Trying again to verify PIC16F716 code memory..."));
                    // This error may be related to supply voltage problems
                    // in the "JDM 2"-interface, but it also occurred with a "COM84"-
                    // interface .   Tried this as a cure:  Verify a second time !
                    PIC_HW_ProgModeOff();  // try to kick this damned thing alive...
                    PIC_HW_ProgMode();     // (which didn't help either)
                    fResult = PicPrg_Verify( dwDeviceAddress,  // TRY AGAIN (yucc...)
                                             pdwSourceData, n, 0x3FFF, rdcmd);
                    if ( !fResult )
                    {
                        APPL_ShowMsg( 0, _("Please try to verify PIC16F716 again (it's STRANGE) !"));
                    }
                }
            }
            return fResult;
        case MT_DATA:
            APPL_ShowMsg( 0, _("ERROR ! There is no DATA MEMORY in a PIC16F716 !"));
            return false;
        case MT_CODE:
            if ( !  PIC16F716_WriteCodeMemory( pdwSourceData, n, dwDeviceAddress ) )
                return false;
            // Verify CODE MEMORY in the PIC16F716 now (not included above):
            return PicPrg_Verify( 0, // start address for code memory
                                  pdwSourceData,  // pointer to source buffer
                                  n,       // count of memory locations to be verified
                                  0x3FFF,  // only look at the 14 bits we have written
                                  0x04 );  // read instruction (for verify, here: 0x04)
        default:
            break;
        } // end switch(eMemType) for PIC_ALGO_PIC16F716
        return false;

    case PIC_ALGO_16F7X7 :  // 16F737 (added by WoBu 2004-07, grumbling again about TWO locations per prog cycle)
        switch (eMemType)
        {
        case MT_CONFIG:
            return PIC_PRG_Program16F7X7(
                       pdwSourceData,  // pointer to source buffer
                       n,    // count of WORDS to be programmed
                       2,    // int nWordsPerProgCycle, here: 2 "words per programming cycle" (for 16F737)
                       dwMask, ldcmd, rdcmd, dwDeviceAddress );
        case MT_DATA:
            return PIC_PRG_Program16FXX( pdwSourceData, n, dwMask, ldcmd, rdcmd, dwDeviceAddress ); // ??  There is no EEPROM in a PIC16F737 !!
        case MT_CODE:
            return PIC_PRG_Program16F7X7(
                       pdwSourceData,  // pointer to source buffer
                       n,    // count of WORDS to be programmed
                       2,    // int nWordsPerProgCycle, here: 2 "words per programming cycle" (for 16F737)
                       // Caution, the 16F716 uses FOUR words per programming cycle !
                       dwMask, ldcmd, rdcmd, dwDeviceAddress );
        default:
            return false;
        }

    case PIC_ALGO_16F81X :  // 16F818 (kindly donated by Johan SM6LKM),   16F819
        return PIC_PRG_Program16F81X( pdwSourceData, n, dwMask, ldcmd, rdcmd, dwDeviceAddress );

    case PIC_ALGO_16F87X:   // 16F87x ( WITHOUT 'A', these are OLD DEVICES !)
        // Difference to 16F84: here: first programm ALL, then verify ALL .
        return PIC_PRG_Program16F87X( pdwSourceData, n, dwMask, ldcmd, rdcmd, dwDeviceAddress );

    case PIC_ALGO_16F87XA:  // 16F87xA not tested in 2003-09-19 (lacking a sample)
        return PIC_PRG_Program16F87XA( pdwSourceData, n, dwMask, ldcmd, rdcmd, dwDeviceAddress );

    case PIC_ALGO_16CXX:   // EPROM-based PICs: 16C61, 16C71, 16C710, 16C711, 16C715
        return PIC_PRG_ProgramEpromMcu( pdwSourceData, n, dwMask, dwDeviceAddress );

    case PIC_ALGO_DATA_EEPROM_16XXX: // DATA(!) EEPROM programming algorithm for almost every PIC:
        return PIC_PRG_Program16FXX( pdwSourceData, n, dwMask, ldcmd, rdcmd, dwDeviceAddress ); // same as CODE(!) programming in a 16F84

    case PIC_ALGO_CONFIG_MEM_16XXX:  // CONFIGURATION(!) MEMORY programming algorithm for many PICs:
        // (originally the same as CODE MEMORY programming for a PIC16F84 )
        return PIC_PRG_Program16FXX( pdwSourceData, n, dwMask, ldcmd, rdcmd, dwDeviceAddress ); // same as CODE(!) programming in a 16F84

    case PIC_ALGO_CONFIG_MEM_16F81X: // CONFIGURATION(!) MEMORY programming algorithm for 16F818/819:
        return PIC_PRG_ProgramConfigMem_16F81X( pdwSourceData, n, dwMask, ldcmd, rdcmd, dwDeviceAddress );

    case PIC_ALGO_CONFIG_MEM_16F87XA: // slightly different for 16F87xA (tested with 16F873A) :
        return PIC_PRG_ProgramConfigMem_16F87XA( pdwSourceData, n, dwMask, ldcmd, rdcmd, dwDeviceAddress );

    case PIC_ALGO_dsPIC30F:           // totally different algorithm for dsPIC30Fxxxx :
    {
        switch ( eMemType )
        {
        case MT_CODE :
            return dsPIC_WriteCodeMemory( dwDeviceAddress, pdwSourceData, n/*dwNrOfLocations*/ );
        default :      // everything else cannot be programmed yet !
            return false;
        }
    } // end case PIC_ALGO_dsPIC30F ("program")

    case PIC_ALGO_PIC18F:     // erase PIC18Fxxxx, separate module by M.v.d. Werff
//#ifdef SUPPORT_PIC18F
        {
            switch ( eMemType )
            {
            case MT_CODE  :
                return PIC18F_WriteCodeMemory( dwDeviceAddress, pdwSourceData, n/*dwNrOfLocations*/ );
            case MT_CONFIG:
                return PIC18F_WriteConfigRegs( dwDeviceAddress, pdwSourceData, n/*dwNrOfLocations*/ );
            case MT_DATA  :
                return PIC18F_WriteDataMemory( dwDeviceAddress, pdwSourceData, n/*dwNrOfLocations*/ );
            }
        }
//#else       // PIC18F not supported by this variant
        return false;
//#endif // SUPPORT_PIC18F
        break;  // end case PIC_ALGO_PIC18F ("program")

    case PIC_ALGO_PIC10F:  /* added 2005-05-01, support for PIC10F20x by W.Buescher    */
    {
        return false;     // totally different subroutine, must be called directly from user interface !
    } // end case PIC_ALGO_PIC10F ("program")

    }
    return false;    // arrived here: something wrong, missing implementation ?

} // end ..Program()


/***************************************************************************/
/** Writes DATA memory (=EEPROM) for various PIC types.
 * Supports PIC16F,  PIC18F,  dsPIC30F.
 */
bool PicPrg_WriteDataMemory(void)
{
    T_PicPrg_ReadWriteFunc *pvReader, *pvWriter;
    bool fResult = false;

    if ( PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex >= 0 )
    {
        PicPrg_iDataMemVerified = 0;  // data memory has not been verified yet..

        // If there is a specialized routine for the selected chip, use it,
        // otherwise use the "old" routine for PIC's with 14-bit core .
        if ( PicPrg_GetReadWriteFunctionPtr( PIC_DeviceInfo.lDataMemBase, &pvReader, &pvWriter ) )
        {
            fResult= pvWriter( PIC_DeviceInfo.lDataMemBase, // dwDestAddress,
                               PicBuf[PIC_BUF_DATA].pdwData , // uint32_t *pdwSourceData,
                               PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex+1); // dwNrOfLocations
        }
        else // not one of the "new" functions (since 2005),  assume 14-bit like PIC16Fxxx :
        {
            fResult= PIC_PRG_Program( PicBuf[PIC_BUF_DATA].pdwData, // source pointer (one BYTE per location)
                                      PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex+1,  // count of words
                                      PIC_DeviceInfo.lCodeMemEraseLatchSize, //-- Reuse the Erase buffer size for Data Row Size
                                      0xFF,                            // mask for programming and VERIFYING(!) BYTES
                                      PIC_DeviceInfo.iCmd_LoadDataDM,  // command to load data (usually 0x03)
                                      PIC_DeviceInfo.iCmd_ReadDataDM,  // command to read data (usually 0x05)
                                      PIC_DeviceInfo.lDataMemBase) ;   // base address (usually 8448dec = 0x2100 for DATA)
            // Note: this old routine may already VERIFY the cells while writing.
            //  In that case, PicPrg_iDataMemVerified will already be non-zero now .
        } // end else  ( -> PICs with 14-bit code )
    } // end if( PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex >= 0 )
    else  // shall "PROGRAM DATA" but there is nothing LOADED....
    {
        APPL_ShowMsg( 0, _("Cannot program DATA MEMORY, nothing in buffer.") );
    }

    return fResult;  // true = ok,   false = error .
} // end PicPrg_WriteDataMemory()

/***************************************************************************/
/** Writes CONFIG memory for various PIC types.
 * Supports PIC16F,  PIC18F,  dsPIC30F.
 */
bool PicPrg_WriteConfigRegs(
    uint32_t dwDestAddress,   //!< destination address inside the chip
    uint32_t *pdwSourceData,  //!< pointer to source buffer
    uint32_t dwNrOfRegisters) //!< number of registers to be written
{
    T_PicPrg_ReadWriteFunc *pvReader, *pvWriter;

    // If there is a specialized routine for the selected chip, use it,
    // otherwise use the "old" routine for PIC's with 14-bit core .
    if ( PicPrg_GetReadWriteFunctionPtr( dwDestAddress, &pvReader, &pvWriter ) )
    {
        return pvWriter( dwDestAddress, pdwSourceData, dwNrOfRegisters );
    }
    else // not one of the "new" functions (since 2005),  assume 14-bit like PIC16Fxxx :
    {
        if ( dwNrOfRegisters>1 )
        {
            PicPrg_iConfMemVerified = 0; // config memory not verified yet
            // (it MAY be verified within PIC_PRG_Program(), or maybe not.. )
        }
        if (   (PIC_DeviceInfo.lConfWordAdr >= (long)dwDestAddress )
                && (PIC_DeviceInfo.lConfWordAdr < (long)(dwDestAddress+dwNrOfRegisters) ) )
        {
            // memory range includes "config word" :
            PicPrg_iConfWordVerified = 0;  // config word not verified yet
        }
        return PIC_PRG_Program( pdwSourceData,
                                dwNrOfRegisters, // number of words to be programmed
                                4,
                                0x3FFF,          // mask for programming and verification
                                PIC_DeviceInfo.iCmd_LoadProg,  // 'load' command pattern
                                PIC_DeviceInfo.iCmd_ReadProg,  // 'read' command pattern (required for verifying)
                                dwDestAddress); // target start address
    }
} // end PicPrg_WriteConfigRegs()


/***************************************************************************/
/** Verifies the contents of a certain memory area for various PIC types.
 * Supports PIC16F,  PIC18F,  dsPIC30F.
 */
bool PicPrg_Verify(
    uint32_t dwDeviceAddress, //!< destination address inside the chip
    uint32_t *pdwSourceData,  //!< source buffer, may be NULL (calling access routine then)
    uint32_t dwNrOfLocations, //!< number of memory locations to be verified
    uint32_t dwMask,          //!< bitmask, like 0x03FFF for 14-bit memory
    int rdcmd)             //!< ICSP instruction to read this memory type
// Since 2005-08-21:
//  pdwSourceData=NULL means " there is no pointer to a source buffer,
//                             call PicBuf_GetBufferWord instead "   .
//              This simplified the implementation of PIC16F7x_PRG.C .
// Side effects:
//   PicPrg_iCodeMemVerified, PicPrg_iDataMemVerified,
//   PicPrg_iConfMemVerified, PicPrg_iConfWordVerified may be modified here .
{
    int i;
    uint32_t dwAddressFactor; // device-dependent relationship between buffer index and display address
    uint32_t dwDeviceAddress2;
    uint16_t  wFlags;
    uint32_t r, w, dwMask2;
    uint32_t dwSourceFromBuffer;
    uint32_t *pdwTempBuf;
    T_PicBufferInfo * pBufInfo;
    T_PicPrg_ReadWriteFunc *pvReader;
    wxChar  *cp;
    int  nGoodConfigMemLocs = 0;
    int  n_errors = 0;

    // "Decide" which buffer must be used for the specified target address range:
    pBufInfo = PicBuf_TargetAddressToBufPtr(
                   dwDeviceAddress, // input:  "device address" as seen by target
                   NULL  );         // optional result (array index) not needed here

    if (    Appl_CheckUserBreak()
            || (dwNrOfLocations==0) || (dwNrOfLocations>PIC_BUF_CODE_SIZE)
            || (pBufInfo==NULL) )
        return false;

    // Note: The address may step by TWO for each 'location', depending on the device !
    dwAddressFactor = pBufInfo->dwAddressFactor;  // how many "address steps per location" ?
    APPL_ShowMsg( 0, _( "Verifying 0x%06lX..0x%06lX" ) ,
                  dwDeviceAddress,
                  (dwDeviceAddress + (dwNrOfLocations-1)*dwAddressFactor) );

    if ( PIC_PRG_iSimulateOnly )
    {
#if(0)
        APPL_ShowMsg( 0, _("Simulated verify failed"));
        PicPrg_SetVerifyResult( dwDeviceAddress, -1 );
        return false;
#else
        APPL_ShowMsg( 0, _("Simulated verify successfull"));
        PicPrg_SetVerifyResult( dwDeviceAddress, +1 );
        return true;
#endif
    }

    if ( ! PicHw_iConnectedToTarget )
    {
        PIC_HW_ConnectToTarget();    // connect to target if not already done
    }
    // call PIC_HW_DisconnectFromTarget() LATER when the USER user wants it !

    // If there is a specialized routine for the selected chip, use it,
    // otherwise use the "old" routine for PIC's with 14-bit core .
    if ( PicPrg_GetReadWriteFunctionPtr( dwDeviceAddress, &pvReader, NULL/*no writer*/ ) )
    {
        // Allocate a temporary array of DWORDS for this job.
        //  For "verify", don't overwrite the caller's buffer !
        //  Note: Some specialized subroutines may read or write data in BANKS
        //        of up to 64(?) memory words. So, for safety, we allocate a bit more
        //        than theoretically required:
        pdwTempBuf = (uint32_t *)malloc( (dwNrOfLocations +256/*for safety*/) * sizeof(uint32_t) );
        if ( pdwTempBuf==NULL )
        {
            ++n_errors;
        }
        else // malloc ok..
        {
            if ( ! pvReader( dwDeviceAddress, pdwTempBuf, dwNrOfLocations ) )
            {
                ++n_errors;
            }
            else // reading ok..
            {
                for ( i=0; (uint32_t)i<dwNrOfLocations; ++i )
                {
                    dwDeviceAddress2 = dwDeviceAddress + i * dwAddressFactor;
                    // (may have to "AND" this with something if there are unimplemented bits)
                    // When programming CONFIG REGS, individual masks for verify MAY be required, so:
                    dwMask2 = PicDev_GetVerifyMaskForAddress( dwDeviceAddress2 );
                    wFlags  = PicBuf_GetMemoryFlags(dwDeviceAddress2);
                    if ( pdwSourceData != NULL ) // may be NULL since 2005-08 (calling buffer access routine then)
                    {
                        w = pdwSourceData[i];   // value which should have been WRITTEN
                    }
                    else  // added 2005-08-21, used in PIC16F7x_PRG.C :
                    {
                        PicBuf_GetBufferWord( dwDeviceAddress2, &w );// word from WinPic's global buffer
                    }

                    r = pdwTempBuf[i];      // value which has just been READ BACK
                    if ( ( w & dwMask2) != (r & dwMask2) )
                    {
                        ++n_errors;
                        PicBuf_SetMemoryFlags(dwDeviceAddress2, wFlags | PIC_BUFFER_FLAG_VFY_ERROR );
                        if ( (long)dwDeviceAddress2 == PIC_DeviceInfo.lConfWordAdr )
                        {
                            PicPrg_iConfWordVerified = -1;  // bad config word
                        }
                        else if ( PicDev_IsConfigMemLocationValid(dwDeviceAddress2) )
                        {
                            // other stuff in the "config memory" (ID location for PIC16Fxxx)
                            nGoodConfigMemLocs = -9999; // "other" config memory locations are bad
                        }
                        if (n_errors<5)
                        {
                            _stprintf(PIC_error_string, _("Verify Error: %06lX: read %06lX, wanted %06lX"),
                                      dwDeviceAddress2, r, w );
                            if ( dwMask2 != dwMask )
                            {
                                // something special -> show the verify-bitmask too
                                cp = PIC_error_string + _tcslen(PIC_error_string);
                                _stprintf(cp, _(", mask %06lX  ==> diff=%06lX"), dwMask2, (r^w) & dwMask2 );
                            }
                            APPL_ShowMsg( 0, PIC_error_string);
                        }
                    } // end if < difference between read and expected value >
                    else  // verify ok, reset "verify error" flag for this location:
                    {
                        PicBuf_SetMemoryFlags(dwDeviceAddress2, wFlags & ~PIC_BUFFER_FLAG_VFY_ERROR );
                        if ( (long)dwDeviceAddress2 == PIC_DeviceInfo.lConfWordAdr )
                        {
                            PicPrg_iConfWordVerified = 1;  // classic config word (with CP bits etc) has been verified
                        }
                        else if ( PicDev_IsConfigMemLocationValid(dwDeviceAddress2) )
                        {
                            // other stuff in the "config memory" was verified ok :
                            if ( nGoodConfigMemLocs>=0 )
                                ++nGoodConfigMemLocs;
                        }
                    }
                } // end for < all verified locations >
            } // end if < reading ok >
            free( pdwTempBuf );
        } // end if < malloc ok >
    }
    else // not one of the "new" functions (since 2005),  assume 14-bit like PIC16Fxxx :
    {
        PIC_HW_ProgMode();       // first(?) Vdd on, then(?) Vpp on

        if (    (dwDeviceAddress >= (uint32_t)PIC_DeviceInfo.lConfMemBase)
                && (dwDeviceAddress <= (uint32_t)PIC_DeviceInfo.lConfWordAdr)  )
        {
            PIC_PRG_LoadConf(dwDeviceAddress, // load the 1st config word and set the address counter to 'dwDeviceAddress'
                             0x3FFF ); /* here, for READING, send dummy (0x3FFF) */
        }

        for ( i=0; (uint32_t)i<dwNrOfLocations; ++i )
        {
            dwDeviceAddress2 = dwDeviceAddress+i;
            PIC_HW_SerialOut_Command6( rdcmd/*0x04?*/  , true );
            r = PIC_HW_SerialRead_14Bit();
            dwMask2 = dwMask;
            if ( (dwDeviceAddress+i) == 0x2008L) // speciality for 16F88 : verify SECOND config word (2 bit?)
                dwMask2 = PIC_DeviceInfo.wCfgmask2_used;
            else
            {
                // When programming CONFIG REGS, individual masks for verify MAY be required, so:
                dwMask2 = PicDev_GetVerifyMaskForAddress( dwDeviceAddress2 );
            }
            if ( pdwSourceData != NULL ) // may be NULL since 2005-08 (calling buffer access routine then)
            {
                dwSourceFromBuffer = pdwSourceData[i];   // value which should have been WRITTEN
            }
            else  // added 2005-08-21, used in PIC16F7x_PRG.C :
            {
                PicBuf_GetBufferWord( dwDeviceAddress+i, &dwSourceFromBuffer );
            }
            if ( ((w = dwSourceFromBuffer) & dwMask2) != (r & dwMask2) )
            {
                ++n_errors;
                PicBuf_SetMemoryFlags(dwDeviceAddress2,
                                      PicBuf_GetMemoryFlags(dwDeviceAddress2) | PIC_BUFFER_FLAG_VFY_ERROR );
                if ( (long)dwDeviceAddress2 == PIC_DeviceInfo.lConfWordAdr )
                {
                    PicPrg_iConfWordVerified = -1;  // error verifying "config-word"
                }
                else if ( PicDev_IsConfigMemLocationValid(dwDeviceAddress2) )
                {
                    // other stuff in the "config memory" (ID location for PIC16Fxxx)
                    nGoodConfigMemLocs = -9999;
                }
                if (n_errors<5)
                {
                    _stprintf(PIC_error_string, _("Verify Error: %06lX: read %06lX, wanted %06lX"), dwDeviceAddress2, r, w );
                    APPL_ShowMsg( 0, PIC_error_string);
                }
            } // end if <verify failed>
            else  // verify ok, reset "verify error" flag for this location:
            {
                PicBuf_SetMemoryFlags(dwDeviceAddress2,
                                      PicBuf_GetMemoryFlags(dwDeviceAddress2) & ~PIC_BUFFER_FLAG_VFY_ERROR );
                if ( (long)dwDeviceAddress2 == PIC_DeviceInfo.lConfWordAdr )
                {
                    PicPrg_iConfWordVerified = 1;
                }
                else if ( PicDev_IsConfigMemLocationValid(dwDeviceAddress2) )
                {
                    // other stuff in the "config memory" was verified ok :
                    if ( nGoodConfigMemLocs>=0 )
                        ++nGoodConfigMemLocs;
                }
            }

            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr,  true  );
            APPL_ShowProgress( (100*i) / dwNrOfLocations );
            if ( (i&15)==0 ) // added 2007-08-27, because a USB<->RS-232 adapter was SOOO TERRIBLY SLOW...
            {
                Appl_CheckUserBreak();
            }
            if (APPL_iUserBreakFlag)
            {
                ++n_errors;
                break;
            }
        } // end for (i.. )

        PIC_HW_ProgModeOff();   // programming voltage off, clock off, data high-z
    } // end else < 14-bit core >

    if (n_errors>=5)
    {
        _stprintf(PIC_error_string, _( "More Verify Errors, unable to list all (total=%d)" ), n_errors);
        APPL_ShowMsg( 0, PIC_error_string);
    }

    PicPrg_SetVerifyResult( dwDeviceAddress, (n_errors==0) ? +1 : -1 );
    if (nGoodConfigMemLocs>0)
        PicPrg_iConfMemVerified = 1; // other stuff in the "config memory" (ID location for PIC16Fxxx)
    else if (nGoodConfigMemLocs<0)
        PicPrg_iConfMemVerified = -1;
    /* if nGoodConfigMemLocs==0, DO NOT TOUCH PicPrg_iConfMemVerified here ! */

    return n_errors == 0;

} // end ..Verify()

/***************************************************************************/
void PicPrg_ShowReadingIndicator(long i32FromAddr, long i32ToAddr)
{
    APPL_ShowMsg( 0, _("Reading 0x%06lX - 0x%06lX ..."),
                  i32FromAddr, i32ToAddr );
}

/***************************************************************************/
/** Blank check or "read all locations".
 * Returns false in case of ANY ERROR !
 * The "is-not-blank-flag" is placed in an extra variable since 2005-06 .
 */
bool PIC_PRG_ReadAll(
    bool blank_check_only,  //!< in: blank check(true) or "read all" ?
    bool *pfIsNotBlank )    //!< out: result of blank check
{
    int i = 0;
    long i32;
    uint16_t w, wFlags;
    bool fResult = true;
    bool not_blank, memrange_blank;
    wxChar sz80Temp[81];
    uint32_t dwVerifyMask = 0;
    uint32_t dwTargetAddress = 0;
    uint32_t dw32TempBuffer[32];
    uint32_t dwEmptyPattern = 0x0003FFF;
    long  iNrWordsToRead, wNDeviceIdWords;

    long prog_mem_last = -1;
    int iLastUsedDataIndex = -1;
    int i1stNonEmptyIdAddr  = 0;
    int cfg_base_adr = 0;
    uint16_t dwCurrentAddress;
    int  iNrCfgLocations;

    not_blank = false;         // assume the PIC is BLANK (!)

    if ( ! PicHw_iConnectedToTarget )
    {
        PIC_HW_ConnectToTarget();    // connect to target if not already done
    }
    // call PIC_HW_DisconnectFromTarget() LATER when the USER user wants it !

    if ( PIC_HW_CanSelectVdd() )       // since 2005-09-29 :
    {
        PIC_HW_SelectVdd( 1/*norm*/ );  // use the "normal" voltage (=5V) for reading
    }

    if ( PIC_DeviceInfo.iBitsPerInstruction==12 )
    {
        return PIC10F_ReadAll( blank_check_only ? PIC_ACTION_BLANK_CHECK : PIC_ACTION_READ );
    }
    else
        if (   PIC_DeviceInfo.iBitsPerInstruction==24
                || PIC_DeviceInfo.iBitsPerInstruction==16 )
        {
            // 24bit per "instruction word" (dsPIC30F), 16 bit (PIC18Fxxxx), or 12 bit (PIC10F20x) ...
            T_PicPrg_ReadWriteFunc *pvReadData, *pvReadCode, *pvReadConfig; // jump vectors,
            pvReadData=pvReadCode=pvReadConfig = PicPrg_ReadWriteDummy; // must all be valid !
            wNDeviceIdWords = 1;
            if ( PIC_DeviceInfo.iBitsPerInstruction==24)
            {
                pvReadData  = dsPIC_ReadDataMemory;   // pointer to "DATA reader"
                pvReadCode  = dsPIC_ReadCodeMemory;   // pointer to "CONFIG reader"
                pvReadConfig= dsPIC_ReadConfigRegs;   // pointer to "CODE reader"
                dwEmptyPattern  = 0x0FFFFFF;
                wNDeviceIdWords = 2;    // here TWO device-ID WORDS ( 2 * 16 bit)
                dsPIC_EnterSTDPMode();  // enter STDP programming mode
            }
//#ifndef PIC18F_INCOMPLETE
            else if ( PIC_DeviceInfo.iBitsPerInstruction==16)
            {
                pvReadData  = PIC18F_ReadDataMemory;
                pvReadCode  = PIC18F_ReadCodeMemory;
                pvReadConfig= PIC18F_ReadConfigRegs;
                dwEmptyPattern = 0x000FFFF;
            }
//#endif
            else if ( PIC_DeviceInfo.iBitsPerInstruction==12)
            {
                // pvReadData  = PIC10F_ReadDataMemory;  // there is no DATA EEPROM in this chip !
                //pvReadCode  = PIC10F_ReadCodeMemory;
                //pvReadConfig= PIC10F_ReadConfigRegs;
                dwEmptyPattern = 0x0000FFF;
            }


            // Read Device ID Word(s) (dsPIC30F: 0xFF0000..0xFF0002,  PIC18F: 0x3FFFFE..0x3FFFFF)
            //  and Config Words      (dsPIC30F: 0xF80000..0xF8000E,  PIC18F: 0x300000..0x30000D) ?
            if ( PIC_DeviceInfo.lDeviceIdAddr!=0 && PIC_DeviceInfo.lDeviceIdAddr!= -1)
            {
                PicPrg_ShowReadingIndicator(    PIC_DeviceInfo.lDeviceIdAddr,
                                                PIC_DeviceInfo.lDeviceIdAddr + 2*wNDeviceIdWords - 1 );
                pvReadConfig( PIC_DeviceInfo.lDeviceIdAddr, dw32TempBuffer, wNDeviceIdWords ); // read DEVICE ID words
                for ( i=0; i<wNDeviceIdWords; i++ )   // SET the DEVICE ID, even when blank-checking !!
                {
                    PicBuf_SetBufferWord( PIC_DeviceInfo.lDeviceIdAddr + 2*i, dw32TempBuffer[i] );
                }
            } // end if < valid address for dsPIC's DEVICE ID word ? >
            if ( PIC_DeviceInfo.lConfMemBase!=0 && PIC_DeviceInfo.lConfMemBase != -1)
            {
                iNrWordsToRead = PicDev_GetConfMemSize();
                // Find out how many WORDs must be read by looking at the BITS in dwConfMemUsedLocations:
                if ( iNrWordsToRead > 0 )
                {
                    PicPrg_ShowReadingIndicator( PIC_DeviceInfo.lConfMemBase,
                                                 PIC_DeviceInfo.lConfMemBase + 2*iNrWordsToRead - 1 );
                    pvReadConfig( PIC_DeviceInfo.lConfMemBase, dw32TempBuffer, iNrWordsToRead ); // read CONFIG REGISTERS
                    memrange_blank = true;
                    for ( i=0; i<=iNrWordsToRead; i++ )
                    {
                        dwTargetAddress = PIC_DeviceInfo.lConfMemBase + 2*i;
                        dwVerifyMask = PicDev_GetVerifyMaskForAddress( dwTargetAddress );
                        if ( (dw32TempBuffer[i] & dwVerifyMask) != dwVerifyMask ) // > " unimplemented bits read as '1' " (?)
                        {
                            if ( blank_check_only && memrange_blank )
                            {
                                APPL_ShowMsg( 0, _("  Config memory is not blank at addr 0x%06lX : read=0x%04lX, mask=0x%04lX ."),
                                              dwTargetAddress, dw32TempBuffer[i], dwVerifyMask );
                            }
                            memrange_blank = false;
                            not_blank = true;
                        }
                        if (! blank_check_only )
                            PicBuf_SetBufferWord( dwTargetAddress, dw32TempBuffer[i] );
                    }
                }
            } // end if < valid address for dsPIC's CONFIGURATION REGISTERS ? >

            // Read the DATA EEPROM MEMORY from dsPIC30F & PIC18F ?  (also used for blank-checking)
            if ( PIC_DeviceInfo.lDataEEPROMSizeInByte > 0 )
            {
                uint32_t * pdwTempBuffer = (uint32_t*)malloc(PIC_DeviceInfo.lDataEEPROMSizeInByte * sizeof(uint32_t)/*!*/ );
                // Using uint32_t arrays for simplicity, though treating "DATA MEMORY" as 8-bit space
                if ( pdwTempBuffer )
                {
                    PicPrg_ShowReadingIndicator(    PIC_DeviceInfo.lDataMemBase,
                                                    PIC_DeviceInfo.lDataMemBase+PIC_DeviceInfo.lDataEEPROMSizeInByte-1 );
                    if ( pvReadData( PIC_DeviceInfo.lDataMemBase, // uint32_t dwSourceAddress,
                                     pdwTempBuffer, // uint32_t *pdwDestBuffer,
                                     PIC_DeviceInfo.lDataEEPROMSizeInByte)) // uint32_t dwNrOfLocations
                    {
                        memrange_blank = true;
                        for (i32=0; i32<PIC_DeviceInfo.lDataEEPROMSizeInByte; ++i32)
                        {
                            dwTargetAddress = PIC_DeviceInfo.lDataMemBase + i32;
//                            PHWInfo.iCurrProgAddress = dwTargetAddress; // added 2008-05 for the plugin-DLL
                            if ( (pdwTempBuffer[i32] & 0x000000FF) != 0x000000FF) // note: 24-bit instructions !
                            {
                                not_blank = true;
                                if ( blank_check_only && memrange_blank )
                                {
                                    APPL_ShowMsg( 0, _("  Data memory is not blank at addr 0x%06lX : read=0x%04lX ."),
                                                  dwTargetAddress, pdwTempBuffer[i32] );
                                }
                            }
                        }
                        if ( ! blank_check_only ) // no "blank check" but "read" :
                        {
                            // Note: WinPic's "data memory" buffer always
                            // uses contains ONE BYTE per array index,
                            // though the array itself is uint32_t memory !
                            PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex = -1;
                            for (i32=0; i32<PIC_DeviceInfo.lDataEEPROMSizeInByte; ++i32)
                            {
                                dwTargetAddress = PIC_DeviceInfo.lDataMemBase + i32;
//                                PHWInfo.iCurrProgAddress = dwTargetAddress; // added 2008-05 for the plugin-DLL
                                PicBuf_SetBufferByte( dwTargetAddress, pdwTempBuffer[i32] );
                                if ( pdwTempBuffer[i32] != 0x00FF)
                                {
                                    wFlags = PIC_BUFFER_FLAG_USED;
                                    PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex = i32;
                                }
                                else wFlags = 0x0000;
                                PicBuf_SetMemoryFlags( dwTargetAddress, wFlags );
                            }
                        } // end else < READ dsPIC >
                    } // end if < successfully read dsPIC's code memory >
                    free( pdwTempBuffer );  // free temporary buffer
                } // end if < successfully allocated temporary buffer >
            } // end if < dsPIC with DATA EEPROM ? >


            // Read CODE MEMORY from dsPIC30F or PIC18F ?
            if ( PIC_DeviceInfo.lCodeMemSize>=4 && PIC_DeviceInfo.lCodeMemSize<=0x020000)
            {
                uint32_t * pdwTempBuffer = (uint32_t*)malloc(PIC_DeviceInfo.lCodeMemSize * sizeof(uint32_t) );
                if ( pdwTempBuffer )
                {
                    PicPrg_ShowReadingIndicator( 0L,  2*PIC_DeviceInfo.lCodeMemSize - 1 );
                    if ( pvReadCode(       0x000000, // uint32_t dwSourceAddress,
                                           pdwTempBuffer, // uint32_t *pdwDestBuffer,
                                           PIC_DeviceInfo.lCodeMemSize)) // uint32_t dwNrOfLocations
                    {
                        if ( blank_check_only )
                        {
                            memrange_blank = true;
                            for (i32=0; i32<PIC_DeviceInfo.lCodeMemSize && i32<PIC_BUF_CODE_SIZE; ++i32)
                            {
//                                PHWInfo.iCurrProgAddress = i32; // added 2008-05 for the plugin-DLL
                                if ( pdwTempBuffer[i32] != dwEmptyPattern) // note: 16- or 24-bit instructions !
                                {
                                    not_blank = true;
                                    if ( blank_check_only && memrange_blank )
                                    {
                                        APPL_ShowMsg( 0, _("  Code memory is not blank at addr 0x%06lX : read=0x%04lX, mask=0x%04lX ."),
                                                      dwTargetAddress, dw32TempBuffer[i], dwVerifyMask );
                                    }
                                    memrange_blank = false;
                                }
                            }
                        }
                        else // no "blank check" but "read"..
                        {
                            PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex = -1;
                            for (i32=0; i32<PIC_DeviceInfo.lCodeMemSize && i32<PIC_BUF_CODE_SIZE; ++i32)
                            {
//                                PHWInfo.iCurrProgAddress = i32; // added 2008-05 for the plugin-DLL
                                PicBuf_SetBufferWord( 2*i32/*TargetAddress*/, pdwTempBuffer[i32]/*data*/ );
                                if ( pdwTempBuffer[i32] != dwEmptyPattern)
                                {
                                    wFlags = PIC_BUFFER_FLAG_USED;
                                    PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex = i32;
                                }
                                else wFlags = 0x0000;
                                PicBuf_SetMemoryFlags( 2*i32/*TargetAddress*/, wFlags );
                            }
                        } // end else < READ dsPIC >
                    } // end if < successfully read dsPIC's code memory >
                    else // failed reading (possibly stopped via ESCAPE key)
                    {
                        APPL_ShowMsg( 0, _("Action aborted") );
                    }
                    free( pdwTempBuffer );  // free temporary buffer
                } // end if < successfully allocated temporary buffer >
            } // end if < reasonable code memory size >
        } // end if < dsPIC/24 bit core    or    PIC18F/16 bit core >
        else  // "bits per instruction" neither 12, 24, nor 16 ..  here comes the old stuff (14 bit):
        {
            // Read the CODE MEMORY (flash or eprom, but NOT dsPIC or 18F)
            PicPrg_ShowReadingIndicator( 0L, PIC_DeviceInfo.lCodeMemSize-1 );
            PIC_HW_ProgMode();        // first(?) Vdd on, then(?) Vpp on
            for ( i=0; i<PIC_DeviceInfo.lCodeMemSize && i<PIC_BUF_CODE_SIZE; ++i )
            {
//                PHWInfo.iCurrProgAddress = i; // added 2008-05 for the plugin-DLL
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_ReadProg, true );
                if ( ( w = (PIC_HW_SerialRead_14Bit() & 0x3FFF)) != 0x3FFF )
                    prog_mem_last = i;
                if (! blank_check_only )
                {
                    PicBuf[PIC_BUF_CODE].pdwData[i] = w;
                    if (w != 0x3FFF)
                        wFlags = PIC_BUFFER_FLAG_USED;
                    else wFlags = 0x0000;
                    PicBuf_SetMemoryFlags( i, wFlags );

                    if ( (i == PIC_DeviceInfo.lAddressOscCal) && (w!=0x3FFF) )
                    {
                        // here is the precious oscillator calibration word:
                        PIC_lOscillatorCalibrationWord = w;   // save it for later
                    }
                }
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true );
                APPL_ShowProgress( (100*i) / PIC_DeviceInfo.lCodeMemSize );
                if ( (i&15)==0 ) // added 2007-08-27, because a USB<->RS-232 adapter was SOOO TERRIBLY SLOW...
                {
                    Appl_CheckUserBreak();
                }
                if (APPL_iUserBreakFlag)
                {
                    fResult=false;
                    break;
                }
            } // end for ..

            // Read the DATA EEPROM MEMORY (if exists..)
            if ( PIC_DeviceInfo.lDataEEPROMSizeInByte > 0 )
            {
                PicPrg_ShowReadingIndicator( PIC_DeviceInfo.lDataMemBase,
                                             PIC_DeviceInfo.lDataMemBase + PIC_DeviceInfo.lDataEEPROMSizeInByte - 1 );
                PIC_HW_ProgMode();        // first(?) Vdd on, then(?) Vpp on
                for ( i=0; i<PIC_DeviceInfo.lDataEEPROMSizeInByte; ++i )
                {
//                    PHWInfo.iCurrProgAddress = PIC_DeviceInfo.lDataMemBase+i; // added 2008-05 for the plugin-DLL
                    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_ReadDataDM, true );
                    if ( ( w = PIC_HW_SerialRead_14Bit() & 0xFF) != 0xFF )
                        iLastUsedDataIndex = i; // INDEX, not ADDRESS !
                    if (! blank_check_only )
                    {
                        PicBuf[PIC_BUF_DATA].pdwData[i] = w;
                        if ( (w&0x00FF) != 0x00FF)
                            PicBuf_SetMemoryFlags( PIC_DeviceInfo.lDataMemBase+i,
                                                   PicBuf_GetMemoryFlags( PIC_DeviceInfo.lDataMemBase+i )
                                                   | PIC_BUFFER_FLAG_USED );
                    } // end if <no blank check>
                    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true );
                    if ( (i&15)==0 ) // added 2007-08-27, because a USB<->RS-232 adapter was SOOO TERRIBLY SLOW...
                    {
                        Appl_CheckUserBreak();
                    }
                    if (APPL_iUserBreakFlag)
                    {
                        fResult=false;
                        break;
                    }
                } // end for < all DATA EEPROM locations >
            } // end if < DATA EEPROM exists >

            // Read ID Locations (0x2000..0x2003),  Device ID Word (0x2006),
            //   and Config Word(s) (0x2007, sometimes also 0x2008)
            // OOOOPS !!!  This failed for a PIC16F818 on 2003-12-28
            //             if reading the CODE MEMORY was skipped !?!?!?!
            // 2005-11: Even in 'blank-check' mode, READ but don't "blank-check"
            //            the ID locations in a PIC16Fxxxx !
            iNrCfgLocations = 8;  // read locations 0x2000 .. 0x2007 (at least, there may be more)
            for ( i=8; i<PIC_BUF_CONFIG_SIZE; ++i )
            {
//                PHWInfo.iCurrProgAddress = PIC_DeviceInfo.lConfMemBase+i; // added 2008-05 for the plugin-DLL
                if (PicDev_IsConfigMemLocationValid(PIC_DeviceInfo.lConfMemBase+i) )
                {
                    // example: if 0x2009 is "valid" (Calibration Word 2),
                    iNrCfgLocations = i+1;  // iNrCfgLocations will be TEN (0x2000..0x2009)
                }
            }
            PicPrg_ShowReadingIndicator( PIC_DeviceInfo.lConfMemBase,
                                         PIC_DeviceInfo.lConfMemBase+iNrCfgLocations-1 );
            PIC_HW_ProgMode();        // first(?) Vdd on, then(?) Vpp on
//            PHWInfo.iCurrProgAddress = PIC_DeviceInfo.lConfMemBase; // added 2008-05 for the plugin-DLL
            PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadConfig, false);  // internal address to 0x2000
//            PHWInfo.iCurrProgAddress = PIC_DeviceInfo.lConfMemBase; // added 2008-05 for the plugin-DLL
            PIC_HW_SerialOut_14Bit(0x3FFF);  // dummy data, immediatly after the command already sent
//            PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadConfig,6,   0x3FFF,14 );
            dwCurrentAddress = PIC_DeviceInfo.lConfMemBase; // should be 0x2000 in 99.99 % of all PICs
            for ( i=0; i<=iNrCfgLocations; ++i )
            {
//                PHWInfo.iCurrProgAddress = dwCurrentAddress; // added 2008-05 for the plugin-DLL
                if ( (dwCurrentAddress==PIC_DeviceInfo.lDeviceIdAddr) || (dwCurrentAddress==PIC_DeviceInfo.lConfWordAdr) || PicDev_IsConfigMemLocationValid(dwCurrentAddress) )
                {
                    // only try to read locations which are physically implemented in this PIC !
                    // ( who knows what may happen when trying to READ locations which don't exist,
                    //   it MAY work but Murphy will make sure it won't )
                    PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_ReadProg, true );
                    w = PIC_HW_SerialRead_14Bit() & 0x3FFF;
                    dwVerifyMask = PicDev_GetVerifyMaskForAddress( dwCurrentAddress );
                    if (! blank_check_only )
                    {
                        PicBuf_SetBufferWord( dwCurrentAddress , w );
                        // Mark this location as "used" if it looks non-empty:
                        if ( (dwVerifyMask!=0) && ((w & dwVerifyMask) != dwVerifyMask) )
                        // ex: if( w != 0x3FFF ) .. which is wrong because on some PICs, unimplemented bits may read ZERO not ONE
                        // Example: PIC16F628A, dwCurrentAddress=0x2007: dwVerifyMask=0x21FF instead of 0x3FFF
                            wFlags = PIC_BUFFER_FLAG_USED;
                        else wFlags = 0x0000;
                        PicBuf_SetMemoryFlags( dwCurrentAddress, wFlags );

                    } // end if < not just a blank-check >

                    // Special treatment after reading the CONFIG WORD, usually at address 0x2007 :
                    if ( (w & dwVerifyMask) != (0x3FFF & dwVerifyMask) )
                    {
                        if ( dwCurrentAddress==PIC_DeviceInfo.lConfWordAdr/*0x2007*/ )
                            cfg_base_adr = PIC_DeviceInfo.lConfWordAdr;
                        else
                        {
                            int IdOffset = dwCurrentAddress - PIC_DeviceInfo.lIdMemBase;
                            if ( (IdOffset >= 0) && (IdOffset < PIC_DeviceInfo.lIdMemSize))
                                i1stNonEmptyIdAddr = dwCurrentAddress;
                            else
                                cfg_base_adr = dwCurrentAddress;
                        }
                    }

                    if (( PIC_DeviceInfo.wCfgmask_bandgap != 0)
                    &&  ( dwCurrentAddress==PIC_DeviceInfo.lConfWordAdr/*0x2007*/ )
                    &&  ! blank_check_only)
                    {
                        // the configuration word contains some "bandgap calibration bits",
                        // save them in an extra variable because they must be restored later
                        PIC_lBandgapCalibrationBits = (w & PIC_DeviceInfo.wCfgmask_bandgap);
                    }
                }
                // SKIP or ADVANCE to next location in the PIC's configuration memory :
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true );
                ++dwCurrentAddress;
            }

            not_blank |=  (prog_mem_last != -1) || (iLastUsedDataIndex > -1)
                          || (i1stNonEmptyIdAddr != 0)  || ( cfg_base_adr != 0) ;

            if (blank_check_only)
            {
                APPL_ShowMsg( 0, _("Blank Check results:"));
                if (!not_blank)
                    APPL_ShowMsg( 0,_("  Device is blank."));
                if (prog_mem_last != -1)
                    APPL_ShowMsg( 0, _("  Program memory %ld is not blank."), prog_mem_last);
                if (iLastUsedDataIndex != -1)
                    APPL_ShowMsg( 0, _("  Data memory %ld is not blank."), iLastUsedDataIndex);
                if (i1stNonEmptyIdAddr != 0)
                    APPL_ShowMsg( 0, _("  ID memory %lX is not blank."), i1stNonEmptyIdAddr);
                if ( cfg_base_adr != 0)
                    APPL_ShowMsg( 0, _("  Config memory %lX is not blank."), cfg_base_adr);
            }
            else // not just BLANK CHECKING, but really READING:
            {
                //    Set infos for dumping to hex file.
                PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex = prog_mem_last;
                PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex = iLastUsedDataIndex;
            }
            PIC_HW_ProgModeOff();           // programming voltage off, clock off, data high-z

            // Emit some urgent warnings if required...
            if (  (PIC_DeviceInfo.lAddressOscCal >= 0)
                    && ((PIC_lOscillatorCalibrationWord & 0xFF00) != 0x3400/*RETLW*/ ) )
            {
                _stprintf(sz80Temp, _( "Oscillator calibration word looks bad (0x%06lX) !" ),
                          PIC_lOscillatorCalibrationWord & 0x0FFFF );
                APPL_ShowMsg( 0, sz80Temp );
            }
        } // end else  ( PIC_DeviceInfo.iBitsPerInstruction != 24 )

    // ex:  return not_blank;
    if ( pfIsNotBlank )
        *pfIsNotBlank = not_blank;
    return fResult;  // since 2005-06, returns true only if the operation was FINISHED,
    // regardless of the "blank" status !
} // end ..Read()


/***************************************************************************/
/** Reads the "Executive Memory" in dsPIC30F.
 * For dsPIC30Fxxxx only ! Hardly ever used, since WinPic doesn't rely
 * on the "programming executive" at all.
 * Result will be placed in PicBuf[PIC_BUF_EXEC].
 */
bool PicPrg_ReadExecutiveCodeMemory(void)
{
    uint32_t dwNLocations;
    if ( PIC_DeviceInfo.iBitsPerInstruction==24 )
    {
        dwNLocations = 0x05C0/2;  // uint32_t dwNLocations (crazy dsPIC: 2 address steps per 24-bit location ! )
        if ( dwNLocations > PicBuf[PIC_BUF_EXEC].dwMaxSize) // never exceed allocated size of target buffer
            dwNLocations = PicBuf[PIC_BUF_EXEC].dwMaxSize;
        if ( dsPIC_STDP_ReadCodeMemory(  0x800000, // uint32_t dwSourceAddress,
                                         PicBuf[PIC_BUF_EXEC].pdwData, // uint32_t *pdwDestBuffer,
                                         dwNLocations ) ) // uint32_t dwNrOfCodeWords
        {
            PicBuf[PIC_BUF_EXEC].dwAddressOffset = 0x800000;
            PicBuf[PIC_BUF_EXEC].dwAddressFactor = 2; // bizarre but true for dsPIC30F !
            PicBuf[PIC_BUF_EXEC].i32LastUsedArrayIndex = dwNLocations-1;
            return true;
        }
        else
        {
            PicBuf[PIC_BUF_EXEC].i32LastUsedArrayIndex = -1;
            return false;
        }
    }
    else // there is no "executive code memory" in NON-dsPICs !
    {
        PicBuf[PIC_BUF_EXEC].i32LastUsedArrayIndex = -1;
        return false;
    }

} // end PIC_PRG_ReadExecutiveCodeMemory(void)

/***************************************************************************/
/** Programs the "Executive Memory" in dsPIC30F.
 * For dsPIC30Fxxxx only ! Hardly ever used, since WinPic doesn't rely
 * on the "programming executive" at all.
 * Input will be taken from PicBuf[PIC_BUF_EXEC].
 */
bool PIC_PRG_WriteExecutiveCodeMemory(void)
{
    if ( PIC_DeviceInfo.iBitsPerInstruction==24 )
    {
        if ( PicBuf[PIC_BUF_EXEC].i32LastUsedArrayIndex >= 0 )
        {
            if ( dsPIC_STDP_WriteCodeMemory( 0x800000, // uint32_t dwSourceAddress,
                                             PicBuf[PIC_BUF_EXEC].pdwData, // uint32_t *pdwDestBuffer,
                                             PicBuf[PIC_BUF_EXEC].i32LastUsedArrayIndex+1 ) ) // uint32_t dwNrOfCodeWords
            {
                return true;
            }
        }
    }
    return false;
} // end PIC_PRG_WriteExecutiveCodeMemory(void)



/* EOF <pic_prg.cpp>  */

