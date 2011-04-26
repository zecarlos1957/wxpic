#include "MessagePanel.h"
#include "MainFrame.h"

//(*InternalHeaders(TMessagePanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(TMessagePanel)
const long TMessagePanel::ID_CLEAR_MSG_BUTTON = wxNewId();
const long TMessagePanel::ID_MESSAGE_LOG_TEXTCTRL = wxNewId();
//*)

BEGIN_EVENT_TABLE(TMessagePanel,wxPanel)
	//(*EventTable(TMessagePanel)
	//*)
END_EVENT_TABLE()

TMessagePanel::TMessagePanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name)
{
	//(*Initialize(TMessagePanel)
	wxBoxSizer* BoxSizer24;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer24 = new wxBoxSizer(wxVERTICAL);
	aClearMsgButton = new wxButton(this, ID_CLEAR_MSG_BUTTON, _("Clear"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CLEAR_MSG_BUTTON"));
	BoxSizer24->Add(aClearMsgButton, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aMessageLog = new wxTextCtrl(this, ID_MESSAGE_LOG_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_MESSAGE_LOG_TEXTCTRL"));
	BoxSizer24->Add(aMessageLog, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	SetSizer(BoxSizer24);
	BoxSizer24->Fit(this);
	BoxSizer24->SetSizeHints(this);

	Connect(ID_CLEAR_MSG_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TMessagePanel::onClearMsgButtonClick);
	//*)
}

TMessagePanel::~TMessagePanel()
{
	//(*Destroy(TMessagePanel)
	//*)
}


//---------------------------------------------------------------------------
void TMessagePanel::onClearMsgButtonClick(wxCommandEvent& event)
//void ::Btn_ClearMessagesClick(TObject *Sender)
{
    aMessageLog->Clear();
    MainFrame::TheMainFrame->aStatusBar->SetStatusText(_T(""));
    MainFrame::TheMainFrame->m_iMessagePanelUsage = MainFrame::MP_USAGE_NOTHING;
    MainFrame::TheMainFrame->aLogCount = 0;
}
//---------------------------------------------------------------------------


void TMessagePanel::EnterBatchMode (void)
{
    aMessageLog->SetFocus();
    aMessageLog->Clear();
};

