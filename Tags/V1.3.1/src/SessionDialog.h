/*-------------------------------------------------------------------------*/
/*  Filename: SessionDialog.h                                              */
/*                                                                         */
/*  Purpose:                                                               */
/*     Show and manage the Session dialog                                  */
/*                                                                         */
/*  Author:                                                                */
/*     Copyright 2011 Philippe Chevrier pch @ laposte.net                  */
/*                                                                         */
/*  License:                                                               */
/*     New WxPic Code is licensed under GPLV3 conditions                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#ifndef SESSIONDIALOG_H
#define SESSIONDIALOG_H

//(*Headers(TSessionDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include "SessionList.h"

class TSessionDialog: public wxDialog
{
	public:


		/**/TSessionDialog(TSessionManager &pSessionManager, wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~TSessionDialog();

		//(*Declarations(TSessionDialog)
		wxListBox* aSessionListBox;
		wxStaticText* StaticText2;
		wxButton* aCloseButton;
		wxButton* aStartButton;
		wxButton* aStartCloseButton;
		wxBoxSizer* aMainBoxSizer;
		wxButton* aNewButton;
		wxTextCtrl* aSessionNameEdit;
		wxButton* aDropChangeButton;
		wxButton* aRefreshButton;
		wxButton* aHelpButton;
		wxButton* aRenameButton;
		wxButton* aSaveButton;
		wxButton* aDeleteButton;
		//*)

		// static T_CONFIG *GetSession (T_CONFIG *pConfig);

	protected:

		//(*Identifiers(TSessionDialog)
		static const long ID_SESSION_LISTBOX;
		static const long ID_NEW_BUTTON;
		static const long ID_START_BUTTON;
		static const long ID_START_CLOSE_BUTTON;
		static const long ID_DELETE_BUTTON;
		static const long ID_REFRESH_BUTTON;
		static const long ID_HELP_BUTTON;
		static const long ID_CLOSE_BUTTON;
		static const long ID_STATICTEXT2;
		static const long ID_SESSION_NAME_EDIT;
		static const long ID_RENAME_BUTTON;
		static const long ID_SAVE_BUTTON;
		static const long ID_DROP_CHANGE_BUTTON;
		//*)

	private:
        TSessionManager               &aSessionManager;  //-- The creator's session manager
        TSessionManager::TSessionInfo *aSessionInfoTab;  //-- The last session information
        int                            aSessionCount;    //-- The number of sessions
        int                            aCurrentSession;  //-- The index of the current session (-1 if no current session)
        int                            aCurrentSelection;//-- The item currently selected in the list
        bool                           aHasFreeSession;  //-- Indicate if there is still a free session
        bool                           aIsSaved;         //-- Indicate if the current session config is saved
        bool                           aIsNameChanged;   //-- The Current session name has been edited but not applied


        //-- Define the content of the control aSessionListBox
        //-- if not pDontUpdateCurrent, fills also the current session name
        void fillSessionList (bool pDontUpdateCurrent = false);
        //-- Set the Enable state for the buttons that depends on the selection
        void setButtonStatus (void);
        //-- Save or Cancel the configuration changes of current session
        void saveConfig      (bool pCancel = false);
        //-- Start a new session and return the session number (-1 if creation failed)
        int  startSession    (void);
        //-- Rename the session and clear name change indicator (return false if rename failed)
        bool renameSession   (void);
        //-- Mark that the Session Name is unmodified
        void setNameSynch    (void);
        //-- Ask for saving unsave config. Return false if user canceled
        bool askSaveConfig   (bool pIsCreate);
        //-- Ask for saving unsave config. Return false if user canceled
        bool askRename       (void);
        //-- Combine the 2 question on Session Change
        bool askAll          (bool pIsCreate) { return askSaveConfig(pIsCreate) && askRename(); }
        //-- Test if the selected session can be started and optionally returns the current session number
        bool isStartable     (int *pSelectedSession = NULL) const;

		//(*Handlers(TSessionDialog)
		void onNewButtonClick(wxCommandEvent& event);
		void onStartButtonClick(wxCommandEvent& event);
		void onStartCloseButtonClick(wxCommandEvent& event);
		void onDeleteButtonClick(wxCommandEvent& event);
		void onRefreshButtonClick(wxCommandEvent& event);
		void onCloseButtonClick(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void onSessionNameEditText(wxCommandEvent& event);
		void onRenameButtonClick(wxCommandEvent& event);
		void onSaveButtonClick(wxCommandEvent& event);
		void onSessionListBoxSelect(wxCommandEvent& event);
		void onSessionListBoxDClick(wxCommandEvent& event);
		void onDropChangeButtonClick(wxCommandEvent& event);
		void onHelpButtonClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
