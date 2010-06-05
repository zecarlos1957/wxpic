///////////////////////////////////////////////////////////////////////////////
// Name:        TAboutU.cpp
// Purpose:     Display the window About WxPic
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.06.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////

#include "AboutU.h"
#include "Language.h"

//(*InternalHeaders(TAbtForm)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include "SVNVersion.h"

//(*IdInit(TAbtForm)
const long TAbtForm::ID_TEXTCTRL1 = wxNewId();
const long TAbtForm::ID_STATICTEXT1 = wxNewId();
const long TAbtForm::ID_VERSION_TEXT = wxNewId();
const long TAbtForm::ID_STATICTEXT2 = wxNewId();
const long TAbtForm::ID_STATICTEXT3 = wxNewId();
const long TAbtForm::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(TAbtForm,wxDialog)
	//(*EventTable(TAbtForm)
	//*)
END_EVENT_TABLE()

TAbtForm::TAbtForm(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(TAbtForm)
	wxBoxSizer* BoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, wxT("wxID_ANY"));
	SetClientSize(wxSize(409,271));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	aAuthorAndLicenseMemo = new wxTextCtrl(this, ID_TEXTCTRL1, _("WxPic\n-------\nSee Credits and Terms Of Use below .\n\n"), wxDefaultPosition, wxSize(300,150), wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, wxT("ID_TEXTCTRL1"));
	BoxSizer1->Add(aAuthorAndLicenseMemo, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Version:"), wxDefaultPosition, wxDefaultSize, 0, wxT("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	aVersionText = new wxStaticText(this, ID_VERSION_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, wxT("ID_VERSION_TEXT"));
	FlexGridSizer1->Add(aVersionText, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Date:"), wxDefaultPosition, wxDefaultSize, 0, wxT("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aDateText = new wxStaticText(this, ID_STATICTEXT3, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, wxT("ID_STATICTEXT3"));
	FlexGridSizer1->Add(aDateText, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	BoxSizer1->Add(FlexGridSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Button1 = new wxButton(this, ID_BUTTON1, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("ID_BUTTON1"));
	BoxSizer1->Add(Button1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	SetSizer(BoxSizer1);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TAbtForm::OnButton1Click);
	//*)
	wxString Version;
	wxString Date;
	if (SVN_MANAGED)
	{
        Version.Printf(_("WxPic %s Rev %d"), SVN_VERSION, SVN_REVISION);
        Date = SVN_DATE;
	}
    else
    {
        Version = _("Unmanaged WxPic Version built");
        Date = wxT(__DATE__ " " __TIME__);
    }
	aVersionText->SetLabel(Version);
	aDateText->SetLabel(Date);
	aAuthorAndLicenseMemo->SetValue(aAuthorAndLicenseMemo->GetValue()
                                    + TLanguage::GetFileText(wxT("Author.txt"))
                                    + _("\n\nTerms of Use\n--------------\n\n")
                                    + TLanguage::GetFileText(wxT("License.txt")));
}

TAbtForm::~TAbtForm()
{
	//(*Destroy(TAbtForm)
	//*)
}



void TAbtForm::OnButton1Click(wxCommandEvent& event)
{
    Close();
}
