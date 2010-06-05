/*-------------------------------------------------------------------------*/
/* PIC16F716_PRG.c                                                         */
/*                                                                         */
/*  Purpose: Serial PIC Programming algorithms for PIC16F716 (in WinPic).  */
/*  Author : Wolfgang Buescher (DL4YHF)                                    */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*                                                                         */
/*  Date   : 2005-12-03  (ISO 8601 format, YYYY-MM-DD)                     */
/*  State  : UNDER DEVELOPMENT - NOT TESTED BY THE AUTHOR !                */
/*  2009-06-04:  Ported to wxWidget by Philippe Chevrier                   */
/*                                                                         */
/*  Literature:                                                            */
/*   [PIC16F716ps] = "PIC16F716 FLASH Memory Programming Specification"    */
/*                Microchip DS40245B (or later ? )                         */
/* C A U T I O N :  The "PIC16F716" prog spec does not cover anything else.*/
/*               We don't know why Microchip produced an extra document    */
/*               for the PIC16F716 so it may be totally different from all */
/*               other PIC's ... easier to write a new module instead of   */
/*               wasting a lot of time to search for "similarities". Grrr. */
/*-------------------------------------------------------------------------*/

#define L_RELEASE_VERSION 0

  // Revsion history - NEWEST entry first !
  //
  // 2005-12-03:  Added this module after someone found out that the
  //     CONFIG MEMORY PROGRAMMING method used for other PICs didn't work
  //     for a PIC16F716 . To avoid trashing other subroutines in PIC_PRG.C ,
  //     this new module named PIC16F716_PRG.C was added to WinPic .
  // Based on PIC16F7x_PRG.C , and then modified according to [PIC16F716ps] .
  //

#include <wx/intl.h>
#include <stdio.h>    // sprintf(), NULL, ..

#include "PIC_HW.h"     // Routines to drive the programmer hardware interface
#include "devices.h"    // need T_PicDeviceInfo here !
#include "config.h"     // permanently saved Config-structure
#include <Wx/Appl.h>       // APPL_ShowMsg(), etc
#include "Pic_Hex.h"    // access routines to the "hex data buffer"
#include "PIC_PRG.H"    // generic defines for programming routines
#include "PIC16F716_PRG.h" // header for the PIC16F716 programming algorithm


//----------------------------------------------------------------------------
//  Implementation
//----------------------------------------------------------------------------

uint32_t PIC16F716_dwCurrAddr;

//----------------------------------------------------------------------------
void PIC16F716_IncAddr(void)
{
//  PHWInfo.iCurrProgAddress = PIC16F716_dwCurrAddr;
  PIC_HW_SerialOut_Command6( 0x06, true ); // 2. Increment address (from 0x2000 to 0x2001)
  ++PIC16F716_dwCurrAddr;
//  PHWInfo.iCurrProgAddress = PIC16F716_dwCurrAddr;
} // end PIC16F716_IncAddr()


//----------------------------------------------------------------------------
void PIC16F716_EraseAll(void)
{
  if(! PIC_PRG_iSimulateOnly)
   { PIC_HW_ProgMode();          // Enter programming mode (first Vdd on, then Vpp on)
//     PHWInfo.iCurrProgAddress = PIC16F716_dwCurrAddr = 0x0000;
     // "Bulk Erase" for PIC16F716 . Caution, to erase the whole damn chip,
     // the PC must point into the configuration memory as explained in [PIC16F716ps] :
     // > To perform a bulk erase of the program memory, user
     // > ID’s and configuration word, the following sequence
     // > must be performed.
     // > 1. Perform a Load Configuration command.
     // > 2. Perform a Bulk Erase Program Memory
     // > command.
     // > 3. Wait TERA to complete bulk erase.
     // > If the PC is pointing to the configuration program
     // > memory (0x2000 - 0x2007), then the program memory,
     // > configuration word, and user ID locations will all be
     // > erased. If the PC is in user memory space (0x0000 -
     // > 0x1FFF) only program memory and the configuration
     // > word will be erased.
     PIC_HW_SerialOut_Command6(0x00, false);  // 1. "Load Config" sets internal address to 0x2000
     PIC_HW_SerialOut_14Bit(0x3FFF);   //     .. followed by a DUMMY WORD
//     PicHw_FlushCommand( 0x00,6,  0x3FFF,14 );
     PIC_HW_SerialOut_Command6(0x09, true);  // 2. "Bulk Erase" for PIC16F716
     PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Erase_us); // 3. Wait TERA (at least 6 ms for a PIC16F716)
     PIC_HW_ProgModeOff();   // programming voltage off, clock off, data high-z

   } // end if(! PIC_PRG_iSimulateOnly)
} // end PIC16F716_EraseAll()


//----------------------------------------------------------------------------
bool PIC16F716_WriteCodeMemory(
      uint32_t *pdwSourceData,  // pointer to source buffer
        int n ,              // number of WORDS to be programmed
      uint32_t dwDeviceAddress )  // device specific "base"-address, should be 0x0000 for code memory
   // - Does *NOT* include ERASE !
   // - Does *NOT* include VERIFY !
   // - Writes ONLY CODE MEMORY, but neither ID LOCATIONS nor the CONFIG WORD
   // - Caution: writes FOUR(!) words at a time
{
 uint32_t dwWritten;           // destination for memory word from WinPic's buffer module (must be uint32_t)
// int   i;
 int   iLocationCounter, iLoadCounter;
// char  sz80Msg[84];

  if( Appl_CheckUserBreak() )
     return false;

  // If not already erased by caller, erase code memory before programming :
  if( ! PicPrg_iCodeMemErased )
   { PIC_PRG_Erase( PIC_ERASE_ALL );  // not necessarily "PIC16F716_EraseAll()" !
   }

  // Write CODE MEMORY - see [PIC16F716ps] :
  //   FIGURE 2-11: FOUR-WORD PROGRAM FLOW CHART - PIC16F716 PROGRAM MEMORY
  PIC_HW_ProgMode();          // Enter programming mode (first Vdd on, then Vpp on)
//  PHWInfo.iCurrProgAddress = PIC16F716_dwCurrAddr = 0x0000;
  iLoadCounter = 0;
  if( n > PIC_DeviceInfo.lCodeMemSize )
      n = PIC_DeviceInfo.lCodeMemSize;
  for ( iLocationCounter=0; iLocationCounter<n; ++iLocationCounter )
   {
     dwWritten = pdwSourceData[iLocationCounter];
     if(! PIC_PRG_iSimulateOnly)
      {
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadProg,false); // "Load Data for program memory" (00010bin = 2dec)
        PIC_HW_SerialOut_14Bit( dwWritten ); // .. followed by data word
        PIC_HW_SetClockAndData(false, false);
//        PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadProg,6,  dwWritten,14 );

        //
        ++iLoadCounter;
        if( (iLoadCounter>=4) || ((iLocationCounter+1)==n) )    //  "Four Loads Done ?" (or last location)
         {
          PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg,true); // "Begin Programming" (11000bin = 24dec)
          PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // "Wait tprog1" (16F716: 2000 us though "typically 1 ms ?!?" )
          PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg,true);   // "End Programming" (*)
          // (*) End Programming is 01110bin=14dec for 16F716, but 10111bin=23dec for many other chips ? ?
          PIC_HW_Delay_us( 200 );  // "Wait Tdis" (16F716: min 100 us)
          iLoadCounter = 0;  // wait for another "four loads" now
         }

        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_IncrAddr, true);  // "Increment Address Command"
      }
     APPL_ShowProgress( (100*iLocationCounter) / n );
     if(APPL_iUserBreakFlag)
       break;
   } // end for(iLocationCounter..)

  PIC_HW_ProgModeOff();   // programming voltage off, clock off, data high-z

  return ( ! APPL_iUserBreakFlag );

} // end PIC16F716_WriteCodeMemory()


//----------------------------------------------------------------------------
bool PIC16F716_WriteConfigMemory(
      uint32_t *pdwSourceData,    // pointer to source buffer
        int n,                 // number of WORDS to be programmed (including the gaps), should be 8 (0x2000..0x2007)
      uint32_t dwDeviceAddress )  // device specific "base"-address, should be 8192=0x2000
   // For PIC16F716, this includes USER ID LOCATIONS and the "CONFIG WORD" !
{
  int i;

  i = 0;
  if( (dwDeviceAddress == 0x2000)
     && ( (n==4) || (n==8) ) ) // write FOUR or EIGHT words from addr 0x2000 ..
   { i=1;  //  (i.e. 4 ID locations and possibly the "config word" too )
   }
  if( (dwDeviceAddress == 0x2007) && (n==1) ) // only write ONE word from addr 0x2007
   { i=1;  //  (i.e. "config word ONLY" )
   }
  if( !i )
   { APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
       _( "PIC16F716_WriteConfigMemory() Error: wrong start addr(0x%04lX) or number of words(%ld)" ),
       (long)dwDeviceAddress,  (long)n );
     return false;
   }

  if( (! APPL_iUserBreakFlag ) && (! PIC_PRG_iSimulateOnly) )
   { // Next: [PIC16F716s] Chapter 2.3.3.1 "LOAD CONFIGURATION" ...
     PIC_HW_ProgMode();          // Enter programming mode (first Vdd on, then Vpp on)
//     PHWInfo.iCurrProgAddress = PIC16F716_dwCurrAddr=0x0000;
     if( (dwDeviceAddress == 0x2000) && (n>=4) )
      {
#if(0)  // the following failed FAR TOO OFTEN ... for unknown reasons !
        // > The following sequence may be used to program the 4
        // > user ID locations and the configuration word:
        // > 1 - Send Load Configuration command with first word of user ID data
        // > 2 - Send Increment Program Counter command
        // > 3 - Send Load Data for Program Memory command with second word of user ID data
        // > 4 - Send Increment Program Counter command
        // > 5 - Send Load Data for Program Memory command with third word of user ID data
        // > 6 - Send Increment Program Counter command
        // > 7 - Send Load Data for Program Memory command with fourth word of user ID data
        // > 8 - Send Begin Programming command
        // > 9 - Wait TPROG then send End Programming command
        // > 10 - Wait TDIS then send Increment Address command 4 times
        // > 11 - Send Load Data for Program Memory commanders configuration word data
        // > 12 - Send Begin Programming command
        // > 13 - Wait TPROG then send End Programming command
        PIC_HW_SerialOut_Command6( 0x00 );  // 1. "Load Config" sets internal address to 0x2000
        PHWInfo.iCurrProgAddress = PIC16F716_dwCurrAddr = 0x2000;
        PIC_HW_SerialOut_14Bit( pdwSourceData[0] ); //    ...with first word of user ID data
        PIC16F716_IncAddr();                // 2. Increment address (from 0x2000 to 0x2001)
        PIC_HW_SerialOut_Command6( 0x02 );  // 3. Send Load Data(!) for Program Memory command
        PIC_HW_SerialOut_14Bit( pdwSourceData[1] ); //    ...with second word of user ID data
        PIC16F716_IncAddr();                // 4. Send Increment Program Counter command
        PIC_HW_SerialOut_Command6( 0x02 );  // 5. Send Load Data(!) for Program Memory command
        PIC_HW_SerialOut_14Bit( pdwSourceData[2] ); //    ...with third word of user ID data
        PIC16F716_IncAddr();                // 6. Send Increment Program Counter command
        PIC_HW_SerialOut_Command6( 0x02 );  // 7. Send Load Data(!) for Program Memory command
        PIC_HW_SerialOut_14Bit( pdwSourceData[3] ); //    ...with fourth word of user ID data
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg); // 8. "Begin Programming" (11000bin = 24dec)
        PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us); // 9. Wait TPROG then send End Programming command
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg); // 9b. "End Programming" (*)
        // (*) "End Programming" is 01110bin=14dec for 16F716, but 10111bin=23dec for many other chips ?
        PIC_HW_Delay_us( 100 );               // 10. Wait TDIS ...
        PicPrg_iConfMemVerified = 0; // have written a part of the config memory now but not verified yet
        if( n>4 )  // also write the CONFIG WORD in the same over ?
         {
           while(PIC16F716_dwCurrAddr<0x2007) // .. then send Increment Address command 4 times :
            { PIC16F716_IncAddr();            // Inc Address ( 8195=0x2003 -> 8199=0x2007 )
            }
           // 11. Send Load Data for Program Memory commanders configuration word data (??) :
           PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadProg);  // "Load Data for program memory" (00010bin = 2dec)
           PIC_HW_SerialOut_14Bit( pdwSourceData[7] );   // config word from address 0x2007
           PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg); // 12. "Begin Programming" (11000bin = 24dec)
           PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us); // 13. Wait TPROG then send End Programming command
           PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg);   // 13b. "End Programming" (*)
           PicPrg_iConfWordProgrammed = 1;  // config word has been programmed now ,
           PicPrg_iConfWordVerified   = 0;  // ...  but not verified yet.
         } // end if < program ID locations *AND* config word > ?
#else   // try ONE instead of FOUR locations of config memory in a single over ?
        PIC_HW_SerialOut_Command6( 0x00, false ); // 1. "Load Config" sets internal address to 0x2000
        PIC_HW_SerialOut_14Bit( 0x3FFF );  //    ...with an "unprogrammed data" word (0x3FFF)
//        PicHw_FlushCommand( 0x00,6,   0x3FFF,14 );
//        PHWInfo.iCurrProgAddress = PIC16F716_dwCurrAddr = 0x2000;
        while(PIC16F716_dwCurrAddr<dwDeviceAddress) // skip unwanted addresses..
         { PIC16F716_IncAddr(); // Inc Address ( 0x2000 -> 0x200? ), keeps PIC16F716_dwCurrAddr up-to-date
         }
        for( i=0; i<n; ++i )                        // write wanted locations..
         {
           if( (PIC16F716_dwCurrAddr>=0x2000 && PIC16F716_dwCurrAddr<=0x2003)
             ||(PIC16F716_dwCurrAddr==0x2007 ) )
            { // only if this a "programmable" location in the PIC16F716's config memory :
              PIC_HW_SerialOut_Command6( 0x02, false );   // Send Load Data(!) for Program Memory command
              PIC_HW_SerialOut_14Bit( pdwSourceData[i] ); //    ...with the desired word
//              PicHw_FlushCommand( 0x02,6,   pdwSourceData[i],14 );
              PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg, true); // "Begin Programming"
              PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);                    //  Wait TPROG ..
              PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg, true );  // then send "End Programming"
              PIC_HW_Delay_us( 100 );          // wait TDIS
              if( PIC16F716_dwCurrAddr==0x2007 ) // was this the "config word" ?
               {
                PicPrg_iConfWordProgrammed = 1;  // config word has been programmed now ,
                PicPrg_iConfWordVerified   = 0;  // ...  but not verified yet.
               }
              else // not the "config word", but another "writeable" location :
               {
                PicPrg_iConfMemProgrammed = 1;  // something else in "config memory" was programmed, but..
                PicPrg_iConfMemVerified   = 0;  // not been verified yet (signal for caller)
               }
            } // end if < "writeable" location in config memory ? >
           PIC16F716_IncAddr(); // Inc Address ( 0x2000 -> 0x200? ), keeps PIC16F716_dwCurrAddr up-to-date
         } // end for
#endif  // use FOUR(--) or ONE(++) locations of config memory in a single over ?
      }
     else  // only ONE location:
      {
        // >  The following sequence is recommended when reprogramming
        // >  the configuration word only :
        // >  * Send Load Configuration command with an
        // >    unprogrammed data word (i.e., 0x3FFF).
        // >  * Send Increment Program Counter command
        // >    seven times.
        // >  * Send Load Data For Program Memory command
        // >    with the desired configuration word.
        // >  * Send Begin Programming command
        // >  * Wait TPROG then send End Programming command.
        // >  * Wait TDIS before the next action.
        PIC_HW_SerialOut_Command6( 0x00, false ); // 1. "Load Config" sets internal address to 0x2000
        PIC_HW_SerialOut_14Bit( 0x3FFF );  //    ...with an "unprogrammed data" word (0x3FFF)
//        PicHw_FlushCommand( 0x00,6,   0x3FFF,14 );

//        PHWInfo.iCurrProgAddress = PIC16F716_dwCurrAddr = 0x2000;
        while(PIC16F716_dwCurrAddr<0x2007) // send Increment Address command 7 times :
            { PIC16F716_IncAddr();         // Inc Address ( 0x2000 -> 0x2007 )
            }
        PIC_HW_SerialOut_Command6( 0x02, false );  // Send Load Data(!) for Program Memory command
        PIC_HW_SerialOut_14Bit( pdwSourceData[0] ); //    ...with the desired config word
//        PicHw_FlushCommand( 0x02,6,   pdwSourceData[0],14 );

        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg,true); // "Begin Programming"
        PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);                   //  Wait TPROG ..
        PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_EndProg,true);   // then send "End Programming"
        PIC_HW_Delay_us( 100 );          // wait TDIS
        PicPrg_iConfWordProgrammed = 1;  // config word has been programmed now ,
        PicPrg_iConfWordVerified   = 0;  // ...  but not verified yet.
      }



     // Since the config word with the CP bits has been programmed now,
     // so there is no way to verify CODE memory programming after this point .
     // But the CONFIG memory will be verified by the caller, so don't
     // turn the programming voltage off (because of problems with "JDM 2" ).
     // ex: PIC_HW_ProgModeOff();  // programming voltage off, clock off, data high-z

   } // end if < ok to program the ID locations   AND   the config bits ? >

  return (! APPL_iUserBreakFlag);
} // end PIC16F716_WriteConfigMemory()



/* EOF < PIC16F716_PRG.c > */



