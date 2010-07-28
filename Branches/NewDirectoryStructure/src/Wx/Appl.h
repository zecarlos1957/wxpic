/*-------------------------------------------------------------------------*/
/* APPL.h                                                                  */
/*     Part of Wolfgang Buescher's  simple PIC-Programmer for Windows.     */
/*     Ported to wxWidget by Philippe Chevrier                             */
/*     Ported to Linux by Erdem U. Altinyurt                               */
/*-------------------------------------------------------------------------*/

//#include <tchar.h> //No linux support for this
#include "CommandOption.h"
#include <stdint.h>
//#ifdef __cplusplus
// #define CPROT extern "C"
//#else
// #define CPROT
//#endif  /* nicht "cplusplus" */


/*-------- Constants  -----------------------------------------------------*/

 // Values to identify the "caller" of a routine ...
#define APPL_CALLER_UNKNOWN  0    // .. or multi-language translator
#define APPL_CALLER_MAIN     1    // main form (user interface)
#define APPL_CALLER_PIC_PRG  2    // PIC programming routines
#define APPL_CALLER_PIC_HEX  4    // PIC Hex-File loading routines (+buffer)
#define APPL_CALLER_SAVE_CFG 0x80 // flag cleared by CFG_Save()

 // Values for APPL_i32CustomizeOptions ...
 //   These flags are loaded from settings.ini,
 //   section [PicMain], parameter 'Customize' .
 // The value is a bitmask with the following meaning (default value = 0):
#define APPL_CUST_NO_INTERFACE_SELECTION 0x0001  // bit 0 : "don't show the interface selection list"
#define APPL_CUST_NO_INTERNAL_DEVICES    0x0002  // bit 1 : "don't use the internal device list"
#define APPL_CUST_NO_HEX_EDITOR          0x0004  // bit 2 : "don't allow using the HEX-EDITOR"
#define APPL_CUST_NO_CONFIG_EDITOR       0x0008  // bit 3 : "don't allow editing the CONFIG word"
#define APPL_CUST_NO_MENU_EDIT           0x0010  // bit 4 : "hide the EDIT menu and all its contents"
#define APPL_CUST_NO_MENU_SPECIALS       0x0020  // bit 5 : "hide the SPECIALS submenu"

#define APPLICATION_NAME (_T("WxPIC"))


/*------- global variables - bad style but this makes living easier -------*/
extern int  APPL_iUserBreakFlag;       // set somewhere when ESC pressed
extern long APPL_i32CustomizeOptions;  // loaded from settings.ini in main form


/*-------- Prototypes -----------------------------------------------------*/

/***************************************************************************/
void APPL_ShowMsg( int caller, int options, const wxChar *pszFormat, ... )
#ifndef UNICODE
__attribute__ ((format (printf, 3, 4)))
#endif
;
  // The application's "error-display" routine.
  // Also called from the PIC - routines (and others),
  //      therefore **NOT** a method of any fancy CLASS.
  // Located somewhere in the "MAIN FORM" or anywhere else...
  //  (the caller doesn't need to know that !!)
  // The "options"-parameter is called "error_level" in other programs .


/***************************************************************************/
void APPL_ShowProgress( int percent );
  // The application's "progress indicator" routine.
  // Also called from the PIC - routines (and others),
  //      therefore **NOT** a method of any CLASS.
  //

/***************************************************************************/
int Appl_CheckUserBreak(void);
  // returns true if user break detected (escape key pressed),
  //         false otherwise.

void APPL_LogEvent( const wxChar * pszFormat, ... );  // logs an "event" with current timestamp

long HexStringToLongint(int nMaxDigits, const wxChar *pszSource);

wxString    &Iso8859_1_TChar     (const char* psz, wxString &Dst);
void         CopyIso8859_1_TChar (wxChar *Dst, const char* psz, int Length);
wxChar       *DupIso8859_1_TChar  (const char* psz);

#ifndef __WXMSW__ //Posix Port emulation layer by E.U.A ;)
#ifndef _TCHAR_to_WXCHAR_
#define _TCHAR_to_WXCHAR_ 1 //definition protector

#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/io.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define _tcscat wxStrcat
#define _tcstol wxStrtol
#define _tcslen wxStrlen
#define _tcscpy wxStrcpy
#define _tcsncpy wxStrncpy
#define _tcsrchr wxStrrchr
#define _tcscmp wxStrcmp
#define _tcsncmp wxStrncmp
#define _tcsicmp wxStricmp
#define _tcstol wxStrtol
#define _stprintf wxSprintf
#define stricmp strcasecmp
#endif

void QueryPerformanceCounter( int64_t* cnt );
void QueryPerformanceFrequency( int64_t* frq );
typedef int HANDLE;
#define INVALID_HANDLE_VALUE EMFILE
typedef struct termios DCB;
typedef int64_t LONGLONG;
typedef int64_t LARGE_INTEGER;
typedef uint32_t DWORD;
typedef uint64_t* HMODULE;

enum{SETDTR=TIOCM_DTR, CLRDTR, SETRTS=TIOCM_RTS, CLRRTS, SETBREAK=TIOCSBRK, CLRBREAK=TIOCCBRK, MS_CTS_ON=TIOCM_CTS };
long GetTickCount(void);
bool EscapeCommFunction( int fd, int flag);
bool GetCommModemStatus( int fd, uint32_t *flag );
uint8_t ReadIoPortByte(uint16_t);
void WriteIoPortByte(uint16_t,uint16_t);
#endif

/* EOF <appl.h> */
