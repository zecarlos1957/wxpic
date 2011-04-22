/*-------------------------------------------------------------------------*/
/* MainFrame.h                                                             */
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

#ifndef MAINFRAME_H
#define MAINFRAME_H

//(*Headers(MainFrame)
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/gauge.h>
#include <wx/statusbr.h>
//*)
#include <wx/toolbar.h>
#include <wx/config.h>
#include <wx/aboutdlg.h>
#include <WinPicPr/Devices.h>
#include "MemAddrGetter.h"
#include "CodeMemPanel.h"
#include "DataMemPanel.h"
#include "DeviceCfgPanel.h"
#include "ConfigMemoryPanel.h"
#include "OptionPanel.h"
#include "InterfacePanel.h"
#include "MessagePanel.h"

#define BUILT_IN_NAME (_T("Built-in"))


class MainFrame: public wxFrame
{
	public:

		MainFrame(void);
		virtual ~MainFrame();

		static bool CreateAndShow (void);
		static void SetStatusText (const wxString &Text);
		static void AddTextToLog  (const wxChar   *szText);
		static void ShowProgress  (int             pPercentage);

		 // "usages" of the message panel ( m_iMessagePanelUsage, Pnl_Message->Caption)
        enum EMpUsage
        {
            MP_USAGE_NOTHING,
            MP_USAGE_BUSY,
            MP_USAGE_INFO,
            MP_USAGE_WARNING,
            MP_USAGE_ERROR,
            MP_USAGE_COMMAND_TIMER,
        };


		//(*Declarations(MainFrame)
		wxMenuItem* aDiscardEditMenuItem;
		wxMenuItem* aEnabHexEditMenuItem;
		wxMenuItem* aReadMenuItem;
		wxMenuItem* aFaqMenuItem;
		wxMenuItem* aAboutMenuItem;
		wxMenuItem* aExitNoSaveMenuItem;
		wxMenu* aRecentFileSubMenu;
		wxMenuItem* aSelectDevMenuItem;
		wxNotebook* aNotebook;
		TDataMemPanel* aDataMemTab;
		wxMenuItem* aProgramIdMenuItem;
		wxMenu* aToolMenu;
		wxMenuItem* aClearBufferMenuItem;
		TCodeMemPanel* aCodeMemTab;
		wxMenuItem* aVerifyMenuItem;
		wxGauge* aProgressGauge;
		wxMenuItem* aHelpIndexMenuItem;
		wxMenuItem* aProgramCfgMenuItem;
		wxMenu* aDeviceMenu;
		wxTimer aTimer;
		TDeviceCfgPanel* aDeviceCfgTab;
		wxMenuItem* aDsPicProgMenuItem;
		TInterfacePanel* aInterfaceTab;
		wxMenuItem* aApplyEditMenuItem;
		wxMenu* aSpecialMenuItem;
		wxMenuItem* aClearFuseMenuItem;
		wxMenuItem* aLoadNProgMenuItem;
		wxMenu* aEditMenu;
		TConfigMemoryPanel* aConfigMemoryTab;
		wxMenuItem* aDevDumpMenuItem;
		TMessagePanel* aMessageTab;
		wxMenu* aFileMenu;
		wxStatusBar* aStatusBar;
		wxMenuItem* aLoadMenuItem;
		wxMenuItem* aReloadNProgMenuItem;
		wxMenuItem* aClearRecentMenuItem;
		wxMenuItem* aDumpBufferMenuItem;
		wxFileDialog* aFileDialog;
		wxMenuItem* aExitMenuItem;
		wxMenuItem* aProgramMenuItem;
		TOptionPanel* aOptionTab;
		wxMenuItem* aDsPicReadMenuItem;
		wxMenuItem* aEraseMenuItem;
		wxMenuItem* aDumpTransMenuItem;
		wxMenu* aHelpMenu;
		wxMenuItem* aResetMenuItem;
		wxMenuItem* aBlankCheckMenuItem;
		wxMenuItem* aBatchPrgMenuItem;
		wxMenuBar* MenuBar;
		//*)

        enum
        {
            TS_ProgramMemory,
            TS_DataMemory,
            TS_DeviceConfig,
            TS_CfgMemTab,
            TS_Options,
            TS_Interface,
            TS_Messages,
        };

		wxToolBar* aToolBar;
		wxToolBarToolBase* aVerifyProgramTool;
		wxToolBarToolBase* aSaveHexFileTool;
		wxToolBarToolBase* aReadProgramTool;
		wxToolBarToolBase* aOpenHexFileTool;
		wxToolBarToolBase* aProgDevTool;

        int                 aLogCount;
        bool                m_fMaySaveSettings;
        bool                m_update_code_mem_display;
        bool                m_update_data_mem_display;
        bool                m_update_id_and_config_display;
        int                 m_Updating;
        int                 m_iMessagePanelUsage;
//        bool                m_progress_visible;
        int                 m_progress_activity_timer;
        bool                m_fPicDeviceConflict;
        bool                m_fReloadAndProgClicked;
        wxString            m_original_title;

        static MainFrame   *TheMainFrame;

	protected:

		//(*Identifiers(MainFrame)
		static const long ID_CODE_MEM_PANEL;
		static const long ID_DATA_MEM_PANEL;
		static const long ID_PANEL4;
		static const long ID_PANEL5;
		static const long ID_PANEL1;
		static const long ID_PANEL7;
		static const long ID_PANEL8;
		static const long ID_NOTEBOOK;
		static const long ID_PROGRESS_GAUGE;
		static const long ID_LOAD_MENU_ITEM;
		static const long ID_LOAD_N_PRG_MENU_ITEM;
		static const long ID_RLOAD_N_PRG_MENU_ITEM;
		static const long ID_CLEAR_RECENT_MENU_ITEM;
		static const long ID_RECENT_FILES_SUB_MENU;
		static const long ID_DUMP_TO_HEX_MENU_ITEM;
		static const long ID_EXIT_NO_SAVE_MENU_ITEM;
		static const long ID_EXIT_MENU_ITEM;
		static const long ID_CLEAR_BUFFER_MENU_ITEM;
		static const long ID_CLEAR_FUSE_MENU_ITEM;
		static const long ID_ENAB_HEX_EDIT_MENU_ITEM;
		static const long ID_APPLY_EDIT_MENU_ITEM;
		static const long ID_DISCARD_EDIT_MENU_ITEM;
		static const long ID_SELECT_DEV_MENU_ITEM;
		static const long ID_PROGRAM_MENU_ITEM;
		static const long ID_PROGRAM_CFG_MENU_ITEM;
		static const long ID_PROGRAM_ID_MENU_ITEM;
		static const long ID_ERASE_MENU_ITEM;
		static const long ID_VERIFY_MENU_ITEM;
		static const long ID_BLANK_CHECK_MENU_ITEM;
		static const long ID_READ_MENU_ITEM;
		static const long ID_RESET_MENU_ITEM;
		static const long ID_BATCH_PRG_MENU_ITEM;
		static const long ID_DEV_DUMP_MENU_ITEM;
		static const long ID_DUMP_TRANS_MENU_ITEM;
		static const long ID_DSPIC_READ_MENU_ITEM;
		static const long ID_DSPIC_PROG_MENU_ITEM;
		static const long ID_SPECIAL_SUB_MENU;
		static const long ID_SHOW_PRG_OPT_MENU_ITEM;
		static const long ID_HELP_INDEX_MENU_ITEM;
		static const long ID_FAQ_MENU_ITEM;
		static const long ID_ABOUT_MENU_ITEM;
		static const long ID_STATUSBAR;
		static const long ID_TIMER;
		//*)
		static const long ID_OPEN_HEX_FILE_TOOL;
		static const long ID_SAVE_HEX_FILE_TOOL;
		static const long ID_RELOAD_PROG_TOOL;
		static const long ID_PROG_DEV_TOOL;
		static const long ID_VERIFY_PROGRAM_TOOL;
		static const long ID_READ_PROGRAM_TOOL;
		static const long ID_TOOLBAR;

	private:

		//(*Handlers(MainFrame)
		void OnRadioBox1Select(wxCommandEvent& event);
		void OnNotebook1PageChanged(wxNotebookEvent& event);
		void OnGrid1CellLeftClick(wxGridEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnButton11Click(wxCommandEvent& event);
		void onTimerTrigger(wxTimerEvent& event);
		void OnLoadMenuItemSelected(wxCommandEvent& event);
		void OnLoadNProgMenuItemSelected(wxCommandEvent& event);
		void OnReloadNProgMenuItemSelected(wxCommandEvent& event);
		void OnClearRecentMenuItemSelected(wxCommandEvent& event);
		void OnExitMenuItemSelected(wxCommandEvent& event);
		void OnExitNoSaveMenuItemSelected(wxCommandEvent& event);
		void onCfgMemGridCellChange(wxGridEvent& event);
		void onApplyIdLocsButtonClick(wxCommandEvent& event);
		void onDevCfgGridCellChange(wxGridEvent& event);
		void onProgOptionChanged(wxCommandEvent& event);
		void onTestXXXChkClick(wxCommandEvent& event);
		void onTestVddChkClick(wxCommandEvent& event);
		void onClockEnableChkClick(wxCommandEvent& event);
		void onDataEnableChkClick(wxCommandEvent& event);
		void onPullMclrToGndChkClick(wxCommandEvent& event);
		void onConnectICSPTargetChkClick(wxCommandEvent& event);
		void onVddSelectRadioSelect(wxCommandEvent& event);
		void onProgramMenuItemSelected(wxCommandEvent& event);
		void onProgramCfgMenuItemSelected(wxCommandEvent& event);
		void onProgramIdMenuItemSelected(wxCommandEvent& event);
		void onEraseMenuItemSelected(wxCommandEvent& event);
		void onReadMenuItemSelected(wxCommandEvent& event);
		void onBlankCheckMenuItemSelected(wxCommandEvent& event);
		void onClearMsgButtonClick(wxCommandEvent& event);
		void onClearBufferMenuItemSelected(wxCommandEvent& event);
		void onClearFuseMenuItemSelected(wxCommandEvent& event);
		void onVerifyMenuItemSelected(wxCommandEvent& event);
		void onAboutMenuItemSelected(wxCommandEvent& event);
		void onHelpIndexMenuItemSelected(wxCommandEvent& event);
		void onFaqMenuItemSelected(wxCommandEvent& event);
		void onDevDumpMenuItemSelected(wxCommandEvent& event);
		void onPartNameChoiceSelect(wxCommandEvent& event);
		void onInitInterfButtonClick(wxCommandEvent& event);
		void onInterfaceTypeChoiceSelect(wxCommandEvent& event);
		void onCustomInterfSelectButtonClick(wxCommandEvent& event);
		void onInterfacePortChoiceSelect(wxCommandEvent& event);
		void onIoPortAddressEditTextEnter(wxCommandEvent& event);
		void onSlowInterfaceChkClick(wxCommandEvent& event);
		void onShowPrgOptMenuItemSelected(wxCommandEvent& event);
		void onConfigWordHexEditText(wxCommandEvent& event);
		void onConfigWordHexEdit2Text(wxCommandEvent& event);
		void onProgMemSizeTextText(wxCommandEvent& event);
		void onEepromMemSizeTextText(wxCommandEvent& event);
		void onHasFlashMemoryChkClick(wxCommandEvent& event);
		void onSelectDevMenuItemSelected(wxCommandEvent& event);
		void onLptInterfHelpButtonClick(wxCommandEvent& event);
		void onCom84HelpButtonClick(wxCommandEvent& event);
		void onBatchPrgMenuItemSelected(wxCommandEvent& event);
		void onEnabHexEditMenuItemSelected(wxCommandEvent& event);
		void onDiscardEditMenuItemSelected(wxCommandEvent& event);
		void onApplyEditMenuItemSelected(wxCommandEvent& event);
		void onResetMenuItemSelected(wxCommandEvent& event);
		void onIdBinHexRadioSelect(wxCommandEvent& event);
		void onShowAllCfgCellsChkClick(wxCommandEvent& event);
		void onStartTestButtonClick(wxCommandEvent& event);
		void onCodeMemColourButtonClick(wxCommandEvent& event);
		void onDataMemColourButtonClick(wxCommandEvent& event);
		void onDsPicReadMenuItemSelected(wxCommandEvent& event);
		void onDsPicProgMenuItemSelected(wxCommandEvent& event);
		void onDumpBufferMenuItemSelected(wxCommandEvent& event);
		void onMPLabDevDirButtonClick(wxCommandEvent& event);
		void onDriverRadioSelect(wxCommandEvent& event);
		void onRedLedBitmapClick(wxCommandEvent& event);
		void onGreenLedBitmapClick(wxCommandEvent& event);
		void onLanguageChoiceSelect(wxCommandEvent& event);
		void OnButton6Click(wxCommandEvent& event);
		void onHelpOnIdButtonClick(wxCommandEvent& event);
		//*)
		void onChar(wxKeyEvent& pEvent);
        void onMenuOpen(wxMenuEvent &pEvent);
        void onMRFMenuItemSelected(wxCommandEvent& pEvent);

        wxArrayString     aMRFTable;
        wxAboutDialogInfo aAboutInfo;

		void     doAddTextLog (const wxChar *szText);

        wxString ProgramWhatInfoString(void);
        void StopParsingCmdLine_Internal(void);
        void DisconnectTarget(void);
        void LoadSmallPort(void);
        bool TestDelayRoutine(void);
        bool ProgramPic(void);
        bool VerifyPic(void);
        bool DumpEverythingToHexFile(const wxChar *fn);
        bool ReadPicAndDumpToFile(const wxChar *fn);
        bool LoadFileAndProgramPic(const wxChar *fn, bool program_too);

        bool RunHexOpenDialog (void);
        void initAuto         (void);
        void initMore         (void);
        void addLines         (wxString &pText, void (wxAboutDialogInfo::*pAdder)(const wxString&));
        void updateMRFMenu    (const wxArrayString &pMRFTable);

 	public:
       void UpdateCodeMemRange( int iWhichMemory, long i32NLocations, int iUCMoptions );
              #define UCM_SHOW_USED_LOCATIONS_ONLY 0x0000
              #define UCM_SHOW_ALL_LOCATIONS       0x0001
        void UpdateConfigBitCombo(void);
        bool EnableHexEditors(void);
        bool ApplyCodeMemoryEdits(void);
        bool ApplyDataMemoryEdits(void);
        bool QueryAndApplyHexEditIfRequired(void);

//        void UpdateToolWindow(void);
        void UpdateAllSheets(void);

        void ShowProgressGauge(bool visible);
        void LoadCommand (void);
        void LoadAndProgramCommand(void);
        void ReloadAndProgClick(void);

//        bool AppHelp(Word Command, int Data, bool &CallHelp);
        static void ChangeDefaultGridForegroundColour(wxGrid *Grid, const wxColour &NewForegroundColour);
        static void ChangeDefaultGridBackgroundColour(wxGrid *Grid, const wxColour &NewBackgroundColour);

		DECLARE_EVENT_TABLE()
};

#endif
