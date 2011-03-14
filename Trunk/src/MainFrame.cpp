/*-------------------------------------------------------------------------*/
/* MainFrame.cpp                                                           */
/*                                                                         */
/*  Purpose:                                                               */
/*       Manage the WxPic Main Window.                                     */
/*  Author:                                                                */
/*       Copyright 2009 Philippe Chevrier pch@laposte.net                  */
/*       from software originally written by Wolfgang Buescher (DL4YHF)    */
/*                                                                         */
/*  License:                                                               */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#include "MainFrame.h"
#include "Appl.h"
#include "CommandOption.h"
#include "Language.h"
#include <WinPicPr/Config.h>
#include <WinPicPr/PIC_HW.h>
#include <WinPicPr/PIC_HEX.h>
#include <WinPicPr/PIC_PRG.h>
#include <../resources/Resource.h>

//(*InternalHeaders(MainFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/utils.h>
#include <wx/settings.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>
#include "SVNVersion.h"

MainFrame *MainFrame::TheMainFrame = NULL;
wxConfig   MainFrame::TheIniFile(APPLICATION_NAME);
wxString   MainFrame::TheLanguageName;


enum {
    aboutLICENSE,
    aboutAUTHOR,
    aboutTRANSLAT,
    aboutDOC,
    aboutCOUNT
};
static const wxChar *AboutInfoFilenames[aboutCOUNT] =
{
	wxT("License.txt"),
	wxT("Author.txt"),
	wxT("Translator.txt"),
	wxT("DocWriter.txt"),
};

static void addMissingFileError (wxString &pErrorText, const wxChar *pMissingFile)
{
    pErrorText += _("Can't find ");
    pErrorText += pMissingFile;
    pErrorText += '\n';
}

//(*IdInit(MainFrame)
const long MainFrame::ID_CODE_MEM_PANEL = wxNewId();
const long MainFrame::ID_DATA_MEM_PANEL = wxNewId();
const long MainFrame::ID_PANEL4 = wxNewId();
const long MainFrame::ID_PANEL5 = wxNewId();
const long MainFrame::ID_PANEL1 = wxNewId();
const long MainFrame::ID_PANEL7 = wxNewId();
const long MainFrame::ID_PANEL8 = wxNewId();
const long MainFrame::ID_NOTEBOOK = wxNewId();
const long MainFrame::ID_PROGRESS_GAUGE = wxNewId();
const long MainFrame::ID_LOAD_MENU_ITEM = wxNewId();
const long MainFrame::ID_LOAD_N_PRG_MENU_ITEM = wxNewId();
const long MainFrame::ID_RLOAD_N_PRG_MENU_ITEM = wxNewId();
const long MainFrame::ID_RECENT_FILE1_MENU_ITEM = wxNewId();
const long MainFrame::ID_RECENT_FILE2_MENU_ITEM = wxNewId();
const long MainFrame::ID_RECENT_FILE3_MENU_ITEM = wxNewId();
const long MainFrame::ID_RECENT_FILE4_MENU_ITEM = wxNewId();
const long MainFrame::ID_RECENT_FILE5_MENU_ITEM = wxNewId();
const long MainFrame::ID_RECENT_FILE6_MENU_ITEM = wxNewId();
const long MainFrame::ID_CLEAR_RECENT_MENU_ITEM = wxNewId();
const long MainFrame::ID_RECENT_FILES_SUB_MENU = wxNewId();
const long MainFrame::ID_DUMP_TO_HEX_MENU_ITEM = wxNewId();
const long MainFrame::ID_EXIT_NO_SAVE_MENU_ITEM = wxNewId();
const long MainFrame::ID_EXIT_MENU_ITEM = wxNewId();
const long MainFrame::ID_CLEAR_BUFFER_MENU_ITEM = wxNewId();
const long MainFrame::ID_CLEAR_FUSE_MENU_ITEM = wxNewId();
const long MainFrame::ID_ENAB_HEX_EDIT_MENU_ITEM = wxNewId();
const long MainFrame::ID_APPLY_EDIT_MENU_ITEM = wxNewId();
const long MainFrame::ID_DISCARD_EDIT_MENU_ITEM = wxNewId();
const long MainFrame::ID_SELECT_DEV_MENU_ITEM = wxNewId();
const long MainFrame::ID_PROGRAM_MENU_ITEM = wxNewId();
const long MainFrame::ID_PROGRAM_CFG_MENU_ITEM = wxNewId();
const long MainFrame::ID_PROGRAM_ID_MENU_ITEM = wxNewId();
const long MainFrame::ID_ERASE_MENU_ITEM = wxNewId();
const long MainFrame::ID_VERIFY_MENU_ITEM = wxNewId();
const long MainFrame::ID_BLANK_CHECK_MENU_ITEM = wxNewId();
const long MainFrame::ID_READ_MENU_ITEM = wxNewId();
const long MainFrame::ID_RESET_MENU_ITEM = wxNewId();
const long MainFrame::ID_BATCH_PRG_MENU_ITEM = wxNewId();
const long MainFrame::ID_DEV_DUMP_MENU_ITEM = wxNewId();
const long MainFrame::ID_DUMP_TRANS_MENU_ITEM = wxNewId();
const long MainFrame::ID_DSPIC_READ_MENU_ITEM = wxNewId();
const long MainFrame::ID_DSPIC_PROG_MENU_ITEM = wxNewId();
const long MainFrame::ID_SPECIAL_SUB_MENU = wxNewId();
const long MainFrame::ID_SHOW_PRG_OPT_MENU_ITEM = wxNewId();
const long MainFrame::ID_HELP_INDEX_MENU_ITEM = wxNewId();
const long MainFrame::ID_FAQ_MENU_ITEM = wxNewId();
const long MainFrame::ID_ABOUT_MENU_ITEM = wxNewId();
const long MainFrame::ID_STATUSBAR = wxNewId();
const long MainFrame::ID_TIMER = wxNewId();
//*)
const long MainFrame::ID_TOOLBAR = wxNewId();
const long MainFrame::ID_SAVE_HEX_FILE_TOOL = wxNewId();
const long MainFrame::ID_OPEN_HEX_FILE_TOOL = wxNewId();
const long MainFrame::ID_RELOAD_PROG_TOOL = wxNewId();
const long MainFrame::ID_PROG_DEV_TOOL = wxNewId();
const long MainFrame::ID_READ_PROGRAM_TOOL = wxNewId();
const long MainFrame::ID_VERIFY_PROGRAM_TOOL = wxNewId();


BEGIN_EVENT_TABLE(MainFrame,wxFrame)
    //(*EventTable(MainFrame)
    //*)
    EVT_MENU_OPEN(MainFrame::onMenuOpen)
END_EVENT_TABLE()


MainFrame::MainFrame(void)
        : aLogCount(0)
        , m_fMaySaveSettings(true)
        , m_fReloadAndProgClicked(false)
{
    initAuto();
    initMore();
    SetIcon(*TResource::GetWxPicIcon());
}

void MainFrame::initAuto (void)
{
    //(*Initialize(MainFrame)
    wxBoxSizer* MainBoxSizer;
    wxMenuItem* ShowPrgOptMenuItem;

    Create(0, wxID_ANY, _("WxPic Programmer"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    MainBoxSizer = new wxBoxSizer(wxVERTICAL);
    aNotebook = new wxNotebook(this, ID_NOTEBOOK, wxPoint(216,312), wxDefaultSize, 0, _T("ID_NOTEBOOK"));
    aCodeMemTab = new TCodeMemPanel(aNotebook, ID_CODE_MEM_PANEL, wxDefaultPosition, wxDefaultSize, 0, _T("ID_CODE_MEM_PANEL"));
    aDataMemTab = new TDataMemPanel(aNotebook, ID_DATA_MEM_PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_DATA_MEM_PANEL"));
    aDeviceCfgTab = new TDeviceCfgPanel(aNotebook, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    aConfigMemoryTab = new TConfigMemoryPanel(aNotebook, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
    aOptionTab = new TOptionPanel(aNotebook, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    aInterfaceTab = new TInterfacePanel(aNotebook, ID_PANEL7, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL7"));
    aMessageTab = new TMessagePanel(aNotebook, ID_PANEL8, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL8"));
    aNotebook->AddPage(aCodeMemTab, _("Code"), false);
    aNotebook->AddPage(aDataMemTab, _("Data"), false);
    aNotebook->AddPage(aDeviceCfgTab, _("Device, Config"), false);
    aNotebook->AddPage(aConfigMemoryTab, _("Config Memory"), false);
    aNotebook->AddPage(aOptionTab, _("Options"), false);
    aNotebook->AddPage(aInterfaceTab, _("Interface"), false);
    aNotebook->AddPage(aMessageTab, _("Messages"), false);
    MainBoxSizer->Add(aNotebook, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 7);
    aProgressGauge = new wxGauge(this, ID_PROGRESS_GAUGE, 100, wxDefaultPosition, wxSize(-1,8), wxGA_SMOOTH, wxDefaultValidator, _T("ID_PROGRESS_GAUGE"));
    aProgressGauge->Hide();
    MainBoxSizer->Add(aProgressGauge, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SetSizer(MainBoxSizer);
    MenuBar = new wxMenuBar();
    aFileMenu = new wxMenu();
    aLoadMenuItem = new wxMenuItem(aFileMenu, ID_LOAD_MENU_ITEM, _("&Load HEX file...\tCTRL-L"), wxEmptyString, wxITEM_NORMAL);
    aFileMenu->Append(aLoadMenuItem);
    aLoadNProgMenuItem = new wxMenuItem(aFileMenu, ID_LOAD_N_PRG_MENU_ITEM, _("Load && &Program Device..."), wxEmptyString, wxITEM_NORMAL);
    aFileMenu->Append(aLoadNProgMenuItem);
    aReloadNProgMenuItem = new wxMenuItem(aFileMenu, ID_RLOAD_N_PRG_MENU_ITEM, _("&Reload && Program"), wxEmptyString, wxITEM_NORMAL);
    aFileMenu->Append(aReloadNProgMenuItem);
    aRecentFileSubMenu = new wxMenu();
    aRecentFile1MenuItem = new wxMenuItem(aRecentFileSubMenu, ID_RECENT_FILE1_MENU_ITEM, _("** None **"), wxEmptyString, wxITEM_NORMAL);
    aRecentFileSubMenu->Append(aRecentFile1MenuItem);
    aRecentFile1MenuItem->Enable(false);
    aRecentFile2MenuItem = new wxMenuItem(aRecentFileSubMenu, ID_RECENT_FILE2_MENU_ITEM, _("** None **"), wxEmptyString, wxITEM_NORMAL);
    aRecentFileSubMenu->Append(aRecentFile2MenuItem);
    aRecentFile2MenuItem->Enable(false);
    aRecentFile3MenuItem = new wxMenuItem(aRecentFileSubMenu, ID_RECENT_FILE3_MENU_ITEM, _("** None **"), wxEmptyString, wxITEM_NORMAL);
    aRecentFileSubMenu->Append(aRecentFile3MenuItem);
    aRecentFile3MenuItem->Enable(false);
    aRecentFile4MenuItem = new wxMenuItem(aRecentFileSubMenu, ID_RECENT_FILE4_MENU_ITEM, _("** None **"), wxEmptyString, wxITEM_NORMAL);
    aRecentFileSubMenu->Append(aRecentFile4MenuItem);
    aRecentFile4MenuItem->Enable(false);
    aRecentFile5MenuItem = new wxMenuItem(aRecentFileSubMenu, ID_RECENT_FILE5_MENU_ITEM, _("** None **"), wxEmptyString, wxITEM_NORMAL);
    aRecentFileSubMenu->Append(aRecentFile5MenuItem);
    aRecentFile5MenuItem->Enable(false);
    aRecentFile6MenuItem = new wxMenuItem(aRecentFileSubMenu, ID_RECENT_FILE6_MENU_ITEM, _("** None **"), wxEmptyString, wxITEM_NORMAL);
    aRecentFileSubMenu->Append(aRecentFile6MenuItem);
    aRecentFile6MenuItem->Enable(false);
    aRecentFileSubMenu->AppendSeparator();
    aClearRecentMenuItem = new wxMenuItem(aRecentFileSubMenu, ID_CLEAR_RECENT_MENU_ITEM, _("Clear recent file history"), wxEmptyString, wxITEM_NORMAL);
    aRecentFileSubMenu->Append(aClearRecentMenuItem);
    aFileMenu->Append(ID_RECENT_FILES_SUB_MENU, _("Recent &Files"), aRecentFileSubMenu, wxEmptyString);
    aFileMenu->AppendSeparator();
    aDumpBufferMenuItem = new wxMenuItem(aFileMenu, ID_DUMP_TO_HEX_MENU_ITEM, _("&Dump buffer to HEX file..."), wxEmptyString, wxITEM_NORMAL);
    aFileMenu->Append(aDumpBufferMenuItem);
    aFileMenu->AppendSeparator();
    aExitNoSaveMenuItem = new wxMenuItem(aFileMenu, ID_EXIT_NO_SAVE_MENU_ITEM, _("Exit without saving the settings"), wxEmptyString, wxITEM_NORMAL);
    aFileMenu->Append(aExitNoSaveMenuItem);
    aExitMenuItem = new wxMenuItem(aFileMenu, ID_EXIT_MENU_ITEM, _("E&xit"), wxEmptyString, wxITEM_NORMAL);
    aFileMenu->Append(aExitMenuItem);
    MenuBar->Append(aFileMenu, _("&File"));
    aEditMenu = new wxMenu();
    aClearBufferMenuItem = new wxMenuItem(aEditMenu, ID_CLEAR_BUFFER_MENU_ITEM, _("Clear &Buffer"), wxEmptyString, wxITEM_NORMAL);
    aEditMenu->Append(aClearBufferMenuItem);
    aClearFuseMenuItem = new wxMenuItem(aEditMenu, ID_CLEAR_FUSE_MENU_ITEM, _("Clear &Fuses"), wxEmptyString, wxITEM_NORMAL);
    aEditMenu->Append(aClearFuseMenuItem);
    aEditMenu->AppendSeparator();
    aEnabHexEditMenuItem = new wxMenuItem(aEditMenu, ID_ENAB_HEX_EDIT_MENU_ITEM, _("&Enable HEX editor"), wxEmptyString, wxITEM_CHECK);
    aEditMenu->Append(aEnabHexEditMenuItem);
    aApplyEditMenuItem = new wxMenuItem(aEditMenu, ID_APPLY_EDIT_MENU_ITEM, _("&Apply edits"), wxEmptyString, wxITEM_NORMAL);
    aEditMenu->Append(aApplyEditMenuItem);
    aDiscardEditMenuItem = new wxMenuItem(aEditMenu, ID_DISCARD_EDIT_MENU_ITEM, _("&Discard edits"), wxEmptyString, wxITEM_NORMAL);
    aEditMenu->Append(aDiscardEditMenuItem);
    MenuBar->Append(aEditMenu, _("&Edit"));
    aDeviceMenu = new wxMenu();
    aSelectDevMenuItem = new wxMenuItem(aDeviceMenu, ID_SELECT_DEV_MENU_ITEM, _("&Select"), wxEmptyString, wxITEM_NORMAL);
    aDeviceMenu->Append(aSelectDevMenuItem);
    aProgramMenuItem = new wxMenuItem(aDeviceMenu, ID_PROGRAM_MENU_ITEM, _("&Program\tCTRL-P"), wxEmptyString, wxITEM_NORMAL);
    aDeviceMenu->Append(aProgramMenuItem);
    aProgramCfgMenuItem = new wxMenuItem(aDeviceMenu, ID_PROGRAM_CFG_MENU_ITEM, _("Program &Config word only"), wxEmptyString, wxITEM_NORMAL);
    aDeviceMenu->Append(aProgramCfgMenuItem);
    aProgramIdMenuItem = new wxMenuItem(aDeviceMenu, ID_PROGRAM_ID_MENU_ITEM, _("Program &ID only"), wxEmptyString, wxITEM_NORMAL);
    aDeviceMenu->Append(aProgramIdMenuItem);
    aEraseMenuItem = new wxMenuItem(aDeviceMenu, ID_ERASE_MENU_ITEM, _("&Erase\tCTRL-E"), wxEmptyString, wxITEM_NORMAL);
    aDeviceMenu->Append(aEraseMenuItem);
    aVerifyMenuItem = new wxMenuItem(aDeviceMenu, ID_VERIFY_MENU_ITEM, _("&Verify\tCTRL-V"), wxEmptyString, wxITEM_NORMAL);
    aDeviceMenu->Append(aVerifyMenuItem);
    aBlankCheckMenuItem = new wxMenuItem(aDeviceMenu, ID_BLANK_CHECK_MENU_ITEM, _("&Blank Check\tCTRL-B"), wxEmptyString, wxITEM_NORMAL);
    aDeviceMenu->Append(aBlankCheckMenuItem);
    aReadMenuItem = new wxMenuItem(aDeviceMenu, ID_READ_MENU_ITEM, _("&Read\tCTRL-R"), wxEmptyString, wxITEM_NORMAL);
    aDeviceMenu->Append(aReadMenuItem);
    aResetMenuItem = new wxMenuItem(aDeviceMenu, ID_RESET_MENU_ITEM, _("Reset and &Go\tF9"), wxEmptyString, wxITEM_NORMAL);
    aDeviceMenu->Append(aResetMenuItem);
    MenuBar->Append(aDeviceMenu, _("&Device"));
    aToolMenu = new wxMenu();
    aBatchPrgMenuItem = new wxMenuItem(aToolMenu, ID_BATCH_PRG_MENU_ITEM, _("&Start Batch Programming"), wxEmptyString, wxITEM_CHECK);
    aToolMenu->Append(aBatchPrgMenuItem);
    aSpecialMenuItem = new wxMenu();
    aDevDumpMenuItem = new wxMenuItem(aSpecialMenuItem, ID_DEV_DUMP_MENU_ITEM, _("Dump &Built-in device list to file..."), wxEmptyString, wxITEM_NORMAL);
    aSpecialMenuItem->Append(aDevDumpMenuItem);
    aDumpTransMenuItem = new wxMenuItem(aSpecialMenuItem, ID_DUMP_TRANS_MENU_ITEM, _("Dump &Translation table to file..."), wxEmptyString, wxITEM_NORMAL);
    aSpecialMenuItem->Append(aDumpTransMenuItem);
    aSpecialMenuItem->AppendSeparator();
    aDsPicReadMenuItem = new wxMenuItem(aSpecialMenuItem, ID_DSPIC_READ_MENU_ITEM, _("&Read dsPIC executive code memory"), wxEmptyString, wxITEM_NORMAL);
    aSpecialMenuItem->Append(aDsPicReadMenuItem);
    aDsPicProgMenuItem = new wxMenuItem(aSpecialMenuItem, ID_DSPIC_PROG_MENU_ITEM, _("&Program dsPIC executive code memory"), wxEmptyString, wxITEM_NORMAL);
    aSpecialMenuItem->Append(aDsPicProgMenuItem);
    aToolMenu->Append(ID_SPECIAL_SUB_MENU, _("S&pecial"), aSpecialMenuItem, wxEmptyString);
    ShowPrgOptMenuItem = new wxMenuItem(aToolMenu, ID_SHOW_PRG_OPT_MENU_ITEM, _("Show Programmer &Options"), wxEmptyString, wxITEM_NORMAL);
    aToolMenu->Append(ShowPrgOptMenuItem);
    MenuBar->Append(aToolMenu, _("&Tools"));
    aHelpMenu = new wxMenu();
    aHelpIndexMenuItem = new wxMenuItem(aHelpMenu, ID_HELP_INDEX_MENU_ITEM, _("&Index\tCTRL-H"), wxEmptyString, wxITEM_NORMAL);
    aHelpMenu->Append(aHelpIndexMenuItem);
    aFaqMenuItem = new wxMenuItem(aHelpMenu, ID_FAQ_MENU_ITEM, _("&Frequently Ask Questions"), wxEmptyString, wxITEM_NORMAL);
    aHelpMenu->Append(aFaqMenuItem);
    aHelpMenu->AppendSeparator();
    aAboutMenuItem = new wxMenuItem(aHelpMenu, ID_ABOUT_MENU_ITEM, _("&About..."), wxEmptyString, wxITEM_NORMAL);
    aHelpMenu->Append(aAboutMenuItem);
    MenuBar->Append(aHelpMenu, _("&Help"));
    SetMenuBar(MenuBar);
    aStatusBar = new wxStatusBar(this, ID_STATUSBAR, 0, _T("ID_STATUSBAR"));
    int __wxStatusBarWidths_1[1] = { -10 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    aStatusBar->SetFieldsCount(1,__wxStatusBarWidths_1);
    aStatusBar->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(aStatusBar);
    aFileDialog = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    aTimer.SetOwner(this, ID_TIMER);
    aTimer.Start(50, false);
    MainBoxSizer->Fit(this);
    MainBoxSizer->SetSizeHints(this);

    Connect(ID_LOAD_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnLoadMenuItemSelected);
    Connect(ID_LOAD_N_PRG_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnLoadNProgMenuItemSelected);
    Connect(ID_RLOAD_N_PRG_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnReloadNProgMenuItemSelected);
    Connect(ID_RECENT_FILE1_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnRecentFile1MenuItemSelected);
    Connect(ID_RECENT_FILE2_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnRecentFile2MenuItemSelected);
    Connect(ID_RECENT_FILE3_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnRecentFile3MenuItemSelected);
    Connect(ID_RECENT_FILE4_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnRecentFile4MenuItemSelected);
    Connect(ID_RECENT_FILE5_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnRecentFile5MenuItemSelected);
    Connect(ID_RECENT_FILE6_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnRecentFile6MenuItemSelected);
    Connect(ID_CLEAR_RECENT_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnClearRecentMenuItemSelected);
    Connect(ID_DUMP_TO_HEX_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onDumpBufferMenuItemSelected);
    Connect(ID_EXIT_NO_SAVE_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnExitNoSaveMenuItemSelected);
    Connect(ID_EXIT_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::OnExitMenuItemSelected);
    Connect(ID_CLEAR_BUFFER_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onClearBufferMenuItemSelected);
    Connect(ID_CLEAR_FUSE_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onClearFuseMenuItemSelected);
    Connect(ID_ENAB_HEX_EDIT_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onEnabHexEditMenuItemSelected);
    Connect(ID_APPLY_EDIT_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onApplyEditMenuItemSelected);
    Connect(ID_DISCARD_EDIT_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onDiscardEditMenuItemSelected);
    Connect(ID_SELECT_DEV_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onSelectDevMenuItemSelected);
    Connect(ID_PROGRAM_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onProgramMenuItemSelected);
    Connect(ID_PROGRAM_CFG_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onProgramCfgMenuItemSelected);
    Connect(ID_PROGRAM_ID_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onProgramIdMenuItemSelected);
    Connect(ID_ERASE_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onEraseMenuItemSelected);
    Connect(ID_VERIFY_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onVerifyMenuItemSelected);
    Connect(ID_BLANK_CHECK_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onBlankCheckMenuItemSelected);
    Connect(ID_READ_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onReadMenuItemSelected);
    Connect(ID_RESET_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onResetMenuItemSelected);
    Connect(ID_BATCH_PRG_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onBatchPrgMenuItemSelected);
    Connect(ID_DEV_DUMP_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onDevDumpMenuItemSelected);
    Connect(ID_DSPIC_READ_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onDsPicReadMenuItemSelected);
    Connect(ID_DSPIC_PROG_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onDsPicProgMenuItemSelected);
    Connect(ID_SHOW_PRG_OPT_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onShowPrgOptMenuItemSelected);
    Connect(ID_HELP_INDEX_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onHelpIndexMenuItemSelected);
    Connect(ID_FAQ_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onFaqMenuItemSelected);
    Connect(ID_ABOUT_MENU_ITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&MainFrame::onAboutMenuItemSelected);
    Connect(ID_TIMER,wxEVT_TIMER,(wxObjectEventFunction)&MainFrame::onTimerTrigger);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&MainFrame::OnClose);
    //*)
}

void MainFrame::initMore (void)
{
    Connect(ID_OPEN_HEX_FILE_TOOL,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::OnLoadMenuItemSelected);
    Connect(ID_SAVE_HEX_FILE_TOOL,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::onDumpBufferMenuItemSelected);
    Connect(ID_RELOAD_PROG_TOOL,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::OnReloadNProgMenuItemSelected);
    Connect(ID_PROG_DEV_TOOL,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::onProgramMenuItemSelected);
    Connect(ID_VERIFY_PROGRAM_TOOL,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::onVerifyMenuItemSelected);
    Connect(ID_READ_PROGRAM_TOOL,wxEVT_COMMAND_TOOL_CLICKED,(wxObjectEventFunction)&MainFrame::onReadMenuItemSelected);
    wxTheApp->Connect(wxID_ANY,wxEVT_KEY_DOWN,(wxObjectEventFunction)&MainFrame::onChar, NULL, this);

    aToolBar = new wxToolBar(this, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER, _T("ID_TOOLBAR"));
    aOpenHexFileTool = aToolBar->AddTool(ID_OPEN_HEX_FILE_TOOL, _("Load HEX File"), *TResource::GetOpenIcon(), wxNullBitmap, wxITEM_NORMAL, _("Load HEX file"), _("Load an Hex file in the buffer and Program"));
    aSaveHexFileTool = aToolBar->AddTool(ID_SAVE_HEX_FILE_TOOL, _("Dump buffer to HEX file"), *TResource::GetSaveIcon(), wxNullBitmap, wxITEM_NORMAL, _("Dump buffer to HEX file"), _("Save the Buffer to an Hex file"));
    aToolBar->AddSeparator();
    aProgDevTool = aToolBar->AddTool(ID_RELOAD_PROG_TOOL, _("Reload and Program Device"), *TResource::GetChipReloadIcon(), wxNullBitmap, wxITEM_NORMAL, _("Reload and Program Device"), _("Reload the Buffer from the last used Hex file and Program the Device"));
    aProgDevTool = aToolBar->AddTool(ID_PROG_DEV_TOOL, _("Program Device"), *TResource::GetChipWriteIcon(), wxNullBitmap, wxITEM_NORMAL, _("Program Device"), _("Program Device with the Buffer Content"));
    aVerifyProgramTool = aToolBar->AddTool(ID_VERIFY_PROGRAM_TOOL, _("Verify Program"), *TResource::GetChipVerifyIcon(), wxNullBitmap, wxITEM_NORMAL, _("Verify Program"), _("Verify that the device program is conform to the loaded buffers"));
    aReadProgramTool = aToolBar->AddTool(ID_READ_PROGRAM_TOOL, _("Read Program"), *TResource::GetChipReadIcon(), wxNullBitmap, wxITEM_NORMAL, _("Read Program"), _("Store in the buffers the program read in the device"));
    aToolBar->Realize();
    SetToolBar(aToolBar);

    m_original_title = GetLabel();

    //-- Initialize and Read parameters
    aEmptyMRFname = aRecentFile1MenuItem->GetLabel();
    CFG_Init();
    CFG_Load(TheIniFile);
    LoadLayout();

    aCodeMemTab->aCodeMemGrid->SetHexFormat(16);
    aDataMemTab->aDataMemGrid->SetHexFormat(8);

    m_update_code_mem_display = true;
    m_update_data_mem_display = true;
//   m_progress_visible        = false;
    m_Updating                = 0;

//    PageControl1->ActivePage = TS_ProgramMemory;
    aNotebook->ChangeSelection(TS_ProgramMemory);

    wxString Errors;
    //-- That Devices.ini exists
    const wxChar *DeviceCharFile = PicDev_GetDeviceFileName();
    if (!wxFileName::FileExists(DeviceCharFile))
            addMissingFileError(Errors, DeviceCharFile);

    //-- About Info Initialisation
    wxString Info[aboutCOUNT];
    for (int i = 0; i < aboutCOUNT; ++i)
    {
        Info[i] = TLanguage::GetFileText(AboutInfoFilenames[i]);
        if (Info[i].IsEmpty())
            addMissingFileError(Errors, AboutInfoFilenames[i]);
    }
    if (!Errors.IsEmpty())
    {
        Errors += _("Please re-install the Program");
        wxMessageBox(Errors, _("Installation Error"));
        Close();
    }
    else
    {
        aAboutInfo.SetIcon(*TResource::GetWxPic32Icon());
        aAboutInfo.SetLicence(Info[aboutLICENSE]);
        addLines(Info[aboutAUTHOR],   &wxAboutDialogInfo::AddDeveloper);
        addLines(Info[aboutTRANSLAT], &wxAboutDialogInfo::AddTranslator);
        addLines(Info[aboutDOC],      &wxAboutDialogInfo::AddDocWriter);

        aAboutInfo.SetCopyright( _T( "Copyright (C) 2009-2010 Philippe Chevrier and Contributors" ) );
        aAboutInfo.SetDescription( _("WxPic is PIC Microcontroller Programmer") );
        aAboutInfo.SetName( _T("WxPic") );
        wxString Version;
#if SVN_MANAGED
        Version.Printf(_T("%s Rev. %d\n%s"), SVN_VERSION, SVN_REVISION, SVN_DATE);
#else
        Version.Printf(_("Unknown Version\nCompiled %s"), _T(__DATE__ " " __TIME__));
#endif
        aAboutInfo.SetVersion(Version);
        aAboutInfo.SetWebSite( _T("http://wxpic.sourceforge.net"));
    }
}


MainFrame::~MainFrame(){
	TheIniFile.Flush();
}


//---------------------------------------------------------------
//---------------------------------------------------------------


void MainFrame::onTimerTrigger(wxTimerEvent& event)
//void MainFrame::Timer1Timer(TObject *Sender)
{
    // OnTimer - method of the main form. Called every 50ms.
    static int siCmdTickCount = 0;  // unit: TIMER TICKS a 50ms
    static int siAlreadyHere = 0;
    static int siLptFooledCount = 0;
    static int siFirstCall=1;
    static int siPrescalerFor200ms = 0;

    if (TheMainFrame == NULL)
        return;

//  wxChar sz255Temp[256];
    wxChar sz255Msg[256];
    wxString s;
    T_PicDeviceInfo MyDeviceInfo;
    int i;

    ++siCmdTickCount;

    if (siAlreadyHere)
        return;

//  if( WinPic_fCloseImmediately )
//   { Close();
//     return;
//   }

    ++siAlreadyHere;

    if ( (!siFirstCall) && PicHw_fTogglingTxD )
    {
        PicHw_FeedChargePump();  // required to produce Vpp with a charge pump (JDM)
    }


    ++siPrescalerFor200ms;
    if (siPrescalerFor200ms>=3)
    {
        siPrescalerFor200ms=0;    // gets here every 200 milliseconds ...

        if (siFirstCall)
        {
            siFirstCall = 0;

            // Initialize the PIC-programming routines
            PicHw_sz255LastError[0] = 0;
            if (!PIC_HW_Init() ) // Initialize PIC programmer interface (HW = HardWare)
            {
                AddTextToLog( _("Error in PIC_HW_Init: ")
                              +wxString(PicHw_sz255LastError) );
            }
            else
                if (PicHw_sz255LastError[0])
                {
                    AddTextToLog( _("Warning in PIC_HW_Init: ")
                                  +wxString(PicHw_sz255LastError) );
                }

            // Check interface + delay routine ONCE
            if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
            {
                APPL_LogEvent( _("Init: Testing the interface") );
            }

            aInterfaceTab->TestTheInterface();  // try to find out if the interface is there
            TestDelayRoutine();  // check the programmer's DELAY routine

            // Fill the "Part"-combo list.
            aDeviceCfgTab->aPartNameChoice->Clear();
            for (i=0; PicDev_GetDeviceInfoByIndex( i, &MyDeviceInfo ) >= 0; ++i )
            {
                wxString DeviceName;
                aDeviceCfgTab->aPartNameChoice->Append(Iso8859_1_TChar(MyDeviceInfo.sz40DeviceName, DeviceName));
            }
            aDeviceCfgTab->aPartNameChoice->SetSelection(0);

            // Now update CODE + DATA memory display, and all the rest..
            UpdateAllSheets();
        } // end if <first call>

        // Parse the next arguments from the command line, if not finished yet.
        //     Most likely, this will be an instruction to LOAD a file
        //     which will be programmed into the device later, etc.
        // There will DELIBERATELY be a certain delay between executing all args.
        if ( CommandOption.WinPic_fCommandLineMode || CommandOption.WinPic_fCommandLineOption_Load )
        {
            int iSecToGo = CommandOption.WinPic_fCommandLineMode ? 20 : 5; // Unit: "timer ticks" a 50 ms
            iSecToGo = (long)(iSecToGo - siCmdTickCount) * 200L / 1000L;
            if ( !(CommandOption.WinPic_fCommandLineOption_NoDelay) && (iSecToGo>0) )
            {
                wxChar sz80Actions[81];
                sz80Actions[0]=0;
                if ( CommandOption.WinPic_fCommandLineOption_Read )
                    _tcscat(sz80Actions,_T("READ+"));
                if ( CommandOption.WinPic_fCommandLineOption_Erase )
                    _tcscat(sz80Actions,_T("ERASE+"));
                if ( CommandOption.WinPic_fCommandLineOption_Load )
                    _tcscat(sz80Actions,_T("LOAD+"));
                if ( CommandOption.WinPic_fCommandLineOption_Program )
                    _tcscat(sz80Actions,_T("PROGRAM+"));
                if ( CommandOption.WinPic_fCommandLineOption_Verify)
                    _tcscat(sz80Actions,_T("VERIFY+"));
                if ( CommandOption.WinPic_fCommandLineOption_Quit )
                    _tcscat(sz80Actions,_T("QUIT"));
                i = _tcslen(sz80Actions);
                if (i>1 && sz80Actions[i-1]=='+')
                    sz80Actions[i-1] = '\0';
                _stprintf(sz255Msg,
                          _("Command execution (%s) continues in %d seconds ... ESC to stop") ,
                          (char*)sz80Actions, (int)iSecToGo );
                aStatusBar->SetStatusText(sz255Msg);
                m_iMessagePanelUsage = MP_USAGE_COMMAND_TIMER;
            }
            else // Time to start the EXECUTION of all actions specified in the command line.
            {
                // Arrived here, the command line has been PARSED but not EXECUTED yet.
                // All actions must be performed in a quite senseful sequence ...
                if ( CommandOption.WinPic_fCommandLineOption_Read )
                {
                    // read the PIC device and dump the result into a HEX file.
                    // This is done BEFORE the device is (optionally) erased !
                    if (! ReadPicAndDumpToFile( Config.sz255HexFileName ) )
                    {
                        siCmdTickCount = 0;
                        CommandOption.WinPic_fCommandLineOption_NoDelay = false;  // slow down !
                    }
                    CommandOption.WinPic_fCommandLineOption_Read = false; // done.
                } // end if( WinPic_fCommandLineOption_Read )
                else
                    if ( CommandOption.WinPic_fCommandLineOption_Erase )
                    {
                        // (bulk-)Erase the PIC device :
                        aStatusBar->SetStatusText(_("Erasing ...")); // used in various places
                        Update();
                        if ( PIC_PRG_Erase( PIC_ERASE_ALL | PIC_SAVE_CALIBRATION) ) // here in command-line mode
                        {
                            if ( PIC_iHaveErasedCalibration
                                    && ((PIC_DeviceInfo.wCfgmask_bandgap != 0) || (PIC_DeviceInfo.lAddressOscCal >= 0)) )
                            {
                                APPL_ShowMsg( 0, _("Device has been erased. PROGRAM TO RESTORE CALIB BITS !!") ); // used more than once !
                                m_iMessagePanelUsage = MP_USAGE_WARNING;
                            }
                            else
                            {
                                APPL_ShowMsg( 0, _("Device has been erased.") );  // used more than once !
                                m_iMessagePanelUsage = MP_USAGE_INFO;
                            }
                        }
                        else
                            APPL_ShowMsg( 0, _("Erasing FAILED !") );

                        CommandOption.WinPic_fCommandLineOption_Erase = false; // done.
                    } // end if( WinPic_fCommandLineOption_Erase )
                    else
                        if ( CommandOption.WinPic_fCommandLineOption_Load )
                        {
                            // Load a HEX file into memory  but do NOT program it yet :
                            if (! LoadFileAndProgramPic( Config.sz255HexFileName, false/*load+prog*/ ) )
                            {
                                StopParsingCmdLine_Internal();
                            }
                            else
                            {
                                APPL_ShowMsg( 0, _("Loaded file \"%s\" through command line ."),
                                            Config.sz255HexFileName );
                            }
                            CommandOption.WinPic_fCommandLineOption_Load = false; // done.
                        } // end if( WinPic_fCommandLineOption_Load )
                        else
                            if ( CommandOption.WinPic_fCommandLineOption_Program )
                            {
                                // Load a HEX file into memory  *AND*  program the PIC device :
                                if (! LoadFileAndProgramPic( Config.sz255HexFileName, true/*load+prog*/ ) )
                                {
                                    StopParsingCmdLine_Internal();
                                }
                                CommandOption.WinPic_fCommandLineOption_Program = false; // done.
                            } // end if( WinPic_fCommandLineOption_Program )
                            else
                                if ( CommandOption.WinPic_fCommandLineOption_Verify )
                                {
                                    // extra VERIFY after programming
                                    if (! VerifyPic() )
                                    {
                                        StopParsingCmdLine_Internal(); // error -> stop parsing command line
                                    }
                                    CommandOption.WinPic_fCommandLineOption_Verify = false; // done.
                                } // end if( WinPic_fCommandLineOption_Verify )
                                else
                                    if ( CommandOption.WinPic_fCommandLineOption_Quit )
                                    {
                                        // Quit after execution of command line :
                                        if ( CommandOption.WinPic_i200msToQuit>0 ) // wait a little longer before "quitting" ?
                                        {
                                            --CommandOption.WinPic_i200msToQuit;
                                            _stprintf(sz255Msg, _("Quitting in %d seconds ... ESC to stop"),
                                                      CommandOption.WinPic_i200msToQuit / 5 );
                                            aStatusBar->SetStatusText(sz255Msg);
                                            m_iMessagePanelUsage = MP_USAGE_COMMAND_TIMER;
                                        }
                                        else
                                        {
//                if( WinPic_OkToCloseDueToErasedCalibration() ) // possibly ask the user!
//                {
//                  PIC_iHaveErasedCalibration = false;  // don't ask again if "ok to close"..
                                            Close();
//                }
                                            CommandOption.WinPic_fCommandLineOption_Quit = false; // done (??)
                                        }
                                    } // end if( WinPic_fCommandLineOption_Quit )
                                    else
                                    {
                                        // seems all command line options are through.
                                        // If not "quit", return to normal operation (not command-line driven)

                                    }
            } // end if <time to parse the next command line argument>
        } // end if <more command line arguments to parse>
        else  // all command line arguments are through (if any)
        {
            if ( m_iMessagePanelUsage == MP_USAGE_COMMAND_TIMER)
            {
                aStatusBar->SetStatusText(_("Command line finished"));
                m_iMessagePanelUsage = MP_USAGE_NOTHING;
            }
        }

        // Perform a few "tests" (launched from the "Debugging" panel)
        switch (CommandOption.WinPic_iTestMode)
        {
        case 2 :    // endlessly power on power off the PIC
            PIC_HW_ProgMode();         // first(?) Vdd on, then(?) Vpp on
            PIC_HW_ProgModeOff();      // programming voltage off, clock off, data high-z
            PIC_HW_LongDelay_ms(100);  // make sure the programming voltage is discharged before returning

        case 3 :    // endlessly send a "read conf" to the PIC
            for (int i = 0; i < 4; ++i)
            {
                PIC_HW_ProgMode();        // first(?) Vdd on, then(?) Vpp on
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_LoadConfig, false);  // internal address to 0x2000
//                PHWInfo.iCurrProgAddress = PIC_DeviceInfo.lConfMemBase; // added 2008-05 for the plugin-DLL
                PIC_HW_SerialOut_14Bit(0x2AAA);
//                PicHw_FlushCommand( PIC_DeviceInfo.iCmd_LoadConfig, 6, 0x2AAA, 14 );
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_BeginProg, true);  // clock LOW, data HIGH after this
                PIC_HW_Delay_us(PIC_DeviceInfo.lTi_Prog_us);  // program delay (usually 10ms)
                PIC_HW_SetClockAndData(false, false);
                PIC_HW_SerialOut_Command6(PIC_DeviceInfo.iCmd_ReadProg, true );
                PIC_HW_SerialRead_14Bit();
                PIC_HW_ProgModeOff();      // programming voltage off, clock off, data high-z
            }
            break;

        case WP_TEST_MODE_GUI_SPEED:      // extended debug display for the USER INTERFACE
            break;

        default:
            CommandOption.WinPic_iTestMode = 0;   // cancel all unknown "test modes"
            break;

        } // end switch(WinPic_iTestMode)

        // Turn the progress bar off if there is no more activity.
        // (ugly but easy, don't have to care for turning it off elsewhere !)
        if (aProgressGauge->IsShown())
        {
            if (m_progress_activity_timer>0)
                --m_progress_activity_timer;
            if (m_progress_activity_timer<=0)
            {
                ShowProgressGauge(false);
            }
        }

        aInterfaceTab->UpdateInterfaceInputSignalDisplay();

        if ( m_update_code_mem_display )
        {
            m_update_code_mem_display = false;
            aCodeMemTab->UpdateCodeMemDisplay();
        }

        if ( m_update_data_mem_display )
        {
            m_update_data_mem_display = false;
            aDataMemTab->UpdateDataMemDisplay();
        }

        if ( m_update_id_and_config_display && (aNotebook->GetSelection()==TS_CfgMemTab) )
        {
            m_update_id_and_config_display = false;
            aConfigMemoryTab->UpdateIdAndConfMemDisplay();
        }

        /* if code protection, watchdog, oscillator configuration has changed: */
        if (   ( aDeviceCfgTab->m_displayed_config_word[0]  != PicBuf_GetConfigWord(0) )
                ||( aDeviceCfgTab->m_displayed_config_word[1]  != PicBuf_GetConfigWord(1) )
                ||( strncmp(aDeviceCfgTab->m_sz40DisplayedDeviceName, PIC_DeviceInfo.sz40DeviceName,40)!=0 )
           )
        {
            strncpy(aDeviceCfgTab->m_sz40DisplayedDeviceName, PIC_DeviceInfo.sz40DeviceName, 40);
            aDeviceCfgTab->m_displayed_config_word[0] = PicBuf_GetConfigWord(0);
            aDeviceCfgTab->m_displayed_config_word[1] = PicBuf_GetConfigWord(1);
            aDeviceCfgTab->UpdateDeviceConfigTab( true/*update HEX display also*/ );   // << here in TIMER METHOD !
            aInterfaceTab->UpdateInterfaceTestDisplay();
        }

//        // If used, let the DLL-based hardware-interface-plugin update its own GUI:
//        PicHw_LetInterfaceDLLDoGraphicStuff();

        // State machine for BATCH PROGRAMMING...
        switch ( PIC_PRG_iBatchProgState )
        {
        case BATCH_PROG_OFF:  // nothing to do FOR THE PROGRAMMING BATCH, but ...
            // Maybe a button in the 'tool window' has been clicked :
            if ( m_fReloadAndProgClicked )
            {
                m_fReloadAndProgClicked = false;
                LoadFileAndProgramPic( Config.sz255HexFileName, true );
                aInterfaceTab->UpdateInterfaceTestDisplay();
            }
            break; // end case < no BATCH PROGRAMMING mode >

        case BATCH_PROG_PREP_START:
            APPL_ShowMsg( 0, _("BATCH PROG: Insert device or connect ICSP, then press Enter or OK !") );
            PIC_PRG_iBatchProgState = BATCH_PROG_WAIT_START;
            break;

        case BATCH_PROG_WAIT_START:
            // Waiting for user to release "OK"-button on programmer
            //   or press "ENTER" on PC keyboard.
            // Transition to "BATCH_PROG_STARTED" on ENTER key is somewhere else.
            i = PicHw_GetOkButtonState();
            if (i==0)
                PIC_PRG_iBatchProgState = BATCH_PROG_WAIT_START2;
            break;

        case BATCH_PROG_WAIT_START2:
            // Waiting for user to press "OK"-button on programmer
            //   or press "ENTER" on PC keyboard.
            // Transition to "BATCH_PROG_STARTED" on ENTER key is somewhere else.
            i = PicHw_GetOkButtonState();
            if (i==1)
                PIC_PRG_iBatchProgState = BATCH_PROG_STARTED;
            break;

        case BATCH_PROG_STARTED:
            APPL_ShowMsg( 0, _("BATCH PROG: please wait..") );
            ProgramPic();  // try to program all data which have been loaded before.
            aInterfaceTab->UpdateInterfaceTestDisplay();
            PIC_PRG_iBatchProgState = BATCH_PROG_PREP_START;
            break;

        case BATCH_PROG_TERMINATE:
            PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
            APPL_ShowMsg( 0, _("BATCH PROG: terminated batch mode.") );
            break;

        default:
            PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
            break;
        } // end switch( PIC_PRG_iBatchProgState )




    } // end if < every 200 milliseconds >

    // Because Windows fools around with the parallel port all the time,
    // try to update the centronics data register.
    //   ( This is VERY VERY CRUDE, but better than nothing ... see FAQ list ! )
    // Note that this "OnTimer" event handler will not be called
    // while the programming algorithm works, because this is NO SEPARATE THREAD !
    if (PicHw_fLptPortOpened)
    {
        if (! PicHw_CheckLptDataBits() ) // debugging stuff ..
        {
            if (siLptFooledCount<10)
            {
                APPL_ShowMsg( 0, _("WARNING ! Windows fooled around with the LPT port bits !") );
                ++siLptFooledCount;
            }
        }
        PicHw_UpdateLptDataBits();  // copy the 'latch' value to the port again
    }



    --siAlreadyHere;

}
//---------------------------------------------------------------------------



void MainFrame::OnLoadMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Load1Click(TObject *Sender)
{
    if (RunHexOpenDialog())
        LoadFileAndProgramPic( Config.sz255HexFileName, false );
}
//---------------------------------------------------------------------------


void MainFrame::OnLoadNProgMenuItemSelected(wxCommandEvent& event)
{
    LoadAndProgramCommand();
}
//---------------------------------------------------------------------------
void MainFrame::LoadAndProgramCommand(void)
{
    if (RunHexOpenDialog())
        LoadFileAndProgramPic( Config.sz255HexFileName, true );
    aInterfaceTab->UpdateInterfaceTestDisplay();
}
//---------------------------------------------------------------------------

void MainFrame::OnReloadNProgMenuItemSelected(wxCommandEvent& event)
{
    ReloadAndProgClick();
}

void MainFrame::ReloadAndProgClick(void)
{
    m_fReloadAndProgClicked = true;
}
//---------------------------------------------------------------------------

//
void MainFrame::OnRecentFile1MenuItemSelected(wxCommandEvent& event)
{
    LoadMRF(0);
}

void MainFrame::OnRecentFile2MenuItemSelected(wxCommandEvent& event)
{
    LoadMRF(1);
}

void MainFrame::OnRecentFile3MenuItemSelected(wxCommandEvent& event)
{
    LoadMRF(2);
}

void MainFrame::OnRecentFile4MenuItemSelected(wxCommandEvent& event)
{
    LoadMRF(3);
}

void MainFrame::OnRecentFile5MenuItemSelected(wxCommandEvent& event)
{
    LoadMRF(4);
}

void MainFrame::OnRecentFile6MenuItemSelected(wxCommandEvent& event)
{
    LoadMRF(5);
}

//---------------------------------------------------------------------------
void MainFrame::LoadMRF(int iMRFindex)
{
    wxString s = GetMRFname(iMRFindex);
    if (s != _T(""))
    {
        _tcscpy( Config.sz255HexFileName, s.c_str() );
        ConfigChanged = true;
        if ( LoadFileAndProgramPic( s.c_str(), false/*load only, don't program*/ ) )
        {
            // only if there was no "load"-error, show the normal tool-window again
//       UpdateToolWindow();
        }
    }
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void MainFrame::OnClearRecentMenuItemSelected(wxCommandEvent& event)
//void MainFrame::MI_ClearMRFsClick(TObject *Sender)
{
    for (int i=0; i<=5; ++i)
        SetMRFname( i, aEmptyMRFname );
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void MainFrame::OnExitMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Exit1Click(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------


void MainFrame::OnExitNoSaveMenuItemSelected(wxCommandEvent& event)
//void MainFrame::MI_ExitDontSaveClick(TObject *Sender)
{
    m_fMaySaveSettings = false;
    Close();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void MainFrame::onProgramMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Menu_ProgramDeviceClick(TObject *Sender)
{
    PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
    if (QueryAndApplyHexEditIfRequired())
    {
        ProgramPic();  // try to program all data which have been loaded before.
        aInterfaceTab->UpdateInterfaceTestDisplay();
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void MainFrame::onProgramCfgMenuItemSelected(wxCommandEvent& event)
//void MainFrame::MI_ProgCfgWordClick(TObject *Sender)
{
    uint32_t dwTemp4[4];

    if (PIC_DeviceInfo.wCfgmask_bandgap != 0)
    {
        //-- If bandgap bits not saved do it now
        if (PIC_lBandgapCalibrationBits<0)
            SaveBandgapCalBit();
        uint32_t &ConfigWordBuffer = PicBuf[PIC_BUF_CONFIG].pdwData[PIC_DeviceInfo.lConfWordAdr - PIC_DeviceInfo.lConfMemBase];
        long   CurBandgapValue  = ConfigWordBuffer & PIC_DeviceInfo.wCfgmask_bandgap;
        if ((PIC_lBandgapCalibrationBits >= 0)
        &&  (PIC_lBandgapCalibrationBits != CurBandgapValue))
        {
            //-- If the bandgap bits in config word does not match those saved
            //-- ask for updating the config word with the bandgap calib bits
            int Choice = wxMessageBox(wxString::Format(_("The Bandgap calibration bits in config register (0x%04X) do not match the bits currently programmed in the device (0x%04X)."
                                    "Do you want to keep the current device bit value (Yes) else the calibration will be overwritten by the value defined in the buffer (No)" ),
                            CurBandgapValue, PIC_lBandgapCalibrationBits),
                        _("Overwrite Bandgap calibration "),
                        wxICON_EXCLAMATION | wxYES_NO | wxCANCEL);
            if (Choice == wxCANCEL)
            {
                APPL_ShowMsg( 0, _("Programming CONFIG-WORD cancelled.") );
                return;
            }
            else if (Choice == wxYES)
            {
                ConfigWordBuffer &= ~PIC_DeviceInfo.wCfgmask_bandgap;
                ConfigWordBuffer |= PIC_lBandgapCalibrationBits;
            }
            else
            {
                APPL_ShowMsg( 0, _("Old Bandgap 0x%04X will be overwritten with 0x%04X"),
                              PIC_lBandgapCalibrationBits, CurBandgapValue );
                PIC_lBandgapCalibrationBits = CurBandgapValue;
            }
        }
    }
    APPL_ShowMsg( 0, _("Programming CONFIG-WORD") );
    dwTemp4[0] = PicBuf_GetConfigWord(0);
    dwTemp4[1] = PicBuf_GetConfigWord(1);
    if (PIC_PRG_Program( dwTemp4, // program config word(s) :
                         1 + // number of words to be programmed ...
                         ((PIC_DeviceInfo.wCfgmask2_used==0x0000)?0:1),  // 2nd cfg word ?
                         PIC_DeviceInfo.lCodeMemWriteLatchSize,
                         PicPrg_GetConfigWordMask(),   // mask to be verified
                         PIC_DeviceInfo.iCmd_LoadProg,
                         PIC_DeviceInfo.iCmd_ReadProg,
                         PIC_DeviceInfo.lConfWordAdr))
    {
        APPL_ShowMsg( 0, _("Programming CONFIG-WORD done.") );
    }
    else
    {
        APPL_ShowMsg( 0, _("Programming CONFIG-WORD FAILED.") );
    }

    if (Config.iDisconnectAfterProg)
        DisconnectTarget();  // since 2002-09-26 . Suggested by Johan Bodin.

    aInterfaceTab->UpdateInterfaceTestDisplay();
}
//---------------------------------------------------------------------------

void MainFrame::onProgramIdMenuItemSelected(wxCommandEvent& event)
//void MainFrame::MI_ProgIDlocsClick(TObject *Sender)
{
    APPL_ShowMsg( 0, _("Programming ID-locations") );
    if (PIC_PRG_Program(PicBuf[PIC_BUF_CONFIG].pdwData,
                        4, // number of words to be programmed
                        4, // Row size for config memory
                        0x3FFF, // mask for programming and verification
                        PIC_DeviceInfo.iCmd_LoadProg, // 'load' command pattern
                        PIC_DeviceInfo.iCmd_ReadProg, // 'read' command pattern (for verifying)
                        PIC_DeviceInfo.lConfMemBase)) // target start address
    {
        APPL_ShowMsg( 0, _("Programming ID-LOCATIONS done.") );
    }
    else
    {
        APPL_ShowMsg( 0, _("Programming ID-LOCATIONS FAILED.") );
    }

    if (Config.iDisconnectAfterProg)
        DisconnectTarget();  // since 2002-09-26 . Suggested by Johan Bodin.

    aInterfaceTab->UpdateInterfaceTestDisplay();
}
//---------------------------------------------------------------------------


void MainFrame::onEraseMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Erase1Click(TObject *Sender)
{
    const wxChar *pszMsg;

    PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
    if (PIC_DeviceInfo.iCodeMemType==PIC_MT_FLASH )
    {
        if (! Config.iUseCompleteChipErase )
        {
            if (wxMessageBox(
                        _("The BULK ERASE option is disabled\n   in the programmer options.\n Erase EVERYTHING anyway ?"),
                        _("Confirm ERASE ALL"),
                        wxICON_QUESTION | wxYES_NO | wxCANCEL ) != wxYES )
                return;
        } // end if(! Config.iUseCompleteChipErase )
        pszMsg = _("Erasing ...");
        aStatusBar->SetStatusText(pszMsg);
//    if(ToolForm) ToolForm->ShowMsg(pszMsg,TWMSG_NO_ERROR);
        Update();
        if ( PIC_PRG_Erase( PIC_ERASE_ALL | PIC_SAVE_CALIBRATION) ) // here in 'manual erase' mode
        {
            pszMsg = _("Device has been erased."); // translation used more than once !
//        if(ToolForm) ToolForm->ShowMsg( pszMsg, TWMSG_SUCCESS );
            if ( PIC_iHaveErasedCalibration
                    && ((PIC_DeviceInfo.wCfgmask_bandgap != 0) || (PIC_DeviceInfo.lAddressOscCal >= 0)) )
            {
                APPL_ShowMsg( 0, _("Device has been erased. PROGRAM TO RESTORE CALIB BITS !!") );
                m_iMessagePanelUsage = MP_USAGE_WARNING;
            }
            else
            {
                APPL_ShowMsg( 0, pszMsg );
                m_iMessagePanelUsage = MP_USAGE_INFO;
            }
        }
        else
        {
            pszMsg = _( "Erasing FAILED !" );
            APPL_ShowMsg( 0, pszMsg );
//        if(ToolForm) ToolForm->ShowMsg( pszMsg, TWMSG_ERROR );
        }
    }
    else
    {
        aStatusBar->SetStatusText(_( "Must not erase non-FLASH device, check \"Device, Config\" !")) ;
        m_iMessagePanelUsage = MP_USAGE_WARNING;
    }
    // Some rare devices have some CALIBRATION BITS which are read before erase.
    // Show the state of these bits on the "Device/Config" tab:
    aDeviceCfgTab->UpdateDeviceConfigTab( true/*fUpdateHexWord*/ );
    aInterfaceTab->UpdateInterfaceTestDisplay();
} // end Erase1Click()
//---------------------------------------------------------------------------

void MainFrame::onReadMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Read1Click(TObject *Sender)
{
    bool ok;
    bool not_blank;
    PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
    m_iMessagePanelUsage = MP_USAGE_BUSY;
    m_fPicDeviceConflict = false;
    Update();

    APPL_iUserBreakFlag = 0;
    ok = PIC_PRG_ReadAll(false, &not_blank);  // READ, not only blank check

    if (Config.iDisconnectAfterProg)
        DisconnectTarget();  // since 2002-09-26 . Suggested by Johan Bodin.

    UpdateAllSheets();  // incl  UpdateInterfaceTestDisplay()
    aConfigMemoryTab->aApplyIdLocsButton->Disable();
//  Btn_ApplyIdLocs->Enabled = false;

    if ( !ok )
    {
        // reading not ok, for any strange reason it was "aborted" ...
        APPL_ShowMsg( 0, _( "Action aborted") );
        m_iMessagePanelUsage = MP_USAGE_ERROR;
        return;
    }

    if ( !not_blank ) // not not blank means "blank", aka "completely empty"..
    {
        APPL_ShowMsg( 0, _( "Reading PIC FAILED (maybe blank)." ) );
        m_iMessagePanelUsage = MP_USAGE_WARNING;
    }
    else  // reading ok, AND the PIC is *not* blank ...
    {
        if ( m_fPicDeviceConflict )
        {
            APPL_ShowMsg( 0, _( "Reading done, Conflict: selected device = %hs,  read = %hs" ),
                         PIC_DeviceInfo.sz40DeviceName, aConfigMemoryTab->m_sz80DetectedPicDevName );
        }
        else
        {
            APPL_ShowMsg( 0, _( "Reading PIC ok.") );
        }
        m_iMessagePanelUsage = MP_USAGE_INFO;
    }
}
//---------------------------------------------------------------------------

void MainFrame::onBlankCheckMenuItemSelected(wxCommandEvent& event)
//void MainFrame::BlankCheck1Click(TObject *Sender)
{
    bool read_ok, not_blank;
    const wxChar *pszMsg;
    PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
    pszMsg = _( "Blank Checking ..." );
    aStatusBar->SetStatusText(pszMsg);
    m_iMessagePanelUsage = MP_USAGE_BUSY;
    Update();
    APPL_iUserBreakFlag = 0;
    read_ok = PIC_PRG_ReadAll(true, &not_blank); //  blank check only,  do NOT READ to buffer
    aInterfaceTab->UpdateInterfaceTestDisplay();
    if (read_ok)
    {
        if (not_blank)
        {
            pszMsg = _( "Device is NOT blank.");
        }
        else
        {
            pszMsg = _( "Device is blank.");
        }
    }
    else  // reading not ok, for any strange reason it was "aborted" ...
    {
        pszMsg = _( "Action aborted");
    }
    APPL_ShowMsg( 0, pszMsg );
    m_iMessagePanelUsage = MP_USAGE_INFO;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void MainFrame::onClearBufferMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Clearbuffer1Click(TObject *Sender)
{
    /* Initialize buffers to imitate the erased state. */
    PIC_HEX_ClearBuffers();
//  REd_CodeMem->Modified = false;
//  REd_DataMem->Modified = false;
    UpdateAllSheets(); // make the changes visible on the screen
    aConfigMemoryTab->aApplyIdLocsButton->Disable();
//  Btn_ApplyIdLocs->Enabled = false;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void MainFrame::onClearFuseMenuItemSelected(wxCommandEvent& event)
//void MainFrame::ClearFusesClick(TObject *Sender)
{
    /* Set the "default" configuration word : */
    if ( CommandOption.WinPic_i32CmdLineOption_OverrideConfigWord >= 0)
        PicBuf_SetConfigWord( 0, (uint16_t)CommandOption.WinPic_i32CmdLineOption_OverrideConfigWord );
    else
        PicBuf_SetConfigWord( 0,
                              PIC_DeviceInfo.wCfgmask_unused  // all unused bits HIGH="ERASED"
                              |  PIC_DeviceInfo.wCfgmask_unknown // all UNKNOWN bits also HIGH
                              |  PIC_DeviceInfo.wCfgmask_cpbits  //  Code memory protection OFF
                              | (PIC_DeviceInfo.wCfgmask_pwrte^PIC_DeviceInfo.wCfgmask_inv_pwrte)
                              |  PIC_DeviceInfo.wCfgmask_osc_rc  // select RC oscillator configuration
                              |  PIC_DeviceInfo.wCfgmask_cpd     //  Data EEPROM Protection off
                              |  PIC_DeviceInfo.wCfgmask_lvp     //  Low Voltage Prog. Enabled
                              |  PIC_DeviceInfo.wCfgmask_boden); //  Brown-out Detect & Reset Enabled
    UpdateAllSheets(); // make the changes visible on the screen
    aConfigMemoryTab->aApplyIdLocsButton->Disable();
//  Btn_ApplyIdLocs->Enabled = false;
}
//---------------------------------------------------------------------------


void MainFrame::onVerifyMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Verify1Click(TObject *Sender)
{
    PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
    QueryAndApplyHexEditIfRequired();
    if ( VerifyPic() )
    {
        if (Config.iDisconnectAfterProg)
            DisconnectTarget();  // since 2002-09-26 . Suggested by Johan Bodin.
    }
    else  // VerifyPic failed :
    {
        // If 'verify' failed, don't disconnect from target. Won't run anyway !
    }

    aInterfaceTab->UpdateInterfaceTestDisplay();
}
//---------------------------------------------------------------------------


void MainFrame::onAboutMenuItemSelected(wxCommandEvent& event)
//void MainFrame::About1Click(TObject *Sender)
{
	wxAboutBox( aAboutInfo );
}

void MainFrame::onHelpIndexMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Index1Click(TObject *Sender)
{
    wxHelpControllerBase *Help = TLanguage::GetHelpController();
    if (Help != NULL)
        Help->DisplaySection(TResource::HELPID_MAIN_INDEX);
//  Application->HelpContext( HELPID_MAIN_INDEX );
}
//---------------------------------------------------------------------------

void MainFrame::onFaqMenuItemSelected(wxCommandEvent& event)
//void MainFrame::MI_FAQsClick(TObject *Sender)
{
    wxHelpControllerBase *Help = TLanguage::GetHelpController();
    if (Help != NULL)
        Help->DisplaySection(TResource::HELPID_FAQ_LISTS);
//  Application->HelpContext( HELPID_FAQ_LISTS );
}
//---------------------------------------------------------------------------


void MainFrame::onDevDumpMenuItemSelected(wxCommandEvent& event)
//void MainFrame::M_DumpDevListClick(TObject *Sender)
{
    PicDev_DumpDeviceListToFile(_T("DevListDump.txt"));
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void MainFrame::onShowPrgOptMenuItemSelected(wxCommandEvent& event)
//void MainFrame::MI_ProgOpsClick(TObject *Sender)
{
    aNotebook->SetSelection(TS_Options);
//  PageControl1->ActivePage = TS_Options;
}
//---------------------------------------------------------------------------

//void MainFrame::Ed_MessageClick(TObject *Sender)
//{
//    aNotebook->SetSelection(TS_Messages);
////  PageControl1->ActivePage = TS_Messages;
//}
////---------------------------------------------------------------------------




void MainFrame::onSelectDevMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Menu_SelectDeviceClick(TObject *Sender)
{
    PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
    aNotebook->SetSelection(TS_DeviceConfig);
//   PageControl1->ActivePage = TS_DeviceConfig;
    aDeviceCfgTab->aPartNameChoice->SetFocus();
//   Combo_PartName->DroppedDown = true;  // TComboBox
}
//---------------------------------------------------------------------------


//void MainFrame::Pnl_MessageClick(TObject *Sender)
//{
//  PageControl1->ActivePage = TS_Messages;
//  m_iMessagePanelUsage = MP_USAGE_NOTHING;  // may overwrite this msg now
//}
////---------------------------------------------------------------------------

//void MainFrame::FormCloseQuery(TObject *Sender, bool/*not bool!*/&CanClose)
//{
//  CanClose = WinPic_OkToCloseDueToErasedCalibration();
//}
////---------------------------------------------------------------------------


//---------------------------------------------------------------------------
wxString MainFrame::ProgramWhatInfoString(void)
{
    wxString sResult;
    int iNeedSeparator=0;
    if (Config.iProgramWhat & PIC_PROGRAM_CODE)
    {
        sResult = _("CODE");
        iNeedSeparator=1;
    }
    if (Config.iProgramWhat & PIC_PROGRAM_DATA)
    {
        if ( iNeedSeparator ) sResult=sResult + _T("+");
        sResult = sResult + _("DATA");
        iNeedSeparator=1;
    }
    if (Config.iProgramWhat & PIC_PROGRAM_CONFIG)
    {
        if ( iNeedSeparator ) sResult=sResult + _T("+");
        sResult = sResult + _("CONFIG");
        iNeedSeparator=1;
    }
    return sResult;
} // end MainFrame::ProgramWhatInfoString()


//---------------------------------------------------------------------------
void MainFrame::onMenuOpen(wxMenuEvent &pEvent)
//void MainFrame::File1Click(TObject *Sender)
{
    int i,j;
    PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
    // Rename some menu items according to programmer options
    aLoadNProgMenuItem->SetText(
        /*Menu_LoadAndProgram->Caption =*/ wxString(_("Load && &Program Device"))
        + _T(" (") + ProgramWhatInfoString() + _T(")..."));
    wxFileName HexFilename(Config.sz255HexFileName);
    aReloadNProgMenuItem->SetText(
        /*MI_ReloadAndProg->Caption =*/ wxString(_("&Reload && Program"))
        + _T(" \"") + HexFilename.GetFullName()/*ExtractFileName(Config.sz255HexFileName)*/ + _T("\""));
    for (i=j=0; i<=5; ++i)
    {
        if ( GetMRFname(i) != wxEmptyString ) ++j;
    }
    aFileMenu->FindItem(ID_RECENT_FILES_SUB_MENU)->Enable(j>0);
//  MI_MRFs->Enabled = (j>0);
//}
////---------------------------------------------------------------------------
//
//void MainFrame::Menu_DeviceClick(TObject *Sender)
//{
    PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
    // Rename some menu items according to programmer options
    aProgramMenuItem->SetText(
//  Menu_ProgramDevice->Caption =
        wxString(_( "Program")) + _T(" (" )
        + ProgramWhatInfoString() + _T(")"));
//}
////---------------------------------------------------------------------------
//
//
////---------------------------------------------------------------------------
//void MainFrame::Menu_ToolsClick(TObject *Sender)
//{
    aBatchPrgMenuItem->Check(PIC_PRG_iBatchProgState != BATCH_PROG_OFF);
//  Menu_StartBatchProgramming->Checked = (PIC_PRG_iBatchProgState != BATCH_PROG_OFF);

    aDsPicReadMenuItem->Enable(PIC_DeviceInfo.iBitsPerInstruction==24);
    aDsPicProgMenuItem->Enable(PIC_DeviceInfo.iBitsPerInstruction==24);
//  MI_dsPIC_ReadExecutiveCodeMem->Enabled  = (PIC_DeviceInfo.iBitsPerInstruction==24);
//  MI_dsPIC_WriteExecutiveCodeMem->Enabled = (PIC_DeviceInfo.iBitsPerInstruction==24);

//  MI_ShowToolWin->Checked = (ToolForm!=NULL && ToolForm->Visible);
//  MI_ShowToolbar->Checked = Pnl_Tools->Visible;
//}
////---------------------------------------------------------------------------
//
////---------------------------------------------------------------------------
//void MainFrame::Edit1Click(TObject *Sender)
//{
    PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
    aEnabHexEditMenuItem->Check(aCodeMemTab->aCodeMemGrid->IsEditable());
//  Menu_EnableEdit->Checked = !REd_CodeMem->ReadOnly;
}
//---------------------------------------------------------------------------



void MainFrame::onBatchPrgMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Menu_StartBatchProgrammingClick(TObject *Sender)
{
    aNotebook->SetSelection(TS_Messages);
//  PageControl1->ActivePage = TS_Messages;
    if (PIC_PRG_iBatchProgState == BATCH_PROG_OFF)
    {
        aMessageTab->aMessageLog->Clear();
//       Mem_Messages->Clear();
        aStatusBar->SetStatusText(wxEmptyString);
        PIC_PRG_iBatchProgState = BATCH_PROG_PREP_START;
    }
    else
    {
        PIC_PRG_iBatchProgState = BATCH_PROG_TERMINATE;
    }
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool MainFrame::ApplyCodeMemoryEdits(void)
{
    aCodeMemTab->aCodeMemAddrGetter1.ApplyChange();
    aCodeMemTab->aCodeMemAddrGetter2.ApplyChange();
    return true;
}


//---------------------------------------------------------------------------
bool MainFrame::ApplyDataMemoryEdits(void)
{
    aDataMemTab->aDataMemAddrGetter.ApplyChange();
    return true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool MainFrame::EnableHexEditors(void)
{
    aCodeMemTab->aCodeMemGrid->EnableEditing(true);
    aDataMemTab->aDataMemGrid->EnableEditing(true);
//     REd_CodeMem->ReadOnly = false;
//     REd_CodeMem->Enabled  = true;
//     REd_DataMem->ReadOnly = false;
//     REd_DataMem->Enabled  = true;

    return true;
} // end MainFrame::EnableHexEditors()


//---------------------------------------------------------------------------
bool MainFrame::QueryAndApplyHexEditIfRequired(void)
{
    if (aCodeMemTab->aCodeMemGrid->IsEditable()
            &&  (aCodeMemTab->aCodeMemAddrGetter1.IsModified()
                 || aCodeMemTab->aCodeMemAddrGetter2.IsModified()
                 || aDataMemTab->aDataMemAddrGetter.IsModified()))
//  if(  !REd_CodeMem->ReadOnly
//    && (REd_CodeMem->Modified || REd_DataMem->Modified )  )
    {
        int Answer = wxMessageBox(
                         _( "Apply changes in the HEX EDITOR ?" ) ,
                         _T("WxPic"),
                         wxICON_QUESTION | wxYES_NO | wxCANCEL );
        if (Answer == wxYES )
        {
            ApplyCodeMemoryEdits();
            ApplyDataMemoryEdits();
        }
        else if (Answer == wxCANCEL )
        {
            return false;
        }
    }
    return true;
} // end MainFrame::QueryAndApplyHexEditIfRequired()


//---------------------------------------------------------------------------
void MainFrame::onEnabHexEditMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Menu_EnableEditClick(TObject *Sender)
{
    // see help on TRichEdit !
    if (aCodeMemTab->aCodeMemGrid->IsEditable() && (aCodeMemTab->aCodeMemAddrGetter1.IsModified() || aCodeMemTab->aCodeMemAddrGetter2.IsModified()))
//  if( !REd_CodeMem->ReadOnly  &&  REd_CodeMem->Modified )
    {
        int Answer = wxMessageBox(
                         _( "Do you want to APPLY the edits\n in the code memory buffer ?" ),
                         _( "WxPic: turning CODE MEMORY editor off.." ),
                         wxICON_QUESTION | wxYES_NO | wxCANCEL );
        if ( Answer == wxYES )
        {
            ApplyCodeMemoryEdits();
        }
        else if (Answer == wxCANCEL)
            return;
    }

    if (aDataMemTab->aDataMemGrid->IsEditable() && aDataMemTab->aDataMemAddrGetter.IsModified())
//  if( !REd_DataMem->ReadOnly  &&  REd_DataMem->Modified )
    {
        int Answer = wxMessageBox(
                         _( "Do you want to APPLY the edits\n in the data memory buffer ?" ),
                         _( "WxPic: turning DATA MEMORY editor off.." ),
                         wxICON_QUESTION | wxYES_NO | wxCANCEL );
        if (Answer == wxYES )
        {
            ApplyDataMemoryEdits();
        }
        else if (Answer == wxCANCEL )
            return;
    }

    // Toggle the "edit mode" for both hex dumps (which are in fact TRichEdits)
    if (!aCodeMemTab->aCodeMemGrid->IsEditable())
//  if(REd_CodeMem->ReadOnly)
    {
        EnableHexEditors();
    }
    else
    {
        aCodeMemTab->aCodeMemGrid->EnableEditing(false);
//       REd_CodeMem->ReadOnly = true;
        // REd_CodeMem->Enabled  = false;    // shit, also disables the SCROLLER(!)
        aDataMemTab->aDataMemGrid->EnableEditing(false);
//     REd_DataMem->ReadOnly = true;
        // REd_DataMem->Enabled  = false;   // shit, also disables the SCROLLER(!)
    }
}
//---------------------------------------------------------------------------


void MainFrame::onDiscardEditMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Menu_DiscardHexEditsClick(TObject *Sender)
{
    m_update_code_mem_display = m_update_data_mem_display = true;
}
//---------------------------------------------------------------------------


void MainFrame::onApplyEditMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Menu_ApplyHexEditsClick(TObject *Sender)
{
    ApplyCodeMemoryEdits();
    ApplyDataMemoryEdits();
}
//---------------------------------------------------------------------------


void MainFrame::onResetMenuItemSelected(wxCommandEvent& event)
//void MainFrame::ResetGo1Click(TObject *Sender)
{
    PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
    PIC_HW_ResetAndGo();
    aInterfaceTab->UpdateInterfaceTestDisplay();
}
//---------------------------------------------------------------------------

void MainFrame::onChar(wxKeyEvent& event)
//void MainFrame::FormKeyDown(TObject *Sender, uint16_t &Key,
//      TShiftState Shift)
{
    int Key = event.GetKeyCode();
    bool Handled = false;
   // See help on OnKeyDown : "KeyPreview" must be set !
   if((Key==13) || (Key == WXK_NUMPAD_ENTER))
    {
      if(  (PIC_PRG_iBatchProgState == BATCH_PROG_WAIT_START)
        || (PIC_PRG_iBatchProgState == BATCH_PROG_WAIT_START2) )
       { PIC_PRG_iBatchProgState =  BATCH_PROG_STARTED;
           Handled = true;
//         Key=0; // handled
       }
    }
   else if(Key==27)
    {
      APPL_iUserBreakFlag = 1;  // signal for the programming routines to leave all loops
      if( CommandOption.WinPic_fCommandLineMode )
       { CommandOption.WinPic_fCommandLineMode = false;
         aStatusBar->SetStatusText(_( "ESC: Command-line driven operation cancelled !"));
         m_iMessagePanelUsage = MP_USAGE_INFO;
           Handled = true;
//         Key=0; // handled
       }
      if(PIC_PRG_iBatchProgState != BATCH_PROG_OFF)
       { PIC_PRG_iBatchProgState =  BATCH_PROG_TERMINATE;
         Key=0; // handled
           Handled = true;
       }
    }
    if (!Handled)
        event.Skip();
}
//---------------------------------------------------------------------------


void MainFrame::onDumpBufferMenuItemSelected(wxCommandEvent& event)
//void MainFrame::Menu_SaveBuffersClick(TObject *Sender)
{
    aFileDialog->SetWindowStyle(wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    aFileDialog->SetMessage(_("Save Hex File"));
    aFileDialog->SetWildcard(_("HEX files (INHX8M, *.hex)|*.hex"));
    aFileDialog->SetFilename(_T("dumped.hex"));
    aFileDialog->SetPath(wxEmptyString);
    /* beware: Win XP saves a history in the registry (grrrrrr) */
//  SaveDialog->Title      = TE( "Save Hex File" );
//  SaveDialog->DefaultExt = "hex";
//  SaveDialog->Filter     = "HEX files (INHX8M, *.hex)|*.hex";
//  SaveDialog->FileName   = "dumped.hex";
//  SaveDialog->InitialDir = "";
//  SaveDialog->HistoryList->Clear();
    if (aFileDialog->ShowModal() == wxID_OK)
//  if( SaveDialog->Execute() )
    {
        DumpEverythingToHexFile( aFileDialog->GetFilename().c_str() );
    }
}
//---------------------------------------------------------------------------

////---------------------------------------------------------------------------
//void MainFrame::HexEditKeyDown(TObject *Sender, uint16_t &Key,
//      TShiftState Shift)
//{
//  if( (!Shift.Contains(ssAlt)) && (!Shift.Contains(ssAlt)) )   // whats a Set ? grumble..
//   {
//    if( (Key>='0' && Key<='9') || (Key>='a' && Key<='f') || (Key>='a' && Key<='f')
//     || (Key==' ') )
//    {
//     if(REd_CodeMem->ReadOnly)
//      {
//        if(MessageBox(  Handle, // handle of owner window
//         TE( "Turn HEX-EDITOR on ?" ),
//         "WinPic",
//         MB_ICONQUESTION | MB_YESNOCANCEL | MB_DEFBUTTON1 ) == IDYES )
//          {
//            EnableHexEditors();
//          }
//       }
//     }
//   }
//}
////---------------------------------------------------------------------------

////---------------------------------------------------------------------------
//void MainFrame::MI_ShowToolWinClick(TObject *Sender)
//{
//  if(ToolForm)
//   { ToolForm->Show();
//     UpdateToolWindow();
//   }
//}
////---------------------------------------------------------------------------


//void MainFrame::MI_SelLanguageClick(TObject *Sender)
//{
//  PageControl1->ActivePage = TS_Options;
//  CB_Language->DroppedDown = true;
//}
//---------------------------------------------------------------------------


void MainFrame::onDsPicReadMenuItemSelected(wxCommandEvent& event)
//void MainFrame::MI_dsPIC_ReadExecutiveCodeMemClick(
//      TObject *Sender)
{
    bool ok;

    if ( PIC_DeviceInfo.iBitsPerInstruction==24 )
    {
        PIC_PRG_iBatchProgState = BATCH_PROG_OFF;
        aStatusBar->SetStatusText(_( "Reading Executive Code Memory ..." ));
        m_iMessagePanelUsage = MP_USAGE_BUSY;
        m_fPicDeviceConflict = false;
        Update();

        ok = PicPrg_ReadExecutiveCodeMemory();  // for dsPIC30F only ! !
        UpdateAllSheets();  // incl  UpdateInterfaceTestDisplay()

        if (ok)
        {
            APPL_ShowMsg( 0, _( "Reading Executive Code Memory ok.") );
            m_iMessagePanelUsage = MP_USAGE_INFO;
        }
        else
        {
            APPL_ShowMsg( 0, _( "Reading Executive Code Memory failed." ) );
            m_iMessagePanelUsage = MP_USAGE_WARNING;
        }
        aCodeMemTab->UpdateCodeMemDisplay();
    } // end if < 24 bit per instruction, most likely dsPIC30Fxxxx >

}
//---------------------------------------------------------------------------


void MainFrame::onDsPicProgMenuItemSelected(wxCommandEvent& event)
//void MainFrame::MI_dsPIC_WriteExecutiveCodeMemClick(
//      TObject *Sender)
{
    if ( PIC_DeviceInfo.iBitsPerInstruction==24 )
    {
    } // end if < 24 bit per instruction, most likely dsPIC30Fxxxx >
}


//---------------------------------------------------------------------------
//  "Speed Buttons" - unnecessary gimmicks for some folks,
//                    may be turned off through the good old main menu .
//---------------------------------------------------------------------------

//void MainFrame::Btn_CloseToolbarClick(TObject *Sender)
//{
//   Pnl_Tools->Visible = false;
//}
////---------------------------------------------------------------------------
//void MainFrame::MI_ShowToolbarClick(TObject *Sender)
//{
//   Pnl_Tools->Visible = !Pnl_Tools->Visible;
//}
////---------------------------------------------------------------------------
//void MainFrame::Btn_FileOpenClick(TObject *Sender)
//{
//   Load1Click(Sender);
//}
////---------------------------------------------------------------------------
//void MainFrame::Btn_ProgramClick(TObject *Sender)
//{
//   Menu_ProgramDeviceClick(Sender);
//}
////---------------------------------------------------------------------------
//void MainFrame::Btn_SaveClick(TObject *Sender)
//{
//   Menu_SaveBuffersClick( Sender );
//}
////---------------------------------------------------------------------------
//void MainFrame::Btn_VerifyClick(TObject *Sender)
//{
//   Verify1Click( Sender );
//}
////---------------------------------------------------------------------------
//void MainFrame::Btn_ReadPICClick(TObject *Sender)
//{
//   Read1Click( Sender );
//}
////---------------------------------------------------------------------------
//void MainFrame::Btn_HelpIndexClick(TObject *Sender)
//{
//   Index1Click( Sender );
//}
//---------------------------------------------------------------------------


//---------------------------------------------------------------
//---------------------------------------------------------------

void MainFrame::OnClose(wxCloseEvent& event)
{
//---------------------------------------------------------------------------
//void MainFrame::FormClose(TObject *Sender, TCloseAction &Action)
//{ // The OnClose-method of the main form should save the config & clean up

//---------------------------------------------------------------------------
    if (    (PIC_iHaveErasedCalibration)
            && ((PIC_lBandgapCalibrationBits >= 0) || (PIC_lOscillatorCalibrationWord >=0) )
            && ((PIC_DeviceInfo.wCfgmask_bandgap!=0) || (PIC_DeviceInfo.lAddressOscCal >= 0) )
            &&  event.CanVeto()
       )
    {
        wxBell();
//        for(int i=0;i<10;++i)
//            MessageBeep(0xFFFFFFFF);
        if (wxMessageBox( _("Some CALIBRATION BITS have been erased\nbut not written back to the PIC yet.\n"
                            "Terminate anyway and loose these bits forever ?"),
                          _("*** PIC Programmer Warning ***"),
                          wxICON_EXCLAMATION | wxYES_NO | wxCANCEL ) != wxYES )
        {
            event.Veto();
            return;
        }
    }

    if ( m_fMaySaveSettings && ConfigChanged )
    {
        SaveLayout();
        strncpy(Config.sz40DeviceName, PIC_DeviceInfo.sz40DeviceName, 40) ;
        CFG_Save(TheIniFile);
        TheIniFile.SetPath(_T("/LANGUAGE"));
        TheIniFile.Write(_T("Name"), MainFrame::TheLanguageName);
    } // end if( m_fMaySaveSettings )

//   if(ToolForm)
//      ToolForm->Close(); // not called automagically ?!
//   PIC_HW_Close();
    Destroy();
}
//---------------------------------------------------------------
//---------------------------------------------------------------

