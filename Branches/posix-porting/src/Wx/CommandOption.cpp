// Code originally written by Wolfgang Buescher (DL4YHF)
// Ported to WxWidget by Philippe Chevrier <pch@laposte.net>
// Permission is granted to use, modify, or redistribute this software
// so long as it is not sold or exploited for profit.

#include "CommandOption.h"
#include "Appl.h"
#include <WinPic/WinPicPr/Config.h>
#include <wx/string.h>

//-- Definition of the single instance
TCommandOption CommandOption;


TCommandOption::TCommandOption(void)
        : WinPic_fCommandLineMode           (false)
        , WinPic_fCommandLineOption_Erase   (false)
        , WinPic_fCommandLineOption_Load    (false)
        , WinPic_fCommandLineOption_Program (false)
        , WinPic_fCommandLineOption_Read    (false)
        , WinPic_fCommandLineOption_Verify  (false)
        , WinPic_fCommandLineOption_Quit    (false)
        , WinPic_fCommandLineOption_NoDelay (false)
        , WinPic_fCommandLineOption_QueryBeforeOverwritingFiles (true)
        , WinPic_i32CmdLineOption_OverrideConfigWord  (-1)
        , WinPic_i200msToQuit (0)
        , WinPic_iTestMode    (0)
{}


void TCommandOption::Load (const wxApp *App)
{
    wxString s;

    wxChar sz255Command[256];
    wxChar *cp;

    // Parse all arguments from the command line.
    //     Most likely, this will be an instruction to LOAD a file
    //     which will be programmed into the device later, etc.
    // There will DELIBERATELY be a certain delay between executing all args.
    for (int iCmdArgumentIndex = 1;
            iCmdArgumentIndex < App->argc;
            ++iCmdArgumentIndex)
    {
        _tcsncpy(sz255Command, App->argv[iCmdArgumentIndex], 255 );
        APPL_ShowMsg( APPL_CALLER_MAIN, 0, _("Parsing argument from command line : \"%s\""), sz255Command );
        cp = sz255Command;
        if (_tcsncmp(cp,wxT("/nodelay"),8)==0)
        {
            WinPic_fCommandLineOption_NoDelay = true;
        }
        else if (_tcsncmp(cp,wxT("/tm="),4)==0) // "/tm=" = test mode
        {
            cp+=4;
            WinPic_iTestMode = HexStringToLongint(4, cp);
            if ( WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
                APPL_LogEvent( _("ParseCommandLine: GUI-Speed-Test enabled") );
        }
        else if (_tcsncmp(cp,wxT("/overwrite"),10)==0)
        {
            // "/overwrite"  =  "don't ask silly questions if an already existing file
            //                   would be overwritten"
            WinPic_fCommandLineOption_QueryBeforeOverwritingFiles = false;
        }
        else if (_tcsncmp(cp,wxT("/device="),8)==0)
        {
            cp+=8;
            // The configuration has been loaded EARLIER,
            // so it makes sense to override the PIC DEVICE here :
            wxCharBuffer Device = wxString(cp).mb_str(wxConvISO8859_1);
            if (strcmp( Device, Config.sz40DeviceName)!=0)
            {
                APPL_ShowMsg( APPL_CALLER_PIC_PRG,0, _("Info: Device set to \"%s\" via command line ."), cp );
            }
            strncpy(Config.sz40DeviceName, Device, 40);
        }
        else if (_tcsncmp(cp,wxT("/config_word="),13)==0)
        {
            cp+=13;
            WinPic_i32CmdLineOption_OverrideConfigWord = HexStringToLongint(4, cp);
        }
        else if (_tcsncmp(cp,wxT("/e"),2)==0)
        {
            // /e = "erase"
            WinPic_fCommandLineOption_Erase   = true;
            WinPic_fCommandLineMode           = true;
        }
        else if (_tcsncmp(cp,wxT("/p"),2)==0)
        {
            // /p = "program"
            WinPic_fCommandLineOption_Program = true;
            WinPic_fCommandLineMode           = true;
        }
        else if (_tcsncmp(cp,wxT("/q"),2)==0)
        {
            // /q = "quit"
            WinPic_fCommandLineOption_Quit    = true;
            WinPic_fCommandLineMode           = true;
            if ( cp[2]==wxT('=') && cp[3]>wxT('0') && cp[4]<=wxT('9')  )
            {
                // "additional number of SECONDS before quitting" ?
                WinPic_i200msToQuit = (cp[3]-wxT('0')) * 5;
            }
        }
        else if (_tcsncmp(cp,wxT("/r"),2)==0)
        {
            // /r = "read"
            WinPic_fCommandLineOption_Read    = true;
            WinPic_fCommandLineMode           = true;
        }
        else if (_tcsncmp(cp,wxT("/v"),2)==0)
        {
            // /v = "verify"
            WinPic_fCommandLineOption_Verify  = true;
            WinPic_fCommandLineMode           = true;
        }
        else // none of these commands, guess the string is a FILENAME..
        {
            _tcscpy( Config.sz255HexFileName, cp );
            // Since 2004-01-09:  If the filename was the *ONLY* parameter,
            //                    automatically set the 'LOAD' flag
            //                    to simplify drag-and-drop on WinPic's icon.
            if ( App->argc == 1 )
            {
                if ( wxFileExists(Config.sz255HexFileName) )
                    WinPic_fCommandLineOption_Load = true;
            }
        }
    } // end for <all command line arguments>
} // end WinPic_ParseCommandLine()

