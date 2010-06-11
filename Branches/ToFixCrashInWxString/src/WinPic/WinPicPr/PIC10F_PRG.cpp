/*-------------------------------------------------------------------------*/
/* PIC10F_PRG.cpp                                                          */
/*                                                                         */
/*  Purpose: Serial PIC Programming algorithms for PIC10F20x (in WinPic).  */
/*  Author : Wolfgang Buescher (DL4YHF)                                    */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*                                                                         */
/*  Date   : 2005-05-18  (ISO 8601 format, YYYY-MM-DD)                     */
/*  State  : UNDER DEVELOPMENT - NOT WORKING PROPERLY YET !                */
/*                                                                         */
/*  2009-06-04:  Ported to wxWidget by Philippe Chevrier                   */
/*                                                                         */
/*  Literature:                                                            */
/*   [PIC10Fps] = "PIC10F200/202/204/206 Memory Programming Specification" */
/*                Microchip DS41228D (or later ? )                         */
/*-------------------------------------------------------------------------*/

#define L_RELEASE_VERSION 0

  // Revsion history - NEWEST entry first !
  //
  // 2005-05-01:  Tried to add support for a PIC10F20x after receiving(!)
  //     a couple of samples from Johan Bodin (SM6LKM), not from Microchip(!) .
  //     Because the PIC10F20x programming algorithms are TOTALLY different
  //     from everything else (what would you expect..), the "new"
  //     algorithms are located in PIC10F_PRG.cpp .
  //

#include <wx/intl.h>
#include "PIC_HW.h"     // Routines to drive the programmer hardware interface
#include "Devices.h"    // need T_PicDeviceInfo here !
#include "Config.h"     // permanently saved Config-structure
#include <Wx/Appl.h>       // APPL_ShowMsg(), etc
#include "PIC_HEX.h"    // access routines to the "hex data buffer"
#include "PIC_PRG.h"    // generic defines for programming routines
#include "PIC10F_PRG.h" // header for the PIC10F20x programming algorithms


//----------------------------------------------------------------------------
//  Variables
//----------------------------------------------------------------------------

int  PIC10F_iCurrTargetAddress;   // current target address ("PC" inside the PIC)
                                  // INCREMENTED IN PIC10F_IncrAddress() ONLY !
static int  PIC10F_iMaskAddress;  // added by L.Lisovskiy 2006-03-28 to support PIC12F508
int  PIC10F_iNrOfErrors;
uint16_t PIC10F_wTempConfigWord;      // config word, read before erase
uint16_t PIC10F_wReadOscCalibWord;    // oscillator calibration word, read before erase
uint16_t PIC10F_wReadOscCalibBackup;  // backup oscillator calibration word, read before erase


//----------------------------------------------------------------------------
//  Low-level stuff to communicate with the PIC10F20x via ICSP .
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
bool PIC10F_EnterProgMode( void )
  // Enter programming mode for PIC10F .
  // > The Program/Verify mode is entered by holding ICSPCLK and ICSPDAT low
  // > while raising Vdd pin from VIL to VDD (YHF: 0 to 5 V) .
  // > Then raise Vpp from VIL ("zero") to VIHH (12 V).
  //
  // This is almost the same as entering "programming mode"
  // for the PIC16Fxxx-family, with Config.iNeedPowerBeforeRaisingMCLR = true (!).
{
//  int i;

//  PHWInfo.iCurrProgAddress = 0x0000; // added 2008-05-18 for the plugin-DLLs

  // ex: PIC_HW_ProgMode();  // raise Vpp to 12 V while CLOCK+DATA = low .

  // Added 2004-05-30, due to problems with JDM programmer with 16F877A
  //                   when programming+verify in two separate overs .......
  if( PIC_HW_interface.type==PIC_INTF_TYPE_JDM2 )
   { // In the "JDM", things may be different, because it is impossible here
     // to turn the PIC's supply voltage quickly on.
     // Here, for the PIC10F (in contrast to many others), the programming spec
     //  clearly shows "Vdd turned on before Vpp" . How to achieve this with the JDM interface :

      // Make sure C2 (100uF) is charged to 8 volts  and  C3 (22uF)
      // charged to 5 volts (=negative bias voltage between PIC's Vss and PC's GROUND).
      PIC_HW_SetClockAndData(false,false);  // -> RTS=-12V, DTR=-12V
      PIC_HW_SetVpp( true );                // -> TXD=+12V , charges C2 (100uF) to ~13.2 volts
         // this is quite fast, because no resistors are involved:  13V*100uF/5mA = 260ms
      PIC_HW_LongDelay_ms(500/*ms*/); // give the CPU to another thread to make windoze happy while charging C2
      // Now pull Vpp / Vpp low, so the PIC enters the RESET state .
      PIC_HW_SetVpp( false );               // -> TXD= -12V (hard limited to -5.7V), charges C3 to 5 volts
                                            //        while C2 remains at 8 volts.
      PIC_HW_Delay_us(10000); // 10ms between exiting and re-entering prog mode with Vdd AND Vpp at GND
      PIC_HW_SetVpp(true);  // Now there should be a faster rise on Vpp = "Vpp",
                            // from 0 V to almost 13 V (from the PIC's point of view).
        // For many PIC devices, this rise time must not exceed 1 MICROSECOND,
        // which is the reason why some ultra-simple (and ugly) programmers fail .
        // Measured on an oscilloscope, where GROUND is connected to the PC's ground!!
        // (even if you don't want it that way), Vpp rises from -5 to +8 volts.
      // After Vpp rised from 0 to 12 volts..
      PIC_HW_Delay_us(2);   // wait "P7" = 2 us in STDP mode, before the 1st clock pulse

   } // end if < JDM programmer >
  else
   { //----------------------- ALL OTHER PROGRAMMERS (except "JDM") --------------------------
       // PIC10Fxxx definitely requires "Vdd turned on before Vpp" . So :
       PIC_HW_SetClockAndData(false,false);
       PIC_HW_SetClockEnable(true );  // Enable the tristate buffer outputs (PC->PIC)
       PIC_HW_SetDataEnable( true );  // (up to now, only required for AN589 programmer)
       PIC_HW_SetClockAndData(false,false);  // both CLOCK and DATA LOW to enter prog mode

       // don't ask why this WAS done THREE times (one time must work) ..
       // for(i=0; i<3; ++i)
        {

          PIC_HW_SetVpp(false);          // ensure Vpp is off
          PIC_HW_SetVdd(false);          // ensure Vdd is off
          PIC_HW_PullMclrToGnd(true);    // tie Vpp to ground (not many adapters support this)
          PIC_HW_LongDelay_ms(500);      // wait a LOOONG time to discharge various caps...
          // Because some "ugly style" programmers switch Vdd together with Vpp,
          // turn Vpp off for a very short time to generate a steep
          // rise on Vpp while Vdd is still present :
          if( PIC_HW_interface.wSeparateVddAndVppControl )  // 0=no, 1=yes
           {
#if(1)      // time between Vdd and Vpp seemed a bit too long, due to the terribly slow I/O-port driver
            PIC_HW_SetVdd(true);    // turn PIC's supply voltage on
            // wait a short time before raising Vpp (*AFTER Vdd*)
            if( Config.iSlowInterface )  // this option for "bad, slow interfaces" was added 2005-06
                 PIC_HW_Delay_us(20);
            else PIC_HW_Delay_us(5);
              // The PIC10F20x prog spec says "Tppdp MIN 5us" but not too clear what it is !
              // (Fig 3-1 shows Tppdp between Vdd rise and Vpp rise, but Table 6-1 says
              //   >  Tppdp = "Hold time *AFTER* Vpp up " (did they mean "Vdd up" ? - bs...@#! )
              // What happens if Vdd is applied, and Vpp rises much later, and MCLR is disabled ?
              // PIC will start running happily with its fast-starting 4-MHz-RC-oscillator !
              // In fact, entering programming mode with a PIC10F206 sample was VERY unreliable .
            PIC_HW_PullMclrToGnd(false);    // stop pulling MCLR to ground (added 2008-05-29)
            PIC_HW_SetVpp(true);    // raise Vpp from 0 V to 13 V within a VERY short time
#else       // Time between Vdd and Vpp seemed a bit too long, due to the terribly slow I/O-port driver,
            // so tried this in 2005-05-18, more reliable programming mode entry ?
            // Test result: DIDN'T WORK AT ALL ! PIC10F20x really seems to NEED Vdd BEFORE Vpp !
            PIC_HW_SetVddAndVpp(true,true);    // turn PIC's supply voltage AND programming voltage on (simultaneously)
#endif
            // [PIC10Fps] requires MCLR rise time for Program/Verify mode entry
            //            must be MAXIMUM 1.0 use (yes, ONE MICROSECOND) .
            //  Caution,  not all programming interfaces can handle this !
           }
          else  // Need power before raising Vpp ,  but no separate Vdd and Vpp control lines :
           {    // (like in the COM84 adapter, which has a Vpp(!) but no Vdd(!) control line)
            PIC_HW_LongDelay_ms(200);  // wait until Vpp is really 0 volt
            PIC_HW_SetVdd(true);    // turn PIC's supply voltage on (no effect for COM84)
            if( Config.iSlowInterface )
                 PIC_HW_Delay_us(20);
            else PIC_HW_Delay_us(5); // see WoBu's rant about lousy programming specs above !
            PIC_HW_PullMclrToGnd(false); // stop pulling MCLR to ground (added 2008-05-29)
            PIC_HW_SetVpp(true);     // an utterly slow rise on Vpp (because of discharged cap)
            if( PicHw_fTogglingTxD ) // If there is a charge pump for "Vpp", keep toggling a bit longer...
             {
              PIC_HW_LongDelay_ms(500/*ms*/);  // while WAITING here, feed the UART's transmit buffer (!)
             }
            else // if there is NO charge pump for "Vpp", use the typical "Vpp-ON-delay",
             {   // then turn Vpp off for a short time, and turn it on to produce a FAST RISE on Vpp:
              PIC_HW_LongDelay_ms(200); // time required by INTERFACE (something like 200ms)
              PIC_HW_SetVpp(false);   // Vdd(!) capacitor should be charged now,
                                      //   the next positive slope on Vpp will be steeper
              PIC_HW_Delay_us(10000); // 10ms between exiting and re-entering prog mode with Vdd AND Vpp at GND
              PIC_HW_PullMclrToGnd(false); // stop pulling MCLR to ground (added 2008-05-29)
              PIC_HW_SetVpp(true);    // now this should be a faster rise on Vpp
             }
           } // end < no separate Vdd and Vpp control lines >
        } // end for < try 2 enter prog mode  ONE or THREE times ? >
   } // end if < not "JDM" programmer >

  // caller must not forget to turn Vpp & Vdd off after he's ready !

  // Program/Verify mode entry will set the address pointer
  //          to 0x1FF for the PIC10F200/204
  //          or 0x3FF for the PIC10F202/206
  //          or 0x7FF for the PIC12F509      !
  //  .. which means, the PC points to the CONFIGURATION WORD now .
  // Modified by L.Lisovskiy 2006-03-28 to support PIC12F508
  // EX:  if( PIC_DeviceInfo.lCodeMemSize > 256 )
  //        PIC10F_iCurrTargetAddress = 0x03FF;  // for PIC10F202 / 206
  //  else  PIC10F_iCurrTargetAddress = 0x01FF;  // for PIC10F200 / 204
  PIC10F_iMaskAddress = ((PIC_DeviceInfo.lCodeMemSize&0xFF80)<<1) - 1;
  PIC10F_iCurrTargetAddress = PIC10F_iMaskAddress;

//  PHWInfo.iCurrProgAddress = PIC10F_iCurrTargetAddress; // added 2008-05-18 for the plugin-DLLs

  return true;
} // end PIC10F_EnterProgMode()

//----------------------------------------------------------------------------
void PIC10F_LeaveProgMode( void )
{
   PIC_HW_ProgModeOff();  // programming voltage off, clock off, data high-z
}


//----------------------------------------------------------------------------
void PIC10F_SendCmd6(uint8_t bCmd6,  bool fFlush)
  // Sends a 6-bit command to the PIC in ICSP mode .
{
 int i, data_bit;
 if(PIC_PRG_iSimulateOnly)
    return;
  // Based on [PIC10Fps], Chapter 3.1.2 "Serial Program/Verify Operation" .
  PIC_HW_SetClockAndData(false,false );  // start condition
  PIC_HW_SetClockEnable( true );         // some interfaces may need this..
  PIC_HW_SetDataEnable( true );   // "Enable" signals only for AN589 programmer
  if( Config.iSlowInterface )
       PIC_HW_Delay_us(100);
  else PIC_HW_Delay_us(1);        // wait at least TDLY2 from last edge of previous cmd

  for ( i=0; i<6; ++i )           // send 6-bit command, LSB first..
   {
     data_bit = (bCmd6&(1<<i)) != 0;     // get next command bit (0 or 1)
     PIC_HW_SetClockAndData(true, data_bit);
     if( Config.iSlowInterface )
          PIC_HW_Delay_us(50);    // extra long delay for "bad" interfaces
     else PIC_HW_Delay_us(1);     // normal mode for "good" interfaces
     PIC_HW_SetClockAndData(false,data_bit); // bit latched on falling edge !  so data must remain valid !
     if( Config.iSlowInterface )
          PIC_HW_Delay_us(50);    // extra long delay for "bad" interfaces
     else PIC_HW_Delay_us(1);     // normal mode for "good" interfaces
   }
  // Added 2008-05-19: Let "smart" interfaces flush the clock- and data-transitions.
  //  This may be required for some programmers with USB interface which use burst-transfers.
//  PicHw_FlushCommand( bCmd6,6,    0,0/*no data*/  );

  PIC_HW_Delay_us(1);             // delay required between command/command or command/data
} // end PIC10F_SendCmd6()


//----------------------------------------------------------------------------
void PIC10F_SendData14(uint16_t wData14)
  // Sends a 14-bit data word ,
  //  of which only 12 bit are used, because the PIC10F has a 12-bit core .
{
 int i, data_bit;
 if(PIC_PRG_iSimulateOnly)
    return;
  // Based on [PIC10Fps], Chapter 3.1.2 "Serial Program/Verify Operation" .
  PIC_HW_SetClockAndData(false,false );  // start condition
  PIC_HW_SetClockEnable( true );         // some interfaces may need this..
  PIC_HW_SetDataEnable( true );          // "Enable" signals only for AN589 programmer
  PIC_HW_SetClockAndData(true, false);   // Send START bit ..
  if( Config.iSlowInterface )
       PIC_HW_Delay_us(50);    // extra long delay for "bad" interfaces
  else PIC_HW_Delay_us(1);     // specified delay for "good" interfaces
  PIC_HW_SetClockAndData(false,false);   // > input data is "don't care" for start bit
  if( Config.iSlowInterface )
       PIC_HW_Delay_us(50);    // extra long delay for "bad" interfaces
  else PIC_HW_Delay_us(1);     // specified delay for "good" interfaces
  // Now send 14 data bits, LSB first, the two last bits are ignored for this 12-bit core :
  for ( i=0; i<14; ++i )          // send 6-bit command, LSB first..
   {
     data_bit = (wData14&(1<<i)) != 0;     // get next data bit (0 or 1)
     PIC_HW_SetClockAndData(true, data_bit); // send data bit
     if( Config.iSlowInterface )
          PIC_HW_Delay_us(50);    // extra long delay for "bad" interfaces
     else PIC_HW_Delay_us(1);     // specified delay for "good" interfaces
     PIC_HW_SetClockAndData(false,data_bit); // bit latched on falling edge !  so data must remain valid !
     if( Config.iSlowInterface )
          PIC_HW_Delay_us(50);    // extra long delay for "bad" interfaces
     else PIC_HW_Delay_us(1);     // specified delay for "good" interfaces
   }
  PIC_HW_SetClockAndData(true, false);   // Send STOP bit ..
  if( Config.iSlowInterface )
       PIC_HW_Delay_us(50);       // extra long delay for "bad" interfaces
  else PIC_HW_Delay_us(1);        // specified delay for "good" interfaces
  PIC_HW_SetClockAndData(false,false);   // > input data is "don't care" for stop bit
  if( Config.iSlowInterface )
       PIC_HW_Delay_us(50);
  else PIC_HW_Delay_us(1);
} // end PIC10F_SendData14()

//----------------------------------------------------------------------------
uint16_t PIC10F_ReadDataFromMemory(void)
  // Reads a 14-bit data word from the current memory location INSIDE THE PIC .
  //  Only 12 bits are meaningful, because the PIC10F has a 12-bit core .
  // The internal address counter is not affected by this command !
{
  if( PIC_PRG_iSimulateOnly )
   { // SIMULATE ONLY -> return some dummy to play with
     return PIC10F_iCurrTargetAddress & 0x0FFF;
   }
  else
   { PIC10F_SendCmd6( 0x04, true );   // send 6-bit command "Read Data From Program Memory", and flush the queue
     return PIC_HW_SerialRead_14Bit() & 0x0FFF; // routine for 14-bit core should work for this too (?)
   }
} // end PIC10F_ReadDataFromMemory()

//----------------------------------------------------------------------------
void PIC10F_IncrAddress(int iNrOfSteps)
  // increments the PIC's internal address pointer,
  // but also PIC10F_iCurrTargetAddress (our "local copy") .
{

  if(iNrOfSteps<0)  // added by L.Lisovskiy 2006-03-28
   {
     APPL_ShowMsg( APPL_CALLER_PIC_PRG,0, _("PIC10F Prog: INVALID address increment=%ld !"),  iNrOfSteps);
     return;
   }

  while(iNrOfSteps--)
   {
     if( ! PIC_PRG_iSimulateOnly )
         PIC10F_SendCmd6( 0x06, true );  // send 6-bit command "Increment Address"
     ++PIC10F_iCurrTargetAddress;  // keep track of the PIC's internal instruction pointer ("PC")..
//     PHWInfo.iCurrProgAddress = PIC10F_iCurrTargetAddress; // added 2008-05-18 for the plugin-DLLs
        // Example for PIC10F200/204 :
        // Lousy spec, about the "program memory map" in chapter 2.1 ...
        // > " The PC will increment from 0x000-0x0FF, then to 0x100 , not to 0x000 "
        // Does this apply to PROGRAMMING MODE ?!  Obviously not, because IF,
        //    there would be no chance to set the PC to zero !
     // EX: if( PIC_DeviceInfo.lCodeMemSize<=256 )
     //      { PIC10F_iCurrTargetAddress &= 0x1FF;
     //      }
     //     else
     //      { // here, the PC should wrap from 0x3FF to 0x000
     //        // (not to 0x200 as the lousy spec seemed to say) ...
     //        PIC10F_iCurrTargetAddress &= 0x3FF;
     //      }
     // more "universal" address wrap by L.L., works for 12F509 too:
     PIC10F_iCurrTargetAddress &= PIC10F_iMaskAddress;
//     PHWInfo.iCurrProgAddress = PIC10F_iCurrTargetAddress; // added 2008-05-18 for the plugin-DLLs

   }
}

//----------------------------------------------------------------------------
void PIC10F_CompareToVerify( uint16_t wReadValue, uint16_t *pwMemFlags )
{
  uint32_t dwTemp;

  if( PicBuf_GetBufferWord( PIC10F_iCurrTargetAddress, &dwTemp ) )
   { if( (uint32_t)(wReadValue&0x0FFF) != (uint32_t)(dwTemp&0x0FFF) )
      {
        if ( PIC10F_iCurrTargetAddress!=PIC_DeviceInfo.lAddressOscCal
           || Config.iDontCareForOsccal ) // ok to complain about THIS location ?
         {
           ++PIC10F_iNrOfErrors;
           *pwMemFlags |= PIC_BUFFER_FLAG_VFY_ERROR;
           if( PIC10F_iNrOfErrors < 20 )
            { APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
                _( "Verify Error: %06lX: read %06lX, wanted %06lX" ),
                (long)PIC10F_iCurrTargetAddress, (long)wReadValue, (long)dwTemp );
            }
         }
        else // oscal value different from buffer contents ..
         { APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
                _("Info: different OSCCAL values: read=%06lX, buffer=%06lX"),
                (long)wReadValue, (long)dwTemp );
         }
      }
     else // verify ok (for current location)
      { *pwMemFlags &= ~PIC_BUFFER_FLAG_VFY_ERROR;
      }
   }
} // end PIC10F_CompareToVerify()

//----------------------------------------------------------------------------
bool PIC10F_ReadAll(
       int iPicPrgAction ) // PIC_ACTION_READ, PIC_ACTION_BLANK_CHECK, or PIC_ACTION_VERIFY .
  // Return value:
  //  true = "device not blank" for PIC_ACTION_BLANK_CHECK + PIC_ACTION_READ
  //  true = "verify ok"        for PIC_ACTION_VERIFY
  //  false= an error occurred
{
// uint32_t dwWordIndex;
// uint32_t dwTemp;
 uint16_t  w;
 uint16_t  wMemFlags;
 int   i,iLastUsedAddress=-1;
 bool  not_blank = false;
 bool  fReadProtected;

  if( PIC_DeviceInfo.lCodeMemSize <= 0 )
      return false;        // avoid div by zero (in progress indicator)

  PIC10F_iNrOfErrors = 0;
  if( ! PicHw_iConnectedToTarget )
   { PIC_HW_ConnectToTarget();  // connect ICSP to target (if necessary)
   }
  // Do NOT change the supply voltage setting here (we may be VERIFYING)
  PIC10F_EnterProgMode();   // enter STDP programming mode (and set PIC10F_iCurrTargetAddress)

  PIC10F_wTempConfigWord = PIC10F_ReadDataFromMemory();   // read CONFIG word from 0x1FF or 0x3FF
  if( iPicPrgAction==PIC_ACTION_READ )
   { // ex: PicBuf_SetBufferWord( PIC10F_iCurrTargetAddress, PIC10F_wTempConfigWord );
     // Due to Microchip's ugly "logic address" game with the CONFIG WORD located
     // at 0x0FFF (instead of 0x03FF or 0x01FF) :
     PicBuf_SetBufferWord( 0x0FFF/*"logic address"-baaah*/ , PIC10F_wTempConfigWord );
   }
  if( iPicPrgAction==PIC_ACTION_VERIFY )
   { PIC10F_CompareToVerify( PIC10F_wTempConfigWord, &wMemFlags );
   }
  PIC10F_IncrAddress(1); // increments PIC10F_iCurrTargetAddress, and wraps from end of address space to zero
  fReadProtected = (PIC10F_wTempConfigWord & 0x008) == 0;  // is the PIC read-protected or not ?

  // Arrived here: PIC10F_iCurrTargetAddress should be ZERO (!) .
  //  Read CODE memory (including the OSCCAL word) :
  for(i=0; i<PIC_DeviceInfo.lCodeMemSize; ++i)
   {
     wMemFlags = PicBuf_GetMemoryFlags( PIC10F_iCurrTargetAddress );
     w = PIC10F_ReadDataFromMemory();
     if( (w&0x0FFF) != 0x0FFF )
      { not_blank = true;
        iLastUsedAddress = i;
      }
     if( iPicPrgAction==PIC_ACTION_READ )
      { PicBuf_SetBufferWord( PIC10F_iCurrTargetAddress, w );
      }
     if( iPicPrgAction==PIC_ACTION_VERIFY )
      { PIC10F_CompareToVerify(w, &wMemFlags );
      }
     if( PIC10F_iCurrTargetAddress == PIC_DeviceInfo.lAddressOscCal )
      { PIC10F_wReadOscCalibWord = w;
      }
     PicBuf_SetMemoryFlags( PIC10F_iCurrTargetAddress, wMemFlags );
     PIC10F_IncrAddress( 1 );   // increment PC for next code memory location
     APPL_ShowProgress( (100*i) / PIC_DeviceInfo.lCodeMemSize );
   } // end for < all PROGRAM MEMORY locations, including the osccal word >

  if( iPicPrgAction!=PIC_ACTION_VERIFY )
   { for(i=0; i<PIC_DeviceInfo.lCodeMemSize; ++i)
      { PicBuf_SetMemoryFlags( i,
         ( i<= iLastUsedAddress ) ? PIC_BUFFER_FLAG_USED : PIC_BUFFER_FLAG_UNUSED );
      }
   }

  // Arrived here, CODE MEMORY has been read, and the PC points to the first USER ID location
  //  (0x100 for PIC10F200/4, 0x200 for PIC10F202/6, 0x400 for PIC12F509),
  //  directly followed by the Backup OSCCAL value (@ 0x104 or 0x204 or 0x404)
  for(i=0; i<=4; ++i)   // read four User ID locations and the BACKUP OSCCAL value..
   {
     w = PIC10F_ReadDataFromMemory();
     if( iPicPrgAction==PIC_ACTION_READ )
      { PicBuf_SetBufferWord( PIC10F_iCurrTargetAddress, w );
      }
     if( i==4 )
      { PIC10F_wReadOscCalibBackup = w;
      }

     PIC10F_IncrAddress( 1 );   // increment PC for next code memory location
   } // end for < all PROGRAM MEMORY locations, including the osccal word >

  PIC10F_LeaveProgMode(); // leave STDP programming mode

  // Check the OSCCAL values. The PIC10F20x only has this poor 4-MHz-RC-oscillator,
  // proper calibration is important for many applications !
  if( (!fReadProtected) && ((PIC10F_wReadOscCalibWord&0x0F00) != 0x0C00) )
   { APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
       _("PIC10F Prog Warning: OSCCAL value (%03lX) looks invalid !"),
       (long)PIC10F_wReadOscCalibWord );
   }
  if( (!fReadProtected) && (PIC10F_wReadOscCalibWord != PIC10F_wReadOscCalibBackup ) )
   { APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
       _("PIC10F Prog Warning: OSCCAL value (%03lX) not equal to OSCCAL BACKUP (%03lX) !"),
       (long)PIC10F_wReadOscCalibWord, (long)PIC10F_wReadOscCalibBackup );
   }
  else if( (PIC10F_wReadOscCalibBackup & 0x0F00) != 0x0C00 )
   { APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
       _("PIC10F Prog Warning: OSCCAL BACKUP value (%03lX) looks invalid !"),
       (long)PIC10F_wReadOscCalibBackup );
   }

  if( iPicPrgAction == PIC_ACTION_VERIFY )
       return PIC10F_iNrOfErrors==0;
  else return not_blank;

} // end PIC10F_ReadAll()


//----------------------------------------------------------------------------
bool PIC10F_ProgAndVerifyOneLocation(
      int iAddress, uint16_t wData, uint16_t wVerifyMask,
      bool fMayShowErrorMessage )
{
 uint16_t wRead;
 uint16_t wMemFlags;
 bool fResult;

//  PHWInfo.iCurrProgAddress = iAddress; // added 2008-05-18 for the plugin-DLLs

  // Based on "program cycle" from [PIC10Fps], Figure 3-10 :
  PIC10F_SendCmd6( 0x02, false ); // "Load Data for Program Memory" ..
  PIC10F_SendData14( wData );     // ..immediately followed by 14 data bits (+START & STOP)
  // Added 2008-05-19: Let "smart" interfaces flush the clock- and data-transitions.
  //  This may be required for some programmers with USB interface which use burst-transfers.
//  PicHw_FlushCommand( 0x02,6,    wData,14 );

  PIC10F_SendCmd6( 0x08, true ); // "Begin Programming" (externally timed)
  PIC_HW_Delay_us( 2000 );       // "typical" 1ms but "max" 2ms ?!?
  PIC10F_SendCmd6( 0x0E, true ); // "End Programming"
  // ex: PIC_HW_Delay_us( 100  );       // wait for internal programming voltage discharge
  PIC_HW_Delay_us( 500  );       // wait a little longer, because programming failed too often

  wRead = PIC10F_ReadDataFromMemory() & wVerifyMask; // read back to verify
  wMemFlags = PicBuf_GetMemoryFlags( iAddress );
  if( wRead != (wData & wVerifyMask) )
    { // report programming failure :
      wMemFlags |= PIC_BUFFER_FLAG_PRG_ERROR;
      if( fMayShowErrorMessage )
       { APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
             _( "Verify Error: %06lX: read %06lX, wanted %06lX" ),
             (long)iAddress,  (long)wRead,   (long)wData  );
       }
      fResult = false;
    }
  else
    { wMemFlags &= ~( PIC_BUFFER_FLAG_PRG_ERROR | PIC_BUFFER_FLAG_VFY_ERROR );
      fResult = true;
    }
  PicBuf_SetMemoryFlags( iAddress, wMemFlags );
  return fResult;
} // end PIC10F_ProgAndVerifyOneLocation()


//----------------------------------------------------------------------------
bool PIC10F_ProgramAll(
       int iPicPrgAction ) // PIC_ACTION_WRITE or PIC_ACTION_ERASE .
   // - Includes "bulk" erase ("full"~"chip erase" or "standard"~"without fuses")
   // - Only enters programming mode ONCE(!!) because some ugly interfaces
   //   have severe problems entering and leaving programming mode rapidly
   // - Takes extra care for the OSCCAL bits and OSCCAL BACKUP bits
{
// uint32_t dwWordIndex;
 uint32_t dwTemp;
 uint16_t  w /*, wRead*/;
 int   i;
 bool  fWasProtected;

  if( PIC_DeviceInfo.lCodeMemSize <= 0 )
      return false;                  // avoid div by zero (in progress indicator)

  PIC10F_iNrOfErrors = 0;
  if( ! PicHw_iConnectedToTarget )
   { PIC_HW_ConnectToTarget();  // connect ICSP to target (if necessary)
   }

  PIC10F_EnterProgMode();   // enter programming mode (and set PIC10F_iCurrTargetAddress)
  // CAUTION: Program/Verify mode entry will set the address pointer
  //          to 0x1FF for the PIC10F200/204
  //         and 0x3FF for the PIC10F202/206 !
  //  .. which means, the PC points to the CONFIGURATION WORD now .
  PIC10F_wTempConfigWord = PIC10F_ReadDataFromMemory(); // read from 0x1FF or 0x3FF
  PIC10F_IncrAddress(1); // increments PIC10F_iCurrTargetAddress, and wraps from end of address space to zero
  fWasProtected = (PIC10F_wTempConfigWord & 0x008) == 0;  // Code protection bit is LOW-active !

  // Increment the address counter to 0x0FF or 0x1FF, depending on device type .
  // In PIC10F20x devices, the Oscillator Calibration Bits are contained in a
  // MOVLW instruction which is located at the Reset "Vector"(!).
  // The "Reset Vector" is not a vector as we know it; anyway it's at
  //  0x0FF for PIC10F200/204 and at 0x1FF for PIC10F202/206 .
  // Where it is, can be seen in the DEVICE INFO STRUCT (defined in devices.h) .
  PIC10F_IncrAddress( PIC_DeviceInfo.lAddressOscCal & PIC10F_iMaskAddress );

  // Arrived here, PIC10F_iCurrTargetAddress must be 0x0FF or 0x1FF ...
  PIC10F_iCurrTargetAddress = PIC10F_iCurrTargetAddress;  // DEBUG: 255 or 511 !
  PIC10F_wReadOscCalibWord = PIC10F_ReadDataFromMemory(); // read from 0x0FF or 0x1FF

  // Increment the address counter another N times to let it point to 0x104 or 0x204 or 0x404:
  PIC10F_IncrAddress( PIC_DeviceInfo.lIdMemBase+4 - (PIC10F_iMaskAddress>>1) );  // guess what the result is.. FIVE ?
  PIC10F_iCurrTargetAddress = PIC10F_iCurrTargetAddress;  // DEBUG: 260 or 516 !

  // Arrived here, the address counter must be 0x104 (260) or 0x204 (516) .
  PIC10F_wReadOscCalibBackup = PIC10F_ReadDataFromMemory(); // read from 0x0FF or 0x1FF
  // Arrived here, the address counter is still at 0x104 or 0x204 ...

  // Decide which of the *three* OCSCCAL values (two read from chip + one from buffer)
  //        should be written back later .
  // Note: The BACKUP OSCCAL VALUE seems to be a complete MOVLW instruction too !
  //  ( it was 0x0CF4 in a sample received from SM6LKM ) .
  if( (PIC10F_wReadOscCalibBackup & 0x0F00) != 0x0C00)
   {  // not a MOVLW instruction (binary: 1100 kkkk kkkk ) !
      APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
       _("PIC10F Prog Warning: Read BACKUP OSCCAL value (%03lX) looks invalid !"),
       (long)PIC10F_wReadOscCalibBackup );

      if( (PIC10F_wReadOscCalibWord & 0x0F00) != 0x0C00 )
       { // both BACKUP AND(!) MAIN OSCCAL word look bad..
         APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
             _("PIC10F Warning: Both OSCCAL (%03lX) and OSCCAL BACKUP (%03lX) look invalid !"),
             (long)PIC10F_wReadOscCalibWord, (long)PIC10F_wReadOscCalibBackup );
         // Use the OSCCAL values from the MEMORY BUFFER ,
         //  and make sure it contains a valid MOVLW opcode .
         PicBuf_GetBufferWord( PIC_DeviceInfo.lIdMemBase+4/*addr backup OSCCAL*/, &dwTemp );
         if( (dwTemp & 0x0F00)==0x0C00 )
          {  // BACKUP VALUE from buffer memory looks good -> use it for both locations
             PIC10F_wReadOscCalibWord = PIC10F_wReadOscCalibBackup = (uint16_t)dwTemp;
          }
         else // BACKUP VALUE from buffer memory is invalid too ->
          {  PicBuf_GetBufferWord( PIC_DeviceInfo.lAddressOscCal, &dwTemp ); // try "the other one"
             if( (dwTemp & 0x0F00)==0x0C00 )
              {  // main OSCCAL VALUE from buffer memory looks good -> use it for both locations
                 PIC10F_wReadOscCalibWord = PIC10F_wReadOscCalibBackup = (uint16_t)dwTemp;
              }
             else
              {
                 if( Config.iDontCareForOsccal )
                  { APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
                     _("PIC10F: Option \"Don't care for OSCCAL\" set, using a 'default' OSCCAL word !!"));
                    PIC10F_wReadOscCalibWord = PIC10F_wReadOscCalibBackup = 0x0C00;
                  }
                 else
                  { APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
                     _("PIC10F: Abort programming (nothing erased). No valid OSCCAL value found !"));
                    APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
                     _("        Enter a valid BACKUP OSCCAL VALUE (like 0x0CF4)"));
                    APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
                     _("        on the Config Register Tab, then try again !"));
                    return false;    // don't try to erase the chip, there is something fishy in here !
                  }
              }
          }
         APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
              _("PIC10F Warning: Using OSCCAL (%03lX) and OSCCAL BACKUP (%03lX) from buffer !"),
              (long)PIC10F_wReadOscCalibWord, (long)PIC10F_wReadOscCalibBackup );
       }
      else // BACKUP OSCCAL looks bad, but MAIN OSCCAL value looks good :
       {
         APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
           _("PIC10F Warning: Replaced bad BACKUP OSCCAL value with main OSCCAL value (%03lX) !"),
           (long)PIC10F_wReadOscCalibWord );
         PIC10F_wReadOscCalibBackup = PIC10F_wReadOscCalibWord;
       }
   } // end if < backup OSCCAL value looks invalid >
  else // OSCCAL BACKUP VALUE valid :
   {
   }

  if( (!fWasProtected) && (PIC10F_wReadOscCalibWord != PIC10F_wReadOscCalibBackup ) )
   { APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
       _("PIC10F Prog Warning: OSCCAL value (%03lX) not equal to OSCCAL BACKUP (%03lX) !"),
       (long)PIC10F_wReadOscCalibWord, (long)PIC10F_wReadOscCalibBackup );
   }

  // If the "main" osccal value looks unhealty, replace it with the backup value :
  if( fWasProtected )
   {  // if the chip WAS protected, the OSCCAL value (which is part of the code memory)
      // cannot have been read successfully.
      APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
        _("PIC10F Prog: PIC was protected, using OSCCAL BACKUP value (%03lX) !"),
        (long)PIC10F_wReadOscCalibBackup );
      PIC10F_wReadOscCalibWord = 0x0C00 | ( PIC10F_wReadOscCalibBackup & 0x00FF);
   }
  else // was NOT protected, OSCCAL value should be valid !
  if( (PIC10F_wReadOscCalibWord & 0x0F00) != 0x0C00 )
   {  // not a MOVLW instruction (binary: 1100 kkkk kkkk ) !
      APPL_ShowMsg( APPL_CALLER_PIC_PRG,0,
        _("PIC10F Prog Warning: Replaced OSCCAL value (%03lX) with OSCCAL BACKUP (%03lX) !"),
        (long)PIC10F_wReadOscCalibWord, (long)PIC10F_wReadOscCalibBackup );
      PIC10F_wReadOscCalibWord = 0x0C00 | ( PIC10F_wReadOscCalibBackup & 0x00FF);
   }

  // Though this is a WRITE routine, place the READ value in a buffer (if it's valid)
  //    Why ? Because programming may fail AFTER SUCCESSFULLY ERASING the PIC,
  //          and in that case the user shall simply "try again".
  PicBuf_SetBufferWord( PIC_DeviceInfo.lAddressOscCal, PIC10F_wReadOscCalibWord );
  if( ! Config.iDontCareForOsccal )
   {
    // A VALID OSCCAL VALUE for this chip will be found in the buffer then .
    // A PIC10F20x firmware without the MOVLW instruction at the reset vector
    // will most likely crash !
    PicBuf_SetBufferWord( PIC_DeviceInfo.lIdMemBase+4/*addr backup OSCCAL*/,
                          PIC10F_wReadOscCalibBackup  );
   }


  // Erase the program memory and the config word, AND MAYBE MORE ... "BULK ERASE" !
  // Note on "bulk erase" : There are two flavours of "bulk erase" for the PIC10F20x .
  //     What Microchip call "bulk erase program memory" in the PIC10F20x prog spec is called
  //     "chip erase" for other PICs, if it erases EVERYTHING .
  //  If fEraseAll=true, PIC10F_ProgramAll() performs what they call
  //     "full device bulk erase" here (== "chip erase" in other specs).
  //     It erases EVERYTHING: program memory, config word, User ID locations,
  //     and even the backup oscillator calibration word !
  if( Config.iUseCompleteChipErase )
   {   // > To perform a FULL DEVICE BULK ERASE (aka "chip erase" ?! ),
       // > increment PC to "0x200/0x400" (NONSENSE, ANOTHER ERROR IN DS41228D)
       // > (first user ID location).
       // But the first user ID location is at 0x100/0x200, stupido !
       // So set the PC from 0x204(0x104) where it is at the moment
       //                 to 0x100(0x200) where it must be.
       // For PIC10F200 (with lCodeMemSize==256), the PC wraps from 0x1FF to 0x000 .
       // For PIC10F206 (with lCodeMemSize==512), the PC wraps from 0x3FF to 0x000 .
       PIC10F_IncrAddress( PIC10F_iMaskAddress - (PIC_DeviceInfo.lIdMemBase+4) + 1 );  // increment PC from OSCCAL backup to MAX, then wrap to zero
       PIC10F_IncrAddress( PIC_DeviceInfo.lIdMemBase );              // increment PC from zero  to 0x100 = 1st USER-ID location
       PIC10F_iCurrTargetAddress = PIC10F_iCurrTargetAddress;
       PIC10F_SendCmd6( 0x09, true );  // send 6-bit command "Bulk Erase [..Program Memory]" and flush the queue
       // Note: There is only one "Bulk Erase" command,
       //   in DS41228D Table 3-1 they call it "Bulk Erase Program Memory" though it's more than this !
       PIC_HW_LongDelay_ms( 50/*milliseconds*/ );   // wait for erase (internally timed)
       // Hopefully the PC is not affected by the erase operation (not specified !).
       // We don't want to exit and re-enter programming mode here, because
       // some crude interfaces out there have serious problems with this .
       // PC should still be at 0x100/0x200, so increment the PC until it wraps to ZERO(!) .
       PIC10F_IncrAddress( PIC10F_iMaskAddress - PIC_DeviceInfo.lIdMemBase + 1 ); // increment PC from 0x200 to 0x3FF, then wrap to zero
   }
  else // no chip erase, but normal erase, without destroying the backup OSCCAL word...
   {   // For a "normal" erase, the PC must point to the Config Word address (0x1FF or 0x3FF) .
       // At the moment, the PC still points to 0x104/0x204 after reading the Backup OSCCAL value .
       // Due to the different size of the "unimplemented" gap before the config word,
       //  another "if"-statement required here ... grumble
       PIC10F_IncrAddress( PIC10F_iMaskAddress - (PIC_DeviceInfo.lIdMemBase+4) ); // increment PC from 0x104 to 0x1FF ( &ConfigWord )
       PIC10F_SendCmd6( 0x09, true );             // send 6-bit command "Bulk Erase Program Memory"
       PIC_HW_LongDelay_ms( 50/*milliseconds*/ ); // wait for erase (internally timed)
       // PC should still be at 0x1FF/0x3FF, so increment ONCE(!) to wrap the PC to ZERO .
       PIC10F_IncrAddress( 1 );                   // increment PC from 0x1FF or 0x3FF to zero
   } // end if "complete" or "partial" erase ?


  // Arrived here, program memory and config word, and possible USER ID + OSCCAL BACKUP
  // are erased; the PC is at zero.
  // Optionally write and verify CODE memory, then write the CONFIG word...
  for(i=0; i<PIC_DeviceInfo.lCodeMemSize; ++i)
   {
     // get next word from buffer (normal code memory location or OSCCAL) :
     PicBuf_GetBufferWord( PIC10F_iCurrTargetAddress, &dwTemp );
     w = dwTemp;  // word from buffer
     if( (PIC10F_iCurrTargetAddress==PIC_DeviceInfo.lAddressOscCal) && (! Config.iDontCareForOsccal) )
      {  // restore the orginal OSCCAL word for this special location :
        w = PIC10F_wReadOscCalibWord;
      }
     w &= 0x0FFF;                    // 12 bit only  !
     if(   (iPicPrgAction & PIC_ACTION_WRITE)
        || ((PIC10F_iCurrTargetAddress==PIC_DeviceInfo.lAddressOscCal) && (! Config.iDontCareForOsccal))
       )
      {
       if( ! PIC10F_ProgAndVerifyOneLocation( PIC10F_iCurrTargetAddress, w, 0x0FFF,
             (PIC10F_iNrOfErrors<20)/*may show error message ?*/ ) )
        {
          ++PIC10F_iNrOfErrors;
        }
      } // end if < WRITE this memory location ? >

     PIC10F_IncrAddress( 1 );   // increment PC for next code memory location
    //APPL_ShowProgress( (100*i) / PIC_DeviceInfo.lCodeMemSize );
   } // end for < all PROGRAM MEMORY locations, including the osccal word >


  // Arrived here, CODE MEMORY is finished, but not the ID locations and the config word.
  //  PC is at the first user ID location now (0x100 for PIC10F200/4, 0x200 for PIC10F202/6)
  PIC10F_iCurrTargetAddress = PIC10F_iCurrTargetAddress;  // DEBUG: must be 256 or 512 (dec.)
  if( Config.iUseCompleteChipErase )
   {    // the User ID locations can only be reprogrammed after "erasing ALL" (?)
     for(i=0; i<=3; ++i)
      {
        PicBuf_GetBufferWord( PIC10F_iCurrTargetAddress, &dwTemp );
        w = dwTemp | 0xFF0;  // only 4 bits per location, write the upper bits as ONES (!)

        if( ! PIC10F_ProgAndVerifyOneLocation( PIC10F_iCurrTargetAddress, w, 0x0FFF, true ) )
           ++PIC10F_iNrOfErrors;
        PIC10F_IncrAddress( 1 );       // increment PC for next code memory location
      } // end for < all USER ID locations >
     // Arrived here, the PC is at the BACKUP OSCCAL value location (0x104 or 0x204).
     // Very important after "erase all":  Restore the BACKUP OSCCAL value !
     if( ! PIC10F_ProgAndVerifyOneLocation( PIC10F_iCurrTargetAddress, PIC10F_wReadOscCalibBackup, 0x0FFF, true ) )
        ++PIC10F_iNrOfErrors;
   } // end if( Config.iUseCompleteChipErase )
  else  // didn't "erase ALL", so cannot program the User ID locations
   {    //  ... instead skip them :
     PIC10F_IncrAddress( 4 );    // increment PC from 0x100(0x200) to 0x104(0x204)
   }


  // Arrived here, the PC still points to the BACKUP OSCCAL BITS (0x104 or 0x204 or 0x404)
  // Skip the "Reserved" area as well as the "Unimplemented" area ...
  PIC10F_IncrAddress( PIC10F_iMaskAddress - (PIC_DeviceInfo.lIdMemBase+4) );

  // Finally program the configuration memory (here: ONE CONFIGURATION WORD) .
  if(iPicPrgAction & PIC_ACTION_WRITE)
   {
     PIC10F_iCurrTargetAddress = PIC10F_iCurrTargetAddress;  // DEBUG: must be 0x01FF (511) or 0x3FF (1023) here
     // Another omission in [PIC10Fps] - it seems impossible to increment the PC
     // to let it point to the CONFIGURATION WORD without quitting and re-entering
     // programming mode ?! The reason for this was unreproducable in 2005-05 .
#if(1)
     // leave and re-enter programming mode - why is this necessary ?
     // PC (theoretically) already points to config word !
     PIC10F_LeaveProgMode();
     PIC10F_EnterProgMode();
#endif
     PicBuf_GetBufferWord( PIC10F_iCurrTargetAddress, &dwTemp );  // -> CONFIG WORD !
     // Another error in [PIC10Fps], Figure 3-11: First "Load Data Command",
     // then "One-Word Programming Cycle from Fig 3-10" - which contains another "Load Data Command" ?!
     if( ! PIC10F_ProgAndVerifyOneLocation( PIC10F_iCurrTargetAddress, (uint16_t)dwTemp, 0x0FFF, true ) )
        ++PIC10F_iNrOfErrors;
   } // end if < "write" >

  PIC10F_LeaveProgMode(); // leave programming mode

  return PIC10F_iNrOfErrors==0;

} // PIC10F_ProgramAll()





// EOF < PIC10F_PRG.C >
