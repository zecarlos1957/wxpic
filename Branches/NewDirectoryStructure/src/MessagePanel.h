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

	protected:

		//(*Identifiers(TMessagePanel)
		static const long ID_BUTTON11;
		static const long ID_TEXTCTRL13;
		//*)

	private:

		//(*Handlers(TMessagePanel)
		void onClearMsgButtonClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
