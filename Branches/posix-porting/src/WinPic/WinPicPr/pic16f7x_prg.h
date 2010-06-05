/*-------------------------------------------------------------------------*/
/* pic16f7xx_prg.h                                                          */
/*                                                                         */
/*  Purpose: Serial PIC Programming algorithms for PIC16F7x (in WinPic).   */
/*  Author : Wolfgang Buescher (DL4YHF)                                    */
/*  Date   : 2005-08-21  (ISO 8601 format, YYYY-MM-DD)                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/

  // Revsion history : see  PIC16F7x_PRG.c !


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


// Possible values for iPicPrgAction (used as parameter in a few "newer" routines)
//  Same defs in PIC16F7x_PRG.H and pic_prg.h !
#define PIC_ACTION_NONE        0
#define PIC_ACTION_READ        1
#define PIC_ACTION_BLANK_CHECK 2
#define PIC_ACTION_VERIFY      4
#define PIC_ACTION_ERASE       8
#define PIC_ACTION_WRITE      16


//------------------------------------------------------------------------
// Prototypes for "public" functions for programming this PIC family :
//------------------------------------------------------------------------


//----------------------------------------------------------------------------
CPROT bool PIC16F7x_ProgramAll(
         bool fEraseAll ); // flag for "normal erase" or "erase EVERYTHING"
   // - Includes erase ("full"~"chip erase" or whatever they call it for this chip)
   // - Only enters programming mode ONCE(!!) because some ugly interfaces
   //   have severe problems entering and leaving programming mode rapidly



// EOF < PIC16F7x_PRG.H >
