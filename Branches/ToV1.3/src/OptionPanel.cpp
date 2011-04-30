/*-------------------------------------------------------------------------*/
/*  Filename: OptionPanel.cpp                                              */
/*                                                                         */
/*  Purpose:                                                               */
/*     Manage the WxPic Options Panel                                      */
/*                                                                         */
/*  Author:                                                                */
/*     Copyright 2009-2011 Philippe Chevrier pch @ laposte.net             */
/*     from software originally written by Wolfgang Buescher (DL4YHF)      */
/*                                                                         */
/*  License:                                                               */
/*     Ported Code is licensed under GPLV3 conditions with original code   */
/*     restriction :                                                       */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#include "OptionPanel.h"
#include "MainFrame.h"
#include "Language.h"
#include "Appl.h"
#include "Config.h"

//(*InternalHeaders(TOptionPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/msgdlg.h>
#include <wx/colordlg.h>
#include <wx/settings.h>

#include <WinPicPr/PIC_PRG.h>
#include <WinPicPr/PIC_HW.h>

//(*IdInit(TOptionPanel)
const long TOptionPanel::ID_PROG_CODE_MEM_CHK = wxNewId();
const long TOptionPanel::ID_PROG_DATA_MEM_CHK = wxNewId();
const long TOptionPanel::ID_PROG_CONFIG_MEM_CHK = wxNewId();
const long TOptionPanel::ID_USE_BULK_ERASE_CHK = wxNewId();
const long TOptionPanel::ID_DISCONNECT_AFTER_PROG_CHK = wxNewId();
const long TOptionPanel::ID_VERIFY_DIFF_VDDS_CHK = wxNewId();
const long TOptionPanel::ID_DONT_CARE_FOR_OSCCAL_CHK = wxNewId();
const long TOptionPanel::ID_DONT_CARE_FOR_BG_CALIB_CHK = wxNewId();
const long TOptionPanel::ID_CLR_BEFORE_LOAD_CHK = wxNewId();
const long TOptionPanel::ID_VDD_BEFORE_MCLR_CHK = wxNewId();
const long TOptionPanel::ID_STATICTEXT1 = wxNewId();
const long TOptionPanel::ID_CHOICE1 = wxNewId();
const long TOptionPanel::ID_CODE_MEM_COLOUR_BUTTON = wxNewId();
const long TOptionPanel::ID_DATA_MEM_COLOUR_BUTTON = wxNewId();
const long TOptionPanel::ID_STATICTEXT2 = wxNewId();
const long TOptionPanel::ID_MPLAB_DEV_DIR_EDIT = wxNewId();
const long TOptionPanel::ID_MPLAB_DEV_DIR_BUTTON = wxNewId();
const long TOptionPanel::ID_CHECKBOX11 = wxNewId();
const long TOptionPanel::ID_CHECKBOX12 = wxNewId();
const long TOptionPanel::ID_START_TEST_BUTTON = wxNewId();
const long TOptionPanel::ID_TEST_CHOICE = wxNewId();
//*)

BEGIN_EVENT_TABLE(TOptionPanel,wxPanel)
	//(*EventTable(TOptionPanel)
	//*)
END_EVENT_TABLE()

TOptionPanel::TOptionPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name)
{
	//(*Initialize(TOptionPanel)
	wxBoxSizer* BoxSizer4;
	wxStaticBoxSizer* StaticBoxSizer2;
	wxStaticBoxSizer* StaticBoxSizer4;
	wxBoxSizer* BoxSizer2;
	wxStaticBoxSizer* StaticBoxSizer3;
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;
	wxBoxSizer* BoxSizer3;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	FlexGridSizer1 = new wxFlexGridSizer(2, 2, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Programmer options"));
	aProgCodeMemChk = new wxCheckBox(this, ID_PROG_CODE_MEM_CHK, _("Program CODE memory"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_PROG_CODE_MEM_CHK"));
	aProgCodeMemChk->SetValue(false);
	StaticBoxSizer1->Add(aProgCodeMemChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aProgDataMemChk = new wxCheckBox(this, ID_PROG_DATA_MEM_CHK, _("Program DATA memory"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_PROG_DATA_MEM_CHK"));
	aProgDataMemChk->SetValue(false);
	StaticBoxSizer1->Add(aProgDataMemChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aProgConfigMemChk = new wxCheckBox(this, ID_PROG_CONFIG_MEM_CHK, _("Program CONFIG memory"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_PROG_CONFIG_MEM_CHK"));
	aProgConfigMemChk->SetValue(false);
	StaticBoxSizer1->Add(aProgConfigMemChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aUseBulkEraseChk = new wxCheckBox(this, ID_USE_BULK_ERASE_CHK, _("Use BULK ERASE (to unprotect)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_USE_BULK_ERASE_CHK"));
	aUseBulkEraseChk->SetValue(false);
	StaticBoxSizer1->Add(aUseBulkEraseChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aDisconnectAfterProgChk = new wxCheckBox(this, ID_DISCONNECT_AFTER_PROG_CHK, _("Automatic disconnect from target"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_DISCONNECT_AFTER_PROG_CHK"));
	aDisconnectAfterProgChk->SetValue(false);
	StaticBoxSizer1->Add(aDisconnectAfterProgChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aVerifyDiffVddsChk = new wxCheckBox(this, ID_VERIFY_DIFF_VDDS_CHK, _("Verify at different supply voltages"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_VERIFY_DIFF_VDDS_CHK"));
	aVerifyDiffVddsChk->SetValue(false);
	StaticBoxSizer1->Add(aVerifyDiffVddsChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aDontCareForOsccalChk = new wxCheckBox(this, ID_DONT_CARE_FOR_OSCCAL_CHK, _("No special treatment for OSCAL"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_DONT_CARE_FOR_OSCCAL_CHK"));
	aDontCareForOsccalChk->SetValue(false);
	StaticBoxSizer1->Add(aDontCareForOsccalChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aDontCareForBGCalibChk = new wxCheckBox(this, ID_DONT_CARE_FOR_BG_CALIB_CHK, _("No special treatment for BG calib"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_DONT_CARE_FOR_BG_CALIB_CHK"));
	aDontCareForBGCalibChk->SetValue(false);
	StaticBoxSizer1->Add(aDontCareForBGCalibChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, _("Other options"));
	aClrBeforeLoadChk = new wxCheckBox(this, ID_CLR_BEFORE_LOAD_CHK, _("Clear buffers before loading HEX file"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CLR_BEFORE_LOAD_CHK"));
	aClrBeforeLoadChk->SetValue(false);
	StaticBoxSizer2->Add(aClrBeforeLoadChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aVddBeforeMCLRChk = new wxCheckBox(this, ID_VDD_BEFORE_MCLR_CHK, _("Raise Vdd before Vpp (MCLR)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_VDD_BEFORE_MCLR_CHK"));
	aVddBeforeMCLRChk->SetValue(false);
	StaticBoxSizer2->Add(aVddBeforeMCLRChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Select language"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer1->Add(StaticText1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aLanguageChoice = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	BoxSizer1->Add(aLanguageChoice, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer2->Add(BoxSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	aCodeMemColourButton = new wxButton(this, ID_CODE_MEM_COLOUR_BUTTON, _("Code memory color"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CODE_MEM_COLOUR_BUTTON"));
	BoxSizer2->Add(aCodeMemColourButton, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aDataMemColourButton = new wxButton(this, ID_DATA_MEM_COLOUR_BUTTON, _("Data memory color"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_DATA_MEM_COLOUR_BUTTON"));
	BoxSizer2->Add(aDataMemColourButton, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	StaticBoxSizer2->Add(BoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	aMplabDirLabel = new wxStaticText(this, ID_STATICTEXT2, _("MPLAB DEV-file directory"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	StaticBoxSizer2->Add(aMplabDirLabel, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	aMplabDevDirEdit = new wxTextCtrl(this, ID_MPLAB_DEV_DIR_EDIT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_MPLAB_DEV_DIR_EDIT"));
	BoxSizer3->Add(aMplabDevDirEdit, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aMPLabDevDirButton = new wxButton(this, ID_MPLAB_DEV_DIR_BUTTON, _("..."), wxDefaultPosition, wxSize(24,-1), 0, wxDefaultValidator, _T("ID_MPLAB_DEV_DIR_BUTTON"));
	BoxSizer3->Add(aMPLabDevDirButton, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer2->Add(BoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, this, _("Debugging"));
	aSimulateOnlyChk = new wxCheckBox(this, ID_CHECKBOX11, _("Simulate only (ignore hardware)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX11"));
	aSimulateOnlyChk->SetValue(false);
	StaticBoxSizer3->Add(aSimulateOnlyChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aVerboseMsgsChk = new wxCheckBox(this, ID_CHECKBOX12, _("Generate verbose messages"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX12"));
	aVerboseMsgsChk->SetValue(false);
	StaticBoxSizer3->Add(aVerboseMsgsChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	FlexGridSizer1->Add(StaticBoxSizer3, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Test"));
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	aStartTestButton = new wxButton(this, ID_START_TEST_BUTTON, _("Start test"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_START_TEST_BUTTON"));
	BoxSizer4->Add(aStartTestButton, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 1);
	aTestChoice = new wxChoice(this, ID_TEST_CHOICE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_TEST_CHOICE"));
	aTestChoice->SetSelection( aTestChoice->Append(_("None")) );
	aTestChoice->Append(_("Speed Test"));
	aTestChoice->Append(_("Switch on/Switch off"));
	aTestChoice->Append(_("Program/Read Config"));
	BoxSizer4->Add(aTestChoice, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 1);
	StaticBoxSizer4->Add(BoxSizer4, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	FlexGridSizer1->Add(StaticBoxSizer4, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	SetSizer(FlexGridSizer1);
	aColourDialog = new wxColourDialog(this);
	aDirDialog = new wxDirDialog(this, _("Select directory"), wxEmptyString, wxDD_DEFAULT_STYLE|wxDD_DIR_MUST_EXIST, wxDefaultPosition, wxDefaultSize, _T("wxDirDialog"));
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_PROG_CODE_MEM_CHK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TOptionPanel::onProgOptionChanged);
	Connect(ID_PROG_DATA_MEM_CHK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TOptionPanel::onProgOptionChanged);
	Connect(ID_PROG_CONFIG_MEM_CHK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TOptionPanel::onProgOptionChanged);
	Connect(ID_USE_BULK_ERASE_CHK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TOptionPanel::onProgOptionChanged);
	Connect(ID_DISCONNECT_AFTER_PROG_CHK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TOptionPanel::onProgOptionChanged);
	Connect(ID_VERIFY_DIFF_VDDS_CHK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TOptionPanel::onProgOptionChanged);
	Connect(ID_DONT_CARE_FOR_OSCCAL_CHK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TOptionPanel::onProgOptionChanged);
	Connect(ID_DONT_CARE_FOR_BG_CALIB_CHK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TOptionPanel::onProgOptionChanged);
	Connect(ID_CLR_BEFORE_LOAD_CHK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TOptionPanel::onProgOptionChanged);
	Connect(ID_VDD_BEFORE_MCLR_CHK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TOptionPanel::onProgOptionChanged);
	Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&TOptionPanel::onLanguageChoiceSelect);
	Connect(ID_CODE_MEM_COLOUR_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TOptionPanel::onCodeMemColourButtonClick);
	Connect(ID_DATA_MEM_COLOUR_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TOptionPanel::onDataMemColourButtonClick);
	Connect(ID_MPLAB_DEV_DIR_EDIT,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&TOptionPanel::onMplabDevDirEditTextEnter);
	Connect(ID_MPLAB_DEV_DIR_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TOptionPanel::onMPLabDevDirButtonClick);
	Connect(ID_CHECKBOX11,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TOptionPanel::onProgOptionChanged);
	Connect(ID_CHECKBOX12,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TOptionPanel::onProgOptionChanged);
	Connect(ID_START_TEST_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TOptionPanel::onStartTestButtonClick);
	//*)
	aMplabDevDirEdit->Connect(wxEVT_KILL_FOCUS,(wxObjectEventFunction)&TOptionPanel::onLooseFocus,NULL,this);

    aLanguageChoice->Append (BUILT_IN_NAME);
    wxArrayString LanguageList = TLanguage::GetLanguageNameList();
    for (unsigned i = 0; i < LanguageList.Count(); ++i)
        aLanguageChoice->Append (LanguageList[i]);
    aLanguageChoice->SetStringSelection(TSessionConfig::GetLanguageName());
}

TOptionPanel::~TOptionPanel()
{
	//(*Destroy(TOptionPanel)
	//*)
}


//---------------------------------------------------------------------------
bool TOptionPanel::InterfaceSpeedTest(void)
{
    int  n_errors = 0;
    int  iTestLoops, i50nsLoops, i50nsLoopsUnstable;
#define N_TEST_LOOPS 10
    double dblDelayTimes_ns[N_TEST_LOOPS];
    bool fDataOutState;
    wxChar sz255Msg[256];
    wxChar *cp;
    LONGLONG i64PTFreq, i64PTStart, i64PTStop;
    double dblLoopTime_ns;


    QueryPerformanceFrequency( (LARGE_INTEGER *) &i64PTFreq );

    // Check how fast we can set and reset the CLOCK signal .
    //  You may be badly surprised how increadibly slow the access
    //  to such a stupid piece of hardware can be under windoze these days !
    QueryPerformanceCounter( (LARGE_INTEGER*)&i64PTStart );
    for (iTestLoops=0; iTestLoops<1000; ++iTestLoops)
    {
        PIC_HW_SetClockAndData( (iTestLoops&1)!=0/*clock_high*/,
                                (iTestLoops&2)!=0/*data_high*/  );
    }
    QueryPerformanceCounter( (LARGE_INTEGER*)&i64PTStop );
    dblLoopTime_ns = (double)(i64PTStop - i64PTStart);
    if (i64PTFreq>0)
    {
        dblLoopTime_ns *= (1e9 / ((double)1000/*Loops*/ * (double)i64PTFreq) );
        _stprintf( sz255Msg, _("SetClockAndData takes %d ns per call ."), dblLoopTime_ns);
        if (dblLoopTime_ns > 1e6 )
        {
            _tcscat( sz255Msg, _(" THIS IS INCREDIBLY SLOW ! ") );
            // One example: A USB<->RS-232 adapter by "Belkin", which identified
            //  itself as "Prolific USB-Serial Adapter" in the system control,
            //  required almost 5 ms (FIVE MILLISECONDS!!!!) for a Clock-and-Data-Cycle.
        }
        APPL_ShowMsg( 0, sz255Msg );
    }
    else
    {
        APPL_ShowMsg( 0, _("Something wrong with QueryPerformanceFrequency !") );
    }


    // The next part of the "interface speed test" just sends
    // ONES and ZEROES to the interface, as fast as possible,
    // and check the time until the wanted levels are read back .
    //  The delay times for the INTERFACE (! - not the chip) should be set
    //  at least THREE TIMES this value, because the PIC's threshold voltage
    //  may be totally different from the serial port's threshold.
    //  Most of the speed-test is described in the manual so we don't
    //  need lengthy explanations here.

    PIC_HW_ConnectToTarget();     // must be connected to the "target" even if no chip inserted
    PIC_HW_SetDataEnable( true ); // rarely used, but may be necessary
    PIC_HW_SetVpp(false);
    PIC_HW_SetVdd(true);     // turn Vdd on, otherwise 'read back' may not work


    for (iTestLoops=0; iTestLoops<N_TEST_LOOPS; ++iTestLoops)
    {
        fDataOutState = (iTestLoops & 1) ? true : false;
        if (!PIC_HW_SetClockAndData( false, fDataOutState ) )
        {
            ++n_errors;
        }
        PIC_HW_LongDelay_ms( 10/*milliseconds*/ ); // wait until everything is settled
        PIC_HW_ShortDelay(PIC_HW_dwCount50ns);  // make sure this routine is in the CPU cache
        PIC_HW_GetDataBit();                    // .. and this routine too

        // Now swith the DATA line to "the other state" and wait until we read that state back..
        fDataOutState = !fDataOutState;
        i50nsLoops = i50nsLoopsUnstable = 0;
        QueryPerformanceCounter( (LARGE_INTEGER*)&i64PTStart );
        if (!PIC_HW_SetClockAndData( false, fDataOutState ) ) // time-critical section..
        {
            ++n_errors;
        }
        // wait for 100 microseconds, observing the read-back DATA signal :
        while (i50nsLoops < 10000/* a few milliseconds*/ )
        {
            ++i50nsLoops;
            if ( PIC_HW_GetDataBit() != fDataOutState )
            {
                // read-back state is not "stable" yet:
                i50nsLoopsUnstable = i50nsLoops;
            }
        }
        QueryPerformanceCounter( (LARGE_INTEGER*)&i64PTStop );
        dblLoopTime_ns = (double)(i64PTStop - i64PTStart);
        if (i64PTFreq>0)
            dblLoopTime_ns *= (1e9 / ((double)i50nsLoops * (double)i64PTFreq) );
        else   dblLoopTime_ns = 50.0; // guesswork if no "performance timer" available
        // Typical loop time, measured with PortTalk + 1.6 GHz-P4 : 1600 ns
        dblDelayTimes_ns[iTestLoops] = dblLoopTime_ns * (double)i50nsLoopsUnstable;
    } // end for(iTestLoops .. )

    // Finished speed-checking loop, turn interface control lines off...
    PIC_HW_SetDataEnable( false );
    PIC_HW_DisconnectFromTarget();
    MainFrame::TheMainFrame->aInterfaceTab->UpdateInterfaceTestDisplay();

    // Present the results...
    _tcscpy( sz255Msg, _("Response times L->H : ") );
    cp = sz255Msg+_tcslen(sz255Msg);
    for (iTestLoops=0; iTestLoops<N_TEST_LOOPS; iTestLoops+=2)
    {
        _stprintf( cp, _(" %dns"), dblDelayTimes_ns[iTestLoops] );
        cp += _tcslen(cp);
    }
    APPL_ShowMsg( 0, sz255Msg );
    _tcscpy( sz255Msg, _("Response times H->L : ") );
    cp = sz255Msg+_tcslen(sz255Msg);
    for (iTestLoops=1; iTestLoops<N_TEST_LOOPS; iTestLoops+=2)
    {
        _stprintf( cp, _(" %lgns"), dblDelayTimes_ns[iTestLoops] );
        cp += _tcslen(cp);
    }
    APPL_ShowMsg( 0, sz255Msg );


    return (n_errors==0);
} // end InterfaceSpeedTest()


//---------------------------------------------------------------------------
void TOptionPanel::UpdateOptionsDisplay(void)
{
    ++(MainFrame::TheMainFrame->m_Updating);

    aProgCodeMemChk->SetValue((TSessionConfig::GetProgramWhat() & PIC_PROGRAM_CODE) != 0);
    aProgDataMemChk->SetValue((TSessionConfig::GetProgramWhat() & PIC_PROGRAM_DATA) != 0);
    aProgConfigMemChk->SetValue((TSessionConfig::GetProgramWhat() & PIC_PROGRAM_CONFIG)!=0);
    aUseBulkEraseChk->SetValue(TSessionConfig::GetUseCompleteChipErase());  // may be disabled !
    aDisconnectAfterProgChk->SetValue(TSessionConfig::GetDisconnectAfterProg());
    aVerifyDiffVddsChk->SetValue(TSessionConfig::GetUseDifferentVoltages());
    aVerifyDiffVddsChk->Enable(PIC_HW_CanSelectVdd());
//   if(PIC_HW_CanSelectVdd() )  // don't DISABLE this checkmark but..
//        Chk_VerifyDiffVdds->Font->Color = clBlack;
//    else // .. make it gray if it has no meaningfull function
//        Chk_VerifyDiffVdds->Font->Color = clGray;

    aDontCareForOsccalChk->SetValue(TSessionConfig::GetDontCareForOsccal());
    aDontCareForBGCalibChk->SetValue(TSessionConfig::GetDontCareForBGCalib());
    aClrBeforeLoadChk->SetValue(TSessionConfig::GetClearBufBeforeLoad());
    aVddBeforeMCLRChk->SetValue(TSessionConfig::GetNeedPowerBeforeMCLR()); // read a long story in PIC_HW_ProgMode() !
    // Note: this MAY be contrary to PIC_DeviceInfo.wVppVddSequence,
    //  but the user may decide to change the Vpp/Vdd sequence
    //  depending on his programmer hardware, and to experiment
    //  with 'future' devices (which may cause problems similar to
    //  early chip revisions of the 16F628) .
    // Since 2005-10-25, the path into MBLAB's "device"-folder
    // can be specified here - so we don't have to copy those *.DEV-files :
    aMplabDevDirEdit->ChangeValue(TSessionConfig::GetMplabDevDir());

    // group "Debugging"...
    aSimulateOnlyChk->SetValue(PIC_PRG_iSimulateOnly);
    aVerboseMsgsChk->SetValue(TSessionConfig::GetVerboseMessages());


    if (MainFrame::TheMainFrame->m_Updating>0)
        --(MainFrame::TheMainFrame->m_Updating);
} // end ::UpdateOptionsDisplay()




//---------------------------------------------------------------------------
void TOptionPanel::onProgOptionChanged(wxCommandEvent& event)
{
    if (MainFrame::TheMainFrame->m_Updating) return;

    int ProgramWhat = TSessionConfig::GetProgramWhat();
    if ( aProgCodeMemChk->GetValue() )
        ProgramWhat |= PIC_PROGRAM_CODE;
    else
        ProgramWhat &=~PIC_PROGRAM_CODE;
    if ( aProgDataMemChk->GetValue() )
        ProgramWhat |= PIC_PROGRAM_DATA;
    else
        ProgramWhat &=~PIC_PROGRAM_DATA;
    if ( aProgConfigMemChk->GetValue() )
        ProgramWhat |= PIC_PROGRAM_CONFIG;
    else
        ProgramWhat &=~PIC_PROGRAM_CONFIG;
    TSessionConfig::SetProgramWhat(ProgramWhat);
    TSessionConfig::SetUseCompleteChipErase(aUseBulkEraseChk->GetValue());
    TSessionConfig::SetDisconnectAfterProg(aDisconnectAfterProgChk->GetValue());
    TSessionConfig::SetUseDifferentVoltages(aVerifyDiffVddsChk->GetValue());
    TSessionConfig::SetDontCareForOsccal(aDontCareForOsccalChk->GetValue());
    TSessionConfig::SetDontCareForBGCalib(aDontCareForBGCalibChk->GetValue());
    TSessionConfig::SetClearBufBeforeLoad(aClrBeforeLoadChk->GetValue());
    TSessionConfig::SetNeedPowerBeforeMCLR(aVddBeforeMCLRChk->GetValue());  // read a long story in PIC_HW_ProgMode() !

    TSessionConfig::SetMplabDevDir(aMplabDevDirEdit->GetValue());

    // From group "Debugging" ...
    TSessionConfig::SetVerboseMessages(aVerboseMsgsChk->GetValue());
    PIC_PRG_iSimulateOnly= aSimulateOnlyChk->GetValue();
} // end MainFrame::ProgOptionsChanged()
//---------------------------------------------------------------------------



////---------------------------------------------------------------------------
void TOptionPanel::onLanguageChoiceSelect(wxCommandEvent& event)
{
    TSessionConfig::ChangeLanguage(aLanguageChoice->GetStringSelection());
    wxMessageBox(_("WxPic must be restarted to take into account the new language selection"), _("WxPic restart needed to apply language"));
}



//---------------------------------------------------------------------------
void TOptionPanel::onCodeMemColourButtonClick(wxCommandEvent& event)
{
// uint32_t dwRGBcolor;
    wxColourData &Colour = aColourDialog->GetColourData();
    Colour.SetColour(aCodeMemColourButton->GetForegroundColour());
    aColourDialog->SetLabel(_("Code Memory Text"));
    if (aColourDialog->ShowModal() == wxID_OK)
        aCodeMemColourButton->SetForegroundColour(Colour.GetColour());

    Colour.SetColour(aCodeMemColourButton->GetBackgroundColour());
    aColourDialog->SetLabel(_("Code Memory Background"));
    if (aColourDialog->ShowModal() == wxID_OK)
        aCodeMemColourButton->SetBackgroundColour(Colour.GetColour());

    MainFrame::TheMainFrame->aCodeMemTab->UpdateCodeMemDisplay();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void TOptionPanel::onDataMemColourButtonClick(wxCommandEvent& event)
{
    wxColourData &Colour = aColourDialog->GetColourData();
    Colour.SetColour(aDataMemColourButton->GetForegroundColour());
    aColourDialog->SetLabel(_("Data Memory Text"));
    if (aColourDialog->ShowModal() == wxID_OK)
    {
        aDataMemColourButton->SetForegroundColour(Colour.GetColour());
    }
    Colour.SetColour(aDataMemColourButton->GetBackgroundColour());
    aColourDialog->SetLabel(_("Data Memory Background"));
    if (aColourDialog->ShowModal() == wxID_OK)
    {
        aDataMemColourButton->SetBackgroundColour(Colour.GetColour());
    }
    MainFrame::TheMainFrame->aDataMemTab->UpdateDataMemDisplay();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void TOptionPanel::onMPLabDevDirButtonClick(wxCommandEvent& event)
{
    aDirDialog->SetPath(aMplabDevDirEdit->GetValue());
    aDirDialog->SetMessage(_("Select MPLAB 'Device' folder"));
    if (aDirDialog->ShowModal() == wxID_OK)
    {
        aMplabDevDirEdit->ChangeValue(aDirDialog->GetPath());
        updateMPLabDevDir(aDirDialog->GetPath());
    }
}
//---------------------------------------------------------------------------



void TOptionPanel::onMplabDevDirEditTextEnter(wxCommandEvent& event)
{
    updateFromMPLabDevText();
}


void TOptionPanel::onLooseFocus(wxFocusEvent& pEvent)
{
    updateFromMPLabDevText();
}


void TOptionPanel::updateFromMPLabDevText(void)
{
    updateMPLabDevDir(aMplabDevDirEdit->GetValue());
}

void TOptionPanel::updateMPLabDevDir (const wxString &pDirPath)
{
    TSessionConfig::SetMplabDevDir(pDirPath);
    TDeviceCfgPanel::UpdateDevice();
}


//---------------------------------------------------------------------------
void TOptionPanel::onStartTestButtonClick(wxCommandEvent& event)
{
    if ( TSessionConfig::TheTestMode )
        TSessionConfig::TheTestMode = 0;
    else
    {
        long TestMode=aTestChoice->GetSelection();
        if (TestMode != 0)
            TSessionConfig::TheTestMode = TestMode;
    }
    switch ( TSessionConfig::TheTestMode ) // any special "one-shot"-test ?
    {
    case 1:
        TSessionConfig::TheTestMode = 0;
        if ( wxMessageBox(
                    _("Remove the PIC from the socket,\n connect interface and power supply.\nDo you want to continue ?"),
                    _("Interface Speed Test"),
                    wxICON_QUESTION | wxYES_NO | wxCANCEL) == wxYES )
        {
            InterfaceSpeedTest();
        }
        break; // end case "test #1"
    case 2:

        break;
    default:
        break;
    }

    if ( TSessionConfig::TheTestMode )
    {
        aStartTestButton->SetLabel(_("Stop test"));
    }
    else
    {
        aStartTestButton->SetLabel(_("Start test"));
    }
}
//---------------------------------------------------------------------------

