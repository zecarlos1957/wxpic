/*-------------------------------------------------------------------------*/
/* MainFrame-2.cpp                                                         */
/*                                                                         */
/*  Purpose:                                                               */
/*       Manage the WxPic Main Window                                      */
/*       Separate from MainFrame.cpp that has been split                   */
/*       to avoid managing files > 2000 lines                              */
/*       All functions that don't managed window events are here           */
/*                                                                         */
/*  Author:                                                                */
/*       Copyright 2009 Philippe Chevrier pch@laposte.net                  */
/*       from software originally written by Wolfgang Buescher (DL4YHF)    */
/*                                                                         */
/*  License:                                                               */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#include "MainFrame.h"
#include "CommandOption.h"
#include "Appl.h"

#include <wx/intl.h>
#include <wx/string.h>
#include <wx/msgdlg.h>
#include <wx/datetime.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/settings.h>
#include <wx/file.h>
#include <wx/tokenzr.h>
#include <WinPicPr/PIC_HEX.h>
#include <WinPicPr/PIC_HW.h>
#include <WinPicPr/PIC_PRG.h>
#include <WinPicPr/dsPIC_PRG.h>
#include <WinPicPr/PIC16F7x_PRG.h>
#include <WinPicPr/PIC10F_PRG.h>
#include <WinPicPr/PIC18F_PRG.h>
#include <WinPicPr/Config.h>

#define MAX_MESSAGES_IN_LOG     500


/*static*/ bool MainFrame::CreateAndShow (void)
{
    TheMainFrame = new MainFrame();
    //-- Overwrite the loaded configuration with the command parameters
    if (!CommandOption.Load(wxTheApp))
        return false;


    // Initialize the memory buffers and other stuff...
    if ( ! PIC_HEX_Init() ) // Initialize buffers for program + data memory  (#1)
    {
        TheMainFrame->Close();     // buffer-init failed ? Almost impossible under windows !
        return false;
    }
    //-- Initialise the interface panel
    TheMainFrame->aInterfaceTab->UpdateInterfaceType();
    PIC_PRG_Init();  // Set default PIC type and type-dependent infos (#2)
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("CreateForm: Setting PIC-Device") );

    TDeviceCfgPanel::SetDevice(Config.sz40DeviceName);

    PIC_HEX_ClearBuffers(); // contents of some memory buffers depends on PIC_DeviceInfo !

    if ( CommandOption.WinPic_i32CmdLineOption_OverrideConfigWord >= 0)
        // override config word if specified in command line:
        PicBuf_SetConfigWord(0, CommandOption.WinPic_i32CmdLineOption_OverrideConfigWord ) ;


    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("CreateForm: Initialising help system") );

    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("CreateForm: done") );

    TheMainFrame->Show();
    return true;
}


void MainFrame::ShowProgressGauge(bool visible)
{
    if (aProgressGauge->IsShown() != visible)
    {
        aProgressGauge->Show(visible);
        Layout();
        wxYieldIfNeeded(); //Will draw changes to main screen
    }
}



/*static*/
void MainFrame::SetStatusText (const wxString &Text)
{
    TheMainFrame->aStatusBar->SetStatusText(Text);
}


/*static*/
void MainFrame::AddTextToLog (const wxChar *szText)
{
    int &Count = TheMainFrame->aLogCount;
    if (Count <= MAX_MESSAGES_IN_LOG)
    {
        const wxChar *LogText = (Count==MAX_MESSAGES_IN_LOG)
                               ? _("< Too many messages in list, click \"CLEAR\" >")
                               : szText;
        TheMainFrame->doAddTextLog(LogText);
        ++Count;
    }
}

void MainFrame::doAddTextLog (const wxChar *szText)
{
    aMessageTab->aMessageLog->AppendText(wxString(szText)+_T("\n"));
}


/*static*/
void MainFrame::ShowProgress (int pPercentage)
{
    TheMainFrame->ShowProgressGauge(true);
    TheMainFrame->aProgressGauge->SetValue(pPercentage);
    TheMainFrame->Update();
}

//---------------------------------------------------------------
//---------------------------------------------------------------

void MainFrame::SaveLayout (void)
{
    /* Save some windows stuff like old screen position & size.              */
    TheIniFile.SetPath(_T("/Layout"));
    wxRect WindowRect = GetRect();
    TheIniFile.Write(_T("Left"),  WindowRect.GetLeft  ());
    TheIniFile.Write(_T("Top"),   WindowRect.GetTop   ());
    TheIniFile.Write(_T("Width"), WindowRect.GetWidth ());
    TheIniFile.Write(_T("Height"),WindowRect.GetHeight());
    TheIniFile.Write(_T("CodeMemBgColor"), aCodeMemTab->aCodeMemGrid->GetDefaultCellBackgroundColour().GetAsString(wxC2S_HTML_SYNTAX));
    TheIniFile.Write(_T("CodeMemFgColor"), aCodeMemTab->aCodeMemGrid->GetDefaultCellTextColour().GetAsString(wxC2S_HTML_SYNTAX));
    TheIniFile.Write(_T("DataMemBgColor"), aDataMemTab->aDataMemGrid->GetDefaultCellBackgroundColour().GetAsString(wxC2S_HTML_SYNTAX));
    TheIniFile.Write(_T("DataMemFgColor"), aDataMemTab->aDataMemGrid->GetDefaultCellTextColour().GetAsString(wxC2S_HTML_SYNTAX));

    // The "most recent file" list is only a part of the user interface:
    TheIniFile.SetPath(_T("/MostRecentFiles"));
    for (int i=0; i<=5; ++i)
        TheIniFile.Write(wxString::Format(_T("file%d"),i), GetMRFname(i));
}

/**static*/
void MainFrame::ChangeDefaultGridForegroundColour(wxGrid *Grid, const wxColour &NewForegroundColour)
{
    wxColor OldDefault = Grid->GetDefaultCellTextColour();
    int RowCount = Grid->GetNumberRows();
    int ColCount = Grid->GetNumberCols();
    for (int Row=0; Row<RowCount; ++Row)
        for (int Col=0; Col<ColCount; ++Col)
            if (Grid->GetCellTextColour(Row, Col) == OldDefault)
                Grid->SetCellTextColour(Row, Col, NewForegroundColour);
}

/**static*/
void MainFrame::ChangeDefaultGridBackgroundColour(wxGrid *Grid, const wxColour &NewBackgroundColour)
{
    wxColor OldDefault = Grid->GetDefaultCellBackgroundColour();
    int RowCount = Grid->GetNumberRows();
    int ColCount = Grid->GetNumberCols();
    for (int Row=0; Row<RowCount; ++Row)
        for (int Col=0; Col<ColCount; ++Col)
            if (Grid->GetCellBackgroundColour(Row, Col) == OldDefault)
                Grid->SetCellBackgroundColour(Row, Col, NewBackgroundColour);
}

void MainFrame::LoadLayout (void)
{
//   IniFile=new wxConfig(ExtractFilePath(Application->ExeName)+CFG_INI_FILE_NAME);
    TheIniFile.SetPath(_T("/Layout"));
    SetSize(TheIniFile.Read(_T("Left"),   -1),
            TheIniFile.Read(_T("Top"),    -1),
            TheIniFile.Read(_T("Width"),  -1),
            TheIniFile.Read(_T("Height"), -1));

    wxColour Color;
    if (Color.Set(TheIniFile.Read(_T("CodeMemBgColor"), _T("#000000"))))
        ChangeDefaultGridForegroundColour(aCodeMemTab->aCodeMemGrid, Color);
    if (Color.Set(TheIniFile.Read(_T("CodeMemFgColor"), _T("#33FF33"))))
        ChangeDefaultGridForegroundColour(aCodeMemTab->aCodeMemGrid, Color);
    if (Color.Set(TheIniFile.Read(_T("DataMemBgColor"), _T("#FFFFFF"))))
        ChangeDefaultGridForegroundColour(aDataMemTab->aDataMemGrid, Color);
    if (Color.Set(TheIniFile.Read(_T("DataMemFgColor"), _T("#000000"))))
        ChangeDefaultGridForegroundColour(aDataMemTab->aDataMemGrid, Color);


    // The "most recent file" list is only a part of the user interface:
    TheIniFile.SetPath(_T("/MostRecentFiles"));
    for (int i=0; i<=5; ++i)
        SetMRFname( i, aEmptyMRFname );
    for (int i=0; i<=5; ++i)
        AddMRFname( TheIniFile.Read(wxString::Format(_T("file%d"),i), aEmptyMRFname) );

}


//---------------------------------------------------------------------------
wxString MainFrame::GetMRFname(int iMRFindex)
{
    switch (iMRFindex)
    {
    case 0:
        return aRecentFile1MenuItem->GetLabel();
    case 1:
        return aRecentFile2MenuItem->GetLabel();
    case 2:
        return aRecentFile3MenuItem->GetLabel();
    case 3:
        return aRecentFile4MenuItem->GetLabel();
    case 4:
        return aRecentFile5MenuItem->GetLabel();
    case 5:
        return aRecentFile6MenuItem->GetLabel();
    default:
        return aEmptyMRFname;
    }
} // end GetMRFname()


void MainFrame::SetMRFname(int iMRFindex, wxString s)
{
    switch (iMRFindex)
    {
    case 0:
        aRecentFile1MenuItem->SetText(s);
        aRecentFile1MenuItem->Enable(s!=aEmptyMRFname);
        break;
    case 1:
        aRecentFile2MenuItem->SetText(s);
        aRecentFile2MenuItem->Enable(s!=aEmptyMRFname);
        break;
    case 2:
        aRecentFile3MenuItem->SetText(s);
        aRecentFile3MenuItem->Enable(s!=aEmptyMRFname);
        break;
    case 3:
        aRecentFile4MenuItem->SetText(s);
        aRecentFile4MenuItem->Enable(s!=aEmptyMRFname);
        break;
    case 4:
        aRecentFile5MenuItem->SetText(s);
        aRecentFile5MenuItem->Enable(s!=aEmptyMRFname);
        break;
    case 5:
        aRecentFile6MenuItem->SetText(s);
        aRecentFile6MenuItem->Enable(s!=aEmptyMRFname);
        break;
    default:
        break;
    }
} // end SetMRFname()


void MainFrame::AddMRFname(wxString s)
{
    int i;
    for (i=0; i<=5; ++i)
    {
        if ( s == GetMRFname(i) )
            return;    // name is already in the list; no need to add it
    }
    for (i=0; i<=5; ++i)
    {
        if ( GetMRFname(i) == aEmptyMRFname )
        {
            // found a free entry ..
            SetMRFname(i, s);
            return;
        }
    }
    // Arrived here: the list of most recent files is full, and it's a NEW name.
    // Scroll the list, so the oldest file disappears BELOW THE BOTTOM,
    //                  and the new file will be entered AT THE TOP.
    for ( i=4; i>=0; --i)
    {
        SetMRFname(i+1, GetMRFname(i) );
    }
    SetMRFname(0, s);

} // end AddMRFname()


//---------------------------------------------------------------------------
bool MainFrame::TestDelayRoutine(void)
{
    long i32StartTime, i32EndTime;
// struct time dos_time;
    wxDateTime DateTime;
    wxChar sz80[82];
    float fltMeasuredDelay;
    LONGLONG i64;

    // Test function of the delay routine :
    DateTime = wxDateTime::UNow();
    i32StartTime = DateTime.GetMillisecond() + 1000*DateTime.GetSecond();
    PIC_HW_Delay_us( 500000/*microseconds*/ );
    DateTime = wxDateTime::UNow();
    i32EndTime = DateTime.GetMillisecond() + 1000*DateTime.GetSecond();
    if (i32EndTime<i32StartTime)
        i32EndTime+= 60 * 1000;  // minute wrap
    fltMeasuredDelay = (float)(i32EndTime-i32StartTime) / 1000.0;
    QueryPerformanceFrequency( (LARGE_INTEGER *) &i64 );
    _stprintf(sz80, _("Testing: delay(500ms) took %.2f seconds, timer_freq=%.4f MHz ..."),
              fltMeasuredDelay,  (float)i64 * 1E-6 );
    // "gettime()" may have a resolution of only 0.1 seconds, so be tolerant :
    if (fltMeasuredDelay>=0.40 && fltMeasuredDelay<=0.60)
    {
        AddTextToLog( wxString(sz80) + _(" ok") );
        return true;
    }
    else
    {
        AddTextToLog( wxString(sz80) + wxString(_(" ERROR !")) );
        return false;
    }
} // end TestDelayRoutine()



//---------------------------------------------------------------------------
void MainFrame::DisconnectTarget(void)
{
    PIC_HW_DisconnectFromTarget(); // since 2002-09-09 . From Johan Bodin:
    // > This signal is used to *totally* disconnect the programmer
    // > from the target system (using relay(s) and/or CMOS analog switches
    // > or whatever).
    // > This eliminates the need to plug/unplug the programming connector
    // > for code test runs.
    // For parallel port interfaces, D7 is used to do the job. HIGH=disconnect(!)
    aInterfaceTab->UpdateInterfaceTestDisplay();
} // end MainFrame::DisconnectTarget()



//---------------------------------------------------------------------------
bool MainFrame::ReadPicAndDumpToFile(const wxChar *fn)
{
    bool ok;
    bool not_blank;

    RedAndGreenLedOff();

    m_iMessagePanelUsage = MP_USAGE_BUSY;
    m_fPicDeviceConflict = false;
    APPL_iUserBreakFlag = 0;
    ok = PIC_PRG_ReadAll(false, &not_blank); // ..(false) = not "blank check only"

    UpdateAllSheets(); // make the changes visible on the screen

    if ( !ok )
    {
        // reading not ok, for any strange reason it was "aborted" ...
        APPL_ShowMsg( 0, _( "Action aborted") );
        m_iMessagePanelUsage = MP_USAGE_ERROR;
        return false;
    }
    if (not_blank)
    {
        return DumpEverythingToHexFile(fn);
    }
    else
    {
        APPL_ShowMsg( 0, _("PIC is erased - nothing to dump") );
        m_iMessagePanelUsage = MP_USAGE_INFO;
        RedAndGreenLedOff();
        return false;
    }
} // end MainFrame::ReadPicAndDumpToFile()
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int PicPrg_GetNrOfConfigMemLocationsToWrite(void)
{
    int iNrWordsWriteable = PicDev_GetConfMemSize();
    if ( iNrWordsWriteable != (PicBuf[PIC_BUF_CONFIG].i32LastUsedArrayIndex+1) )
    {
        // If there are LESS config memory words in the device info
        //  than loaded from the hex file, show a warning (file may be fishy).
        // If there are MORE config memory words in the device info
        //  than loaded, it's ok, those at the end are often "not so important" .
        if ( iNrWordsWriteable > (PicBuf[PIC_BUF_CONFIG].i32LastUsedArrayIndex+1)  )
        {
            // Only program as many config mem words as "loaded", ignore the rest.
            // AT LEAST THE dsPIC DOES NOT ERASE THESE BITS, SO ITS 100% SAFE
            // TO LEAVE THEM "AS THEY ARE" !   ( ToDo: Check this for other devices)
            iNrWordsWriteable = PicBuf[PIC_BUF_CONFIG].i32LastUsedArrayIndex+1;
        }
        else // iNrWordsWriteable < (PicBuf[PIC_BUF_CONFIG].i32LastUsedArrayIndex+1) :
        {
            // There are more config mem words "loaded" than exist in the device ?!
            // ( a smart HEX load routine may already have signalled this, but ... )
            APPL_ShowMsg( 0, _("Problem: Buffer contains more CONFIG MEMORY WORDS (%d) than exist in device (%d) ."),
                          PicBuf[PIC_BUF_CONFIG].i32LastUsedArrayIndex+1, iNrWordsWriteable );
        }
    }
    return iNrWordsWriteable;
}


//---------------------------------------------------------------------------
bool MainFrame::ProgramPic(void)
{
    // programs the buffer contents into the PIC, and verifies (if possible)
    int error_count = 0;
    uint32_t dw;
    long i32FirstAddress, i32LastAddress;
    bool fProgramAll;
    bool fOkToGo = true;
    bool fResult = true;
    bool fDidSomething = false;
    wxChar sz80Temp[100];
    wxChar sz255Temp[256];
    const wxChar *pszMsg;
    uint32_t dwTemp4[4];


    APPL_iUserBreakFlag = 0;

    _stprintf( sz80Temp, _("Programming...") );
    m_iMessagePanelUsage = MP_USAGE_BUSY;
    APPL_ShowMsg( 0, sz80Temp  );


    PicPrg_iCodeMemErased   = 0;
    PicPrg_iCodeMemVerified = 0;  // 0 = not yet, 1=verified "ok", -1=error(s)
    PicPrg_iDataMemErased   = 0;
    PicPrg_iDataMemVerified = 0;
    PicPrg_iConfMemErased   = 0;
    PicPrg_iConfMemProgrammed=0;
    PicPrg_iConfMemVerified = 0;
    PicPrg_iConfWordProgrammed=0; // .. means the "classic" config word with readout protection.
    PicPrg_iConfWordVerified = 0;

    RedAndGreenLedOff();

    PIC_HW_ConnectToTarget();  // also HERE in MainFrame::ProgramPic() since 2003-12-28
    if ( PIC_HW_CanSelectVdd() )       // since 2005-09-29 :
    {
        PIC_HW_SelectVdd( 1/*norm*/ );  // use the "normal" voltage (=5V) for programming
    }

    fProgramAll = false;      // what are we going to do, "program EVERYTHING" or just a part ?
    if (  (Config.iProgramWhat==PIC_PROGRAM_ALL)
            ||( Config.iProgramWhat==(PIC_PROGRAM_CODE+PIC_PROGRAM_CONFIG)
                && (PIC_DeviceInfo.lDataEEPROMSizeInByte <= 0) )  // PICs w/o EEPROM ?
       )
    {
        fProgramAll = true;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // First optional step: bulk erase (all but CALIBRATION BITS where applicable)
    if ( fProgramAll
            && (Config.iUseCompleteChipErase)
            && (PIC_DeviceInfo.iCodeMemType==PIC_MT_FLASH)
            && (PIC_DeviceInfo.wCodeProgAlgo!=PIC_ALGO_PIC10F) // for PIC10F20x, erase is part of "ProgramAll" !
            && (PIC_DeviceInfo.wCodeProgAlgo!=PIC_ALGO_PIC16F7X) // ... similar "special case" for PIC16F74 etc
       )
    {
        fDidSomething = true;
        _tcscpy(sz80Temp, _("Erasing (\"bulk\" or \"chip\") ...") );
        APPL_ShowMsg( 0, sz80Temp );
        Update();
        if ( PIC_PRG_Erase( PIC_ERASE_ALL | PIC_SAVE_CALIBRATION ) ) // here in ProgramPic()
        {
            aStatusBar->SetStatusText(_("Erased"));
            PicPrg_iCodeMemErased   = 1;    // have erased CODE memory now
            PicPrg_iDataMemErased   = 1;    // have erased EEPROM DATA (if exists) too
            PicPrg_iConfMemErased   = 1;    // ..but not *necessarily* the config bits
        }
        else
        {
            pszMsg = _("Erasing FAILED !");
            APPL_ShowMsg( 0, pszMsg );
        }
        Update();
    } // end if <ok to erase the PIC ?>
    else
    {
        // no ERASE before PROGRAMMING ?!?
        // Impossible with many devices, except if the location ARE ALREADY CLEAR.
        // Most new chips do NOT support erasing individial cells
        // (which is one of the major differences between FLASH and EEPROM)
        // so trying to "program the chip" without prior erase
        // generally doesn't make sense .
        // Some PIC programming routines will always erase the chip (PIC16F7x for example).
    }


    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Second, optional, but usually the most important step: program CODE
    if (Config.iProgramWhat&PIC_PROGRAM_CODE)
    {
        // If there is a valid OSCILLATOR CALIBRATION WORD
        //   (which in fact is a RETLW at the end of the program memory)
        // smuggle it into the code memory buffer now:
        if (    (PIC_DeviceInfo.lAddressOscCal >= 0)
                && (!Config.iDontCareForOsccal)  )
        {
            // there SHOULD BE an oscillator calib word:
            if ( PicBuf_GetBufferWord(PIC_DeviceInfo.lAddressOscCal, &dw ) > 0)
            {
                if ( (PIC_lOscillatorCalibrationWord & 0xFF00) == 0x3400/*RETLW*/ )
                {
                    // have read a valid oscillator config word earlier, usually before bulk erase.
                    PicBuf_SetBufferWord(PIC_DeviceInfo.lAddressOscCal, PIC_lOscillatorCalibrationWord );
                    if (dw != (uint32_t)PIC_lOscillatorCalibrationWord)
                    {
                        APPL_ShowMsg( 0, _("Replaced OSCCAL word in buffer: old=0x%06lX, new=0x%06lX ."),
                                      dw, PIC_lOscillatorCalibrationWord );
                    }
                }
                else // there should be an oscillator calib word,
                {
                    // but PIC_lOscillatorCalibrationWord does not look like a RETLW instruction:
                    if ( (dw & 0xFF00) == 0x3400)
                    {
                        // The osc calib word in the program buffer is a RETLW instruction.. use it.
                        APPL_ShowMsg( 0, _("Using old OSCCAL word from buffer (0x%06lX) .") , dw );
                    }
                    else // there is still no valid osc calib word in the buffer ...
                    {
                        if (PIC_iHaveErasedCalibration)
                        {
                            PicBuf_SetBufferWord(PIC_DeviceInfo.lAddressOscCal, 0x3480/*RETLW 0x80*/ );
                            APPL_ShowMsg( 0, _("Warning: using default OSC CALIB word instead of 0x%06lX ."), dw );
                        }
                    }
                }
            } // end if < successfully read a "config word" >
            // If a BULK ERASE has been done before, make sure CODE PROGRAMMING
            // goes as far as the config word.
            // Without BULK ERASE, this is not required.
            if (PIC_iHaveErasedCalibration)
            {
                if (PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex < PIC_DeviceInfo.lAddressOscCal)
                    PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex = PIC_DeviceInfo.lAddressOscCal;
            }
        } // end if <need to care for the PIC's oscillator calib word ?>

        i32FirstAddress = PicBuf_ArrayIndexToTargetAddress(PIC_BUF_CODE, 0 );
        i32LastAddress  = PicBuf_ArrayIndexToTargetAddress(PIC_BUF_CODE, PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex);
        if ( PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex >= 0 )
        {
            fOkToGo = true;
            if (PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex >= PIC_DeviceInfo.lCodeMemSize)
            {
                _stprintf(sz80Temp, _("Warning: Last program addr (0x%06lX) exceeds device memory size (0x%06lX) !"),
                          PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex,
                          PIC_DeviceInfo.lCodeMemSize );
                APPL_ShowMsg( 0, sz80Temp );
                _stprintf(sz255Temp, _T("%s\n%s"), sz80Temp,
                          _("Select YES to ignore this problem and truncate the buffer contents.") );
                if (wxMessageBox(sz255Temp, _("Problem encountered before CODE MEMORY programming") ,
                                 wxICON_QUESTION | wxYES_NO | wxCANCEL ) == wxYES )
                {
                    PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex = PIC_DeviceInfo.lCodeMemSize;
                    fOkToGo = true;
                }
                else
                {
                    APPL_ShowMsg( 0, _("Skipped CODE MEMORY programming.") );
                    fOkToGo = false;
                }
            } // end if <problem with CODE MEMORY size>

            if (fOkToGo)
            {
                // Erase code memory  only  if not already "erased ALL" :
                if ( (Config.iProgramWhat!=PIC_PROGRAM_ALL) && (PIC_DeviceInfo.iCodeMemType==PIC_MT_FLASH)
                        && (PIC_DeviceInfo.wCodeProgAlgo!=PIC_ALGO_PIC10F) // for PIC10F20x, there is only "ProgramAll" !
                        && (PIC_DeviceInfo.wCodeProgAlgo!=PIC_ALGO_PIC16F7X) // ... similar for PIC16F74 ("ProgramAll")
                   )
                {
                    // If the option "program all" is NOT selected, must erase program memory here
                    // before programming it !
                    // However, there is the option NOT TO ERASE before programming, so :
                    pszMsg = _("Erasing CODE");
                    APPL_ShowMsg( 0, pszMsg );
//            if(ToolForm)
//                ToolForm->ShowMsg( pszMsg, TWMSG_NO_ERROR );
                    Update();
                    if ( ! PIC_PRG_Erase( PIC_ERASE_CODE | PIC_SAVE_CALIBRATION ) ) // here in ProgramPic(), "CODE" only
                    {
                        APPL_ShowMsg( 0, _("Erasing FAILED !") );
                        ++error_count;
                    }
                } // end if < not "program all" >
                if (PIC_DeviceInfo.wCanRead)
                {
                    _stprintf( sz80Temp, _("Programming and Verifying CODE, 0x%06lX..0x%06lX"),
                               i32FirstAddress, i32LastAddress );
                }
                else
                {
                    _stprintf( sz80Temp, _("Programming CODE, 0x%06lX..0x%06lX"),
                               i32FirstAddress, i32LastAddress );
                }
                APPL_ShowMsg( 0, sz80Temp );

                if ( PIC_DeviceInfo.iBitsPerInstruction==24 )
                {
                    // most likely a dsPIC30F (24 bits per instruction)
                    if (!PIC_PRG_iSimulateOnly )
                    {
                        if (! dsPIC_WriteCodeMemory( 0x000000L, // dwDestAddress,
                                                     PicBuf[PIC_BUF_CODE].pdwData , // uint32_t *pdwSourceData,
                                                     PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex+1)) // dwNrOfLocations
                        {
                            ++error_count;
                        }
                    }
                    fDidSomething = true;
                }
                else
                    if ( PIC_DeviceInfo.iBitsPerInstruction==16 )
                    {
                        // may be a PIC18Fxxxx ...
                        if (!PIC_PRG_iSimulateOnly )
                        {
                            if (! PIC18F_WriteCodeMemory( 0x000000L, // dwDestAddress,
                                                          PicBuf[PIC_BUF_CODE].pdwData ,  // uint32_t *pdwSourceData,
                                                          PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex+1)) // dwNrOfLocations
                            {
                                ++error_count;
                            }
                        }
                        fDidSomething = true;
                    }
                    else  // neither 24 nor 16 bits per instruction in CODE MEMORY ...
                        if ( PIC_DeviceInfo.iBitsPerInstruction==14 )
                        {
                            if ( PIC_DeviceInfo.wCodeProgAlgo == PIC_ALGO_PIC16F7X )
                            {
                                // PIC16F7x (PIC16F74): Totally different prog spec, see PIC16F7x_PRG.C !
                                if ( !PIC16F7x_ProgramAll( PIC_ACTION_ERASE | PIC_ACTION_WRITE ) )
                                {
                                    ++error_count;
                                }
                                PicPrg_iCodeMemVerified = 1;  // PIC16F7x_ProgramAll verifies while writing !
                                fDidSomething = true;
                            } // end if < PIC16F73, PIC16F74, PIC16F76, PIC16F77 >
                            else // everything else is HOPEFULLY not a "special case" / "extra module" :
                            {
                                if (!PIC_PRG_Program(   // only for 14-bit core !
                                            PicBuf[PIC_BUF_CODE].pdwData, // pointer to source data
                                            PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex+1, // number of words to be programmed
                                            PIC_DeviceInfo.lCodeMemWriteLatchSize, //-- Number of words per row
                                            0x3FFF, // mask for programming and verification
                                            PIC_DeviceInfo.iCmd_LoadProg, // 'load' command pattern
                                            PIC_DeviceInfo.iCmd_ReadProg, // 'read' command pattern (for verifying)
                                            0) ) // target start address
                                {
                                    ++error_count;
                                }
                                fDidSomething = true;
                            } // end else < no "special case" >
                        }
                        else // neither 14, 16, nor 24 bits per instruction in CODE MEMORY
                            if ( PIC_DeviceInfo.iBitsPerInstruction==12 )
                            {
                                // 12-bit-core : ONLY PIC10F20x supported by this version of WinPic ! ! !
                                if ( !PIC10F_ProgramAll( PIC_ACTION_ERASE | PIC_ACTION_WRITE ) )
                                {
                                    ++error_count;
                                }
                                PicPrg_iCodeMemVerified = 1;  // PIC10F_ProgramAll verifies while writing !
                                fDidSomething = true;
                            } // end else  -> neither 12, 14, 16, nor 24 bits per instruction in CODE MEMORY

                // If the CODE MEMORY hasn't been verified yet, do that now ...
                if ( PicPrg_iCodeMemVerified == 0 )
                {
                    // code memory not verified yet -> do that now :
                    _stprintf(sz80Temp, _("Verifying CODE, 0x%06lX..0x%06lX"),
                              i32FirstAddress, i32LastAddress );
                    APPL_ShowMsg( 0, sz80Temp );

                    if ( ! PicPrg_Verify( 0, // dwDeviceAddress
                                          PicBuf[PIC_BUF_CODE].pdwData, // uint32_t *pdwSourceData
                                          PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex+1, // number of words to be programmed
                                          0x3FFF, // mask for programming and verification (ignored for non-14-bit-core)
                                          PIC_DeviceInfo.iCmd_ReadProg)) // 'read' command pattern (for verifying)
                    {
                        ++error_count;
                    }
                } // end if < code memory not verified yet >
            } // end if (fOkToGo)
        }   // end if ( PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex >= 0 )
        else // no program in memory !
        {
            APPL_ShowMsg( 0, _("Cannot Program CODE MEMORY, nothing in buffer.") );
            m_iMessagePanelUsage = MP_USAGE_WARNING;
        } // end else < no program in memory >
    } // end if(Config.iProgramWhat&PIC_PROGRAM_CODE)

    if (APPL_iUserBreakFlag)
    {
        APPL_ShowMsg( 0, _("USER BREAK while programming.") );
        m_iMessagePanelUsage = MP_USAGE_WARNING;
        APPL_iUserBreakFlag = 0;
        return false;
    }


    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Third, optional step: program DATA EEPROM
    if ( ( Config.iProgramWhat&PIC_PROGRAM_DATA )
            &&(PIC_DeviceInfo.wCodeProgAlgo!=PIC_ALGO_PIC10F) // PIC10F20x has no data EEPROM !
       )
    {
        i32FirstAddress = PicBuf_ArrayIndexToTargetAddress(PIC_BUF_DATA, 0 );
        i32LastAddress  = PicBuf_ArrayIndexToTargetAddress(PIC_BUF_DATA, PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex );
        if ( PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex >= 0 )
        {
            fDidSomething = true;

            if ( (Config.iProgramWhat!=PIC_PROGRAM_ALL) && (PIC_DeviceInfo.iCodeMemType==PIC_MT_FLASH) )
            {
                // If the option "program all" is NOT selected, must erase program memory here
                // before programming it !
                // However, there is the option NOT TO ERASE before programming, so :
                _tcscpy(sz80Temp, _("Erasing: DATA ..") );
                APPL_ShowMsg( 0, sz80Temp );

                Update();
                if ( PIC_PRG_Erase( PIC_ERASE_DATA ) ) // here in ProgramPic(), "DATA" only
                {
                    APPL_ShowMsg( 0, _("Erasing FAILED !") );
                    ++error_count;
                }
            } // end if < not "program all" >

            _stprintf(sz80Temp, _("Programming DATA, 0x%06lX..0x%06lX"),
                      i32FirstAddress, i32LastAddress );
            APPL_ShowMsg( 0, sz80Temp );

            if ( ! PicPrg_WriteDataMemory() ) // Note: doesn't necessarily verify !
            {
                ++error_count;
                m_iMessagePanelUsage = MP_USAGE_WARNING;
            }

            // If the DATA MEMORY (EEPROM) hasn't been verified yet, do that now ...
            if ( PicPrg_iDataMemVerified == 0 ) // (some algo's verify while writing)
            {
                // code memory not verified yet -> do that now :
                _stprintf(sz80Temp, _("Verifying DATA, 0x%06lX..0x%06lX"),
                          i32FirstAddress, i32LastAddress );
                APPL_ShowMsg( 0, sz80Temp );

                if ( ! PicPrg_Verify( i32FirstAddress, // dwDeviceAddress
                                      PicBuf[PIC_BUF_DATA].pdwData,  // pointer to source data
                                      PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex+1, // number of words to be verified
                                      0x3FFF, // mask for programming and verification (only for 14-bit-core)
                                      PIC_DeviceInfo.iCmd_ReadProg)) // 'read' command pattern (only for 14-bit-core)
                {
                    ++error_count;
                }
            } // end if < code memory not verified yet >

        }
    } // end   if(  (Config.iProgramWhat&PIC_PROGRAM_DATA)

    if (APPL_iUserBreakFlag)
    {
        APPL_ShowMsg( 0, _("USER BREAK while programming.") );
        m_iMessagePanelUsage = MP_USAGE_WARNING;
        APPL_iUserBreakFlag = 0;
        return false;
    }

    // Intermediate step (moved here 2006-07): - - - - - - - - - - - - - - - - >
    //  Restore the original BANDGAP calibration bits for some 14-bit devices
    //  in the config-memory-buffer, before writing the config memory / config word:
    if ( (Config.iDontCareForBGCalib==0)
            && (PIC_iHaveErasedCalibration )
            && (PIC_lBandgapCalibrationBits>=0)
            && (PIC_DeviceInfo.wCfgmask_bandgap!=0) )
    {
        // restore the bandgap calibration bits which have been read before bulk-erase ?
        APPL_ShowMsg( 0, _("Restoring BANDGAP calibration bits for CONFIG-WORD") );
        if (Config.iVerboseMessages)
        {
            _stprintf(sz80Temp, _("Cfg word before restoring BG calib: 0x%06lX"),
                      PicBuf_GetConfigWord(0) );
            APPL_ShowMsg( 0, sz80Temp );
        }
        PicBuf_SetConfigWord( 0,
                              (PicBuf_GetConfigWord(0) & ~PIC_DeviceInfo.wCfgmask_bandgap)
                              |(PIC_lBandgapCalibrationBits & PIC_DeviceInfo.wCfgmask_bandgap) );
        if (Config.iVerboseMessages)
        {
            _stprintf(sz80Temp, _("Cfg word after restoring BG calib: 0x%06lX"),
                      PicBuf_GetConfigWord(0) );
            APPL_ShowMsg( 0, sz80Temp );
        }
    } // end if < must restore BANDGAP calibration bits ? >


    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Fourth, optional step: program CONFIGURATION REGISTERS, and possibly "ID-Locations".
    //  ( after CODE + DATA, since this may already set the CP bits in PIC18F + dsPIC30F.
    //    For 14-bit core, this does NOT include the "classic config word", see below )
    // Note: For most(?) chips, the CONFIGURATION memory can be programmed without prior erase.
    if (  (PIC_DeviceInfo.wCodeProgAlgo!=PIC_ALGO_PIC10F) // for PIC10F20x, everything is in "ProgramAll" !
            && (PIC_DeviceInfo.wCodeProgAlgo!=PIC_ALGO_PIC16F7X) // ... similar for PIC16F74 ("ProgramAll")
       )
    {
        if (  (Config.iProgramWhat & PIC_PROGRAM_CONFIG)
                && (PicBuf[PIC_BUF_CONFIG].i32LastUsedArrayIndex >= 0 )
           )
        {
            int iNrWordsWriteable = PicPrg_GetNrOfConfigMemLocationsToWrite();
            fDidSomething |= (iNrWordsWriteable>0);
            i32FirstAddress = PicBuf_ArrayIndexToTargetAddress(PIC_BUF_CONFIG, 0 );
            i32LastAddress  = PicBuf_ArrayIndexToTargetAddress(PIC_BUF_CONFIG, iNrWordsWriteable-1 );
            _stprintf( sz80Temp, _("Programming CONFIG, 0x%06lX..0x%06lX"),
                       i32FirstAddress, i32LastAddress );
            APPL_ShowMsg( 0, sz80Temp );
            if ( ! PicPrg_WriteConfigRegs( // Note: doesn't necessarily verify !
                        PIC_DeviceInfo.lConfMemBase, // uint32_t dwDestAddress
                        PicBuf[PIC_BUF_CONFIG].pdwData, // uint32_t *pdwSourceData
                        iNrWordsWriteable) ) // uint32_t dwNrOfRegisters
            {
                ++error_count;
                m_iMessagePanelUsage = MP_USAGE_WARNING;
            }

            if ( !PicPrg_iConfMemVerified ) // WriteConfigRegs() didn't verify yet:
            {
                // config memory not verified yet -> do that now :
                _stprintf( sz80Temp, _("Verifying CONFIG, 0x%06lX..0x%06lX"),
                           i32FirstAddress, i32LastAddress );
                APPL_ShowMsg( 0, sz80Temp );

                if ( ! PicPrg_Verify(  // Verify CONFIG MEMORY (including config word)
                            PIC_DeviceInfo.lConfMemBase,    // dwDeviceAddress
                            PicBuf[PIC_BUF_CONFIG].pdwData, // uint32_t *pdwSourceData
                            iNrWordsWriteable , // number of registers be verified
                            0x3FFF, // bitmask for verification (14-bit-core only)
                            PIC_DeviceInfo.iCmd_ReadProg)) // 'read' command pattern (only for 14-bit-core)
                {
                    ++error_count;
                }
            } // end if < config memory not verified yet >

        } // end if <option to program CODE memory> and <CONFIG memory present>
        else  // why not program configuration memory ?
        {
            if (  (Config.iProgramWhat & PIC_PROGRAM_CONFIG)
                    && (PicBuf[PIC_BUF_CONFIG].i32LastUsedArrayIndex < 0 ) )
            {
                // Suspicious: shall programm CONFIG memory (as usual),
                // but there is no entry in the CONFIG memory buffer  ?
                // Too bad for older chips where we have already "erased everything".
                // No problem for newer chips which do NOT erase configuration memory,
                //   not even when performing "bulk erase" (for example dsPIC30F) .
                APPL_ShowMsg( 0, _("Suspicious: Buffer contains no data for the CONFIGURATION BITS.") );
            }
        }
    } // end if < not PIC10F20x >


    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Fifth, VERY optional step, for 14-bit core only(*): program the CLASSIC CONFIG-WORD
    //    (code protection, watchdog, oscillator, bandgap calibration bits)
    if (  ( PIC_DeviceInfo.iBitsPerInstruction==14 )
            && ( ! PicPrg_iConfWordProgrammed )
            && (PIC_DeviceInfo.wCodeProgAlgo!=PIC_ALGO_PIC10F) // for PIC10F20x, already done in "ProgramAll"
            && (PIC_DeviceInfo.wCodeProgAlgo!=PIC_ALGO_PIC16F7X) // ... similar for PIC16F74 (all in "ProgramAll")
       )
    {
        // This must be done AFTER programming CODE and/or DATA because otherwise
        // verification during programming would be impossible with the CP bits set.
        //
        // For some 14-bit devices without "bandgap calibration", this may be
        // unnecessary if the "config word" has already been programmed as part
        // of step 4 ( "CONFIGURATION REGISTERS" ) .
        //
        // (*) 14-bit core only, because for PIC18F and dsPIC30F there is no
        //     "classic config word" with the code protection bits.
        //     Instead, these bits are spread all over the configuration MEMORY,
        //     so this fifth step is unnecessary.
        //     For some 14-bit devices, the "configuration memory"
        //     ( programmed in the previous step ) did NOT include the CP bits yet,
        //     because that way the "code memory" could not be verified !
        //
        if (  (PicPrg_iConfWordProgrammed==0)
                && ( Config.iProgramWhat & PIC_PROGRAM_CONFIG)!=0 )   // May I ... ?
        {
            _stprintf( sz80Temp, _("Programming CONFIG-WORD") );
            APPL_ShowMsg( 0, sz80Temp );

            if (Config.iVerboseMessages)
            {
                _stprintf(sz80Temp, _("Config word = 0x%06lX; Config mask = 0x%06lX"),
                          PicBuf_GetConfigWord(0) ,
                          PicPrg_GetConfigWordMask() );
                APPL_ShowMsg( 0, sz80Temp );
            }
            dwTemp4[0] = PicBuf_GetConfigWord(0);
            dwTemp4[1] = PicBuf_GetConfigWord(1);
            fDidSomething = true;
            if (!PIC_PRG_Program( dwTemp4, // program config word(s) :
                                  1 + // number of words to be programmed ...
                                  ((PIC_DeviceInfo.wCfgmask2_used==0x0000)?0:1),  // 2nd cfg word ?
                                  4,
                                  PicPrg_GetConfigWordMask(),   // mask for programming and verification (ex:0x3FFF) :
                                  PIC_DeviceInfo.iCmd_LoadProg, // 'load' command pattern
                                  PIC_DeviceInfo.iCmd_ReadProg, // 'read' command pattern (for verifying)
                                  PIC_DeviceInfo.lConfWordAdr)) // target start address
            {
                ++error_count;
            }

        } // end if (Config.iProgramWhat&PIC_PROGRAM_CONFIG) ..
    } // end if < "classic config word" for 12- or 14- bit PIC devices only >


    if (APPL_iUserBreakFlag)
    {
        APPL_ShowMsg( 0, _("USER BREAK while programming.") );
        m_iMessagePanelUsage = MP_USAGE_WARNING;
        APPL_iUserBreakFlag = 0;
        fResult = false;
    }
    else // no 'user break' during PROGRAM
    {
        if (error_count==0)
        {
            PIC_iHaveErasedCalibration = 0;   // some calibration value are back where they belong
            if (fDidSomething)
            {
                pszMsg = _("Programming finished, no errors.");
            }
            else
            {
                pszMsg = _("Programming finished, nothing to do.");
            }
            APPL_ShowMsg( 0, pszMsg );
            m_iMessagePanelUsage = MP_USAGE_INFO;
        }
        else
        {
            pszMsg = _("ERROR: Programming FAILED !");
            APPL_ShowMsg( 0, pszMsg  );
            Update();
            m_iMessagePanelUsage = MP_USAGE_ERROR;
            // highlight good or bad CODE LOCATIONS ?
        }
        fResult = (error_count==0);
    }


    if (Config.iDisconnectAfterProg)
        DisconnectTarget();  // since 2002-09-26 . Suggested by Johan Bodin.

    UpdateAllSheets(); // and make the changes visible on the screen

    UpdateLedsForResult( fResult );

    return fResult;
} // end MainFrame::ProgramPic()
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool MainFrame::VerifyPic(void)
{
    // compares the buffer contents against the PIC's different memories.
    int error_count = 0;
    bool fVerifyAtDifferentVoltages;
    bool fVoltageSwitchSupported;
    int iSupplyVoltageLoop;
    int iSupplyVoltageLevel; // 0=low supply voltage, 1=normal, 2=high voltage
    bool fDidSomething = false;
    const wxChar *pszMsg;
    uint32_t dwTemp4[4];
    long i32FirstAddress, i32LastAddress;


    APPL_iUserBreakFlag = 0;
    PicPrg_iCodeMemVerified = PicPrg_iDataMemVerified = 0;  // force verify
    PicPrg_iConfMemVerified = PicPrg_iConfWordVerified= 0;  // force verify

    fVerifyAtDifferentVoltages = false;
    if (Config.iVerifyAtDifferentVoltages)
    {
        if (PIC_HW_CanSelectVdd() )
            fVerifyAtDifferentVoltages = true;
        else
            APPL_ShowMsg( 0, _("Cannot verify at different voltages with this hardware.") );
    }

    RedAndGreenLedOff();
    if ( ! PicHw_iConnectedToTarget )
    {
        PIC_HW_ConnectToTarget();    // here (in MainFrame::VerifyPic) since 2005-12-17
    }

    for (iSupplyVoltageLoop = 0;
            iSupplyVoltageLoop < ( fVerifyAtDifferentVoltages ? 3 : 1 );
            ++iSupplyVoltageLoop)
    {

        switch (iSupplyVoltageLoop)
        {
        default:
        case 0:
            iSupplyVoltageLevel = 1; // first check at NOMINAL voltage ...
            if (fVerifyAtDifferentVoltages)
                pszMsg = _T("Verifying at nominal supply voltage...");
            else
                pszMsg = _T("Verifying...");
            break;
        case 1:
            iSupplyVoltageLevel = 0; // ... then check at LOW voltage ...
            pszMsg = _T("Verifying at low supply voltage...");
            break;
        case 2:
            iSupplyVoltageLevel = 2; // ... and finally check at HIGH voltage
            pszMsg = _T("Verifying at high supply voltage...");
            break;
        }
        // Select supply voltage for verify mode .
        // Since 2005-09-25, this routine is also used to check if the interface
        //       supports verify at a certain supply voltage at all.
        //       If PIC_HW_SelectVdd() returns false, the function may have failed
        //             or the interface may not support this voltage-switch at all.
        fVoltageSwitchSupported = PIC_HW_SelectVdd(iSupplyVoltageLevel);
        if ( iSupplyVoltageLoop==0 || fVoltageSwitchSupported )
        {
            // verify at this voltage if "nominal" voltage of "switching supported" :

            APPL_ShowMsg( 0, pszMsg );

            aInterfaceTab->UpdateInterfaceTestDisplay();  // show the "should-be" state of the control lines

            if ( PIC_DeviceInfo.iBitsPerInstruction==12 )
            {
                fDidSomething = true;       // 12-bit core must be PIC10F20x (nothing else)
                if ( ! PIC10F_ReadAll( PIC_ACTION_VERIFY ) )
                {
                    ++error_count;
                }
            }
            else // not 12-bit core ...
            {
                // verify CODE ?
                if ( Config.iProgramWhat & PIC_PROGRAM_CODE )
                {
                    if ( PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex >= 0 )
                    {
                        fDidSomething = true;
                        i32FirstAddress = PicBuf_ArrayIndexToTargetAddress(PIC_BUF_CODE, 0 );
                        i32LastAddress  = PicBuf_ArrayIndexToTargetAddress(PIC_BUF_CODE, PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex);
                        APPL_ShowMsg( 0, _("Verifying CODE, 0x%06lX..0x%06lX"),
                                      i32FirstAddress, i32LastAddress );
                        if (! PicPrg_Verify( 0 ,  // // dwDeviceAddress
                                             PicBuf[PIC_BUF_CODE].pdwData, // uint32_t *pdwSourceData
                                             PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex+1, // number of words to be verified
                                             0x3FFF,
                                             PIC_DeviceInfo.iCmd_ReadProg) )
                        {
                            ++error_count;
                        }
                    }
                    else // no program in memory !
                    {
                        APPL_ShowMsg( 0, _("No PROGRAM CODE in buffer to verify.") );
                        m_iMessagePanelUsage = MP_USAGE_WARNING;
                    }
                } // end if < shall verify CODE >


                // verify DATA EEPROM ?
                if ( Config.iProgramWhat & PIC_PROGRAM_DATA )
                {
                    if ( PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex >= 0 )
                    {
                        fDidSomething = true;
                        i32FirstAddress = PicBuf_ArrayIndexToTargetAddress(PIC_BUF_DATA, 0 );
                        i32LastAddress  = PicBuf_ArrayIndexToTargetAddress(PIC_BUF_DATA, PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex );
                        APPL_ShowMsg( 0, _("Verifying DATA, 0x%06lX..0x%06lX"),
                                      i32FirstAddress, i32LastAddress );
                        if (!PicPrg_Verify( PIC_DeviceInfo.lDataMemBase,
                                            PicBuf[PIC_BUF_DATA].pdwData,
                                            PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex+1,
                                            0xFF,PIC_DeviceInfo.iCmd_ReadDataDM ) )
                            ++error_count;
                    }
                    else // no DATA in memory...
                    {
                        APPL_ShowMsg( 0, _("No DATA MEMORY in buffer to verify.") );
                        m_iMessagePanelUsage = MP_USAGE_WARNING;
                    } // end if ( PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex >= 0 )
                } // end if < shall verify DATA >


                // Verify CONFIGURATION MEMORY and/or "ID locations" ?
                if ( Config.iProgramWhat & PIC_PROGRAM_CONFIG )
                {
                    if ( PicBuf[PIC_BUF_CONFIG].i32LastUsedArrayIndex >= 0 )
                    {
                        fDidSomething = true;
                        int iNrWordsWritten = PicPrg_GetNrOfConfigMemLocationsToWrite();
                        i32FirstAddress = PicBuf_ArrayIndexToTargetAddress(PIC_BUF_CONFIG, 0 );
                        i32LastAddress  = PicBuf_ArrayIndexToTargetAddress(PIC_BUF_CONFIG, iNrWordsWritten-1 );
                        APPL_ShowMsg( 0, _("Verifying CONFIG, 0x%06lX..0x%06lX"),
                                      i32FirstAddress, i32LastAddress );
                        if (!PicPrg_Verify( PIC_DeviceInfo.lConfMemBase,
                                            PicBuf[PIC_BUF_CONFIG].pdwData, // source buffer
                                            iNrWordsWritten,                // count of WORDs to be verified
                                            0x3FFF,  // mask for a 14-bit word (ignored for dsPIC etc)
                                            PIC_DeviceInfo.iCmd_ReadProg))  // command pattern to READ (usually 0x04)
                        {
                            ++error_count;
                        }
                        else // verification of config memory "ok":
                        {
                            PicPrg_iConfMemVerified = 1;  // "classic" config word has been VERIFIED OK now
                        }
                    } // end if < verify configuration memory ? >


                    // Verify "classic" CONFIG-WORD (code protection, watchdog, oscillator) ?
                    if (   ( PIC_DeviceInfo.iBitsPerInstruction==14 ) // only for PIC16Fxxx..
                            && ( ! PicPrg_iConfWordVerified ) // often included in 'Verify Config/ID-Memory" (above)
                            && ( PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex >= 0 ) )
                    {
                        fDidSomething = true;
                        APPL_ShowMsg( 0, _(" Verifying CONFIG-WORD%c"),
                                      (PIC_DeviceInfo.wCfgmask2_used!=0) ? _T('s') : _T(' ') );
                        dwTemp4[0] = PicBuf_GetConfigWord(0);
                        dwTemp4[1] = PicBuf_GetConfigWord(1);
                        if (!PicPrg_Verify( PIC_DeviceInfo.lConfWordAdr,
                                            dwTemp4, // uint32_t *buf
                                            1 + // number of words to be verified ...
                                            ((PIC_DeviceInfo.wCfgmask2_used==0x0000)?0:1),  // 2nd cfg word ?
                                            PicPrg_GetConfigWordMask(), // mask to be verified for FIRST (!) config word
                                            PIC_DeviceInfo.iCmd_ReadProg))  // command pattern to READ
                        {
                            ++error_count;
                        }
                    } // end if ( PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex >= 0 )
                } // end if < shall verify CONFIG and/or "ID locations" >
            } // end else < no 12-bit core >
        } // end if < nominal supply voltage, or  switch to "special" voltage supported >

        // No other verify-attempts at other voltages if this one failed:
        if ( error_count > 1/*!*/ )
            break;
        if (!fDidSomething)
            break;
    } // end    for(iSupplyVoltageLoop = 0; ...

    if ( PIC_HW_CanSelectVdd() )
    {
        PIC_HW_SelectVdd( Config.iIdleSupplyVoltage );  // back to the "normal" voltage
    }

    if (APPL_iUserBreakFlag)
    {
        APPL_ShowMsg( 0, _("USER BREAK while verifying.") );
        m_iMessagePanelUsage = MP_USAGE_WARNING;
        APPL_iUserBreakFlag = 0;
        error_count = 1;
    }
    else // no 'user break' during PROGRAM
    {
        if (error_count==0)
        {
            if (fDidSomething)
                pszMsg = _("Verify finished, no errors.");
            else
                pszMsg = _("Nothing to verify in buffer.");
            APPL_ShowMsg( 0, pszMsg );
            m_iMessagePanelUsage = MP_USAGE_INFO;
        }
        else
        {
            pszMsg = _("ERROR: Verifying FAILED !");
            APPL_ShowMsg( 0, pszMsg );
            m_iMessagePanelUsage = MP_USAGE_ERROR;
        }
    }
    UpdateAllSheets(); // highlight good or bad CODE LOCATIONS
    aInterfaceTab->UpdateInterfaceTestDisplay();

    UpdateLedsForResult( error_count==0 );
    return error_count==0;
} // end MainFrame::VerifyPic()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool MainFrame::RunHexOpenDialog(void)
{
    /* see help on TOpenDialog - beware: Win XP saves a history in the registry (HOLY SHIT) */
    aFileDialog->SetMessage(_("Load HEX File"));
    //-- Note the following string cannot be static to avoid translation
    //-- to be performed before the language selection
    const wxChar *theHexFileFilter =
    #ifdef __WXMSW__
        _("HEX files (INHX8M, *.hex)|*.hex");
    #else
        //-- when case is sensitive, accept both cases
        _("HEX files (INHX8M, *.hex)|*.hex;*.HEX|*.HEX");
    #endif
    aFileDialog->SetWildcard(theHexFileFilter);
    wxFileName DefaultFile(Config.sz255HexFileName);
    aFileDialog->SetFilename(DefaultFile.GetFullName());
    if ( DefaultFile.FileExists() )
        aFileDialog->SetPath(DefaultFile.GetFullPath());
    else
        aFileDialog->SetFilename(DefaultFile.GetFullName());
    aFileDialog->SetWindowStyle(wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if ( aFileDialog->ShowModal() == wxID_OK)
    {
        _tcscpy( Config.sz255HexFileName, aFileDialog->GetPath().c_str() );
        ConfigChanged = true;
        return true;
    }
    return false;
} // end RunHexOpenDialog()


//---------------------------------------------------------------------------
bool MainFrame::LoadFileAndProgramPic(const wxChar *fn, bool program_too)
{
// FILE *fp;
    bool ok;
    wxChar sz255Temp[256];
    const wxChar *cp;

    RedAndGreenLedOff();

    {
        wxFile ReadTest(fn);
        if (!ReadTest.IsOpened())
        {
            _stprintf(sz255Temp, _("Can't open hexfile \"%s\"") , fn);
            APPL_ShowMsg( 0, sz255Temp );
            //       if(ToolForm)
            //        ToolForm->ShowMsg( sz255Temp, TWMSG_ERROR );
            return false;
        }
    }

    AddMRFname( wxString(fn) );  // add this file to the list of 'recent files'

    /* Initialize buffers to remove old junk from program & data eeprom ? */
    if (Config.iClearBufBeforeLoad)
    {
        PIC_HEX_ClearBuffers();
    }

    ok  = ( PIC_HEX_LoadFile(fn)==0);  // 0 here means "NO error"

    if (Config.iVerboseMessages)
    {
        _stprintf(sz255Temp,
                  _("Results from LoadHex: LastProgAdr=0x%06lX LastDataAdr=0x%06lX"),
                  PicBuf_ArrayIndexToTargetAddress(PIC_BUF_CODE, PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex),
                  PicBuf_ArrayIndexToTargetAddress(PIC_BUF_DATA, PicBuf[PIC_BUF_DATA].i32LastUsedArrayIndex)
                 );
        cp = sz255Temp + _tcslen(sz255Temp);
        _stprintf( const_cast<wxChar*>(cp), _(" (%s)"), fn );    // added 2006-03-26
        APPL_ShowMsg( 0, sz255Temp );
    }
    if (CommandOption.WinPic_i32CmdLineOption_OverrideConfigWord>=0)
    {
        // replace the CONFIGURATION WORD which should have been in the config file ?
        if ( (uint16_t)PicBuf_GetConfigWord(0) != (uint16_t)CommandOption.WinPic_i32CmdLineOption_OverrideConfigWord)
        {
            APPL_ShowMsg( 0, _("Info: Config word set to 0x%06lX from command line .") ,
                          CommandOption.WinPic_i32CmdLineOption_OverrideConfigWord);
        }
        PicBuf_SetConfigWord( 0, CommandOption.WinPic_i32CmdLineOption_OverrideConfigWord );
    } // end if < override configuration word > ?

    UpdateAllSheets(); // make the changes visible on the screen

    if (!ok)
    {
        if (program_too)
        {
            _tcscpy(sz255Temp, _("Aborting LoadAndProgram because of error(s)") );
            APPL_ShowMsg( 0, sz255Temp );
        }
        return false;
    }

  if ( ! program_too )
   {
     return true;    // ready at this step (do not program)
   }

    // now try to program all loaded elements into the PIC:
    return ProgramPic();
} // end LoadFileAndProgramPic()
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool MainFrame::DumpEverythingToHexFile(const wxChar *fn)
{

    if ( CommandOption.WinPic_fCommandLineOption_QueryBeforeOverwritingFiles )
    {
        if ( wxFile::Exists(fn) )
        {
            if (wxMessageBox( _("Dump would overwrite existing file.\n  Erase old file ?"),
                              _("Confirm overwrite"),
                              wxICON_QUESTION | wxYES_NO | wxCANCEL ) != wxYES )
            {
                UpdateLedsForResult( false );
                return false;
            }
        }
    }

    if ( PIC_HEX_DumpHexFile(fn) <= 0 ) // dump all buffers which contain "something"
    {
        APPL_ShowMsg( 0, _("Can't create hexfile") );
        m_iMessagePanelUsage = MP_USAGE_WARNING;
        UpdateLedsForResult( false );
        return false;
    }

    UpdateLedsForResult( true );
    return true;

} // end DumpEverythingToHexFile( :o)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void MainFrame::StopParsingCmdLine_Internal(void)
{
    CommandOption.WinPic_fCommandLineMode = false;  // error, stop parsing command line
    aStatusBar->SetStatusText(aStatusBar->GetStatusText() + _(".. stopped parsing cmd-line")) ;
    m_iMessagePanelUsage = MP_USAGE_ERROR;
} // end StopParsingCmdLine_Internal()


//---------------------------------------------------------------------------
void MainFrame::UpdateAllSheets(void)
{
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("Update all tabs: CODE memory ..") );
    aCodeMemTab->UpdateCodeMemDisplay();     // show CODE in the hex display
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("Update all tabs: DATA memory ..") );
    aDataMemTab->UpdateDataMemDisplay();     // show EEPROM DATA  "  "  "
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("Update all tabs: ID and Config memory dump ..") );
    aConfigMemoryTab->UpdateIdAndConfMemDisplay();  // show ID and CONFIG MEMORY as hex- or bin- dump
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("Update all tabs: Device/Config tab ..") );
    aDeviceCfgTab->UpdateDeviceConfigTab(true/*fUpdateHexWord*/ );
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("Update all tabs: Interface display ..") );
    aInterfaceTab->UpdateInterfaceTestDisplay(); // show current state of control lines
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("Update all tabs: Options display ..") );
    aOptionTab->UpdateOptionsDisplay();
    Update();  // make the changes visible on the screen
    // 2005-03-05: .. and turned the contents of the TRichEdit into a mess ?!
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("Update all tabs: DONE .") );
} // end UpdateAllSheets()


void MainFrame::addLines (wxString &pText, void (wxAboutDialogInfo::*pAdder)(const wxString&)){    wxStringTokenizer Tokenizer(pText, wxT("\n\r\t"), wxTOKEN_STRTOK);    for (wxString Line = Tokenizer.GetNextToken(); !Line.IsEmpty(); Line = Tokenizer.GetNextToken())    {        if (Line.GetChar(Line.length()-1) == wxT(':'))        {            //-- If line ends with ':'  conacatennate next line            Line += wxT(' ');            Line += Tokenizer.GetNextToken();        }        (aAboutInfo.*pAdder)(Line);    }}

