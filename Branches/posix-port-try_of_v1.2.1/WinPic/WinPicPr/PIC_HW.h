/*-------------------------------------------------------------------------*/
/* PIC_HW.h                                                                */
/*                                                                         */
/*  Purpose:  low-level hardware access routines                           */
/*            for "W.B.'s PIC-Programmer for Windows"                      */
/*  Author : Wolfgang Buescher (DL4YHF)                                    */
/*  Date   : 2005-02-18  (ISO 8601 format, YYYY-MM-DD)                     */
/*                                                                         */
/*  2009-06-04:  Ported to wxWidget by Philippe Chevrier                   */
/*-------------------------------------------------------------------------*/

   // Lastest mods:
   //  2005-02-18:  Can include this header WITHOUT windows.h now,
   //               which was required for new module dsPIC_PRG.cpp .
   //  2005-08-21:  Made it possible to call some routines in this module
   //               from external "C"-functions (like PIC16F7x_PRG.H) .

#ifndef PIC_HW_H
#define PIC_HW_H
#include "wx/string.h"
#ifdef __cplusplus
 #define CPROT extern "C"
 // Note: It's 'extern "C"' , not 'extern "c"'
 //  as stated in the bugged Borland help system ! (at least for BCB4)
#else
 #define CPROT
#endif  /* nicht "cplusplus" */

 // If WINDOWS.H was *NOT* included before THIS header,
 //   define some basic data types compatible to windows.h / wtypes.h :
#ifndef  WORD
#define WORD unsigned short
#endif
#ifndef  uint32_t
#define uint32_t unsigned long
#endif


#ifndef _PHW_INTF_DLL_H_
 #define BUILDING_DLL 0 // we are not BUILDING a DLL now (only USING them)
// #include <interface_dlls/PHW_intf_dll.h> // header for all "interface-plugins"
#endif // ! def _PHW_INTF_DLL_H_


//#include <tchar.h> //instead will use char

//#include <WinPic\smport\TSmPort.h>  // A.Weitzman's SMALL PORT plus WoBu's C++ wrapper
//                                    // (to replace the good old inportb() & outportb() )


/*------------- Constants and other definitions ---------------------------*/

  // Constant definitions for the used programming interface
  // after adding new interface types, also extend the combo box in
  // WinPicPr.cpp !
  // DO NOT MODIFY (which would make user setting files incompatible)
  // but ADD NEW TYPES AT THE END OF THIS LIST !
#define PIC_INTF_TYPE_UNKNOWN   0
#define PIC_INTF_TYPE_COM84     1 // serial, clock=RTS, data=DTR+CTS, Vpp=TXD (not exactly the same as "JDM" ! )
#define PIC_INTF_TYPE_PIP84_V1  2       // parallel, used by SM6LKM, RB7->PAPER OUT
#define PIC_INTF_TYPE_PIP84_V2  3       // parallel, used by SM6LKM, RB7->ACKNOWLEDGE
#define PIC_INTF_TYPE_LKM_FLASHPR_V1 4  // parallel "Flash PIC programmer" by SM6LKM, 2002-09-29
#define PIC_INTF_TYPE_TAIT_7406_4066 5  // parallel, by David Tait, 1st way
#define PIC_INTF_TYPE_TAIT_7407_4066 6  // parallel, by David Tait, 2nd way
#define PIC_INTF_TYPE_TAIT_7406_PNP  7  // parallel, by David Tait, 3rd way
#define PIC_INTF_TYPE_TAIT_7407_PNP  8  // parallel, by David Tait, 4th way
#define PIC_INTF_TYPE_LPT_AN589      9  // parallel, by Microchip's Application Note "AN589"
#define PIC_INTF_TYPE_CUSTOM_LPT    10  // "custom" interface on parallel port
#define PIC_INTF_TYPE_CUSTOM_COM    11  // "custom" interface on serial port

#define PIC_INTF_TYPE_JDM2          12  // similar to COM84, but uses a tricky way to eliminate external power supply
#define PIC_INTF_TYPE_LPT_NOPPP     13  // NOPPP parallel
//#define PIC_INTF_TYPE_PLUGIN_DLL 13
#define PIC_INTF_TYPE_MAX        14

#define PIC_HW_COM_ADDR_MIN  0x0200   /* min I/O address for COM port */
#define PIC_HW_COM_ADDR_MAX  0x03F8   /* max I/O address for COM port */


/*------------- Data Types ------------------------------------------------*/

typedef struct
{ /* Interface Type and Interface-Type - related PARAMETERS                */

  WORD type;                 // a PIC_INTF_TYPE_xxx - constant
  WORD wSeparateVddAndVppControl;  // 0=no, 1=yes

  char psz80SupportFile[81]; // filename with definitions for a 'custom' interface

  long vpp_on_delay_us;      // delay time *AFTER* raising Vpp from 0 V to 13 V in microseconds

  // Other stuff like bitmasks for port accesses may follow here....
  //   ( but finally went into T_PicHwFuncs , see below )


} T_PIC_INTF_TYPE;

typedef int (*T_InOutFunc)(int iNewState);

#define N_IO_FUNCS 4
typedef struct // T_InOutFuncs
{ // It may be necessary to modify MORE THAN ONE output line at a time, so...
  T_InOutFunc pFunc[N_IO_FUNCS];  // up to 4 Input-Output functions, NULL=unused
} T_InOutFuncs;

typedef struct // T_PicHwFuncs
{
   int  iPicIntfType;        // Table index: a PIC_INTF_TYPE_xxx - code

   // SIGNAL INPUT vectors to hardware access routines...
   T_InOutFuncs GetInBit;   // INPUT function, reads the state of serial data from PIC to PC.
                            //  Should return 1=HIGH or 0=LOW, but nothing else.
                            //  iNewState must be ignored for this 'read-only' access.
   T_InOutFuncs GetOkButton;

   // SIGNAL OUTPUT vectors to hardware access routines.
   // Parameter (passed as argument to the I/O function):
   //   iNewState = 1   means "set the output to a logic HIGH state for the PIC"
   //   iNewState = 0   means "set the output to a logic LOW  state for the PIC"
   //   iNewState < 0   means "leave output unchanged but tell me the current state"
   T_InOutFuncs SetVpp,SetVdd,SetClk,SetData; // << the most important control signals
   T_InOutFuncs SetClkEnable,SetDataEnable;  // param: 1=output enabled, 0=output disabled
   int iStateOfDataOutWhileReading; // LOGIC(!) state of data-output-line while reading (passive output)
                                    // usually '1' = HIGH, so the data line can be pulled low by PIC
   T_InOutFuncs PullMclrToGnd;               // param: 1=tied to GND,  0=not tied to GND but hi-Z (open)
   T_InOutFuncs ConnectTarget;               // param: 1=connected,    0=not connected
   T_InOutFuncs SetGreenLed, SetRedLed;      // param: 1=LED on    0=LED off
   T_InOutFuncs SelectVddLow,SelectVddNorm,SelectVddHigh; // no parameters for these functions

   int iTest73;  // test value to check if CONSTANT TABLES are properly initialized
                 // ( BCB didn't yell when a constant initializer was missing ,
                 //   see initialisation of array "PicHwFuncs" ! )
} T_PicHwFuncs;



/*------------- Variables  ------------------------------------------------*/
//#undef EXTERN
//#ifdef _I_AM_PIC_HW_
// #define EXTERN
//#else
// #define EXTERN extern
//#endif

//extern TSmPort SmallPort;  // an instance of SMALL PORT is required for direct port access

//extern bool PicHw_fUseSmallPort; // must be set by application if SmallPort shall be used
extern T_PIC_INTF_TYPE PIC_HW_interface;  // interface and related parameters
extern int  PicHw_iConnectedToTarget;  // 0=no; >0=yes  (only a "flag", see PIC_HW_ConnectToTarget() )
extern long PicHw_iStateOfDataOutWhileReading;
extern bool PicHw_fLptPortOpened;
extern bool PicHw_fTogglingTxD;  // Must call PicHw_UpdateComOutputBits() periodically
                                 // -as often as possible- if this flag is set
                                 // to produce a TOGGLING signal on the TxD output.

extern wxString PicHw_sz255LastError;
extern int   PicHw_iPresentVddSelection;

extern uint32_t PIC_HW_dwCount500ns; // parameter for PIC_HW_ShortDelay() to wait 500 nanoseconds
extern uint32_t PIC_HW_dwCount50ns;  // parameter for PIC_HW_ShortDelay() to wait 50 nanoseconds

extern T_PicHwFuncs *PicHw_FuncPtr;     // pointers to all hardware I/O functions

//extern T_PHWInfo PHWInfo; // "PIC-Programmer Hardware Info" (for all plugin-DLLs, since 2008-05)

// To read the current state of the control lines (from last access to the outputs)
CPROT int  PicHw_IsTargetConnected(void);  // not the same as PicHw_iConnectedToTarget !
CPROT int  PicHw_IsVppOn(void);            // 1=yes 0=no -1=not supported
CPROT int  PicHw_IsVddOn(void);
CPROT int  PicHw_IsMclrPulledToGnd(void);  // as far as I know, only for Microchip's "AN589"
CPROT int  PicHw_IsClockHigh(void);
CPROT int  PicHw_IsDataOutHigh(void);
CPROT int  PicHw_IsClockEnabled(void);
CPROT int  PicHw_IsDataEnabled(void);

CPROT int  PicHw_GetOkButtonState(void);

CPROT int PIC_HW_SetRedLed(int iNewState);
CPROT int PIC_HW_SetGreenLed(int iNewState);

CPROT void PIC_HW_ShortDelay(uint32_t dwLoops);
CPROT void PIC_HW_Delay_50ns(void);
CPROT void PIC_HW_Delay_500ns(void);


/*------------- COM-port access routines  ---------------------------------*/
      // called from the low-level hardware accesses in PIC_PRG,
      // used to drive the "COM84-style" PIC-programmer
CPROT bool COM_OpenPicPort(void);
CPROT WORD COM_GetPicDataBit(void);
CPROT bool COM_SetPicClockAndData( bool clock_high, bool data_high );
CPROT bool COM_SetPicVpp( bool vpp_high );
CPROT bool COM_ClosePicPort(void);
CPROT bool PicHw_UpdateComOutputBits(void);
CPROT void PicHw_FeedChargePump(void); // required for "JDM 2" and others. Call periodically !


/*------------- LPT-port access routines  ---------------------------------*/
//      // First attempt in February 2002 after discovering that direct port
//      //  access is possible even under Windoze XP,
//      //  using the SmallPort utility.
//      // Up to now, only used to drive the "PIP84" PIC-programmer
//      // and possibly the four variants of DAVID TAIT's PIC programmer.
CPROT  bool LPT_OpenPicPort(void);

CPROT  void LPT_ClosePicPort(void);

// Routine to write the modified bits back to the centronics data port.
//    Also called periodically when not programming, because
//    Windoze likes to fool around with the parallel port
//    if a printer driver is installed (or whatever...)
CPROT bool PicHw_UpdateLptDataBits(void);
CPROT bool PicHw_UpdateLptCtrlBits(void);

CPROT bool PicHw_CheckLptDataBits(void);  // debugging stuff ..

//CPROT void PicHw_LetInterfaceDLLDoGraphicStuff(void);


/*-----------  general Low-Level access for PIC Programmer routines -------*/

/***************************************************************************/
CPROT bool PIC_HW_Init(void);
 /* Initializes the PIC Programmer's hardware interface.
  * The "default" interface (taken from Config) is "opened" if required.
  */


/***************************************************************************/
CPROT void PIC_HW_Close(void);
 /* Cleanup after work, for example close open handles.
  * (don't rely on destructor of the main form !!)
  */


/***************************************************************************/
CPROT bool PIC_HW_SetInterfaceType( int new_interface_type );
CPROT void PIC_HW_Delay_us( int microseconds );
CPROT void PIC_HW_LongDelay_ms( int milliseconds );

CPROT bool PIC_HW_SetVpp( bool vpp_high ); // switch programming voltage (12V)
CPROT bool PIC_HW_SetVdd( bool vdd_on );   // switch PIC supply voltage  (5V +-X)
CPROT bool PIC_HW_SetVddAndVpp(bool vdd_on, bool vpp_on); // switch PIC's supply voltage AND programming voltage (simultaneously)
CPROT bool PIC_HW_CanSelectVdd(void);      // false=NO=cannot, true=YES,I can("production grade")
CPROT bool PIC_HW_PullMclrToGnd(bool force_low); // only for "AN589" ICSP programmer
CPROT bool PIC_HW_SetClockEnable(bool fEnable ); // only for "AN589" ICSP programmer
CPROT bool PIC_HW_SetDataEnable( bool fEnable ); // only for "AN589" ICSP programmer
CPROT bool PIC_HW_SetClockAndData( bool clock_high, bool data_high );
CPROT int  PIC_HW_GetDataBit(void);           // negative return means 'error' here
CPROT void PIC_HW_ClockOut( WORD data_bit );
CPROT int  PIC_HW_ClockIn(void);              // negative return means 'error' here


//----------------------------------------------------------------------------
// Mid Level functions. May be replaced to drive 'intelligent' programmers.
//----------------------------------------------------------------------------
CPROT bool PIC_HW_DisconnectFromTarget(void);// since 2002-09-09
CPROT bool PIC_HW_ConnectToTarget(void);     // since 2002-09-09
CPROT bool PIC_HW_SelectVdd(int iVddLevel);  // 0=select low supply voltage, 1=normal, 2=high voltage
CPROT void PIC_HW_ProgMode(void);            // switch PIC into programming mode
CPROT void PIC_HW_ProgModeOff(void);         // programming voltage off, clock off, data high-z
CPROT void PIC_HW_ResetAndGo(void);          // disconnect ICSP; reset target; run target
CPROT void PIC_HW_SerialOut_14Bit(WORD w);
CPROT void PIC_HW_SerialOut_Command6(int cmd, bool fFlush);  // Sends a 6-bit ICSP command to the PIC
CPROT WORD PIC_HW_SerialRead_14Bit(void);
//CPROT void PicHw_FlushCommand( uint32_t dwCommand,uint32_t dwNumCommandBits,
//                               uint32_t dwData, uint32_t dwNumDataBits );


void RedAndGreenLedOff  (void);
void UpdateLedsForResult(bool fOk);

/* EOF <pic_hw.h> */
#endif // PIC_HW_H
