// ------------------------------------------------------------------------------
// PIC programming routines for PIC18F devices
// ------------------------------------------------------------------------------
// Author : Martin van der Werff
// Date   : 19 may 2005  (last modified 2008-05-19 by WoBu)
// ------------------------------------------------------------------------------
// Copyright (C) 2005  Martin van der Werff
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the author be held liable for any damages
// arising from the use of this software.
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose and without fee is hereby granted,
// provided that the above copyright notice and disclaimer appear in all
// copies and supporting documentation.
//
//
// ------------------------------------------------------------------------------
//
// Supported devices (PIC18FXX2/XX8)
//
//   PIC18F242   PIC18F248   PIC18F252   PIC18F258   PIC18F442   PIC18F448
//   PIC18F452   PIC18F458
//
// Supported devices (PIC18F2XX0/2XX5/4XX0/4XX5)
//
//   PIC18F2410  PIC18F4410  PIC18F2420  PIC18F4420  PIC18F2455  PIC18F4455
//   PIC18F2480  PIC18F4480  PIC18F2510  PIC18F4510  PIC18F2515  PIC18F4515
//   PIC18F2520  PIC18F4520  PIC18F2525  PIC18F4525  PIC18F2550  PIC18F4550
//   PIC18F2580  PIC18F4580  PIC18F2585  PIC18F4585  PIC18F2610  PIC18F4610
//   PIC18F2620  PIC18F4620  PIC18F2680  PIC18F4680
//
// These routines were tested using the JDM2 programmer on
//
//   PIC18LF2550 PIC18LF4550 PIC18LF4455 PIC18LF458
//
// ------------------------------------------------------------------------------

// Last modifications:
//  2006-02-28, WoBu: Modified PIC18F_SetDeviceFamily() (now three arguments)
//  2007-01-25, WoBu: Modified "Bulk Erase" in PIC18F_EraseChip_4550()
//          because Microchip seem to have changed the command patterns.
//          At least, a PIC18F4685 failed to erase with the OLD procedure.
//          (reported by Howard Cripe).
//  2008-05-19:  Minor changes for "intelligent" programmers ,
//          and for the planned "hardware interface plugin" (DLL) .
//  2010-01-02, PCh:  Ported to WxPic


#include <stdio.h>

#include "pic_hw.h"
#include "pic18f_prg.h"
#include "../../Wx/Appl.h"
#include <wx/datetime.h>

#define PIC18F_CORE_INSTRUCTION              0
#define PIC18F_SHIFT_OUT_TABLAT_REGISTER     2
#define PIC18F_TABLE_READ_POST_INCREMENT     9
#define PIC18F_TABLE_WRITE                   12
#define PIC18F_TABLE_WRITE_POST_INC_BY_2     13
#define PIC18F_TABLE_WRITE_START_PROGRAMMING 15

// void PIC_HW_Delay_50ns(void);    // now defined in PIC_HW.H ..
// void PIC_HW_Delay_500ns(void);

uint32_t dwStartOfOperation;
uint32_t dwChipFamily = PIC18_FAMILY_458;
uint32_t PIC18F_dwWriteBufferSize_words = 4;  /* .. WORDS ! */
uint32_t PIC18F_dwEraseBufferSize_words = 32; /* .. WORDS ! */
bool  PIC18F_fUseOldBulkEraseCommands= false; // flag for PIC18F_EraseChip_4550(), added 2007-01-25

void PIC18F_SetDeviceFamily(uint32_t dwFamily,           // PIC18_FAMILY_xxxx
             uint32_t dwWriteBufSize_wd,  // (*) size of the WRITE BUFFER in "words" (!)
             uint32_t dwEraseBufSize_wd)  // (*) size of the ERASE BUFFER in "words" (!)
  // (*) these parameters were added 2006-02-28 for more flexibility / new devices
{
  dwChipFamily = dwFamily;
  PIC18F_dwWriteBufferSize_words = dwWriteBufSize_wd;
  PIC18F_dwEraseBufferSize_words = dwEraseBufSize_wd;
}

void PIC18F_SendProgrammingCommand( uint16_t w4BitControlCode )
{
  PIC_HW_SetClockEnable( true );
  PIC_HW_SetDataEnable( true );

  for(int i = 0; i < 4; ++i)
  {
    PIC_HW_SetClockAndData(true, w4BitControlCode & 0x0001 );
    PIC_HW_Delay_50ns();
    PIC_HW_SetClockAndData(false,w4BitControlCode & 0x0001 );
    PIC_HW_Delay_50ns();
    w4BitControlCode >>= 1;
  }
}

void PIC18F_SendPayload( uint16_t wPayload )
{
  for( int i = 0; i < 16; ++i )
  {
    PIC_HW_SetClockAndData(true, wPayload & 1 );
    PIC_HW_Delay_50ns();
    PIC_HW_SetClockAndData(false,wPayload & 1 );
    PIC_HW_Delay_50ns();
    wPayload >>= 1;
  }
  if( PIC_HW_interface.type == PIC_INTF_TYPE_JDM2 )
  {
    PIC_HW_SetClockAndData(false, false);
  }
}

void PIC18F_SendProgrammingCommandWithPayload( uint16_t w4BitControlCode, uint16_t wPayload )
{
  PIC18F_SendProgrammingCommand( w4BitControlCode );
  PIC_HW_Delay_50ns();
  PIC18F_SendPayload( wPayload );
//  PicHw_FlushCommand( w4BitControlCode,4,  wPayload,16 ); // added 2008-05-19
}

void PIC18F_ExecuteCoreInstruction( uint16_t wInstruction )
{
  PIC18F_SendProgrammingCommandWithPayload(PIC18F_CORE_INSTRUCTION, wInstruction);
}

uint8_t PIC18F_ReadFromChip( uint16_t w4BitControlCode )
{
  uint8_t ucResult = 0;

  PIC_HW_SetClockEnable( true );
  PIC_HW_SetDataEnable( true );

  PIC18F_SendProgrammingCommand( w4BitControlCode );

  // Eight clock cycles are required to process the command
  for( int i = 0; i < 8; ++i )
  {
    PIC_HW_SetClockAndData(true,  PicHw_iStateOfDataOutWhileReading );
    PIC_HW_Delay_50ns();
    PIC_HW_SetClockAndData(false, PicHw_iStateOfDataOutWhileReading );
    PIC_HW_Delay_50ns();
  }
//  // Added 2008-05-19: Let "smart" interfaces know what we just sent :
//  PicHw_FlushCommand( w4BitControlCode,4,
//          ((PicHw_iStateOfDataOutWhileReading>0) ? 0x0FF : 0x00),16 );


  // In the next 8 clock cycles we receive the byte read from memory
  PIC_HW_SetDataEnable( false );
  for( int i = 0; i < 8; ++i )
  {
    PIC_HW_SetClockAndData( true, PicHw_iStateOfDataOutWhileReading);
    PIC_HW_Delay_50ns();
    ucResult |= PIC_HW_GetDataBit() << i;
    if(PIC_HW_interface.type==PIC_INTF_TYPE_JDM2)
    {
      PIC_HW_SetClockAndData( false, false );
    }
    else
    {
      PIC_HW_SetClockAndData( false, true  );
    }
    PIC_HW_Delay_50ns();
  }

  PIC_HW_SetDataEnable( true );
  if(PIC_HW_interface.type==PIC_INTF_TYPE_JDM2)
  {
    PIC_HW_SetClockAndData( false, false );
  }

  return ucResult;
}

void PIC18F_SendProgrammingNOP()
{
    // The programming duration is externally timed and is controlled by PGC.
    // After a Start Programming command is issued (4-bit command, ‘1111’), a
    // NOP is issued, where the 4th PGC is held high for the duration
    // of the programming time, P9 (= min 1 ms)
    for(int i = 0; i < 4; ++i)
    {
      PIC_HW_SetClockAndData(true,  false);
      if( i == 3 )
      { // 4th PGC
        PIC_HW_Delay_us( 1100 ); // keep PGC high for P9 (min 1 ms)
      }
      else
      { // PGC 1 - 3
        PIC_HW_Delay_50ns();
      }
      PIC_HW_SetClockAndData(false, false);
      PIC_HW_Delay_50ns();
    }
//    PicHw_FlushCommand( 0/*w4BitControlCode*/, 4/*bits*/,
//                                0/*wPayload*/, 0/*bits*/ ); // added 2008-05-19

    // After PGC is brought low, the programming sequence is terminated. PGC
    // must be held low for the time specified by parameter P10 (= min 100 us)
    // to allow high-voltage discharge of the memory array.
    PIC_HW_Delay_us( 120 );

    // Now send payload for NOP instruction
    PIC18F_SendPayload( 0x0000 );
//    PicHw_FlushCommand( 0/*w4BitControlCode*/, 0/*bits*/,
//                          0x0000/*wPayload*/, 16/*bits*/ ); // added 2008-05-19

}

void PIC18F_ConnectToTarget(const char * message)
{
#ifndef COMPILING_WINPIC
  fprintf(stderr, "%s\n", message);
#endif

  PIC_HW_ConnectToTarget();

  // Sometimes JDM2 fails to bring the pic into programming mode.
  // This redundant sequence seems to solve the problem.
  PIC_HW_ProgMode();
  PIC_HW_ProgModeOff();

  PIC_HW_ProgMode();

  dwStartOfOperation = wxDateTime::Now().GetTicks();
}

void PIC18F_DisconnectFromTarget(const char * message)
{

#ifndef COMPILING_WINPIC
  uint32_t dwEndOfOperation = wxDateTime::Now().GetTicks();
  fprintf(stderr, "%s (took %.3f sec)\n", message, (dwEndOfOperation - dwStartOfOperation) / 1000.0);
#endif

  PIC_HW_ProgModeOff();
}

void PIC18F_SetTablePointer(uint32_t dwTablePointer)
{
  uint16_t TBLPTRU = (dwTablePointer >> 16) & 0xff;
  uint16_t TBLPTRH = (dwTablePointer >> 8) & 0xff;
  uint16_t TBLPTRL = dwTablePointer & 0xff;
  // Set Table Pointer register
  PIC18F_ExecuteCoreInstruction( 0x0e00 | TBLPTRU ); // MOVLW Addr[21:16]
  PIC18F_ExecuteCoreInstruction( 0x6ef8 );           // MOVWF TBLPTRU
  PIC18F_ExecuteCoreInstruction( 0x0e00 | TBLPTRH ); // MOVLW <Addr[15:8]>
  PIC18F_ExecuteCoreInstruction( 0x6ef7 );           // MOVWF TBLPTRH
  PIC18F_ExecuteCoreInstruction( 0x0e00 | TBLPTRL ); // MOVLW <Addr[7:0]>
  PIC18F_ExecuteCoreInstruction( 0x6ef6 );           // MOVWF TBLPTRL
}

void PIC18F_SetEEPROMAddress(uint32_t dwEEADR)
{
  uint16_t EEADRH = (dwEEADR >> 8) & 0xff;
  uint16_t EEADR  = dwEEADR & 0xff;
  // Set Table Pointer register
  PIC18F_ExecuteCoreInstruction( 0x0e00 | EEADR );   // MOVLW <Addr[7:0]>
  PIC18F_ExecuteCoreInstruction( 0x6ea9 );           // MOVWF EEADR
  PIC18F_ExecuteCoreInstruction( 0x0e00 | EEADRH );  // MOVLW <Addr[15:8]>
  PIC18F_ExecuteCoreInstruction( 0x6eaa );           // MOVWF EEADRH
}

uint32_t PIC18F_ReadDeviceID()
{
  PIC18F_ConnectToTarget("Start reading 18F device for DeviceID");

  // Set Table Pointer register to 0x3ffffe (here we find DEVID1)
  PIC18F_SetTablePointer(0x3ffffe);

  uint8_t devID1 = PIC18F_ReadFromChip(PIC18F_TABLE_READ_POST_INCREMENT);

  // at 0x3fffff we find DEVID2
  uint8_t devID2 = PIC18F_ReadFromChip(PIC18F_TABLE_READ_POST_INCREMENT);

  PIC18F_DisconnectFromTarget("Done reading 18F device for DeviceID");

  // The PIC18LF2550 rev A3 reads out as DEVID1 = 42 and DEVID2 = 12

  // return "DEVID2" in bits 15..8 and "DEVID1" in bits 7..0
  return ((uint16_t)devID2) << 8 | devID1;
}

bool PIC18F_ReadCodeMemory(
  uint32_t dwAddress,
  uint32_t *pdwDestBuffer,
  uint32_t dwNumberOfCodeWords
)
{
  bool fResult = true; // returns true if not interrupted, or other error

  PIC18F_ConnectToTarget("Start reading code memory 18F device");

  // Set Table Pointer register to dwAddress (convert to bytes)
  PIC18F_SetTablePointer(dwAddress);

  for( uint32_t i = 0; i < dwNumberOfCodeWords; ++i )
  {
    uint32_t evenByte = (uint32_t) PIC18F_ReadFromChip(PIC18F_TABLE_READ_POST_INCREMENT);
    uint32_t oddByte  = (uint32_t) PIC18F_ReadFromChip(PIC18F_TABLE_READ_POST_INCREMENT);
    pdwDestBuffer[i] =  (oddByte << 8) | evenByte;

    APPL_ShowProgress( 100 * i / dwNumberOfCodeWords );
    if( APPL_iUserBreakFlag ) // signal for any programming loop to "stop", set via ESCAPE
    {
      fResult = false;
      break;
    }
  }

  PIC18F_DisconnectFromTarget("Done reading code memory 18F device");

  return fResult;
}

bool PIC18F_WriteCodeMemory(
  uint32_t dwAddress,
  uint32_t *pdwSourceData,
  uint32_t dwNumberOfCodeWords
)
{
  uint32_t dwChipWriteBufferSize = 16; // write-buffer-size in WORDS (not BYTES)
  bool fResult = true; // returns true if not interrupted, or other error

  PIC18F_ConnectToTarget("Start writing code memory 18F device");

  // Programming code memory is accomplished by first loading data into
  // the write buffer and then initiating a programming sequence.
  //
  // The size of the write buffer differs between devices
  //
  //  8 bytes  PIC18F2410, PIC18F2510, PIC18F4410, PIC18F4510
  //           PIC18F2420, PIC18F2520, PIC18F4420, PIC18F4520
  //           PIC18F2480, PIC18F2580, PIC18F4480, PIC18F4580
  //
  // 32 bytes  PIC18F2455, PIC18F2550, PIC18F4455, PIC18F4550
  //
  // 64 bytes  PIC18F2515, PIC18F2610, PIC18F4515, PIC18F4610
  //           PIC18F2525, PIC18F2620, PIC18F4585, PIC18F4280
  //           PIC18F2585, PIC18F2680, PIC18F4585, PIC18F4680
  //
  // The 458 family of devices have an 8 byte buffer for each panel (=8K flash)
  // Since 2006-02-28, the WRITE- and ERASE-"buffer"-size is defined in the
  // device definition file, and passed as an argument to
  // ONLY IF THAT INFORMATION IS OBVIOUSLY MISSING, we set it here:
  if( PIC18F_dwWriteBufferSize_words > 0 )
   { dwChipWriteBufferSize = PIC18F_dwWriteBufferSize_words;
   }
  else  // must GUESS the write-buffer-size :
   { switch(dwChipFamily)  // (the "old" way, before 2006-02-28)
      {
        case PIC18_FAMILY_458  : dwChipWriteBufferSize =  4; break;
        case PIC18_FAMILY_4580 : dwChipWriteBufferSize =  4; break;
        case PIC18_FAMILY_4550 : dwChipWriteBufferSize = 16; break;
        case PIC18_FAMILY_4680 : dwChipWriteBufferSize = 32; break;
        default: return false;
      }
   }

  // Disable the Multi-Panel Write mode for the 458 family of devices. This is
  // slower, but allows us to use PIC18F_WriteCodeMemory for code memory, id
  // locations and the boot block. Writing the full code memory (32K) of the
  // 18f458 takes now 7.8 seconds. This could be reduced to about 2.5 seconds
  // by using the Multi-Panel Write mode.
  if( dwChipFamily == PIC18_FAMILY_458 )
  {
    // Enable writes and direct access to config memory
    PIC18F_ExecuteCoreInstruction(0x8ea6); // BSF EECON1, EEPGD
    PIC18F_ExecuteCoreInstruction(0x8ca6); // BSF EECON1, CFGS
    // Write 00h to 3C0006h to enable single panel writes
    PIC18F_ExecuteCoreInstruction(0x0e3c); // MOVLW 3Ch
    PIC18F_ExecuteCoreInstruction(0x6ef8); // MOVWF TBLPTRU
    PIC18F_ExecuteCoreInstruction(0x0e00); // MOVLW 00h
    PIC18F_ExecuteCoreInstruction(0x6ef7); // MOVWF TBLPTRH
    PIC18F_ExecuteCoreInstruction(0x0e06); // MOVLW 06h
    PIC18F_ExecuteCoreInstruction(0x6ef6); // MOVWF TBLPTRL
    PIC18F_SendProgrammingCommandWithPayload(PIC18F_TABLE_WRITE, 0x0000); // Write 00h to 3C0006h
  }

  uint32_t dwNumberOfCodeWordsLeft = dwNumberOfCodeWords;
  uint32_t dwCodeWordPtr = 0;
  do
  {
    // Direct access to code memory and enable writes
    PIC18F_ExecuteCoreInstruction(0x8ea6); // BSF EECON1, EEPGD
    PIC18F_ExecuteCoreInstruction(0x9ca6); // BCF EECON1, CFGS

    // Set Table Pointer register to dwAddress
    PIC18F_SetTablePointer(dwAddress + (dwCodeWordPtr << 1));

    uint32_t dwNumWordsToWrite = dwNumberOfCodeWordsLeft;
    if( dwNumWordsToWrite > dwChipWriteBufferSize )
      dwNumWordsToWrite = dwChipWriteBufferSize;

    for(uint32_t i = 0; i < dwNumWordsToWrite; ++i)
    {
      uint16_t word = (uint16_t)pdwSourceData[dwCodeWordPtr + i];
      PIC18F_SendProgrammingCommandWithPayload( (i == (dwNumWordsToWrite-1)) ?
        PIC18F_TABLE_WRITE_START_PROGRAMMING : PIC18F_TABLE_WRITE_POST_INC_BY_2,
        word);
    }

    PIC18F_SendProgrammingNOP();

    dwNumberOfCodeWordsLeft -= dwNumWordsToWrite;
    dwCodeWordPtr += dwNumWordsToWrite;

    APPL_ShowProgress( 100 * (dwNumberOfCodeWords - dwNumberOfCodeWordsLeft) / dwNumberOfCodeWords );
    if( APPL_iUserBreakFlag ) // signal for any programming loop to "stop", set via ESCAPE
    {
      fResult = false;
      break;
    }
  }
  while( dwNumberOfCodeWordsLeft > 0 );

  PIC18F_DisconnectFromTarget("Done writing code memory 18F device");

  return fResult;
}

bool PIC18F_EraseChip_458 (void)
{
  PIC18F_ConnectToTarget("Start erasing 18F device (458 family)");

  // Erasing code or data EEPROM is accomplished by writing an “erase option”
  // to address 3C0004h. Code memory may be erased portions at a time, or the
  // user may erase the entire device in one action. “Bulk Erase” operations
  // will also clear any code protect settings associated with the memory block
  // erased.
  //
  // Chip Erase          80h
  // Erase Data EEPROM   81h
  // Erase Boot Block    83h
  // Erase Panel 1       88h
  // Erase Panel 2       89h
  // Erase Panel 3       8Ah
  // Erase Panel 4       8Bh

  // Write 80h to 3C0004h to to erase entire device
  PIC18F_ExecuteCoreInstruction(0x0e3c); // MOVLW 3Ch
  PIC18F_ExecuteCoreInstruction(0x6ef8); // MOVWF TBLPTRU
  PIC18F_ExecuteCoreInstruction(0x0e00); // MOVLW 00h
  PIC18F_ExecuteCoreInstruction(0x6ef7); // MOVWF TBLPTRH
  PIC18F_ExecuteCoreInstruction(0x0e04); // MOVLW 04h
  PIC18F_ExecuteCoreInstruction(0x6ef6); // MOVWF TBLPTRL
  PIC18F_SendProgrammingCommandWithPayload(PIC18F_TABLE_WRITE, 0x0080); // Write 80h TO 3C0004h
  PIC18F_ExecuteCoreInstruction(0x0000); // NOP

  PIC18F_SendProgrammingCommand(0);

  // The actual Bulk Erase function is a self-timed operation. Once the erase
  // has started (falling edge of the 4th SCLK after the WRITE command), serial
  // execution will cease until the erase completes (P11 = min 5 ms). During
  // this time, SCLK may continue to toggle, but SDATA must be held low.

  // keep SDATA low for P11
  PIC_HW_Delay_us( 5500 );

  PIC18F_DisconnectFromTarget("Done erasing 18F device (458 family)");

  return true;
}

bool PIC18F_EraseChip_4550 (void)
{
  PIC18F_ConnectToTarget("Start erasing 18F device (4550 family)");

  // Erasing code or data EEPROM is accomplished by configuring two Bulk
  // Erase Control registers located at 3C0004h and 3C0005h. Code memory may
  // be erased portions at a time, or the user may erase the entire device in
  // one action. Bulk Erase operations will also clear any code-protect
  // settings associated with the memory block erased.
  //
  // Chip Erase                  0F87h (*)
  // Erase Data EEPROM           0084h
  // Erase Boot Block            0081h
  // Erase Config Bits           0082h
  // Erase Code EEPROM Block 0   0180h
  // Erase Code EEPROM Block 1   0280h
  // Erase Code EEPROM Block 2   0480h
  // Erase Code EEPROM Block 3   0880h
  //
  // (*) Note on Chip Erase: One fine day, Microchip changed
  //     the chip erase pattern from 0x0F0F 0x8787 to 0x3F3F 0x8F8F
  //     ... at least document DS39622J says on page 14 .
  //  The question is, will OLDER chips which *did* work with the
  //  original code (based on DS39622B) still work with the NEWER algorithm ?
  //  To avoid trouble with this right from the start, the flag
  //  'PIC18F_fUseOldBulkEraseCommands' was added by WoBu (2007-01-25) .

  // Write 0F87h (*) to 3C0004/5 to to erase entire device
  PIC18F_ExecuteCoreInstruction(0x0e3c); // MOVLW 3Ch
  PIC18F_ExecuteCoreInstruction(0x6ef8); // MOVWF TBLPTRU
  PIC18F_ExecuteCoreInstruction(0x0e00); // MOVLW 00h
  PIC18F_ExecuteCoreInstruction(0x6ef7); // MOVWF TBLPTRH
  PIC18F_ExecuteCoreInstruction(0x0e05); // MOVLW 05h
  PIC18F_ExecuteCoreInstruction(0x6ef6); // MOVWF TBLPTRL
  if( PIC18F_fUseOldBulkEraseCommands )
   {  // This is the OLD code, which worked ok until Microchip changed something somewhere :o(
      PIC18F_SendProgrammingCommandWithPayload(PIC18F_TABLE_WRITE, 0x0f0f); // Write 0Fh to 3C0005h
   }
  else  // command pattern for "Bulk Erase" changed by Microchip ?! ?! ?!
   {  PIC18F_SendProgrammingCommandWithPayload(PIC18F_TABLE_WRITE, 0x3f3f); // Write 3Fh to 3C0005h
   }
  PIC18F_ExecuteCoreInstruction(0x0e3c); // MOVLW 3Ch
  PIC18F_ExecuteCoreInstruction(0x6ef8); // MOVWF TBLPTRU
  PIC18F_ExecuteCoreInstruction(0x0e00); // MOVLW 00h
  PIC18F_ExecuteCoreInstruction(0x6ef7); // MOVWF TBLPTRH
  PIC18F_ExecuteCoreInstruction(0x0e04); // MOVLW 04h
  PIC18F_ExecuteCoreInstruction(0x6ef6); // MOVWF TBLPTRL
  if( PIC18F_fUseOldBulkEraseCommands )
   {  // This is the OLD code, which worked ok until Microchip changed something somewhere :o(
      // DS39622B (yet another "Flash Memory Programming Specification") wanted it this way:
      PIC18F_SendProgrammingCommandWithPayload(PIC18F_TABLE_WRITE, 0x8787); // Write 87h TO 3C0004h
   }
  else // new command pattern for "Bulk Erase" changed by Microchip ?! ?! ?!
   {   // DS39622J (= newer "Flash Memory Programming Specification") wants this:
      PIC18F_SendProgrammingCommandWithPayload(PIC18F_TABLE_WRITE, 0x8F8F); // Write 8Fh TO 3C0004h
   }

  PIC18F_ExecuteCoreInstruction(0x0000); // NOP

  PIC18F_SendProgrammingCommand(0);

  // The actual Bulk Erase function is a self-timed operation. Once the erase
  // has started (falling edge of the 4th PGC after the NOP command), serial
  // execution will cease until the erase completes (P11 = min 5 ms).
  // During this time, PGC may continue to toggle but PGD must be held low.

  // PGC must further be held low for the time specified by parameter P10
  // (= min 100 us) to allow high-voltage discharge of the memory array.
  // [this is not in the text but it is in the bulk erase timing figure (3-2)]

  // keep PGD low for P11 + P10 (5 ms + 100us)
  PIC_HW_Delay_us( 5500 );

  PIC18F_DisconnectFromTarget("Done erasing 18F device (4550 family)");

  return true;
}

bool PIC18F_EraseChip (void)
{
  switch(dwChipFamily)
  {
    case PIC18_FAMILY_458  : return PIC18F_EraseChip_458();
    case PIC18_FAMILY_4580 :
    case PIC18_FAMILY_4550 :
    case PIC18_FAMILY_4680 : return PIC18F_EraseChip_4550();
    default: return false;
  }
}

bool PIC18F_ReadDataMemory(
  uint32_t dwAddress,
  uint32_t *pdwDestBuffer,
  uint32_t dwNumberOfBytes
)
{
  bool fResult = true; // returns true if not interrupted, or other error

  PIC18F_ConnectToTarget("Start reading data memory 18F device");

  // Data EEPROM is accessed one byte at a time via an address pointer
  // (register pair EEADRH:EEADR) and a data latch (EEDATA). Data EEPROM is
  // read by loading EEADRH:EEADR with the desired memory location and
  // initiating a memory read by appropriately configuring the EECON1 register.
  // The data will be loaded into EEDATA, where it may be serially output on
  // PGD via the 4-bit command, ‘0010’.

  // Step 1: Direct access to data EEPROM

  PIC18F_ExecuteCoreInstruction(0x9ea6); // BCF EECON1, EEPGD
  PIC18F_ExecuteCoreInstruction(0x9ca6); // BCF EECON1, CFGS

  for( uint32_t i = 0; i < dwNumberOfBytes; ++i )
  {
    // Step 2: Set the data EEPROM address pointer

    PIC18F_SetEEPROMAddress(dwAddress + i);

    // Step 3: Initiate a memory read

    PIC18F_ExecuteCoreInstruction(0x80a6); // BSF EECON1, RD

    // Step 4: Load data into the Serial Data Holding register

    PIC18F_ExecuteCoreInstruction( 0x50a8 ); // MOVF EEDATA, W, 0
    PIC18F_ExecuteCoreInstruction( 0x6ef5 ); // MOVWF TABLAT
    PIC18F_ExecuteCoreInstruction( 0x0000 ); // NOP
    pdwDestBuffer[i] = PIC18F_ReadFromChip(PIC18F_SHIFT_OUT_TABLAT_REGISTER);

    APPL_ShowProgress( 100 * i / dwNumberOfBytes );
    if( APPL_iUserBreakFlag ) // signal for any programming loop to "stop", set via ESCAPE
    {
      fResult = false;
      break;
    }
  }

  PIC18F_DisconnectFromTarget("Done reading data memory 18F device");

  return fResult;
}

bool PIC18F_WriteDataMemory(
  uint32_t dwAddress,       // address in bytes
  uint32_t *pdwDestBuffer,  // only lower 8 bit in each uint32_t used here
  uint32_t dwNumberOfBytes  // for EEPROM, count of bytes(!) to be read
)
{
  bool fResult = true; // returns true if not interrupted, or other error

  PIC18F_ConnectToTarget("Start writing data memory 18F device");

  // Data EEPROM is accessed one byte at a time via an address pointer
  // (register pair EEADRH:EEADR) and a data latch (EEDATA). Data EEPROM is
  // written by loading EEADRH:EEADR with the desired memory location, EEDATA
  // with the data to be written and initiating a memory write by appropriately
  // configuring the EECON1 register. A byte write automatically erases the
  // location and writes the new data (erase-before-write).

  // Step 1: Direct access to data EEPROM
  //
  //   When using the EECON1 register to perform a data EEPROM write, both the
  //   EEPGD and CFGS bits must be cleared (EECON1<7:6> = 00).

  PIC18F_ExecuteCoreInstruction(0x9ea6); // BCF EECON1, EEPGD
  PIC18F_ExecuteCoreInstruction(0x9ca6); // BCF EECON1, CFGS

  for(uint32_t i = 0; i < dwNumberOfBytes; ++i)
  {
    // Step 2: Set the data EEPROM address pointer

    PIC18F_SetEEPROMAddress(dwAddress + i);

    // Step 3: Load the data to be written

    uint16_t EEDATA = (uint16_t)pdwDestBuffer[i];
    PIC18F_ExecuteCoreInstruction( 0x0e00 | EEDATA ); // MOVLW <Data>
    PIC18F_ExecuteCoreInstruction( 0x6ea8 );          // MOVWF EEDATA

    // Step 4: Enable memory writes.
    //
    //   The WREN bit must be set (EECON1<2> = 1) to enable writes of any sort
    //   and this must be done prior to initiating a write sequence.

    PIC18F_ExecuteCoreInstruction( 0x84a6 ); // BSF EECON1, WREN

    if( dwChipFamily == PIC18_FAMILY_458 )
    {
      // To help prevent inadvertent writes when using the EECON1 register,
      // EECON2 is used to “enable” the WR bit. This register must be
      // sequentially loaded with 55h and then, AAh, immediately prior to
      // asserting the WR bit in order for the write to occur.

      PIC18F_ExecuteCoreInstruction( 0x0e55 ); // MOVLW 0x55
      PIC18F_ExecuteCoreInstruction( 0x6ea7 ); // MOVWF EECON2
      PIC18F_ExecuteCoreInstruction( 0x0eaa ); // MOVLW 0xAA
      PIC18F_ExecuteCoreInstruction( 0x6ea7 ); // MOVWF EECON2
    }

    // Step 5: Initiate write.
    //
    //   The write sequence is initiated by setting the WR bit (EECON1<1> = 1).
    //   The write begins on the falling edge of the 4th PGC after the WR bit is
    //   set.

    PIC18F_ExecuteCoreInstruction( 0x82a6); // BSF EECON1, WR

    // Step 6: Poll WR bit, repeat until the bit is clear
    //
    //   It ends when the WR bit is cleared by hardware.

    uint8_t wr;
    int count = 0;
    do
    {
      PIC18F_ExecuteCoreInstruction( 0x50a6 ); // MOVF EECON1, W, 0
      PIC18F_ExecuteCoreInstruction( 0x6ef5 ); // MOVWF TABLAT
      PIC18F_ExecuteCoreInstruction( 0x0000 ); // NOP
      wr = PIC18F_ReadFromChip(PIC18F_SHIFT_OUT_TABLAT_REGISTER);
      ++count;
      if( APPL_iUserBreakFlag ) // signal for any programming loop to "stop", set via ESCAPE
      {
        fResult = false;
        break;
      }
    }
    while( ((wr & 0x02) == 0x02) && (count < 100) );

    // Step 7: Hold PGC low for time P10
    //
    //   After the programming sequence terminates, PGC must still be held low for
    //   the time specified by parameter P10 (= min 100 us) to allow high-voltage
    //   discharge of the memory array.

    PIC_HW_Delay_us( 110 );

    // Step 8: Disable writes

    PIC18F_ExecuteCoreInstruction( 0x94a6 ); // BCF EECON1, WREN

    APPL_ShowProgress( 100 * i / dwNumberOfBytes );
    if( APPL_iUserBreakFlag ) // signal for any programming loop to "stop", set via ESCAPE
    {
      fResult = false;
      break;
    }
  }
  PIC18F_DisconnectFromTarget("Done writing data memory 18F device");

  return fResult;
}

bool PIC18F_ReadConfigRegs(uint32_t dwSourceAddress, uint32_t *pdwDestBuffer, uint32_t dwNrOfRegisters)
{
  // The code memory reading algorithm will work to read any memory in the
  // 000000h to 3FFFFFh address space, so it also applies to the reading of
  // the ID and Configuration registers

  return PIC18F_ReadCodeMemory(dwSourceAddress, pdwDestBuffer, dwNrOfRegisters);
}

bool PIC18F_WriteConfigRegs(uint32_t dwDestAddress, uint32_t *pdwSourceData, uint32_t dwNrOfRegisters)
{
  PIC18F_ConnectToTarget("Start writing configuration words 18F device");

  // Note: Enabling the write protection of configuration bits (WRTC = 0 in
  //       CONFIG6H) will prevent further writing of configuration bits. Always
  //       write all the configuration bits before enabling the write protection
  //       for configuration bits.
  // Note 2 (by Wolf; nothing changed, just for clarity) :
  //       Though Microchip treats the PIC18F's config memory like an
  ///      array of BYTES, we treat them as 16-BIT-"WORD"-registers here.
  //       For example, a PIC18F252 has SEVEN CONFIG REGISTERS (plus one unused dummy
  //       at address 0x300000) so dwNrOfRegisters will usually be '8' here
  //        for the following registers:
  //       CONFIG0L,CONFIG0H,  CONFIG1L,CONFIG1H, ...  , CONFIG7L,CONFIG7H.
  //
  for(uint32_t ptr = 0; ptr < dwNrOfRegisters; ++ptr)
  {
    // Enable writes and direct access to config memory
    PIC18F_ExecuteCoreInstruction(0x8ea6); // BSF EECON1, EEPGD
    PIC18F_ExecuteCoreInstruction(0x8ca6); // BSF EECON1, CFGS

    // Set Table Pointer register to configuration word
    uint32_t dwTablePointer = (dwDestAddress & 0xfffffffe) + (ptr << 1);
    PIC18F_SetTablePointer( dwTablePointer );

    uint16_t word = (uint16_t)pdwSourceData[ptr];

    PIC18F_SendProgrammingCommandWithPayload(PIC18F_TABLE_WRITE_START_PROGRAMMING, word);
    PIC18F_SendProgrammingNOP();

    uint16_t TBLPTRL = ++dwTablePointer & 0xff;
    PIC18F_ExecuteCoreInstruction( 0x0e00 | TBLPTRL ); // MOVLW <Addr[7:0]>
    PIC18F_ExecuteCoreInstruction( 0x6ef6 );           // MOVWF TBLPTRL

    PIC18F_SendProgrammingCommandWithPayload(PIC18F_TABLE_WRITE_START_PROGRAMMING, word);
    PIC18F_SendProgrammingNOP();
  }

  PIC18F_DisconnectFromTarget("Done writing configuration words 18F device");

  return true;
}

