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
// int line,i,j,used_locations;
// int words_per_line;
// long i32ArrayIndex;
// int  text_index;
// int  line_length;
// wxChar szTemp[1024];
// wxChar *cp;    // buffer and pointer to assemble RTF string
// uint8_t b;
// uint32_t dwData, dwDisplayedAddress;
// uint16_t  wFlags, wOldFlags=0xFFFF;
// uint32_t dw;
    wxString old_msg;
// bool fOldModifyFlag;

//  fOldModifyFlag = REd_DataMem->Modified;
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

//  // Same as for the CODE MEMORY display:
//  //   Instead of the utterly slow "TRichEdit.Lines->Add()" - method,
//  //   use a TMemoryStream to build an RTF-file-compatible structure ..
//  pRtfMemStream = new TMemoryStream();
//  pRtfMemStream->Seek(0, soFromBeginning );
//  cp = szTemp;  // assemble next section in a good old C-string ...
//  RTF_GenerateHeader( cp/*destination*/,
//      (uint32_t)Pnl_DataMemColors->Font->Color, // "text" colour
//      (uint32_t)Pnl_DataMemColors->Color );     // "background" colour
//  RTF_SkipToEnd( &cp );
//  RTF_WriteStringToStream( pRtfMemStream, szTemp );  // et voila, RTF header is complete

    MainFrame::TheMainFrame->m_update_data_mem_display = false;

    aDataMemGrid->LoadHexData (aDataMemAddrGetter.SetMemory(PIC_BUF_DATA, i32NLocations), i32NLocations, aDataMemAddrGetter);
    aDataMemGrid->SetDefaultCellBackgroundColour (MainFrame::TheMainFrame->aOptionTab->aDataMemColourButton->GetBackgroundColour());
    aDataMemGrid->SetDefaultCellTextColour       (MainFrame::TheMainFrame->aOptionTab->aDataMemColourButton->GetForegroundColour());

//  words_per_line = 8;
//  try{
//    for(i32ArrayIndex=0, line=0; i32ArrayIndex<i32NLocations ; ++line )
//     {
//        dwDisplayedAddress = PicBuf_ArrayIndexToTargetAddress( PIC_BUF_DATA, i32ArrayIndex);
//        wFlags = PIC_BUFFER_FLAG_UNUSED;
//        for(i=0; (i<words_per_line) && (i32ArrayIndex<i32NLocations ) ; ++i)
//         {
//             if( PicBuf[PIC_BUF_DATA].pwFlags[i32ArrayIndex+i] & PIC_BUFFER_FLAG_USED)
//             { ++used_locations; wFlags |= PIC_BUFFER_FLAG_USED; }
//         }
//        cp = szTemp;   // pointer for "assembling" an RTF string
//
//        // Change the text colour for the address already, depending on the flags ?
//        if( wOldFlags != wFlags )
//         {
//            wOldFlags  = wFlags;
//            RTF_SetColorForMemFlags( &cp, wFlags );
//         }
//        _stprintf(cp, _("%06lX:"), dwDisplayedAddress);
//        RTF_SkipToEnd( &cp );
//
//        for(i=0; (i<words_per_line)
//              && (i32ArrayIndex<PIC_BUF_DATA_SIZE)
//              && (i32ArrayIndex<PIC_DeviceInfo.lDataEEPROMSizeInByte) ; ++i)
//         {
//           dwData = PicBuf[PIC_BUF_DATA].pdwData[i32ArrayIndex+i];
//           wFlags = PicBuf[PIC_BUF_DATA].pwFlags[i32ArrayIndex+i];
//
//           // Change the text colour for the output, depending on the flags ?
//           if( wOldFlags != wFlags )
//            {
//                wOldFlags  = wFlags;
//               RTF_SetColorForMemFlags( &cp, wFlags );
//            }
//           _stprintf(cp, _T(" %02X"), dwData & 0x00FF );
//           RTF_SkipToEnd( &cp );
//         }
//
//        _tcscpy(cp, _T("    ") );  // separator between HEX- and ASCII display
//        RTF_SkipToEnd( &cp );
//        for(i=0; (i<words_per_line)
//              && (i32ArrayIndex<PIC_BUF_DATA_SIZE)
//              && (i32ArrayIndex<PIC_DeviceInfo.lDataEEPROMSizeInByte) ; ++i)
//         {
//           b = PicBuf[PIC_BUF_DATA].pdwData[i32ArrayIndex+i];
//           if( (uint8_t)b==(uint8_t)'{' )// RTF treat's curly braces something special, so ESCAPE it
//            {
//              _tcscpy(cp, _T("\\{"));
//              cp+=2;
//            }
//           else if( (uint8_t)b==(uint8_t)'}' )
//            {
//                _tcscpy(cp, _T("\\}"));
//                cp+=2;
//            }
//           else if( b==_T('\\') )       // a SINGLE(!) backslash begins an RTF token, so escape it
//            {
//                _tcscpy(cp, _T("\\\\"));
//                cp+=2;  // caution, these are TWO backslashes
//            }
//           else if( b>=0x20 && b<=0x7F )
//                  *cp++ = b;   // "printable"
//             else
//                *cp++ = _T('.');       // "non-printable" (really ?)
//         }
//        strcpy(cp, _("\\par ") );   // Note: \par = END OF PARAGRAPH, important !
//        RTF_WriteStringToStream( pRtfMemStream, szTemp );  // append string to code memory dump
//
//        i32ArrayIndex += words_per_line;
//     } // end for (line..)
//  }
//  catch(...){
//    throw;
//  }

//  // Finish RTF file structure, and copy the stream into the Rich Edit control:
//  RTF_WriteStringToStream( pRtfMemStream, _("\\par }") );
//  aStatusBar->SetStatusText(_("Streaming DATA display..")) ;
//  Update();
//  REd_DataMem->PlainText = false;    // let the Rich Edit control accept RTF
//  REd_DataMem->Color = Pnl_DataMemColors->Color; // background colour (impossible via RTF code ?)
//  dw = pRtfMemStream->Size;          // get the length of the RTF in bytes
//  dw += dw/4;                        // add some safety margin
//  if(dw<65536) dw=65536;             // don't use less than 64 kByte for the text buffer
//  REd_DataMem->MaxLength = dw;       // adjust the Rich Edit's text buffer size
//  pRtfMemStream->Seek(0, soFromBeginning );
//  REd_DataMem->Lines->LoadFromStream(pRtfMemStream); // copy the stream into the Rich Edit control
//  delete pRtfMemStream;
//  pRtfMemStream = NULL;    // forget this pointer, it's no longer valid !
//
//  // Send an EM_SCROLLCARET message to scroll the caret into view:
//  SendMessage( REd_DataMem->Handle, // handle of destination window
//               EM_SCROLLCARET,      // message to send
//               0,                   // wParam, first message parameter
//               0 );                 // lParam, second message parameter
//  REd_DataMem->Modified = fOldModifyFlag;
    MainFrame::TheMainFrame->aStatusBar->SetStatusText(old_msg);
    MainFrame::TheMainFrame->Update();
} // end UpdateDataMemDisplay()
//---------------------------------------------------------------------------

