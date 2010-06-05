// Code originally written by Wolfgang Buescher (DL4YHF)
// Ported to WxWidget by Philippe Chevrier <pch@laposte.net>
// Permission is granted to use, modify, or redistribute this software
// so long as it is not sold or exploited for profit.

#ifndef COMMANDOPTION_H_INCLUDED
#define COMMANDOPTION_H_INCLUDED

#include <wx/app.h>

#define WP_TEST_MODE_GUI_SPEED 128

class TCommandOption
{
public:
    bool WinPic_fCommandLineMode;
    bool WinPic_fCommandLineOption_Erase;
    bool WinPic_fCommandLineOption_Load;  // load from file w/o program
    bool WinPic_fCommandLineOption_Program;
    bool WinPic_fCommandLineOption_Read;
    bool WinPic_fCommandLineOption_Verify;
    bool WinPic_fCommandLineOption_Quit;
    bool WinPic_fCommandLineOption_NoDelay;
    bool WinPic_fCommandLineOption_QueryBeforeOverwritingFiles;
    long WinPic_i32CmdLineOption_OverrideConfigWord;
    int  WinPic_i200msToQuit;
    int  WinPic_iTestMode;

    TCommandOption(void);
    void Load (const wxApp *App);
};

extern TCommandOption CommandOption;

#endif // COMMANDOPTION_H_INCLUDED
