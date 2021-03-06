/*-------------------------------------------------------------------------*/
/*  Filename: Appl.cpp                                                     */
/*                                                                         */
/*  Purpose:                                                               */
/*     Miscelaneous general usage function created for adaptation of       */
/*     the originalm WinPic to the WxPic new structure and framework       */
/*                                                                         */
/*  Author:                                                                */
/*     Copyright 2009-2011 Philippe Chevrier pch @ laposte.net             */
/*     from software originally written by Wolfgang Buescher (DL4YHF)      */
/*                                                                         */
/*  License:                                                               */
/*     Licensed under GPLV3 conditions                                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#include "Appl.h"
#include <wx/msgdlg.h>
#include <wx/datetime.h>
#include <wx/strconv.h>
#include <wx/arrstr.h>
#include "MainFrame.h"

// Options and parameters from the command line .
//  Initialized with their "default values" (if not specified)
long APPL_i32CustomizeOptions = 0;
int  APPL_iUserBreakFlag = 0;  // signal for any programming loop to "stop" ,
                               // set by pressing ESCAPE .
wxCSConv Iso8859_1(wxConvISO8859_1);

void APPL_ShowMsg( int error_level, const wxChar *pszFormat, ... )
  /* The application's "error-display" routine.
   * Also called from the PIC - routines (and other, possibly "C"-modules),
   *      therefore **NOT** a method of any CLASS.
   */
{
    va_list parameter;
    wxString Text;

    va_start( parameter, pszFormat );
    Text.PrintfV(pszFormat, parameter );
    va_end(parameter);

    MainFrame::SetStatusText(Text);
    if(  (error_level>=127/*important*/)  /*&&  (ToolForm!=NULL)*/  )
        wxMessageBox(Text);

    APPL_LogEvent(Text);

} // end APPL_ShowMsg()


//---------------------------------------------------------------------------
void APPL_LogEvent( const wxChar * pszFormat, ... )  // logs an "event" with current timestamp
{
  va_list arglist;
  wxString Text;

  // Print to string and append to edit control
  va_start(arglist, pszFormat);
  Text.PrintfV(pszFormat, arglist);
  va_end(arglist);

  // Show the TIME of this event :
  wxDateTime Now = wxDateTime::UNow();
  wxString LogText;
  LogText.Printf(_("%02d:%02d:%02d.%02d %s"), Now.GetHour(), Now.GetMinute(), Now.GetSecond(), Now.GetMillisecond(), Text.c_str());

//  _tcscat(cp, _("\n"));

  MainFrame::AddTextToLog(LogText);
}


/***************************************************************************/
void APPL_ShowProgress( int percent )
  /* The application's "progress indicator" routine.
   * Also called from the PIC - routines (and others),
   *      therefore **NOT** a method of any CLASS.
   */
{
    // Avoid unnecessary updates because
    //  we are calling "unknown", possibly slow, Win API routine here .
    static int APPL_iPreviousProgressPercentage = 0;
    if( APPL_iPreviousProgressPercentage != percent )
    {
        APPL_iPreviousProgressPercentage = percent;
        MainFrame::ShowProgress(percent);
   }
} // end APPL_ShowProgress()


/***************************************************************************/
int Appl_CheckUserBreak(void)
  // returns true if user break detected (here: ESCAPE key pressed),
  //         false otherwise.
{
//  if ( GetAsyncKeyState( VK_ESCAPE) )
    if (wxGetKeyState(WXK_ESCAPE))
      APPL_iUserBreakFlag = 1;  // signal for the programming routines to leave all loops
  //  Return Values of GetAsyncKeyState() :
  //     If the function succeeds, the return value specifies whether the
  //     key was pressed since the last call to GetAsyncKeyState,
  //     and whether the key is currently up or down.
  //  If the most significant bit is set, the key is down,
  //  and if the least significant bit is set, the key was pressed
  //  after the previous call to GetAsyncKeyState.
  //  The return value is zero if a window in another thread or process
  //  currently has the keyboard focus.


  return APPL_iUserBreakFlag;
} // end Appl_CheckUserBreak()



bool HexStringToLongint(int nMaxDigits, const wxChar *pszSource, uint32_t *pValue)
{
 uint32_t lResult = 0;
 wxChar c;
 int i;
  if(pszSource[0]==_T('0') && ((pszSource[1]==_T('x')) || (pszSource[1]==_T('X'))))
     pszSource += 2;
  else if ((pszSource[0]==_T('$'))
        || (pszSource[0]==_T('H'))
        || (pszSource[0]==_T('h')))
     pszSource += 1;

  for(i=0; ; ++i, ++pszSource)
   {
     c=*pszSource;
     if ( c==_T(' ') || c==_T(',') || c==_T('h') || c==_T('H') )  // "early" end of the HEX string
     {
        while (*++pszSource == _T(' '))
            ;
        c = *pszSource;
        if (c != _T('\0'))
            return false;
     }
     if (c==_T('\0'))
     {
        *pValue = lResult;
        return true;
     }

     if (i > nMaxDigits)
        return false;

     lResult <<= 4;  // shift "older, upper digits" 4 bits left
     if(c>=_T('0') && c<=_T('9'))
        lResult += (c-_T('0'));
     else
     if(c>=_T('a') && c<=_T('f'))
        lResult += (c-_T('a')+10);
     else
     if(c>=_T('A') && c<=_T('F'))
        lResult += (c-_T('A')+10);
     else
        return false;  // not a valid HEX digit
   }
} // end HexStringToLongint()


wxString StringJoin (const wxArrayString &pArray, const wxString &pSep)
{
    wxString Result;
    int Count = pArray.Count();
    if (Count)
    {
        int Size = (Count-1) * pSep.Len();
        for (int i = 0; i < Count; ++i)
            Size += pArray[i].Length();
        Result.Alloc(Size);
        for (int i = 0; i < Count; ++i)
        {
            if (i > 0)
                Result += pSep;
            Result += pArray[i];
        }
    }
    return Result;
}


wxString &Iso8859_1_TChar (const char* psz, wxString &Dst)
{
#ifdef _UNICODE
    int Length = Iso8859_1.ToWChar(NULL, 0, psz);
    wxChar *DstChar = Dst.GetWriteBuf(Length);
    Iso8859_1.ToWChar(DstChar, Length, psz);
    Dst.UngetWriteBuf(Length-1);
#else
    Dst = psz;
#endif
    return Dst;
}

void CopyIso8859_1_TChar (wxChar *Dst, const char* psz, int Length)
{
#ifdef _UNICODE
    Iso8859_1.ToWChar(Dst, Length, psz);
#else
    strncpy(Dst, psz, Length);
#endif
}

wxChar *DupIso8859_1_TChar (const char* psz)
{
#ifdef _UNICODE
    int Length = Iso8859_1.ToWChar(NULL, 0, psz);
    wxChar *DstChar = (wxChar*)malloc(Length*sizeof(wxChar));
    Iso8859_1.ToWChar(DstChar, Length, psz);
    return DstChar;
#else
    return strdup(psz);
#endif
}

#ifndef __WXMSW__	//Posix Port emulation layer by E.U.A ;)
void QueryPerformanceCounter( int64_t* cnt ){
	timespec clk;
	clock_gettime( CLOCK_MONOTONIC, &clk );
	*cnt = clk.tv_nsec +clk.tv_sec*1000000000ll;
	}

void QueryPerformanceFrequency( int64_t* frq ){
	*frq = 1000000000LL;
	}

	long GetTickCount(){
		timespec tm;
		clock_gettime( CLOCK_MONOTONIC, &tm ) ;
		return tm.tv_sec*1000+tm.tv_nsec/1000000;
	}

	bool EscapeCommFunction( int fd, int flag){
		int status;
		ioctl(fd, TIOCMGET, &status);//Read status register
		if( flag == SETDTR ){ status |= TIOCM_DTR; }
		else if( flag == CLRDTR ){ status &= ~TIOCM_DTR; }
		else if( flag == SETRTS ){ status |= TIOCM_RTS; }
		else if( flag == CLRRTS ){ status &= ~TIOCM_RTS; }
		else if( flag == SETBREAK ){
			 ioctl (fd, TIOCSBRK, 0);
			 return true;
			 }
		else if( flag == CLRBREAK ){
			 ioctl (fd, TIOCCBRK, 0);
			 return true;
			 }
		else if( flag == MS_CTS_ON ){ status |= TIOCM_CTS; }
		ioctl(fd, TIOCMSET, &status);//Write
		return true;
		}

	bool GetCommModemStatus( int fd, uint32_t *flag ){
		return ioctl(fd, TIOCMGET, flag)==0;
		}
	uint8_t ReadIoPortByte(uint16_t addr){
		return inb(addr);
		}
	void WriteIoPortByte(uint16_t addr,uint16_t data){
		outb(data,addr);
		}
#endif

