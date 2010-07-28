/*-------------------------------------------------------------------------*/
/* PIC10F_PRG.h                                                            */
/*                                                                         */
/*  Purpose: Serial PIC Programming algorithms for PIC10F20x .             */
/*  Author : Wolfgang Buescher (DL4YHF)                                    */
/*  Date   : 2005-05-01  (ISO 8601 format, YYYY-MM-DD)                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/

  // Revsion history : see  PIC10F_PRG.cpp !

//------------------------------------------------------------------------
// External references used by these PIC programming routines..
//------------------------------------------------------------------------
//#ifdef COMPILING_WINPIC
// extern int  PIC_PRG_iSimulateOnly;  // don't simulate, use "the real thing"
// CPROT  void APPL_ShowProgress( int percent ); // the application's "progress indicator" routine
// extern int  APPL_iUserBreakFlag; // signal for any programming loop to "stop", set via ESCAPE
//#endif // COMPILING_WINPIC


// Possible values for iPicPrgAction (used as parameter in a few "newer" routines)
//  Same defs in PIC10F_PRG.H and PIC_PRG.H !
#define PIC_ACTION_NONE        0
#define PIC_ACTION_READ        1
#define PIC_ACTION_BLANK_CHECK 2
#define PIC_ACTION_VERIFY      4
#define PIC_ACTION_ERASE       8
#define PIC_ACTION_WRITE      16


//------------------------------------------------------------------------
// Prototypes for "public" functions for programming this PIC family :
//------------------------------------------------------------------------
bool PIC10F_EnterProgMode( void );
void PIC10F_LeaveProgMode( void );

//----------------------------------------------------------------------------
bool PIC10F_ReadAll(
      int iPicPrgAction ); // PIC_ACTION_READ, PIC_ACTION_BLANK_CHECK, or PIC_ACTION_VERIFY .

//----------------------------------------------------------------------------
bool PIC10F_ProgramAll(
         int iPicPrgAction );          // flag for "normal erase" or "erase EVERYTHING"
   // - Includes "bulk" erase ("full"~"chip erase" or "standard"~"without fuses")
   // - Only enters programming mode ONCE(!!) because some ugly interfaces
   //   have severe problems entering and leaving programming mode rapidly



// EOF < PIC10F_PRG.H >
