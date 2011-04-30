/*-------------------------------------------------------------------------*/
/*  Filename: Config.h                                                     */
/*                                                                         */
/*  Purpose:                                                               */
/*     Manage the WxPic parameters (reading / writing)                     */
/*     Starting with V1.3 most of parameters are managed by session        */
/*     Manage the creation, deletion, renaming of these sessions           */
/*                                                                         */
/*  Author:                                                                */
/*     Copyright 2009-2011 Philippe Chevrier pch @ laposte.net             */
/*     from software originally written by Wolfgang Buescher (DL4YHF)      */
/*                                                                         */
/*  License:                                                               */
/*     Ported Code is licensed under GPLV3 conditions with original code   */
/*     restriction :                                                       */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*                                                                         */
/*-------------------------------------------------------------------------*/


#include <stdint.h>
#include <wx/arrstr.h>

#include "SessionList.h"
#include <WinPicPr/PIC_HW.h>

class wxApp;
class wxWindow;
class wxConfigBase;
class wxColour;
class wxRect;
class wxSingleInstanceChecker;


class TSessionConfig : public TSessionManager
{
public:
    //-- Must be called once at startup
    //-- If Init returns false caller must terminate immediatly (don't call SaveRectAndCloseSession)
    static bool Init (const wxApp *pApp);

    //-- To be called to display the Session dialog and (possibly) change of session and config
    //-- If ShowDialog returns false caller mustt terminate AFTER CALLING SaveRectAndCloseSession
    static bool ShowDialog (wxWindow *pParent);

    //-- Return the session tab from static call
    static TSessionInfo *GetCurSessionTab (void) { return theConfig->GetSessionTab(); }

    //-- Select a new session and save config of previous if necessary
    static bool QuickSelect (int pSession);

    //-- Return the name of the current selected Language name
    //-- (may not be the language of the translation if changed after initialisation)
    static const wxString &GetLanguageName (void) { return theLanguageName; }

    //-- Store the operator selected language
    //-- Do not actually change the language (this needs an application restart)
    static void ChangeLanguage (const wxString &pLang);

    //-- Save config parameters
    static void SaveCurConfig (void) { theConfig->SaveConfig(); }

    //-- Get the list of most recent files in config file
    static wxArrayString GetMostRecentFiles (void);

    //-- Add a file to the most recent file list
    static wxArrayString AddMostRecentFile (const wxString &pName);

    //-- Clear the most recent file list
    static void ClearMostRecentFiles (void);

    //-- Get the Window Rect and colours
    static void GetRectAndColour (wxRect &pRect, wxColour &pCodeFgCol, wxColour &pCodeBgCol, wxColour &pDataFgCol, wxColour &pDataBgCol);

    //-- Save the given Window rectangle and colours and destroy the configuration manager
    //-- No call to Session config may be made after this one
    static void SaveRectAndCloseSession (const wxRect &pRect, const wxColour &pCodeFgCol, const wxColour &pCodeBgCol, const wxColour &pDataFgCol, const wxColour &pDataBgCol);


    //-- Config Getters and Setters
    //-------------------------------

    // Device name of the COM port
    static EInterfaceType GetInterfaceType                   (void) { return theConfig->a.InterfaceType; }
    static void           SetInterfaceType  (EInterfaceType pValue) { theConfig->setInt((TIntDataPtr)&TData::InterfaceType, (int)pValue); if (pValue != theConfig->a.InterfaceType) {theConfig->a.InterfaceType = pValue; theConfig->aIsSaved = false;} }

    static const wxChar  *GetInterfaceFile                   (void) { return theConfig->a.InterfaceFile; }
    static void           SetInterfaceFile (const wxString &pValue) { theConfig->setStr((TCharDataPtr)&TData::InterfaceFile, pValue.c_str(), 255); }

    static const wxChar  *GetComPortName                     (void) { return theConfig->a.ComPortName; }
    static void           SetComPortName   (const wxString &pValue) { theConfig->setStr((TCharDataPtr)&TData::ComPortName, pValue.c_str(), 40); }

    static int            GetLptPortNr                       (void) { return theConfig->a.LptPortNr; }
    static void           SetLptPortNr                 (int pValue) { theConfig->setInt(&TData::LptPortNr, pValue); }

    static int            GetExtraRdDelay_us                 (void) { return theConfig->a.ExtraRdDelay_us; }
    static void           SetExtraRdDelay_us           (int pValue) { theConfig->setInt(&TData::ExtraRdDelay_us, pValue); }

    static int            GetExtraClkDelay_us                (void) { return theConfig->a.ExtraClkDelay_us; }
    static void           SetExtraClkDelay_us          (int pValue) { theConfig->setInt(&TData::ExtraClkDelay_us, pValue); }

    static int            GetSlowInterface                   (void) { return theConfig->a.SlowInterface; }
    static void           SetSlowInterface             (int pValue) { theConfig->setInt(&TData::SlowInterface, pValue); }

    static int            GetIdleSupplyVoltage               (void) { return theConfig->a.IdleSupplyVoltage; }
    static void           SetIdleSupplyVoltage         (int pValue) { theConfig->setInt(&TData::IdleSupplyVoltage, pValue); }

    static int            GetLptIoAddress                    (void) { return theConfig->a.LptIoAddress; }
    static void           SetLptIoAddress              (int pValue) { theConfig->setInt(&TData::LptIoAddress, pValue); }

    static const wxChar  *GetHexFileName                     (void) { return theConfig->a.HexFileName; }
    static void           SetHexFileName   (const wxString &pValue) { theConfig->setStr((TCharDataPtr)&TData::HexFileName, pValue.c_str(), 255); }

    static int            GetProgramWhat                     (void) { return theConfig->a.ProgramWhat; }
    static void           SetProgramWhat               (int pValue) { theConfig->setInt(&TData::ProgramWhat, pValue); }

    static int            GetUseCompleteChipErase            (void) { return theConfig->a.UseCompleteChipErase; }
    static void           SetUseCompleteChipErase      (int pValue) { theConfig->setInt(&TData::UseCompleteChipErase, pValue); }

    static int            GetDisconnectAfterProg             (void) { return theConfig->a.DisconnectAfterProg; }
    static void           SetDisconnectAfterProg       (int pValue) { theConfig->setInt(&TData::DisconnectAfterProg, pValue); }

    static int            GetUseDifferentVoltages            (void) { return theConfig->a.UseDifferentVoltages; }
    static void           SetUseDifferentVoltages      (int pValue) { theConfig->setInt(&TData::UseDifferentVoltages, pValue); }

    static int            GetDontCareForOsccal               (void) { return theConfig->a.DontCareForOsccal; }
    static void           SetDontCareForOsccal         (int pValue) { theConfig->setInt(&TData::DontCareForOsccal, pValue); }

    static int            GetDontCareForBGCalib              (void) { return theConfig->a.DontCareForBGCalib; }
    static void           SetDontCareForBGCalib        (int pValue) { theConfig->setInt(&TData::DontCareForBGCalib, pValue); }

    static int            GetClearBufBeforeLoad              (void) { return theConfig->a.ClearBufBeforeLoad; }
    static void           SetClearBufBeforeLoad        (int pValue) { theConfig->setInt(&TData::ClearBufBeforeLoad, pValue); }

    static int            GetNeedPowerBeforeMCLR             (void) { return theConfig->a.NeedPowerBeforeMCLR; }
    static void           SetNeedPowerBeforeMCLR       (int pValue) { theConfig->setInt(&TData::NeedPowerBeforeMCLR, pValue); }

    static int            GetVerboseMessages                 (void) { return theConfig->a.VerboseMessages; }
    static void           SetVerboseMessages           (int pValue) { theConfig->setInt(&TData::VerboseMessages, pValue); }

    static const wxChar  *GetMplabDevDir                     (void) { return theConfig->a.MplabDevDir; }
    static void           SetMplabDevDir   (const wxString &pValue) { theConfig->setStr((TCharDataPtr)&TData::MplabDevDir, pValue.c_str(), 255); }

    static const wxChar  *GetDeviceName                      (void) { return theConfig->a.DeviceName; }
    static void           SetDeviceName    (const wxString &pValue) { theConfig->setStr((TCharDataPtr)&TData::DeviceName, pValue.c_str(), 40); }

    static int            GetHasFlashMemory                  (void) { return theConfig->a.HasFlashMemory; }
    static void           SetHasFlashMemory            (int pValue) { theConfig->setInt(&TData::HasFlashMemory, pValue); }

    static uint32_t       GetUnknownCodeMemorySize           (void) { return theConfig->a.UnknownCodeMemorySize; }
    static void           SetUnknownCodeMemorySize(uint32_t pValue) { theConfig->setInt((TIntDataPtr)&TData::UnknownCodeMemorySize, (int)pValue); }

    static uint32_t       GetUnknownDataMemorySize           (void) { return theConfig->a.UnknownDataMemorySize; }
    static void           SetUnknownDataMemorySize(uint32_t pValue) { theConfig->setInt((TIntDataPtr)&TData::UnknownDataMemorySize, (int)pValue); }

    //-- Command Line Parameters

    static bool           IsCommandLineMode                  (void) { return theCommandLineMode; }
    static void           ClearCommandLineMode               (void) { theCommandLineMode = false; }

    static bool           IsEraseOption                      (void) { return theEraseOption; }
    static void           ClearEraseOption                   (void) { theEraseOption = false; }

    static bool           IsLoadOption                       (void) { return theLoadOption; }
    static void           ClearLoadOption                    (void) { theLoadOption = false; }

    static bool           IsProgramOption                    (void) { return theProgramOption; }
    static void           ClearProgramOption                 (void) { theProgramOption = false; }

    static bool           IsReadOption                       (void) { return theReadOption; }
    static void           ClearReadOption                    (void) { theReadOption = false; }

    static bool           IsVerifyOption                     (void) { return theVerifyOption; }
    static void           ClearVerifyOption                  (void) { theVerifyOption = false; }

    static bool           IsQuitOption                       (void) { return theQuitOption; }
    static void           ClearQuitOption                    (void) { theQuitOption = false; }

    static bool           IsNoDelayOption                    (void) { return theNoDelayOption; }
    static void           ClearNoDelayOption                 (void) { theNoDelayOption = false; }

    static bool           IsQueryBeforeOverwritingFiles      (void) { return theQueryBeforeOverwritingFiles; }

    static long           GetOverrideConfigWord              (void) { return theOverrideConfigWord; }

    static int            Get200msTimeToQuit                 (void) { return the200msTimeToQuit; }

    static bool           IsCmdLineDeviceName                (void) { return theDeviceNameIsGiven; }

    static int  TheTestMode;


    //-- Interface TSessionDialog::TSessionManager
    //--------------------------------------------
    TSessionInfo  *GetSessionTab (void) const;
    int            SetSession    (int pSession);
    TSessionState  DeleteSession (int pSession);
    void           SaveConfig    (void);
    void           RevertConfig  (void);
    bool           RenameConfig  (const wxString &pNewName);
    //--------------------------------------------

private:
    int                      aSession;
    wxSingleInstanceChecker *aLock;
    wxString                 aName;
    bool                     aIsSaved;

    //-- Config data
    struct TData
    {
        EInterfaceType InterfaceType;        // pic_interface_type PIC_INTF_TYPE_COM84 for example

        wxChar         ComPortName[41];      // Device name of the COM port
        int            LptPortNr;            // used for the parallel port programmer
        wxChar         InterfaceFile[256];   // only applicable for 'custom' interfaces defined by an external file
        int            ExtraRdDelay_us;      // extra read-delay before sampling the DATA-IN line
        int            ExtraClkDelay_us;     // extra lengthening of the clock pulses
        int            SlowInterface;        // 0=use fast clock pulses,  1=use SLOW clock pulses for "slow" interfaces
                                             // (in fact, make MOST delay loops longer by a factor of ten)
        int            IdleSupplyVoltage;    // for "production grade" - programmers
        int            LptIoAddress;         // can be modified for LPT3, LPT4 etc.  0="standard" value

        wxChar         HexFileName[256];
        int            ProgramWhat;          // PIC_PROGRAM_ALL for example
        int            UseCompleteChipErase; // 0=no 1=yes  (once called "bulk erase" but too misleading now,
                                             //              since "bulk erase" doesn't erase "everything"
                                             //              in certain devices. Thanks Microchip --- grrr )
        int            DisconnectAfterProg;  // 0=no 1=yes
        int            UseDifferentVoltages; // 0=no 1=yes
        int            DontCareForOsccal;    // 0= NORMAL OPERATION: take care not to lose OSCCAL value
                                             // 1= SPECIAL: TREAT OSCCAL LOCATION LIKE NORMAL CODE MEMORY
        int            DontCareForBGCalib;   // 0= NORMAL OPERATION: save and restore bandgap calib bits
        int            ClearBufBeforeLoad;   // 0= do NOT clear old buffer,  1=clear buffer before loading
        // Vdd/Vpp sequence: A NEVER ENDING STORY ! More about this in PIC_HW.C::PIC_HW_ProgMode() !
        int            NeedPowerBeforeMCLR;  // 0= do NOT turn Vdd on before lifting MCLR from 0V to Vpp
                                             // 1= Turn Vdd on, wait <100 us, then lift MCLR from 0V to Vpp
        int            VerboseMessages;      // 0= normal message display,   1= a LOT of display messages (for debugging)
        wxChar         MplabDevDir[256];     // path to MPLAB's 'DEVICE'-folder
        wxChar         DeviceName[44];       // like PIC16F628 (complete name, used in combo and ini file)
        int            HasFlashMemory;       // useful for unknown PIC devices
        uint32_t       UnknownCodeMemorySize;// used for PIC_DEV_TYPE_UNKNOWN..
        uint32_t       UnknownDataMemorySize;// ..for a trial to program exotic types
//      wxChar sz80InterfacePluginDLL[81];   // only applicable for 'custom' interfaces using a plugin-DLL
    } a;


    //-- The constructors are private because they are called only by Init
    /**/ TSessionConfig (int pSession, const wxString &pSessionName, wxConfigBase &pConfigIO, wxSingleInstanceChecker *pLock);
    /**/ TSessionConfig (wxConfigBase &pConfigIO);  //-- Ask operator which config to load (unless only one)

    //-- Set the session and if pQuickSave save config of previous session (that must exist)
    bool doSetSession      (wxConfigBase &pConfigIO, int pSession, bool pQuickSave);
    //-- Save the configuration of current session
    void saveConfig        (wxConfigBase &pConfigIO);
    //-- Load the current configuration
    //-- pLock is the session Lock that must have been acquired (or NULL if session is already current)
    void loadConfig        (wxConfigBase &pConfigIO, wxSingleInstanceChecker *pLock);
    //-- Set config default values
    void setDefault        (void);
    //-- Set the configuration path on given heading of current session
    void setConfigPath     (wxConfigBase &pConfigIO, const wxChar *pHeading);

    //-- Return the Lock and the Name of the given session
    //-- If the session does not exist, the return Lock is NULL
    //-- the pConfigIO must be set on the Session Name Path before calling
    static wxSingleInstanceChecker *getLockAndName (wxConfigBase &pConfigIO, int pSession, wxString &pSessionName);

    static int  getMostRecentFiles    (wxConfigBase &pConfigIO, wxArrayString &pFileTable);
    static bool loadCmdLineParameters (const wxApp *pApp);
    static bool loadCmdLineSession    (wxConfigBase &pConfigIO);
    static void printSessionNameError (const wxChar *pError);


    static wxString     theLanguageName;

    static bool         theCommandLineMode;
    static bool         theEraseOption;
    static bool         theLoadOption;
    static bool         theProgramOption;
    static bool         theReadOption;
    static bool         theVerifyOption;
    static bool         theQuitOption;
    static bool         theNoDelayOption;
    static bool         theQueryBeforeOverwritingFiles;
    static bool         theDeviceNameIsGiven;
    static bool         theConfigWordIsOveridden;
    static bool         theSessionIsGiven;
    static bool         theFilenameIsGiven;
    static long         theOverrideConfigWord;
    static int          the200msTimeToQuit;
    static wxString     theCmdLineDeviceName;
    static wxString     theCmdLineHexFilename;
    static wxString     theSessionName;


    typedef wxChar TData::*TCharDataPtr;
    typedef int    TData::*TIntDataPtr;
    void setStr (TCharDataPtr pVariable, const wxChar *pValue, size_t pLength)
        { _tcsncpy(&(a.*pVariable), pValue, pLength); (&(a.*pVariable))[pLength]=_T('\0'); aIsSaved = false; }
    void setInt (TIntDataPtr pVariable, int pValue)
        { if (pValue != a.*pVariable) { a.*pVariable = pValue; aIsSaved = false; } }

    static TSessionConfig *theConfig;
};

