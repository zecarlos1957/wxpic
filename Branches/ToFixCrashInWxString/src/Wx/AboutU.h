///////////////////////////////////////////////////////////////////////////////
// Name:        TAboutU.h
// Purpose:     Display the window About WxPic
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.06.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////

#ifndef TABOUTU_H
#define TABOUTU_H

//(*Headers(TAbtForm)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class TAbtForm: public wxDialog
{
	public:

		TAbtForm(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~TAbtForm();

		//(*Declarations(TAbtForm)
		wxTextCtrl* aAuthorAndLicenseMemo;
		wxStaticText* StaticText2;
		wxStaticText* aVersionText;
		wxButton* Button1;
		wxStaticText* StaticText1;
		wxStaticText* aDateText;
		//*)

	protected:

		//(*Identifiers(TAbtForm)
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT1;
		static const long ID_VERSION_TEXT;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(TAbtForm)
		void OnTextCtrl1Text(wxCommandEvent& event);
		void OnTextCtrl1Text1(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
