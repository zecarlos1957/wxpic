#include "CodeMemPanel.h"
#include "MainFrame.h"

//(*InternalHeaders(TCodeMemPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <WinPic/WinPicPr/PIC_HEX.h>

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


////---------------------------------------------------------------------------
//void RTF_WriteStringToStream( TMemoryStream * pms, char *pszSrc )
//{
//  pms->Write( pszSrc, strlen(pszSrc) );
//}

////---------------------------------------------------------------------------
//void RTF_SkipToEnd( char **cpp )
//{
//  char *cp=*cpp;
//  while(*cp) ++cp;
//  *cpp = cp;
//}

//void TCodeMemPanel::MakeWeakCodeColour(void)
//{
//    wxColour FGColour = aCodeMemGrid->GetDefaultCellTextColour();
//    wxColour BGColour = aCodeMemGrid->GetDefaultCellBackgroundColour();
//    unsigned char iRed  = ( FGColour.Red()   + BGColour.Red()   ) / 2;
//    unsigned char iGreen= ( FGColour.Green() + BGColour.Green() ) / 2;
//    unsigned char iBlue = ( FGColour.Blue()  + BGColour.Blue()  ) / 2;
//    aWeakCodeColour.Set(iRed, iGreen, iBlue);
//}

////---------------------------------------------------------------------------
//void RTF_SetColorForMemFlags( char **ppszDest, WORD wMemFlags )
//{
//  // Bring some colour into the Code Display, using RTF tags :
//  //  \\cf16 = "weak" colour for unused locations,
//  //  \\cf17 = "strong" colour for used locations,
//  //  \\cf18 = "error" colour for bad locations,
//  //  \\cf19 = "special" colour to mark the OSCILLATOR CALIBRATION WORD (etc).
//  if( wMemFlags & (PIC_BUFFER_FLAG_PRG_ERROR | PIC_BUFFER_FLAG_VFY_ERROR | PIC_BUFFER_FLAG_DEAD ) )
//    strcpy( *ppszDest, "\\cf18 " ); // note the important SPACE at the end !
//  else if( wMemFlags & (PIC_BUFFER_FLAG_SPECIAL) )
//    strcpy( *ppszDest, "\\cf19 " );
//  else if( wMemFlags & (PIC_BUFFER_FLAG_USED) )
//    strcpy( *ppszDest, "\\cf17 " );
//  else
//    strcpy( *ppszDest, "\\cf16 " );
//  RTF_SkipToEnd( ppszDest );
//}
//
////---------------------------------------------------------------------------
//void RTF_GenerateHeader( char *cp/*destination*/,
//                          uint32_t dwRGBTextColor,
//                          uint32_t dwRGBBackgndColor  )
//   // Generates a Rich Text File header IN MEMORY .
//{
// int  i, iRed, iGreen, iBlue;
//
//  // Create a default font table in the RTF header :
//  strcpy( cp, "{\\rtf1\\ansi\\deff0\\deftab720"\
//    "{\\fonttbl{\\f0\\fswiss MS SansSerif;}"\
//              "{\\f1\\froman\\fcharset2 Symbol;}"\
//              "{\\f2\\fmodern Courier New;}}\r\n" );
//  RTF_SkipToEnd( &cp );
//  // Create "simple" colour table in the RTF header (DOS-text-compatible!) :
//  //    0=BLACK      1=BLUE           2=GREEN       3=CYAN
//  //    4=RED        5=MAGENTA        6=BROWN       7=LIGHTGRAY
//  //    8=DARKGRAY   9=LIGHTBLUE     10=LIGHTGREEN 11=LIGHTCYAN
//  //    12=LIGHTRED 13=LIGHTMAGENTA  14=YELLOW     15=WHITE
//  strcpy( cp, "{\\colortbl"\
//      "\\red0\\green0\\blue0;\\red0\\green0\\blue192;\\red0\\green127\\blue0;\\red0\\green192\\blue192;"\
//      "\\red192\\green0\\blue0;\\red192\\green0\\blue192;\\red192\\green128\\blue0;\\red192\\green192\\blue192;"\
//      "\\red63\\green63\\blue63;\\red63\\green63\\blue255;\\red0\\green255\\blue0;\\red0\\green255\\blue255;"\
//      "\\red255\\green0\\blue0;\\red255\\green0\\blue255;\\red255\\green255\\blue0;\\red127\\green127\\blue127;" );
//  RTF_SkipToEnd( &cp );
//  // Append some "custom" colours for the HEX DUMP :
//  //    16 = "weak" colour for unused location,
//  //    17 = "strong" colour for used locations
//  //    18 = "error" colour for bad locations
//  //    18 = "special" colour to mark the OSCILLATOR CALIBRATION WORD (etc)
//  // Get the RGB-mix for the "weak" colour in the memory dump :
//  iRed  = (  (dwRGBTextColor & 0x000FF) + ( dwRGBBackgndColor & 0x000FF) ) / 2;
//  iGreen= ((((dwRGBTextColor >> 8) & 0x000FF) + ((dwRGBBackgndColor >> 8) & 0x000FF) ) / 2);
//  iBlue = ((((dwRGBTextColor >> 16) & 0x000FF)+ ((dwRGBBackgndColor >> 16) & 0x000FF) ) / 2);
//  sprintf(cp, "\\red%d\\green%d\\blue%d;", iRed, iGreen, iBlue );  // colour #16
//  RTF_SkipToEnd( &cp );
//  // Get the RGB-mix for the "strong" colour in the memory dump :
//  iRed  =  dwRGBTextColor & 0x000FF;
//  iGreen= (dwRGBTextColor >> 8) & 0x000FF;
//  iBlue = (dwRGBTextColor >> 16) & 0x000FF;
//  sprintf(cp, "\\red%d\\green%d\\blue%d;", iRed, iGreen, iBlue );  // colour #17
//  RTF_SkipToEnd( &cp );
//  strcpy(cp, "\\red255\\green0\\blue0;" );  // colour #18
//  RTF_SkipToEnd( &cp );
//  strcpy(cp, "\\red255\\green0\\blue255;"); // colour #19
//  RTF_SkipToEnd( &cp );
//  strcpy(cp, "}\r\n" );                     // end of the "colortbl" list
//  RTF_SkipToEnd( &cp );
//  strcpy( cp, "\\deflang1033\\pard\\plain\\cf8\\f2\\fs20 "); // important trailing space !
//
//} // end RTF_GenerateHeader()



////---------------------------------------------------------------------------
//int MainFrame::UpdateCodeMemRange( int iWhichMemory, long i32NLocations, int iUCMoptions )
//{
// int line, total_lines, used_locations, i,j;
// int words_per_line;
// long i32ArrayIndex;
// int  text_index;
//// wxChar szTemp[1024];
//// wxChar *cp;    // buffer and pointer to assemble RTF string
// wxString s;
// WORD wFlags, wOldFlags=0xFFFF;
// uint32_t dw, dwData, dwDisplayedAddress;
//
//
//  // Reject invalid arguments:
//  if(iWhichMemory<0 || iWhichMemory>PIC_BUFFERS || i32NLocations<1 || iStartingRow<0)
//     return 0;
//  if(PicBuf[iWhichMemory].dwMaxSize == 0)
//     return 0;  // memory buffer has not been allocated properly, don't access it
//  if(i32NLocations > (long)PicBuf[iWhichMemory].dwMaxSize)
//     i32NLocations = (long)PicBuf[iWhichMemory].dwMaxSize; // obey buffer size limit
//
//
//  words_per_line = 8;
//  total_lines    = 0;
//
//  // Individual colors in a rich text edit control can be achieved
//  //    by the property SelAttributes. See Borland's help on TRichEdit....
//  //    but individual colors are not too simple :-(
//  try{
//    for(i32ArrayIndex=0, line=0; (line<=8192) && (i32ArrayIndex<i32NLocations); ++line )
//     {  // 2005-02: increased number of lines, for up to 64k instructions
//      // no longer needed, fast enough : APPL_ShowProgress( (100*i32ArrayIndex) / i32NLocations );
//
//      // Only add the line if it contains USED locations, or if "all" locations shall be displayed:
//      used_locations = 0;
//      wFlags = PIC_BUFFER_FLAG_UNUSED;
//      for(i=0; (i<words_per_line) && (i32ArrayIndex<i32NLocations ) ; ++i)
//       { if( PicBuf[iWhichMemory].pwFlags[i32ArrayIndex+i] & PIC_BUFFER_FLAG_USED)
//           { ++used_locations; wFlags |= PIC_BUFFER_FLAG_USED; }
//       }
//      if( (used_locations>0) || (iUCMoptions & UCM_SHOW_ALL_LOCATIONS) )
//       {
//        dwDisplayedAddress = PicBuf_ArrayIndexToTargetAddress( iWhichMemory, i32ArrayIndex );
////        cp = szTemp;   // pointer for "assembling" an RTF string
//
////        // Change the text colour for the address already, depending on the flags ?
////        if( wOldFlags != wFlags )
////         {  wOldFlags  = wFlags;
////            RTF_SetColorForMemFlags( &cp, wFlags );
////         }
//
//          if( wFlags & (PIC_BUFFER_FLAG_PRG_ERROR | PIC_BUFFER_FLAG_VFY_ERROR | PIC_BUFFER_FLAG_DEAD ) )
////            strcpy( *ppszDest, "\\cf18 " ); // note the important SPACE at the end !
//          else if( wFlags & (PIC_BUFFER_FLAG_SPECIAL) )
////            strcpy( *ppszDest, "\\cf19 " );
//          else if( wFlags & (PIC_BUFFER_FLAG_USED) )
////            strcpy( *ppszDest, "\\cf17 " );
//          else
////            strcpy( *ppszDest, "\\cf16 " );
//
//        _stprintf(cp, _T("%06lX:"), dwDisplayedAddress);
//        RTF_SkipToEnd( &cp );
//
//        // Now add the hex data words to the RTF structure :
//        for(i=0; (i<words_per_line) && (i32ArrayIndex<i32NLocations ) ; ++i)
//         {
//          dwData = PicBuf[iWhichMemory].pdwData[i32ArrayIndex+i];
//          wFlags = PicBuf[iWhichMemory].pwFlags[i32ArrayIndex+i];
//          if( (i32ArrayIndex+i) == PIC_DeviceInfo.lAddressOscCal)
//             wFlags |= PIC_BUFFER_FLAG_SPECIAL;
//
//          // Change the text colour for the output, depending on the flags ?
//          if( wOldFlags != wFlags )
//           {  wOldFlags  = wFlags;
//              RTF_SetColorForMemFlags( &cp, wFlags );
//           }
//
//          if( PIC_DeviceInfo.iBitsPerInstruction>16 )
//           { // 24 bits per "instruction word", means 6 hex digits per value :
//             _stprintf(cp, _T(" %06lX"), dwData );
//           }
//          else if( PIC_DeviceInfo.iBitsPerInstruction>12 )
//           { // 14..16 bits per "instruction word" :
//             _stprintf(cp, _T(" %04lX"), dwData );
//           }
//          else
//           { // 12 or less bits per "instruction word" :
//             _stprintf(cp, _T(" %03lX"), dwData );
//           }
//          RTF_SkipToEnd( &cp );
//         } // end for < N locations per line >
//
//        _tcscpy(cp, _T("\\par ") );   // Note: \par = END OF PARAGRAPH, important !
//        RTF_WriteStringToStream( pRtfMemStream, szTemp );  // append string to code memory dump
//
//        ++total_lines;
//       } // end if( (iUCMoptions & UCM_SHOW_ALL_LOCATIONS) || used_locations>0 )
//      i32ArrayIndex += words_per_line;
//     } // end for (line..)
//
//  }
//  catch(...){
//    throw;
//  }
//
//} // end UpdateCodeMemRange()

////---------------------------------------------------------------------------
//bool WinPic_ApplyHexDumpLine( const char *pszSource )
//{
//  uint32_t dwAddr, dwData, dwOldData;
////  int  i;
//  int nBytesPerLine;
//
//  if(! PicHex_GetHexValueFromSource( &pszSource, &dwAddr ) )  // parse the address
//       return false;
//
//  if(*pszSource==_T(':'))
//    ++pszSource;
//  else
//    return false;
//
//
//  // There is a trick to enter data as ASCII characters:
//  //    Erase all the hex DATA fields, modify the ASCII chars.
//  //    If there are no HEX values, parse the ASCII data later.
//  nBytesPerLine = 0;
//  while(  (nBytesPerLine<8) &&   // FIRST get the hex values
//       PicHex_GetHexValueFromSource( &pszSource, &dwData ) )
//   {
//     if( PicBuf_GetBufferWord( dwAddr, &dwOldData) > 0 )
//      {
//       if(dwData != dwOldData)
//        { PicBuf_SetMemoryFlags(dwAddr, PicBuf_GetMemoryFlags(dwAddr) | PIC_BUFFER_FLAG_USED);
//        }
//       PicBuf_SetBufferWord(dwAddr,  (WORD)dwData);
//      }
//     ++dwAddr;
//     ++nBytesPerLine;
//   }
//
//
//
//  while( (nBytesPerLine<8) &&    // .. parse any remaining chars in ASCII
//     *pszSource>=32 )
//   { dwData    = (int)(*pszSource++);
//     if( PicBuf_GetBufferWord(dwAddr, &dwOldData) > 0 )
//      {
//       if(dwData != dwOldData)
//        { PicBuf_SetMemoryFlags(dwAddr, PicBuf_GetMemoryFlags(dwAddr) | PIC_BUFFER_FLAG_USED);
//        }
//       PicBuf_SetBufferWord(dwAddr, (WORD)dwData);
//      }
//     ++dwAddr;
//     ++nBytesPerLine;
//   }
//
//  return true;
//
//} // end ApplyHexDumpLine( )


////---------------------------------------------------------------------------
//void MainFrame::MI_DumpMemAsRTFClick(TObject *Sender)
//{
//  REd_CodeMem->Lines->SaveToFile(_T("CodeMemDump.rtf"));
//  REd_DataMem->Lines->SaveToFile(_T("DataMemDump.rtf"));
//}
////---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void TCodeMemPanel::UpdateCodeMemDisplay(void)
{
    wxString old_msg;
// int  i;
// wxChar szTemp[1024];
// wxChar *cp;
// uint32_t dw;
// boolEAN fOldModifyFlag;

//  fOldModifyFlag = REd_CodeMem->Modified;
    old_msg = MainFrame::TheMainFrame->aStatusBar->GetStatusText();
    MainFrame::TheMainFrame->aStatusBar->SetStatusText(_("Updating CODE display..")) ;
    MainFrame::TheMainFrame->Update();
    MainFrame::TheMainFrame->m_update_code_mem_display = false;


//  // Prepare a temporary "memory stream" in which we will build
//  // the coloured, RichText-formatted, lines of the code memory window :
//  //   Note: Forget TRichEdit.Lines->Add() - it's UTTERLY slow !
//  //   Instead use a TMemoryStream to build an RTF-file-compatible structure
//  //   which can then be "loaded" into the TRichEdit in a single over.
//  //   This is significantly faster than appending lines "one-by-one" !
//  pRtfMemStream = new TMemoryStream();
//
//  // Update the hex dump in Borland's sluggish TRichEdit control :
//  // Dont update visible control during update (too slow for my 266MHz-P2).
//  // Note: Setting up the TEXT of the hex-"monitor" via "Lines->Add" seems
//  //       to be a little faster than the "Text"-property of a TRichEdit.
//  //       But still way too slow for large code memories.
//  //       *possibly* faster: Use the TRichEdit's "LoadFromStream" method.
//  //       Some interesting websites on this subject:
//  //         - "Automatic Syntax Highlighting Using a RichEdit Control"
//  //             http://www.undu.com/Articles/981027d.html
//  //         - "YourPasEdit" (better info on TRichEdit than in the Borland help system)
//  //             locally saved as c:\downloads\WindowsKnowHow\yrpasedit.zip
//  pRtfMemStream->Seek(0, soFromBeginning );        // warp to the begin of the stream
//
//  cp = szTemp;  // assemble next section in a good old C-string ...
//
    aCodeMemGrid->SetDefaultCellBackgroundColour (MainFrame::TheMainFrame->aOptionTab->aCodeMemColourButton->GetBackgroundColour());
    aCodeMemGrid->SetDefaultCellTextColour       (MainFrame::TheMainFrame->aOptionTab->aCodeMemColourButton->GetForegroundColour());
//  // Prepare an RTF header in the memory stream ...
//  RTF_GenerateHeader( cp/*destination*/,
//      (uint32_t)Pnl_CodeMemColors->Font->Color, // "text" colour
//      (uint32_t)Pnl_CodeMemColors->Color );     // "background" colour
//  RTF_SkipToEnd( &cp );
//  RTF_WriteStringToStream( pRtfMemStream, szTemp );  // et voila, RTF header is complete

//#if(1)
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
//      {
//          RTF_WriteStringToStream( pRtfMemStream, _T("User Code Memory") );
//        UpdateCodeMemRange( PIC_BUF_CODE, PIC_DeviceInfo.lCodeMemSize, UCM_SHOW_USED_LOCATIONS_ONLY );
//      }
        else
            aCodeMemGrid->UnloadData(0);
        int TotalExecLocation = std::min(PicBuf[PIC_BUF_EXEC].i32LastUsedArrayIndex+1, (long)PicBuf[PIC_BUF_EXEC].dwMaxSize);
        aCodeMemGrid->LoadHexData (aCodeMemAddrGetter2.SetMemory(PIC_BUF_EXEC, TotalExecLocation), TotalExecLocation, aCodeMemAddrGetter2, _T("Executive Code Memory"), 1);
//     RTF_WriteStringToStream( pRtfMemStream, _T("Executive Code Memory") );
//     UpdateCodeMemRange( PIC_BUF_EXEC, PicBuf[PIC_BUF_EXEC].i32LastUsedArrayIndex+1, UCM_SHOW_ALL_LOCATIONS );
    }
    else // no EXECUTIVE MEMORY, only 'ordinary' code memory : No need to show what it is !
    {
        aCodeMemGrid->LoadHexData (aCodeMemAddrGetter1.SetMemory(PIC_BUF_CODE, TotalLocation), TotalLocation, aCodeMemAddrGetter1);
        aCodeMemGrid->UnloadData(1);
        aCodeMemAddrGetter2.Invalidate();
//       UpdateCodeMemRange( PIC_BUF_CODE, PIC_DeviceInfo.lCodeMemSize, UCM_SHOW_ALL_LOCATIONS );
    }
//#else
//  for(i=1; i<=15; ++i)
//   {
//     _stprintf( szTemp, _T("\\cf%dThis should be line %d in colour #%d .\\par "), i, i, i );
//     RTF_WriteStringToStream( pRtfMemStream, szTemp );
//   }
//#endif // (0,1)
//
//  // Finish RTF file structure (in memory of course ! ) :
//  RTF_WriteStringToStream( pRtfMemStream, _T("\\par }") );
//
//#if(0)   // For debugging purposes: Save the RTF structure as file, too :
//  pRtfMemStream->SaveToFile(_T("code_mem.rtf"));
//#endif
//
//  // Copy the RTF character stream into the visible TRichEdit control ....
//  // For strange reasons, the TRichEdit control seems to be too stupid
//  //  to adjust it's own buffer automatically to the required size.
//  // If the transfer of the memory stream into the TRichEdit control fails,
//  //  and the RTF tags are displayed as plain text, the "MaxLength" parameter
//  //  may be too low, causing misinterpretation of the RTF structure  !
//  // ( yes, it's windoze, don't try to understand this)...  TRY THIS:
//  aStatusBar->SetStatusText(_("Streaming CODE display..")) ;
//  Update();
//  REd_CodeMem->Lines->BeginUpdate(); // should be faster with this, but no visible effect ?!
//  REd_CodeMem->PlainText = false;    // let the Rich Edit control accept RTF
//  REd_CodeMem->Color = Pnl_CodeMemColors->Color; // background colour (impossible via RTF code ?)
//  dw = pRtfMemStream->Size;          // get the length of the RTF in bytes
//  dw += dw/4;                        // add some safety margin
//  if(dw<65536) dw=65536;             // don't use less than 64 kByte for the text buffer
//  REd_CodeMem->MaxLength = dw;       // adjust the Rich Edit's text buffer size
//  pRtfMemStream->Seek(0, soFromBeginning );
//  REd_CodeMem->Lines->LoadFromStream(pRtfMemStream); // copy the stream into the Rich Edit control
//  REd_CodeMem->Lines->EndUpdate();
//  delete pRtfMemStream;
//  pRtfMemStream = NULL;    // forget this pointer, it's no longer valid !

//  // After restoring the TRichEdit's cursor position ,
//  //   the scroll position is at the end and the cursor('caret') is invisible.
//  // Borland won't tell you how to scroll the cursor in a TRichEdit in View.
//  // But the Win32 programmer's help says in the Rich Edit Control Reference:
//  //   > An application sends an EM_SCROLLCARET message
//  //   > to scroll the caret into view in an edit control.
//  SendMessage(  // hooray, this works...
//     REd_CodeMem->Handle, // handle of destination window
//     EM_SCROLLCARET,      // message to send
//     0,                   // wParam, first message parameter
//     0 );                 // lParam, second message parameter
//
//  REd_CodeMem->Modified = fOldModifyFlag;


    MainFrame::TheMainFrame->aStatusBar->SetStatusText(old_msg);
    MainFrame::TheMainFrame->Update();

} // end UpdateCodeMemDisplay()
//---------------------------------------------------------------------------


