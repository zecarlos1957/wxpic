/*-------------------------------------------------------------------------*/
/*  Filename: SessionDialog.cpp                                            */
/*                                                                         */
/*  Purpose:                                                               */
/*     Implementation of SessionDialog.h                                   */
/*                                                                         */
/*  Author:                                                                */
/*     Copyright 2011 Philippe Chevrier pch @ laposte.net                  */
/*                                                                         */
/*  License:                                                               */
/*     New WxPic Code is licensed under GPLV3 conditions                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#include "SessionDialog.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(TSessionDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)


//-- Some frequently used string providing functions
static const wxString &getAvailableString (void) { static wxString Result = _("Available"); return Result; }
static const wxString &getInUseString     (void) { static wxString Result = _("In Use");    return Result; }
static const wxString &getCurrentString   (void) { static wxString Result = _("Current");   return Result; }



//(*IdInit(TSessionDialog)
const long TSessionDialog::ID_SESSION_LISTBOX = wxNewId();
const long TSessionDialog::ID_NEW_BUTTON = wxNewId();
const long TSessionDialog::ID_START_BUTTON = wxNewId();
const long TSessionDialog::ID_START_CLOSE_BUTTON = wxNewId();
const long TSessionDialog::ID_DELETE_BUTTON = wxNewId();
const long TSessionDialog::ID_REFRESH_BUTTON = wxNewId();
const long TSessionDialog::ID_BUTTON1 = wxNewId();
const long TSessionDialog::ID_CLOSE_BUTTON = wxNewId();
const long TSessionDialog::ID_STATICTEXT2 = wxNewId();
const long TSessionDialog::ID_SESSION_NAME_EDIT = wxNewId();
const long TSessionDialog::ID_RENAME_BUTTON = wxNewId();
const long TSessionDialog::ID_SAVE_BUTTON = wxNewId();
const long TSessionDialog::ID_DROP_CHANGE_BUTTON = wxNewId();
//*)

BEGIN_EVENT_TABLE(TSessionDialog,wxDialog)
	//(*EventTable(TSessionDialog)
	//*)
END_EVENT_TABLE()

TSessionDialog::TSessionDialog(TSessionManager &pSessionManager, wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
: aSessionManager (pSessionManager)
, aSessionInfoTab (NULL)
, aCurrentSession (-1)
{
	//(*Initialize(TSessionDialog)
	wxBoxSizer* BoxSizer4;
	wxStaticBoxSizer* StaticBoxSizer2;
	wxBoxSizer* BoxSizer6;
	wxBoxSizer* BoxSizer5;
	wxBoxSizer* BoxSizer2;
	wxStaticBoxSizer* StaticBoxSizer3;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxBoxSizer* BoxSizer3;

	Create(parent, wxID_ANY, _("Manage Sessions"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	aMainBoxSizer = new wxBoxSizer(wxVERTICAL);
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Session List"));
	aSessionListBox = new wxListBox(this, ID_SESSION_LISTBOX, wxDefaultPosition, wxDefaultSize, 0, 0, wxLB_SINGLE|wxLB_NEEDED_SB, wxDefaultValidator, _T("ID_SESSION_LISTBOX"));
	aSessionListBox->SetToolTip(_("Select the session to start"));
	StaticBoxSizer3->Add(aSessionListBox, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 2);
	BoxSizer3->Add(StaticBoxSizer3, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
	BoxSizer5->Add(BoxSizer3, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	aNewButton = new wxButton(this, ID_NEW_BUTTON, _("New Session"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_NEW_BUTTON"));
	aNewButton->SetToolTip(_("Create a new session with current configuration and set it current"));
	BoxSizer2->Add(aNewButton, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 2);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, _("Selected Session"));
	aStartButton = new wxButton(this, ID_START_BUTTON, _("Switch to"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_START_BUTTON"));
	aStartButton->SetToolTip(_("Switch to the selected session"));
	StaticBoxSizer2->Add(aStartButton, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 2);
	aStartCloseButton = new wxButton(this, ID_START_CLOSE_BUTTON, _("Switch and Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_START_CLOSE_BUTTON"));
	aStartCloseButton->SetToolTip(_("Run WxPic in the selected session"));
	StaticBoxSizer2->Add(aStartCloseButton, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 2);
	StaticBoxSizer2->Add(0,20,0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
	aDeleteButton = new wxButton(this, ID_DELETE_BUTTON, _("Delete..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_DELETE_BUTTON"));
	aDeleteButton->SetToolTip(_("Delete the session and all its configuration parameters"));
	StaticBoxSizer2->Add(aDeleteButton, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 2);
	BoxSizer2->Add(StaticBoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
	BoxSizer2->Add(0,20,0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
	aRefreshButton = new wxButton(this, ID_REFRESH_BUTTON, _("Refresh List"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_REFRESH_BUTTON"));
	aRefreshButton->SetToolTip(_("Refresh the session list in case it has been modified by an other instance"));
	BoxSizer2->Add(aRefreshButton, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 2);
	Button1 = new wxButton(this, ID_BUTTON1, _("Help"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(Button1, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 2);
	aCloseButton = new wxButton(this, ID_CLOSE_BUTTON, _("Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CLOSE_BUTTON"));
	aCloseButton->SetToolTip(_("Close this dialog"));
	BoxSizer2->Add(aCloseButton, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 2);
	BoxSizer5->Add(BoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	aMainBoxSizer->Add(BoxSizer5, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Current Session"));
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Name:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer4->Add(StaticText2, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	aSessionNameEdit = new wxTextCtrl(this, ID_SESSION_NAME_EDIT, _("No current session"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SESSION_NAME_EDIT"));
	aSessionNameEdit->SetMaxLength(30);
	aSessionNameEdit->Disable();
	BoxSizer4->Add(aSessionNameEdit, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 2);
	StaticBoxSizer1->Add(BoxSizer4, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 0);
	BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	aRenameButton = new wxButton(this, ID_RENAME_BUTTON, _("Rename"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RENAME_BUTTON"));
	aRenameButton->Disable();
	aRenameButton->SetToolTip(_("Apply the modification of the name of the current session"));
	BoxSizer6->Add(aRenameButton, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 2);
	aSaveButton = new wxButton(this, ID_SAVE_BUTTON, _("Save Configuration"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SAVE_BUTTON"));
	aSaveButton->Disable();
	aSaveButton->SetToolTip(_("Save the configuration changes"));
	BoxSizer6->Add(aSaveButton, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 2);
	aDropChangeButton = new wxButton(this, ID_DROP_CHANGE_BUTTON, _("Drop Changes"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_DROP_CHANGE_BUTTON"));
	aDropChangeButton->Disable();
	aDropChangeButton->SetToolTip(_("Drop session configuration changes"));
	BoxSizer6->Add(aDropChangeButton, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 2);
	StaticBoxSizer1->Add(BoxSizer6, 0, wxALL|wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL, 0);
	aMainBoxSizer->Add(StaticBoxSizer1, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	SetSizer(aMainBoxSizer);
	aMainBoxSizer->Fit(this);
	aMainBoxSizer->SetSizeHints(this);

	Connect(ID_SESSION_LISTBOX,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&TSessionDialog::onSessionListBoxSelect);
	Connect(ID_SESSION_LISTBOX,wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,(wxObjectEventFunction)&TSessionDialog::onSessionListBoxDClick);
	Connect(ID_NEW_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TSessionDialog::onNewButtonClick);
	Connect(ID_START_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TSessionDialog::onStartButtonClick);
	Connect(ID_START_CLOSE_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TSessionDialog::onStartCloseButtonClick);
	Connect(ID_DELETE_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TSessionDialog::onDeleteButtonClick);
	Connect(ID_REFRESH_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TSessionDialog::onRefreshButtonClick);
	Connect(ID_CLOSE_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TSessionDialog::onCloseButtonClick);
	Connect(ID_SESSION_NAME_EDIT,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TSessionDialog::onSessionNameEditText);
	Connect(ID_RENAME_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TSessionDialog::onRenameButtonClick);
	Connect(ID_SAVE_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TSessionDialog::onSaveButtonClick);
	Connect(ID_DROP_CHANGE_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TSessionDialog::onDropChangeButtonClick);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&TSessionDialog::OnClose);
	//*)

	fillSessionList();
}

TSessionDialog::~TSessionDialog()
{
	//(*Destroy(TSessionDialog)
	//*)
    delete[] aSessionInfoTab;
}

void TSessionDialog::fillSessionList (bool pDontUpdateCurrent)
{
    if (aSessionInfoTab != NULL)
        delete[] aSessionInfoTab;
    aIsSaved = false;
    aSessionInfoTab = aSessionManager.GetSessionTab();
    TSessionListBuilder ListBuilder(aSessionInfoTab, aSessionListBox);
    for (aSessionCount = 0; /*until return*/; ++aSessionCount)
    {
        switch (aSessionInfoTab[aSessionCount].State)
        {
        case TSessionManager::sessionStateNONE:
            aHasFreeSession = true;
            break;

        case TSessionManager::sessionStateIDLE:
            ListBuilder.AddEntry(aSessionCount, getAvailableString());
            break;

        case TSessionManager::sessionStateUSED:
            ListBuilder.AddEntry(aSessionCount, getInUseString());
            break;

        case TSessionManager::sessionStateCUR:
            aIsSaved = true;
            //-- No break here
        case TSessionManager::sessionStateDIRT:
            aSaveButton->Enable(!aIsSaved);
            aDropChangeButton->Enable(!aIsSaved);
            aCurrentSession = aSessionCount;
            ListBuilder.AddEntry(aSessionCount, getCurrentString());
            if (!pDontUpdateCurrent)
            {
                aSessionNameEdit->SetValue(aSessionInfoTab[aSessionCount].Name);
                aSessionNameEdit->Enable();
                setNameSynch();
            }
            break;

        case TSessionManager::sessionStateLAST:
            aSessionNameEdit->Enable((aCurrentSession >= 0));
            aNewButton->Enable(aHasFreeSession);
            setButtonStatus();
            aMainBoxSizer->Fit(this);
            return;
        }
    }
    //-- Can't exit from the loop except by quitting the function
    //-- The instructions before quitting the function
    //-- are located in the sessionLAST case above
}


void TSessionDialog::setButtonStatus (void)
{
    int SelectedSession;
    bool Available = isStartable(&SelectedSession);
    aStartButton->Enable(Available);
    aStartCloseButton->Enable(Available);
    aDeleteButton->Enable(Available && (SelectedSession != 0));
}


void TSessionDialog::saveConfig (bool pCancel)
{
    if (pCancel)
        aSessionManager.RevertConfig();
    else
        aSessionManager.SaveConfig();
    aIsSaved = true;
    aSaveButton->Enable(false);
    aDropChangeButton->Enable(false);
}

void TSessionDialog::renameSession (void)
{
    bool NoChange = false;
    bool Failed   = false;

    wxString  NewName = aSessionNameEdit->GetValue();
    for (int i = 0; i < aSessionCount; ++i)
        if (aSessionInfoTab[i].Name == NewName)
        {
            if (i == aCurrentSession)
                NoChange = true;
            else
                Failed = true;
        }

    bool Result = NoChange || (!Failed && aSessionManager.RenameConfig(NewName));
    if (!Result)
    {
        wxMessageBox(wxString::Format(_("Another session is already named '%s'."), NewName.c_str()),
                   _("Rename Session Failure"));
        if (!Failed)
            //-- The renaming failed but we didn't know it would
            //-- So refresh the list so that this becomes clear
            //-- But do not update the session name list
            fillSessionList(/*pDontUpdateCurrent*/true);
    }
    else
    {
        setNameSynch();
        if (!NoChange)
            //-- No need to update current this is already done
            fillSessionList(/*pDontUpdateCurrent*/true);
    }
}

void TSessionDialog::setNameSynch (void)
{
    aIsNameChanged = false;
    aRenameButton->Disable();
}


int TSessionDialog::startSession (void)
{
    if (!askAll(/*pIsCreate*/false))
        return -1;

    int Result = aSessionManager.SetSession(TSessionListBuilder::GetSelectedSession(aSessionListBox));
    if (Result < 0)
    {
        wxMessageBox(_("A conflict occured with another WxPic instance."),
                     _("Start Session Failure"));
        //-- Refresh the session list to understand the problem
        fillSessionList();
    }
    return Result;
}


bool TSessionDialog::askSaveConfig (bool pIsCreate)
{
    if ((aCurrentSession < 0) || aIsSaved)
        return true;

    const wxChar *Message = (pIsCreate)
                ? _("Though it will be used for your new session you may want to save this configuration before leaving.\n")
                : _("The changes will be lost when switching to the new session");

    int Answer = wxMessageBox (wxString::Format(_("You have not saved your configuration in the session you are leaving.\n"
                                                  "%s\n"
                                                  "Do you want to save your configuration?"), Message),
                                _("Save your configuration"),
                                wxYES_NO | wxNO_DEFAULT | wxCANCEL | wxICON_QUESTION,
                                this);
    if (Answer == wxYES)
        saveConfig();
    return (Answer != wxCANCEL);
}


bool TSessionDialog::askRename (void)
{
    if ((aCurrentSession < 0) || !aIsNameChanged)
        return true;
    int Answer = wxMessageBox (_("You have edited your session name, but the changes have not been applied.\n"
                                 "Do you want to apply your change?"),
                                _("Rename your Session"),
                                wxYES_NO | wxNO_DEFAULT | wxCANCEL | wxICON_QUESTION,
                                this);
    if (Answer == wxYES)
        renameSession();
    return (Answer != wxCANCEL);
}


bool TSessionDialog::isStartable (int *pSelectedSession) const
{
    bool Result = false;
    int  SelectedSession = TSessionManager::sessionNONE;
    if (aSessionListBox->GetSelection() >= 0)
    {
        SelectedSession = TSessionListBuilder::GetSelectedSession(aSessionListBox);
        Result = (aSessionInfoTab[SelectedSession].State == TSessionManager::sessionStateIDLE);
    }
    if (pSelectedSession != NULL)
        *pSelectedSession = SelectedSession;
    return Result;
}


//----------------------------------------
//----------------------------------------
//--  Events methods
//----------------------------------------
//----------------------------------------

void TSessionDialog::onNewButtonClick(wxCommandEvent& event)
{
    if (!askAll(/*pIsCreate*/true))
        return;

    int Result = aSessionManager.SetSession(-1);
    fillSessionList();
    if (Result == -1)
        wxMessageBox(_("All free sessions have been created by another instance of WxPic"),
                     _("Session Creation Failed"));
}

void TSessionDialog::onStartButtonClick(wxCommandEvent& event)
{
    if (startSession() >= 0)
        fillSessionList();
}

void TSessionDialog::onStartCloseButtonClick(wxCommandEvent& event)
{
    if (startSession() >= 0)
        Close();
}

void TSessionDialog::onDeleteButtonClick(wxCommandEvent& event)
{
    if (wxMessageBox(_("Are you sure you want to delete the session and all its configuration?"),
                     _("Delete Session Confirmation"),
                     wxOK | wxCANCEL | wxICON_QUESTION,
                     this)
        == wxCANCEL)
        return;

    TSessionManager::TSessionState Result = aSessionManager.DeleteSession(TSessionListBuilder::GetSelectedSession(aSessionListBox));
    fillSessionList();
    switch (Result)
    {
        case TSessionManager::sessionStateUSED:
            wxMessageBox(_("The session is now used by another WxPic instance"),
                         _("Session Deletion Failed"));
            break;
        case TSessionManager::sessionStateNONE:
            wxMessageBox(_("The session has already been deleted by another WxPic instance"),
                         _("Session Deletion Failed"));
            break;
        default:
            ;
    }
}

void TSessionDialog::onRefreshButtonClick(wxCommandEvent& event)
{
    fillSessionList(/*pDontUpdateCurrent*/true);
}

void TSessionDialog::onCloseButtonClick(wxCommandEvent& event)
{
    Close();
}

void TSessionDialog::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto() && !askRename())
        event.Veto();
    else
        EndModal(0);
}

void TSessionDialog::onSessionNameEditText(wxCommandEvent& event)
{
    if (!aIsNameChanged)
    {
        aIsNameChanged = true;
        aRenameButton->Enable();
    }
}

void TSessionDialog::onRenameButtonClick(wxCommandEvent& event)
{
    renameSession();
}

void TSessionDialog::onSaveButtonClick(wxCommandEvent& event)
{
    saveConfig();
}

void TSessionDialog::onSessionListBoxSelect(wxCommandEvent& event)
{
    setButtonStatus();
}

void TSessionDialog::onSessionListBoxDClick(wxCommandEvent& event)
{
    if (!isStartable())
        wxMessageBox(_("This session is not available."),
                     _("Start Session Failure"),
                     wxOK,
                     this);
    else if (startSession() >= 0)
        Close();
}

void TSessionDialog::onDropChangeButtonClick(wxCommandEvent& event)
{
    saveConfig(/*Cancel*/true);
}
