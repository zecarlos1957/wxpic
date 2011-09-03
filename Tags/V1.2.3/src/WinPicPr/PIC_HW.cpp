/*-------------------------------------------------------------------------*/
/* PIC_HW.cpp                                                              */
/*                                                                         */
/*   low-level hardware access routines                                    */
/*   for "W.B.'s PIC-Programmer for Windows"                               */
/*                                                                         */
/* Last changes ...                                                        */
/* 2009-10-07: Replace SmallPort that is incompatible with VISTA and       */
/*             PortTalk that causes a License issue by WinRing0            */
/*             See WinRing0 License below                                  */
/*                                                                         */
/* 2010-6-10   Ported to Linux by Erdem U. Altinyurt                       */
/*-------------------------------------------------------------------------*/

/*Copyright (c) 2007-2009 OpenLibSys.org. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/

#include <wx/intl.h> //-- intl.h must be included before OlsApiInit.h because it uses _T macro without declaring it

#ifdef __WXMSW__
#include <windows.h>
#include <WinRing0/OlsApiInit.h>  //-- WinRing0 Header
#endif

#include <stdio.h>     // don't panic.. just required for sprintf !
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>
#include <wx/log.h>

#include "Config.h"    // permanently saved Config-structure
#include <Appl.h>      // call the APPLication to display message strings

//#include <WinPic/smport/TSmPort.h>   // A.Weitzman's SMALL PORT plus WoBu's C++ wrapper
//                       // (to replace the good old inportb() & outportb() )

#include "PIC_PRG.h"  // needs some timing information from PIC_dev_param.xyz

#define _I_AM_PIC_HW_ 1
#include "PIC_HW.h"   // Routines to drive the programmer hardware interface


//-----------  Global Vars -------------------------------------------------
// (ugly old "C" style, but there will be only one instance running anyway)

//TSmPort SmallPort;  // an instance of SMALL PORT is required for direct port access
//bool PicHw_fUseSmallPort = false;  // must be set by application if SmallPort shall be used
bool PicHw_fLptPortOpened = false;
int  PicHw_iWrittenLptDataBits = -1;  // LPT port data bits which have been written
// during last port access. Used to check if windows "fools around"
// with the port (which it does, at least under WinXP with a HP printer driver).
int PicHw_iPresentVddSelection = false;

//-- The Handle to the LPT driver
HMODULE m_hOpenLibSys = NULL;

T_PIC_INTF_TYPE PIC_HW_interface;  // interface and related parameters

wxChar PicHw_sz255LastError[256];  // only changed if a function returned AN ERROR

T_PicHwFuncs *PicHw_FuncPtr = NULL;
long PicHw_iStateOfDataOutWhileReading = 1;

bool PrivilegeRequested = false;

int  PicHw_iConnectedToTarget=0; // 0=no; >0=yes  (only a "flag", see PIC_HW_ConnectToTarget() )

// For dynamically loaded "hardware-access plugins" (DLLs) :
//T_PHWInfo PHWInfo;  // "PIC-Programmer Hardware Info" (for all plugin-DLLs)
HMODULE PicHw_hFilterPluginDLL=NULL; // handle to the interface plugin DLL,
                                     // NULL = no plugin-DLL loaded .
// Function pointer *types* for the PIC-Hardware-Interface DLL .
//   MUST MATCH THE PROTOTYPES in PHW_intf_dll.h !
//   Only used for dynamically loading the DLL and casting the results of
//   GetProcAddress() into the proper function-pointer types.
#define API_IMPORT(type) __declspec(dllimport) type
//typedef API_IMPORT(int)(*T_PHW_Init)(T_PHWInfo *pInfo);
//typedef API_IMPORT(int)(*T_PHW_Exit)(T_PHWInfo *pInfo);
//typedef API_IMPORT(void)(*T_PHW_OnTimer)(T_PHWInfo *pInfo);
//typedef API_IMPORT(int)(*T_PHW_ExecCmd)(T_PHWInfo *pInfo, char *pszCommand, char *psz255Response);
//typedef API_IMPORT(int)(*T_PHW_SetVpp)(T_PHWInfo *pInfo, int iVppLevel );
//typedef API_IMPORT(int)(*T_PHW_SetVdd)(T_PHWInfo *pInfo, int iVddLevel );
//typedef API_IMPORT(int)(*T_PHW_SetClockAndData)(T_PHWInfo *pInfo,int iClockState,int iDataState);
//typedef API_IMPORT(int)(*T_PHW_SetClockEnable)(T_PHWInfo *pInfo, int iNewState );
//typedef API_IMPORT(int)(*T_PHW_SetDataEnable)(T_PHWInfo *pInfo, int iNewState );
//typedef API_IMPORT(int)(*T_PHW_PullMclrToGnd)(T_PHWInfo *pInfo, int iNewState );
//typedef API_IMPORT(int)(*T_PHW_ConnectTarget)(T_PHWInfo *pInfo, int iNewState );
//typedef API_IMPORT(int)(*T_PHW_SetLeds)(T_PHWInfo *pInfo,int iGreenLedState,int iRedLedState);
//typedef API_IMPORT(int)(*T_PHW_GetDataInBit)( T_PHWInfo *pInfo );
//typedef API_IMPORT(int)(*T_PHW_GetOkButton)( T_PHWInfo *pInfo );
//typedef API_IMPORT(int)(*T_PHW_FlushCommand)(T_PHWInfo *pInfo, uint32_t dwCommand, uint32_t dwNumCommandBits, uint32_t dwData, uint32_t dwNumDataBits);


// Function pointers for the PIC-Hardware-Interface DLL :
//T_PHW_Init g_PHW_Init; // pointer to the PHW_Init() function, loaded from DLL
//T_PHW_Exit g_PHW_Exit; // pointer to PHW_Exit(), loaded from DLL ... etc etc
//T_PHW_OnTimer g_PHW_OnTimer;
//T_PHW_ExecCmd g_PHW_ExecCmd;
//T_PHW_SetVpp  g_PHW_SetVpp;
//T_PHW_SetVdd  g_PHW_SetVdd;
//T_PHW_SetClockAndData g_PHW_SetClockAndData;
//T_PHW_SetClockEnable  g_PHW_SetClockEnable;
//T_PHW_SetDataEnable   g_PHW_SetDataEnable;
//T_PHW_PullMclrToGnd   g_PHW_PullMclrToGnd;
//T_PHW_ConnectTarget   g_PHW_ConnectTarget;
//T_PHW_SetLeds         g_PHW_SetLeds;
//T_PHW_GetDataInBit    g_PHW_GetDataInBit;
//T_PHW_GetOkButton     g_PHW_GetOkButton;
//
//T_PHW_FlushCommand    g_PHW_FlushCommand;



/*-----------  Low-Level access for PIC Programmer routines -----------------*/

/***************************************************************************/
/*  Controls COM port output signals to the PIC programmer                 */
/***************************************************************************/
HANDLE  COM_hComPort = INVALID_HANDLE_VALUE;
DCB  COM_dcb;
#ifdef __WXMSW__
OVERLAPPED COM_sOverlappedIo = { 0,0,0,0,NULL }; // structure for OVERLAPPED I/O
#endif
uint16_t PicHw_wDataControlBits;   // for data format reg.  (reg 03)
uint16_t PicHw_wModemControlBits;  // for modem control reg (reg 04)
LONGLONG PicHw_i64LastTimeOfTxdFeed; // timestamp of last call to PicHw_UpdateComOutputBits()

bool  PicHw_fVppIsOn, PicHw_fVddIsOn, PicHw_fMclrPulledToGnd,
      PicHw_fClockIsHigh, PicHw_fDataOutIsHigh,
      PicHw_fClockIsEnabled, PicHw_fDataIsEnabled;
bool  PicHw_fTogglingTxD;

#ifdef __WXMSW__
bool COM_OpenPicPort(void)
{
    DCB MyDCB;
    COMMTIMEOUTS MyCommTimeouts;

    if( COM_hComPort != INVALID_HANDLE_VALUE )
    {
        // if a COM-port has already been opened; close it (may be different now)
        COM_ClosePicPort();
    }


// Open the COM port with a windoze API routine (!) .
    if( (COM_hComPort == INVALID_HANDLE_VALUE) )
    {
        // A process uses the CreateFile function to open a handle
        // to a communications resource.
        // For example, specifying COM1 opens a handle to a serial port,
        // and LPT1 opens a handle to a parallel port.
        // If the specified resource is currently being used
        // by another process, CreateFile fails.
        // Any thread of the process can use the handle returned
        // by CreateFile to identify the resource in any of the functions
        // that access the resource.
        // When the process uses CreateFile to open a communications
        // resource, it must specify certain values for the following
        //  parameters:
        //    The fdwShareMode parameter must be zero, opening the resource
        //                     for exclusive access.
        //    The fdwCreate parameter
        //                     must specify the OPEN_EXISTING flag.
        //    The hTemplateFile parameter must be NULL.
        // Under WinXP (NT?), it seems to be impossible to do simulaneous
        //    READ and WRITE operations without the OVERLAPPED hassle.
        // Under Win95, there were no problems !!!
        memset( &COM_sOverlappedIo, 0, sizeof(OVERLAPPED)); // structure for OVERLAPPED I/O
        COM_sOverlappedIo.hEvent = ::CreateEvent( NULL, true, false, NULL );
        COM_hComPort = CreateFile(
                           Config.sz40ComPortName,        // pointer to name of the file
                           GENERIC_READ | GENERIC_WRITE,  // access (read-write) mode
                           0,                             // share mode
                           NULL,                          // pointer to security attributes
                           OPEN_EXISTING,                 // how to create
                           FILE_FLAG_OVERLAPPED,          // file attributes..
                           NULL );                        // handle to file with attributes to copy
        if( COM_hComPort == INVALID_HANDLE_VALUE )
        {
            _tcscpy(PicHw_sz255LastError, _("Cannot open COM port"));
            return false;
        }


        // To determine the initial configuration of a serial communications
        // resource, a process calls the GetCommState function,
        // which fills in a serial port DCB structure with the current
        // configuration settings.
        MyDCB.DCBlength = sizeof(DCB);  // for compatibility checks..
        if( ! GetCommState( COM_hComPort, &MyDCB ) )
        {
            _tcscpy(PicHw_sz255LastError, _("Cannot read CommState."));
            CloseHandle(COM_hComPort);
            COM_hComPort = INVALID_HANDLE_VALUE;
            return false;
        }

        // To modify this configuration, a process specifies a DCB structure
        // in a call to the SetCommState function.
        // Members of the DCB structure specify the configuration settings
        // such as the baud rate, the number of data bits per byte,
        // and the number of stop bits per byte. Other DCB members specify special
        // characters and enable parity checking and flow control.
        // When a process needs to modify only a few of these configuration settings,
        // it should first call GetCommState to fill in a DCB structure with the
        // current configuration. Then the process can adjust the important values
        // in the DCB structure and reconfigure the device by calling SetCommState
        // and specifying the modified DCB structure.
        //       (WB: that's exactly how it's done here.)
        // This procedure ensures that the unmodified members of the DCB structure
        // contain appropriate values. For example, a common error is to configure
        // a device with a DCB structure in which the structure's XonChar member
        // is equal to the XoffChar member. Some members of the DCB structure
        // are different from those in previous versions of Microsoft Windows.
        // In particular, the flags for controlling RTS (request-to-send)
        // and DTR (data-terminal-ready) have changed.
        //  WoBu: We set almost everything here to the driver package's "default"
        //        value   so we know quite well what's going on,
        //        regardless of the WINDOZE VERSION and System Settings on this PC !
        MyDCB.BaudRate = 115200; // ex: = 9600;  // Set the baudrate for the serial port,
        // just in case the TxD line will be "toggling" to drive a charge pump.
        MyDCB.fBinary = true;             // binary mode, no EOF check
        MyDCB.fParity = false;            // true=enable parity checking
        MyDCB.fOutxCtsFlow= false;        // no CTS output flow control
        MyDCB.fOutxDsrFlow= false;        // DSR output flow control
        MyDCB.fDtrControl=DTR_CONTROL_ENABLE; // DTR flow control type: DTR_CONTROL_ENABLE=0x01="leave it on"(!!)
        MyDCB.fDsrSensitivity=false;      // DSR sensitivity
        MyDCB.fTXContinueOnXoff=false;    // XOFF continues Tx
        MyDCB.fOutX = false;              // XON/XOFF out flow control
        MyDCB.fInX  = false;              // XON/XOFF in flow control
        MyDCB.fErrorChar= false;          // enable error replacement
        MyDCB.fNull = false;              // enable null stripping (false: don't throw away NULL bytes!)
        MyDCB.fRtsControl=RTS_CONTROL_ENABLE;  // RTS flow control ..
        // RTS_CONTROL_ENABLE: Enable the RTS line when the device is opened and leave it on.
        MyDCB.fAbortOnError=false;        // abort reads/writes on error
        //  MyDCB.fDummy2=MyDCB.fDummy2;  // reserved
        //  MyDCB.wReserved=MyDCB.wReserved; // not currently used

        MyDCB.XonLim = 2048;    // transmit XON threshold (65535 geht unter WinXP nicht ?)
        MyDCB.XoffLim= 2048;    // transmit XOFF threshold
        MyDCB.ByteSize= 8;      // number of bits/byte, 4-8
        MyDCB.Parity  = 0;      // 0..4 = no,odd,even,mark,space  "parity"
        MyDCB.StopBits= 0;      // 0,1,2 = 1, 1.5, 2
        MyDCB.XonChar = 0x11;   // Tx and Rx XON character
        MyDCB.XoffChar= 0x13;   // Tx and Rx XOFF character
        MyDCB.ErrorChar=0x00;   // error replacement character
        MyDCB.EofChar = 0x00;   // end of input character
        MyDCB.EvtChar = 0x00;   // received event character
        //  MyDCB.wReserved1=MyDCB.wReserved1; // reserved; do not use

        if( ! SetCommState( COM_hComPort, &MyDCB ) )
        {
            _tcscpy(PicHw_sz255LastError, _("Cannot set CommState."));
            CloseHandle(COM_hComPort);
            COM_hComPort = INVALID_HANDLE_VALUE;
            return false;
        }

        // Now define the RX- and TX timeouts for calls to ReadFile and WriteFile.
        // See Win32 Programmer's Reference on COMMTIMEOUTS.
        MyCommTimeouts.ReadIntervalTimeout = 0;
        // Specifies the maximum time, in milliseconds,
        // allowed to elapse between the arrival of
        // two characters on the communications line.

        MyCommTimeouts.ReadTotalTimeoutMultiplier = 0;
        // Specifies the multiplier, in milliseconds,
        // used to calculate the total time-out period
        // for read operations. For each read operation,
        // this value is multiplied by the requested
        // number of bytes to be read.

        MyCommTimeouts.ReadTotalTimeoutConstant = 5; // ex: 20;
        // Specifies the constant, in milliseconds,
        // used to calculate the total time-out period
        // for read operations. For each read operation,
        // this value is added to the product of the
        // ReadTotalTimeoutMultiplier member and the
        // requested number of bytes.
        // Modified 2007-08-27 because an USB<->RS-232 interface (*)
        //  was INCREDIBLY SLOW in the EscapeCommFunction()
        // (*) "Prolific USB-Serial" alias "BELKIN, Made In China" .. uuurgh

        MyCommTimeouts.WriteTotalTimeoutMultiplier = 10; // ex: 80;
        // Specifies the multiplier, in milliseconds,
        // used to calculate the total time-out period
        // for write operations. For each write operation,
        // this value is multiplied by the number of bytes
        // to be written.
        // WB: should be somehow baudrate-dependent !

        MyCommTimeouts.WriteTotalTimeoutConstant = 100;
        // Specifies the constant, in milliseconds,
        // used to calculate the total time-out period
        // for write operations. For each write operation,
        // this value is added to the product of the
        // WriteTotalTimeoutMultiplier member
        // and the number of bytes to be written.

        // The SetCommTimeouts function sets the time-out parameters
        // for all read and write operations on a specified communications device.
        // (P.S. not really important yet, since we only "write" to the port
        //  to keep the charge-pump in the JDM-interface happy)
        if( ! SetCommTimeouts( COM_hComPort, &MyCommTimeouts ) )
        {
            _tcscpy(PicHw_sz255LastError, _("Cannot set CommTimeouts."));
            CloseHandle(COM_hComPort);
            COM_hComPort = INVALID_HANDLE_VALUE;
            return false;
        }

    } // end if < need to open and parametrize serial port ? >

    PicHw_wModemControlBits = 0x00;  // data to be written to register


    return true;
} // end COM_OpenPicPort()

bool COM_ClosePicPort(void)
{
    DCB dcb;

    if( COM_sOverlappedIo.hEvent != NULL )    // close 'Overlapped I/O' handle
    {
        CloseHandle( COM_sOverlappedIo.hEvent );
        COM_sOverlappedIo.hEvent = NULL;
    }

    if( COM_hComPort != INVALID_HANDLE_VALUE )
    {
        dcb.DCBlength = sizeof( DCB );
        GetCommState( COM_hComPort, &dcb );
        dcb.fDtrControl = DTR_CONTROL_DISABLE;
        dcb.fRtsControl = RTS_CONTROL_DISABLE;
        SetCommState( COM_hComPort, &dcb );
        CloseHandle( COM_hComPort );
        COM_hComPort = INVALID_HANDLE_VALUE;
        return true;
    }
    return false;
} // end COM_ClosePicPort()
#else
bool COM_OpenPicPort(void)
{
    wxString szPort;
    struct termios MyTermios;

//COMMTIMEOUTS MyCommTimeouts;

    if( COM_hComPort != INVALID_HANDLE_VALUE )
    {
        // if a COM-port has already been opened; close it (may be different now)
        COM_ClosePicPort();
    }

    if( (COM_hComPort == INVALID_HANDLE_VALUE) )
    {
        //    The fdwShareMode parameter must be zero, opening the resource for exclusive access.
        //    The fdwCreate parameter must specify the OPEN_EXISTING flag.
        //    The hTemplateFile parameter must be NULL.
        // Under WinXP (NT?), it seems to be impossible to do simulaneous
        //    READ and WRITE operations without the OVERLAPPED hassle.

        int fd; /* File descriptor for the port */
        szPort = wxString( Config.sz40ComPortName );

        fd = open(szPort.mb_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1)
        {
            _tcscpy(PicHw_sz255LastError, _("Cannot open COM port"));
        }
        else
            fcntl(fd, F_SETFL, 0);
        COM_hComPort = fd;

        if( tcgetattr( COM_hComPort, &MyTermios ) )
        {
            _tcscpy(PicHw_sz255LastError, _("Cannot read CommState."));
            close(COM_hComPort);
            COM_hComPort = INVALID_HANDLE_VALUE;
            return false;
        }


        // To modify this configuration, a process specifies a DCB structure
        // in a call to the SetCommState function.
        // Members of the DCB structure specify the configuration settings
        // such as the baud rate, the number of data bits per byte,
        // and the number of stop bits per byte. Other DCB members specify special
        // characters and enable parity checking and flow control.
        // When a process needs to modify only a few of these configuration settings,
        // it should first call GetCommState to fill in a DCB structure with the
        // current configuration. Then the process can adjust the important values
        // in the DCB structure and reconfigure the device by calling SetCommState
        // and specifying the modified DCB structure.
        //       (WB: that's exactly how it's done here.)
        // This procedure ensures that the unmodified members of the DCB structure
        // contain appropriate values. For example, a common error is to configure
        // a device with a DCB structure in which the structure's XonChar member
        // is equal to the XoffChar member. Some members of the DCB structure
        // are different from those in previous versions of Microsoft Windows.
        // In particular, the flags for controlling RTS (request-to-send)
        // and DTR (data-terminal-ready) have changed.
        //  WoBu: We set almost everything here to the driver package's "default"
        //        value   so we know quite well what's going on,
        //        regardless of the WINDOZE VERSION and System Settings on this PC !
        //MyDCB.BaudRate = 115200; // ex: = 9600;  // Set the baudrate for the serial port,
        cfsetispeed(&MyTermios, B115200);
        cfsetospeed(&MyTermios, B115200);
        MyTermios.c_cflag |= (CLOCAL | CREAD);

        // just in case the TxD line will be "toggling" to drive a charge pump.
        //MyDCB.fBinary = true;             // binary mode, no EOF check
        //MyDCB.fParity = false;            // true=enable parity checking
        //8N1
        MyTermios.c_cflag &= ~PARENB;
        MyTermios.c_cflag &= ~CSTOPB;
        MyTermios.c_cflag &= ~CSIZE;
        MyTermios.c_cflag |= CS8;


        //MyTermios.c_cflag &= ~CRTSCTS;
        MyTermios.c_cflag &= ~IXON;
        //	MyTermios.c_cflag &= ~IXOFF;

//     MyDCB.fOutxCtsFlow= false;        // no CTS output flow control
//     MyDCB.fOutxDsrFlow= false;        // DSR output flow control
        // MyDCB.fDtrControl=DTR_CONTROL_ENABLE; // DTR flow control type: DTR_CONTROL_ENABLE=0x01="leave it on"(!!)
        // MyDCB.fDsrSensitivity=false;      // DSR sensitivity
        //MyDCB.fTXContinueOnXoff=false;    // XOFF continues Tx
        //MyDCB.fOutX = false;              // XON/XOFF out flow control
        //MyDCB.fInX  = false;              // XON/XOFF in flow control
        //MyDCB.fErrorChar= false;          // enable error replacement
        //MyDCB.fNull = false;              // enable null stripping (false: don't throw away NULL bytes!)
        //MyDCB.fRtsControl=RTS_CONTROL_ENABLE;  // RTS flow control ..
        // RTS_CONTROL_ENABLE: Enable the RTS line when the device is opened and leave it on.
        //MyDCB.fAbortOnError=false;        // abort reads/writes on error
        //  MyDCB.fDummy2=MyDCB.fDummy2;  // reserved
        //  MyDCB.wReserved=MyDCB.wReserved; // not currently used

        //MyDCB.XonLim = 2048;    // transmit XON threshold (65535 geht unter WinXP nicht ?)
        //MyDCB.XoffLim= 2048;    // transmit XOFF threshold
        //MyDCB.ByteSize= 8;      // number of bits/byte, 4-8
        //MyDCB.Parity  = 0;      // 0..4 = no,odd,even,mark,space  "parity"
        //MyDCB.StopBits= 0;      // 0,1,2 = 1, 1.5, 2
        //MyDCB.XonChar = 0x11;   // Tx and Rx XON character
        //MyDCB.XoffChar= 0x13;   // Tx and Rx XOFF character
        //MyDCB.ErrorChar=0x00;   // error replacement character
        //MyDCB.EofChar = 0x00;   // end of input character
        //MyDCB.EvtChar = 0x00;   // received event character
        //  MyDCB.wReserved1=MyDCB.wReserved1; // reserved; do not use

        if( tcsetattr( COM_hComPort, TCSANOW, &MyTermios ) )
        {
            _tcscpy(PicHw_sz255LastError, _("Cannot set CommState."));
            close(COM_hComPort);
            COM_hComPort = INVALID_HANDLE_VALUE;
            return false;
        }
        /*
             // Now define the RX- and TX timeouts for calls to ReadFile and WriteFile.
             // See Win32 Programmer's Reference on COMMTIMEOUTS.
             MyCommTimeouts.ReadIntervalTimeout = 0;
                         // Specifies the maximum time, in milliseconds,
                         // allowed to elapse between the arrival of
                         // two characters on the communications line.

             MyCommTimeouts.ReadTotalTimeoutMultiplier = 0;
                         // Specifies the multiplier, in milliseconds,
                         // used to calculate the total time-out period
                         // for read operations. For each read operation,
                         // this value is multiplied by the requested
                         // number of bytes to be read.

             MyCommTimeouts.ReadTotalTimeoutConstant = 5; // ex: 20;
                         // Specifies the constant, in milliseconds,
                         // used to calculate the total time-out period
                         // for read operations. For each read operation,
                         // this value is added to the product of the
                         // ReadTotalTimeoutMultiplier member and the
                         // requested number of bytes.
                  // Modified 2007-08-27 because an USB<->RS-232 interface (*)
                  //  was INCREDIBLY SLOW in the EscapeCommFunction()
                  // (*) "Prolific USB-Serial" alias "BELKIN, Made In China" .. uuurgh

             MyCommTimeouts.WriteTotalTimeoutMultiplier = 10; // ex: 80;
                         // Specifies the multiplier, in milliseconds,
                         // used to calculate the total time-out period
                         // for write operations. For each write operation,
                         // this value is multiplied by the number of bytes
                         // to be written.
                         // WB: should be somehow baudrate-dependent !

             MyCommTimeouts.WriteTotalTimeoutConstant = 100;
                         // Specifies the constant, in milliseconds,
                         // used to calculate the total time-out period
                         // for write operations. For each write operation,
                         // this value is added to the product of the
                         // WriteTotalTimeoutMultiplier member
                         // and the number of bytes to be written.

             // The SetCommTimeouts function sets the time-out parameters
             // for all read and write operations on a specified communications device.
             // (P.S. not really important yet, since we only "write" to the port
             //  to keep the charge-pump in the JDM-interface happy)
             if( ! SetCommTimeouts( COM_hComPort, &MyCommTimeouts ) )
              {
                _tcscpy(PicHw_sz255LastError, _("Cannot set CommTimeouts."));
                CloseHandle(COM_hComPort);
                COM_hComPort = INVALID_HANDLE_VALUE;
                return false;
              }
        */
    } // end if < need to open and parametrize serial port ? >

    PicHw_wModemControlBits = 0x00;  // data to be written to register

    return true;
}
bool COM_ClosePicPort(void)
{
    if( COM_hComPort != INVALID_HANDLE_VALUE )
    {
        int status;
        ioctl(COM_hComPort, TIOCMGET, &status);
        status &= ~TIOCM_DTR;
        status &= ~TIOCM_RTS;
        ioctl(COM_hComPort, TIOCMSET, &status);
        close( COM_hComPort );
        COM_hComPort = INVALID_HANDLE_VALUE;
        return true;
    }
    return false;
}
#endif

uint16_t COM_GetPicDataBit(void)
{
    if( COM_hComPort != INVALID_HANDLE_VALUE )
    {
        // The GetCommModemStatus function retrieves modem control-register values.
        // If the function succeeds, the return value is nonzero.
        // If the function fails, the return value is zero.
        DWORD dwModemStatus;
        if( GetCommModemStatus( COM_hComPort, &dwModemStatus ) )
        {
            return ( dwModemStatus & MS_CTS_ON ) != 0;
        }
    }
    _tcscpy(PicHw_sz255LastError, _("Cannot read state of CTS line"));
    return 0;
} // end COM_GetPicDataBit()

bool COM_SetPicClockAndData( bool clock_high, bool data_high ) /* only for "COM84" + "JDM" */
{
    bool fResult = true;

    if( COM_hComPort != INVALID_HANDLE_VALUE )
    {
        // > The EscapeCommFunction function directs a specified
        // > communications device to perform an extended function.
        // > bool EscapeCommFunction(  HANDLE hFile,  uint32_t dwFunc )
        // > hFile  [in] Handle to the communications device.
        // >             The CreateFile function returns this handle.
        // > dwFunc [in] Extended function to be performed.
        // >             This parameter can be one of the following values.
        // > CLRBREAK Restores character transmission and places
        // >   the transmission line in a nonbreak state. The CLRBREAK
        // >   extended function code is identical to the ClearCommBreak function.
        // > CLRDTR Clears the DTR (data-terminal-ready) signal.
        // > CLRRTS Clears the RTS (request-to-send) signal.
        // > SETBREAK Suspends character transmission
        // >        and places the transmission line in a break state
        // >        until the ClearCommBreak function is called
        // >        (or EscapeCommFunction is called with the CLRBREAK extended
        // >        function code). The SETBREAK extended function code
        // >        is identical to the SetCommBreak function.
        // >        Note that this extended function does not flush data
        // >        that has not been transmitted.
        // > SETDTR  Sends the DTR (data-terminal-ready) signal.
        // > SETRTS  Sends the RTS (request-to-send) signal.
        // > SETXOFF Causes transmission to act as if an XOFF character has been received.
        // > SETXON  Causes transmission to act as if an XON character has been received.
        // Sounds too good to be true ... why doesn't the lousy help system
        // have a pointer from 'SetCommState' to 'EscapeCommFunction' ?
        if( data_high )
            fResult = EscapeCommFunction( COM_hComPort, SETDTR );  // DTR high
        else fResult = EscapeCommFunction( COM_hComPort, CLRDTR );  // DTR low
        if( clock_high )
            fResult &= EscapeCommFunction( COM_hComPort, SETRTS ); // RTS high
        else fResult &= EscapeCommFunction( COM_hComPort, CLRRTS ); // RTS low
        // Note: with certain USB<->RS-232 adapters, the "EscapeCommFunction"
        // can be INCREDIBLY SLOW, causing PIC programming to fail always .
        // In that case, forget about WinPic, or buy a PC with a real serial port !
        return fResult;
    }
    // Arrived here: cannot set CLOCK and DATA for COM84 + JDM ...
    _tcscpy(PicHw_sz255LastError, _("Cannot set CLOCK+DATA ."));
    return false;

} // end COM_SetPicClockAndData()


bool COM_SetPicVpp( bool vpp_high )
{
    bool fResult = true;

    // The only way to set a CONSTANT level on TXD is the "BREAK"-signal:

    if( COM_hComPort != INVALID_HANDLE_VALUE )
    {
        // a more compatible way to set the state of the TXD output, using Win API only,
        // may even work with USB<->RS232  :
        if( vpp_high )  // BREAK on,  continuous high pegel on TXD
            fResult = EscapeCommFunction( COM_hComPort, SETBREAK );
        else            // BREAK off,  TXD controlled by serial output, or low pegel
            fResult = EscapeCommFunction( COM_hComPort, CLRBREAK );
        return fResult;
    }
    // Arrived here: cannot switch the Vpp signal (for COM84 or JDM) .
    _tcscpy(PicHw_sz255LastError, _("Invalid COM port address"));
    return false;

} // end COM_SetPicVpp()

int PicHw_Inv3State( int iState )
{
    if( iState<0 ) return iState;   // error or "leave unchanged"
    if( iState>0 ) return 0;        // set -> clear
    else           return 1;        // clear -> set
}

// A bunch of "port bit manipulation routines" .
//  The actual WRITE of the modified value to the port MAY be done separately !
//  These functions also provide a way to READ BACK THE CURRENT OUTPUT VALUE .
// Input parameter:
//   iNewState = 1   means "set the output to a logic HIGH state for the PIC"
//   iNewState = 0   means "set the output to a logic LOW  state for the PIC"
//   iNewState < 0   means "leave output unchanged but tell me the current state"
int PicHw_SetTXD(int iNewState)  // used for Vpp control on COM84 programmer
{
    bool fResult = true;

//  if ( PicHw_fUseSmallPort && (COM_io_address>0) )
//   { if(iNewState >0) PicHw_wDataControlBits |= 0x40;  // BREAK on,  continuous high pegel on TXD
//     if(iNewState==0) PicHw_wDataControlBits &= ~0x40; // BREAK off, TXD controlled by serial output
//     return ( PicHw_wDataControlBits & 0x40) != 0;  // Check value for register[3] .
//   }
//  else // use API functions only :
    if( COM_hComPort != INVALID_HANDLE_VALUE )
    {
        // a more compatible way to set the state of the TXD output, using Win API only,
        // may even work with USB<->RS232 :
        if(iNewState>0)  // BREAK on,  continuous high pegel on TXD
        {
            PicHw_wDataControlBits |= 0x40;
            fResult = EscapeCommFunction( COM_hComPort, SETBREAK );
        }
        if(iNewState==0) // BREAK off,  TXD controlled by serial output, or low pegel
        {
            PicHw_wDataControlBits &= ~0x40;
            fResult = EscapeCommFunction( COM_hComPort, CLRBREAK );
        }
        if( fResult )
            return ( PicHw_wDataControlBits & 0x40) != 0;  // Check value for register[3] .
    }
    return -1;
}

int PicHw_SetNotTXD(int iNewState)
{
    return PicHw_Inv3State( PicHw_SetTXD( PicHw_Inv3State( iNewState ) ) ) ;
}

int PicHw_SetDTR(int iNewState)  // used for SERIAL DATA OUT on COM84 programmer
{
    bool fResult = true;

//  if ( PicHw_fUseSmallPort && (COM_io_address>0) )
//   { if(iNewState >0) PicHw_wModemControlBits |= 0x01;  // DTR high
//     if(iNewState==0) PicHw_wModemControlBits &=~0x01;  // DTR low
//     return ( PicHw_wModemControlBits & 0x01) != 0;  // Check value for register[4] .
//   }
//  else // use API functions only :
    if( COM_hComPort != INVALID_HANDLE_VALUE )
    {
        // a more compatible way to set the state of the DTR output, using Win API only,
        // may even work with USB<->RS232 :
        if(iNewState >0)
        {
            PicHw_wModemControlBits |= 0x01;  // DTR high
            fResult = EscapeCommFunction( COM_hComPort, SETDTR );
        }
        if(iNewState==0)
        {
            PicHw_wModemControlBits &=~0x01;  // DTR low
            fResult = EscapeCommFunction( COM_hComPort, CLRDTR );
        }
        if( fResult )
            return ( PicHw_wModemControlBits & 0x01) != 0;  // Check value for register[4] .
    }
    return -1;
}

int PicHw_SetNotDTR(int iNewState)  // only for custom interfaces on serial port
{
    return PicHw_Inv3State( PicHw_SetDTR( PicHw_Inv3State( iNewState ) ) ) ;
}

int PicHw_SetRTS(int iNewState)  // used for SERIAL CLOCK OUT on COM84 programmer
{
    bool fResult = true;

//  if ( PicHw_fUseSmallPort && (COM_io_address>0) )
//   { if(iNewState >0) PicHw_wModemControlBits |= 0x02;  // RTS high
//     if(iNewState==0) PicHw_wModemControlBits &=~0x02;  // RTS low
//     return ( PicHw_wModemControlBits & 0x02) != 0;  // Check value for register[4] .
//   }
//  else // use API functions only :
    if( COM_hComPort != INVALID_HANDLE_VALUE )
    {
        // a more compatible way to set the state of the RTS output,
        //    using Win API only, may even work with USB<->RS232 :
        if(iNewState >0)
        {
            PicHw_wModemControlBits |= 0x02;  // RTS high
            fResult = EscapeCommFunction( COM_hComPort, SETRTS );
        }
        if(iNewState==0)
        {
            PicHw_wModemControlBits &=~0x02;  // RTS low
            fResult = EscapeCommFunction( COM_hComPort, CLRRTS );
        }
        if( fResult )
            return ( PicHw_wModemControlBits & 0x02) != 0;  // return current state
    }
    return -1;
} // end PicHw_SetRTS()

int PicHw_SetNotRTS(int iNewState)  // only for custom interfaces on serial port
{
    return PicHw_Inv3State( PicHw_SetRTS( PicHw_Inv3State( iNewState ) ) ) ;
}

int PicHw_TestCTS(int iNewState) // test bit. ignore iNewState.
{
    // To read back the signal on the CTS input at the serial port,
    // poll the Modem Status Register (0x10 = mask for "CTS").
    // Used for serial data from PIC to PC in the "COM84" programmer.

    if( COM_hComPort != INVALID_HANDLE_VALUE )
    {
        // The GetCommModemStatus function retrieves modem control-register values.
        // If the function succeeds, the return value is nonzero.
        // If the function fails, the return value is zero.
        DWORD dwModemStatus;
        if( GetCommModemStatus( COM_hComPort, &dwModemStatus ) )
        {
            return ( dwModemStatus & MS_CTS_ON ) != 0;
        }
    }
    // Arrived here: impossible to sense the state of the CTS-line
    return -1;
} // end PicHw_TestCTS()

int PicHw_TestNotCTS(int iNewState) // inverted test bit. ignore iNewState.
{
    return PicHw_Inv3State( PicHw_TestCTS( PicHw_Inv3State( iNewState ) ) ) ;
}
#ifdef __WXMSW__
bool PicHw_UpdateComOutputBits(void)
//  Writes the modified bits back to the serial output lines.
//  Also keeps the TxD ouput toggling if required
//    (for charge pump in certain programmers) .
//  *Must* be called PERIODICALLY if the flag 'PicHw_fTogglingTxD' is set.
{

    // Update the signal levels on RTS, DTR, and TXD .
    // WARNING: These outputs may not change their state at the very same time,
    //     so DON'T RELY ON TWO CONTROL OUTPUTS CHANGING AT THE "SAME" TIME !
    {
        if( COM_hComPort != INVALID_HANDLE_VALUE )
        {
            // if only windows API routines are used,
            // most signals have been updated immediately
            // to avoid unnecessary calls of the EscapeCommFunction() here .
            if(PicHw_fTogglingTxD)
            {
                // If the state of the TxD line shall toggle (produce a square wave),
                // keep the transmit-shift-register filled with a 01010101-value .
                // Note: MUST USE NON-BLOCKING I/O HERE .
                // See help system on WriteFile(), or google for ERROR_IO_PENDING .
                char sz4Temp[4];
                DWORD dwNumBytesWritten;
                sz4Temp[0] = 0x55;
                if( WriteFile( COM_hComPort, // handle to file to write to
                               sz4Temp,   // pointer to data to write to file
                               1,         // number of bytes to write
                               &dwNumBytesWritten,  // pointer to number of bytes written
                               &COM_sOverlappedIo)==0) // pointer to structure needed for overlapped I/O
                {
                    // Arrived here: WriteFile "failed" . But not really...
                    // Since we are using "overlapped" (=non-blocking) I/O here,
                    // it's totally ok that WriteFile fails if the tx buffer is full.
                    uint32_t dwError = GetLastError();
                    if( dwError == ERROR_IO_PENDING )
                    {
                        // everything perfect. WriteFile is still busy from the previous "write",
                        // which means the charge pump in the JDM-interface is still
                        // being fed with a square wave.
                        // This code was just added for clarity, and to check if we really
                        // got that "ERROR_IO_PENDING" message !
                        dwError = dwError;   // <<< set breakpoint here

                    } // end if <WriteFile still busy from previous call >
                    else
                    {
                        // arrived here: something REALLY wrong with 'WriteFile'. What ?
                        dwError = dwError;   // <<< set breakpoint here
                    }
                } // end if <WriteFile seems to have failed (for the serial port) >
                QueryPerformanceCounter( (LARGE_INTEGER *) &PicHw_i64LastTimeOfTxdFeed );
            } // end if(PicHw_fTogglingTxD)

            return true;
        }
    }
    _tcscpy(PicHw_sz255LastError, _("Cannot update COM port bits"));
    return false;

} // end PicHw_UpdateComOutputBits()
#else
bool PicHw_UpdateComOutputBits(void)
{

    // Update the signal levels on RTS, DTR, and TXD .
    // WARNING: These outputs may not change their state at the very same time,
    //     so DON'T RELY ON TWO CONTROL OUTPUTS CHANGING AT THE "SAME" TIME !
    {
        if( COM_hComPort != INVALID_HANDLE_VALUE )
        {
            // if only windows API routines are used,
            // most signals have been updated immediately
            // to avoid unnecessary calls of the EscapeCommFunction() here .
            if(PicHw_fTogglingTxD)
            {
                // If the state of the TxD line shall toggle (produce a square wave),
                // keep the transmit-shift-register filled with a 01010101-value .
                // Note: MUST USE NON-BLOCKING I/O HERE .
                // See help system on WriteFile(), or google for ERROR_IO_PENDING .
                char sz4Temp[4];
                sz4Temp[0] = 0x55;
                if( write( COM_hComPort, // handle to file to write to
                           sz4Temp,   // pointer to data to write to file
                           1))         // number of bytes to write)
                {
                    // Arrived here: WriteFile "failed" . But not really...
                    // Since we are using "overlapped" (=non-blocking) I/O here,
                    // it's totally ok that WriteFile fails if the tx buffer is full.
                    uint32_t dwError = errno;
                    if( dwError == EAGAIN )
                    {
                        // everything perfect. WriteFile is still busy from the previous "write",
                        // which means the charge pump in the JDM-interface is still
                        // being fed with a square wave.
                        // This code was just added for clarity, and to check if we really
                        // got that "ERROR_IO_PENDING" message !
                        dwError = dwError;   // <<< set breakpoint here

                    } // end if <WriteFile still busy from previous call >
                    else
                    {
                        // arrived here: something REALLY wrong with 'WriteFile'. What ?
                        dwError = dwError;   // <<< set breakpoint here
                    }
                } // end if <WriteFile seems to have failed (for the serial port) >
                QueryPerformanceCounter( (LARGE_INTEGER *) &PicHw_i64LastTimeOfTxdFeed );
            } // end if(PicHw_fTogglingTxD)

            return true;
        }
    }
    _tcscpy(PicHw_sz255LastError, _("Cannot update COM port bits"));
    return false;
}

#endif


// Special routines for programmers on the serial port.
int PicHw_ToggleTXD(int iNewState)
// Used to produce Vpp for programers on serial port with a charge pump (like "JDM")
{
    PicHw_wDataControlBits &= ~0x40; // BREAK off, TXD will be controlled by serial output
    if(iNewState >0) PicHw_fTogglingTxD = true;
    if(iNewState==0) PicHw_fTogglingTxD = false;
    return PicHw_fTogglingTxD;
}


//----------end of COM-PORT control routines (for "COM84" programmer) -------



/***************************************************************************/
/*  Controls LPT port output signals to the PIC programmer                 */
/***************************************************************************/
// /*var*/  int LPT_iLptPortHandle = 0;
FILE* LPT_pfileLptPort = NULL;
uint16_t LPT_io_address = 0x0000;

// Bit pattern which will be sent to the CENTRONICS DATA PORT...
uint16_t PicHw_wLptDataBits;
uint16_t PicHw_wLptCtrlBits;  // .. and to the CENTRONICS CONTROL PORT


bool LPT_OpenPicPort(void)
{
    bool fResult = true;
    char szPort[16];

    if(PicHw_fLptPortOpened)
        // if a LPT-port has already been opened; close it (may be different now)
        LPT_ClosePicPort();

    switch(Config.iLptPortNr)
    {
    case 1:
        LPT_io_address = 0x0378;
        break;
    case 2:
        LPT_io_address = 0x0278;
        break;
    default:
        LPT_io_address = Config.iLptIoAddress;
        break;
    }

    if(LPT_io_address == 0)
    {
        fResult = false;
        _tcscpy(PicHw_sz255LastError, _("Illegal LPT port address"));
    }



    // Open the LPT port to prevent other applications to fool around with it,
    //  unfortunately in certain situations Windoze just doesn't care
    //  and changes some output bits.
    // Windows XP does not seem to recognize the "LPT1" as a device name at all.
    if (fResult)
    {
        int LptPortNr = Config.iLptPortNr;
        if (LptPortNr != 0)
        {
#ifdef __WXMSW__
            sprintf( szPort, "LPT%1d", LptPortNr );
#else
            sprintf( szPort, "/dev/parport%1d", LptPortNr-1 );
#endif
            if (Config.iVerboseMessages)
            {
                //-- <looks like a valid LPT-port *NUMBER* to try to occupy it>
                //-- NB: .. unfortunately this does not stop windoze from fooling around with the port !
                wxString Log;
                Log.Printf(_("Open %hs port"), szPort);
                APPL_ShowMsg( 0, Log.c_str() );
            }

            LPT_pfileLptPort = fopen( szPort, "w" );

            if(LPT_pfileLptPort==NULL)
            {
                _stprintf(PicHw_sz255LastError, _("Cannot occupy %hs: Error=%d"), szPort, errno);
                //  fResult = false;  // no... try to use the port anyway !
                if (Config.iVerboseMessages)
                    APPL_ShowMsg( 0, PicHw_sz255LastError );
            }

        }
        else
        {
            if (Config.iVerboseMessages)
            {
                wxString Log;
                Log.Printf(_("Start to use LPT@%04X port"), LPT_io_address);
                APPL_ShowMsg( 0, Log.c_str() );
            }
        }

#ifdef __WXMSW__
        HANDLE TokenHandle;
        if (!PrivilegeRequested)
        {
            if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &TokenHandle))
            {
                APPL_ShowMsg( 0, _("Failed to get the security token"));
                PrivilegeRequested = true;
                fResult = false;
            }
        }

        LUID Luid;
        if (!PrivilegeRequested)
        {
            if ( !LookupPrivilegeValue(NULL, SE_LOAD_DRIVER_NAME, &Luid ) )        // receives LUID of privilege
            {
                APPL_ShowMsg( 0, _("Failed to find Load Driver privilege ID!!!"));
                PrivilegeRequested = true;
            }
        }
        if (!PrivilegeRequested)
        {
            TOKEN_PRIVILEGES Tp;
            Tp.PrivilegeCount = 1;
            Tp.Privileges[0].Luid = Luid;
            Tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            if ( !AdjustTokenPrivileges(TokenHandle, false, &Tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL) )
            {
                APPL_ShowMsg( 0, _("Failed to get the Load Driver Privilege"));
                PrivilegeRequested = true;
            }
        }
        if (!PrivilegeRequested)
        {
            if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
            {
                APPL_ShowMsg( 0, _("The user does not have the Load Driver Privilege.\n"));
                fResult = false;
            }
        }

        if (fResult)
        {
            if (! InitOpenLibSys(&m_hOpenLibSys))
            {
                _tcscpy(PicHw_sz255LastError, _("Error Initialize WinRing0\n"));
                fResult = false;
                if (Config.iVerboseMessages)
                    APPL_ShowMsg( 0, PicHw_sz255LastError );
            }
            else
                PicHw_fLptPortOpened = true;
        }

        if (fResult && Config.iVerboseMessages)
            APPL_ShowMsg( 0, _("WinRing0 initialized") );
#else
        fResult = !ioperm(LPT_io_address,5,1);
        PicHw_fLptPortOpened = fResult;
        if (!fResult)
            APPL_ShowMsg( 0, _("Failed to get the security token"));
#endif
    }



    return fResult;
} // end LPT_OpenPicPort()


void LPT_ClosePicPort(void)
{
    if(LPT_pfileLptPort != NULL)
    {
        fclose(LPT_pfileLptPort);
        LPT_pfileLptPort = NULL;
        if (Config.iVerboseMessages)
            APPL_ShowMsg( 0, _("LPT port released") );
    }
#ifdef __WXMSW__
    DeinitOpenLibSys(&m_hOpenLibSys);
    if (Config.iVerboseMessages)
        APPL_ShowMsg( 0, _("WinRing0 de-initialized") );
#endif
    PicHw_fLptPortOpened = false;
} // end LPT_ClosePicPort()



bool PicHw_dummy(void)
{
    return true;
}


/*--------------------------------------------------------------*/
/* General hardware access routines for some "LPT programmers"  */
/*--------------------------------------------------------------*/

int PicHw_SetNotPrinterStrobe(int iNewState)
{
    // 'strobe' seems to be inverted by PC HARDWARE !
    if(iNewState >0)
    {
        PicHw_wLptCtrlBits |=  0x01;
        PicHw_UpdateLptCtrlBits();
    }
    if(iNewState==0)
    {
        PicHw_wLptCtrlBits &= ~0x01;
        PicHw_UpdateLptCtrlBits();
    }
    return ( PicHw_wLptCtrlBits & 0x01) !=/*!*/ 0;
}
int PicHw_SetPrinterStrobe(int iNewState)
{
    // 'strobe' seems to be inverted by PC HARDWARE !
    if(iNewState >0)
    {
        PicHw_wLptCtrlBits &= ~0x01;
        PicHw_UpdateLptCtrlBits();
    }
    if(iNewState==0)
    {
        PicHw_wLptCtrlBits |=  0x01;
        PicHw_UpdateLptCtrlBits();
    }
    return ( PicHw_wLptCtrlBits & 0x01) ==/*!*/ 0;
}

int  PicHw_TestPrinterAck(int iNewState) // test bit. ignore iNewState.
{
    // To read back the signal at "RB7" of the PIC,
    // poll the Centronics Control register (0x40 = mask for "ACKNOWNLEDGE")

//   if( PicHw_fUseSmallPort && (LPT_io_address>0) )
    if (PicHw_fLptPortOpened)
        return ( /*SmallPort.ReadByte*/ReadIoPortByte((uint16_t)LPT_io_address+1) & 0x40) != 0;
    else
        return -1;
}
int PicHw_TestNotPrinterAck(int iNewState) // inverted test bit. ignore iNewState.
{
    // To read back the INVERTED signal at "RB7" of the PIC,
    // poll the Centronics Control register (0x20 = mask for "PAPER OUT").
    // I hope Windoze XP stays cool
    //        if something happens on the ACKNOWLEDGE line !
//   if( PicHw_fUseSmallPort && (LPT_io_address>0) )
    if (PicHw_fLptPortOpened)
    {
        return ( /*SmallPort.ReadByte*/ReadIoPortByte((uint16_t)LPT_io_address+1) & 0x40) == 0;
    }
    else
        return -1;
}

int PicHw_TestPrinterBusy(int iNewState)
{
//   if( PicHw_fUseSmallPort && (LPT_io_address>0) )
    if (PicHw_fLptPortOpened)
        // the "hardware inversion" of PRINTER_BUSY is considered here:
        return ( /*SmallPort.ReadByte*/ReadIoPortByte((uint16_t)LPT_io_address+1) & 0x80) == 0;
    else
        return -1;
}
int PicHw_TestNotPrinterBusy(int iNewState)
{
//   if( PicHw_fUseSmallPort && (LPT_io_address>0) )
    if (PicHw_fLptPortOpened)
        // the "hardware inversion" of PRINTER_BUSY is considered here:
        return ( /*SmallPort.ReadByte*/ReadIoPortByte((uint16_t)LPT_io_address+1) & 0x80) != 0;
    else
        return -1;
}

int PicHw_SetPrinterALF(int iNewState)
{
    // the "inversion by hardware" of ALF is considered here:
    if(iNewState >0) { PicHw_wLptCtrlBits &= ~0x02; PicHw_UpdateLptCtrlBits(); }
    if(iNewState==0) { PicHw_wLptCtrlBits |=  0x02; PicHw_UpdateLptCtrlBits(); }
    return ( PicHw_wLptCtrlBits & 0x02) == 0;
}
int PicHw_SetNotPrinterALF(int iNewState)
{
    // the "inversion by hardware" of ALF is considered here:
    if(iNewState >0) { PicHw_wLptCtrlBits |=  0x02; PicHw_UpdateLptCtrlBits(); }
    if(iNewState==0) { PicHw_wLptCtrlBits &= ~0x02; PicHw_UpdateLptCtrlBits(); }
    return ( PicHw_wLptCtrlBits & 0x02) != 0;
}

int PicHw_TestPrinterError(int iNewState)
{
    if (PicHw_fLptPortOpened)
        return ( /*SmallPort.ReadByte*/ReadIoPortByte((uint16_t)LPT_io_address+1) & 0x08) != 0;
    else
        return -1;
}
int PicHw_TestNotPrinterError(int iNewState)
{
    if (PicHw_fLptPortOpened)
        return ( /*SmallPort.ReadByte*/ReadIoPortByte((uint16_t)LPT_io_address+1) & 0x08) != 0;
    else
        return -1;
}

int PicHw_SetPrinterInit(int iNewState)
{
    if(iNewState >0) { PicHw_wLptCtrlBits |=  0x04; PicHw_UpdateLptCtrlBits(); }
    if(iNewState==0) { PicHw_wLptCtrlBits &= ~0x04; PicHw_UpdateLptCtrlBits(); }
    return ( PicHw_wLptCtrlBits & 0x04) != 0;
}
int PicHw_SetNotPrinterInit(int iNewState)
{
    if(iNewState >0) { PicHw_wLptCtrlBits &= ~0x04; PicHw_UpdateLptCtrlBits(); }
    if(iNewState==0) { PicHw_wLptCtrlBits |=  0x04; PicHw_UpdateLptCtrlBits(); }
    return ( PicHw_wLptCtrlBits & 0x04) == 0;
}

int PicHw_SetPrinterSelect(int iNewState)
{
    // the "hardware inversion" of the SELECT_PRINTER output is considered here:
    if(iNewState >0) { PicHw_wLptCtrlBits &= ~0x08; PicHw_UpdateLptCtrlBits(); }
    if(iNewState==0) { PicHw_wLptCtrlBits |=  0x08; PicHw_UpdateLptCtrlBits(); }
    return ( PicHw_wLptCtrlBits & 0x08) == 0;
}
int PicHw_SetNotPrinterSelect(int iNewState)
{
    // the "hardware inversion" of the SELECT_PRINTER output is considered here:
    if(iNewState >0) { PicHw_wLptCtrlBits |=  0x08; PicHw_UpdateLptCtrlBits(); }
    if(iNewState==0) { PicHw_wLptCtrlBits &= ~0x08; PicHw_UpdateLptCtrlBits(); }
    return ( PicHw_wLptCtrlBits & 0x08) != 0;
}


int  PicHw_TestPrinterSelected(int iNewState) // test bit only. ignore iNewState.
{
    //if( PicHw_fUseSmallPort && (LPT_io_address>0) )
    if (PicHw_fLptPortOpened)
        return ( /*SmallPort.ReadByte*/ReadIoPortByte((uint16_t)LPT_io_address+1) & 0x10) != 0;
    else
        return -1;
}
int PicHw_TestNotPrinterSelected(int iNewState) // inverted test bit. ignore iNewState.
{
    // if( PicHw_fUseSmallPort && (LPT_io_address>0) )
    if (PicHw_fLptPortOpened)
        return ( /*SmallPort.ReadByte*/ReadIoPortByte((uint16_t)LPT_io_address+1) & 0x10) == 0;
    else
        return -1;
}

int  PicHw_TestPaperOut(int iNewState) // test bit. ignore iNewState.
{
    // To read the NON-INVERTED signal from "PAPER OUT"
    //    from the Centronics Control register (0x20 = mask for "PAPER OUT").
//   if( PicHw_fUseSmallPort && (LPT_io_address>0) )
    if (PicHw_fLptPortOpened)
        return ( /*SmallPort.ReadByte*/ReadIoPortByte((uint16_t)LPT_io_address+1) & 0x20) != 0;
    else
        return -1;
}

int PicHw_TestNotPaperOut(int iNewState) // inverted test bit. ignore iNewState.
{
    // To read the INVERTED signal from "PAPER OUT"
    //    from the Centronics Control register (0x20 = mask for "PAPER OUT").
    // Problem: Windoze XP fools around with the data bits
    //          if something happens on the PAPER OUT line !
//   if( PicHw_fUseSmallPort && (LPT_io_address>0) )
    if (PicHw_fLptPortOpened)
    {
        // Reading back is only possible with PIP84
        //      if the transistor which DRIVES RB7
        //   is turned off !
        return ( /*SmallPort.ReadByte*/ReadIoPortByte((uint16_t)LPT_io_address+1) & 0x20) == 0;
    }
    else
        return -1;
}


int PicHw_TestAlwaysTrue(int iNewState)  // returns a constant. ignore iNewState.
{
    return 1;
}
int PicHw_TestAlwaysFalse(int iNewState) // returns a constant. ignore iNewState.
{
    return 0;
}
int PicHw_AlwaysMinusOne(int iNewState)  // returns a constant. ignore iNewState.
{
    return -1;
}

// A bunch of "port bit manipulation routines" which in fact only act on a data latch.
//  The actual WRITE of the modified value to the port will be done separately !
//  These functions also provide a way to READ BACK THE CURRENT OUTPUT VALUE .
//    Read-back used by the interface state-display on the "INTERFACE TEST" tab,
//    when iNewState < 0  there is no change on the output but only the
//    'current logic state' is returned as numeric value 0 or 1 .
// Input parameter:
//   iNewState = 1   means "set the output to a logic HIGH state for the PIC"
//   iNewState = 0   means "set the output to a logic LOW  state for the PIC"
//   iNewState < 0   means "leave output unchanged but tell me the current state"
int PicHw_SetD0(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits |=  0x01;
    if(iNewState==0) PicHw_wLptDataBits &= ~0x01;
    return ( PicHw_wLptDataBits & 0x01) != 0;
}
int PicHw_SetNotD0(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits &= ~0x01;
    if(iNewState==0) PicHw_wLptDataBits |=  0x01;
    return ( PicHw_wLptDataBits & 0x01) == 0;
}
int PicHw_SetD1(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits |=  0x02;
    if(iNewState==0) PicHw_wLptDataBits &= ~0x02;
    return ( PicHw_wLptDataBits & 0x02) != 0;
}
int PicHw_SetNotD1(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits &= ~0x02;
    if(iNewState==0) PicHw_wLptDataBits |=  0x02;
    return ( PicHw_wLptDataBits & 0x02) == 0;
}
int PicHw_SetD2(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits |=  0x04;
    if(iNewState==0) PicHw_wLptDataBits &= ~0x04;
    return ( PicHw_wLptDataBits & 0x04) != 0;
}
int PicHw_SetNotD2(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits &= ~0x04;
    if(iNewState==0) PicHw_wLptDataBits |=  0x04;
    return ( PicHw_wLptDataBits & 0x04) == 0;
}
int PicHw_SetD3(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits |=  0x08;
    if(iNewState==0) PicHw_wLptDataBits &= ~0x08;
    return ( PicHw_wLptDataBits & 0x08) != 0;
}
int PicHw_SetNotD3(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits &= ~0x08;
    if(iNewState==0) PicHw_wLptDataBits |=  0x08;
    return ( PicHw_wLptDataBits & 0x08) == 0;
}
int PicHw_SetD4(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits |=  0x10;
    if(iNewState==0) PicHw_wLptDataBits &= ~0x10;
    return ( PicHw_wLptDataBits & 0x10) != 0;
}
int PicHw_SetNotD4(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits &= ~0x10;
    if(iNewState==0) PicHw_wLptDataBits |=  0x10;
    return ( PicHw_wLptDataBits & 0x10) == 0;
}
int PicHw_SetD5(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits |=  0x20;
    if(iNewState==0) PicHw_wLptDataBits &= ~0x20;
    return ( PicHw_wLptDataBits & 0x20) != 0;
}
int PicHw_SetNotD5(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits &= ~0x20;
    if(iNewState==0) PicHw_wLptDataBits |=  0x20;
    return ( PicHw_wLptDataBits & 0x20) == 0;
}
int PicHw_SetD6(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits |=  0x40;
    if(iNewState==0) PicHw_wLptDataBits &= ~0x40;
    return ( PicHw_wLptDataBits & 0x40) != 0;
}
int PicHw_SetNotD6(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits &= ~0x40;
    if(iNewState==0) PicHw_wLptDataBits |=  0x40;
    return ( PicHw_wLptDataBits & 0x40) == 0;
}
int PicHw_SetD7(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits |=  0x80;
    if(iNewState==0) PicHw_wLptDataBits &= ~0x80;
    return ( PicHw_wLptDataBits & 0x80) != 0;
}
int PicHw_SetNotD7(int iNewState)
{
    if(iNewState >0) PicHw_wLptDataBits &= ~0x80;
    if(iNewState==0) PicHw_wLptDataBits |=  0x80;
    return ( PicHw_wLptDataBits & 0x80) == 0;
}

int PicHw_SetDummy(int iNewState)
{
    iNewState = iNewState;  // to avoid "unused" warning from compiler
    return -1;  // something didn't work because this output has no function
}

bool PicHw_UpdateLptDataBits(void)
//  Write the modified bits back to the centronics data port.
//    Also called periodically when not programming, because
//    Windoze likes to fool around with the parallel port
//    if a printer driver is installed (or whatever...)
{
//   if( PicHw_fUseSmallPort && (LPT_io_address>0) )
    if (PicHw_fLptPortOpened)
    {
        // ex: biosprint(..)
        /*SmallPort.WriteByte*/WriteIoPortByte(LPT_io_address,PicHw_wLptDataBits);
        PicHw_iWrittenLptDataBits = PicHw_wLptDataBits;
        return true;
    }
    else
    {
        _tcscpy(PicHw_sz255LastError, _("LPT port failed"));
        return false;
    }
}

// Routine to check if Windoze fooled around with the centronics data port.
//    Called periodically when not programming.
bool PicHw_CheckLptDataBits(void)
{
//   if( PicHw_fUseSmallPort && (LPT_io_address>0) && (PicHw_iWrittenLptDataBits>=0) )
    if ((PicHw_fLptPortOpened) && (PicHw_iWrittenLptDataBits>=0))
    {
        return (/*SmallPort.ReadByte*/ReadIoPortByte(LPT_io_address) & 0x00FF)
               == (PicHw_iWrittenLptDataBits & 0x00FF);
    }
    else
    {
        return true;   // no "fooling" if there is no LPT port..
    }
} // end PicHw_CheckLptDataBits()


bool PicHw_UpdateLptCtrlBits(void)
{
// if( PicHw_fUseSmallPort && (LPT_io_address>0) )
    if (PicHw_fLptPortOpened)
    {
        /*SmallPort.WriteByte*/WriteIoPortByte(LPT_io_address+2, PicHw_wLptCtrlBits & 0x0F);
        // Don't set bit 4 in this register, its the "centronics interrupt enable bit"
        return true;
    }
    else
    {
        _tcscpy(PicHw_sz255LastError, _("LPT port failed"));
        return false;
    }
}


// A table of hardware access routines for a few different "simple"-programmers:

#define N_PIC_HW_FUNCS 11

T_PicHwFuncs PicHwFuncs[N_PIC_HW_FUNCS] =
{
    {
        PIC_INTF_TYPE_PIP84_V1  , // LPT port, used by SM6LKM, RB7 -> PAPER OUT
        // (Windoze XP fools around with the port after slopes on PAPER OUT)
        // SIGNAL INPUT vectors to hardware access routines...
        {{PicHw_TestNotPaperOut,NULL,NULL,NULL}},        // Read serial data ("RB7") from 'PAPER OUT', inverted ("not PAPER OUT")
        {{PicHw_TestNotPrinterSelected,NULL,NULL,NULL}}, // Read state of "button" on programming interface

        // SIGNAL OUTPUT vectors to hardware access routines...
        {{PicHw_SetNotD4,NULL,NULL,NULL}},    // Vpp control: on,off, inverted     (D4)
        {{PicHw_SetNotD0,NULL,NULL,NULL}},    // Vdd control: on,off, inverted     (D0)
        {{PicHw_SetD3   ,NULL,NULL,NULL}},    // Clock      : D3, not inverted
        {{PicHw_SetD2   ,NULL,NULL,NULL}},    // Data output: D2, not inverted
        {{PicHw_SetDummy,NULL,NULL,NULL}},    // no tristate control for CLOCK
        {{PicHw_SetDummy,NULL,NULL,NULL}},    // no tristate control for DATA
        1 , // LOGIC(!) state of data-output-line while reading, usually 1 so PIC can pull this line down
        {{PicHw_SetDummy,NULL,NULL,NULL}},    // PullMclrToGnd : not used here (only for AN589, D4)
        {{PicHw_SetNotD7,NULL,NULL,NULL}},    // ConnectToTarget: D7, INVERTED
        {{PicHw_SetNotD1,NULL,NULL,NULL}},    // Set Green Led (0=off, 1=on, -1=get current state)
        {{PicHw_SetDummy,NULL,NULL,NULL}},    // Set Green Led (0=off, 1=on, -1=get current state)

        {{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}}, // no selectable Vdd

        73 /* de DL4YHF (to check if nothing is missing in this struct) */
    }, // end PIC_INTF_TYPE_PIP84_V1

    {
        PIC_INTF_TYPE_PIP84_V2  , // LPT port, used by SM6LKM, RB7 -> ACKNOWLEDGE
        // (Windoze XP keeps cool if something happens on ACKNOWLEDGE)
        // SIGNAL INPUT vectors to hardware access routines...
        {{PicHw_TestNotPrinterAck,     NULL,NULL,NULL}}, // Read from RB7 (PIC->PC): ACK, inverted
        {{PicHw_TestNotPrinterSelected,NULL,NULL,NULL}}, // Read state of "button" on programming interface

        // SIGNAL OUTPUT vectors to hardware access routines...
        {{PicHw_SetNotD4,NULL,NULL,NULL}},       // Vpp control inverted     (D4)
        {{PicHw_SetNotD0,NULL,NULL,NULL}},       // Vdd control inverted     (D0)
        {{PicHw_SetD3,   NULL,NULL,NULL}},       // Clock not inverted       (D3)
        {{PicHw_SetD2,   NULL,NULL,NULL}},       // Data to RB7 not inverted (D2)
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // no tristate control for CLOCK
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // no tristate control for DATA
        1 , // LOGIC(!) state of data-output-line while reading, usually 1 so PIC can pull this line down
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // PullMclrToGnd : not used here (only for AN589, D4)
        {{PicHw_SetNotD7,NULL,NULL,NULL}},       // ConnectToTarget: D7, INVERTED
        {{PicHw_SetNotD1,NULL,NULL,NULL}},       // Set Green Led (0=off, 1=on, -1=get current state)
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // Set Green Led (0=off, 1=on, -1=get current state)
        {{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}}, // no selectable Vdd

        73 /* de DL4YHF (to check if nothing is missing in this struct) */
    }, // end PIC_INTF_TYPE_PIP84_V2

    {
        PIC_INTF_TYPE_LKM_FLASHPR_V1  , // LPT port, used by SM6LKM, RB7 -> ACKNOWLEDGE
        // (Windoze XP keeps cool if something happens on ACKNOWLEDGE)
        // SIGNAL INPUT vectors to hardware access routines...
        {{PicHw_TestNotPrinterAck,     NULL,NULL,NULL}}, // Read from RB7 (PIC->PC): ACK, inverted
        {{PicHw_TestNotPrinterSelected,NULL,NULL,NULL}}, // Read state of "button" on programming interface

        // SIGNAL OUTPUT vectors to hardware access routines...
        {{PicHw_SetNotD4,NULL,NULL,NULL}},       // Vpp control inverted     (D4)
        {{PicHw_SetD0,   NULL,NULL,NULL}},       // Vdd control not inverted (D0)
        {{PicHw_SetD3,   NULL,NULL,NULL}},       // Clock not inverted       (D3)
        {{PicHw_SetD2,   NULL,NULL,NULL}},       // Data to RB7 not inverted (D2)
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // no tristate control for CLOCK
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // no tristate control for DATA
        1 , // LOGIC(!) state of data-output-line while reading, usually 1 so PIC can pull this line down
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // PullMclrToGnd : not used here (only for AN589, D4)
        {{PicHw_SetNotD7,NULL,NULL,NULL}},       // ConnectToTarget: D7, INVERTED since 2003-01
        {{PicHw_SetNotD1,NULL,NULL,NULL}},       // Set Green Led (0=off, 1=on, -1=get current state)
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // Set Green Led (0=off, 1=on, -1=get current state)

        {{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}}, // no selectable Vdd

        73 /* de DL4YHF (to check if nothing is missing in this struct) */
    }, // end PIC_INTF_TYPE_LKM_FLASHPR_V1

    // 2004-01-27 : All TAIT-stype interfaces corrected (?) without being able to test them (!)
    {
        PIC_INTF_TYPE_TAIT_7406_4066, // parallel, by David Tait, 1st way = 7406 / 4066 :
        // SIGNAL INPUT vectors to hardware access routines...
        {{PicHw_TestNotPrinterAck, NULL,NULL,NULL}}, // read serial data from the PIC via PAPER OUT, INVERTED
        {{PicHw_SetDummy,       NULL,NULL,NULL}},  // Read state of "button" on programming interface (n/a)

        // SIGNAL OUTPUT vectors to hardware access routines...
        {{PicHw_SetNotD3,NULL,NULL,NULL}},       // Vpp control: on,off, inverted by 7406 (D3)
        {{PicHw_SetNotD2,NULL,NULL,NULL}},       // Vdd control: on,off, inverted by 7406 (D2)
        {{PicHw_SetNotD1,NULL,NULL,NULL}},       // Clock      : D1, inverted by 7406
        {{PicHw_SetNotD0,NULL,NULL,NULL}},       // Data output: D0, inverted by 7406
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // no tristate control for CLOCK
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // no tristate control for DATA
        1 , // LOGIC(!) state of data-output-line while reading, usually 1 so PIC can pull this line down
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // PullMclrToGnd : not used here (only for AN589, D4)
        {{PicHw_SetNotD7,NULL,NULL,NULL}},       // ConnectToTarget: D7, INVERTED
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // Set Green Led (0=off, 1=on, -1=get current state)
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // Set Green Led (0=off, 1=on, -1=get current state)

        {{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}}, // no selectable Vdd

        73 /* de DL4YHF (to check if nothing is missing in this struct) */
    },

    {
        PIC_INTF_TYPE_TAIT_7407_4066, // parallel, by David Tait, 2nd way = 7407 / 4066 :
        // SIGNAL INPUT vectors to hardware access routines...
        {{PicHw_TestPrinterAck,  NULL,NULL,NULL}},    // Read from RB7 (PIC->PC): Paper Out, not inverted
        {{PicHw_SetDummy, NULL,NULL,NULL}},    // Read state of "button" on programming interface (n/a)

        // SIGNAL OUTPUT vectors to hardware access routines...
        {{PicHw_SetD3,   NULL,NULL,NULL}},       // Vpp control: D3, not inverted
        {{PicHw_SetD2,   NULL,NULL,NULL}},       // Vdd control: D2, not inverted
        {{PicHw_SetD1,   NULL,NULL,NULL}},       // Clock      : D1, not inverted (7407 is a non-inverting driver)
        {{PicHw_SetD0,   NULL,NULL,NULL}},       // Data output: D0, not inverted (7407 is a non-inverting driver)
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // no tristate control for CLOCK
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // no tristate control for DATA
        1 , // LOGIC(!) state of data-output-line while reading, usually 1 so PIC can pull this line down
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // PullMclrToGnd : not used here (only for AN589, D4)
        {{PicHw_SetD7,NULL,NULL,NULL}},          // ConnectToTarget: D7, not inverted
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // Set Green Led (0=off, 1=on, -1=get current state)
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // Set Green Led (0=off, 1=on, -1=get current state)

        {{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}}, // no selectable Vdd

        73 /* de DL4YHF (to check if nothing is missing in this struct) */
    },

    {
        PIC_INTF_TYPE_TAIT_7406_PNP , // parallel, by David Tait, 3rd way = 7406 / PNP  :
        // SIGNAL INPUT vectors to hardware access routines...
        {{PicHw_TestNotPrinterAck,NULL,NULL,NULL}},      // Read from RB7 (PIC->PC): ACK, inverted
        {{PicHw_SetDummy,NULL,NULL,NULL}},        // Read state of "button" on programming interface (n/a)

        // SIGNAL OUTPUT vectors to hardware access routines...
        {{PicHw_SetD3   ,NULL,NULL,NULL}},       // Vpp control: D3, NOT inverted (actually inverted TWO times)
        {{PicHw_SetD2   ,NULL,NULL,NULL}},       // Vdd control: D2, NOT inverted (actually inverted TWO times)
        {{PicHw_SetNotD1,NULL,NULL,NULL}},       // Clock      : D1, inverted
        {{PicHw_SetNotD0,NULL,NULL,NULL}},       // Data output: D0, inverted
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // no tristate control for CLOCK
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // no tristate control for DATA
        1 , // LOGIC(!) state of data-output-line while reading, usually 1 so PIC can pull this line down
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // PullMclrToGnd : not used here (only for AN589, D4)
        {{PicHw_SetNotD7,NULL,NULL,NULL}},       // ConnectToTarget: D7, INVERTED
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // Set Green Led (0=off, 1=on, -1=get current state)
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // Set Green Led (0=off, 1=on, -1=get current state)

        {{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}}, // no selectable Vdd

        73 /* de DL4YHF (to check if nothing is missing in this struct) */
    },

    {
        PIC_INTF_TYPE_TAIT_7407_PNP , // parallel, by David Tait, 4th way = 7407 / PNP  :
        // SIGNAL INPUT vectors to hardware access routines...
        {{PicHw_TestPrinterAck, NULL,NULL,NULL}}, // Read from RB7 (PIC->PC): ACK, not inverted
        {{PicHw_SetDummy,NULL,NULL,NULL}},        // Read state of "button" on programming interface (n/a)

        // SIGNAL OUTPUT vectors to hardware access routines...
        {{PicHw_SetNotD3,NULL,NULL,NULL}},       // Vpp control: D3, inverted (once by PNP transistor)
        {{PicHw_SetNotD2,NULL,NULL,NULL}},       // Vdd control: D2, inverted (once by PNP transistor)
        {{PicHw_SetD1,   NULL,NULL,NULL}},       // Clock      : D1, not inverted (7407 is non-inverting buffer)
        {{PicHw_SetD0,   NULL,NULL,NULL}},       // Data output: D0, not inverted (7407 is non-inverting buffer)
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // no tristate control for CLOCK
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // no tristate control for DATA
        1 , // LOGIC(!) state of data-output-line while reading, usually 1 so PIC can pull this line down
        {{PicHw_SetDummy,NULL,NULL,NULL}},       // PullMclrToGnd : not used here (only for AN589, D4)
        {{PicHw_SetD7,NULL,NULL,NULL}},     // ConnectToTarget: D7 (not on original Tate interface !)
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // Set Green Led (0=off, 1=on, -1=get current state)
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // Set Green Led (0=off, 1=on, -1=get current state)

        {{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}}, // no selectable Vdd

        73 /* de DL4YHF (to check if nothing is missing in this struct) */
    },

    {
        PIC_INTF_TYPE_LPT_AN589     , // parallel, by Microchip's "AN589" = ? ? ?
        // SIGNAL INPUT vectors to hardware access routines...
        {{PicHw_TestPrinterAck,NULL,NULL,NULL}},  // read serial data from the PIC
        {{PicHw_SetDummy,NULL,NULL,NULL}}, // Read state of "button" on programming interface (n/a)

        // SIGNAL OUTPUT vectors to hardware access routines...
        {{PicHw_SetD3,   NULL,NULL,NULL}},  // Vpp
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // Vdd
        {{PicHw_SetD1,   NULL,NULL,NULL}},  // Clock (through NON-inverting buffer)
        {{PicHw_SetD0,   NULL,NULL,NULL}},  // Data  (through NON-inverting buffer)
        {{PicHw_SetNotD5,NULL,NULL,NULL}},  // tristate control for CLOCK
        {{PicHw_SetNotD2,NULL,NULL,NULL}},  // tristate control for DATA
        // (Application Note 589 uses a 74LS244 which has two INVERTING tristate
        //  control inputs; H=high-Z, L=enable )
        1 , // LOGIC(!) state of data-output-line while reading, usually 1 so PIC can pull this line down
        {{PicHw_SetD4,   NULL,NULL,NULL}},  // PullMclrToGnd : only used here for AN589, D4=H means "pull down"
        {{PicHw_SetDummy,NULL,NULL,NULL}},  //  int (*ConnectToTarget)(void);
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // Set Green Led (0=off, 1=on, -1=get current state)
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // Set Green Led (0=off, 1=on, -1=get current state)

        {{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}}, // no selectable Vdd

        73 /* de DL4YHF (to check if nothing is missing in this struct) */
    },
    {
        PIC_INTF_TYPE_LPT_NOPPP     , // parallel NOPPP
        // SIGNAL INPUT vectors to hardware access routines...
        {{PicHw_TestPrinterBusy,NULL,NULL,NULL}},  // read serial data from the PIC
        {{PicHw_SetDummy,NULL,NULL,NULL}}, // Read state of "button" on programming interface (n/a)

        // SIGNAL OUTPUT vectors to hardware access routines...
        {{PicHw_SetNotD0,NULL,NULL,NULL}},  // Vpp
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // Vdd
        {{PicHw_SetPrinterStrobe,NULL,NULL,NULL}},  // Clock
        {{PicHw_SetPrinterALF,PicHw_SetPrinterSelect,NULL,NULL}},  // Data
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // tristate control for CLOCK
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // tristate control for DATA
        1 , // LOGIC(!) state of data-output-line while reading, usually 1 so PIC can pull this line down
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // PullMclrToGnd
        {{PicHw_SetDummy,NULL,NULL,NULL}},  //  int (*ConnectToTarget)(void);
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // Set Green Led (0=off, 1=on, -1=get current state)
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // Set Green Led (0=off, 1=on, -1=get current state)

        {{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}}, // no selectable Vdd

        73 /* de DL4YHF (to check if nothing is missing in this struct) */
    },
    {
        PIC_INTF_TYPE_CUSTOM_LPT,  // DEFAULTS for a custom interface on LPT port..

        // SIGNAL INPUT vectors to hardware access routines...
        {{PicHw_TestNotPrinterAck,NULL,NULL,NULL}},     // Read from RB7 (PIC->PC): ACK, inverted
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // Read state of "button" on programming interface (n/a)

        // SIGNAL OUTPUT vectors to hardware access routines...
        {{PicHw_SetNotD4,NULL,NULL,NULL}},  // Vpp control inverted     (D4)
        {{PicHw_SetNotD0,NULL,NULL,NULL}},  // Vdd control inverted     (D0)
        {{PicHw_SetD3,   NULL,NULL,NULL}},  // Clock not inverted       (D3)
        {{PicHw_SetD2,   NULL,NULL,NULL}},  // Data to RB7 not inverted (D2)
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // no tristate control for CLOCK
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // no tristate control for DATA
        1 , // LOGIC(!) state of data-output-line while reading, usually 1 so PIC can pull this line down
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // PullMclrToGnd : not used here (only for AN589, D4)
        {{PicHw_SetNotD7,NULL,NULL,NULL}},  // ConnectToTarget: D7, INVERTED
        {{PicHw_SetNotD1,NULL,NULL,NULL}},  // Set Green Led (0=off, 1=on, -1=get current state)
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // Set Green Led (0=off, 1=on, -1=get current state)

        {{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}}, // no selectable Vdd

        73 /* de DL4YHF (to check if nothing is missing in this struct) */
    }, // end PIC_INTF_TYPE_CUSTOM_LPT
    {
        PIC_INTF_TYPE_CUSTOM_COM,  // DEFAULTS for a custom interface on serial port..

        // SIGNAL INPUT vectors to hardware access routines...
        {{PicHw_TestNotPrinterAck,NULL,NULL,NULL}}, // Read from RB7 (PIC->PC): ACK, inverted
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // Read state of "button" on programming interface (n/a)

        // SIGNAL OUTPUT vectors to hardware access routines...
        {{PicHw_SetNotD4,NULL,NULL,NULL}},  // Vpp control inverted     (D4)
        {{PicHw_SetNotD0,NULL,NULL,NULL}},  // Vdd control inverted     (D0)
        {{PicHw_SetD3,   NULL,NULL,NULL}},  // Clock not inverted       (D3)
        {{PicHw_SetD2,   NULL,NULL,NULL}},  // Data to RB7 not inverted (D2)
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // no tristate control for CLOCK
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // no tristate control for DATA
        1 , // LOGIC(!) state of data-output-line while reading, usually 1 so PIC can pull this line down
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // PullMclrToGnd : not used here (only for AN589, D4)
        {{PicHw_SetNotD7,NULL,NULL,NULL}},  // ConnectToTarget: D7, INVERTED
        {{PicHw_SetNotD1,NULL,NULL,NULL}},  // Set Green Led (0=off, 1=on, -1=get current state)
        {{PicHw_SetDummy,NULL,NULL,NULL}},  // Set Green Led (0=off, 1=on, -1=get current state)

        {{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}},{{NULL,NULL,NULL,NULL}}, // no selectable Vdd

        73 /* de DL4YHF (to check if nothing is missing in this struct) */
    } // end PIC_INTF_TYPE_CUSTOM_COM
}; // end PicHwFuncs[]

typedef   int(T_PicHwInOutFunc)(int iNewState);

T_PicHwInOutFunc *PicHw_TokenToInOutFunc(const char **ppszToken)
{
    // this routine is just needed to "assemble" the pointers. Not time-critical !

    // Signals for PARALLEL PORT interfaces  ("LPT")
    if(strncmp(*ppszToken,"!D0",3)==0) {*ppszToken+=3;   return PicHw_SetNotD0; }
    if(strncmp(*ppszToken,"!D1",3)==0) {*ppszToken+=3;   return PicHw_SetNotD1; }
    if(strncmp(*ppszToken,"!D2",3)==0) {*ppszToken+=3;   return PicHw_SetNotD2; }
    if(strncmp(*ppszToken,"!D3",3)==0) {*ppszToken+=3;   return PicHw_SetNotD3; }
    if(strncmp(*ppszToken,"!D4",3)==0) {*ppszToken+=3;   return PicHw_SetNotD4; }
    if(strncmp(*ppszToken,"!D5",3)==0) {*ppszToken+=3;   return PicHw_SetNotD5; }
    if(strncmp(*ppszToken,"!D6",3)==0) {*ppszToken+=3;   return PicHw_SetNotD6; }
    if(strncmp(*ppszToken,"!D7",3)==0) {*ppszToken+=3;   return PicHw_SetNotD7; }

    if(strncmp(*ppszToken,"D0",2)==0)  {*ppszToken+=2;   return PicHw_SetD0;    }
    if(strncmp(*ppszToken,"D1",2)==0)  {*ppszToken+=2;   return PicHw_SetD1;    }
    if(strncmp(*ppszToken,"D2",2)==0)  {*ppszToken+=2;   return PicHw_SetD2;    }
    if(strncmp(*ppszToken,"D3",2)==0)  {*ppszToken+=2;   return PicHw_SetD3;    }
    if(strncmp(*ppszToken,"D4",2)==0)  {*ppszToken+=2;   return PicHw_SetD4;    }
    if(strncmp(*ppszToken,"D5",2)==0)  {*ppszToken+=2;   return PicHw_SetD5;    }
    if(strncmp(*ppszToken,"D6",2)==0)  {*ppszToken+=2;   return PicHw_SetD6;    }
    if(strncmp(*ppszToken,"D7",2)==0)  {*ppszToken+=2;   return PicHw_SetD7;    }

    if(strncmp(*ppszToken,"!str",4)==0){*ppszToken+=4;   return PicHw_SetNotPrinterStrobe;}
    if(strncmp(*ppszToken,"!ack",4)==0){*ppszToken+=4;   return PicHw_TestNotPrinterAck;  }
    if(strncmp(*ppszToken,"!bsy",4)==0){*ppszToken+=4;   return PicHw_TestNotPrinterBusy; }
    if(strncmp(*ppszToken,"!pap",4)==0){*ppszToken+=4;   return PicHw_TestNotPaperOut;    }
    if(strncmp(*ppszToken,"!sld",4)==0){*ppszToken+=4;   return PicHw_TestNotPrinterSelected;}
    if(strncmp(*ppszToken,"!alf",4)==0){*ppszToken+=4;   return PicHw_SetNotPrinterALF;   }
    if(strncmp(*ppszToken,"!err",4)==0){*ppszToken+=4;   return PicHw_TestNotPrinterError;}
    if(strncmp(*ppszToken,"!ini",4)==0){*ppszToken+=4;   return PicHw_SetNotPrinterInit;  }
    if(strncmp(*ppszToken,"!psl",4)==0){*ppszToken+=4;   return PicHw_SetNotPrinterSelect;}

    if(strncmp(*ppszToken,"str",3)==0) {*ppszToken+=3;   return PicHw_SetPrinterStrobe;   }
    if(strncmp(*ppszToken,"ack",3)==0) {*ppszToken+=3;   return PicHw_TestPrinterAck;     }
    if(strncmp(*ppszToken,"bsy",3)==0) {*ppszToken+=3;   return PicHw_TestPrinterBusy;    }
    if(strncmp(*ppszToken,"pap",3)==0) {*ppszToken+=3;   return PicHw_TestPaperOut;       }
    if(strncmp(*ppszToken,"sld",3)==0) {*ppszToken+=3;   return PicHw_TestPrinterSelected;}
    if(strncmp(*ppszToken,"alf",3)==0) {*ppszToken+=3;   return PicHw_SetPrinterALF;      }
    if(strncmp(*ppszToken,"err",3)==0) {*ppszToken+=3;   return PicHw_TestPrinterError;   }
    if(strncmp(*ppszToken,"ini",3)==0) {*ppszToken+=3;   return PicHw_SetPrinterInit;     }
    if(strncmp(*ppszToken,"psl",3)==0) {*ppszToken+=3;   return PicHw_SetPrinterSelect;   }


    // Signals for SERIAL PORT interfaces  ("COM")
    if(strncmp(*ppszToken,"!txd",4)==0){*ppszToken+=4; return PicHw_SetNotTXD;  }
    if(strncmp(*ppszToken,"!dtr",4)==0){*ppszToken+=4; return PicHw_SetNotDTR;  }
    if(strncmp(*ppszToken,"!rts",4)==0){*ppszToken+=4; return PicHw_SetNotRTS;  }
    if(strncmp(*ppszToken,"!cts",4)==0){*ppszToken+=4; return PicHw_TestNotCTS; }

    if(strncmp(*ppszToken,"txd",3)==0) {*ppszToken+=3; return PicHw_SetTXD;     }
    if(strncmp(*ppszToken,"toggle_txd",10)==0) {*ppszToken+=10; return PicHw_ToggleTXD; }
    if(strncmp(*ppszToken,"dtr",3)==0) {*ppszToken+=3; return PicHw_SetDTR;     }
    if(strncmp(*ppszToken,"rts",3)==0) {*ppszToken+=3; return PicHw_SetRTS;     }
    if(strncmp(*ppszToken,"cts",3)==0) {*ppszToken+=3; return PicHw_TestCTS;    }

    if(strncmp(*ppszToken,"nc",2)==0)  {*ppszToken+=2; return PicHw_SetDummy;   }


    return PicHw_SetDummy;
} // end PicHw_TokenToInOutFunc()

//--------------------------------------------------------------------------
bool PicHw_TokensToInOutFuncs(const char *pszTokens,  T_InOutFuncs *pInOutFuncs)
{
    int i;
    for(i=0; i<N_IO_FUNCS; ++i)
    {
        if( (pszTokens==NULL) || (*pszTokens=='\0') )  // caution; evaluationg left to right!
            pInOutFuncs->pFunc[i] = NULL;
        else
        {
            pInOutFuncs->pFunc[i] = PicHw_TokenToInOutFunc( &pszTokens );
            if(pInOutFuncs->pFunc[i]==PicHw_SetDummy)
                pInOutFuncs->pFunc[i] = NULL;  // non-implemented functions must be NULL
            while(*pszTokens==' ') ++pszTokens;
            if( (*pszTokens==',') || (*pszTokens=='+') )
                ++pszTokens;   // separator between signal definitions
            while(*pszTokens==' ') ++pszTokens;
        }
    }
    return pInOutFuncs->pFunc[0]!=PicHw_SetDummy;
} // end PicHw_TokensToInOutFuncs()

//--------------------------------------------------------------------------
int CallIoFunc(T_InOutFuncs *pInOutFuncs,int iNewState )
{
    int iResult;

    // The first function pointer (="main function") in T_InOutFuncs must always exist.
    // It also returns the "current state" of a control signal, or -1 if not available.
    if(pInOutFuncs->pFunc[0]!=NULL)
        iResult = (*pInOutFuncs->pFunc[0])(iNewState);
    else iResult = -1;  // if there is no 'pointer to a function', don't call it !

    // The second and all further function pointers in T_InOutFuncs are optional:
#if(N_IO_FUNCS>=2)
    if(pInOutFuncs->pFunc[1]!=NULL)
        (*pInOutFuncs->pFunc[1])(iNewState);
#endif
#if(N_IO_FUNCS>=3)
    if(pInOutFuncs->pFunc[2]!=NULL)
        (*pInOutFuncs->pFunc[2])(iNewState);
#endif
#if(N_IO_FUNCS>=4)
    if(pInOutFuncs->pFunc[3]!=NULL)
        (*pInOutFuncs->pFunc[3])(iNewState);
#endif

    return iResult;   // result is always delivered by the 1st function
} // end CallIoFunc()


//--------------------------------------------------------------------------
bool PicHw_ReadCustomProgrammerDefsFromIniFile(const wxChar *pszFileName, T_PicHwFuncs *pHwFuncs)
{
// wxString section;
    wxString s;
    wxCharBuffer ss;
// bool  fResult;

    wxXmlDocument doc;
    {
        wxLogNull logNo; //-- Suppress error dialog
        if (!doc.Load(pszFileName))
            return false;
    }

    wxXmlNode *Root = doc.GetRoot();
    if (Root->GetName() != _T("programmer-control-lines"))
        return false;

    // main INPUT function, reads the state of serial data from PIC to PC
    Root->GetPropVal(_T("DataIn"), &s);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->GetInBit );
    // input function to query the programming adapter's "ok"-button :
    Root->GetPropVal(_T("OkButton"), &s);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->GetOkButton );

    // set/check Vpp, result:  1=yes 0=no -1=not supported
    s = Root->GetPropVal(_T("VppOnOff"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->SetVpp );
    // set/check Vdd, result:  1=yes 0=no -1=not supported
    s = Root->GetPropVal(_T("VddOnOff"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->SetVdd );
    // SELECT Vdd (actually done BEFORE turning Vdd on !)
    s = Root->GetPropVal(_T("SelVddLow"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->SelectVddLow );
    s = Root->GetPropVal(_T("SelVddNorm"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->SelectVddNorm );
    s = Root->GetPropVal(_T("SelVddHigh"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->SelectVddHigh );

    // Connect/disconnect programmer with target, return: 1=yes 0=no -1=not supported
    s = Root->GetPropVal(_T("Connect"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->ConnectTarget );
    // Tie MCLR to ground (like D4 on the "AN589" programmer :
    s = Root->GetPropVal(_T("PullMclrDown"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->PullMclrToGnd );
    // set/check CLOCK bit, result:  1=yes 0=no -1=not supported
    s = Root->GetPropVal(_T("ClockOut"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->SetClk );

    // set/check DATA OUTPUT bit, result:  1=yes 0=no -1=not supported
    s = Root->GetPropVal(_T("DataOut"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->SetData );
    // LOGIC(!) state of data-output-line while reading
    s = Root->GetPropVal(_T("DataOutWhileReading"), _T("1"));  // ( usually 1 so PIC can pull this line down )
    ss = s.mb_str(wxConvISO8859_1);
    s.ToLong (&PicHw_iStateOfDataOutWhileReading);

    // Test Clock Enable,    result:  1=yes(enabled) 0=no(not enabled) -1=not supported
    s = Root->GetPropVal(_T("ClkEnable"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->SetClkEnable );
    // Test Data  Enable,    result:  1=yes(enabled) 0=no(not enabled) -1=not supported
    s = Root->GetPropVal(_T("OutEnable"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->SetDataEnable );
    s = Root->GetPropVal(_T("RedLed"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->SetRedLed );
    s = Root->GetPropVal(_T("GreenLed"), wxEmptyString);
    ss = s.mb_str(wxConvISO8859_1);
    PicHw_TokensToInOutFuncs(ss, &pHwFuncs->SetGreenLed );

    bool fResult = true;

    if( pHwFuncs->GetInBit.pFunc[0] == PicHw_SetDummy )
    {
        _tcscpy(PicHw_sz255LastError, _("Invalid 'DataIn' def."));
        fResult = false;
    }
    if( pHwFuncs->SetClk.pFunc[0] == PicHw_SetDummy)
    {
        _tcscpy(PicHw_sz255LastError, _("Invalid 'ClockOut' def."));
        fResult = false;
    }
    if( pHwFuncs->SetData.pFunc[0] == PicHw_SetDummy)
    {
        _tcscpy(PicHw_sz255LastError, _("Invalid 'DataOut' def."));
        fResult = false;
    }

    return fResult;

} // end PicHw_ReadCustomProgrammerDefsFromIniFile()

////--------------------------------------------------------------------------
//void PicHw_UnloadInterfaceDLL( void )
//{
//   if ( PicHw_hFilterPluginDLL != NULL )
//    {
//     if(g_PHW_Exit!=NULL)  // call FFP_Exit() to let the plugin clean up resources:
//      { g_PHW_Exit( &PHWInfo );
//      }
//     // A well-behaving plugin should close its own control panel (if it created one),
//     // and set PHWInfo.dwhPluginWindow = 0 to indicate the window has
//     // been closed *and destroyed* .
//     // If not, we'll close and destroy the window here:
//     if( PHWInfo.dwhPluginWindow != 0 )
//      {
//        APPL_ShowMsg( 0, _("Hardware Interface plugin didn't destroy its control panel !") );
//        DestroyWindow( (HWND)PHWInfo.dwhPluginWindow );  // DANGEROUS !!
//        PHWInfo.dwhPluginWindow = 0;
//      }
//
//     // Before 'unloading' the DLL, invalidate all old function POINTERS.
//     g_PHW_Init = NULL;    // (to avoid calling an invalid function pointer)
//     g_PHW_Exit = NULL;
//     g_PHW_OnTimer = NULL;  g_PHW_ExecCmd = NULL;
//     g_PHW_SetVpp  = NULL;  g_PHW_SetVdd  = NULL;
//     g_PHW_SetClockAndData = NULL;  g_PHW_SetClockEnable = NULL;
//     g_PHW_SetDataEnable   = NULL;  g_PHW_PullMclrToGnd  = NULL;
//     g_PHW_ConnectTarget   = NULL;  g_PHW_SetLeds = NULL;
//     g_PHW_GetDataInBit    = NULL;  g_PHW_GetOkButton = NULL;
//
//     // Unload the DLL from memory :
//     FreeLibrary( PicHw_hFilterPluginDLL );
//     PicHw_hFilterPluginDLL = NULL; // forget the DLL handle, it's no longer valid
//     PHWInfo.dwhPluginDll = 0;
//     APPL_ShowMsg( 0, _("Unloaded plugin \"%s\" ."), PHWInfo.sz255PluginFileName );
//   } // end if ( PicHw_hFilterPluginDLL != NULL )
//} // end PicHw_UnloadInterfaceDLL()

////--------------------------------------------------------------------------
//bool PicHw_LoadInterfaceDLL( const wxChar *pszPluginFileName )
//{
//  int iPluginErrorCode;
//  bool fResult=false;
//
//  PicHw_UnloadInterfaceDLL();
//
//  // Fill the "PIC-Hardware-Info" before loading the plugin .
//  //   A pointer to this structure (T_PHWInfo) will be passed to all
//  //   functions implemented in that DLL, which we are going to load here .
//  PHWInfo.pInstData = NULL;  // no "extra" datablock allocated by plugin yet
//  // PHWInfo.dwhMainWindow = (uint32_t)Handle;   // save main window handle (*)
//  // PHWInfo.dwhAppInstance= (uint32_t)GetWindowLong(hwndApplMainWindow,GWL_HINSTANCE);
//  // (*) to keep this module VCL-free, the main window handle is set in
//  //     the WinPic GUI only. We cannot retrieve it from here .
//  //  For this reason, do not " memset( &PHWInfo, 0, sizeof(PHWInfo) ) " here !
//  PHWInfo.iSizeOfStruct = sizeof(T_PHWInfo); // for compatibility check
//  PHWInfo.dwhPluginWindow= 0; // handle for plugin's own window not set (yet)
//  PHWInfo.dwhPluginDll = 0;
//
//  // Try to load the new FFT Filter Plugin from a DLL (dynamically) :
//  if ( (PicHw_hFilterPluginDLL = LoadLibrary(pszPluginFileName)) != NULL )
//   {
//     // Arrived here: Library exists and could be loaded.
//     // Save the name of the plugin as info for the plugin (it may need it,
//     //  to find out if someone has RENAMED the original file name) .
//     _tcsncpy( PHWInfo.sz255PluginFileName, pszPluginFileName, 255 );
//
//     // Also save the "DLL handle", because the plugin may need to know this
//     // to access resources in its own file (see long story in PHW_framework1).
//     PHWInfo.dwhPluginDll = (uint32_t)PicHw_hFilterPluginDLL; // HMODULE returned by LoadLibrary()
//     // Get the entry points ("addresses") of all required functions.
//     // GetProcAddress returns a NULL pointer if a routine is missing.
//     // More info about GetProcAddress() from the help system:
//     // > The spelling and case of the function name pointed to by lpProcName
//     // > must be identical to that in the EXPORTS statement
//     // > of the source DLL's module-definition (.DEF) file.
//     // (but there is no DEF file, hope the compiler doesn't mangle the names)
//     g_PHW_Init   = (T_PHW_Init)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_Init" );  // mandatory !
//     g_PHW_Exit   = (T_PHW_Exit)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_Exit" );  // mandatory !
//     g_PHW_OnTimer= (T_PHW_OnTimer)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_OnTimer" );
//     g_PHW_ExecCmd= (T_PHW_ExecCmd)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_ExecCmd" );
//     g_PHW_SetVpp = (T_PHW_SetVpp)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_SetVpp" );
//     g_PHW_SetVdd = (T_PHW_SetVdd)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_SetVdd" );
//     g_PHW_SetClockAndData=(T_PHW_SetClockAndData)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_SetClockAndData" );
//     g_PHW_SetClockEnable=(T_PHW_SetClockEnable)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_SetClockEnable" );
//     g_PHW_SetDataEnable= (T_PHW_SetDataEnable)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_SetDataEnable" );
//     g_PHW_PullMclrToGnd= (T_PHW_PullMclrToGnd)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_PullMclrToGnd" );
//     g_PHW_ConnectTarget= (T_PHW_ConnectTarget)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_ConnectTarget" );
//     g_PHW_SetLeds      = (T_PHW_SetLeds)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_SetLeds" );
//     g_PHW_GetDataInBit = (T_PHW_GetDataInBit)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_GetDataInBit" );
//     g_PHW_GetOkButton  = (T_PHW_GetOkButton)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_GetOkButton" );
//     g_PHW_FlushCommand = (T_PHW_FlushCommand)GetProcAddress(PicHw_hFilterPluginDLL,"PHW_FlushCommand" );
//
//     // Are the "MANDATORY" functions present in the DLL ?
//     if(   (g_PHW_Init!=NULL) && (g_PHW_Exit!=NULL) )
//      { // Immediately call PHW_Init() so the plugin can tell us something about itself.
//        iPluginErrorCode = g_PHW_Init( &PHWInfo );
//        if( iPluginErrorCode!=PHW_ERROR_ALL_OK)
//         {   // something went wrong in PHW_Init(), most likely version incompatible..
//          switch( iPluginErrorCode )
//           { case PHW_ERROR_VERSION_INCOMPATIBLE :
//                _tcscpy( PHWInfo.sz255ErrorText, _("PLUGIN VERSION INCOMPATIBLE !"));
//                break;
//             default:
//                _stprintf( PHWInfo.sz255ErrorText, _("Error code %d in PHW_Init"), (int)iPluginErrorCode);
//                break;
//           } // end switch
//         }
//        else // "all ok":
//         { fResult = true;
//         }
//      }
//     else // something fishy with this DLL, most likely not an FFT-filter-plugin:
//      { PicHw_UnloadInterfaceDLL();
//        _tcscpy(PHWInfo.sz255ErrorText, _("Missing function in filter plugin"));
//      }
//   } // end if <LoadLibrary successful>
//  return fResult;
//} // end PicHw_LoadInterfaceDLL()

////--------------------------------------------------------------------------
//void PicHw_LetInterfaceDLLDoGraphicStuff(void)
//  // Call this periodically, whenever there is sufficient time left.
//  //     Calls the plugin's "OnTimer"-function periodically,
//  //     every 200 ms (self-synchronising) .
//{
//  static bool fAlreadyHere = false;
//  static LONGLONG i64LastCallTime = 0;
//  LONGLONG t_now, freq;
//
//  if( ! fAlreadyHere )
//   { fAlreadyHere = true;  // for what it's worth, prevent recursion
//     QueryPerformanceCounter( (LARGE_INTEGER *) &t_now );
//     QueryPerformanceFrequency( (LARGE_INTEGER *) &freq );
//     if(  t_now > (i64LastCallTime + (freq/5) ) )
//      { i64LastCallTime = t_now;
//        if ( (PicHw_hFilterPluginDLL!= NULL) && (g_PHW_OnTimer!=NULL) )
//         { g_PHW_OnTimer( &PHWInfo );
//         }
//      }
//     fAlreadyHere = false;
//   }
//} // end PicHw_LetInterfaceDLLDoGraphicStuff()


//--------------------------------------------------------------------------
bool PicHw_SelectProgrammerHardware(
    int iInterfaceType,   // for "built-in" programmer support
    const wxChar *pszSupportFileName ) // filename with definitions for a 'custom' interface
// Before calling this routine, the serial or parallel port
// must have been opened already !
{
    int i;
    wxString s;

    PicHw_wLptDataBits=0xFF; // it's a good idea to set this to a defined state ;-)
    PicHw_wLptCtrlBits=0x0F; // never set bit 4 in this register (=printer interrupt enable)
    // (will be overwritten with a "safe" value in PIC_HW_SetInterfaceType)
    PicHw_FuncPtr = NULL;
    PicHw_iStateOfDataOutWhileReading = 1;  // For most interfaces, the DATA OUT line
    // must be set "HIGH" when reading, the PIC will pull it low if the bit is a logic ZERO .

    for(i=0; i<N_PIC_HW_FUNCS; ++i)
    {
        // Check if the array of predefined "hardware access function pointers"
        //  is properly initialized. COMPILER WILL NOT NOTICE IF A LINE IN THAT ARRAY
        //  HAS BEEN DELETED BY ACCIDENT - TRY YOURSELF BY REMOVING THE "73"-INITIALIZER:
        //  NO ERROR MESSAGE DURING COMPILATION, so check this table during runtime !
        if (PicHwFuncs[i].iTest73 != 73)
        {
            APPL_ShowMsg( 0, _("Error in PicHwFuncs[%d], missing initializer ?"), i );
            break; //-- If a shift occured in the table, the remaining is likely to be shifted too. No need to check.
        }

        if (PicHwFuncs[i].iPicIntfType == iInterfaceType)
        {
            PicHw_FuncPtr = &PicHwFuncs[i];
            PicHw_iStateOfDataOutWhileReading = PicHwFuncs[i].iStateOfDataOutWhileReading;
            // break;  // 2004-11-07  removed, to check the whole list for errors !
        }
    }

//    wxFileName DllFilename (wxStandardPaths::Get().GetExecutablePath());
//    DllFilename.AppendDir(_T("interfaces"));
//    DllFilename.SetFullName(pszSupportFileName);
//    s = DllFilename.GetFullPath();

    if( (  (iInterfaceType == PIC_INTF_TYPE_CUSTOM_LPT)
            ||(iInterfaceType == PIC_INTF_TYPE_CUSTOM_COM)
        ) && (PicHw_FuncPtr != NULL) )
    {
        // For simple "custom" interfaces,
        //   all required definitions will be read from an INI-file.
        if(! PicHw_ReadCustomProgrammerDefsFromIniFile( pszSupportFileName/*s.c_str()*/,  PicHw_FuncPtr/*points to RAM!!*/ ) )
        {
            if(PicHw_sz255LastError[0]==0)
                _stprintf(PicHw_sz255LastError, _("Cannot read definition file \"%s\"."), s.c_str() );
            return false;
        }
    }  // end if(iInterfaceType == PIC_INTF_TYPE_CUSTOM_xxx)

//  if( iInterfaceType == PIC_INTF_TYPE_PLUGIN_DLL )
//   { // an "exotic" interface, in the form of a plugin-DLL :
//     // In this case, we don't use the fixed functions pointers,
//     // but a dynamically loaded DLL :
//     if(    (_tcscmp( s.c_str(), PHWInfo.sz255PluginFileName)==0)
//         && (PicHw_hFilterPluginDLL != NULL ) )
//      { // hardware-interface-DLL (plugin) already loaded, no need to load again
//        return true;
//      }
//     if(! PicHw_LoadInterfaceDLL( s.c_str() ) )
//      {
//        if(PicHw_sz255LastError[0]==0)
//         { _stprintf(PicHw_sz255LastError, _("Cannot load interface-DLL (%s)"), PHWInfo.sz255ErrorText );
//         }
//        return false;
//      }
//     else
//      { APPL_ShowMsg( 0, _("Loaded interface driver %s ."), s.c_str() );
//        return true;
//      }
//   } // end if( iInterfaceType == PIC_INTF_TYPE_PLUGIN_DLL )

    if(  (iInterfaceType == PIC_INTF_TYPE_COM84)
            ||(iInterfaceType == PIC_INTF_TYPE_JDM2 )
            ||(iInterfaceType == PIC_INTF_TYPE_CUSTOM_COM) )
    {
        // a "simple" interface on the serial port, must use direct I/O port access..
        return true;
    }
    else  // not a 'simple' interface on the serial port, but on the PARALLEL port...
    {
        if ((PicHw_fLptPortOpened) && (PicHw_FuncPtr!=NULL))
//  if( PicHw_fUseSmallPort && (PicHw_FuncPtr!=NULL) && (LPT_io_address>0) )
        {
            /*SmallPort.WriteByte*/WriteIoPortByte(LPT_io_address+2,0x0F); /* clear high nibble of LPT control reg */
            return true;
        }
        else
        {
            return false;
        }
    }
} // end PicHw_SelectProgrammerHardware()



//----end of LPT-PORT control routines (for "PIP84" and "Tate" programmer) -----



//---------------------------------------------------------------------------

/***************************************************************************/

/***************************************************************************/
bool PIC_HW_SetInterfaceType( int new_interface_type )
// Switches to an other type of PIC-programmer interface.
{
    bool fResult;

    PIC_HW_interface.type = new_interface_type;

    switch(new_interface_type)
    {
        // handle any interface-type depending "specialities" here....
    case PIC_INTF_TYPE_COM84:  // a widely used "very simple" interface
    case PIC_INTF_TYPE_JDM2 :  // similar to COM84, but tricky way to eliminate external power supply
    case PIC_INTF_TYPE_CUSTOM_COM:     // customized interface on the serial port
        // This ugly programmer switches Vdd together with Vpp,
        // and it takes a few milliseconds to charge a 10uF-Cap !
        PIC_HW_interface.wSeparateVddAndVppControl = 0;  // 0=no, 1=yes
        PIC_HW_interface.vpp_on_delay_us = 200000;  // 0.2sec should be ok
        LPT_ClosePicPort();  //-- We are looking to use the COM port, LPT can be freed (il open)
        if(! COM_OpenPicPort() )
        {
            return false;
        }
        fResult = PicHw_SelectProgrammerHardware(
                      new_interface_type, Config.sz255InterfaceSupportFile );
        return fResult;

    case PIC_INTF_TYPE_PIP84_V1       : // parallel, by Johan Bodin (SM6LKM)
    case PIC_INTF_TYPE_PIP84_V2       : // parallel, by Johan Bodin (SM6LKM), mod by DL4YHF
    case PIC_INTF_TYPE_LKM_FLASHPR_V1 : // parallel, by Johan Bodin (SM6LKM), cct 2002-09-29 (YYYY-MM-DD)
    case PIC_INTF_TYPE_TAIT_7406_4066 : // parallel, by David Tait, 1st way
    case PIC_INTF_TYPE_TAIT_7407_4066 : // parallel, by David Tait, 2nd way
    case PIC_INTF_TYPE_TAIT_7406_PNP  : // parallel, by David Tait, 3rd way
    case PIC_INTF_TYPE_TAIT_7407_PNP  : // parallel, by David Tait, 4th way
    case PIC_INTF_TYPE_LPT_AN589      : // parallel, from Microchip's "AN589"
    case PIC_INTF_TYPE_LPT_NOPPP      : // parallel, NOPPP
    case PIC_INTF_TYPE_CUSTOM_LPT     : // "custom" interface, usually on parallel port
        // These programmers have separate control outputs
        // for Vdd and Vpp, so just a few milliseconds required...
        PIC_HW_interface.wSeparateVddAndVppControl = 1;  // 0=no, 1=yes
        PIC_HW_interface.vpp_on_delay_us = 100000;  // 0.1sec should be ok
        COM_ClosePicPort();  //-- We are looking to use the LPT port, COM can be freed (il open)
        if( !LPT_OpenPicPort() )
        {
            return false;   // LastError already set !
        }
        fResult = PicHw_SelectProgrammerHardware(
                      new_interface_type, Config.sz255InterfaceSupportFile );
        if( (fResult==true) && (PicHw_FuncPtr!=NULL) )
        {
            PicHw_wLptDataBits=0xFF; // set a "safe" value for all LPT data bits...
            PicHw_wLptCtrlBits=0x0F; // never set bit 4 in this register (=printer interrupt enable)
            CallIoFunc(&PicHw_FuncPtr->ConnectTarget,0);
            CallIoFunc(&PicHw_FuncPtr->SetVpp, 0);
            CallIoFunc(&PicHw_FuncPtr->SetVdd, 0);
            CallIoFunc(&PicHw_FuncPtr->SetClk, 0);
            CallIoFunc(&PicHw_FuncPtr->SetData, 0);
            CallIoFunc(&PicHw_FuncPtr->SetClkEnable, 0);
            CallIoFunc(&PicHw_FuncPtr->SetDataEnable, 0);
            CallIoFunc(&PicHw_FuncPtr->PullMclrToGnd, 0);
            CallIoFunc(&PicHw_FuncPtr->SetRedLed, 0);
            CallIoFunc(&PicHw_FuncPtr->SetGreenLed, 0);
            PicHw_UpdateLptCtrlBits();
            fResult = PicHw_UpdateLptDataBits();  // set all safe bits from the LATCHES in one cycle
            PIC_HW_DisconnectFromTarget();
        }
        return fResult;

//    case PIC_INTF_TYPE_PLUGIN_DLL  : // hardware access functions loaded from a plugin-DLL .
//         return PicHw_SelectProgrammerHardware( new_interface_type, Config.sz80InterfacePluginDLL/*!*/ );
//
    default:  // all other interfaces are not supported yet...
        _tcscpy(PicHw_sz255LastError, _("Interface not supported"));
        return false;
    }
} // end ..SetInterfaceType()

uint32_t PIC_HW_dwCount500ns = 0;
uint32_t PIC_HW_dwCount50ns = 0;

//#ifdef USE_PORTTALK  /* AllowIoWrapper.cpp not included for copyright reasons ? */
// extern "C" void asm_TenNops(void);  // located in inoutport.asm , assemble with NASM .
//#endif

//-----------------------------------------------------------------------
void PIC_HW_ShortDelay(uint32_t dwLoops)
{
    while(dwLoops--)
    {
//#ifdef USE_PORTTALK  // AllowIoWrapper.cpp + inoutport.asm included ?
//      asm_TenNops(); // eat a few nanoseconds (not more)
//#else
        ;
//#endif
    }
}

//-----------------------------------------------------------------------
void PIC_HW_Delay_50ns(void)
{
    // required by PIC18Fxxxx programming routines,
    //  and possibly for dsPIC30Fxxxx too .
    // CAUTION: Are you sure your 3-meter-long parallel port cable
    //    is good enough for 3 * 20 MHz pass (1/ 50ns  + harmonics)  ?
    //    The author's "JDM 2" required an additional delay of at least
    //    ONE MICROSECONDS for every serial clock-pulse to work properly,
    //    and an additional delay before sampling the data-in line
    //    when reading a dsPIC .
    // ( This is taken into account inside this delay, since 2005-11-07 :
    //   In fact, it waits AT LEAST 50 NANOSECONDS PLUS AN ADDITIONAL NUMBER
    //   OF MICROSECONDS required by the interface: Config.iExtraClkDelay_us )
    uint32_t dwAdditionalDelayLoops = 0;
    if( Config.iExtraClkDelay_us > 0 )
    {
        dwAdditionalDelayLoops += 20 * (uint32_t)Config.iExtraClkDelay_us * PIC_HW_dwCount50ns ;
    }

    // To trace problems with "very slow" interfaces :
    if( Config.iSlowInterface )
    {
        // add another 5 microseconds for this "Very Slow Mode" :
        dwAdditionalDelayLoops += 100 * PIC_HW_dwCount50ns ;
    }

    // Note: Most interfaces (especially the SERIAL) have a signal rise time
    // of ~250ns..500ns at the end of the cable, so 50ns-pulses are just an illusion .
    // But not a big issue since the introduction of Config.iExtraClkDelay_us ...
    PIC_HW_ShortDelay( PIC_HW_dwCount50ns + dwAdditionalDelayLoops );

} // end PIC_HW_Delay_50ns()


//-----------------------------------------------------------------------
void PIC_HW_Delay_500ns(void)
// required by PIC18Fxxxx programming routines,
//  and possibly for dsPIC30Fxxxx too .
// Since 2005-11-07, additional delays may be included here
//  ( similar as PIC_HW_Delay_50ns) because for some interfaces,
//    even 500 nanoseconds are too short ! )
{
    uint32_t dwAdditionalDelayLoops = 0;
    if( Config.iExtraClkDelay_us > 0 )
    {
        dwAdditionalDelayLoops += 20 * (uint32_t)Config.iExtraClkDelay_us * PIC_HW_dwCount50ns ;
    }

    // To trace problems with "very slow" interfaces :
    if( Config.iSlowInterface )
    {
        // add another 5 microseconds for this "Very Slow Mode" :
        dwAdditionalDelayLoops += 100 * PIC_HW_dwCount50ns ;
    }

    PIC_HW_ShortDelay( PIC_HW_dwCount500ns + dwAdditionalDelayLoops );
} // end PIC_HW_Delay_500ns()


void PIC_HW_SingleTimingLoopCalibration(void)
{
//  uint32_t y;
    uint32_t count = 0x01000000;      // if "Ten NOPs" last 3ns, calibration takes 50ms .
    // On a 1.7 GHz-P4, calibration actually took 0.14 seconds,
    //  o this should work even on a 10-GHz-machine ;-)
    uint32_t dwStartOfOperation, dwEndOfOperation;
    dwStartOfOperation = GetTickCount(); // number of milliseconds since Windows was started
    PIC_HW_ShortDelay( count );
    dwEndOfOperation = GetTickCount();

    double duration = (dwEndOfOperation - dwStartOfOperation) / 1000.0;

    // count takes duration seconds
    // count / duration takes 1 seconds
    // 5.0e-7 * count / duration takes 500 ns seconds
    PIC_HW_dwCount500ns = (long)(5.0e-7 * count / duration);
    // On a 1.7 GHz-P4 :  count500ns = 59
    PIC_HW_dwCount50ns = (PIC_HW_dwCount500ns+5) / 10;

#ifdef DEBUG_TIMING
    APPL_ShowMsg( 0, "TimingLoopTest took %.3f sec\n", duration);
    APPL_ShowMsg( 0, "Count for 500 ns = %d\n", count500ns);
#endif
} // end PIC_HW_SingleTimingLoopCalibration()


//-----------------------------------------------------------------------
void PicHw_FeedChargePump(void)  // required for "JDM 2" and others. Call periodically !
{
    LONGLONG t1,freq;
    QueryPerformanceCounter( (LARGE_INTEGER *) &t1 );
    QueryPerformanceFrequency( (LARGE_INTEGER *) &freq );
    // Really time to feed the charge-pump ?
    if(  (PicHw_fTogglingTxD)
            && (COM_hComPort != INVALID_HANDLE_VALUE)
            && (t1 > (PicHw_i64LastTimeOfTxdFeed + freq/20) )
      )
    {
        // required to produce Vpp with a charge pump (JDM)
        PicHw_UpdateComOutputBits();
    }
}



//-----------------------------------------------------------------------
void PIC_HW_Delay_us(int microseconds)
{
    static int ran_wild_before=0;
    LONGLONG t1,t2,freq;  // don't ask the author for the difference between types
    // "LONGLONG" & "LARGE_INTEGER" and why these must both be used here
    LONGLONG emgcy_tmo = (LONGLONG)10000 * (LONGLONG)(microseconds+10);
    // Win32 programmer's help warns that some PCs do not support the
    // high-resolution "PerformanceCounter" so prevent lockup by this.
    // Don't fear overflow .. 64 bits are a lot for an INTEGER value.
    // 10000 steps per microsecond are enough for a 100GHz-CPU.
    if (microseconds<=0) return;
    QueryPerformanceCounter( (LARGE_INTEGER *) &t1 );
    QueryPerformanceFrequency( (LARGE_INTEGER *) &freq );
    if(freq>0)
        t2 = t1 + ( freq * (LONGLONG)microseconds + 999999) / 1000000;
    else
    {
        t2 = t1 + 10 * microseconds;
        ++ran_wild_before;
        APPL_ShowMsg( 0, _("WARNING: \"PERFORMANCE COUNTER\" does not work !!"));
    }
    if( microseconds >= 500 )
    {
        PicHw_FeedChargePump();  // required to produce Vpp with a charge pump (JDM)
    }
    do
    {
        QueryPerformanceCounter( (LARGE_INTEGER *) &t1 );
    }
    while ( (emgcy_tmo-- > 0) && (t1<=t2) );
    if( (emgcy_tmo <= 0) && (ran_wild_before<2) )
    {
        ++ran_wild_before;
        APPL_ShowMsg( 0, _("WARNING: PROGRAMMING TIMER RUNNING WILD !"));
    }

} // end ..Delay_us()


//-----------------------------------------------------------------------
void PIC_HW_LongDelay_ms(int milliseconds)
{
    LONGLONG t1,t2,freq;  // don't ask the author for the difference between types
    LONGLONG tNextSleep;  // "LONGLONG" & "LARGE_INTEGER" and why these must both be used here
    LONGLONG emgcy_tmo = (LONGLONG)10000 * (LONGLONG)(milliseconds+10);

    if (milliseconds<=0) return;
//  PicHw_LetInterfaceDLLDoGraphicStuff();  // keep the plugin's GUI alive
    if( PicHw_fTogglingTxD )
    {
        // If the TxD signal shall keep TOGGLING, cannot simply "Sleep" for N milliseconds,
        // instead must feed the RS232 transmit buffer (-> square wave for the "Vpp" charge pump)
        QueryPerformanceCounter( (LARGE_INTEGER *) &t1 );
        QueryPerformanceFrequency( (LARGE_INTEGER *) &freq );
        if( freq<10000 ) freq=10000;
        t2 = t1 + ( freq * (LONGLONG)milliseconds + 999) / 1000;
        tNextSleep = t1 + (freq * 30 + 999) / 1000;
        do
        {
            PicHw_FeedChargePump(); // required to produce Vpp with a charge pump
            if(t1>tNextSleep)
            {
                wxMilliSleep(10);           // ideally "Sleep" for 10ms every 30ms (-> 66% CPU load)
                tNextSleep = t1 + (freq * 30 + 999) / 1000;
            }
            // Caution: Sleep(1) worked under Win98, but fails under XP !
            //  (the min "Sleep"-time under XP seems to be 10 ms).
            // We cannot use the Win API to send a string of bytes to the serial port here,
            // (because of the direct access to the other UART registers)
            // so the UART's transmit buffer register must be fed one by one !
            // But if a thread occupies the CPU for too long, the scheduler
            // will take it away less politely - and we stare into the tube..
            // A cure could be feeding the UART's transmit buffer in a different thread,
            // but then, a critical section would be required to lock the SmallPort
            // access driver against simultaneous calls from multiple threads.
            // (would it be worth the effort ? Guess no...)
            QueryPerformanceCounter( (LARGE_INTEGER *) &t1 );
        }
        while ( (emgcy_tmo-- > 0) && (t1<=t2) );
    }
    else  // no need to TOGGLE any pin while waiting -> give the CPU to another thread !
    {
        wxMilliSleep(milliseconds);
    }
} // end PIC_HW_LongDelay_ms()


//-----------------------------------------------------------------------
bool PIC_HW_PullMclrToGnd(bool force_low)
{
    PicHw_fMclrPulledToGnd = force_low;
    switch(PIC_HW_interface.type)
    {
    case PIC_INTF_TYPE_COM84:
    case PIC_INTF_TYPE_JDM2 :
        PicHw_UpdateComOutputBits();
        return true;  // ignore, but no error

    case PIC_INTF_TYPE_CUSTOM_COM:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the serial port...
        {
            CallIoFunc(&PicHw_FuncPtr->PullMclrToGnd, force_low);
            return PicHw_UpdateComOutputBits();
        }
        return false;

//    case PIC_INTF_TYPE_PLUGIN_DLL: // hardware driven through a "plugin" (DLL):
//       if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_PullMclrToGnd!=NULL) )
//        { // Only call this DLL function if it has been successfully loaded :
//          return ( (*g_PHW_PullMclrToGnd)(&PHWInfo, force_low)  >= 0);
//        }
//       return false; // end case PIC_INTF_TYPE_PLUGIN_DLL
//
    default:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        {
            CallIoFunc(&PicHw_FuncPtr->PullMclrToGnd, force_low);
            return PicHw_UpdateLptDataBits();
        }
        return false;
    }
} // end PIC_HW_PullMclrToGnd()

bool PIC_HW_SetVpp( bool vpp_high )
{
    PicHw_fVppIsOn = vpp_high;
    switch(PIC_HW_interface.type)
    {
    case PIC_INTF_TYPE_COM84:
    case PIC_INTF_TYPE_JDM2 :
        return COM_SetPicVpp( vpp_high );

    case PIC_INTF_TYPE_CUSTOM_COM:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the serial port...
        {
            CallIoFunc(&PicHw_FuncPtr->SetVpp, vpp_high);
            return PicHw_UpdateComOutputBits();
        }
        return false;

//    case PIC_INTF_TYPE_PLUGIN_DLL: // hardware driven through a "plugin" (DLL):
//       if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_SetVpp!=NULL) )
//        { // Only call this DLL function if it has been successfully loaded :
//          return ( (*g_PHW_SetVpp)(&PHWInfo, vpp_high)  >= 0);
//        }
//       return false; // end case PIC_INTF_TYPE_PLUGIN_DLL
//
    default:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        {
            CallIoFunc(&PicHw_FuncPtr->SetVpp, vpp_high);
            return PicHw_UpdateLptDataBits();
        }
        return false;
    }
} // end ..SetPicVpp()

//------------------------------------------------------------------------
bool PIC_HW_SetVdd( bool vdd_on )
{
    PicHw_fVddIsOn = vdd_on;
    switch(PIC_HW_interface.type)
    {
    case PIC_INTF_TYPE_COM84:
    case PIC_INTF_TYPE_JDM2 :
        return false; // switching Vdd not supported (COM port has too few output lines)

    case PIC_INTF_TYPE_CUSTOM_COM:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the serial port...
        {
            CallIoFunc(&PicHw_FuncPtr->SetVdd, vdd_on);
            return PicHw_UpdateComOutputBits();
        }
        return false;

//    case PIC_INTF_TYPE_PLUGIN_DLL: // hardware driven through a "plugin" (DLL):
//       if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_SetVdd!=NULL) )
//        { // Only call this DLL function if it has been successfully loaded :
//          return ( (*g_PHW_SetVdd)(&PHWInfo, vdd_on)  >= 0);
//        }
//       return false; // end case PIC_INTF_TYPE_PLUGIN_DLL
//
    default:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        {
            CallIoFunc(&PicHw_FuncPtr->SetVdd, vdd_on);
            return PicHw_UpdateLptDataBits();
        }
        return false;
    }
} // end ..SetPicVdd()

//------------------------------------------------------------------------
bool PIC_HW_SetVddAndVpp(bool vdd_on, bool vpp_on)
// switch PIC's supply voltage AND programming voltage (simultaneously) .
// Added 2005-05-18 only to see if this would make entering prog mode
//       more reliable with SM6LKM's "PIC Flash Programmer" .
//       Result: NEGATIVE (see PIC10F_PRG.CPP) ! But kept this routine..
// Note: Only a few interfaces can really switch Vdd *and* Vpp
//       *really* simultaneously !
{
    bool fResult;
    PicHw_fVddIsOn = vdd_on;
    PicHw_fVppIsOn = vpp_on;
    switch(PIC_HW_interface.type)
    {
    case PIC_INTF_TYPE_LKM_FLASHPR_V1 : // parallel "Flash PIC programmer" by SM6LKM, 2002-09-29
    case PIC_INTF_TYPE_PIP84_V1       : // .. and other interfaces on LPT ...
    case PIC_INTF_TYPE_PIP84_V2       :
    case PIC_INTF_TYPE_TAIT_7406_4066 :
    case PIC_INTF_TYPE_TAIT_7407_4066 :
    case PIC_INTF_TYPE_TAIT_7406_PNP  :
    case PIC_INTF_TYPE_TAIT_7407_PNP  :
    case PIC_INTF_TYPE_LPT_AN589      :
    case PIC_INTF_TYPE_CUSTOM_LPT     :
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        {
            CallIoFunc(&PicHw_FuncPtr->SetVdd, vdd_on);
            CallIoFunc(&PicHw_FuncPtr->SetVpp, vpp_on);
            return PicHw_UpdateLptDataBits();
        }
        break;

    case PIC_INTF_TYPE_CUSTOM_COM: // interfaces for the serial port...
        if(PicHw_FuncPtr!=NULL)
        {
            CallIoFunc(&PicHw_FuncPtr->SetVdd, vdd_on);
            CallIoFunc(&PicHw_FuncPtr->SetVpp, vpp_on);
            return PicHw_UpdateComOutputBits();
        }
        break;

//    case PIC_INTF_TYPE_PLUGIN_DLL: // hardware driven through a "plugin" (DLL):
//         // Call the "standard" functions below !
//         break;
//
    default:   // for all other interfaces, use the standard routines :
        break;
    }
    // Arrived here ? Use the "standard" routines !
    fResult = PIC_HW_SetVdd( vdd_on );
    fResult = PIC_HW_SetVpp( vpp_on ) & fResult;
    return fResult;
} // end PIC_HW_SetVddAndVpp()

//------------------------------------------------------------------------
bool PIC_HW_CanSelectVdd(void)
// Queries if the programming adapter can select different supply voltages
{
//  if(PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL)
//   { // hardware driven through a "plugin" (DLL):
//     return (PHWInfo.iProductionGradeProgrammer >= 1);
//   }
//
    if(PicHw_FuncPtr!=NULL)
    {
        return (   (PicHw_FuncPtr->SelectVddLow.pFunc[0]  != NULL)
                   && (PicHw_FuncPtr->SelectVddLow.pFunc[0]  != PicHw_SetDummy) )
               || (   (PicHw_FuncPtr->SelectVddNorm.pFunc[0] != NULL)
                      && (PicHw_FuncPtr->SelectVddNorm.pFunc[0] != PicHw_SetDummy) )
               || (   (PicHw_FuncPtr->SelectVddHigh.pFunc[0] != NULL)
                      && (PicHw_FuncPtr->SelectVddHigh.pFunc[0] != PicHw_SetDummy) )
               ;
    }
    else
    {
        return false;
    }
} // end PIC_HW_CanSelectVdd()


//------------------------------------------------------------------------
bool PIC_HW_SelectVdd(int iVddLevel) // 0=low supply voltage, 1=normal, 2=high voltage
// Since 2005-09-25, this routine is also used to check if the interface
//       supports verify at a certain supply voltage at all.
//       If PIC_HW_SelectVdd() returns false, the function may have failed
//             or the interface may not support this voltage-switch at all.
{
    bool fResult;

    PicHw_iPresentVddSelection /*PHWInfo.iPresentVddSelection*/ = iVddLevel;

// if(PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL)
//   { // hardware driven through a "plugin" (DLL):
//     // Here, the supply voltage (Vdd) will be updated
//     // by the "driver" (hardware interface DLL) if possible
//     // when we turn Vpp on .
//     // No need to call a special function for that .
//     return (PHWInfo.iProductionGradeProgrammer >= 1);
//  }
//
    if(PicHw_FuncPtr!=NULL)
    {
        switch(iVddLevel)
        {
        case 0:
            fResult = ( PicHw_FuncPtr->SelectVddLow.pFunc[0] != PicHw_SetDummy)
                      &&( PicHw_FuncPtr->SelectVddLow.pFunc[0] != NULL);
            CallIoFunc(&PicHw_FuncPtr->SelectVddLow, 1);
            break;
        case 2:
            fResult =(PicHw_FuncPtr->SelectVddHigh.pFunc[0] != PicHw_SetDummy)
                     &&(PicHw_FuncPtr->SelectVddHigh.pFunc[0] != NULL);
            CallIoFunc(&PicHw_FuncPtr->SelectVddHigh,1);
            break;
        default: // Note: verify at VddNorm should alway be possible !
            fResult =(PicHw_FuncPtr->SelectVddNorm.pFunc[0] != PicHw_SetDummy)
                     &&(PicHw_FuncPtr->SelectVddNorm.pFunc[0] != NULL);
            CallIoFunc(&PicHw_FuncPtr->SelectVddNorm,1);
            break;
        }
        if(PIC_HW_interface.type==PIC_INTF_TYPE_CUSTOM_COM)
            fResult &= PicHw_UpdateComOutputBits();
        else  // programmer interface connected to the parallel port (LPT:)
            fResult &= PicHw_UpdateLptDataBits();
        PIC_HW_LongDelay_ms(50/*ms*/);  // there may be slow analog circuitry somewhere
        return fResult;
    }
    else
        return false;
} // end ..SelectVdd()

//------------------------------------------------------------------------
bool PIC_HW_SetClockEnable( bool fEnable ) // only used in the "AN589" programmer
{
    PicHw_fClockIsEnabled = fEnable;
    switch(PIC_HW_interface.type)
    {
    case PIC_INTF_TYPE_COM84:
    case PIC_INTF_TYPE_JDM2 :
        return true;  // ignore, but no error

    case PIC_INTF_TYPE_CUSTOM_COM:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the serial port...
        {
            CallIoFunc(&PicHw_FuncPtr->SetClkEnable, fEnable);
            return PicHw_UpdateComOutputBits();
        }
        return false;

//    case PIC_INTF_TYPE_PLUGIN_DLL: // hardware driven through a "plugin" (DLL):
//       if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_SetClockEnable!=NULL) )
//        { // Only call this DLL function if it has been successfully loaded :
//          return ( (*g_PHW_SetClockEnable)(&PHWInfo, fEnable)  >= 0);
//        }
//       return false; // end case PIC_INTF_TYPE_PLUGIN_DLL
//

    default:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        {
            CallIoFunc(&PicHw_FuncPtr->SetClkEnable, fEnable);
            return PicHw_UpdateLptDataBits();
        }
        return false;
    }
} // end ..SetClockEnable()

bool PIC_HW_SetDataEnable( bool fEnable ) // rarely used, for example in "AN589" programmer
{

    PicHw_fDataIsEnabled = fEnable;
    switch(PIC_HW_interface.type)
    {
    case PIC_INTF_TYPE_COM84:
    case PIC_INTF_TYPE_JDM2 :
        return true;  // ignore, but no error

    case PIC_INTF_TYPE_CUSTOM_COM:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the serial port...
        {
            CallIoFunc(&PicHw_FuncPtr->SetDataEnable, fEnable);
            return PicHw_UpdateComOutputBits();
        }
        return false;

//    case PIC_INTF_TYPE_PLUGIN_DLL: // hardware driven through a "plugin" (DLL):
//       if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_SetDataEnable!=NULL) )
//        { // Only call this DLL function if it has been successfully loaded :
//          return ( (*g_PHW_SetDataEnable)(&PHWInfo, fEnable)  >= 0);
//        }
//       return false; // end case PIC_INTF_TYPE_PLUGIN_DLL

    default:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        {
            CallIoFunc(&PicHw_FuncPtr->SetDataEnable, fEnable);
            return PicHw_UpdateLptDataBits();
        }
        return false;
    }
} // end ..SetDataEnable()


bool PIC_HW_SetClockAndData( bool clock_high, bool data_high )
{

    PicHw_fClockIsHigh = clock_high;
    PicHw_fDataOutIsHigh= data_high;

    switch(PIC_HW_interface.type)
    {
    case PIC_INTF_TYPE_COM84:
    case PIC_INTF_TYPE_JDM2 :
        return COM_SetPicClockAndData(clock_high,data_high );

    case PIC_INTF_TYPE_CUSTOM_COM:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the serial port...
        {
            CallIoFunc(&PicHw_FuncPtr->SetClk,  clock_high);
            CallIoFunc(&PicHw_FuncPtr->SetData, data_high);
            return PicHw_UpdateComOutputBits();
        }
        return false;

//    case PIC_INTF_TYPE_PLUGIN_DLL: // hardware driven through a "plugin" (DLL):
//       if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_SetClockAndData!=NULL) )
//        { // Only call this DLL function if it has been successfully loaded :
//          return ( (*g_PHW_SetClockAndData)(&PHWInfo, clock_high, data_high) >= 0);
//        }
//       return false; // end case PIC_INTF_TYPE_PLUGIN_DLL


    default:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        {
            CallIoFunc(&PicHw_FuncPtr->SetClk,  clock_high);
            PicHw_UpdateLptDataBits();  // << Added 2006-07-23, thanks Mark !
            CallIoFunc(&PicHw_FuncPtr->SetData, data_high);
            return PicHw_UpdateLptDataBits();
        }
        return false;
    }
} // end ..SetClockAndData()

int PIC_HW_GetDataBit(void)
{
    if( Config.iExtraRdDelay_us )  // does the INTERFACE need an additional delay
    {
        // before we can sample the state of the data-in line ?
        // (JDM2 seems to, especially when 'PortTalk' is in use)
        PIC_HW_Delay_us( Config.iExtraRdDelay_us );  // extra delay before sampling data-in
    }
    switch(PIC_HW_interface.type)
    {
    case PIC_INTF_TYPE_COM84:
    case PIC_INTF_TYPE_JDM2 :
        return COM_GetPicDataBit() ? 1 : 0 ;

//    case PIC_INTF_TYPE_PLUGIN_DLL: // hardware driven through a "plugin" (DLL):
//       if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_GetDataInBit!=NULL) )
//        { // Only call this DLL function if it has been successfully loaded :
//          return (*g_PHW_GetDataInBit)(&PHWInfo);
//        }
//       return false; // end case PIC_INTF_TYPE_PLUGIN_DLL

    default:
        if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        {
            return CallIoFunc(&PicHw_FuncPtr->GetInBit, -1/*don't change, only read state*/ );
        }
        return 0;
    }
} // end ..GetDataBit()

////---------------------------------------------------------------------------
//void PicHw_FlushCommand( uint32_t dwCommand,
//                         uint32_t dwNumCommandBits, // usually 6 (!) "command" bits
//                         uint32_t dwData,
//                         uint32_t dwNumDataBits )   // usually 14(!) "data" bits
//  // Required for some "intelligent" programmers like USB-interfaces,
//  // which collect all clock- and data transition into a single packet,
//  // for example using a single USB BULK TRANSFER instead of sending each
//  // clock- or data-transition in a single transfer (which would be incredibly
//  // slow, depending on the USB hardware).
//  // When the ..._FlushCommand() is issued, the PIC hardware driver knows
//  // that it's time to send the previously collected block to the programmer.
//{
// switch(PIC_HW_interface.type)
//  { case PIC_INTF_TYPE_COM84:
//    case PIC_INTF_TYPE_JDM2 :
//       break;
//
////    case PIC_INTF_TYPE_PLUGIN_DLL: // hardware driven through a "plugin" (DLL):
////       if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_FlushCommand!=NULL) )
////        { // Only call this DLL function if it has been successfully loaded :
////          (*g_PHW_FlushCommand)(&PHWInfo,
////                         dwCommand, dwNumCommandBits,  dwData, dwNumDataBits );
////          // integer return value is ignored; but this may change in future !
////        }
////       break;
//
//    default:
//       break;
//  }
//
//} // end PicHw_FlushCommand()

//---------------------------------------------------------------------------
// To read the current state of the control lines (from last access to the outputs)
int  PicHw_IsVppOn(void)        // 1=yes 0=no -1=not supported
{
    if(   (PIC_HW_interface.type==PIC_INTF_TYPE_COM84)
            || (PIC_HW_interface.type==PIC_INTF_TYPE_JDM2)
//    || (PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL)
      )
    {
        return PicHw_fVppIsOn;  // KISS: just return the last known state
    }
    if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        return CallIoFunc(&PicHw_FuncPtr->SetVpp, -1/*don't change, only read state*/ );
    return -1;
}

int  PicHw_IsVddOn(void)
{
    if(   (PIC_HW_interface.type==PIC_INTF_TYPE_COM84)
            || (PIC_HW_interface.type==PIC_INTF_TYPE_JDM2)
      )
    {
        return -1;  // the COM84 has no Vdd switch,  Vpp also drives Vdd !
    }
// if( PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL )
//  {  return PicHw_fVddIsOn;   // KISS: just return the last known state
//  }

    if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        return CallIoFunc(&PicHw_FuncPtr->SetVdd, -1/*don't change, only read state*/ );
    return -1;
}

int  PicHw_IsClockHigh(void)
{
    if(   (PIC_HW_interface.type==PIC_INTF_TYPE_COM84)
            || (PIC_HW_interface.type==PIC_INTF_TYPE_JDM2)
//    || (PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL)
      )
    {
        return PicHw_fClockIsHigh;
    }
    if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        return CallIoFunc(&PicHw_FuncPtr->SetClk, -1/*don't change, only read state*/ );
    return -1;
}


int  PicHw_IsDataOutHigh(void)
{
    if(   (PIC_HW_interface.type==PIC_INTF_TYPE_COM84)
            || (PIC_HW_interface.type==PIC_INTF_TYPE_JDM2)
//    || (PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL)
      )
    {
        return PicHw_fDataOutIsHigh;
    }
    if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        return CallIoFunc(&PicHw_FuncPtr->SetData, -1/*don't change, only read state*/ );
    return -1;
}

int  PicHw_IsClockEnabled(void)
{
    if( (PIC_HW_interface.type==PIC_INTF_TYPE_COM84) || (PIC_HW_interface.type==PIC_INTF_TYPE_JDM2) )
        return -1;
// if( PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL )
//  { if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_SetClockEnable!=NULL) )
//     { return PicHw_fClockIsEnabled;
//     }
//    else
//     { return -1; // indicator on the interface control panel will be grayed now
//     }
//  }
    if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        return CallIoFunc(&PicHw_FuncPtr->SetClkEnable, -1/*don't change, only read state*/ );
    return -1;
}

int  PicHw_IsDataEnabled(void)
{
    if( (PIC_HW_interface.type==PIC_INTF_TYPE_COM84) || (PIC_HW_interface.type==PIC_INTF_TYPE_JDM2) )
        return -1;
// if( PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL )
//  { if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_SetDataEnable!=NULL) )
//     { return PicHw_fDataIsEnabled;
//     }
//    else
//     { return -1; // indicator on the interface control panel will be grayed now
//     }
//  }
    if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        return CallIoFunc(&PicHw_FuncPtr->SetDataEnable, -1/*don't change, only read state*/ );
    return -1;
}

int PicHw_IsMclrPulledToGnd(void)
{
    if( (PIC_HW_interface.type==PIC_INTF_TYPE_COM84) || (PIC_HW_interface.type==PIC_INTF_TYPE_JDM2) )
        return -1;
// if( PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL )
//  { if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_PullMclrToGnd!=NULL) )
//     { // Only return the "set" state if the DLL can really set it :
//       return PicHw_fMclrPulledToGnd;
//     }
//    else
//     { return -1; // indicator on the interface control panel will be grayed now
//     }
//  } // end if  PIC_INTF_TYPE_PLUGIN_DLL
    if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        return CallIoFunc(&PicHw_FuncPtr->PullMclrToGnd, -1/*don't change, only read state*/ );
    return -1;
}

int PicHw_IsTargetConnected(void)
{
    if( (PIC_HW_interface.type==PIC_INTF_TYPE_COM84) || (PIC_HW_interface.type==PIC_INTF_TYPE_JDM2) )
        return -1;

// if( PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL )
//  { // hardware driven through a "plugin" (DLL):
//    if( PicHw_hFilterPluginDLL!=NULL )
//     { // Only call this DLL function if it has been successfully loaded :
//       return PicHw_iConnectedToTarget;
//     }
//    return -1;
//  }

    if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        return CallIoFunc(&PicHw_FuncPtr->ConnectTarget, -1/*don't change, only read state*/ );
    return -1;
}

int  PicHw_GetOkButtonState(void)
{
    if( (PIC_HW_interface.type==PIC_INTF_TYPE_COM84) || (PIC_HW_interface.type==PIC_INTF_TYPE_JDM2) )
        return -1;

// if( PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL )
//  { // hardware driven through a "plugin" (DLL):
//    if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_GetOkButton!=NULL) )
//     { // Only call this DLL function if it has been successfully loaded :
//       return (*g_PHW_GetOkButton)(&PHWInfo);
//     }
//    return -1;
//  }

    if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        return CallIoFunc(&PicHw_FuncPtr->GetOkButton, -1/*don't change, only read state*/ );
    return -1;
}

int PIC_HW_SetRedLed( int iNewState )
{
    if( (PIC_HW_interface.type==PIC_INTF_TYPE_COM84) || (PIC_HW_interface.type==PIC_INTF_TYPE_JDM2) )
        return -1;
    if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        return CallIoFunc(&PicHw_FuncPtr->SetRedLed, iNewState );
    return -1;
}

int PIC_HW_SetGreenLed( int iNewState )
{
    if( (PIC_HW_interface.type==PIC_INTF_TYPE_COM84) || (PIC_HW_interface.type==PIC_INTF_TYPE_JDM2) )
        return -1;
    if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
        return CallIoFunc(&PicHw_FuncPtr->SetGreenLed, iNewState );
    return -1;
}


void PIC_HW_ClockOut( uint16_t data_bit )
{
    // "clocks" a single bit out to the PIC
    PIC_HW_SetClockAndData(true, data_bit);
    if( Config.iSlowInterface )  // this option for "bad, slow interfaces" was added 2005-06
        PIC_HW_Delay_us(10 * PIC_DeviceInfo.lTi_Clock_us + Config.iExtraClkDelay_us );
    else PIC_HW_Delay_us( PIC_DeviceInfo.lTi_Clock_us + Config.iExtraClkDelay_us );
    PIC_HW_SetClockAndData(false,data_bit);
    if( Config.iSlowInterface )
        PIC_HW_Delay_us(10 * PIC_DeviceInfo.lTi_Clock_us + Config.iExtraClkDelay_us );
    else PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Clock_us + Config.iExtraClkDelay_us );
// ex: PIC_HW_SetClockAndData(false, false);
// but: there's no need to set the data bit back to zero here (except.. see below)
#if(1) // Added 2007-05-08 for testing purposes :
// Special requirement by some JDM 2 interfaces, thanks Ken Healy for the note:
// > "At programming PIC's will only D3 be used. RTS and DTR need to be low
// >  to power it, and they must not be high for long time.
// >  The current is extra high when reading a zero from the PIC,
// >  and the active reading time with DTR high must be short.
// >  To compensate the power used, will extra time at RTS low need to be inserted."
// > To achieve this, I believe you need to make the clock low time greater
// > than the clock high time. I think this change should be made
// >  in the PIC_HW_ClockOut() function ..
    if( PIC_HW_interface.type == PIC_INTF_TYPE_JDM2 )
    {
        // Here: 'data'=DTR, 'clock'=RTS .
        // Both should be LOW for a bit longer to power the JDM 2 adapter, as described above:
        PIC_HW_SetClockAndData( false/*clk=RTS*/, false/*data=DTR*/ );
        // To make sure, the voltage on RTS and DTR were measured
        // against RS-232-GROUND after stopping the program at this point. Result:
        // RTS=pin4 on DSUB9 : -9.7 Volts (against pin5=GND)
        // DTR=pin7 on DSUB9 : -5.5 Volts (against pin5=GND)
        if( Config.iSlowInterface ) // extra delay to recharge the JDM 2 :
            PIC_HW_Delay_us(20 * PIC_DeviceInfo.lTi_Clock_us + 50/*us*/ );
        else PIC_HW_Delay_us( 2 * PIC_DeviceInfo.lTi_Clock_us + 50/*us*/ );
    } // end if < JDM 2 >
#endif // 0,1 (test 2007-05-08)

} // end PIC_HW_ClockOut()


int PIC_HW_ClockIn(void)
{
    // "clocks" a single bit in  from the PIC.  negative return means 'error' here

    // COM84-programmer:
    //   Keep the data output from PC to PIC high, because
    //   a "high" signal can be overridden quite easy by a "low",
    //   but not vice versa.

    // PIP84-programmer:
    //   Keep the data output from PC to PIC high, otherwise
    //   an NPN transistor (after an inverter) would tie RB7 to ground.
    //   See WinPicPr/Doku/pipschem.gif (by Johan Bodin) .

    // Microchip "AN589" programmer:
    //   The direction of CLOCK and DATA buffers are set
    //   before this routine is called.

#if(0)
    // old stuff, don't know where it came from...
    PIC_HW_SetClockAndData(true, true  );
    PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Clock_us);
    // 2002-10-10: shouldn't the DATA BIT be sampled HERE ?!?
    PIC_HW_SetClockAndData(false,true  );          // OLD STUFF
    PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Clock_us);
    return PIC_HW_GetDataBit();   // negative return means 'error' here
#else
    // modified version from 2002-10-10:
    // ClockHigh - wait Tdly3 - sample data - ClockLow
    int iResult;

    // ex: PIC_HW_SetClockAndData(true, true  );   // problematic with "JDM"-programmer (discharges C3)
    // but unavoidable, because we USUALLY(*) need the HIGH DATA BIT (output) to enable reading !
    // (*) Since 2004-11, the level of the DATA OUTPUT (from PC to PIC) while READING
    //                    is also customizeable via interface definition file.
    PIC_HW_SetClockAndData( true,
                            PicHw_iStateOfDataOutWhileReading); // state of data-output-line while reading
    if( Config.iSlowInterface )
        PIC_HW_Delay_us(10 * PIC_DeviceInfo.lTi_Clock_us + Config.iExtraClkDelay_us );
    else PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Clock_us + Config.iExtraClkDelay_us );
    iResult = PIC_HW_GetDataBit();          // sample data from PIC while clock=HIGH
    if(PIC_HW_interface.type==PIC_INTF_TYPE_JDM2)
    {
        // for the JDM programmer, avoid discharging C3 (22uF) by setting CLOCK + DATA LOW :
        PIC_HW_SetClockAndData( false, true );
//     PIC_HW_SetClockAndData( false, false );
#if(1) // extra delay to make the clock low time longer than the high time :
        if( Config.iSlowInterface ) // extra delay to recharge the JDM 2 :
            PIC_HW_Delay_us(20 * PIC_DeviceInfo.lTi_Clock_us + 50/*us*/ );
        else PIC_HW_Delay_us( 2 * PIC_DeviceInfo.lTi_Clock_us + 50/*us*/ );
#endif
    } // end if < JDM 2 >
    else  // all other interfaces should keep the DATA OUTPUT (to PIC) high :
    {
        PIC_HW_SetClockAndData( false, true  );
    }
    if( Config.iSlowInterface )
        PIC_HW_Delay_us(10 * PIC_DeviceInfo.lTi_Clock_us + Config.iExtraClkDelay_us );
    else PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Clock_us + Config.iExtraClkDelay_us );
    return iResult;   // negative return means 'error' here
#endif // OLD,NEW

} // end PIC_HW_ClockIn()


// High Level functions. May be replaced to drive 'intelligent' programmers.
//
// (or a 'Fundamentally Different Hardware' as they call it in pp06).
// High Level Functions (from pp06 at sourceforge.net) :
//  void run_mode(int mode);
//  void reset(int channel);
//  void prog_mode(int volt);
//  void out_word(int w);
//  int in_word(void);
//  void command(int cmd);


//----------------------------------------------------------------------------
bool PIC_HW_DisconnectFromTarget(void) // since 2002-09-09
{

    if( PIC_HW_CanSelectVdd() )      // since 2005-09-29 :
    {
        PIC_HW_SelectVdd( Config.iIdleSupplyVoltage );  // back to the "normal" voltage
    }
    PicHw_iConnectedToTarget = 0;    // no longer "connected to target" now...

//  if(PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL)
//   { // hardware driven through a "plugin" (DLL):
//     if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_ConnectTarget!=NULL) )
//        { // Only call this DLL function if it has been successfully loaded :
//          return ( (*g_PHW_ConnectTarget)(&PHWInfo, 0/*disconnect*/ ) >= 0);
//        }
//     return true;
//   }

    if(PIC_HW_interface.type==PIC_INTF_TYPE_JDM2)
    {
        // for the JDM programmer, things are very different.
        // Here, TXD(Vpp), DTR(DataOut), and RTS(Clock) but be set HIGH(!)
        //       to turn the PIC's supply voltage off (!!) :
        PIC_HW_SetClockAndData( true, true );
        PIC_HW_SetVpp( true );   // Yes, indeed true to turn the PIC's supply voltage off !
        PIC_HW_LongDelay_ms(100/*ms*/);
        return true;
    }
    else  // Not JDM... for all other interfaces : turn everything OFF by setting it LOW :
    {
        PIC_HW_SetVpp( false/*turn Vpp off*/ );  // FIRST, turn Vpp off before disconnecting !
    }
    PIC_HW_LongDelay_ms(100/*ms*/);  // there may be something slow, like a RELAY

    if(PicHw_FuncPtr!=NULL)  // looks like a programmer for the LPT port...
    {
        CallIoFunc(&PicHw_FuncPtr->ConnectTarget,0);
        if(PIC_HW_interface.type==PIC_INTF_TYPE_CUSTOM_COM)
            return PicHw_UpdateComOutputBits();
        else  // programmer interface connected to the parallel port (LPT:)
            return PicHw_UpdateLptDataBits();
    }
    else
        return false;
} // end PIC_HW_DisconnectFromTarget()


//----------------------------------------------------------------------------
bool PIC_HW_ConnectToTarget(void)  // since 2002-09-09
{
    bool fResult;

// before we start, update this param from the config... ugly but simple
// PHWInfo.iSlowInterface = Config.iSlowInterface;

//  if(PIC_HW_interface.type==PIC_INTF_TYPE_PLUGIN_DLL)
//   { // hardware driven through a "plugin" (DLL):
//     PicHw_iConnectedToTarget = 1;
//     if( (PicHw_hFilterPluginDLL!=NULL) && (g_PHW_ConnectTarget!=NULL) )
//        { // Only call this DLL function if it has been successfully loaded :
//          return ( (*g_PHW_ConnectTarget)(&PHWInfo, 1/*connect*/ ) >= 0 );
//        }
//     return true;
//   }


    if(PicHw_FuncPtr!=NULL)
    {
        CallIoFunc(&PicHw_FuncPtr->ConnectTarget, 1);
        if(PIC_HW_interface.type==PIC_INTF_TYPE_CUSTOM_COM)
            fResult=PicHw_UpdateComOutputBits();
        else  // programmer interface connected to the parallel port (LPT:)
            fResult=PicHw_UpdateLptDataBits();
        PIC_HW_LongDelay_ms(200/*ms*/);  // there may be a slow ELECTROMECHANIC RELAY somewhere
        PicHw_iConnectedToTarget = 1;
        return fResult;
    }
    else
    {
        PicHw_iConnectedToTarget = 1;  // "connected to target" even if there is no extra control signal for it !
        return false;
    }
} // end PIC_HW_ConnectToTarget()


//----------------------------------------------------------------------------
void PIC_HW_ResetAndGo(void)  // disconnect ICSP; reset target; run target
{
    PIC_HW_SetVpp( false );        // switch programming voltage off - just to be sure
    PIC_HW_DisconnectFromTarget(); // release clock- and data lines
    PIC_HW_SetVdd( true );         // switch PIC supply voltage (5V) ON
    PIC_HW_PullMclrToGnd(true);    // pull MCLR low, only(?) for "AN589" ICSP programmer
    PIC_HW_LongDelay_ms(50);       // wait 50ms with RESET (MCLR) held low
    PIC_HW_PullMclrToGnd(false);   // ready, steady, GO !
} // end PIC_HW_ResetAndGo()

//----------------------------------------------------------------------------
void PIC_HW_ProgMode(void)  // switch PIC into programming mode
{
    // Starts every programming sequence, for both FLASH and EPROM-based MCU's .
    // Valid for all 12F's, 16F's to enter "ICSP" mode
    //            and for dsPIC's to enter "STDP" mode :
    //   Fast rise on /MCLR("Vpp") from zero to about 12 volts ,
    //   while both CLOCK("PGC") and  DATA("PGD") are held low by the host.
    //
    // The lousy documentation for certain PICs did not mention the Vdd-/ Vpp(MCLR)-
    //     activation sequence, and in fact different PICs behave completely different.
    // In 2003-12-30, a formerly removed feature had to be re-activated
    //     because of a lousy difference between 16F628 and 16F818 !
    //     Reading a 16F818 FAILED if Vpp was applied to the MCLR pin
    //               before turning Vdd on (!! the F818 needed "Vdd before Vpp" !!),
    //             while a 16F628 was happy with exactly this situation.
    //     Unfortunately the "COM port" programmers do not allow switching
    //     the supply voltage and the MCLR signal (alias RESET and Vpp) independently !
    // In 2004-01-27, the author found the programming spec for a PIC16F628A(!),
    //     where a diagram clearly shows Vpp=MCLR being raised to 13 Volts
    //     while Vdd=supply is ZERO volts,  and *THEN* (after "Tppdp") setting
    //     Vdd to 5 Volts.     Whow.        So the F628A needs "Vpp before Vdd" !
    // In 2007-08-28, a PIC16F690 refused to enter programming mode (with JDM2),
    //     and yet another "Memory Programming Specification" (DS41204G this time)
    //     mentions the following "VPP-FIRST PROGRAM/VERIFY MODE ENTRY" (figure 3-1):
    //                ________________________
    //      Vpp   ___/
    //                   _____________________
    //      Vdd   ______/
    //                          _ _ _ _ _ _ _
    //   ICSPDAT  ____________/_ _ _ _ _ _ _ _
    //                         _   _   _   _
    //   ICSPCLK  ____________/ \_/ \_/ \_/ \_
    //
    //   > "Note: This method of entry is valid, regardless of Configuration Word"
    // WB: In other words, "the other" method may fail depending of the config word.
    //     Murphy says "the other" method WILL fail, so for PIC16F690 (and similar),
    //     we MUST use the "Vpp-before-Vdd" sequence. Even with a JDM-2.  Ouch ! !


    if( PIC_HW_interface.type==PIC_INTF_TYPE_JDM2 )
    {
        // Added 2004-05-30, due to problems with JDM programmer with 16F877A
        //                   when programming+verify in two separate overs .......
        // In the "JDM", thing are very different, because it is impossible here
        // to turn the PIC's supply voltage quickly on.
        // This makes it very tricky+ugly to switch PICs properly into programming mode
        // which do not have a RESET INPUT (like many modern PICs where the reset
        // pin can be used as digital input via configuration bit).
        // There *were* two different cases how to treat the "JDM" programmer:
        // 1 - for older PICs which "need power before raising MCLR" ,
        // 2 - for newer PICs which need the fast rise on Vpp(MCLR) before applying Vdd.
        // But since the 2nd never worked too well, it was withdrawn...
        // EX: Because even with the 1st, certain PICs failed under "tough conditions",
        // EX: (like PIC16F628 with internal RC-oscillator and MCLR disabled),
        // EX: tried the following ugly sequence TWO TIMES in the hope it works better.
        // EX: for(int i=0; i<2; ++i)
        // Result : The ugly "two-times-trick" didn't work either. More research needed.
        if(Config.iNeedPowerBeforeRaisingMCLR)    // the old "Vdd before Vpp"-annoyance..
        {
            // here for old PICs which may have Vpp ("power") before raising MCLR to the Vpp level:

            // Make sure C2 (100uF) is charged to 8 volts  and  C3 (22uF)
            // charged to 5 volts (=negative bias voltage between PIC's Vss and PC's GROUND).
            PIC_HW_SetClockAndData(false,false);  // -> RTS=-12V, DTR=-12V
            PIC_HW_SetVpp( true );                // -> TXD=+12V , charges C2 (100uF) to ~13.2 volts
            // this is relatively fast, because no resistors are involved:  13V*100uF/5mA = 260ms at least !
            PicHw_ToggleTXD( 0 );           // no need to toggle TXD now  !
            // (charge from RTS+DTR=-12V and TXD=+12V, permanently)
            PIC_HW_LongDelay_ms(500/*ms*/); // give the CPU to "someone else" while charging C2
            // Now pull MCLR / Vpp low, so the PIC enters the RESET state .
            // Note: Not all PIC's do have a RESET necessarily, see other case below !
            PIC_HW_SetVpp( false );  // -> TXD= -12V (hard limited to -5.7V), charges C3 to 5 volts
            //        while C2 remains at 8 volts.
            // wait a short time for the Vpp(!) switch ..
            //     (the PIC itself only needs to see MCLR=LOW for 2 us) :
            if( Config.iSlowInterface )
                PIC_HW_Delay_us(1000);  // added 2005-06 for testing bad interfaces
            else PIC_HW_Delay_us(100);   // normal mode, works for "good" interfaces
            PIC_HW_SetVpp(true);  // Now there should be a faster rise on /MCLR = "Vpp",
            // from 0 V to almost 13 V (from the PIC's point of view).
            // For many PIC devices, this rise time must not exceed 1 MICROSECOND,
            //          which is the reason why many ultra-ugly programmers fail .
            // Measured on an oscilloscope, where GROUND is connected to the PC's ground!!
            // (even if you don't want it that way), Vpp rises from -5 to +8 volts.
            // ex: PIC_HW_LongDelay_ms(10); // make sure the supply voltage is stable (JDM programmer)
            // don't wait too long, some cap in the JDM may be discharged too soon !
            if( Config.iSlowInterface )
                PIC_HW_Delay_us(5000);  // added 2005-06 for testing bad interfaces
            else PIC_HW_Delay_us(500);   // normal mode, works for "good" interfaces
        }
        else  // JDM-2,  Config.iNeedPowerBeforeRaisingMCLR = 0 -------------->>>
        {
            // Config.iNeedPowerBeforeRaisingMCLR = 0  means:
            //   "do NOT turn Vdd on before lifting MCLR from 0V to Vpp"
            // Here, for the JDM programmer, raise Vpp to 13 Volt BEFORE turning Vdd on :
            // This is tricky and VERY UGLY, and it failed quite often (!)
            PIC_HW_SetClockAndData(true,true);  // -> RTS=+12V, DTR=+12V

            PIC_HW_SetVpp( true );              // -> TXD=+12V
            // All control lines from PC to PIC are *POSITIVE* now
            //  (ok, RTS is brutally clamped to +0.7 V via D4 on the "JDM2"-interface) .
            // Check this: (Caution, the original JDM circuit uses a 25-pin connector, WE DON'T ! USE 9-pin INSTEAD)
            //    Voltmeter's GROUND connected to 9-pin RS232 pin 5 (PC GND, not "PIC GND"),
            //    measured:  "Vpp"  = TXD(3)= +9 V
            //               "data" = DTR(4)= +10 V
            //               "clock"= RTS(7)= +0.35 V ('cos WoBu used a Schottky diode "BAT42" for D4) .
            //               "PIC GND" against "RS232 GND" = - 1.xx V (slowly drifting around)
            // In this state, C2 (100 uF) will be charged, but -more important-
            // C3 (22 uF) will be DISCHARGED after about 500 ms .
            // For some safty margin, we use significantly more (2000 ms) here...
            PicHw_ToggleTXD( 1 );           // toggle TXD while waiting in the next line
            PIC_HW_LongDelay_ms(2000);      // wait until C3 in the JDM interface is discharged
            // Arrived here: The voltage at the PIC's "Vss"(GND) pin is about 0.7 V
            // above the RS-232's GROUND line. In other words, there is no supply voltage
            // for the PIC because C3 (22 uF, between PIC's Vss and Vdd) is empty.
            PIC_HW_SetVpp(false);   // pull voltage at \MCLR down (alias TxD, switches from +9 V to -5.8 V)
            //   causing the PIC to enter the RESET state (if it HAS a reset input ! )
            // Unfortunately this also starts charging C3, but not very fast.
            // (about 0.5 volt per millisecond when tested on DL4YHF's machine).
            // wait a very short time for the Vpp(!) switch
            if( Config.iSlowInterface )
                PIC_HW_Delay_us(500);  // added 2005-06 for testing bad interfaces
            else PIC_HW_Delay_us(50);   // normal mode, works for "good" interfaces

            PIC_HW_SetClockAndData(false,false);  // -> RTS=-12V, DTR=-12V   as they should be for the 1st serial command
            // Check as explained above (measured against PIN 5 of 9-pole RS232 connector, modified "JDM" ) :
            //      "Vpp"  = TXD(3)= - 5.9 V
            //      "data" = DTR(4)= -10.1 V
            //      "clock"= RTS(7)= - 5.6 V
            //      "PIC GND" against "RS232 GND" = - 5.2 V (stable)
            PIC_HW_SetVpp(true);  // "fast" rise on Vpp again,
            // Hopefully Vpp reaches the threshold (near 13 V)
            // before the PIC starts running because C3 also charges rapidly.
            // It may take another 50 ms  until Vdd has reached about 4.5 V,
            // so wait until Vdd is stable enough for programming:
            PIC_HW_LongDelay_ms(50);  // make sure the supply voltage is stable (JDM programmer)
            // Check as explained above (measured against PIN 5 of 9-pole RS232 connector, modified "JDM") :
            //      "Vpp"  = TXD(3)= + 8.9 V
            //      "data" = DTR(4)= -10.1 V
            //      "clock"= RTS(7)= - 5.5 V
            //      "PIC GND" against "RS232 GND" = - 5.15 V (stable)
            PicHw_ToggleTXD( 0 );           // stop toggling TXD now !
        }
    } // end if < JDM programmer >
    else
    {
        //----------------------- ALL OTHER PROGRAMMERS (except "JDM") --------------------------
        if(Config.iNeedPowerBeforeRaisingMCLR)  // Result of the "long story", here in PIC_HW_ProgMode()...
        {
            // Config.iNeedPowerBeforeRaisingMCLR = 1 means:
            //   "Turn Vdd on, wait <100 us, then lift MCLR from 0V to Vpp"
            //   Turning on Vdd before Vpp seems to be important for a 16F818,
            //   though ITS programming spec says absolutely NOTHING about this !
            //  The 16F876A also "needs power (supply voltage) before raising MCLR to the Vpp level" !
            // This was once considered to be the result of a misinterpretation
            // of an inaccurate OLD programming spec, but was re-installed in 2003-12 :
            // First, set everything as it should be before turning anything on:
            PIC_HW_PullMclrToGnd(true);  // pull MCLR to ground (not many adapters support this)
            PIC_HW_SetClockAndData(false,false);
            // (for some PICs its very important to have RB6 & RB7 low while raising Vpp)
            PIC_HW_SetClockEnable(true );  // Enable the tristate buffer outputs (PC->PIC)
            PIC_HW_SetDataEnable( true );  // Enable data OUTPUT (serial data TO PIC)
            PIC_HW_SetVpp(false);          // ensure Vpp is off
            PIC_HW_SetVdd(false);          // ensure Vdd is off
            if( Config.iSlowInterface )
                PIC_HW_Delay_us(8000);  // added 2005-06 for testing bad interfaces
            else PIC_HW_Delay_us(800);   // 800us to discharge a 100nF capacitor via 4k7
            PIC_HW_PullMclrToGnd(false); // release MCLR/Vpp (don't pull to ground any longer, D4=LOW for "AN589")
            //  >  Because some "ugly style" programmers switch Vdd together with
            //  >  Vpp,  turn Vpp off for a very short time to generate a steep
            //  >  rise on Vpp while Vdd is still on :
            if( PIC_HW_interface.wSeparateVddAndVppControl )  // 0=no, 1=yes
            {
                PIC_HW_SetVdd(true);    // turn PIC's supply voltage on
                if( Config.iSlowInterface ) // wait a very short time for the Vdd(!) switch
                    PIC_HW_Delay_us(500);  // a test for BAD interfaces
                else PIC_HW_Delay_us(50);   // this should work for BETTER interfaces
                PIC_HW_SetVpp(true);    // lift MCLR from 0 V to 13 V within 100 us
            }
            else  // Need power before raising MCLR ,  but no separate Vdd and Vpp control lines :
            {
                // (like in the COM84 adapter, which has a Vpp(!) but no Vdd(!) control line)
                // Caution: Some PICs may not have a reset input.
                //   In that case, the rising edge on Vpp *must* be generated only *once* !
                //   The ugly on-off-on-off sequence to charge an electrolyte capacitor in certain
                //   ugly programmers has been removed because it causes trouble with new PICs...
                //   ... Sorry for those folks using ridiculously simple programmers ...
                if( Config.iSlowInterface )
                    PIC_HW_Delay_us(10 * PIC_DeviceInfo.lTi_Prog_us); // test for "bad" interfaces
                else PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);      // normal mode for "good" interfaces
                PIC_HW_SetVdd(true);    // turn PIC's supply voltage on (no effect for COM84)
                // wait a very short time for the Vdd(!) switch ..
                if( Config.iSlowInterface )
                    PIC_HW_Delay_us(500); // extra long delay for "bad" interfaces
                else PIC_HW_Delay_us(50);  // orignal delay for "good" interfaces
                PIC_HW_SetVpp(true);    // an utterly slow rise on MCLR (because of discharged cap)
                if( PicHw_fTogglingTxD )  // If there is a charge pump for "Vpp", keep toggling a bit longer...
                {
                    PIC_HW_LongDelay_ms(500/*ms*/);  // while WAITING here, feed the UART's transmit buffer (!)
                }
                else // if there is NO charge pump for "Vpp", use the typical "Vpp-ON-delay",
                {
                    // then turn Vpp off for a short time, and turn it on to produce a FAST RISE on Vpp/MCLR:
                    if( Config.iSlowInterface )
                        PIC_HW_Delay_us( 10 * PIC_HW_interface.vpp_on_delay_us );
                    else PIC_HW_Delay_us(PIC_HW_interface.vpp_on_delay_us);  // time required by INTERFACE (often 200ms)
                    PIC_HW_SetVpp(false);   // Vdd(!) capacitor should be charged now,
                    //   the next positive slope on Vpp will be steeper
                    if( Config.iSlowInterface )
                        PIC_HW_Delay_us(500); // extra long delay for "bad" interfaces
                    else PIC_HW_Delay_us(50);  // normal mode: wait a very short time for the Vpp(!) switch
                    PIC_HW_SetVpp(true);    // now this should be a faster rise on /MCLR = "Vpp"
                }
            } // end < no separate Vdd and Vpp control lines >
        }
        else
        {
            // Config.iNeedPowerBeforeRaisingMCLR = 0  means:
            //   "do NOT turn Vdd on before lifting MCLR from 0V to Vpp"
            //   Caution, a 16F876A (and possibly some others) DO NOT WORK WITH THIS Vpp/Vdd sequence !
            // Programming spec for 12F675 has a timing diagram which suggests to
            // raise Vpp BEFORE(!) Vdd ... which older spec's did not have..
            // if(    ( PIC_DeviceInfo.wVppVddSequence == PROGMODE_VPP_THEN_VDD )
            //    && ( PIC_HW_interface.wSeparateVddAndVppControl/*0=no, 1=yes*/ )
            //  )
            // { // this may turn out to be the only correct way (2004: no, it didn't)...
            // The following is based on Microchip's DS41173b, page 3,  figure 2-2,
            //   locally saved by the author as PIC12F675_prog_spec.pdf .
            // The same seems to be true for the PIC16F628A (!), where they even
            //  put the Vpp / Vdd sequence in the
            //   "PROGRAM FLOW CHART - PIC16F627A/628A/648A PROGRAM MEMORY" .
            //
            // First, set everything as it should be before turning anything on:
            PIC_HW_PullMclrToGnd(true);  // pull MCLR to ground (not many adapters support this)
            PIC_HW_SetClockAndData(false,false);
            // (for some PICs its very important to have RB6 & RB7 low while raising Vpp)
            PIC_HW_SetClockEnable(true );  // Enable the tristate buffer outputs (PC->PIC)
            PIC_HW_SetDataEnable( true );  // Enable data OUTPUT (serial data TO PIC)
            PIC_HW_SetVpp(false);    // ensure Vpp off
            PIC_HW_SetVdd(false);    // ensure Vdd off
            if( Config.iSlowInterface )
                PIC_HW_Delay_us(8000);
            else PIC_HW_Delay_us(800);   // 800us to discharge a 100nF capacitor via 4k7
            PIC_HW_PullMclrToGnd(false); // release MCLR/Vpp (don't pull to ground any longer, D4=LOW for "AN589")
            PIC_HW_SetVpp(true);     // yes, really apply Vpp before Vdd !
            // Why ? Some PICs like the 12F675 may not have a reset input at all.
            // Here, Vpp must be HIGH before applying Vdd, otherwise the PIC starts running,
            // and may initialize port directions which disturbs in-circuit programming,
            // and the program counter may be > 0 when entering programming mode too late.
            // > "holdup time after Vpp(!) up" ... ("Tppdp", min 5 us)
            PIC_HW_Delay_us(25);     // 5us is the min delay dictated by spec, we use much more
            if( PicHw_fTogglingTxD )  // If there is a charge pump for "Vpp", keep toggling a bit longer...
            {
                PIC_HW_LongDelay_ms(300/*ms*/);  // while WAITING here, feed the UART's transmit buffer (!)
            }
            PIC_HW_SetVdd(true);     // turn supply voltage on now
            // > "CLOCK, DATA holdup time after /MCLR(!!) up" ... ("Thld0", min 5 us) ..
            // ( why this ?!?  "Vpp(!)" and "/MCLR(!!)" is the SAME PIN, stupido !!
            //   If they really have a bloody typo in Figure 2-2, then
            //   "Tppdp" is the time between rising Vpp(high voltage) and Vdd(supply) ,
            //   "Thld0" is the time between rising Vpp and beginning the first clock pulse.
            // )
            // ex: PIC_HW_Delay_us(10);   // 10us
            // Because there is a quite useless electrolyte cap of 22uF in many circuits,
            // a significant time may be required to charge it until the PIC is ready for business:
            if( PicHw_fTogglingTxD )  // If there is a charge pump for "Vpp", keep toggling a bit longer...
            {
                PIC_HW_LongDelay_ms(300/*ms*/);  // while WAITING here, feed the UART's transmit buffer (!)
            }
            else // no need to toggle TxD, use the normal (precise) delay routine :
            {
                if( Config.iSlowInterface )
                    PIC_HW_Delay_us(10 * PIC_HW_interface.vpp_on_delay_us);
                else PIC_HW_Delay_us(PIC_HW_interface.vpp_on_delay_us);  // time required by INTERFACE (often 200ms)
                PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // time required by PIC DEVICE
            }

        } // end else <Config.iNeedPowerBeforeRaisingMCLR = 0>
    } // end if < not "JDM" programmer >

    // caller must not forget to turn Vpp & Vdd off after he's ready.

} // end PIC_HW_ProgMode()   // yucc - the end of a long story. Did you understand it ? ;-)


//----------------------------------------------------------------------------
void PIC_HW_ProgModeOff(void)  // programming voltage off, clock off, data high-z
{
    PIC_HW_SetClockAndData(false, false);

    PIC_HW_SetClockEnable(false );   // Set the tristate buffers to 'high-impedance'
    PIC_HW_SetDataEnable( false );   // (up to now, only required for AN589 programmer)

    if(PIC_HW_interface.type==PIC_INTF_TYPE_JDM2)
    {
        // For the "JDM 2" programmer (the one with the charge-pump),
        //     things are very different :
        // Here, TXD(Vpp), DTR(DataOut), and RTS(Clock) but be set HIGH(!)
        //     to turn the PIC's supply voltage off (!!) :
        PIC_HW_SetClockAndData( true, true );
        PIC_HW_SetVpp( true );   // Yes, indeed true to turn the PIC's supply voltage off !
    }
    else  // Not JDM... for all other interfaces : turn everything OFF by setting it LOW :
    {
        PIC_HW_SetVpp( false );  // turn programming voltage off (maybe = RESET !)
        PIC_HW_SetVdd( false );  // turn PIC supply voltage off
    }
} // end PIC_HW_ProgModeOff()


//----------------------------------------------------------------------------
void PIC_HW_SerialOut_14Bit(uint16_t w)
{
    int b;
    PIC_HW_SetClockEnable( true );
    PIC_HW_SetDataEnable( true );
    PIC_HW_ClockOut(0);
    for ( b=0; b<14; ++b )
        PIC_HW_ClockOut(w&(1<<b));
    PIC_HW_ClockOut(0);
} // end PIC_HW_SerialOut_14Bit()

//----------------------------------------------------------------------------
void PIC_HW_SerialOut_Command6(int cmd , bool fFlush )
// Sends a 6-bit ICSP command to the PIC (NOT TIME-CRITICAL)
// fFlush:  //-- Not used inWxPic
//          true = tell "smart" programmers to flush the queue;
//          false= no need to flush the queue yet, because an additional
//                 data word (typically with 14 bits) will follow.
//    The fFlush-option was added 2008-05-19, reasons explained in
//    PIC_HW.CPP;  PicHw_FlushCommand() .
{
    int b;

    PIC_HW_SetClockAndData(false,false );  // start condition
    PIC_HW_SetClockEnable( true );
    PIC_HW_SetDataEnable( true );   // "Enable" signals only for AN589 programmer
    if( Config.iSlowInterface )
        PIC_HW_Delay_us(10 * PIC_DeviceInfo.lTi_Clock_us + Config.iExtraClkDelay_us ); // test for "bad" interfaces
    else PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Clock_us + Config.iExtraClkDelay_us );      // normal mode for "good" interfaces
    for ( b=0; b<6; ++b )
        PIC_HW_ClockOut(cmd&(1<<b));
    PIC_HW_SetClockAndData(false, true );  // data input not driven for 1us
//  if( fFlush )
//   { PicHw_FlushCommand( cmd,6,  0,0/*no data*/  );
//   }

    // Wait <TDLY1> from data sheet.  "PIP02" does not seem to do this !!
    // Note: The "JDM programmer" needs a longer clock-low-time
    //       to charge C3 to 5 volts again !
    if( Config.iSlowInterface )
        PIC_HW_Delay_us(10 * PIC_DeviceInfo.lTi_Clock_us + Config.iExtraClkDelay_us ); // "bad" interfaces
    else PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Clock_us + Config.iExtraClkDelay_us );      // "good" interfaces
    if( PIC_HW_interface.type==PIC_INTF_TYPE_JDM2 )
    {
        //    Some additional delay to recharge C3 (22uF) in the JDM programmer.
        PIC_HW_Delay_us( 20 /*microseconds to recharge C3 */ );
    } // end if < "JDM" programmer >
} // end PIC_HW_SerialOut_Command6()


//----------------------------------------------------------------------------
uint16_t PIC_HW_SerialRead_14Bit(void)
{
    int b, w;
    PIC_HW_SetClockEnable( true );
    PIC_HW_SetDataEnable( false );   // disable output buffer for the serial DATA line (TO PIC)
    PIC_HW_ClockIn();
    for ( w=0, b=0; b<14; ++b )
        w += PIC_HW_ClockIn()<<b;
    PIC_HW_ClockIn();
    PIC_HW_SetClockAndData(false,false);  // DATA OUTPUT now also low
    PicHw_FeedChargePump();  // required to produce Vpp with a charge pump (JDM)
    return w;
} // end PIC_HW_SerialRead_14Bit()



//--------------------------------------------------------------------------
//  Module initialisation and -cleanup
//  ( remnant from old "C"-program )
//--------------------------------------------------------------------------


/***************************************************************************/
bool PIC_HW_Init(void)
/*
 * Initializes the PIC Programmer's hardware interface.
 * The "default" interface (taken from Config) is "opened" if required.
 */
{
//   PHWInfo.iSlowInterface = Config.iSlowInterface;
    PicHw_fVppIsOn = PicHw_fVddIsOn = PicHw_fMclrPulledToGnd = false;
    PicHw_fClockIsHigh = PicHw_fDataOutIsHigh = false;
    PicHw_fClockIsEnabled = PicHw_fDataIsEnabled = PicHw_fTogglingTxD = false;
    PicHw_iPresentVddSelection /*PHWInfo.iPresentVddSelection*/ = 1;  // 0=low, 1=norm, 2=high supply voltage
    PIC_HW_SingleTimingLoopCalibration();  // for 50- and 500ns-delay

    return PIC_HW_SetInterfaceType( Config.pic_interface_type );

} // end ..Init()


/***************************************************************************/
void PIC_HW_Close(void)
/* Cleanup after work, for example close open handles.
 * (don't rely on destructor of the main form !!)
 */
{
    PIC_HW_DisconnectFromTarget(); // since 2002-09-09
    PIC_HW_SetVpp( false );        // turn programming voltage off
    PIC_HW_SetVdd( false );        // turn PIC supply voltage off

    // if a COM-port has  been opened; restore the "old" settings & close it
    COM_ClosePicPort();

    LPT_ClosePicPort();  // same for the parallel port, etc

//  PicHw_UnloadInterfaceDLL();

//  SmallPort.CloseDriver();
}


//---------------------------------------------------------------------------
void RedAndGreenLedOff(void)
{
    PIC_HW_SetRedLed( 0 );
    PIC_HW_SetGreenLed( 0 );
}


//---------------------------------------------------------------------------
void UpdateLedsForResult(bool fOk)
{
    PIC_HW_SetRedLed( !fOk );
    PIC_HW_SetGreenLed( fOk );
}



/* EOF <pic_hw.cpp> */
