// Copyright (C) 2009 Philippe Chevrier <pch@laposte.net>
// Permission is granted to use, modify, or redistribute this software
// so long as it is not sold or exploited for profit.

#include "Appl.h"
#include <wx/msgdlg.h>
#include <wx/datetime.h>
#include <wx/strconv.h>
#include "MainFrame.h"

// Options and parameters from the command line .
//  Initialized with their "default values" (if not specified)
long APPL_i32CustomizeOptions = 0;
int  APPL_iUserBreakFlag = 0;  // signal for any programming loop to "stop" ,
                               // set by pressing ESCAPE .
wxCSConv Iso8859_1(wxConvISO8859_1);

void APPL_ShowMsg( int caller, int error_level, const char *pszFormat, ... )
  /* The application's "error-display" routine.
   * Also called from the PIC - routines (and other, possibly "C"-modules),
   *      therefore **NOT** a method of any CLASS.
   */
{
    va_list parameter;
    char szText[512];

    va_start( parameter, pszFormat );
    _vstprintf( szText, pszFormat, parameter );
    va_end(parameter);

    (void)caller;

    MainFrame::SetStatusText(szText);
    if(  (error_level>=127/*important*/)  /*&&  (ToolForm!=NULL)*/  )
        wxMessageBox(szText);

    APPL_LogEvent(szText);

} // end APPL_ShowMsg()


//---------------------------------------------------------------------------
void APPL_LogEvent( const char * pszFormat, ... )  // logs an "event" with current timestamp
{
  va_list arglist;
  char sz500[512];
  char *cp;

  // Show the TIME of this event :
  wxDateTime Now = wxDateTime::UNow();
  _stprintf(sz500, _("%02d:%02d:%02d.%02d "), Now.GetHour(), Now.GetMinute(), Now.GetSecond(), Now.GetMillisecond());
  cp = sz500+_tcslen(sz500);

  // Print to string and append to edit control
  va_start(arglist, pszFormat);
  _vstprintf(cp, pszFormat, arglist);
  va_end(arglist);

//  _tcscat(cp, _("\n"));

  MainFrame::AddTextToLog(sz500);
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
//        if (!PicMain->m_progress_visible)
//      {
//        PicMain->m_progress_visible    = true;
//        PicMain->ProgressBar1->Visible = true;
//        if(ToolForm)
//           ToolForm->ProgressBar1->Visible = true;
//      }
//     PicMain->m_progress_activity_timer = 2;
//     Appl_CheckUserBreak(); // -> APPL_iUserBreakFlag , here updated in APPL_ShowProgress()
//     PicMain->ProgressBar1->Position = percent;
//     if(ToolForm)
//        ToolForm->ProgressBar1->Position = percent;
//     PicHw_LetInterfaceDLLDoGraphicStuff();  // keep the plugin's GUI alive
//     PicMain->Update();
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



long HexStringToLongint(int nMaxDigits, const char *pszSource)
{
 long lResult = 0;
 char c;
 int i;
  if(pszSource[0]==wxT('0') && pszSource[1]==wxT('x'))
     pszSource += 2;
  else if(pszSource[0]==wxT('$'))
     pszSource += 1;

  for(i=0; i<nMaxDigits; ++i)
   {
     c=pszSource[i];
     if(c==wxT('\0') || c==wxT(' ') || c==wxT(',') )  // "early" end of the HEX string
        return lResult;

     lResult <<= 4;  // shift "older, upper digits" 4 bits left
     if(c>=wxT('0') && c<=wxT('9'))
        lResult += (c-wxT('0'));
     else
     if(c>=wxT('a') && c<=wxT('f'))
        lResult += (c-wxT('a')+10);
     else
     if(c>=wxT('A') && c<=wxT('F'))
        lResult += (c-wxT('A')+10);
     else
        return -1;  // not a valid HEX digit
   }
  return lResult;
} // end HexStringToLongint()



wxString &Iso8859_1_TChar (const char* psz, wxString &Dst)
{
#ifdef _UNICODE
    int Length = Iso8859_1.ToWChar(NULL, 0, psz);
    char *DstChar = Dst.GetWriteBuf(Length);
    Iso8859_1.ToWChar(DstChar, Length, psz);
    Dst.UngetWriteBuf(Length-1);
#else
    Dst = psz;
#endif
    return Dst;
}

void CopyIso8859_1_TChar (char *Dst, const char* psz, int Length)
{
#ifdef _UNICODE
    Iso8859_1.ToWChar(Dst, Length, psz);
#else
    strncpy(Dst, psz, Length);
#endif
}

char *DupIso8859_1_TChar (const char* psz)
{
#ifdef _UNICODE
    int Length = Iso8859_1.ToWChar(NULL, 0, psz);
    char *DstChar = (char*)malloc(Length*sizeof(char));
    Iso8859_1.ToWChar(DstChar, Length, psz);
    return DstChar;
#else
    return strdup(psz);
#endif
}