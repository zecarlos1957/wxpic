/*-------------------------------------------------------------------------*/
/*  Filename: DataMemPanel.cpp                                             */
/*                                                                         */
/*  Purpose:                                                               */
/*    Display and manage the panel that displays the data memory           */
/*                                                                         */
/*  Author:                                                                */
/*     Copyright 2009-2011 Philippe Chevrier pch @ laposte.net             */
/*     from software originally written by Wolfgang Buescher (DL4YHF)      */
/*                                                                         */
/*  License:                                                               */
/*     Licensed under GPLV3 conditions                                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#include "DataMemPanel.h"
#include "MainFrame.h"

//(*InternalHeaders(TDataMemPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <WinPicPr/PIC_HEX.h>

//(*IdInit(TDataMemPanel)
const long TDataMemPanel::ID_DATA_MEM_GRID = wxNewId();
//*)

BEGIN_EVENT_TABLE(TDataMemPanel,wxPanel)
	//(*EventTable(TDataMemPanel)
	//*)
END_EVENT_TABLE()

TDataMemPanel::TDataMemPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name)
{
	//(*Initialize(TDataMemPanel)
	wxBoxSizer* BoxSizer6;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	aDataMemGrid = new THexGrid(this, ID_DATA_MEM_GRID, wxDefaultPosition, wxDefaultSize, 0, _T("ID_DATA_MEM_GRID"));
	aDataMemGrid->CreateGrid(0,8);
	aDataMemGrid->SetMinSize(wxSize(0,0));
	for(int i=0;i<8;++i)aDataMemGrid->SetColLabelValue(i,wxString::Format(_T("%d"),i));
	BoxSizer6->Add(aDataMemGrid, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	SetSizer(BoxSizer6);
	BoxSizer6->Fit(this);
	BoxSizer6->SetSizeHints(this);
	//*)
}

TDataMemPanel::~TDataMemPanel()
{
	//(*Destroy(TDataMemPanel)
	//*)
}

//---------------------------------------------------------------------------
void TDataMemPanel::UpdateDataMemDisplay(void)
{
    long i32NLocations;
    wxString old_msg;

    i32NLocations = PIC_DeviceInfo.lDataEEPROMSizeInByte;
    if ( i32NLocations > 2*65536 )
        i32NLocations = 2*65536;
    if ( i32NLocations <= 0 )
    {
        Disable();
        return;
    }
    Enable();
    old_msg = MainFrame::TheMainFrame->aStatusBar->GetStatusText();
    MainFrame::TheMainFrame->aStatusBar->SetStatusText(_("Updating DATA display..")) ;
    MainFrame::TheMainFrame->Update();

    MainFrame::TheMainFrame->m_update_data_mem_display = false;

    aDataMemGrid->LoadHexData (aDataMemAddrGetter.SetMemory(PIC_BUF_DATA, i32NLocations), i32NLocations, aDataMemAddrGetter);
    aDataMemGrid->SetDefaultCellBackgroundColour (MainFrame::TheMainFrame->aOptionTab->aDataMemColourButton->GetBackgroundColour());
    aDataMemGrid->SetDefaultCellTextColour       (MainFrame::TheMainFrame->aOptionTab->aDataMemColourButton->GetForegroundColour());

    MainFrame::TheMainFrame->aStatusBar->SetStatusText(old_msg);
    MainFrame::TheMainFrame->Update();
} // end UpdateDataMemDisplay()
//---------------------------------------------------------------------------

