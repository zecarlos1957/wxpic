/*-------------------------------------------------------------------------*/
/* PIC16F716_PRG.h                                                         */
/*                                                                         */
/*  Purpose: Serial PIC Programming algorithms for PIC16F716 (in WinPic).  */
/*  Author : Wolfgang Buescher (DL4YHF)                                    */
/*  Date   : 2005-12-03  (ISO 8601 format, YYYY-MM-DD)                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/

  // Revsion history : see  PIC16F716_PRG.c !


#ifdef __cplusplus      /* to call these routines from CPP modules : */
 #define CPROT extern "C"
 // Note: It's 'extern "C"' , not 'extern "c"' as stated in
 //       Borland's bugged help system ! (at least for BCB4)
#else
 #define CPROT          /* to call these routines from ordinary "C": */
#endif  /* nicht "cplusplus" */


//------------------------------------------------------------------------
// External references used by these PIC programming routines..
//------------------------------------------------------------------------
//#ifdef COMPILING_WINPIC
// extern int  PIC_PRG_iSimulateOnly;  // don't simulate, use "the real thing"
// CPROT  void APPL_ShowProgress( int percent ); // the application's "progress indicator" routine
// extern int  APPL_iUserBreakFlag; // signal for any programming loop to "stop", set via ESCAPE
//#endif // COMPILING_WINPIC


//------------------------------------------------------------------------
// Prototypes for "public" functions for programming this PIC family :
//------------------------------------------------------------------------

//----------------------------------------------------------------------------
CPROT void PIC16F716_EraseAll(void);
CPROT bool PIC16F716_WriteCodeMemory( uint32_t *pdwSourceData, int n, uint32_t dwDeviceAddress );
CPROT bool PIC16F716_WriteConfigMemory( uint32_t *pdwSourceData, int n, uint32_t dwDeviceAddress );



// EOF < PIC16F7x_PRG.H >
