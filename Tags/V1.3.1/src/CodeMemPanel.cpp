/*-------------------------------------------------------------------------*/
/*  Filename: CodeMemPanel.cpp                                             */
/*                                                                         */
/*  Purpose:                                                               */
/*     Display and manage the Code Memory Panel                            */
/*                                                                         */
/*  Author:                                                                */
/*     Copyright 2009-2011 Philippe Chevrier pch @ laposte.net             */
/*                                                                         */
/*  License:                                                               */
/*     Licensed under GPLV3 conditions                                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#include "CodeMemPanel.h"
#include "MainFrame.h"

//(*InternalHeaders(TCodeMemPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <WinPicPr/PIC_HEX.h>

//(*IdInit(TCodeMemPanel)
const long TCodeMemPanel::ID_CODE_MEM_GRID = wxNewId();
//*)

BEGIN_EVENT_TABLE(TCodeMemPanel,wxPanel)
	//(*EventTable(TCodeMemPanel)
	//*)
END_EVENT_TABLE()

TCodeMemPanel::TCodeMemPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name)
{
	//(*Initialize(TCodeMemPanel)
	wxBoxSizer* BoxSizer5;
	
	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	aCodeMemGrid = new THexGrid(this, ID_CODE_MEM_GRID, wxDefaultPosition, wxDefaultSize, 0, _T("ID_CODE_MEM_GRID"));
	aCodeMemGrid->CreateGrid(0,8);
	aCodeMemGrid->SetMinSize(wxSize(0,0));
	aCodeMemGrid->EnableEditing(true);
	aCodeMemGrid->EnableGridLines(true);
	aCodeMemGrid->SetDefaultCellFont( aCodeMemGrid->GetFont() );
	aCodeMemGrid->SetDefaultCellTextColour( aCodeMemGrid->GetForegroundColour() );
	BoxSizer5->Add(aCodeMemGrid, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	SetSizer(BoxSizer5);
	BoxSizer5->Fit(this);
	BoxSizer5->SetSizeHints(this);
	//*)
}

TCodeMemPanel::~TCodeMemPanel()
{
	//(*Destroy(TCodeMemPanel)
	//*)
}



//---------------------------------------------------------------------------
void TCodeMemPanel::UpdateCodeMemDisplay(void)
{
    MainFrame::TheMainFrame->m_update_code_mem_display = false;


    aCodeMemGrid->SetDefaultCellBackgroundColour (MainFrame::TheMainFrame->aOptionTab->aCodeMemColourButton->GetBackgroundColour());
    aCodeMemGrid->SetDefaultCellTextColour       (MainFrame::TheMainFrame->aOptionTab->aCodeMemColourButton->GetForegroundColour());

    //  Speciality for PICs with so-called EXECUTIVE MEMORY (kind of 'bootloader'):
    //  - If the executive code buffer is loaded, only show the 'ordinary' memory
    //    locations which really contain 'something' .
    //  - If the executive code buffer is not loaded, don't show the executive memory
    //    at all, but instead show ALL locations in the 'ordinary' code buffer .
    int TotalLocation = std::min(PIC_DeviceInfo.lCodeMemSize, (long)PicBuf[PIC_BUF_CODE].dwMaxSize);
    aCodeMemGrid->SetHexFormat(PIC_DeviceInfo.iBitsPerInstruction);
    if ( PicBuf[PIC_BUF_EXEC].dwMaxSize>0 && PicBuf[PIC_BUF_EXEC].i32LastUsedArrayIndex>0 )
    {
        // there seems to be "loaded" EXECUTIVE CODE MEMORY -> show multiple sections !
        if ( PicBuf[PIC_BUF_CODE].i32LastUsedArrayIndex >=/*!*/ 0 )
            aCodeMemGrid->LoadHexData (aCodeMemAddrGetter1.SetMemory(PIC_BUF_CODE, TotalLocation), TotalLocation, aCodeMemAddrGetter1, _T("User Code Memory"), 0);
        else
            aCodeMemGrid->UnloadData(0);
        int TotalExecLocation = std::min(PicBuf[PIC_BUF_EXEC].i32LastUsedArrayIndex+1, (long)PicBuf[PIC_BUF_EXEC].dwMaxSize);
        aCodeMemGrid->LoadHexData (aCodeMemAddrGetter2.SetMemory(PIC_BUF_EXEC, TotalExecLocation), TotalExecLocation, aCodeMemAddrGetter2, _T("Executive Code Memory"), 1);
    }
    else // no EXECUTIVE MEMORY, only 'ordinary' code memory : No need to show what it is !
    {
        aCodeMemGrid->LoadHexData (aCodeMemAddrGetter1.SetMemory(PIC_BUF_CODE, TotalLocation), TotalLocation, aCodeMemAddrGetter1);
        aCodeMemGrid->UnloadData(1);
        aCodeMemAddrGetter2.Invalidate();
    }

    MainFrame::TheMainFrame->Update();

} // end UpdateCodeMemDisplay()
//---------------------------------------------------------------------------


