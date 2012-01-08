/*-------------------------------------------------------------------------*/
/*  Filename: Config.cpp                                                   */
/*                                                                         */
/*  Purpose:                                                               */
/*     Implementation of Config.h                                          */
/*                                                                         */
/*  Author:                                                                */
/*     Copyright 2011 Philippe Chevrier pch @ laposte.net                  */
/*                                                                         */
/*  License:                                                               */
/*     New WxPic Code is licensed under GPLV3 conditions                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/

//-------------------------------------------------------------------------
//-- Implementation notes:
//--   Allows up to 20 sessions to store different configurations
//--   that can be run simultaneously
//--
//--   Note that WxPic is not protected against conflict between
//--   instances of WxPic trying to use or modify the same session
//--   at exactly the same time. Though this is VERY unlikely because
//--   this would require the operator to act on the both instances
//--   at almost the same time
//--   But WxPic is protected against conflicting actions made at distinct
//--   moments (such as deleting a session and then trying to rename it
//--   with another instance)
//-------------------------------------------------------------------------

#include "Config.h"
#include "Language.h"
#include "SessionDialog.h"
#include "FitInDisplay.h"


#include <wx/app.h>
#include <wx/config.h>
#include <wx/snglinst.h>
#include <wx/cmdline.h>
#include <wx/msgout.h>

#include <Appl.h>      // call the APPLication to display message strings
#include <WinPicPr/PIC_HW.h>   // interface types etc for default values
#include <WinPicPr/PIC_PRG.h>

//-- Some constant definitions
static const wxChar * const theSessionNamePath = _T("/SessionName");
static const wxChar * const theMRFPath         = _T("/MostRecentFiles");
static const wxChar * const theMRFileIndexKey  = _T("LastIndex");

enum
{
    sessionMAX     = 20,

    mostRecentMAX  = 6, //-- No more than 10 to keep index on one digit
};


TSessionConfig *TSessionConfig::theConfig;
wxString        TSessionConfig::theLanguageName;
int             TSessionConfig::TheTestMode = 0;


static wxConfigBase &getTheConfigIO (void)
{
    return *wxConfig::Get();
}

//----------------------------------------------------------------------
//--   Local functions converting the session number in session name
//--   Conversion depends on the context
//--   - Global context for naming the inter-process mutext
//--   - The path to session configuration parameters with legacy path for default session
//--   - The default session name for the operator
//--   - The reversible session number used as key to the session name
//--
//--   Last one convert a Most Recent File (MRF) index in corresponding key
//----------------------------------------------------------------------

static wxString getExternSessionName (int pSession)
{
    return wxString::Format(_T("WxPic-%s-Session%02d"), wxGetUserId().c_str(), pSession);
}
static wxString getSessionPath (int pSession)
{
    return (pSession == TSessionManager::sessionDEFAULT) ? wxString(_T("Config")) : wxString::Format(_T("Session%02d-Config"), pSession);
}
static wxString getSessionDefaultName (int pSession)
{
    return (pSession == TSessionManager::sessionDEFAULT) ? wxString(_T("Default")) : wxString::Format(_T("Session%02d"), pSession);
}
static wxString getSessionNumber (int pSession)
{
    return wxString::Format(_T("%02d"), pSession);
}
static wxString getMRFKey (int pIndex)
{
    return wxString::Format(_T("file%d"), pIndex);
}
//----------------------------------------------------------------------
//-- Public Static Methods

/**static*/bool TSessionConfig::Init (const wxApp *pApp)
{
    wxConfigBase  &ConfigIO = getTheConfigIO();

    //-- Read the language in parameters
    TLanguage::Init();
    ConfigIO.SetPath(_T("/LANGUAGE"));
    ConfigIO.Read(_T("Name"), &theLanguageName);

    //-- Set the Language and Help
    if (theLanguageName != TLanguage::TheDefaultName)
        TLanguage::SetLanguage(theLanguageName);
    if (theLanguageName.IsEmpty())
        theLanguageName = TLanguage::TheDefaultName;
    TLanguage::SetHelp();

    //-- Load the command line parameters
    if (!loadCmdLineParameters(pApp))
        return false;

    bool Result;

    if (theCommandLineMode | theLoadOption | theSessionIsGiven)
        Result = loadCmdLineSession();
    else
    {
        //-- Select the configuration using Dialog if necessary
        theConfig = new TSessionConfig();
        Result = (theConfig->aSession != sessionNONE);
        if (!Result)
            //-- No session has been selected
            //-- Delete the configuration (the caller will terminate immediately)
            delete theConfig;
    }
    return Result;
}


/**static*/bool TSessionConfig::ShowDialog (wxWindow *pParent)
{
    TSessionDialog SessionDialog(*theConfig, pParent);
    SessionDialog.ShowModal();
    return (theConfig->aSession != sessionNONE);
}

/**static*/ bool TSessionConfig::QuickSelect (int pSession)
{
    wxASSERT(pSession>=0);
    return theConfig->doSetSession(pSession, /*QuickSave*/true);
}

/**static*/void TSessionConfig::ChangeLanguage (const wxString &pLang)
{
    wxConfigBase  &ConfigIO = getTheConfigIO();

    ConfigIO.SetPath(_T("/LANGUAGE"));
    ConfigIO.Write(_T("Name"), pLang);
    theLanguageName = pLang;
}

/**static*/wxArrayString TSessionConfig::GetMostRecentFiles (void)
{
    wxArrayString   FileTable;

    getMostRecentFiles(FileTable);
    return FileTable;
}

/**static*/wxArrayString TSessionConfig::AddMostRecentFile(const wxString &pName)
{
    wxArrayString  FileTable;

    int LastIndex = getMostRecentFiles(FileTable);
    int FileCount = FileTable.GetCount();
    int i;
    for (i = 0; i < FileCount; ++i)
        if (FileTable[i] == pName)
            break;

    if (i == FileCount)
    {
        //-- pName is not found, it must be added
        //-- If the table is already full, remove the oldest item first
        if (FileTable.GetCount() >= mostRecentMAX)
            FileTable.RemoveAt(0);
        FileTable.Add(pName);
        ++LastIndex;
        if (LastIndex == mostRecentMAX)
            LastIndex = 0;
        wxString LastIndexImage = getMRFKey(LastIndex);
        wxConfigBase  &ConfigIO = getTheConfigIO();
        ConfigIO.Write(theMRFileIndexKey, LastIndexImage);
        ConfigIO.Write(LastIndexImage, pName);
    }
    return FileTable;
}

/**static*/ void TSessionConfig::ClearMostRecentFiles (void)
{
    getTheConfigIO().DeleteGroup(theMRFPath);
}

void TSessionConfig::GetRectAndColour (wxRect &pRect, wxColour &pCodeFgCol, wxColour &pCodeBgCol, wxColour &pDataFgCol, wxColour &pDataBgCol)
{
    wxConfigBase  &ConfigIO = getTheConfigIO();

    ConfigIO.SetPath(_T("/Layout"));

    int Left, Top, Width, Height;
    if (ConfigIO.Read(_T("Left"),   &Left))
        pRect.SetLeft(Left);
    if (ConfigIO.Read(_T("Top"),    &Top))
        pRect.SetTop(Top);
    if (ConfigIO.Read(_T("Width"),  &Width))
        pRect.SetWidth(Width);
    if (ConfigIO.Read(_T("Height"), &Height))
        pRect.SetHeight(Height);

    MoveToFitInDisplay(pRect);

    wxString EmptyString;
    pCodeBgCol.Set(ConfigIO.Read(_T("CodeMemBgColor"), EmptyString));
    pCodeFgCol.Set(ConfigIO.Read(_T("CodeMemFgColor"), EmptyString));
    pDataBgCol.Set(ConfigIO.Read(_T("DataMemBgColor"), EmptyString));
    pDataFgCol.Set(ConfigIO.Read(_T("DataMemFgColor"), EmptyString));
}

void TSessionConfig::SaveRectAndCloseSession (const wxRect &pRect, const wxColour &pCodeFgCol, const wxColour &pCodeBgCol, const wxColour &pDataFgCol, const wxColour &pDataBgCol)
{
    wxConfigBase  &ConfigIO = getTheConfigIO();

    ConfigIO.SetPath(_T("/Layout"));
    ConfigIO.Write(_T("Left"),   pRect.x);
    ConfigIO.Write(_T("Top"),    pRect.y);
    ConfigIO.Write(_T("Width"),  pRect.width);
    ConfigIO.Write(_T("Height"), pRect.height);
    ConfigIO.Write(_T("CodeMemBgColor"), pCodeBgCol.GetAsString(wxC2S_HTML_SYNTAX));
    ConfigIO.Write(_T("CodeMemFgColor"), pCodeFgCol.GetAsString(wxC2S_HTML_SYNTAX));
    ConfigIO.Write(_T("DataMemBgColor"), pDataBgCol.GetAsString(wxC2S_HTML_SYNTAX));
    ConfigIO.Write(_T("DataMemFgColor"), pDataFgCol.GetAsString(wxC2S_HTML_SYNTAX));
}

//----------------------------------------------------------------------
/**/ TSessionConfig::TSessionConfig (int pSession, const wxString &pSessionName, wxSingleInstanceChecker *pLock)
: aSession (pSession)
, aLock    (pLock)
, aName    (pSessionName)
, aIsSaved (true)
{
    setDefault(); //-- In case some parameters would fail reading
    loadConfig(NULL);
}

/**/ TSessionConfig::TSessionConfig (void)
: aSession (sessionDEFAULT)
, aLock    (NULL)
, aIsSaved (false)
{
    //-- Define default configuration parameter values
    setDefault();

    wxConfigBase  &ConfigIO = getTheConfigIO();

    ConfigIO.SetPath(theSessionNamePath);
    switch (ConfigIO.GetNumberOfEntries())
    {
    case 0:
        //-- Run for the first time, create default session
        RenameConfig(getSessionDefaultName(sessionDEFAULT));
        ConfigIO.SetPath(_T("/"));
        if (!ConfigIO.HasGroup(getSessionPath(sessionDEFAULT)))
            //-- Normal case: the session does not exist and the configuration doesn't either
            //-- Default config will be saved
            saveConfig();
        //-- Else The configuration already exists
        //-- This should mean that a previous version of WxPic was installed before
        //-- We will try to read the configuration of this previous version

        //-- No Break here: we will now load the session we have added

    case 1:
        //-- If there is only one session this must be the default one
        {
            ConfigIO.SetPath(theSessionNamePath);
            wxSingleInstanceChecker *Lock = getLockAndName(sessionDEFAULT, aName);
            wxASSERT(Lock != NULL);
            if (!Lock->IsAnotherRunning())
            {
                //-- This unique session is available don't need to show the session dialog
                //-- Just load its configuration
                loadConfig(Lock);
                break;
            }
            //-- The default session is already used, free the lock on this session
            delete Lock;
        }
        //-- No break here, we must show ne session dialog as in default case

    default:
        //-- We don't know yet which session will be chosen
        aSession = sessionNONE;

        //-- Open the session dialog for the operator to select a session
		TSessionDialog SessionDialog(*this, NULL);
		SessionDialog.ShowModal();
        break;
    }
}

TSessionManager::TSessionInfo *TSessionConfig::GetSessionTab (void) const
{
    wxConfigBase  &ConfigIO = getTheConfigIO();

    ConfigIO.SetPath(theSessionNamePath);

    TSessionInfo * const Result = new TSessionInfo[sessionMAX+1];
    TSessionInfo *       CurRes = Result;
    for (int Session = sessionDEFAULT; Session < sessionMAX; ++Session, ++CurRes)
    {
        if (Session == aSession)
        {
            CurRes->State = (aIsSaved) ? TSessionManager::sessionStateCUR : TSessionManager::sessionStateDIRT;
            CurRes->Name  = aName;
        }
        else
        {
            wxString SessionKey = getSessionNumber(Session);
            if (ConfigIO.Exists(SessionKey))
            {
                wxSingleInstanceChecker Lock(getExternSessionName(Session));
                ConfigIO.Read(SessionKey, &(CurRes->Name));
                CurRes->State = (Lock.IsAnotherRunning())
                                ? TSessionManager::sessionStateUSED
                                : TSessionManager::sessionStateIDLE;
            }
            else
                CurRes->State = TSessionManager::sessionStateNONE;
        }
    }
    CurRes->State = TSessionManager::sessionStateLAST;
    return Result;
}


int TSessionConfig::SetSession (int pSession)
{
    if (pSession < 0)
    {
        //-- A new session has been requested, search one free
        getTheConfigIO().SetPath(theSessionNamePath);
        wxString SessionName;
        int Session;
        wxSingleInstanceChecker *Lock;
        for (Session = 1; Session < sessionMAX; ++Session)
        {
            Lock = getLockAndName(Session, SessionName);

            if ((Lock != NULL) && (!Lock->IsAnotherRunning()))
                break; //-- Found

            delete Lock;
            Lock = NULL;
        }
        //-- No free session has been found: failed
        if (Lock == NULL)
            return -1;

        //-- Set the session as current session
        aSession = Session;
        if (aLock != NULL)
            delete aLock;
        aLock = Lock;

        //-- Create an initial name for the session
        int Number = Session;
        for (;;)
        {
            aName = getSessionDefaultName(Number);
            if (RenameConfig(aName))
                break;
            Number += 100;
        }
        //-- Save current parameters as the new session config
        saveConfig();
    }
    else if (!doSetSession (pSession, /*QuickSave*/false))
        return -1;

    return aSession;
}

TSessionManager::TSessionState TSessionConfig::DeleteSession (int pSession)
{
    wxSingleInstanceChecker Lock (getExternSessionName(pSession));
    TSessionManager::TSessionState Result = (Lock.IsAnotherRunning())
                                            ? TSessionManager::sessionStateUSED
                                            : TSessionManager::sessionStateNONE;
    if (Result == TSessionManager::sessionStateNONE)
    {
        wxConfigBase &ConfigIO = getTheConfigIO();
        ConfigIO.SetPath(theSessionNamePath);
        wxString SessionKey = getSessionNumber(pSession);
        if (ConfigIO.Exists(SessionKey))
        {
            ConfigIO.DeleteEntry(SessionKey, false);
            ConfigIO.DeleteGroup(getSessionPath(pSession));
            Result = TSessionManager::sessionStateIDLE;
        }
    }
    return Result;
}

void TSessionConfig::SaveConfig (void)
{
    if (!aIsSaved)
        saveConfig();
}

void TSessionConfig::RevertConfig (void)
{
    if (!aIsSaved)
        loadConfig(/*Lock*/NULL);
}

bool TSessionConfig::RenameConfig (const wxString &pNewName)
{
    wxConfigBase &ConfigIO = getTheConfigIO();

    ConfigIO.SetPath(theSessionNamePath);
    bool     MustContinue;
    long     Index;
    wxString Key;
    for (MustContinue = ConfigIO.GetFirstEntry(Key, Index); MustContinue; MustContinue = ConfigIO.GetNextEntry(Key, Index))
    {
        long KeyValue;
        Key.ToLong(&KeyValue);
        if (KeyValue != aSession)
        {
            wxString Value;
            ConfigIO.Read(Key, &Value);
            if (Value.CmpNoCase(pNewName) == 0)
                break;
        }
    }
    if (MustContinue)
        return false;

    ConfigIO.Write(getSessionNumber(aSession), pNewName);
    aName = pNewName;
    return true;
}


/**static*/int TSessionConfig::getMostRecentFiles(wxArrayString &pFileTable)
{
    //-- Legacy flag indicates that we have not found the new format of Most Recent File
    //-- This is the first time the program runs (or the operator has cleared the MRF list)
    //-- Though it may exist old data from an old version using slightly different format
    bool Legacy = true;
    int  Result = mostRecentMAX-1;
    wxConfigBase &ConfigIO = getTheConfigIO();

    ConfigIO.SetPath(theMRFPath);
    wxString LastIndexImage;
    if (ConfigIO.Read(theMRFileIndexKey, &LastIndexImage, wxEmptyString))
    {
        Legacy = false;
        //-- Index number in last character
        Result = LastIndexImage.GetChar(LastIndexImage.Len()-1) - _T('0');
    }
    wxString MRFile;
    int LastFound = mostRecentMAX-1;
    int CurIndex = Result;
    do
    {
        ++CurIndex;
        if (CurIndex == mostRecentMAX)
            CurIndex = 0;

        if (ConfigIO.Read (getMRFKey(CurIndex), &MRFile, wxEmptyString)
        &&  (MRFile[0] != _T('*'))) //-- In old versions, unused entries were marked by a name starting with *
        {
            pFileTable.Add(MRFile);
            LastFound = CurIndex;
        }
    }
    while (CurIndex != Result);
    if (Legacy)
        ConfigIO.Write(theMRFileIndexKey, getMRFKey(LastFound));

    return Result;
}


bool TSessionConfig::doSetSession (int pSession, bool pQuickSave)
{
    if (pSession == aSession)
        return true;  //-- Already done: exit with succes

    getTheConfigIO().SetPath(theSessionNamePath);

    wxString SessionName;
    wxSingleInstanceChecker *Lock = getLockAndName(pSession, SessionName);

    if (Lock == NULL)
        return false;

    if (Lock->IsAnotherRunning())
    {
        delete Lock;
        return false;
    }

    if (pQuickSave && !aIsSaved)
        saveConfig();

    aName = SessionName;
    aSession = pSession;
    loadConfig(Lock);
    return true;
}


void TSessionConfig::setConfigPath(const wxChar *pHeading)
{
    wxString SessionPath = getSessionPath (aSession);
    getTheConfigIO().SetPath(wxString::Format(_T("/%s/%s"), SessionPath.c_str(), pHeading));
}


void TSessionConfig::setDefault (void)
{
    memset( &a, 0, sizeof(a) );
    _tcscpy( a.ComPortName,
#ifdef __WXMSW__
           wxT("COM1"));
#else
           wxT("/dev/ttyS0"));
#endif
    a.InterfaceType     = PIC_INTF_TYPE_COM84;
    a.IdleSupplyVoltage = 1; // norm
                             // Note: the author's "JDM 2" required at least 2 microseconds before READ,
                             //       and 1 microseconds for every clock-L and clock-H-period;
                             //    so setting ExtraRdDelay_us=3
    a.ExtraClkDelay_us  = 2; //       and ExtraClkDelay_us=2 by default sounds reasonable.
    a.ExtraRdDelay_us   = 3; // seemed to be important for the JDM2

    _tcscpy(a.DeviceName, _T("PIC??????"));
    a.UnknownCodeMemorySize = 4096;  // used for PIC_DEV_TYPE_UNKNOWN..
    a.UnknownDataMemorySize = 256;   // ..for a trial to program exotic types
}

void TSessionConfig::saveConfig (void)
{
    wxConfigBase &ConfigIO = getTheConfigIO();

    setConfigPath(_T("INTERFACE"));
    ConfigIO.Write(_T("InterfaceType"),           a.InterfaceType);
    ConfigIO.Write(_T("SupportFile"),             a.InterfaceFile);
//  ConfigIO.Write(_T("PluginDLL"),        Config.sz80InterfacePluginDLL );
    ConfigIO.Write(_T("ExtraRdDelay_us"),         a.ExtraRdDelay_us);
    ConfigIO.Write(_T("ExtraClkDelay_us"),        a.ExtraClkDelay_us);
    ConfigIO.Write(_T("SlowClockPulses"),         a.SlowInterface);
    ConfigIO.Write(_T("IdleSupplyVoltage"),       a.IdleSupplyVoltage);

//  ConfigIO.Write(_T("PortAccessDriver"), Config.iWhichPortAccessDriver );

//  ConfigIO.SetPath(_T("/PROGRAMMING_ALGORITHM"));
  // ConfigIO.Write(_T("ProgModeSequence"), PIC_dev_param.iProgModeSequence );

    setConfigPath(_T("COM84_INTERFACE"));
    ConfigIO.Write(_T("ComPortName"),             a.ComPortName);

    setConfigPath(_T("LPT_INTERFACE"));
    ConfigIO.Write(_T("LptPortNumber"),           a.LptPortNr);
    ConfigIO.Write(_T("UnusualIoAddress"),        a.LptIoAddress);

    setConfigPath(_T("SESSION"));
    ConfigIO.Write(_T("HexFileName"),             a.HexFileName);

    setConfigPath(_T("PROGRAMMER"));
    ConfigIO.Write(_T("ProgramWhat"),             a.ProgramWhat);
    ConfigIO.Write(_T("UseBulkErase"),            a.UseCompleteChipErase);
    ConfigIO.Write(_T("Disconnect"),              a.DisconnectAfterProg);
    ConfigIO.Write(_T("VerifyDifferentVoltages"), a.UseDifferentVoltages);
    ConfigIO.Write(_T("DontCareForOsccal"),       a.DontCareForOsccal);
    ConfigIO.Write(_T("DontCareForBGCalib"),      a.DontCareForBGCalib);
    ConfigIO.Write(_T("ClearBufferBeforeLoading"),a.ClearBufBeforeLoad);
    ConfigIO.Write(_T("NeedVddBeforeRaisingMCLR"),a.NeedPowerBeforeMCLR );
    ConfigIO.Write(_T("VerboseMessages"),         a.VerboseMessages);

    setConfigPath(_T("PIC")); // PIC-specific stuff ...
    ConfigIO.Write(_T("PathToDevFiles"),          a.MplabDevDir);
    ConfigIO.Write(_T("DeviceType"),              a.DeviceName);
    ConfigIO.Write(_T("HasFlashMemory"),          a.HasFlashMemory);
    ConfigIO.Write(_T("UnknownCodeSize"),   (long)a.UnknownCodeMemorySize);
    ConfigIO.Write(_T("UnknownDataSize"),   (long)a.UnknownDataMemorySize);

    aIsSaved = true;
}

void TSessionConfig::loadConfig(wxSingleInstanceChecker *pLock)
{
    if (pLock != NULL)
    {
        //-- If we have 2 locks delete the old one
        if (aLock != NULL)
            delete aLock;
        //-- If we have a new Lock this is the one to keep
        aLock = pLock;
    }

    wxString      s;
    wxConfigBase &ConfigIO = getTheConfigIO();

    //-- Read Interface parameters
    setConfigPath(_T("INTERFACE"));
    ConfigIO.Read(_T("InterfaceType"),      (int*)&a.InterfaceType, a.InterfaceType);
    if ((a.InterfaceType >= PIC_INTF_TYPE_MAX) || (a.InterfaceType < PIC_INTF_TYPE_UNKNOWN))
        SetInterfaceType(PIC_INTF_TYPE_UNKNOWN);
    s.Empty();
    ConfigIO.Read(_T("SupportFile"),              &s);
    setStr((TCharDataPtr)&TData::InterfaceFile, s.c_str(), 256);
    ConfigIO.Read(_T("ExtraRdDelay_us"),          &a.ExtraRdDelay_us, a.ExtraRdDelay_us);
    ConfigIO.Read(_T("ExtraClkDelay_us"),         &a.ExtraClkDelay_us, a.ExtraClkDelay_us);
    ConfigIO.Read(_T("SlowClockPulses"),          &a.SlowInterface, 0 );
    ConfigIO.Read(_T("IdleSupplyVoltage"),        &a.IdleSupplyVoltage, 1/*norm*/ );

//  ConfigIO.Read(_T("PluginDLL"),   &s, wxEmptyString);
//  _tcsncpy(Config.sz80InterfacePluginDLL  , s.c_str(), 80);
//  ConfigIO.Read(_T("PortAccessDriver"), &Config.iWhichPortAccessDriver, CFG_PORTACCESS_SMPORT );

//  ConfigIO.SetPath(_T("/PROGRAMMING_ALGORITHM"));
//  ConfigIO.Read("ProgModeSequence", &PIC_dev_param.iProgModeSequence, 0); // 0=PROGMODE_VDD_THEN_VPP (usually except for DS41173b)

    //-- Read serial Interface parameters
    setConfigPath(_T("COM84_INTERFACE"));
    ConfigIO.Read(_T("ComPortName"),              &s, a.ComPortName );
    setStr((TCharDataPtr)&TData::ComPortName, s.c_str(), 40) ;

    //-- Read parallel Interface parameters
    setConfigPath(_T("LPT_INTERFACE"));
    ConfigIO.Read(_T("LptPortNumber"),            &a.LptPortNr, 1 );
    ConfigIO.Read(_T("UnusualIoAddress"),         &a.LptIoAddress, 0 );

    //-- Read Current Hex filename
    setConfigPath(_T("SESSION"));
    s.Empty();
    ConfigIO.Read(_T("HexFileName"),              &s, a.HexFileName);
    setStr((TCharDataPtr)&TData::HexFileName, s.c_str(), 255 ) ;

    //-- Read Programmer parameters
    setConfigPath(_T("PROGRAMMER"));
    ConfigIO.Read(_T("ProgramWhat"),              &a.ProgramWhat, PIC_PROGRAM_ALL );
    ConfigIO.Read(_T("UseBulkErase"),             &a.UseCompleteChipErase, 1 );
    ConfigIO.Read(_T("Disconnect"),               &a.DisconnectAfterProg, 1 );
    ConfigIO.Read(_T("VerifyDifferentVoltages"),  &a.UseDifferentVoltages, 1 );
    ConfigIO.Read(_T("DontCareForOsccal"),        &a.DontCareForOsccal, 0 );
    ConfigIO.Read(_T("DontCareForBGCalib"),       &a.DontCareForBGCalib, 0 );
    ConfigIO.Read(_T("ClearBufferBeforeLoading"), &a.ClearBufBeforeLoad, 0 );
    ConfigIO.Read(_T("NeedVddBeforeRaisingMCLR"), &a.NeedPowerBeforeMCLR, 1 );

    ConfigIO.Read(_T("VerboseMessages"),          &a.VerboseMessages, 0 );

    //-- Read Device parameters
    setConfigPath(_T("PIC")); // PIC-specific stuff ...
    s.Empty();
    ConfigIO.Read(_T("PathToDevFiles"),           &s );
    setStr((TCharDataPtr)&TData::MplabDevDir, s.c_str(), 255); // path to MPLAB's 'DEVICE'-folder

    // old:  Config.pic_device_type = ConfigIO.ReadInteger(section, "DeviceType",Config.pic_device_type );
    // Since the device TYPE NUMBERS keep changing and never had a useful meaning,
    //       a full PIC DEVICE NAME is saved as a STRING in the ini file since Nov. 2002 .
    s.Empty();
    ConfigIO.Read(_T("DeviceType"),               &s );
    setStr((TCharDataPtr)&TData::DeviceName, s.c_str(), 40);
    ConfigIO.Read(_T("HasFlashMemory"),           &a.HasFlashMemory, a.HasFlashMemory);

    ConfigIO.Read(_T("UnknownCodeSize"),   (long*)&a.UnknownCodeMemorySize, a.UnknownCodeMemorySize );
    ConfigIO.Read(_T("UnknownDataSize"),   (long*)&a.UnknownDataMemorySize, a.UnknownDataMemorySize );

    aIsSaved = true;
}


/**static*/ wxSingleInstanceChecker *TSessionConfig::getLockAndName (int pSession, wxString &pSessionName)
{
    wxSingleInstanceChecker *Result = new wxSingleInstanceChecker(getExternSessionName(pSession));
    if (! getTheConfigIO().Read(getSessionNumber(pSession), &pSessionName))
    {
        delete Result;
        Result = NULL;
    }
    return Result;
}


/**static*/ bool TSessionConfig::loadCmdLineSession (void)
{
    int Session;
    wxString SessionName;
    wxSingleInstanceChecker *Lock;

    getTheConfigIO().SetPath(theSessionNamePath);

    if (theSessionName.IsEmpty())
    {
        Session = 0;
        Lock = getLockAndName(0, SessionName);
    }
    else
    {
        //-- Try to find a session that matches the given name
        for (Session = sessionDEFAULT; Session < sessionMAX; ++Session)
        {
            Lock = getLockAndName(Session, SessionName);

            if (Lock != NULL)
            {
                if (SessionName.CmpNoCase(theSessionName) == 0)
                    break; //-- Session found

                delete Lock;
                Lock = NULL;
            }
        }
    }
    //-- If no session is locked this means that the requested session does not exist
    if (Lock == NULL)
    {
        printSessionNameError(_("The session '%s' does exist"));
        return false;  /// <<-- Anticipated return
    }
    //-- Can't work with a session already used by another instance
    if (Lock->IsAnotherRunning())
    {
        delete Lock;
        printSessionNameError(_("The session %s is already used"));
        return false;  /// <<-- Anticipated return
    }
    //-- The config has been found load it in a Config Object
    theConfig = new TSessionConfig(Session, SessionName, Lock);

    //-- Adjust the Device and/or Session name if they have been defined by command line parameters
    if (theDeviceNameIsGiven)
        SetDeviceName(theCmdLineDeviceName);
    if (theFilenameIsGiven)
        SetHexFileName(theCmdLineHexFilename.c_str());

    return true;
}

/**static*/ void TSessionConfig::printSessionNameError (const wxChar *pError)
{
    wxMessageOutput *MsgOut = wxMessageOutput::Get();
    if (MsgOut != NULL)
        MsgOut->Printf(pError, theSessionName.c_str());
}


//-------------------------------
//-- Command Line Parameters
//-------------------------------


//-- Definition of Command Line Parameter variables
//--------------------------------------------------
bool            TSessionConfig::theCommandLineMode    = false;
bool            TSessionConfig::theEraseOption        = false;
bool            TSessionConfig::theLoadOption         = false;
bool            TSessionConfig::theProgramOption      = false;
bool            TSessionConfig::theReadOption         = false;
bool            TSessionConfig::theVerifyOption       = false;
bool            TSessionConfig::theQuitOption         = false;
bool            TSessionConfig::theNoDelayOption      = false;
bool            TSessionConfig::theQueryBeforeOverwritingFiles = true;
bool            TSessionConfig::theDeviceNameIsGiven  = false;
bool            TSessionConfig::theSessionIsGiven     = false;
bool            TSessionConfig::theFilenameIsGiven    = false;
wxString        TSessionConfig::theCmdLineDeviceName;
wxString        TSessionConfig::theCmdLineHexFilename;
wxString        TSessionConfig::theSessionName;
long            TSessionConfig::theOverrideConfigWord = -1;
int             TSessionConfig::the200msTimeToQuit    = 0;





/**static*/ bool TSessionConfig::loadCmdLineParameters (const wxApp *pApp)
{

    //-- Command Line Syntax
    //-----------------------
    static const wxChar theProgramSwitchName  [] = wxT("p");
    static const wxChar theEraseSwitchName    [] = wxT("e");
    static const wxChar theReadSwitchName     [] = wxT("r");
    static const wxChar theVerifySwitchName   [] = wxT("v");
    static const wxChar theQuitOptionName     [] = wxT("q");
    static const wxChar theNoDelaySwitchName  [] = wxT("nodelay");
    static const wxChar theOverwriteSwitchName[] = wxT("overwrite");
    static const wxChar theDeviceOptionName   [] = wxT("device");
    static const wxChar theConfigOptionName   [] = wxT("config_word");
    static const wxChar theSessionOptionName  [] = wxT("session");

    static const wxCmdLineEntryDesc theCmdLineDesc[] =
    {
        { wxCMD_LINE_PARAM,  NULL,                   NULL, _("input or output HEX file"),                                               wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_SWITCH, theProgramSwitchName,   NULL, _("Program the device with content of the HEX file") },
        { wxCMD_LINE_SWITCH, theEraseSwitchName,     NULL, _("Erase the device") },
        { wxCMD_LINE_SWITCH, theReadSwitchName,      NULL, _("Read the device and write the result to the HEX file") },
        { wxCMD_LINE_SWITCH, theVerifySwitchName,    NULL, _("Verify the device is programmed as defined in the HEX file") },
        { wxCMD_LINE_OPTION, theQuitOptionName,      NULL, _("Quit WxPic at the end of the operations waiting specified delay in second"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_NEEDS_SEPARATOR },
        { wxCMD_LINE_SWITCH, theNoDelaySwitchName,   NULL, _("Do not wait before starting the operations") },
        { wxCMD_LINE_SWITCH, theOverwriteSwitchName, NULL, _("Do not ask before writing the HEX file when it already exists") },
        { wxCMD_LINE_OPTION, theDeviceOptionName,    NULL, _("Define the model of the device (default=last used)"),                     wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
        { wxCMD_LINE_OPTION, theConfigOptionName,    NULL, _("Overrides the config word value read in the HEX file (4 Hexa digits)"),   wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
        { wxCMD_LINE_OPTION, theSessionOptionName,   NULL, _("Defines the name of the session to use instead of the default session"),  wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR },
        { wxCMD_LINE_NONE }
    };


    wxString Error;
    wxCmdLineParser Parser(theCmdLineDesc, pApp->argc, pApp->argv);
    for(;;) //-- Will never loop (exit through break or return at first pass)
    {
        wxString OptionValue;
        if (Parser.Parse() != 0)
            return false;

        if (Parser.Found(theNoDelaySwitchName))
            theNoDelayOption = true;
        theDeviceNameIsGiven = Parser.Found(theDeviceOptionName,  &theCmdLineDeviceName);
        theSessionIsGiven    = Parser.Found(theSessionOptionName, &theSessionName);

        if (Parser.Found(theConfigOptionName, &OptionValue))
        {
            uint32_t NewConfig;
            if (HexStringToLongint(4, OptionValue.c_str(), &NewConfig) >= 0)
                theOverrideConfigWord = NewConfig;
            else
            {
                Error.Printf(_("Error: Invalid Config Word value = %s"), OptionValue.c_str() );
                break;
            }
        }
        if (Parser.Found(theEraseSwitchName))
        {
            theEraseOption     = true;
            theCommandLineMode = true;
        }
        if (Parser.Found(theProgramSwitchName))
        {
            theProgramOption   = true;
            theCommandLineMode = true;
        }
        if (Parser.Found(theReadSwitchName))
        {
            theReadOption      = true;
            theCommandLineMode = true;
        }
        if (Parser.Found(theVerifySwitchName))
        {
            theVerifyOption    = true;
            theCommandLineMode = true;
        }
        long Second;
        if (Parser.Found(theQuitOptionName, &Second))
        {
            if (Second < 0)
            {
                Error.Printf(_("Error: Invalid quit duration value = %ld (must be positive or 0)"), Second );
                break;
            }
            the200msTimeToQuit = Second * 5;
            theQuitOption      = true;
            theCommandLineMode = true;
        }
        if (Parser.Found(theOverwriteSwitchName))
        {
            theQueryBeforeOverwritingFiles = false;
            if (!theReadOption)
            {
                wxMessageOutput *MsgOut = wxMessageOutput::Get();
                if (MsgOut != NULL)
                    MsgOut->Printf(_("Info: Overwrite option ignored (no read option)"));
            }
        }
        if (Parser.GetParamCount() > 0)
        {
            theCmdLineHexFilename = Parser.GetParam();
            if (theCmdLineHexFilename.Len() > 255)
            {
                Error = _("Error: File name too long (max length=255)");
                break;
            }
            theFilenameIsGiven = true;
            if (!theReadOption
            &&  ((theProgramOption | theVerifyOption)
              || ( !theQuitOption && wxFileExists(OptionValue) ) ))
                theLoadOption = true;
        }
        else if (theProgramOption | theVerifyOption | theReadOption)
        {
            Error = _("Error: Missing HEX file name parameter");
            break;
        }
        return true;
    }
    Parser.SetLogo(Error);
    Parser.Usage();
    return false;
} // end LoadCmdLineParameters()

