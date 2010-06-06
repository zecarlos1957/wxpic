/*-------------------------------------------------------------------------*/
/*     CONFIG - module for DL4YHF's simple PIC-Programmer under Windows    */
/*                                                                         */
/*     Ported to WxPic by Philippe Chevrier                                */
/*-------------------------------------------------------------------------*/
//#include <tchar.h>
#include <wx/confbase.h>
#include <stdint.h>

/*----------------- constants  --------------------------------------------*/
#define CFG_INI_FILE_NAME  "settings.ini"


//// possible values for T_CONFIG.iWhichPortAccessDriver :
//enum
//{
//    CFG_PORTACCESS_USE_API_ONLY,    /* keep hands off any register, use windows API only */
//    CFG_PORTACCESS_SMPORT,          /* this was the default port access method */
//    CFG_PORTACCESS_PORTTALK,        /* open PortTalk driver to grant access */
//    CFG_PORTACCESS_ALREADY_GRANTED, /* no driver needed, someone did this for us */
//};



/*----------------- data types --------------------------------------------*/

typedef struct  // T_CONFIG structure, used for var Config .
{
  int iComPortNr;  // used for the COM84 programmer
  int iComIoAddress; // only used if iComPortNr==0; otherwise COM1=0x03F8 etc (see pic_hw.cpp)
  int iLptPortNr;  // used for the TAIT-style programmer + "PIP84" (used by SM6LKM)
  int iLptIoAddress; // can be modified for LPT3, LPT4 etc.  0="standard" value
//  int iWhichPortAccessDriver;   // since 2005-11, CFG_PORTACCESS_...

  wxString sz40DeviceName; // like PIC16F628 (complete name, used in combo and ini file)
  int pic_interface_type;  // PIC_INTF_TYPE_COM84 for example
  int iExtraRdDelay_us;    // extra read-delay before sampling the DATA-IN line
  int iExtraClkDelay_us;   // extra lengthening of the clock pulses
  int iSlowInterface;      // 0=use fast clock pulses,  1=use SLOW clock pulses for "slow" interfaces
                           // (in fact, make MOST delay loops longer by a factor of ten)
  int iIdleSupplyVoltage;  // added 2005-09-29 for "production grade" - programmers

  wxString sz255InterfaceSupportFile; // only applicable for 'custom' interfaces defined in an INI file
  wxString sz80InterfacePluginDLL;   // only applicable for 'custom' interfaces using a plugin-DLL
  wxString sz255MplabDevDir;     // path to MPLAB's 'DEVICE'-folder

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

  wxString sz255HexFileName;


  uint32_t dwUnknownCodeMemorySize;  // used for PIC_DEV_TYPE_UNKNOWN..
  uint32_t dwUnknownDataMemorySize;  // ..for a trial to program exotic types
  int  iUnknownDevHasFlashMemory;    // useful for unknown PIC devices

} T_CONFIG;



/*---------------- variables ----------------------------------------------*/
#undef EXTERN
#ifdef _I_AM_CONFIG_
 #define EXTERN
#else
 #define EXTERN extern
#endif

EXTERN T_CONFIG Config;
EXTERN int Config_changed;


/*---------------- prototypes ---------------------------------------------*/

void CFG_Init(void);
void CFG_Load(wxConfigBase &IniFile); /* Loads configuration data             */
void CFG_Save(wxConfigBase &IniFile); /* Saves the APPLICATION'S configuration */


/* EOF <config.h> */
