///////////////////////////////////////////////////////////////////////////////
// Name:        WxApp.cpp
// Purpose:     Create the WxPic main frame
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.03.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////


#include "MainFrame.h"
#include <wx/app.h>
#include <wx/snglinst.h>
#include <wx/msgdlg.h>
#include <wx/filename.h>
#include "Appl.h"
#include "Language.h"
#include <../resources/Resource.h>

//(*AppHeaders
#include <wx/image.h>
//*)

class MyApp : public wxApp
{
	public:
		virtual bool OnInit();
		virtual int  OnExit();

        MyApp(void) {}

    private:
        wxSingleInstanceChecker aInstanceChecker;
};

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    bool wxsOK = true;

    TLanguage::SetLangDefDir();
    MainFrame::TheIniFile.SetPath(_T("/LANGUAGE"));
    MainFrame::TheIniFile.Read(_T("Name"), &MainFrame::TheLanguageName);
    if (MainFrame::TheLanguageName != BUILT_IN_NAME)
        TLanguage::SetLanguage(MainFrame::TheLanguageName);
    if (MainFrame::TheLanguageName.IsEmpty())
        MainFrame::TheLanguageName = BUILT_IN_NAME;
#if defined( __WXMSW__ )
    TLanguage::SetHelp();
//#elif defined (__WXMAC__)
#else // elif defined( __WXGTK__ )
	if( not TLanguage::SetHelp() )// try for Static build executables first those doesn't require install.
	 TLanguage::SetHelp( _T("/usr/share/doc/packages/wxpic/") );
#endif

    if (aInstanceChecker.Create(APPLICATION_NAME)
    &&  aInstanceChecker.IsAnotherRunning())
    {
        wxMessageBox( _("Please do not start more than\none instance of this program !"),
           _("Error - WxPic already running"),
           wxICON_EXCLAMATION | wxOK );
        wxsOK = false;
    }

    TResource::Load();
    SetTopWindow(MainFrame::TheMainFrame);

    //(*AppInitialize
    wxInitAllImageHandlers();
    //*)

    if (wxsOK)
        wxsOK = MainFrame::CreateAndShow();

    return wxsOK;
}


int MyApp::OnExit()
{
    TResource::Unload();
    return wxApp::OnExit();
}
