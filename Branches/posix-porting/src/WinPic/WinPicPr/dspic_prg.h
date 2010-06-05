/*-------------------------------------------------------------------------*/
/* dspic_prg.h                                                             */
/*                                                                         */
/*  Purpose: Serial PIC Programming algorithms for dsPIC30F .              */
/*  Author : Wolfgang Buescher (DL4YHF)                                    */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*                                                                         */
/*  Date   : 2005-07-01  (ISO 8601 format, YYYY-MM-DD)                     */
/*                                                                         */
/*  Last changes :                                                         */
/*    2006-11-05:  Handed this file (dspic_prg.cpp) over to Yves Rondeau   */
/*                 who plans to support of the dsPIC33 family in WinPic.   */
/*                 No changes by Wolfgang Buescher for a while .           */
/*                                                                         */
/*-------------------------------------------------------------------------*/

  // Revsion history : see  dspic_prg.cpp !

//------------------------------------------------------------------------
// External references used by the dsPIC programming routines..
//------------------------------------------------------------------------
//#ifdef COMPILING_WINPIC
// CPROT  void APPL_ShowProgress( int percent ); // the application's "progress indicator" routine
// extern int  APPL_iUserBreakFlag; // signal for any programming loop to "stop", set via ESCAPE
//#endif // COMPILING_WINPIC



//------------------------------------------------------------------------
// Prototypes for "public" functions for dsPIC programming :
//------------------------------------------------------------------------
bool dsPIC_EnterSTDPMode( void );
void dsPIC_LeaveSTDPMode( void );
uint16_t dsPIC_STDP_Read16BitLocation( uint32_t dwSourceAddress );
bool dsPIC_STDP_ReadCodeMemory(uint32_t dwSourceAddress, uint32_t *pdwDestBuffer, uint32_t dwNrOfCodeWords);
bool dsPIC_STDP_WriteCodeMemory( uint32_t dwDestAddress, uint32_t *pdwSourceData, uint32_t dwNrOfCodeWords);
bool dsPIC_STDP_WriteConfigRegs( uint32_t *pdwSourceData, int iNrOfRegisters);

  // More general routines which may decide themselves which algorithm to use :
  //  If there is a programming executive, use it;  otherwise use the slower STDP method.
bool dsPIC_EraseAll(void);

  // Note: All "Read"- and "Write"- routines must have compatible prototypes,
  //  because they may be called via function pointer from the application
  //  (see type definition of T_PicPrgReadWriteFunc in pic_prg.h ) .
  // The DATA are always passed as array of DWORDs, one uint32_t per "location".
  //  A "location" may be :
  //    - a 12, 14, 16, or 24- bit location in CODE MEMORY
  //    - an 8-bit location in DATA MEMORY (~EEPROM)
  //    - a 16-bit location in CONFIGURATION MEMORY
bool dsPIC_ReadCodeMemory(uint32_t dwSourceAddress, uint32_t *pdwDestBuffer, uint32_t dwNrOfCodeWords );
bool dsPIC_WriteCodeMemory(uint32_t dwDestAddress , uint32_t *pdwSourceData, uint32_t dwNrOfCodeWords );
bool dsPIC_ReadConfigRegs( uint32_t dwSourceAddress,uint32_t *pdwDestBuffer, uint32_t dwNrOfRegisters );
bool dsPIC_WriteConfigRegs(uint32_t dwDestAddress,  uint32_t *pdwSourceData, uint32_t dwNrOfRegisters );

  // Note on DATA MEMORY : Though the access may be WORD-wise internally,
  // treat the EEPROM like a BYTE-memory as in old 14-bit PICs !
bool dsPIC_ReadDataMemory( uint32_t dwSourceAddress,uint32_t *pdwDestBuffer, uint32_t dwNrOfBytes );
bool dsPIC_WriteDataMemory(uint32_t dwDestAddress,  uint32_t *pdwSourceBuffer,uint32_t dwNrOfBytes );

// EOF < dspic_prg.H >
