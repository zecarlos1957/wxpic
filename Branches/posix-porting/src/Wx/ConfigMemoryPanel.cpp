#include "ConfigMemoryPanel.h"
#include "MainFrame.h"
#include "Appl.h"
#include "Language.h"
#include <Res/Resource.h>

//(*InternalHeaders(TConfigMemoryPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/settings.h>

#include <WinPic/WinPicPr/PIC_HEX.h>

//(*IdInit(TConfigMemoryPanel)
const long TConfigMemoryPanel::ID_CFG_MEM_GRID = wxNewId();
const long TConfigMemoryPanel::ID_STATICTEXT10 = wxNewId();
const long TConfigMemoryPanel::ID_ID_BIN_HEX_RADIO = wxNewId();
const long TConfigMemoryPanel::ID_SHOW_ALL_CFG_CELLS_CHK = wxNewId();
const long TConfigMemoryPanel::ID_STATICTEXT11 = wxNewId();
const long TConfigMemoryPanel::ID_DEV_ID_TITLE = wxNewId();
const long TConfigMemoryPanel::ID_DEV_ID = wxNewId();
const long TConfigMemoryPanel::ID_DEV_ID_DECODED = wxNewId();
const long TConfigMemoryPanel::ID_APPLY_ID_LOCS_BUTTON = wxNewId();
const long TConfigMemoryPanel::ID_BUTTON6 = wxNewId();
const long TConfigMemoryPanel::ID_PANEL6 = wxNewId();
//*)

enum
{
    configMemSTAR,
    configMemINFO,
    configMemVALUE,
};

enum
{
    radioDevIdBIN,
    radioDevIdHEX,
};



BEGIN_EVENT_TABLE(TConfigMemoryPanel,wxPanel)
	//(*EventTable(TConfigMemoryPanel)
	//*)
END_EVENT_TABLE()

TConfigMemoryPanel::TConfigMemoryPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name)
{
	//(*Initialize(TConfigMemoryPanel)
	wxBoxSizer* BoxSizer13;
	wxBoxSizer* BoxSizer12;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, wxT("id"));
	BoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
	aCfgMemGrid = new wxGrid(this, ID_CFG_MEM_GRID, wxDefaultPosition, wxSize(-1,0), 0, wxT("ID_CFG_MEM_GRID"));
	aCfgMemGrid->CreateGrid(0,3);
	aCfgMemGrid->SetMinSize(wxSize(0,0));
	BoxSizer12->Add(aCfgMemGrid, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 7);
	Panel6 = new wxPanel(this, ID_PANEL6, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL, wxT("ID_PANEL6"));
	BoxSizer13 = new wxBoxSizer(wxVERTICAL);
	StaticText10 = new wxStaticText(Panel6, ID_STATICTEXT10, _("ID and Config Memory"), wxDefaultPosition, wxDefaultSize, 0, wxT("ID_STATICTEXT10"));
	BoxSizer13->Add(StaticText10, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	wxString __wxRadioBoxChoices_1[2] =
	{
		_("Binary"),
		_("Hexadecimal")
	};
	aIdBinHexRadio = new wxRadioBox(Panel6, ID_ID_BIN_HEX_RADIO, wxEmptyString, wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 1, wxRA_HORIZONTAL, wxDefaultValidator, wxT("ID_ID_BIN_HEX_RADIO"));
	BoxSizer13->Add(aIdBinHexRadio, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	aShowAllCfgCellsChk = new wxCheckBox(Panel6, ID_SHOW_ALL_CFG_CELLS_CHK, _("Show ALL locations"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("ID_SHOW_ALL_CFG_CELLS_CHK"));
	aShowAllCfgCellsChk->SetValue(false);
	BoxSizer13->Add(aShowAllCfgCellsChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	StaticText11 = new wxStaticText(Panel6, ID_STATICTEXT11, _("(* = reserved)"), wxDefaultPosition, wxDefaultSize, 0, wxT("ID_STATICTEXT11"));
	BoxSizer13->Add(StaticText11, 0, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 1);
	BoxSizer13->Add(0,5,0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	aDevIdTitle = new wxStaticText(Panel6, ID_DEV_ID_TITLE, _("Device ID WORD:"), wxDefaultPosition, wxDefaultSize, 0, wxT("ID_DEV_ID_TITLE"));
	BoxSizer13->Add(aDevIdTitle, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aDevId = new wxStaticText(Panel6, ID_DEV_ID, _("11 1111 1111 1111"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, wxT("ID_DEV_ID"));
	BoxSizer13->Add(aDevId, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aDevIdDecoded = new wxStaticText(Panel6, ID_DEV_ID_DECODED, _("= <unknown ID>"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, wxT("ID_DEV_ID_DECODED"));
	BoxSizer13->Add(aDevIdDecoded, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	BoxSizer13->Add(0,5,0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	aApplyIdLocsButton = new wxButton(Panel6, ID_APPLY_ID_LOCS_BUTTON, _("Apply edits"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("ID_APPLY_ID_LOCS_BUTTON"));
	aApplyIdLocsButton->Disable();
	BoxSizer13->Add(aApplyIdLocsButton, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 3);
	aHelpOnIdButton = new wxButton(Panel6, ID_BUTTON6, _("Help"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("ID_BUTTON6"));
	BoxSizer13->Add(aHelpOnIdButton, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 3);
	Panel6->SetSizer(BoxSizer13);
	BoxSizer13->Fit(Panel6);
	BoxSizer13->SetSizeHints(Panel6);
	BoxSizer12->Add(Panel6, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	SetSizer(BoxSizer12);
	BoxSizer12->Fit(this);
	BoxSizer12->SetSizeHints(this);

	Connect(ID_CFG_MEM_GRID,wxEVT_GRID_CELL_CHANGE,(wxObjectEventFunction)&TConfigMemoryPanel::onCfgMemGridCellChange);
	Connect(ID_ID_BIN_HEX_RADIO,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&TConfigMemoryPanel::onIdBinHexRadioSelect);
	Connect(ID_SHOW_ALL_CFG_CELLS_CHK,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TConfigMemoryPanel::onShowAllCfgCellsChkClick);
	Connect(ID_APPLY_ID_LOCS_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TConfigMemoryPanel::onApplyIdLocsButtonClick);
	Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TConfigMemoryPanel::onHelpOnIdButtonClick);
	//*)

    wxGridCellAttr *ReadOnlyAttr = new wxGridCellAttr;
    ReadOnlyAttr->SetReadOnly();
    aCfgMemGrid->SetColLabelValue(configMemSTAR, wxT("*"));
    aCfgMemGrid->SetColLabelValue(configMemINFO, _("Info"));
    aCfgMemGrid->SetColLabelValue(configMemVALUE, _("Value"));
    aCfgMemGrid->SetColSize(configMemSTAR, 20);
    aCfgMemGrid->SetColSize(configMemVALUE, 200);
    ReadOnlyAttr->IncRef();
    aCfgMemGrid->SetColAttr(configMemSTAR, ReadOnlyAttr);
    ReadOnlyAttr->IncRef();
    aCfgMemGrid->SetColAttr(configMemINFO, ReadOnlyAttr);
    aCfgMemGrid->EnableDragColMove(false);
    aCfgMemGrid->EnableDragRowSize(false);
    aCfgMemGrid->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
    ReadOnlyAttr->DecRef();
}

TConfigMemoryPanel::~TConfigMemoryPanel()
{
	//(*Destroy(TConfigMemoryPanel)
	//*)
}

//---------------------------------------------------------------------------
static wxChar *WordToSeparatedBinary(uint16_t w, int iNrBits)
{
    int j,k;
    static wxChar sz80Result[82];

    for (j=k=0;j<iNrBits;++j)
    {
        sz80Result[k++] = wxT('0') +  ( (w & (1<<(iNrBits-1-j)))!=0);
        if ( ((iNrBits-1-j)&3) == 0)
            sz80Result[k++] = wxT(' ');
    }
    sz80Result[k] = wxT('\0');
    return sz80Result;
}

//---------------------------------------------------------------------------
void TConfigMemoryPanel::ApplyIdLocationDisplay(void)
{
    int i,j;
    wxChar sz80Temp[82];
    wxChar *cp;
    uint16_t w;
    uint32_t dwAddress;
    bool ok;
    uint16_t wBitsPerIdLocation = PicDev_GetNrOfBitsPerIdLocation();  // 14 or 16 ?


    int RowCount = aCfgMemGrid->GetNumberRows();
    for (i=0; i<RowCount; ++i)
    {
        _tcsncpy(sz80Temp, aCfgMemGrid->GetRowLabelValue(i).c_str(), 80);
        dwAddress = HexStringToLongint(6,sz80Temp);
        _tcsncpy(sz80Temp, aCfgMemGrid->GetCellValue(i,configMemVALUE).c_str(), 80);
        w=0;
        ok=true;
        cp=sz80Temp;
        if ( (cp[0]==wxT('$')) || (cp[0]==wxT('0') && cp[1]==wxT('x')) )
        {
            w = HexStringToLongint(6,cp);
        }
        else // not HEX but BIN:
        {
            for (j=0;j<wBitsPerIdLocation;++j)
            {
                while (*cp==wxT(' ')) ++cp; // skip spaces, they are no syntax element here
                if (*cp==wxT('1'))
                {
                    w |= (1<<(wBitsPerIdLocation-1-j));
                    ++cp;
                }
                else if (*cp==wxT('0'))
                {
                    ++cp;
                }
                else ok=false;
            }
        }
        uint32_t Old;
        PicBuf_GetBufferWord(dwAddress, &Old);
        if (ok && (w != Old))
        {
            PicBuf_SetBufferWord (dwAddress, w);
            PicBuf_SetMemoryFlags(dwAddress, (PicBuf_GetMemoryFlags(dwAddress) | PIC_BUFFER_FLAG_USED)
                                                & ~(PIC_BUFFER_FLAG_PRG_ERROR|PIC_BUFFER_FLAG_VFY_ERROR));
        }
    } // end for < all GRID LINES >

} // end ApplyIdLocationDisplay()
//---------------------------------------------------------------------------


static const wxString Star (wxT("*"));

//---------------------------------------------------------------------------
void TConfigMemoryPanel::UpdateIdAndConfMemDisplay(void)
{
    int i;
// int j,k;
    int iGridLine;
// char sz80Temp[82];
    uint32_t dw, dwAddress;
    uint16_t wBitsPerIdLocation = PicDev_GetNrOfBitsPerIdLocation();
    wxString old_msg;

    ++(MainFrame::TheMainFrame->m_Updating);


    old_msg = MainFrame::TheMainFrame->aStatusBar->GetStatusText();
    MainFrame::TheMainFrame->aStatusBar->SetStatusText(_("Updating config-MEMORY-grid")) ;
    MainFrame::TheMainFrame->Update();


    // Update the CONFIG MEMORY TABLE (binary or hexadecimal dump, NO BIT-DECODERS)
//  SG_CfgMem->RowCount = 1+PIC_BUF_CONFIG_SIZE; // usually 1 + 64

//  SG_CfgMem->ColCount = 3;
//  // 2005-06-30: Why was this routine so UTTERLY slow - just because it's VCL stuff ? ?
//  //  ( took 3 seconds to fill this ridiculously small TStringGrid ! )
//  SG_CfgMem->Cols[0]->BeginUpdate();    // does this help to speed things up ?
//  SG_CfgMem->Cols[1]->BeginUpdate();
//  SG_CfgMem->Cols[2]->BeginUpdate();
    aCfgMemGrid->BeginBatch();
    int RowCount = aCfgMemGrid->GetNumberRows();
    if (RowCount > 0)
        aCfgMemGrid->DeleteRows(0, RowCount);

//  SG_CfgMem->Cells[0][0] = _("Address");   //  [ACol][ARow] ..
//  SG_CfgMem->Cells[1][0] = _("Info");
//  SG_CfgMem->Cells[2][0] = _("Value");

    for ( i=0,iGridLine=0 ; i<PIC_BUF_CONFIG_SIZE; ++i)
    {
        if ( PIC_DeviceInfo.iBitsPerInstruction == 24 )
        {
            // 24 bit per instruction in code memory .. looks like a dsPIC : only EVEN addresses useable
            dwAddress = PIC_DeviceInfo.lConfMemBase + 2*i;
        }
        else if ( PIC_DeviceInfo.iBitsPerInstruction == 16 )
        {
            // 16 bit per instruction in code memory .. may be 18Fxxxx, supported "one fine day" ?
            dwAddress = PIC_DeviceInfo.lConfMemBase + 2*i;
        }
        else // everything else is either PIC12F... or PIC16F... ?
        {
            dwAddress = PIC_DeviceInfo.lConfMemBase + i;
        }
        if ( (aShowAllCfgCellsChk->IsChecked()) || (PicDev_IsConfigMemLocationValid(dwAddress)) )
        {
            if ( PicBuf_GetBufferWord( dwAddress, &dw ) > 0 )
            {
                aCfgMemGrid->AppendRows();
                aCfgMemGrid->SetRowLabelValue(iGridLine, wxString::Format(wxT("0x%4.4X"), dwAddress)); // sprintf is smart enough to use 6 digits if necessary !
                aCfgMemGrid->SetCellValue(iGridLine, configMemSTAR, ( !PicDev_IsConfigMemLocationValid(dwAddress) ) ? Star : wxString(wxEmptyString));
//      if( !PicDev_IsConfigMemLocationValid(dwAddress) )
//        strcat( sz80Temp, wxT(" *") );
//      SG_CfgMem->Cells[0][iGridLine] = sz80Temp;
                aCfgMemGrid->SetCellValue(iGridLine, configMemINFO, PicDev_GetInfoOnConfigMemory( dwAddress ));
//      SG_CfgMem->Cells[1][iGridLine] = PicDev_GetInfoOnConfigMemory( dwAddress );
                aCfgMemGrid->SetCellValue(iGridLine, configMemVALUE,
                                          (aIdBinHexRadio->GetSelection()==radioDevIdBIN)
                                          ? wxString(WordToSeparatedBinary(dw, wBitsPerIdLocation))
                                          : wxString::Format(wxT("0x%4.4X"), dw));
//
//        memset(sz80Temp,0,80);
//        if(RB_IdBin->Checked)
//          SG_CfgMem->Cells[2][iGridLine] = WordToSeparatedBinary(dw, wBitsPerIdLocation);
//        else
//         {
//          sprintf(sz80Temp, wxT("0x%4.4X"), dw );
//          SG_CfgMem->Cells[2][iGridLine] = sz80Temp;
//         }
                iGridLine++;
                if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
                    APPL_LogEvent( _("UpdateIdAndConfMemDisplay: line %d "), iGridLine );
            } // end if < successfully retrieved data from a buffer for this location >
        }
    } // end for

    // Since 2005-05, for PIC10F20x: Here the USER ID LOCATIONS are located
    //  in a totally different memory area, which *MAY* be appended to the config bits:
    if (  (PIC_DeviceInfo.lIdMemBase > (PIC_DeviceInfo.lConfMemBase+PIC_BUF_CONFIG_SIZE) )
            ||(PIC_DeviceInfo.lIdMemBase < PIC_DeviceInfo.lConfMemBase )
       )
    {
        for ( i=0; i<PIC_DeviceInfo.lIdMemSize; ++i)
        {
            dwAddress = PIC_DeviceInfo.lIdMemBase + i;
            if ( PicBuf_GetBufferWord( dwAddress, &dw ) > 0 )
            {
                aCfgMemGrid->AppendRows();
                aCfgMemGrid->SetColLabelValue(iGridLine, wxString::Format(wxT("0x%4.4X"), dwAddress)); // sprintf is smart enough to use 6 digits if necessary !
//        sprintf(sz80Temp, wxT("0x%4.4X"), dwAddress); // sprintf is smart enough to use 6 digits if necessary !
//        SG_CfgMem->Cells[0][iGridLine] = sz80Temp;
                aCfgMemGrid->SetCellValue(iGridLine, configMemINFO, PicDev_GetInfoOnConfigMemory( dwAddress ));
//        SG_CfgMem->Cells[1][iGridLine] = PicDev_GetInfoOnConfigMemory( dwAddress );
                aCfgMemGrid->SetCellValue(iGridLine, configMemVALUE,
                                          (aIdBinHexRadio->GetSelection()==radioDevIdBIN)
                                          ? wxString(WordToSeparatedBinary(dw, wBitsPerIdLocation))
                                          : wxString::Format(wxT("0x%4.4X"), dw));
//            memset(sz80Temp,0,80);
//           if(RB_IdBin->Checked)
//              SG_CfgMem->Cells[2][iGridLine] = WordToSeparatedBinary(dw, wBitsPerIdLocation);
//           else
//            { sprintf(sz80Temp, wxT("0x%4.4X"), dw );
//              SG_CfgMem->Cells[2][iGridLine] = sz80Temp;
//            }
                iGridLine++;
                if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
                    APPL_LogEvent( _("UpdateIdAndConfMemDisplay: line %d "), (int)iGridLine );
            } // end if < successfully retrieved data from a buffer for this location >
        }
    } // end if < "ID memory" separated from "Config memory" >

//  if(iGridLine>1)
//        SG_CfgMem->RowCount = iGridLine;
//  else
//        SG_CfgMem->RowCount = 2;
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("UpdateIdAndConfMemDisplay: Calling EndUpdate()") );
    aCfgMemGrid->EndBatch();
//  SG_CfgMem->Cols[0]->EndUpdate();    // does this help to speed things up ?
//  SG_CfgMem->Cols[1]->EndUpdate();
//  SG_CfgMem->Cols[2]->EndUpdate();
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("UpdateIdAndConfMemDisplay: EndUpdate() finished") );

    if ( PicBuf_GetBufferWord( PIC_DeviceInfo.lDeviceIdAddr, &dw ) > 0 )
    {
        if (aIdBinHexRadio->GetSelection()==radioDevIdBIN)
            aDevId->SetLabel(WordToSeparatedBinary(dw, wBitsPerIdLocation));
        else
        {
            aDevId->SetLabel(wxString::Format(wxT("0x%4.4X"),(int)dw));
        }
    }
    else
    {
        dw=0;
        aDevId->SetLabel(_("<error>"));
    }

    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("UpdateIdAndConfMemDisplay: Searching Device Name by ID WORD...") );

    // Note 1: PicDev_GetDeviceNameByIdWord() can be TERRIBLY SLOW
    //         when called for the first time, due to the sluggish reading
    //         of the INI-file "devices.ini" .  But too late to change that now.
    // Note 2: Because of a device ID collosion between certain PIC16F's and PIC18F's
    //         (like PIC16F630 and PIC18F4220), PicDev_GetDeviceNameByIdWord()
    //         needs to know the number of bits per instruction word"; 14,16,24,..
    strcpy( m_sz80DetectedPicDevName, PicDev_GetDeviceNameByIdWord(dw, PIC_DeviceInfo.iBitsPerInstruction) );
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("UpdateIdAndConfMemDisplay: Finished searching Device Name") );


    MainFrame::TheMainFrame->m_fPicDeviceConflict = false;
    if ( m_sz80DetectedPicDevName[0]!=0 )
    {
        // successfully ''decoded' the Device ID Word. Selected the right device ?
        if ( strcmp(m_sz80DetectedPicDevName,PIC_DeviceInfo.sz40DeviceName) != 0)
        {
            if (dw!=0 && dw!=0xFFFF)
                MainFrame::TheMainFrame->m_fPicDeviceConflict = true;
        }
    }
    else // unknown Device ID Word.  Not an error, because some PICs don't support this.
    {
        strcpy(m_sz80DetectedPicDevName, "---------");
    }
    aDevIdDecoded->SetLabel(wxString::Format(wxT(" = %hs"), m_sz80DetectedPicDevName));
    if ( MainFrame::TheMainFrame->m_fPicDeviceConflict )
    {
        aDevIdTitle  ->SetForegroundColour(*wxRED);
        aDevId       ->SetForegroundColour(*wxRED);
        aDevIdDecoded->SetForegroundColour(*wxRED);
        MainFrame::TheMainFrame->aNotebook->SetSelection(MainFrame::TS_CfgMemTab);
//     PageControl1->ActivePage = TS_CfgMemTab;
    }
    else
    {
        wxColour NormalColour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
        aDevIdTitle  ->SetForegroundColour(NormalColour);
        aDevId       ->SetForegroundColour(NormalColour);
        aDevIdDecoded->SetForegroundColour(NormalColour);
    }

    MainFrame::TheMainFrame->m_update_id_and_config_display = false;  // "done"


    MainFrame::TheMainFrame->aStatusBar->SetStatusText(old_msg);
    MainFrame::TheMainFrame->Update();

    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("UpdateIdAndConfMemDisplay: finished") );

    --(MainFrame::TheMainFrame->m_Updating);

} // end UpdateIdAndConfMemDisplay()


//---------------------------------------------------------------------------
void TConfigMemoryPanel::onCfgMemGridCellChange(wxGridEvent& event)
//void ::SG_CfgMemSetEditText(TObject *Sender, int ACol,
//      int ARow, const wxString Value)
{
    if (!MainFrame::TheMainFrame->m_Updating)
        aApplyIdLocsButton->Enable();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void TConfigMemoryPanel::onIdBinHexRadioSelect(wxCommandEvent& event)
//void ::RB_IdBinClick(TObject *Sender)
{
    ApplyIdLocationDisplay();
    UpdateIdAndConfMemDisplay();  // ... in "normalized" form
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void TConfigMemoryPanel::onShowAllCfgCellsChkClick(wxCommandEvent& event)
//void ::Chk_ShowAllCfgCellsClick(TObject *Sender)
{
// ApplyIdLocationDisplay();
    UpdateIdAndConfMemDisplay();  // ... with the new settings
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void TConfigMemoryPanel::onApplyIdLocsButtonClick(wxCommandEvent& event)
//void ::Btn_ApplyIdLocsClick(TObject *Sender)
{
    ApplyIdLocationDisplay();
    UpdateIdAndConfMemDisplay();  // ... in "normalized" form
    aApplyIdLocsButton->Enable(false);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TConfigMemoryPanel::onHelpOnIdButtonClick(wxCommandEvent& event)
//void ::Btn_HelpOnIdLocsClick(TObject *Sender)
{
    wxHelpControllerBase *Help = TLanguage::GetHelpController();
    if (Help != NULL)
        Help->DisplaySection(TResource::HELPID_ID_LOCATIONS);
//  Application->HelpContext(HELPID_ID_LOCATIONS);
}
//---------------------------------------------------------------------------


