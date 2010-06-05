/*-------------------------------------------------------------------------*/
/* PIC16F7x_PRG.c                                                          */
/*                                                                         */
/*  Purpose: Serial PIC Programming algorithms for PIC16F7x (in WinPic).   */
/*  Author : Wolfgang Buescher (DL4YHF)                                    */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*                                                                         */
/*  Date   : 2005-08-21  (ISO 8601 format, YYYY-MM-DD)                     */
/*  State  : UNDER DEVELOPMENT - NOT TESTED BY THE AUTHOR !                */
/*                                                                         */
/*  2009-06-04:  Ported to wxWidget by Philippe Chevrier                   */
/*                                                                         */
/*  Literature:                                                            */
/*   [PIC16F7xps] = "PIC16F7x Memory Programming Specification"            */
/*                Microchip DS30324B (or later ? )                         */
/*  C A U T I O N :  The "PIC16F7x" prog spec does not cover the PIC16F72. */
/*               We don't know why Microchip produced an extra document    */
/*               for the PIC16F72 which is *not* supported by this module. */
/*  Furthermore, the PIC16F716 is also not compatible (see PIC16F716_PRG.C)*/
/*-------------------------------------------------------------------------*/

#define L_RELEASE_VERSION 0

  // Revsion history - NEWEST entry first !
  //
  // 2005-08-21:  Added this module after someone found out that the
  //     CONFIG MEMORY PROGRAMMING method used for other PICs didn't work
  //     for a PIC16F74 . To avoid trashing other subroutines in PIC_PRG.C ,
  //     this new module named PIC16F7X.C was added to WinPic .
  //     Mainly based on the subroutine PIC_PRG_Program16F7X7(), the routine
  //     in this module strictly follows the flowchart in [PIC16F7xps] .
  //

#include <wx/string.h>
#include <wx/intl.h>
//#include <stdio.h>    // sprintf(), NULL, ..

#include "pic_hw.h"     // Routines to drive the programmer hardware interface
#include "devices.h"    // need T_PicDeviceInfo here !
#include "config.h"     // permanently saved Config-structure
#include <Wx/Appl.h>       // APPL_ShowMsg(), etc
#include "pic_hex.h"    // access routines to the "hex data buffer"
#include "pic_prg.h"    // generic defines for programming routines
#include "PIC16F7x_PRG.h" // header for the PIC16F7x programming algorithms


//----------------------------------------------------------------------------
//  Variables
//----------------------------------------------------------------------------

int  PIC16F7x_iCurrTargetAddress;   // current target address ("PC" inside the PIC)
                                    // INCREMENTED IN PIC16F7x_IncrAddress() ONLY !
int  PIC16F7x_iNrOfErrors;
uint16_t PIC16F7x_wTempConfigWord;      // config word, read before erase
bool PIC16F7x_fHaveReadOscCalib;
uint16_t PIC16F7x_wReadOscCalibWord;    // oscillator calibration word, read before erase
uint16_t PIC16F7x_wReadOscCalibBackup;  // backup oscillator calibration word, read before erase


//----------------------------------------------------------------------------
void PIC16F7x_IncrAddress(int iNrOfSteps)
  // increments the PIC's internal address pointer,
  // but also PIC16F7x_iCurrTargetAddress (our "local copy") .
{
  while(iNrOfSteps--)
   {
     if( ! PIC_PRG_iSimulateOnly )
         PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true);  // send 6-bit command "Increment Address"
     // Keep track of the PIC's internal instruction pointer ("PC") :
     // > The PC will increment from 0x0000 to 0x1FFF and wrap to 0x0000,
     // >  [or] 0x2000 to 0x3FFF and wrap around to 0x2000 (not to 0x0000).
     // The question is, will the PC alway wrap from 0x1FFF to 0x0000,
     //  (as stated in [PIC16F7xps] Chapter 2.1 "User Program Memory Map" )
     // or will it wrap from 0x0FFF for 4K-devices like PIC16F73/74 ?
     // Anyway, if we expect it to wrap from 0x1FFF to 0x0000 always,
     // it doesn't hurt if it already wrapped from 0x0FFF - it'll simply wrap twice.
     if( PIC16F7x_iCurrTargetAddress == 0x1FFF )
         PIC16F7x_iCurrTargetAddress =  0x0000;
     else
     if( PIC16F7x_iCurrTargetAddress == 0x3FFF )
         PIC16F7x_iCurrTargetAddress =  0x2000;
     else ++PIC16F7x_iCurrTargetAddress;
//     PHWInfo.iCurrProgAddress = PIC16F7x_iCurrTargetAddress; // since 2008-05, for DLL-plugin
   }
} // end PIC16F7x_IncrAddress()


/***************************************************************************/
bool PIC16F7x_VerifyRange( uint32_t dwNrOfLocations )
{
 int i;
// uint16_t  wFlags;
 uint32_t dwRead, dwWritten, dwMask;
 uint32_t dwVerifyStartAddr = (uint32_t)PIC16F7x_iCurrTargetAddress;
 int  n_errors = 0;
 wxChar sz80Msg[84];

  // NO-NO-NO :  PIC_HW_ProgMode();
  for ( i=0; (uint32_t)i<dwNrOfLocations; ++i )
   {// PHWInfo.iCurrProgAddress = PIC16F7x_iCurrTargetAddress; // since 2008-05, for DLL-plugin
     if(! PIC_PRG_iSimulateOnly)
      { PIC_HW_SerialOut_Command6( PIC_DeviceInfo.iCmd_ReadProg, true/*flush cmd*/);
        dwRead = PIC_HW_SerialRead_14Bit();
      }
     else  // SIMULATE only:
      {
        PicBuf_GetBufferWord( PIC16F7x_iCurrTargetAddress, &dwRead );
        if( i == 13 )   // simulate a verify error at this unfortunate location :
         { dwRead ^= 0x0001;
         }
      } // end if < simulate or not >

     dwMask = PicDev_GetVerifyMaskForAddress( PIC16F7x_iCurrTargetAddress );
     PicBuf_GetBufferWord( PIC16F7x_iCurrTargetAddress, &dwWritten );
     if ( ((dwWritten) & dwMask) != (dwRead & dwMask) )
      {
        ++n_errors;
        PicBuf_SetMemoryFlags(PIC16F7x_iCurrTargetAddress,
           PicBuf_GetMemoryFlags(PIC16F7x_iCurrTargetAddress) | PIC_BUFFER_FLAG_VFY_ERROR );

        if(n_errors<5)
         {
           _stprintf(sz80Msg, _("Verify Error: %06lX: read %06lX, wanted %06lX"),
                           (long)PIC16F7x_iCurrTargetAddress, (long)dwRead, (long)dwWritten );
           APPL_ShowMsg( APPL_CALLER_PIC_PRG,0, sz80Msg );
         }
      } // end if <verify failed>
     else  // verify ok, reset "verify error" flag for this location:
      {
        PicBuf_SetMemoryFlags(PIC16F7x_iCurrTargetAddress,
          PicBuf_GetMemoryFlags(PIC16F7x_iCurrTargetAddress) & ~PIC_BUFFER_FLAG_VFY_ERROR );
      }

     PIC16F7x_IncrAddress( 1/*iNrOfSteps*/ );  // "Increment Address" -> PIC16F7x_iCurrTargetAddress
     if( dwNrOfLocations > 100 )
      { APPL_ShowProgress( (100*i) / dwNrOfLocations );
      }

     if(APPL_iUserBreakFlag)
        break;
   } // end for (i.. )

  // NO-NO-NO:  PIC_HW_ProgModeOff();   // programming voltage off, clock off, data high-z

  if(n_errors>=5)
   {
    _stprintf(sz80Msg, _( "More Verify Errors, unable to list all (total=%d)" ),
            (int)n_errors);
    APPL_ShowMsg( APPL_CALLER_PIC_PRG,0, sz80Msg );
   }

  PicPrg_SetVerifyResult( dwVerifyStartAddr, (n_errors==0) ? +1 : -1 );

  return n_errors == 0;

} // end ..Verify()



//----------------------------------------------------------------------------
bool PIC16F7x_ProgramAll(
       bool fEraseAll ) // PIC_ACTION_WRITE or PIC_ACTION_ERASE .
   // - Includes "bulk" erase ("full"~"chip erase" or "standard"~"without fuses")
   // - Writes CODE MEMORY,   ID LOCATIONS,     and    CONFIG MEMORY
   //    in one over, as described in Microchip's DS30324B "PIC16F7x prog spec"
   // - Caution: TWO(!) words at a time where applicable
   //
   // Some angry notes of the author (grumbling again about INCOMPATIBLE PROGRAMMING SPECS..)
   // - Some blockhead decided to write the binary command values ("Command mapping")
   //   in the programming specs in LSB...MSB order (Least Significant Bit FIRST - BAAAAAH ! ),
   //   while in older specs (16F628 for example) the used MSB...LSB (MSbit first) .
   //   Don't be fooled by this, a closer look shows :
   //    PIC16F7X7:  "Load Data for Memory",          LSB..MSB(!) 0 1 0 0 x x
   //    PIC16F628:  "Load Data for PROGRAM Memory",  MSB..LSB(!) x x 0 0 1 0
   //    which is the same, because these seem to be 6-bit-numbers though more bits are actually sent.
   //
{
 int   nWordsPerProgCycle = 2; // 2 "words per programming cycle" in the 16F737
 uint32_t dwTemp;           // destination for memory word from WinPic's buffer module (must be uint32_t)
 uint32_t dwMask = 0x3FFF;  // only "1"-bits in the mask may be PROGRAMMED TO ONE and verified
 int   iLocationCounter;
 bool  fOk = true;

  if( Appl_CheckUserBreak() )
     return false;

  PIC16F7x_iCurrTargetAddress = 0x0000;   // this PIC starts at PC=0 when switching to program mode
//  PHWInfo.iCurrProgAddress = PIC16F7x_iCurrTargetAddress; // since 2008-05, for DLL-plugin
  if(! PIC_PRG_iSimulateOnly)
   { PIC_HW_ProgMode();          // first(?) Vdd on, then(?) Vpp on ? ? ? ?

     // "Chip Erase" for PIC16F73/74/76/77 .
     //   6-bit-command for PIC16F7x :  0x09="CHIP erase"
     PIC_HW_SerialOut_Command6( PIC_DeviceInfo.iCmd_EraseProg, true);

     //  Caution: stupid spec shows binary numbers BEGINNING WITH THE LSB - yucc !
     PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Erase_us); // PIC16F74 requires 30(!!) ms,
     // but the chip uses "self-timing" so it doesn't hurt to use a longer delay here.

     // don't leave programming mode after this ! Some interfaces seem to have problems
     // leaving programming mode and re-entering it immediately !
   }

  // Write CODE MEMORY - see [PIC16F7xps] flowchart figure 2-2, sheet 1
  for ( iLocationCounter=0; iLocationCounter<PIC_DeviceInfo.lCodeMemSize; ++iLocationCounter )
   {
//     PHWInfo.iCurrProgAddress = PIC16F7x_iCurrTargetAddress; // since 2008-05, for DLL-plugin
     // get next word from buffer (normal code memory location or OSCCAL (?) ) :
     PicBuf_GetBufferWord( PIC16F7x_iCurrTargetAddress, &dwTemp ); // here: CODE MEMORY
     if(  (PIC16F7x_fHaveReadOscCalib)
       && (PIC16F7x_iCurrTargetAddress==PIC_DeviceInfo.lAddressOscCal)
       && (! Config.iDontCareForOsccal) )
      {  // restore the orginal OSCCAL word for this special location :
        dwTemp = PIC16F7x_wReadOscCalibWord;
      }
     dwTemp &= dwMask;                    // 12 bit only  !

     if(! PIC_PRG_iSimulateOnly)
      {
        // Steps 5..8 from Microchip's programming spec for the 16F7x:
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
        // The actual sequence will be:
        //    LOAD(even) -INCREMENT - LOAD(odd) - BEGIN_PROG - WAIT - END_PROG - INCREMENT
        // Sidenote: The alternative sequence for programming one memory word at a time
        //           didn't work for a 16F7x7(!), so we don't use it here as well .

        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadProg, false);  // "Load Data Command" (00010bin for PROGRAM)
        PIC_HW_SerialOut_14Bit(dwTemp);       // .. followed by data word (here: CODE MEMORY word
//        PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadProg,6,  dwTemp,14 );
        PIC_HW_SetClockAndData(false, false);

        //  "Two Loads Done ?"  [ future-compatible, WHO KNOWS with how many
        //                        "Words per Programming Cycle" Microchip
        //                        will come up next ?! GRRR ]
        // This only happens at OFF memory addresses (for nWordsPerProgCycle=2).
        // Example: 16F737, "TWO words per programming cycle" -> nWordsPerProgCycle=2 ->
        if(  ((iLocationCounter % nWordsPerProgCycle) == (nWordsPerProgCycle-1) )  // "odd" memory location ?
           ||((iLocationCounter+1)==PIC_DeviceInfo.lCodeMemSize)  )  // or "last location" ?
         {
          // Send command "Begin Programming Only Command" (16F7x7: BeginProgramming = ??1000bin = 0x08) :
          PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg, true );
          PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // "Wait tprog" (PIC16F74: 1 ms)
          // Caution: this command is most likely NOT internally timed, so don't violate Tprog !
          // Send command "End Programming Command" (16F7x: ??1110bin = 0x0E = 14 decimal) :
          PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg, true );
         }
      } // end if(! PIC_PRG_iSimulateOnly)
     if( PIC_PRG_iSimulateOnly )
      {
        if(  ((iLocationCounter % nWordsPerProgCycle) == (nWordsPerProgCycle-1) )  // "odd" memory location ?
           ||((iLocationCounter+1)==PIC_DeviceInfo.lCodeMemSize)  )  // or "last location" ?
         {
           PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // "Wait tprog" (PIC16F74: 1 ms)
         }
      } // end if( PIC_PRG_iSimulateOnly )

     // Send command "Increment Address Command" and keep track of the program counter:
     PIC16F7x_IncrAddress( 1/*iNrOfSteps*/ );  // "Increment Address Command" -> PIC16F7x_iCurrTargetAddress (!)

     APPL_ShowProgress( (100*iLocationCounter) / PIC_DeviceInfo.lCodeMemSize );
     if(APPL_iUserBreakFlag)
       { fOk = false;
         break;
       }
   } // end for < all CODE MEMORY locations >

  // Arrived here in [PIC16F7xps] flowchart figure 2-2, sheet 1: "all locations done".
  // Coming up next: "Verify all locations at Vdd = Vddapp" :
  // Verify... belongs to this PIC's "programming algorithm"
  //           because we cannot read directly after writing a single uint16_t !
  if( ! APPL_iUserBreakFlag )
   {
    APPL_ShowMsg( APPL_CALLER_PIC_PRG, 0,
                    _( "Verifying 0x%06lX..0x%06lX" ) ,
                   (long) 0/*device address*/, (long)(PIC_DeviceInfo.lCodeMemSize-1) );
    // To avoid exiting and re-entering programming mode,
    //  increment the program counter to let it WRAP(!) to 0x0000 .
    // The question is, will the PC alway wrap from 0x1FFF to 0x0000,
    //  (as stated in [PIC16F7xps] Chapter 2.1 "User Program Memory Map" )
    // or will it wrap from 0x0FFF for 4K-devices like PIC16F73/74 ?
    while( PIC16F7x_iCurrTargetAddress != 0 )
     { PIC16F7x_IncrAddress( 1/*iNrOfSteps*/ );  // "Increment Address Command" -> PIC16F7x_iCurrTargetAddress
     }
    // Arrived here, the PIC's program counter should be back at zero .
    // Now verify CODE memory :
    fOk &= PIC16F7x_VerifyRange( PIC_DeviceInfo.lCodeMemSize/*dwNrOfLocations*/ );
   } // end if < ok to verify ? >

  if( ! APPL_iUserBreakFlag )
   { // Arrived here in [PIC16F7xps] flowchart figure 2-2, sheet 1: "all locations done".
     // Coming up next: ID Locations, so: "Load Configuration (PC = 0x2000) ...
     if(! PIC_PRG_iSimulateOnly)
      { PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadConfig, false);  // internal address to 0x2000
        PIC_HW_SerialOut_14Bit(0x3FFF);  // ..immediately followed by a  DUMMY WORD (not suitable for 16F716 for this reason)
//        PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadConfig,6,  0x3FFF,14 );
        // > After receiving the Load Configuration command,
        // > the PC will be set to 0x2000 and the data sent with
        // > the command is discarded. The four ID locations and
        // > the configuration word can then be programmed using
        // > the normal programming sequence, as described in Section 2.3.
      }
     PIC16F7x_iCurrTargetAddress = 0x2000; // ignore PIC_DeviceInfo.lConfMemBase; ALWAYS 0x2000 in this case
//     PHWInfo.iCurrProgAddress = PIC16F7x_iCurrTargetAddress; // since 2008-05, for DLL-plugin

     // From [PIC16F7xps] flowchart figure 2-2, sheet 1: Program FOUR(!) ID Locations ( 2 * 2 )
     //  ( Vpp = 12.75 to 13.25 V  ---  why do they notice that in the flowchart ? )
     for ( iLocationCounter=0; iLocationCounter<=3; ++iLocationCounter )
      {
        PicBuf_GetBufferWord( PIC16F7x_iCurrTargetAddress, &dwTemp ); // here: ID LOCATION buffer
        if(! PIC_PRG_iSimulateOnly)
         { PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadProg, false);  // "Load Data Command" (00010bin here too)
           PIC_HW_SerialOut_14Bit(dwTemp);       // .. followed by data word (here: ID LOCATION word )
           PIC_HW_SetClockAndData(false, false);
//           PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadProg,6,  dwTemp,14 );

           //  "Two Loads Done ?" -> "Program Two ID Locations" (in figure 2-2 sheet 1)
           if( (iLocationCounter % nWordsPerProgCycle) == (nWordsPerProgCycle-1) )  // "odd" memory location ?
            {
              // Send command "Begin Programming Only Command" (16F7x7: ??1000bin = 0x08) :
              PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg, true);
              PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // "Wait tprog" (PIC16F74: 1 ms)
              // Send command "End Programming Command" (16F7x: ??1110bin = 0x0E = 14 decimal) :
              PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg,  true);
            }
         } // end if(! PIC_PRG_iSimulateOnly)
        // Send command "Increment Address Command" and keep track of the program counter:
        PIC16F7x_IncrAddress( 1/*iNrOfSteps*/ );  // "Increment Address Command" -> PIC16F7x_iCurrTargetAddress (!)
      } // end for < all ID LOCATIONS >

     // Arrived here in [PIC16F7xps] flowchart figure 2-2, sheet 1, label "A" .
     // Next: [PIC16F7xps] flowchart figure 2-2, sheet 2, "Increment Address to configuration word".
     // In fact, we are at TargetAddress = 0x2005 at this point, so increment TWICE here:
     PIC16F7x_IncrAddress( PIC_DeviceInfo.lConfWordAdr - PIC16F7x_iCurrTargetAddress /*iNrOfSteps*/ );

     // Next: "Load Data For Memory" (for config word; this may set code protection bits)
     PicBuf_GetBufferWord( PIC16F7x_iCurrTargetAddress, &dwTemp ); // here: CONFIG WORD (@0x2007)
     if(! PIC_PRG_iSimulateOnly)
      { PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadProg, false);  // "Load Data Command" (00010bin here too)
        PIC_HW_SerialOut_14Bit(dwTemp);       // .. followed by data word (here: ID LOCATION word )
        PIC_HW_SetClockAndData(false, false);
//        PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadProg,6,  dwTemp,14 );
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg, true ); //
        PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // "Wait tprog" (PIC16F74: 1 ms)
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg, true );
      }
     PicPrg_iConfWordProgrammed = 1;  // config word has been programmed now !

     // Next: "Verify all configuration memory locations" .
     if(! PIC_PRG_iSimulateOnly)
      { PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadConfig,false);  // this will hopefully take us back to 0x2000..
        PIC_HW_SerialOut_14Bit(0x3FFF);       // here: DUMMY WORD after "Load Config"
//        PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadConfig,6,  0x3FFF,14 );
      }
     PIC16F7x_iCurrTargetAddress = 0x2000; // ALWAYS 0x2000 in this case
//     PHWInfo.iCurrProgAddress = PIC16F7x_iCurrTargetAddress; // since 2008-05, for DLL-plugin
     PIC16F7x_IncrAddress( PIC_DeviceInfo.lConfWordAdr - PIC16F7x_iCurrTargetAddress /*iNrOfSteps*/ );
     fOk &= PIC16F7x_VerifyRange( 1/*dwNrOfLocations*/ );

   } // end if < ok to program the ID locations   AND   the config bits ? >


  if( ! PIC_PRG_iSimulateOnly )
   {
     PIC_HW_ProgModeOff();   // programming voltage off, clock off, data high-z
   }

  if(APPL_iUserBreakFlag)
     fOk = false;



  return fOk/*?*/ ;
} // end PIC16F7x_ProgramAll()


/* EOF < PIC16F7x_PRG.c > */

