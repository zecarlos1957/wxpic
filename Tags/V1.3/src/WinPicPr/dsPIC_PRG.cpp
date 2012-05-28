/*-------------------------------------------------------------------------*/
/* dsPIC_PRG.cpp                                                           */
/*                                                                         */
/*  Purpose: Serial PIC Programming algorithms for dsPIC30F .              */
/*                                                                         */
/*  Author:   Wolfgang Buescher (DL4YHF)                                   */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*                                                                         */
/*  Date   : 2005-11-02  (ISO 8601 format, YYYY-MM-DD)                     */
/*  State  : UNDER DEVELOPMENT - NOT REALLY TESTED YET !                   */
/*                                                                         */
/*                                                                         */
/*  Literature:                                                            */
/*    [dsPICps] = "dsPIC30F Flash Programming Specification"               */
/*                Microchip DS70102D (or later ? )                         */
/*                                                                         */
/*  Last changes :                                                         */
/*    2009-06-04:  Ported to wxWidget by Philippe Chevrier                 */
/*    2006-11-05:  Handed this file (dsPIC_PRG.cpp) over to Yves Rondeau   */
/*                 who plans to support of the dsPIC33 family in WinPic.   */
/*                 No changes by Wolfgang Buescher for a while .           */
/*    2008-05-19:  Minor changes for "intelligent" programmers .           */
/*                                                                         */
/*-------------------------------------------------------------------------*/

  // Revsion history - NEWEST entry first !
  //
  // 2005-02-18:  Tried to add support for a dsPIC30F201 after BUYING(!)
  //        a sample chip, since Microchip was unable to supply samples.
  //     Because the dsPIC programming algorithms are TOTALLY different
  //     from 16F-family, the "new" algorithms are located in dsPIC_PRG.cpp .
  //
  // 2005-07-01:  First half-way stable release
  //
  // 2005-11-02:  Fixed an issue with the magic 64k address boundry
  //              in dsPIC_STDP_ReadCodeMemory() reported by Rolf Ziegler .
  //

#include "PIC_HW.h"    // Routines to drive the programmer hardware interface
#include "Config.h"
#include <Appl.h>

#include "dsPIC_PRG.h" // header for the dsPIC programming algorithms

//----------------------------------------------------------------------------
//  Low-level stuff to communicate with the dsPIC in "STDP" mode .
//
//  "STDP" is Microchip's Standard DUT Programming protocol ,
//   which unfortunately has got NOTHING to do with the old-fashioned
//   in-circuit programming mode from the 12F- and 16F-family.
//   Instead, it's a method of shifting a MICRCONTROLLER COMMAND
//   into the microcontroller's INSTRUCTION REGISTER (!) to execute them.
//  In fact, this seems to be the only way to program a dsPIC if the
//   so-called "executive memory" is not programmed.
//  See [dsPICps] Chapter 11.  Here just a few (?) notes :
//
//  - "Fresh" dsPIC from the factory are shipped WITHOUT the "programming
//    executive" .  So "STDP" is always required, even though it may be
//    utterly slow because A LOT OF MACHINE CODE INSTRUCTIONS must be
//    clocked into the dsPIC for every erase-, write-, or read-operation .
//
//  - In fact, the "executive memory" gets erased whenever doing a "bulk erase"
//    through an STDP command. So the STDP programming algorithms must be
//    implemented in first place. It is questionable that WinPic will ever
//    support the dsPIC's "ICSP" protocol which requires the executive stuff.
//
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void dsPIC_STDP_SendCpuControlCode( uint16_t w4BitControlCode )
   // Sends a 4-bit CPU control in STDP mode as explained in Chapter 11.2 .
   // - Upon entry, the interface's CLOCK and DATA output line
   //    (which are called "PGC" and "PGD" here) must already
   //    be defined as OUTPUTS by the caller .
{ int i;
  for(i=0; i<=3; ++i)
   {
     // Notes:
     //  - The dsPIC seems to sample PGD on FALLING PGC edge .
     //  - The LSB is transmitted first .
     //  - The serial clock periods are in the XX nanosecond range,
     //    so we're safe to use 1us for the high- and low-periods here.
     //    The slow port access under WinXP will make the actual clock rate
     //    MUCH MUCH SLOWER (measured:  ~ 10 us per half clock period) .
     // Note: Since the optional use of 'PortTalk' (..thanks Martin for the pointer)
     //   .. the PIC_HW_Delay_50ns() routine wait for an additional time of
     //      <iExtraClkDelay_us> microseconds  if required for the interface.
     PIC_HW_SetClockAndData(true,  w4BitControlCode & 0x0001 );
     PIC_HW_Delay_50ns();   // "P3", spec requires 15ns(!) for PGC low time
     PIC_HW_SetClockAndData(false, w4BitControlCode & 0x0001 );
     PIC_HW_Delay_50ns();   // "P3", spec requires 15ns(!) for PGC low time
     w4BitControlCode >>= 1;
   }
} // end dsPIC_STDP_SendCpuControlCode()

//----------------------------------------------------------------------------
void dsPIC_STDP_ExecuteCoreInstruction(
      uint32_t dwInstruction )  // a 24-bit dsPIC30F machine code instruction
  // From [dsPICps] Chapter 11.2.1 "SIX SERIAL INSTRUCTION EXECUTION"
  //  > The SIX control code allows execution of dsPIC30F assembly instructions.
  //  > When the SIX code is received, the CPU is suspended for 24 clock cycles
  //  > as the instruction is then clocked into the internal buffer.
  //  > Once the instruction is shifted in, the state machine allows it to be
  //  > executed over the next four clock states.
  //  > While the received instruction is executed, the state machine simul-
  //  > taneously shifts in the next 4-bit command (see figure 11-1).
{
  int i;
  uint32_t dwShiftReg;

  PIC_HW_SetClockEnable( true );     // enable PGC output on interface
  PIC_HW_SetDataEnable( true );      // enable PGD output on interface

  dsPIC_STDP_SendCpuControlCode( 0x0000 ); // send "SIX" control code = 0000b

#if( 1 )
  PIC_HW_Delay_500ns();     // Wait "P4" between 4-bit cmd and operand, required: 20ns
#else // to see a clearer gap on the oscilloscope when debugging :
  PIC_HW_Delay_us( 20 );
#endif

  // Shift out the 24-bit operand,
  //  which the prog spec calls a dsPIC30 "assembly" instruction .
  dwShiftReg = dwInstruction;
  for( i=0; i<=23; ++i )
   { // Note: LSBit shifted out to dsPIC first !
     PIC_HW_SetClockAndData(true, dwShiftReg & 1 );
     PIC_HW_Delay_500ns();   // "P1a", spec requires 20ns in STDP mode for PGC high time
     PIC_HW_SetClockAndData(false,dwShiftReg & 1 );
     PIC_HW_Delay_500ns();   // "P1b", spec requires 20ns in STDP mode for PGC low time
     dwShiftReg >>= 1;       // next bit please
   }

//  PicHw_FlushCommand( 0x0000/*Command,here: "Control Code"*/,4/*numBitsCommand*/,
//                dwInstruction/*data*/, 24/*numBitsData*/ ); // added 2008-05-19


  // For the dsPIC's side, "ExecuteAssemblyInstruction" finished here .
  // But for the "JDM programmer", we need a longer clock-low time
  //       to charge C3 to 5 volts again !
  if( PIC_HW_interface.type==PIC_INTF_TYPE_JDM2 )
   { // For the JDM programmer, turn DATA (out) off as soon as possible
     // to avoid discharging C3 (22uF) :
     PIC_HW_SetClockAndData(false, false );
     if( TSessionConfig::GetSlowInterface() )
          PIC_HW_Delay_us( 100 );
     else PIC_HW_Delay_us( 50 );   // just imagine how many thousands of "core instructions" we need !
   } // end if < "JDM" programmer >

} // end dsPIC_STDP_ExecuteCoreInstruction()

//----------------------------------------------------------------------------
uint16_t dsPIC_STDP_ReadVISIRegister( void )
  // Reads data from the device (contents of the VISI register) in STDP mode.
  // From [dsPICps] Chapter 11.2.2 "REGOUT SERIAL INSTRUCTION EXECUTION" :
  //  > The REGOUT control code allows for data to be extracted from the
  //  > device in STDP mode. It is used to clock the contents of
  //  > the VISI register out of the device over the PGD pin.
{
 int i;
 uint16_t wResult = 0;

  PIC_HW_SetClockEnable( true );     // enable PGC output on interface
  PIC_HW_SetDataEnable( true );      // enable PGD output on interface

  dsPIC_STDP_SendCpuControlCode( 0x0001 ); // send "REGOUT" control code = 0001b
  PIC_HW_Delay_500ns();  // Wait "P4" between 4-bit cmd and operand, required: 20ns

  //  > After the REGOUT control code is received, eight clock cycles
  //  > are required to process the command. During this time, the CPU
  //  > is help idle.
  // Furthermore, looking at [dsPICps] Figure 11-2, the host
  // still treats PGD as an INPUT (to the dsPIC) during these 8 clocks.
  for( i=0; i<=7; ++i )
   {
     PIC_HW_SetClockAndData(true,  false );
     PIC_HW_Delay_500ns();  // "P1a", spec requires 20ns in STDP mode for PGC high time
     PIC_HW_SetClockAndData(false, false );
     PIC_HW_Delay_500ns();  // "P1b", spec requires 20ns in STDP mode for PGC low time
   }

//  PicHw_FlushCommand( 0x0001/*Command,here: "Control Code"*/,4/*numBitsCommand*/,
//                      0/*data*/, 8/*numBitsData*/ ); // added 2008-05-19

  // > After these eight cycles, an additional 16 cycles are required
  // > to clock the data "OUT" [WB "IN" from the programmer's point of view]
  // > The REGOUT instruction is unique because the PGD pin is an input
  // > when the control code is transmitted to the device. However,
  // > after the control code is processed, the PGD pin becomes an output
  // > as the VISI register is shifted out. After the contents of the VISI
  // > register are shifted out, PGD becomes an input again as the state
  // > machine holds the CPU idle until the next 4-bit control code
  // > is shifted in .
  PIC_HW_SetDataEnable( false ); // turn the programmer's PGD line into an INPUT
  PIC_HW_Delay_500ns();      // Wait "P5" between REGOUT and reading VISI, required: 20ns
  for( i=0; i<=15; ++i )
   {
     PIC_HW_SetClockAndData( true,   // set PGC high
       PicHw_iStateOfDataOutWhileReading); // state of data-output-line while reading
     PIC_HW_Delay_500ns();   // "P1a", spec requires 20ns in STDP mode for PGC high time
     wResult |= PIC_HW_GetDataBit() << i;  // sample data from PIC while clock=HIGH
     PIC_HW_SetClockAndData( false, PicHw_iStateOfDataOutWhileReading ); // set PGC low
     PIC_HW_Delay_500ns();   // "P1b", spec requires 20ns in STDP mode for PGC low time
   } // end for < 16 bits read from VISI register >

  PIC_HW_SetDataEnable( true ); // turn the programmer's PGD line into an OUTPUT again

  // For the dsPIC's side, "ReadVISIRegister" is finished here .
  // But for the "JDM programmer", we need a longer clock-low time
  //       to charge C3 to 5 volts again !
  if( PIC_HW_interface.type==PIC_INTF_TYPE_JDM2 )
   { // For the JDM programmer, turn DATA (out) off as soon as possible
     // to avoid discharging C3 (22uF) :
     PIC_HW_SetClockAndData(false, false );
     // some time for the JDM interface to "recharge" C3 :
     if( TSessionConfig::GetSlowInterface() )
          PIC_HW_Delay_us( 100 );
     else PIC_HW_Delay_us( 50 );   // just imagine how many thousands of "core instructions" we need !
   } // end if < "JDM" programmer >

#if(0)
  static uint16_t wTest=0;
  wResult = wTest++;
#endif

  return wResult;
} // end dsPIC_STDP_ReadVISIRegister()


//----------------------------------------------------------------------------
bool dsPIC_EnterSTDPMode( void )
  // Enter "STDP" programming mode for dsPIC30F .
  // > The STDP mode is entered by holding PGC and PGD low,
  // > and then raising /MCLR / Vpp to high voltage .
  //
  // This is almost the same as entering "programming mode"
  // for the PIC16Fxxx-family, with
  //  TSessionConfig::GetNeedPowerBeforeMCLR() = true ,
  // so use the routine in PIC_HW.CPP which is already complicated enough
  // due to the large variety of programming interfaces !
  // Because this *may* change, use dsPIC_EnterSTDPMode()
  // when playing with dsPIC's. Read the full story in PIC_HW.CPP !
{
  // ex: PIC_HW_ProgMode();  // raise /MCLR to 12 V while CLOCK+DATA = low .
  // Something to remember from [dsPICps] :
  // > Note 1: The sequence that places the device into STDP mode
  // >    places all unused I/O pins to the high impedance state
  // > 2: Once STDP mode is entered, the PC is set to 0x0 (the Reset vector).
  // > 3: Before leaving the Reset vector with a GOTO instruction,
  // >    two NOP instructions must be executed .

//  PHWInfo.iCurrProgAddress = 0x000000; // added 2008-05-18 for the plugin-DLLs (here: "STDP-mode" sets PC=0)

  // Added 2004-05-30, due to problems with JDM programmer with 16F877A
  //                   when programming+verify in two separate overs .......
  if( PIC_HW_interface.type==PIC_INTF_TYPE_JDM2 )
   { // In the "JDM", things may be different, because it is impossible here
     // to turn the PIC's supply voltage quickly on.
     // Here, for the dsPIC, [dsPICps] Figure 11-3 clearly shows
     //  "Vdd turned on before Vpp" . How to achieve this with the JDM interface :

      // Make sure C2 (100uF) is charged to 8 volts  and  C3 (22uF)
      // charged to 5 volts (=negative bias voltage between PIC's Vss and PC's GROUND).
      PIC_HW_SetClockAndData(false,false);  // -> RTS=-12V, DTR=-12V
      PIC_HW_SetVpp( true );                // -> TXD=+12V , charges C2 (100uF) to ~13.2 volts
         // this is quite fast, because no resistors are involved:  13V*100uF/5mA = 260ms
      PIC_HW_LongDelay_ms(260/*ms*/); // give the CPU to another thread to make windoze happy while charging C2
      // Now pull MCLR / Vpp low, so the PIC enters the RESET state .
      // Note: Not all PIC's do have a RESET necessarily, see other case below !
      PIC_HW_SetVpp( false );               // -> TXD= -12V (hard limited to -5.7V), charges C3 to 5 volts
                                            //        while C2 remains at 8 volts.
      PIC_HW_Delay_us(100); // wait a short time for the Vpp(!) switch
                            // (the PIC itself only needs to see MCLR=LOW for 2 us).
      PIC_HW_SetVpp(true);  // Now there should be a faster rise on /MCLR = "Vpp",
                            // from 0 V to almost 13 V (from the PIC's point of view).
        // For many PIC devices, this rise time must not exceed 1 MICROSECOND,
        //          which is the reason why some ultra-ugly programmers fail .
        // Measured on an oscilloscope, where GROUND is connected to the PC's ground!!
        // (even if you don't want it that way), Vpp rises from -5 to +8 volts.
      // After /MCLR rised from 0 to 12 volts..
      PIC_HW_Delay_us(2);   // wait "P7" = 2 us in STDP mode, before the 1st clock pulse

   } // end if < JDM programmer >
  else
   { //----------------------- ALL OTHER PROGRAMMERS (except "JDM") --------------------------
       // For the dsPIC in STDP mode, force PGC and PGD low while raising Vpp :
       PIC_HW_SetClockAndData(false,false);
       PIC_HW_SetClockEnable(true );  // Enable the tristate buffer outputs (PC->PIC)
       PIC_HW_SetDataEnable( true );  // (up to now, only required for AN589 programmer)
       PIC_HW_SetVpp(false);          // ensure Vpp is off
       PIC_HW_SetVdd(false);          // ensure Vdd is off
       PIC_HW_Delay_us(800);          // 800us to discharge a 100nF capacitor via 4k7
       PIC_HW_PullMclrToGnd(true);    // do tie MCLR to ground (not many adapters support this)
       // Because some "ugly style" programmers switch Vdd together with Vpp,
       // turn Vpp off for a very short time to generate a steep
       // rise on Vpp while Vdd is still present :
       if( PIC_HW_interface.wSeparateVddAndVppControl )  // 0=no, 1=yes
        {
         PIC_HW_SetVdd(true);    // turn PIC's supply voltage on
         PIC_HW_Delay_us(50);    // wait a very short time for the Vdd(!) switch
         PIC_HW_SetVpp(true);    // lift MCLR from 0 V to 13 V within 100 us
        }
       else  // Need power before raising MCLR ,  but no separate Vdd and Vpp control lines :
        {    // (like in the COM84 adapter, which has a Vpp(!) but no Vdd(!) control line)
         PIC_HW_Delay_us(20000);  // wait a short while until MCLR is really 0 volt
         PIC_HW_SetVdd(true);    // turn PIC's supply voltage on (no effect for COM84)
         PIC_HW_Delay_us(50);    // wait a very short time for the Vdd(!) switch
         PIC_HW_SetVpp(true);    // an utterly slow rise on MCLR (because of discharged cap)
         if( PicHw_fTogglingTxD )  // If there is a charge pump for "Vpp", keep toggling a bit longer...
          {
           PIC_HW_LongDelay_ms(500/*ms*/);  // while WAITING here, feed the UART's transmit buffer (!)
          }
         else // if there is NO charge pump for "Vpp", use the typical "Vpp-ON-delay",
          {   // then turn Vpp off for a short time, and turn it on to produce a FAST RISE on Vpp/MCLR:
           PIC_HW_Delay_us(200000); // time required by INTERFACE (something like 200ms)
           PIC_HW_SetVpp(false);   // Vdd(!) capacitor should be charged now,
                                   //   the next positive slope on Vpp will be steeper
           PIC_HW_Delay_us(50);    // wait a very short time for the Vpp(!) switch
           PIC_HW_SetVpp(true);    // now this should be a faster rise on /MCLR = "Vpp"
          }
        } // end < no separate Vdd and Vpp control lines >
   } // end if < not "JDM" programmer >

  // caller must not forget to turn Vpp & Vdd off after he's ready !

  return true;
} // end dsPIC_EnterSTDPMode()

//----------------------------------------------------------------------------
void dsPIC_LeaveSTDPMode( void )
{
   PIC_HW_ProgModeOff();  // programming voltage off, clock off, data high-z
}


//----------------------------------------------------------------------------
uint16_t dsPIC_STDP_Read16BitLocation( uint32_t dwSourceAddress )
  // Reads a single 16-bit memory location.
  // Based on [dsPICps] Chapter 11.13, "Reading Data Memory",
  // but simplified for reading just a single memory location,
  //  which may be DATA MEMORY,            (addr 0x7FF000 - 0x7FFFFE in dsPIC30F )
  //               CONFIGURATION MEMORY,   (addr 0xF80000 - 0xF8000E in dsPIC30F )
  //            or DEVICE ID locations     (addr 0xFF0000 - 0xFF0002 in dsPIC30F )
  // UTTERLY SLOW, but only a few dozen locations will be read with this routine.
{
 uint32_t dwOpcode;    // 24-bit machine code for dsPIC
 uint16_t  wResult;

  // NOT INCLUDED HERE:  dsPIC_EnterSTDPMode();  // enter STDP programming mode, set PC to 0x0000 .

//  PHWInfo.iCurrProgAddress = dwSourceAddress; // added 2008-05-18 for the plugin-DLLs


  // > Step 1 : Exit the reset vector
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

  // > Step 2 : Initialize TBLPAG and the read pointer (W6) for TBLRD instruction
  dwOpcode = 0x200000L;             // 200xx0     // prepare opcode for MOV #x, W0
  dwOpcode |= ((dwSourceAddress>>12) & 0x000FF0); // addr bit23..16 -> opcode bit 11..4 (!)
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<SourceAddress23:16>, W0
  dsPIC_STDP_ExecuteCoreInstruction( 0x880190L ); // MOV  W0, TBLPAG
  dwOpcode = 0x200006L;             // 2xxxx6     // prepare opcode for MOV #x, W6
  dwOpcode |= ((dwSourceAddress<<4) & 0x0FFFF0);  // addr bit15..0 -> opcode bit 19..4 (!)
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<SourceAddress15:0>, W6

  // > Step 3 : Initialize the write pointer (W7)
  // >   and store a single 16-bit memory location to W0
  dsPIC_STDP_ExecuteCoreInstruction( 0xEB0380L ); // CLR      W7
  dsPIC_STDP_ExecuteCoreInstruction( 0xBA1B96L ); // TBLRDL   [W6], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

  // > Step 4 : Output W0 using the VISI register and REGOUT command
  dsPIC_STDP_ExecuteCoreInstruction( 0x883C20L ); // MOV      W0, VISI
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  wResult = dsPIC_STDP_ReadVISIRegister();   // clock out contents of VISI register
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

  // > Step 5 : Reset device internal PC
  dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

  // NOT INCLUDED HERE:    dsPIC_LeaveSTDPMode(); // leave STDP programming mode (to allow proper reset)

#if(0) // (0) = release,  (1)=TEST
  wResult = (uint16_t)dwSourceAddress; // TEST - works without a chip :)
#endif

  return wResult;

} // end dsPIC_STDP_Read16BitLocation()

//----------------------------------------------------------------------------
void dsPIC_UnlockNVMCONAndWrite(void) // Unlock NVMCON for programming, initiate write, wait, and clear WR-flag again
{
  // Unlock NVMCON for programming
  dsPIC_STDP_ExecuteCoreInstruction( 0x200558L ); // MOV #0x55, W8
  dsPIC_STDP_ExecuteCoreInstruction( 0x883B38L ); // MOV W8, NVMKEY
  dsPIC_STDP_ExecuteCoreInstruction( 0x200AA9L ); // MOV #0xAA, W9
  dsPIC_STDP_ExecuteCoreInstruction( 0x883B39L ); // MOV W9, NVMKEY

  // Initiate the write cycle
  dsPIC_STDP_ExecuteCoreInstruction( 0xA8E761L ); // BSET NVMCON, #WR
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

  // Externally time 2ms (until write-operation is complete)
  PIC_HW_Delay_us( 2000 );  // externally time 2 msec

  // Clear the WR-bit in NVMCON register to prevent accidents..
  dsPIC_STDP_ExecuteCoreInstruction( 0xA9E761L ); // BCLR NVMCON, #WR
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

} // end dsPIC_UnlockNVMCONAndWrite()



//----------------------------------------------------------------------------
bool dsPIC_STDP_EraseAll( void )  // aka "Bölk Erase"
  // Erase "everything" using STDP as explained in [dsPICps] Chapter 11.5 :
  // > The procedure for erasing program memory (all of code memory,
  // > data memory, EXECUTIVE MEMORY(!!!!), and code-protect bits)
  // > consists of setting NVMCON to 0x407F, unlocking NVMCOM for erasing,
  // > and then executing the programming cycle.
  // > This method for bulk erasing program memory only works for systems
  // > where Vdd is between 4.5 and 5.5 volts. [..]
  // > Note:   PROGRAM MEMORY MUST BE ERASED BEFORE WRITING ANY DATA
  // >         TO PROGRAM MEMORY .
{

  dsPIC_EnterSTDPMode();  // enter STDP programming mode, set PC to 0x0000 .

  // > Step 1 : Exit the reset vector
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

  // > Step 2 : Set NVMCON to erase all of Program Memory
  dsPIC_STDP_ExecuteCoreInstruction( 0x2407FAL ); // MOV  #0x407F, W10
  dsPIC_STDP_ExecuteCoreInstruction( 0x883B0AL ); // MOV  W10, NVMCON

  // > Step 3 : Unlock NVMCON for programming, +
  // > Step 4 : Initiate the erase cycle :
  dsPIC_UnlockNVMCONAndWrite(); // Unlock NVMCON for programming, initiate write, wait, and clear WR-flag again

  dsPIC_LeaveSTDPMode(); // leave STDP programming mode (to allow proper reset)

  return true;  // no chance to detect "success" of this yet !

} // end dsPIC_STDP_EraseAll()


//----------------------------------------------------------------------------
bool dsPIC_STDP_ReadConfigRegs( uint32_t dwSourceAddress, // source is usually 0xF80000
        uint32_t *pdwDestBuffer,   // pointer to destination buffer (array of DWORDs! )
        uint32_t dwNrOfRegisters)  // number of locations (usually 7 config regs a 16 bit)
{
 uint32_t dwWordIndex;
// uint16_t  w;

  dsPIC_EnterSTDPMode();  // enter STDP programming mode
  for ( dwWordIndex=0; dwWordIndex<dwNrOfRegisters; dwWordIndex++ )
   { *pdwDestBuffer++ = dsPIC_STDP_Read16BitLocation( dwSourceAddress + 2*/*!*/dwWordIndex );
#ifdef COMPILING_WINPIC
     APPL_ShowProgress( (100*dwWordIndex) / dwNrOfRegisters );
#endif
   }
  dsPIC_LeaveSTDPMode(); // leave STDP programming mode

  return true;  // no chance to detect "success" of this yet !

} // dsPIC_STDP_ReadConfigRegs()



//----------------------------------------------------------------------------
bool dsPIC_STDP_WriteConfigRegs(
        uint32_t *pdwSourceData,  // pointer to source data (array of DWORDs)
        int   iNrOfRegisters)  // number of locations (usually 7 config regs a 16 bit)
{
 int i;
 uint32_t dwOpcode;

  dsPIC_EnterSTDPMode();  // enter STDP programming mode, set PC to 0x0000 .

  // > Step 1 : Exit the reset vector
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

  // > Step 2 : Initialize the write pointer (W7) for the TBLWT instruction
  dsPIC_STDP_ExecuteCoreInstruction( 0x200007L ); // MOV  #0x0000, W7

  for(i=0;i<iNrOfRegisters;++i){ // steps 3..9 must usually be repeated for SEVEN config regs..

  // > Step 3 : Set NVMCON to program 1 configuration register
  dsPIC_STDP_ExecuteCoreInstruction( 0x24008AL ); // MOV  #0x4008, W10
  dsPIC_STDP_ExecuteCoreInstruction( 0x883B0AL ); // MOV  W10, NVMCON

  // > Step 4 : Initialize the TBLPAG register
  dsPIC_STDP_ExecuteCoreInstruction( 0x200F80L ); // MOV  #0x00F8, W0
  dsPIC_STDP_ExecuteCoreInstruction( 0x880190L ); // MOV  W0, TBLPAG

  // > Step 5 : Load the configuration register data to W0
  dwOpcode = 0x200000L;             // 2xxxx0     // prepare opcode for MOV #x, W0
  dwOpcode|=((pdwSourceData[i]<<4) & 0x0FFFF0);   // config-"word" -> opcode bit 11..4
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #0xFFFF, W0 ( to clear : 0x2FFFF0L )

  // > Step 6 : Set the read pointer (W6) and load the write latch
  dsPIC_STDP_ExecuteCoreInstruction( 0xEB0300L ); // CLR  W6
  dsPIC_STDP_ExecuteCoreInstruction( 0xBB1B96L ); // TBLWTL [W6], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

  // > Step 7 : Unlock NVMCON for programming +
  // > Step 8 : Initiate the write cycle, wait, and clear WR-flag again :
  dsPIC_UnlockNVMCONAndWrite();

  // > Step 9 : Reset device internal PC
  dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

  // > Step 10: Repeat steps 3-9 until all 7 configuration registers are [[cleared]]->PROGRAMMED
    }

  dsPIC_LeaveSTDPMode(); // leave STDP programming mode (to allow proper reset)

  return true;  // no chance to detect "success" of this yet !


} // dsPIC_STDP_WriteConfigRegs()


//----------------------------------------------------------------------------
bool dsPIC_STDP_ClearConfigRegisters( void )
  // Clear the dsPIC's config registers as explained in [dsPICps] Chapter 11.7 :
  // > Configuration registers are not erasable. It is recommended that
  // > all configuration register bits be cleared by programming them
  // > to 0x1 AFTER erasing program memory. [...]
{
  // Instead of implementing TWO routines from chapters 11.7 AND chapter 11.9,
  // only ONE write-routine for the config registers is implemented here.
  // To "clear" the config registers, simply write this dummy array :
  uint32_t dwSevenFFFFs[7] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
  return dsPIC_STDP_WriteConfigRegs( dwSevenFFFFs, 7 );
} // end dsPIC_STDP_ClearConfigRegisters()


//----------------------------------------------------------------------------
bool dsPIC_STDP_WriteCodeMemory(
        uint32_t dwDestAddress,   // device-internal destination address
        uint32_t *pdwSourceData,  // pointer to source data (array of DWORDs)
        uint32_t dwNrOfCodeWords) // number CODE MEMORY WORDS (here: 24 bit each)
  // Writes N locations into the CODE MEMORY .
  //   IMPORTANT: dwNrOfLocations must be a multiple of 32,
  //   because the dsPIC programming algorithms writes 32 locations per "bank".
  // From [dsPICps] Chapter 11.8,  "Writing Code Memory" (via STDP) :
  // > The procedure for writing code memory is similar to the procedure
  // > for clearing the configuration registers, except that 32 instruction
  // > "words"(a 24 bit, wb) are programmed at a time [...]
{
 int i;
 uint32_t dwOpcode;
 uint16_t  wNrOfBlocks = (dwNrOfCodeWords+31L)/32L;  // number of "blocks", 32 instructions each
 long  i32ProgressTotal;
 long i32ProgressPercent;
 bool  fResult = true;

  i32ProgressTotal = wNrOfBlocks;  // prepare calculation of progress indicator
  if(i32ProgressTotal<1) i32ProgressTotal=1;

  dsPIC_EnterSTDPMode();  // enter STDP programming mode, set PC to 0x0000 .

  // > Step 1 : Exit the reset vector
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

//  PHWInfo.iCurrProgAddress = dwDestAddress; // added 2008-05-18 for the plugin-DLLs

  while(wNrOfBlocks--) // from Step 10: "repeat steps 2-9 until all code memory is programmed"
  {

  // > Step 2 : Set NVMCON to program 32 instruction words
  dsPIC_STDP_ExecuteCoreInstruction( 0x24001AL ); // MOV  #0x4001, W10
  dsPIC_STDP_ExecuteCoreInstruction( 0x883B0AL ); // MOV  W10, NVMCON

  for(i=0; i<=7; ++i) // from Step 6: "repeat steps 3-5 eight times" ( 8 * 4 locations)
   {

  // > Step 3 : Initialize the write pointer (W7) for TBLWT instruction
  dwOpcode = 0x200000L;             // 200xx0     // prepare opcode for MOV #x, W0
  dwOpcode |= ((dwDestAddress>>12) & 0x000FF0);   // addr bit23..16 -> opcode bit 11..4 (!)
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<DestinationAddress23:16>, W0
  dsPIC_STDP_ExecuteCoreInstruction( 0x880190L ); // MOV  W0, TBLPAG
  dwOpcode = 0x200007L;             // 2xxxx7     // prepare opcode for MOV #x, W7
  dwOpcode |= ((dwDestAddress<< 4) & 0x0FFFF0);   // addr bit15..0 -> opcode bit 19..4 (!)
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<DestinationAddress15:0>, W7

  // > Step 4 : "Initialize the read pointer (W6) and" (WB: nonsense, see step 5, but..)
  // >          Load W0:W5 with the next 4 instruction words to program
  // See [dsPICps] Figure 11-4 "Packed instruction words in W0:W5" ..
  // LSW0 = Least Significant Word (16bit) of instruction[0] to be programmed -> W0
  // MSB0 = Most  Significant Byte (8bit)  of instruction[0] to be programmed -> lower half of W1
  // LSW0 = Least Significant Word (16bit) of instruction[1] to be programmed -> W2
  // MSB1 = Most  Significant Byte (8bit)  of instruction[1] to be programmed -> upper half of W1
  // etc , up to LSW3 -> Register R5
  dwOpcode = 0x200000L;             // 2xxxx0     // prepare opcode for MOV #x, W0
  dwOpcode|=((pdwSourceData[0]<<4) & 0x0FFFF0);   // LSW0 -> opcode bit 11..4
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<LSW0>, W0
  dwOpcode = 0x200001L;             // 2xxxx1     // prepare opcode for MOV #x, W1
  dwOpcode|=((pdwSourceData[0]>>12)& 0x000FF0);   // MSB0 -> opcode bit 11..4
  dwOpcode|=((pdwSourceData[1]>>4 )& 0x0FF000);   // MSB1 -> opcode bit 19..12
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<MSB1:MSB0>, W1
  dwOpcode = 0x200002L;             // 2xxxx2     // prepare opcode for MOV #x, W2
  dwOpcode|=((pdwSourceData[1]<<4) & 0x0FFFF0);   // LSW1 -> opcode bit 11..4
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<LSW1>, W2
  dwOpcode = 0x200003L;             // 2xxxx3     // prepare opcode for MOV #x, W3
  dwOpcode|=((pdwSourceData[2]<<4) & 0x0FFFF0);   // LSW2 -> opcode bit 11..4
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<LSW2>, W3
  dwOpcode = 0x200004L;             // 2xxxx4     // prepare opcode for MOV #x, W4
  dwOpcode|=((pdwSourceData[2]>>12)& 0x000FF0);   // MSB2 -> opcode bit 11..4
  dwOpcode|=((pdwSourceData[3]>>4 )& 0x0FF000);   // MSB3 -> opcode bit 19..12
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<MSB3:MSB2>, W4
  dwOpcode = 0x200005L;             // 2xxxx5     // prepare opcode for MOV #x, W5
  dwOpcode|=((pdwSourceData[3]<<4) & 0x0FFFF0);   // LSW3 -> opcode bit 11..4
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<LSW3>, W5

  // > Step 5 : Set the read pointer (W6)
  // >          and load the (next set of) write latches
  dsPIC_STDP_ExecuteCoreInstruction( 0xEB0300L ); // CLR  W6
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBB0BB6L ); // TBLWTL [W6++], [W7]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBBDBB6L ); // TBLWTL.B [W6++], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBBEBB6L ); // TBLWTL.B [W6++], [++W7]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBB1BB6L ); // TBLWTL [W6++], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBB0BB6L ); // TBLWTL [W6++], [W7]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBBDBB6L ); // TBLWTL.B [W6++], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBBEBB6L ); // TBLWTL.B [W6++], [++W7]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBB1BB6L ); // TBLWTL [W6++], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP


  // > Step 6 : Repeat steps 3-5 eight times to load the write latches for 32 instructions
      pdwSourceData += 4;  // advance source pointer to the next FOUR instructions
      dwDestAddress += 8;  // advance destination address for FOUR instructions = EIGHT address steps

//      PHWInfo.iCurrProgAddress = dwDestAddress; // added 2008-05-18 for the plugin-DLLs
    } // end for

  // > Step 7 : Unlock NVMCON for programming +
  // > Step 8 : Initiate the write cycle, wait, and clear WR-flag again :
  dsPIC_UnlockNVMCONAndWrite();

  // > Step 9 : Reset device internal PC
  dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

#ifdef COMPILING_WINPIC
  // Update WinPic's progress indicator, very important because this routine is utterly slow !
  i32ProgressPercent = 100 * (i32ProgressTotal - wNrOfBlocks/*Left*/ ) / i32ProgressTotal;
  APPL_ShowProgress( i32ProgressPercent );
  if( APPL_iUserBreakFlag ) // signal for any programming loop to "stop", set via ESCAPE
   { fResult = false;
     break;
   }
#endif

  // > Step 10: Repeat steps 2-9 until all code memory is programmed
  } // end while(wNrOfBlocks--)

  dsPIC_LeaveSTDPMode(); // leave STDP programming mode (to allow proper reset)

  return fResult;

} // end dsPIC_STDP_WriteCodeMemory()


//----------------------------------------------------------------------------
bool dsPIC_STDP_ReadCodeMemory( uint32_t dwSourceAddress, uint32_t *pdwDestBuffer, uint32_t dwNrOfCodeWords )
  // Reads N locations (~instructions) from CODE MEMORY. See [dsPICps] Chapter 11.11 :
  // > Reading from code memory is performed by executing a series of TBLRD
  // > instructions and clocking out the data using the REGOUT command.
  // > To ensure efficient execution and facilitate verification of the
  // > programmer, four instruction words are read from the device a time.
  // > [..] To minimize the reading time, the packed instruction word format
  // > that was utilized for writing is also used for reading (Figure 11-4) .
  //
  //  NOTE:   UTTERLY SLOW !   TAKES A HALF MINUTE TO READ A dsPIC30F2010 !
{
 uint32_t dwTemp, dwOpcode;
 long  i32ProgressTotal;
 long i32ProgressPercent;
 bool  fResult = true;
 uint32_t dwNrOfQuads = (dwNrOfCodeWords+3) / 4;

  i32ProgressTotal = dwNrOfQuads;  // prepare calculation of progress indicator
  if(i32ProgressTotal<1) i32ProgressTotal=1;

  dsPIC_EnterSTDPMode();  // enter STDP programming mode, set PC to 0x0000 .

  // > Step 1 : Exit the reset vector
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

//  PHWInfo.iCurrProgAddress = dwSourceAddress; // added 2008-05-18 for the plugin-DLLs

  while(dwNrOfQuads--) // from Step 6: "repeat steps 2-5 until all desired code memory is read"
  {

  // > Step 2 : Initialize TBLPAG and the read pointer (W6) for TBLRD instruction
  //    ( 2005-11-02: moved this INSIDE the loop because of problems >64k )
  dwOpcode = 0x200000L;             // 200xx0     // prepare opcode for MOV #x, W0
  dwOpcode |= ((dwSourceAddress>>12) & 0x000FF0); // addr bit23..16 -> opcode bit 11..4 (!)
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<SourceAddress23:16>, W0
  dsPIC_STDP_ExecuteCoreInstruction( 0x880190L ); // MOV  W0, TBLPAG
  dwOpcode = 0x200006L;             // 2xxxx6     // prepare opcode for MOV #x, W6
  dwOpcode |= ((dwSourceAddress<<4) & 0x0FFFF0);  // addr bit15..0 -> opcode bit 19..4 (!)
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<SourceAddress15:0>, W6



  // > Step 3 : Initialize the write pointer (W7)
  // >   and store the next four locations of code memory to W0:W5
  // LSW0 = Least Significant Word (16bit) of instruction[0]  -> W0
  // MSB0 = Most  Significant Byte (8bit)  of instruction[0]  -> lower half of W1
  // LSW0 = Least Significant Word (16bit) of instruction[1]  -> W2
  // MSB1 = Most  Significant Byte (8bit)  of instruction[1]  -> upper half of W1
  // etc , up to LSW3 -> Register R5
  dsPIC_STDP_ExecuteCoreInstruction( 0xEB0380L ); // CLR      W7
  dsPIC_STDP_ExecuteCoreInstruction( 0xBA1B96L ); // TBLRDL   [W6], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBADBB6L ); // TBLRDH.B [W6++], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBADBD6L ); // TBLRDH.B [++W6], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBA1BB6L ); // TBLRDL   [W6++], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBA1B96L ); // TBLRDL   [W6], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBADBB6L ); // TBLRDH.B [W6++], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBADBD6L ); // TBLRDH.B [++W6], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBA0BB6L ); // TBLRDL.B [W6++], [W7]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP


  // > Step 4 : Output W0:W5 using the VISI register and REGOUT command
  dsPIC_STDP_ExecuteCoreInstruction( 0x883C20L ); // MOV      W0, VISI   ; W0 = LSW0
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  pdwDestBuffer[0] = dsPIC_STDP_ReadVISIRegister();   // clock out contents of VISI register
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x883C21L ); // MOV      W1, VISI   ; W1 = MSB1:MSB0
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dwTemp = dsPIC_STDP_ReadVISIRegister();             // clock out contents of VISI register
  pdwDestBuffer[0] |= ( dwTemp & 0x0000FF ) << 16;    // combine LSW0 and MSB0
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x883C22L ); // MOV      W2, VISI   ; W2 = LSW1
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  pdwDestBuffer[1] = dsPIC_STDP_ReadVISIRegister();   // clock out contents of VISI register
  pdwDestBuffer[1] |= ( dwTemp & 0x00FF00 ) << 8 ;    // combine LSW1 and MSB1
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x883C23L ); // MOV      W3, VISI   ; W3 = LSW2
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  pdwDestBuffer[2] = dsPIC_STDP_ReadVISIRegister();   // clock out contents of VISI register
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x883C24L ); // MOV      W4, VISI   ; W4 = MSB3:MSB2
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dwTemp = dsPIC_STDP_ReadVISIRegister();             // clock out contents of VISI register
  pdwDestBuffer[2] |= ( dwTemp & 0x0000FF ) << 16;    // combine LSW2 and MSB2
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x883C25L ); // MOV      W5, VISI   ; W5 = LSW3
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  pdwDestBuffer[3] = dsPIC_STDP_ReadVISIRegister();   // clock out contents of VISI register
  pdwDestBuffer[3] |= ( dwTemp & 0x00FF00 ) << 8 ;    // combine LSW3 and MSB3
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

  // > Step 5 : Reset device internal PC
  dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

  // > Step 6 : Repeat steps 3-5 until all desired code memory is read
  pdwDestBuffer += 4;  // advance destination pointer to the next FOUR instructions = TWELVE BYTES
  dwSourceAddress+= 8; // advance source address BY EIGHT(!) for these TWELVE BYTES(!)
      // Note the ugly consequence of Microchip's mad decision
      //      to let the address increment by TWO for every THREE bytes !
//  PHWInfo.iCurrProgAddress = dwSourceAddress; // added 2008-05-18 for the plugin-DLLs

#ifdef COMPILING_WINPIC
  // Update WinPic's progress indicator, very important because this routine is utterly slow !
  i32ProgressPercent = 100 * (i32ProgressTotal - dwNrOfQuads/*Left*/ ) / i32ProgressTotal;
  APPL_ShowProgress( i32ProgressPercent );
  if( APPL_iUserBreakFlag ) // signal for any programming loop to "stop", set via ESCAPE
   { fResult = false;
     break;
   }
#endif // COMPILING_WINPIC

  } // end while(dwNrOfQuads--)

  dsPIC_LeaveSTDPMode(); // leave STDP programming mode (to allow proper reset)

  return fResult;

} // end dsPIC_STDP_ReadCodeMemory()


//----------------------------------------------------------------------------
bool dsPIC_STDP_ReadDataMemory( uint32_t dwSourceAddress, uint32_t *pdwDestBuffer, uint32_t dwNrOfBytes )
  // Reads N locations (here: BYTES!!!) from DATA MEMORY .
  // Why BYTE-WISE, since the dsPIC's DATA EEPROM is organized 16-bit wide ?
  //     For historic reasons (PIC12Fxxx, PIC16Cxx, PIC16Fxxx, ... ),
  //     and because the traders at Microchip specify the EEPROM size in
  //     BYTES , while the CODE MEMORY size is specified in INSTRUCTION WORDS.
  // Furthermore, WinPic's hex editor treats the data eeprom as 8-bit array, basta.
  //     Each entry in "dwDestBuffer" shall only receive ONE character.
  // Example for dsPIC30F2010, "1 kByte EEPROM" :
  //          dwSourceAddress=0x7FFC00, dwNrOfBytes = 1024 .
{
 bool  fResult = true;
 uint32_t dwByteIndex;
 uint16_t  w = 0;

  dsPIC_EnterSTDPMode();  // enter STDP programming mode
  for ( dwByteIndex=0; dwByteIndex<dwNrOfBytes; dwByteIndex++ )
   { // Note: here TWO EEPROM BYTE LOCATIONS are read in a single access !
     // moved to end of loop for debugging purposes: APPL_ShowProgress( (100*i) / PIC_DeviceInfo.lDataEEPROMSizeInByte );
     if( (dwByteIndex&1) == 0)  // only "read" a WORD every 2nd over (FROM EVEN ADDRESS)
      { w = dsPIC_STDP_Read16BitLocation( dwSourceAddress );
        dwSourceAddress += 2;
      }
     else
      { w >>=8;
      }
     *pdwDestBuffer++ = w & 0x00FF;
#ifdef COMPILING_WINPIC
     APPL_ShowProgress( (100*dwByteIndex) / dwNrOfBytes );
     if( APPL_iUserBreakFlag ) // signal for any programming loop to "stop", set via ESCAPE
      { fResult = false;
        break;
      }
#endif // COMPILING_WINPIC
   }
  dsPIC_LeaveSTDPMode(); // leave STDP programming mode
  return fResult;
} // end dsPIC_STDP_ReadDataMemory()

//----------------------------------------------------------------------------
bool dsPIC_STDP_WriteDataMemory(
        uint32_t dwDestAddress,   // device-internal address: DEPENDS ON EEPROM SIZE FOR dsPIC !
        uint32_t *pdwSourceData,  // pointer to source data (array of DWORDs, but only one BYTE per array element!)
        uint32_t dwNrOfBytes )    // for historic reasons: NUMBER OF BYTES(!) to be written
  // Writes N bytes into the DATA MEMORY (EEPROM) .
{
// int i;
 int iNrOfWords = ( dwNrOfBytes+1 ) / 2;
 uint16_t  wData;
 uint32_t dwOpcode;
 long  i32ProgressTotal;
 long i32ProgressPercent;
 bool  fResult = true;


#if(1)   // New routine by DL4YHF, *not* from Microchip's prog-spec , but from
  // DS70052C, Chapter 5.5.4 "Writing One Word of Data EEPROM Memory"
  i32ProgressTotal = iNrOfWords;
  if(i32ProgressTotal<1) i32ProgressTotal=1;

  dsPIC_EnterSTDPMode();  // enter STDP programming mode, set PC to 0x0000 .

  // > Step 1 : Exit the reset vector
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

//  PHWInfo.iCurrProgAddress = dwDestAddress; // added 2008-05-18 for the plugin-DLLs


  while(iNrOfWords--) // from Step 10: "repeat steps 2-9 until all code memory is programmed"
   {

     // Here, the DATA EEPROM is programmed word-by-word,
     //  (acceptable because the 2-ms write interval is not the
     //   worst speed-limiting factor) .
     // Based on an example in the dsPIC30F2010 datasheet, DS70118E, Ch. 7.3.2 :
     // > To write a block of data EEPROM, write to all sixteen
     // > latches first, then set the NVMCON register and program the block.

     // Setup a pointer to DATA EEPROM :
     // 2FC000    MOV     #LOW_ADDR_WORD,W0
     // 2007F1    MOV     #HIGH_ADDR_BYTE,W1
     // 880191    MOV     W1,TBLPAG
     dwOpcode = 0x200000L;                           // opcode MOV #x, W0
     dwOpcode |= ((dwDestAddress<<4) & 0x0FFFF0);    // low address word -> opcode bit 19..4 (!)
     dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #LOW_ADDR_WORD, W0
     dwOpcode = 0x200001L;                           // opcode MOV #x, W1
     dwOpcode |= ((dwDestAddress>>12) & 0x000FF0);   // high address byte-> opcode bit 11..4 (!)
     dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #HIGH_ADDR_BYTE,W1
     dsPIC_STDP_ExecuteCoreInstruction( 0x880191L ); // MOV  W1, TBLPAG

     // Write data value to holding latch ..
     // 2ABCD2    MOV     #0xABCD,W2         ; Get the data (ABCD=dummy)
     // BB1802    TBLWTL  W2,[ W0++ ]        ; write data
     // Here: Combine TWO BYTES from WinPic's source buffer into one word :
     wData  =  (*pdwSourceData++) & 0x00FF;        // byte for EVEN address -> LSB
     wData |= ((*pdwSourceData++) & 0x00FF) << 8;  // byte for ODD address  -> MSB
     dwOpcode = 0x200002L | ((uint32_t)wData<<4);       // prepare opcode for MOV #x, W2
     dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV    #wData, W2
     dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
     dsPIC_STDP_ExecuteCoreInstruction( 0xBB1802L ); // TBLWTL W2,[ W0++ ]
     dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
     dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
      // Note: NVMADR captures write address from the TBLWTL instruction !

     // Setup NVMCON for programming one word to data EEPROM ...
     //  240040   MOV     #0x4004,W0         ; Select data EEPROM for single word op
     //  883B00   MOV     W0,NVMCON          ; Operate Key to allow program operation
     //  200550   MOV     #0x55,W0
     //  883B30   MOV     W0,NVMKEY          ; Write the 0x55 key
     //  200AA1   MOV     #0xAA,W1
     //  883B31   MOV     W1,NVMKEY          ; Write the 0xAA key
     //  A8E761   BSET    NVMCON,#WR         ; Start write cycle
     dsPIC_STDP_ExecuteCoreInstruction( 0x240040L); //  MOV  #0x400A,W0
     dsPIC_STDP_ExecuteCoreInstruction( 0x883B00L); //  MOV  W0,NVMCON
     // Unlock NVMCON for programming,
     // initiate the write cycle,  wait, and clear WR-flag again :
     dsPIC_UnlockNVMCONAndWrite();

     // Reset device internal PC
     dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
     dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

#ifdef COMPILING_WINPIC
     // Update WinPic's progress indicator, very important because this routine is utterly slow !
     i32ProgressPercent = 100 * (i32ProgressTotal - iNrOfWords ) / i32ProgressTotal;
     APPL_ShowProgress( i32ProgressPercent );
     if( APPL_iUserBreakFlag ) // signal for any programming loop to "stop", set via ESCAPE
      { fResult = false;
        break;
      }
#endif

     dwDestAddress += 2;  // dsPIC address increments by TWO for each word
//     PHWInfo.iCurrProgAddress = dwDestAddress; // added 2008-05-18 for the plugin-DLLs
   } // end while(iNrOfWords--)

  dsPIC_LeaveSTDPMode(); // leave STDP programming mode (to allow proper reset)


#else //----------------------------------------------------------------

  // Original code from [dsPICps] Chapter 11.10 ...
  //   IMPORTANT: dwNrOfBytes must be a multiple of 32 ( = 16 "data words"),
  //   because the dsPIC programming algorithms writes 16 EEPROM WORDS(!) per "bank".
 uint16_t  wNrOfBlocks = (dwNrOfBytes+31)/32;  // number of "blocks", 16 words = 32 byte each
  i32ProgressTotal = wNrOfBlocks;  // prepare calculation of progress indicator
  if(i32ProgressTotal<1) i32ProgressTotal=1;


  // From [dsPICps] Chapter 11.10,  "Writing Data Memory" (via STDP) :
  // > The procedure for writing data memory is similar to the procedure
  // > for writing code memory, except that fewer words are programmed
  // > in each operation. When writing data memory, one row of data memory
  // > is programmed at a time. Each row consists of sixteen 16-bit data words.

  dsPIC_EnterSTDPMode();  // enter STDP programming mode, set PC to 0x0000 .

  // > Step 1 : Exit the reset vector
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

  // > Step 2 : Set NVMCON to program 16 data words
  dsPIC_STDP_ExecuteCoreInstruction( 0x24005AL ); // MOV  #0x4005, W10
  dsPIC_STDP_ExecuteCoreInstruction( 0x883B0AL ); // MOV  W10, NVMCON

  for(i=0; i<=3; ++i) // from Step 6: "repeat steps 3-4 four times" ( 4 * 8 bytes )
   {

  // > Step 3 : Initialize the write pointer (W7) for TBLWT instruction
  dwOpcode = 0x200000L;             // 200xx0     // prepare opcode for MOV #x, W0
  dwOpcode |= ((dwDestAddress>>12) & 0x000FF0);   // addr bit23..16 -> opcode bit 11..4 (!)
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<DestinationAddress23:16>, W0
  dsPIC_STDP_ExecuteCoreInstruction( 0x880190L ); // MOV  W0, TBLPAG
  dwOpcode = 0x200007L;             // 2xxxx7     // prepare opcode for MOV #x, W7
  dwOpcode |= ((dwDestAddress<< 4) & 0x0FFFF0);   // addr bit15..0 -> opcode bit 19..4 (!)
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<DestinationAddress15:0>, W7

  // > Step 4 : Load W0:W3 with the next 4 data words to program ( = 8 bytes, 8 address steps)
  dwOpcode = 0x200000L;             // 2xxxx0     // prepare opcode for MOV #x, W0
  dwOpcode|=((pdwSourceData[0]<<4) & 0x000FF0);   // BYTE[0] -> opcode bit 11..4
  dwOpcode|=((pdwSourceData[1]<<12)& 0x0FF000);   // BYTE[1] -> opcode bit 19..12
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<WORD0>, W0
  dwOpcode = 0x200001L;             // 2xxxx1     // prepare opcode for MOV #x, W1
  dwOpcode|=((pdwSourceData[2]<<4) & 0x000FF0);   // BYTE[2] -> opcode bit 11..4
  dwOpcode|=((pdwSourceData[3]<<12)& 0x0FF000);   // BYTE[3] -> opcode bit 19..12
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<WORD1>, W1
  dwOpcode = 0x200002L;             // 2xxxx2     // prepare opcode for MOV #x, W2
  dwOpcode|=((pdwSourceData[4]<<4) & 0x000FF0);   // BYTE[4] -> opcode bit 11..4
  dwOpcode|=((pdwSourceData[5]<<12)& 0x0FF000);   // BYTE[5] -> opcode bit 19..12
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<WORD2>, W2
  dwOpcode = 0x200003L;             // 2xxxx3     // prepare opcode for MOV #x, W3
  dwOpcode|=((pdwSourceData[6]<<4) & 0x000FF0);   // BYTE[6] -> opcode bit 11..4
  dwOpcode|=((pdwSourceData[7]<<12)& 0x0FF000);   // BYTE[7] -> opcode bit 19..12
  dsPIC_STDP_ExecuteCoreInstruction( dwOpcode );  // MOV  #<WORD3>, W3

  // > Step 5 : Set the read pointer (W6) to 0x0000 = "&W0"
  // >          and load the (next set of) write latches
  dsPIC_STDP_ExecuteCoreInstruction( 0xEB0300L ); // CLR  W6
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBB1BB6L ); // TBLWTL [W6++], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBB1BB6L ); // TBLWTL [W6++], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBB1BB6L ); // TBLWTL [W6++], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0xBB1BB6L ); // TBLWTL [W6++], [W7++]
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP


  // > Step 6 : Repeat steps 3-5(!) eight times to load the write latches for 16 words
      pdwSourceData += 8;  // advance source array-pointer to the next EIGHT BYTES
  // ?? dwDestAddress += 8;  // advance destination address for FOUR words = EIGHT address steps
    } // end for

  // > Step 7 : Unlock NVMCON for programming +
  // > Step 8 : Initiate the write cycle, wait, and clear WR-flag again :
  dsPIC_UnlockNVMCONAndWrite();

  // > Step 9 : Reset device internal PC
  dsPIC_STDP_ExecuteCoreInstruction( 0x040100L ); // GOTO 0x0100
  dsPIC_STDP_ExecuteCoreInstruction( 0x000000L ); // NOP

#ifdef COMPILING_WINPIC
  // Update WinPic's progress indicator, very important because this routine is utterly slow !
  i32ProgressPercent = 100 * (i32ProgressTotal - wNrOfBlocks/*Left*/ ) / i32ProgressTotal;
  APPL_ShowProgress( i32ProgressPercent );
  if( APPL_iUserBreakFlag ) // signal for any programming loop to "stop", set via ESCAPE
   { fResult = false;
     break;
   }
#endif

  // > Step 10: Repeat steps 2-9 until all data memory is programmed
  dwDestAddress += 32;  // advance destination address for next SIXTEEN WORDS ? ! ?!

  } // end while(wNrOfBlocks--)

  dsPIC_LeaveSTDPMode(); // leave STDP programming mode (to allow proper reset)

#endif // (new or old algorithm ?)


  return fResult;

} // end dsPIC_STDP_WriteDataMemory()


//----------------------------------------------------------------------------
bool dsPIC_EraseAll(void)
{
 bool fOk;
  // As long as the "programming executive" is not supported by WoBu,
  // must use the slower "STDP" method to erase the chip completely :
  fOk = dsPIC_STDP_EraseAll();       // FIRST erase "All" (not really "All"), ..
  if( fOk )
    fOk &= dsPIC_STDP_ClearConfigRegisters();  // THEN(!) clear config registers

  return fOk;
} // dsPIC_EraseAll()

//----------------------------------------------------------------------------
bool dsPIC_ReadCodeMemory( uint32_t dwSourceAddress, uint32_t *pdwDestBuffer, uint32_t dwNrOfCodeWords )
{ return dsPIC_STDP_ReadCodeMemory( dwSourceAddress, pdwDestBuffer, dwNrOfCodeWords );
}

//----------------------------------------------------------------------------
bool dsPIC_WriteCodeMemory( uint32_t dwDestAddress, uint32_t *pdwSourceData, uint32_t dwNrOfCodeWords )
{ return dsPIC_STDP_WriteCodeMemory( dwDestAddress, pdwSourceData, dwNrOfCodeWords );
}

//----------------------------------------------------------------------------
bool dsPIC_ReadDataMemory( uint32_t dwSourceAddress, uint32_t *pdwDestBuffer, uint32_t dwNrOfBytes )
{ return dsPIC_STDP_ReadDataMemory( dwSourceAddress, pdwDestBuffer, dwNrOfBytes );
}

//----------------------------------------------------------------------------
bool dsPIC_WriteDataMemory( uint32_t dwDestAddress, uint32_t *pdwSourceBuffer, uint32_t dwNrOfBytes )
{ return dsPIC_STDP_WriteDataMemory( dwDestAddress, pdwSourceBuffer, dwNrOfBytes );
}

//----------------------------------------------------------------------------
bool dsPIC_ReadConfigRegs( uint32_t dwSourceAddress, uint32_t *pdwDestBuffer, uint32_t dwNrOfRegisters )
{ return dsPIC_STDP_ReadConfigRegs( dwSourceAddress, pdwDestBuffer, dwNrOfRegisters );
}

//----------------------------------------------------------------------------
bool dsPIC_WriteConfigRegs(uint32_t dwDummyAddress, uint32_t *pdwSourceData, uint32_t dwNrOfRegisters )
{ // "dwDummyAddress" required to maintain compatibility of argument list !
  dwDummyAddress = dwDummyAddress;
  return dsPIC_STDP_WriteConfigRegs( pdwSourceData, dwNrOfRegisters );
}


// EOF < dsPIC_PRG.C >
