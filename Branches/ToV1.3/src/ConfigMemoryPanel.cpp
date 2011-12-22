#include "ConfigMemoryPanel.h"
#include "MainFrame.h"
#include "Appl.h"
#include "Language.h"
#include <../resources/Resource.h>

//(*InternalHeaders(TConfigMemoryPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/settings.h>

#include <WinPicPr/PIC_HEX.h>

//(*IdInit(TConfigMemoryPanel)
const long TConfigMemoryPanel::ID_CFG_MEM_GRID = wxNewId();
const long TConfigMemoryPanel::ID_STATICTEXT10 = wxNewId();
const long TConfigMemoryPanel::ID_ID_BIN_HEX_RADIO = wxNewId();
const long TConfigMemoryPanel::ID_SHOW_ALL_CFG_CELLS_CHK = wxNewId();
const long TConfigMemoryPanel::ID_STATICTEXT11 = wxNewId();
const long TConfigMemoryPanel::ID_DEV_ID_TITLE = wxNewId();
const long TConfigMemoryPanel::ID_DEV_ID = wxNewId();
const long TConfigMemoryPanel::ID_DEV_ID_DECODED = wxNewId();
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

	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
	aCfgMemGrid = new wxGrid(this, ID_CFG_MEM_GRID, wxDefaultPosition, wxSize(-1,0), 0, _T("ID_CFG_MEM_GRID"));
	aCfgMemGrid->CreateGrid(0,3);
	aCfgMemGrid->SetMinSize(wxSize(0,0));
	aCfgMemGrid->EnableEditing(true);
	aCfgMemGrid->EnableGridLines(true);
	aCfgMemGrid->SetDefaultCellFont( aCfgMemGrid->GetFont() );
	aCfgMemGrid->SetDefaultCellTextColour( aCfgMemGrid->GetForegroundColour() );
	BoxSizer12->Add(aCfgMemGrid, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 7);
	Panel6 = new wxPanel(this, ID_PANEL6, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL6"));
	BoxSizer13 = new wxBoxSizer(wxVERTICAL);
	StaticText10 = new wxStaticText(Panel6, ID_STATICTEXT10, _("ID and Config Memory"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	BoxSizer13->Add(StaticText10, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	wxString __wxRadioBoxChoices_1[2] =
	{
		_("Binary"),
		_("Hexadecimal")
	};
	aIdBinHexRadio = new wxRadioBox(Panel6, ID_ID_BIN_HEX_RADIO, wxEmptyString, wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 1, wxRA_HORIZONTAL, wxDefaultValidator, _T("ID_ID_BIN_HEX_RADIO"));
	BoxSizer13->Add(aIdBinHexRadio, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	aShowAllCfgCellsChk = new wxCheckBox(Panel6, ID_SHOW_ALL_CFG_CELLS_CHK, _("Show ALL locations"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SHOW_ALL_CFG_CELLS_CHK"));
	aShowAllCfgCellsChk->SetValue(false);
	BoxSizer13->Add(aShowAllCfgCellsChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	StaticText11 = new wxStaticText(Panel6, ID_STATICTEXT11, _("(* = reserved)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	BoxSizer13->Add(StaticText11, 0, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 1);
	BoxSizer13->Add(0,5,0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	aDevIdTitle = new wxStaticText(Panel6, ID_DEV_ID_TITLE, _("Device ID WORD:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_DEV_ID_TITLE"));
	BoxSizer13->Add(aDevIdTitle, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aDevId = new wxStaticText(Panel6, ID_DEV_ID, _("11 1111 1111 1111"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_DEV_ID"));
	BoxSizer13->Add(aDevId, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aDevIdDecoded = new wxStaticText(Panel6, ID_DEV_ID_DECODED, _("= <unknown ID>"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_DEV_ID_DECODED"));
	BoxSizer13->Add(aDevIdDecoded, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	BoxSizer13->Add(0,15,0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	aHelpOnIdButton = new wxButton(Panel6, ID_BUTTON6, _("Help"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
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
	Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TConfigMemoryPanel::onHelpOnIdButtonClick);
	//*)

    wxGridCellAttr *ReadOnlyAttr = new wxGridCellAttr;
    ReadOnlyAttr->SetReadOnly();
    aCfgMemGrid->SetColLabelValue(configMemSTAR, _T("*"));
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
        sz80Result[k++] = _T('0') +  ( (w & (1<<(iNrBits-1-j)))!=0);
        if ( ((iNrBits-1-j)&3) == 0)
            sz80Result[k++] = _T(' ');
    }
    sz80Result[k] = _T('\0');
    return sz80Result;
}



void TConfigMemoryPanel::LoadConfigBuffer (void)
{
    //-- Load the MemAddressGetter for the configuration
    aCfgBufferBase = aCfgMemAddrGetter.SetMemory(PIC_BUF_CONFIG, PicDev_GetConfMemSize());

    //-- Compute Index of the First Config Word
    if (PIC_DeviceInfo.lConfWordAdr >= 0)
        aConfigWordIndex = PicBuf[PIC_BUF_CONFIG].AddressToTargetArrayIndex(PIC_DeviceInfo.lConfWordAdr);

    // Since 2005-05, for PIC10F20x: Here the USER ID LOCATIONS are located
    //  in a totally different memory area, which *MAY* be appended to the config bits:
    isIdSeparated = (  (PIC_DeviceInfo.lIdMemBase > (PIC_DeviceInfo.lConfMemBase+PIC_BUF_CONFIG_SIZE) )
                      || (PIC_DeviceInfo.lIdMemBase <  PIC_DeviceInfo.lConfMemBase )  );
    if (isIdSeparated)
        aIdBufferBase = aIdMemAddrGetter.SetMemory(PIC_BUF_ID_LOCATIONS, PIC_DeviceInfo.lIdMemSize);

    UpdateIdAndConfMemDisplay(/*Rebuild*/true);
    // After loading the configuration Buffer, the Config word must be updated
    MainFrame::TheMainFrame->aDeviceCfgTab->UpdateDeviceConfigTab(true/*fUpdateHexWord*/ );
}


static const wxString Star (_T("*"));

//---------------------------------------------------------------------------
void TConfigMemoryPanel::UpdateIdAndConfMemDisplay (bool pRebuild)
{
    ++(MainFrame::TheMainFrame->m_Updating);

    aCfgMemGrid->BeginBatch();

    if (pRebuild)
    {
        //-- Remove existing rows from the grid before recreating them
        int RowCount = aCfgMemGrid->GetNumberRows();
        if (RowCount > 0)
            aCfgMemGrid->DeleteRows(0, RowCount);
    }

    int iGridLine = 0;
    //-- For each word
    for ( int i=0; i<PIC_BUF_CONFIG_SIZE; ++i)
    {
        uint32_t dwAddress = aCfgMemAddrGetter(i).address;
        bool Valid = PicDev_IsConfigMemLocationValid(dwAddress);
        if (Valid || aShowAllCfgCellsChk->IsChecked())
        {
            uint32_t WordValue = aCfgBufferBase[i];
            if (pRebuild)
            {
                //-- Recreate and fill all the columns
                aCfgMemGrid->AppendRows();
                aCfgMemGrid->SetRowLabelValue(iGridLine, wxString::Format(_T("0x%4.4X"), dwAddress)); // sprintf is smart enough to use 6 digits if necessary !
                aCfgMemGrid->SetCellValue(iGridLine, configMemSTAR, ( !Valid ) ? Star : wxString(wxEmptyString));
                aCfgMemGrid->SetCellValue(iGridLine, configMemINFO, PicDev_GetInfoOnConfigMemory( dwAddress ));
            }
            //-- Only the value column is updated when not rebuilding all
            setWordValueAtGridLine (iGridLine, WordValue);
            iGridLine++;
        }
    } // end for

    if (isIdSeparated)
    {
        aFirstId = iGridLine;
        for ( int i=0; i<PIC_DeviceInfo.lIdMemSize; ++i)
        {
            uint32_t dwAddress = aIdMemAddrGetter(i).address;
            uint32_t WordValue = aIdBufferBase[i];

            if (pRebuild)
            {
                aCfgMemGrid->AppendRows();
                aCfgMemGrid->SetRowLabelValue(iGridLine, wxString::Format(_T("0x%4.4X"), dwAddress)); // sprintf is smart enough to use 6 digits if necessary !
                aCfgMemGrid->SetCellValue(iGridLine, configMemINFO, PicDev_GetInfoOnConfigMemory( dwAddress ));
            }
            setWordValueAtGridLine (iGridLine, WordValue);
            iGridLine++;
        }
    } // end if < "ID memory" separated from "Config memory" >

    aCfgMemGrid->EndBatch();

    //-- Display the device ID word
    uint32_t WordValue = 0;
    aDevId->SetLabel( (PicBuf_GetBufferWord(PIC_DeviceInfo.lDeviceIdAddr, &WordValue) > 0)
                     ? getWordValueImage (WordValue)
                     : wxString(_("<error>")) );

    // Note 1: PicDev_GetDeviceNameByIdWord() can be TERRIBLY SLOW
    //         when called for the first time, due to the sluggish reading
    //         of the INI-file "devices.ini" .  But too late to change that now.
    // Note 2: Because of a device ID collosion between certain PIC16F's and PIC18F's
    //         (like PIC16F630 and PIC18F4220), PicDev_GetDeviceNameByIdWord()
    //         needs to know the number of bits per instruction word"; 14,16,24,..
    strcpy( m_sz80DetectedPicDevName, PicDev_GetDeviceNameByIdWord(WordValue, PIC_DeviceInfo.iBitsPerInstruction) );

    MainFrame::TheMainFrame->m_fPicDeviceConflict = false;
    if ( m_sz80DetectedPicDevName[0]!=0 )
    {
        // successfully ''decoded' the Device ID Word. Selected the right device ?
        if ((WordValue!=0) && (WordValue!=0xFFFF) && (strcmp(m_sz80DetectedPicDevName,PIC_DeviceInfo.sz40DeviceName) != 0))
            MainFrame::TheMainFrame->m_fPicDeviceConflict = true;
    }
    else // unknown Device ID Word.  Not an error, because some PICs don't support this.
        strcpy(m_sz80DetectedPicDevName, "---------");

    aDevIdDecoded->SetLabel(wxString::Format(_T(" = %hs"), m_sz80DetectedPicDevName));
    if ( MainFrame::TheMainFrame->m_fPicDeviceConflict )
    {
        aDevIdTitle  ->SetForegroundColour(*wxRED);
        aDevId       ->SetForegroundColour(*wxRED);
        aDevIdDecoded->SetForegroundColour(*wxRED);
        if (pRebuild)
            MainFrame::TheMainFrame->aNotebook->SetSelection(MainFrame::TS_CfgMemTab);
    }
    else
    {
        wxColour NormalColour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
        aDevIdTitle  ->SetForegroundColour(NormalColour);
        aDevId       ->SetForegroundColour(NormalColour);
        aDevIdDecoded->SetForegroundColour(NormalColour);
    }

    MainFrame::TheMainFrame->m_update_id_and_config_display = false;  // "done"

    MainFrame::TheMainFrame->Update();

    --(MainFrame::TheMainFrame->m_Updating);

} // end UpdateIdAndConfMemDisplay()


void TConfigMemoryPanel::ApplyConfigEdit (void)
{
    aCfgMemAddrGetter.ApplyChange();
    if (isIdSeparated)
        aIdMemAddrGetter.ApplyChange();
}


wxString  TConfigMemoryPanel::getWordValueImage (int pWordValue)
{
    return (aIdBinHexRadio->GetSelection()==radioDevIdBIN)
              ? wxString(WordToSeparatedBinary(pWordValue, PicDev_GetNrOfBitsPerIdLocation()))
              : wxString::Format(_T("0x%4.4X"), pWordValue);
}

void TConfigMemoryPanel::setWordValueAtGridLine (int pGridLine, int pWordValue)
{
    aCfgMemGrid->SetCellValue(pGridLine, configMemVALUE, getWordValueImage(pWordValue));
}



//---------------------------------------------------------------------------
void TConfigMemoryPanel::onCfgMemGridCellChange(wxGridEvent& event)
//void ::SG_CfgMemSetEditText(TObject *Sender, int ACol,
//      int ARow, const wxString Value)
{
    if (!MainFrame::TheMainFrame->m_Updating)
    {
        int EventRow = event.GetRow();

        uint32_t EnteredValue = 0;
        bool     Error        = false;

        const wxChar *CurChar = aCfgMemGrid->GetCellValue(EventRow,configMemVALUE).c_str();
        while (*CurChar==_T(' '))
            ++CurChar; // skip spaces, they are no syntax element here

        const wxChar *ValueString = CurChar;
        int BitCount = 0;
        for (BitCount = 0; *CurChar != _T('\0'); ++BitCount, ++CurChar)
        {
            if (*CurChar == _T(' '))
                --BitCount; //-- Don't count Space as a bit

            else
            {
                EnteredValue <<= 1;
                if (*CurChar == _T('1'))
                    EnteredValue |= 1;

                else if (*CurChar != _T('0'))
                    Error = true;
            }
        }
        if (BitCount != PicDev_GetNrOfBitsPerIdLocation())
            Error = true;
        if (Error)
            // not BIN but HEX may be:
            Error = !HexStringToLongint(6, ValueString, &EnteredValue);

        if (!Error)
        {
            bool UseIdBuffer = isIdSeparated && (EventRow >= aFirstId);
            TMemAddrGetter *AddrGetter = (UseIdBuffer) ? &aIdMemAddrGetter : &aCfgMemAddrGetter;
            uint32_t       *Buffer     = (UseIdBuffer) ? aIdBufferBase     : aCfgBufferBase;
            //-- Get the row address by reading the label
            uint32_t        dwAddress;
            HexStringToLongint(6, aCfgMemGrid->GetRowLabelValue(EventRow).c_str(), &dwAddress);

            uint32_t        Index      = AddrGetter->AddressToTargetArrayIndex(dwAddress);

            //-- Get old value to verify it has already changed
            if (EnteredValue != Buffer[Index])
            {
                Buffer[Index] = EnteredValue;
                AddrGetter->SetModified(Index);

                if ((Index == aConfigWordIndex) || ((PIC_DeviceInfo.wCfgmask2_used != 0x0000) && (Index == aConfigWordIndex+1)))
                    MainFrame::TheMainFrame->aDeviceCfgTab->UpdateCfgWordValue();
            }
            setWordValueAtGridLine (EventRow, EnteredValue);
        }
        else
            wxBell();
    }
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void TConfigMemoryPanel::onIdBinHexRadioSelect(wxCommandEvent& event)
//void ::RB_IdBinClick(TObject *Sender)
{
    UpdateIdAndConfMemDisplay(/*Rebuild*/false);  // ... in "normalized" form
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void TConfigMemoryPanel::onShowAllCfgCellsChkClick(wxCommandEvent& event)
//void ::Chk_ShowAllCfgCellsClick(TObject *Sender)
{
    UpdateIdAndConfMemDisplay(/*Rebuild*/true);  // ... with the new settings
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


