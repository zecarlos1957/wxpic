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
#include "Config.h"
#include <wx/app.h>
#include <wx/filename.h>
#include "Appl.h"
#include "Language.h"
#include <../resources/Resource.h>

//(*AppHeaders
#include <wx/image.h>
//*)


#if 0
int BreakPoint (void)
{
    return 0;
}
int BreakOnInit = BreakPoint();
#endif

class MyApp : public wxApp
{
	public:
		virtual bool OnInit();
		virtual int  OnExit();

        MyApp(void) {}
};

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    bool wxsOK = true;

    wxsOK = TSessionConfig::Init(NULL);

    if (wxsOK)
    {
        TResource::Load();
        SetTopWindow(MainFrame::TheMainFrame);

        //(*AppInitialize
        wxInitAllImageHandlers();
        //*)

        wxsOK = MainFrame::CreateAndShow();
    }

    return wxsOK;
}


int MyApp::OnExit()
{
    TResource::Unload();
    return wxApp::OnExit();
}
