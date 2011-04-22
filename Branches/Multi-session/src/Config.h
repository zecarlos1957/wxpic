/*-------------------------------------------------------------------------*/
/*     CONFIG - module for DL4YHF's simple PIC-Programmer under Windows    */
/*                                                                         */
/*     Ported to WxPic by Philippe Chevrier                                */
/*-------------------------------------------------------------------------*/
//#include <tchar.h> //No linux support for this
#include <stdint.h>
#include <wx/arrstr.h>

#include "SessionManagerInt.h"
#include <WinPicPr/PIC_HW.h>

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
    static bool Init (wxWindow *pParent);

    //-- To be called to display the Session dialog and (possibly) change of session and config
    //-- If ShowDialog returns false caller mustt terminate AFTER CALLING SaveRectAndCloseSession
    static bool ShowDialog (wxWindow *pParent);

    //-- Return the name of the current selected Language name
    //-- (may not be the language of the translation if changed after initialisation)
    static const wxString &GetLanguageName (void) { return theLanguageName; }

    //-- Store the operator selected language
    //-- Do not actually change the language (this needs an application restart)
    static void ChangeLanguage (const wxString &pLang);

    //-- Save config parameters
    static void SaveCurConfig (void) { theConfig->SaveConfig(); }

    //-- Maximum number of Most Recent Files
    static int GetMaxMRF (void);

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



    //-- Interface TSessionDialog::TSessionManager
    //--------------------------------------------
    TSessionInfo  *GetSessionTab (void) const;
    int            SetSession    (int pSession);
    TSessionState  DeleteSession (int pSession);
    void           SaveConfig    (void);
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


    //-- The constructor is private because it is called only by Init
    /**/ TSessionConfig (wxWindow *pParent, wxConfigBase &pConfigIO);

    void saveConfig        (wxConfigBase &pConfigIO);
    void loadConfig        (wxConfigBase &pConfigIO, int pSession, wxSingleInstanceChecker *pLock);
    void setConfigPath     (wxConfigBase &pConfigIO, const wxChar *pHeading);

    static int getMostRecentFiles(wxConfigBase &pConfigIO, wxArrayString &pFileTable);


    static wxString     theLanguageName;

    typedef wxChar TData::*TCharDataPtr;
    typedef int    TData::*TIntDataPtr;
    void setStr (TCharDataPtr pVariable, const wxChar *pValue, size_t pLength)
        { _tcsncpy(&(a.*pVariable), pValue, pLength); (&(a.*pVariable))[pLength]=_T('\0'); aIsSaved = false; }
    void setInt (TIntDataPtr pVariable, int pValue)
        { if (pValue != a.*pVariable) { a.*pVariable = pValue; aIsSaved = false; } }

    static TSessionConfig *theConfig;
};



/*----------------- data types --------------------------------------------*/

typedef struct  // T_CONFIG structure, used for var Config .
{
  wxChar sz40ComPortName[41]; // Device name of the COM port
  int iLptPortNr;  // used for the TAIT-style programmer + "PIP84" (used by SM6LKM)
  int iLptIoAddress; // can be modified for LPT3, LPT4 etc.  0="standard" value
//  int iWhichPortAccessDriver;   // since 2005-11, CFG_PORTACCESS_...

  char sz40DeviceName[44]; // like PIC16F628 (complete name, used in combo and ini file)
  int pic_interface_type;  // PIC_INTF_TYPE_COM84 for example
  int iExtraRdDelay_us;    // extra read-delay before sampling the DATA-IN line
  int iExtraClkDelay_us;   // extra lengthening of the clock pulses
  int iSlowInterface;      // 0=use fast clock pulses,  1=use SLOW clock pulses for "slow" interfaces
                           // (in fact, make MOST delay loops longer by a factor of ten)
  int iIdleSupplyVoltage;  // added 2005-09-29 for "production grade" - programmers

  wxChar sz255InterfaceSupportFile[256]; // only applicable for 'custom' interfaces defined in an INI file
//  wxChar sz80InterfacePluginDLL[81];   // only applicable for 'custom' interfaces using a plugin-DLL
  wxChar sz255MplabDevDir[256];     // path to MPLAB's 'DEVICE'-folder

  int iProgramWhat;         // PIC_PROGRAM_ALL for example
  int iUseCompleteChipErase;      // 0=no 1=yes  (once called "bulk erase" but too misleading now,
                                  //              since "bulk erase" doesn't erase "everything"
                                  //              in certain devices. Thanks Microchip --- grrr )
  int iDisconnectAfterProg;       // 0=no 1=yes
  int iVerifyAtDifferentVoltages; // 0=no 1=yes
 // int iVppVddSequence;    //  PROGMODE_VPP_THEN_VDD, PROGMODE_VDD_THEN_VPP or PROGMODE_AS_SPECIFIED
                            //   (was a result of incomplete old programming specs)
                            //
  int iDontCareForOsccal;   // 0= NORMAL OPERATION: take care not to lose OSCCAL value
                            // 1= SPECIAL: TREAT OSCCAL LOCATION LIKE NORMAL CODE MEMORY
  int iDontCareForBGCalib;  // 0= NORMAL OPERATION: save and restore bandgap calib bits
  int iClearBufBeforeLoad;  // 0= do NOT clear old buffer,  1=clear buffer before loading
  int iNeedPowerBeforeRaisingMCLR;  // 0= do NOT turn Vdd on before lifting MCLR from 0V to Vpp
                                    // 1= Turn Vdd on, wait <100 us, then lift MCLR from 0V to Vpp
            // Vdd/Vpp sequence: A NEVER ENDING STORY ! More about this in PIC_HW.C::PIC_HW_ProgMode() !
  int iVerboseMessages;     // 0= normal message display,   1= a LOT of display messages (for debugging)

  wxChar sz255HexFileName[256];


  uint32_t dwUnknownCodeMemorySize;  // used for PIC_DEV_TYPE_UNKNOWN..
  uint32_t dwUnknownDataMemorySize;  // ..for a trial to program exotic types
  int  iUnknownDevHasFlashMemory;    // useful for unknown PIC devices

} T_CONFIG;


/* EOF <config.h> */
