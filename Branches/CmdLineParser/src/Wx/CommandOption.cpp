// Code originally written by Wolfgang Buescher (DL4YHF)
// Ported to WxWidget by Philippe Chevrier <pch@laposte.net>
// Permission is granted to use, modify, or redistribute this software
// so long as it is not sold or exploited for profit.

#include "CommandOption.h"
#include "Appl.h"
#include <WinPic/WinPicPr/Config.h>
#include <wx/string.h>
#include <wx/cmdline.h>

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

static const wxChar theProgramSwitchName  [] = wxT("p");
static const wxChar theEraseSwitchName    [] = wxT("e");
static const wxChar theNoDelaySwitchName  [] = wxT("nodelay");
static const wxChar theReadSwitchName     [] = wxT("r");
static const wxChar theOverwriteSwitchName[] = wxT("overwrite");
static const wxChar theVerifySwitchName   [] = wxT("v");
static const wxChar theDeviceOptionName   [] = wxT("device");
static const wxChar theConfigOptionName   [] = wxT("config_word");
static const wxChar theQuitOptionName     [] = wxT("q");

static const wxCmdLineEntryDesc theCmdLineDesc[] =
{
    { wxCMD_LINE_PARAM,  NULL,                   NULL, _("input or output HEX file"),                                               wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, theProgramSwitchName,   NULL, _("Program the device with content of the HEX file") },
    { wxCMD_LINE_SWITCH, theEraseSwitchName,     NULL, _("Erase the device") },
    { wxCMD_LINE_SWITCH, theNoDelaySwitchName,   NULL, _("Do not wait before starting the operations") },
    { wxCMD_LINE_SWITCH, theReadSwitchName,      NULL, _("Read the device and write the result to the HEX file") },
    { wxCMD_LINE_SWITCH, theOverwriteSwitchName, NULL, _("Do not ask before writing the HEX file when it already exists") },
    { wxCMD_LINE_SWITCH, theVerifySwitchName,    NULL, _("Verify the device") },
    { wxCMD_LINE_OPTION, theDeviceOptionName,    NULL, _("Model of the device to program (default=last used)"),                     wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_OPTION, theConfigOptionName,    NULL, _("Overrides the config word value read in the HEX file (4 Hexa digits)"),   wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_OPTION, theQuitOptionName,      NULL, _("Quit WxPic at the end of the operation waiting specified delay (0-9 s)"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_NEEDS_SEPARATOR },
    { wxCMD_LINE_NONE }
};



bool TCommandOption::Load (const wxApp *pApp)
{
    wxString Error;
    wxCmdLineParser Parser(theCmdLineDesc, pApp->argc, pApp->argv);
    for(;;) //-- Will never loop (exit through break or return at first pass)
    {
        wxString OptionValue;
        if (Parser.Parse() != 0)
            return false;

        APPL_ShowMsg( APPL_CALLER_PIC_PRG, 0, _("Info: Parsing argument from command line") );
        if (Parser.Found(theNoDelaySwitchName))
            WinPic_fCommandLineOption_NoDelay = true;
        if (Parser.Found(theDeviceOptionName, &OptionValue))
        {
            const wxWX2MBbuf DeviceName = OptionValue.mb_str(wxConvISO8859_1);
            if (strcmp( DeviceName, Config.sz40DeviceName) != 0)
            {
                APPL_ShowMsg( APPL_CALLER_PIC_PRG, 0, _("Info: Device set to \"%s\" via command line"), OptionValue.c_str() );
                strncpy(Config.sz40DeviceName, DeviceName, 40);
            }
        }
        if (Parser.Found(theConfigOptionName, &OptionValue))
        {
            long NewConfig = HexStringToLongint(4, OptionValue.c_str());
            if (NewConfig >= 0)
                WinPic_i32CmdLineOption_OverrideConfigWord = NewConfig;
            else
            {
                Error.Printf(_("Error: Invalid Config Word value = %s (ignored)"), OptionValue.c_str() );
                break;
            }
        }
        if (Parser.Found(theEraseSwitchName))
        {
            WinPic_fCommandLineOption_Erase   = true;
            WinPic_fCommandLineMode           = true;
        }
        if (Parser.Found(theProgramSwitchName))
        {
            WinPic_fCommandLineOption_Program = true;
            WinPic_fCommandLineMode           = true;
        }
        if (Parser.Found(theReadSwitchName))
        {
            WinPic_fCommandLineOption_Read    = true;
            WinPic_fCommandLineMode           = true;
        }
        if (Parser.Found(theVerifySwitchName))
        {
            WinPic_fCommandLineOption_Verify  = true;
            WinPic_fCommandLineMode           = true;
        }
        long Second;
        if (Parser.Found(theQuitOptionName, &Second))
        {
            if (Second < 0)
            {
                Error.Printf(_("Error: Invalid quit duration value = %ld (must be positive or 0)"), Second );
                break;
            }
            WinPic_i200msToQuit = Second * 5;
            WinPic_fCommandLineOption_Quit    = true;
            WinPic_fCommandLineMode           = true;
        }
        if (Parser.Found(theOverwriteSwitchName))
        {
            WinPic_fCommandLineOption_QueryBeforeOverwritingFiles = false;
            if (!WinPic_fCommandLineOption_Read)
                APPL_ShowMsg( APPL_CALLER_PIC_PRG, 0, _("Info: Overwrite option ignored (no read option)") );
        }
        if (Parser.GetParamCount() > 0)
        {
            OptionValue = Parser.GetParam();
            if (OptionValue.Len() > 255)
            {
                Error = _("Error: File name too long (max length=255)");
                break;
            }
            wxStrcpy(Config.sz255HexFileName, OptionValue.c_str());
            if (!WinPic_fCommandLineOption_Read
            &&  (WinPic_fCommandLineOption_Program | WinPic_fCommandLineOption_Verify)
              || ( !WinPic_fCommandLineOption_Quit && wxFileExists(OptionValue) ))
                WinPic_fCommandLineOption_Load = true;
        }
        else if (WinPic_fCommandLineOption_Program | WinPic_fCommandLineOption_Verify | WinPic_fCommandLineOption_Read)
        {
            Error = _("Error: Missing HEX file name parameter");
            break;
        }
        return true;
    }
    Parser.SetLogo(Error);
    Parser.Usage();
    return false;
} // end WinPic_ParseCommandLine()

