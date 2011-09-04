#ifndef MESSAGEPANEL_H
#define MESSAGEPANEL_H

//(*Headers(TMessagePanel)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/button.h>
//*)

class TMessagePanel: public wxPanel
{
	public:

		TMessagePanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name);
		virtual ~TMessagePanel();

		//(*Declarations(TMessagePanel)
		wxTextCtrl* aMessageLog;
		wxButton* aClearMsgButton;
		//*)

		void EnterBatchMode (void);

	protected:

		//(*Identifiers(TMessagePanel)
		static const long ID_CLEAR_MSG_BUTTON;
		static const long ID_MESSAGE_LOG_TEXTCTRL;
		//*)

	private:

		//(*Handlers(TMessagePanel)
		void onClearMsgButtonClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
