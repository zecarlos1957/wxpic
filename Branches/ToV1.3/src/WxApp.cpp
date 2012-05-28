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

#ifndef __WXMSW__
#include <wx/msgdlg.h>
#include <unistd.h>
#include <pwd.h>
#endif // ndef __WXMSW__

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

#ifndef __WXMSW__
static bool CheckUserID (void)
{
    if (!iopl(3)==0)
        //-- If we fail to get the IO privileges there is no risk of problem with UID
        return  true;

    //-- return to normal privilege level
    iopl(0);

    //--Get the User UID
    uid_t       Uid = getuid();
    struct stat HomeStat;
    const char *HomeDir  = getenv("HOME");
    stat(HomeDir, &HomeStat);
    bool Result = (Uid == HomeStat.st_uid);
    if (!Result)
    {
        const passwd *UserData = getpwuid(Uid);
        const char   *UserName = strdup(UserData->pw_name);
        UserData = getpwuid(HomeStat.st_uid);
        Result = (wxMessageBox(wxString::Format(_("WxPic is running with priviledges under %hs account (UID=%u)"
                                                  " but with home directory (%hs) that belongs to %hs (UID=%u).\n"
                                                  "This discrepency may cause WxPic and Browser file protection corruption."
                                                  " This is generally caused by the use of sudo without -H option."
                                                  " WxPic will abort to avoid corruption. Click No to continue (not recommended)" ),
                                                UserName, Uid, HomeDir, UserData->pw_name, HomeStat.st_uid),
                               _("Invalid Execution Environment"),
                               wxYES_NO | wxICON_EXCLAMATION )
                  == wxNO);
        delete  UserName;
    }
    return Result;
}
#endif // ndef __WXMSW__

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
    //(*AppInitialize
    wxInitAllImageHandlers();
    //*)

    TResource::Load();

    TSessionConfig::InitLanguage();

#ifndef __WXMSW__
    if (!CheckUserID())
        return false;
#endif // ndef __WXMSW__

    bool wxsOK = TSessionConfig::Init(this);

    if (wxsOK)
        SetTopWindow(MainFrame::CreateAndShow());

    return wxsOK;
}


int MyApp::OnExit()
{
    TResource::Unload();
    return wxApp::OnExit();
}
