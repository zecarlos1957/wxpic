/*-------------------------------------------------------------------------*/
/*     CONFIG - module for DL4YHF's simple PIC-Programmer under Windows    */
/*-------------------------------------------------------------------------*/

#include <wx/string.h>
#include <wx/confbase.h>
#include <wx/app.h>


#include "../../Wx/Appl.h"      // call the APPLication to display message strings
#include "pic_hw.h"   // interface types etc for default values
#include "pic_prg.h"

#define _I_AM_CONFIG_  /* for single-source-variables in CONFIG.H */
#include "config.h"


/*---------------- Implementation -----------------------------------------*/
void CFG_Init(void)
{
  memset( &Config, 0, sizeof(Config) );
  Config.pic_interface_type = PIC_INTF_TYPE_COM84;
  Config.iComPortNr = 1;    // enter your favorite COM port number here ;-)
  Config.iComIoAddress = 0; // do NOT use a non-standard I/O address for the COM port
  strcpy(Config.sz40DeviceName, "PIC??????");
  Config.dwUnknownCodeMemorySize = 4096;  // used for PIC_DEV_TYPE_UNKNOWN..
  Config.dwUnknownDataMemorySize = 256;   // ..for a trial to program exotic types

  _tcscpy( Config.sz255HexFileName, _T("test.hex") );
  _tcscpy( Config.sz255InterfaceSupportFile, _T("MyProg.ini") );
  Config.sz80InterfacePluginDLL[0] = '\0';
  Config.sz255MplabDevDir[0] = '\0';      // don't read DEV-files from MPLAB directly
  Config.iExtraRdDelay_us  = 3; // seemed to be important for the JDM2, when using PortTalk access
  Config.iExtraClkDelay_us = 2; // "       "   " ...  (2005-11-07)
     // Note: the author's "JDM 2" required at least 2 microseconds before READ,
     //       and 1 microseconds for every clock-L and clock-H-period;
     //    so setting Config.iExtraRdDelay_us=3
     //       and Config.iExtraClkDelay_us=2 by default sounds reasonable.

  Config.iSlowInterface = 0;    // hardly required any longer since the above "fine-tuning" delays
  Config.iIdleSupplyVoltage = 1/*norm*/ ;
  Config_changed = 0x0000;
}

void CFG_Load(wxConfigBase &IniFile) /* Loads configuration data from a file */
{
  wxString s;

  IniFile.SetPath(_T("/Config/INTERFACE"));
  IniFile.Read(_T("InterfaceType"), &Config.pic_interface_type, Config.pic_interface_type);
  IniFile.Read(_T("SupportFile"), &s, wxEmptyString);
  _tcsncpy(Config.sz255InterfaceSupportFile, s.c_str(), 256);
  IniFile.Read(_T("PluginDLL"),   &s, wxEmptyString);
  _tcsncpy(Config.sz80InterfacePluginDLL  , s.c_str(), 80);
  IniFile.Read(_T("ExtraRdDelay_us"), &Config.iExtraRdDelay_us, Config.iExtraRdDelay_us);
  IniFile.Read(_T("ExtraClkDelay_us"), &Config.iExtraClkDelay_us, Config.iExtraClkDelay_us);
  IniFile.Read(_T("SlowClockPulses"), &Config.iSlowInterface, 0 );
  IniFile.Read(_T("IdleSupplyVoltage"), &Config.iIdleSupplyVoltage, 1/*norm*/ );
//  IniFile.Read(_T("PortAccessDriver"), &Config.iWhichPortAccessDriver, CFG_PORTACCESS_SMPORT );

//  IniFile.SetPath(_T("/PROGRAMMING_ALGORITHM"));
//  IniFile.Read("ProgModeSequence", &PIC_dev_param.iProgModeSequence, 0); // 0=PROGMODE_VDD_THEN_VPP (usually except for DS41173b)

  IniFile.SetPath(_T("/Config/COM84_INTERFACE"));
  IniFile.Read(_T("ComPortNumber"), &Config.iComPortNr, Config.iComPortNr );
  IniFile.Read(_T("UnusualIoAddress"), &Config.iComIoAddress, 0 );

  IniFile.SetPath(_T("/Config/LPT_INTERFACE"));
  IniFile.Read(_T("LptPortNumber"),    &Config.iLptPortNr,    Config.iLptPortNr );
  IniFile.Read(_T("UnusualIoAddress"), &Config.iLptIoAddress, 0 );

  IniFile.SetPath(_T("/Config/SESSION"));
  IniFile.Read(_T("HexFileName"), &s, Config.sz255HexFileName);
  _tcsncpy(Config.sz255HexFileName, s.c_str(), 255 ) ;
  Config.sz255HexFileName[255]=_T('\0');  // keep strings terminated !

  IniFile.SetPath(_T("/Config/PROGRAMMER"));
  IniFile.Read(_T("ProgramWhat"), &Config.iProgramWhat, PIC_PROGRAM_ALL );
  IniFile.Read(_T("UseBulkErase"), &Config.iUseCompleteChipErase, 1 );
  IniFile.Read(_T("Disconnect"), &Config.iDisconnectAfterProg, 1 );
  IniFile.Read(_T("VerifyDifferentVoltages"), &Config.iVerifyAtDifferentVoltages, 1 );
  IniFile.Read(_T("DontCareForOsccal"), &Config.iDontCareForOsccal, 0 );
  IniFile.Read(_T("DontCareForBGCalib"), &Config.iDontCareForBGCalib, 0 );
  IniFile.Read(_T("ClearBufferBeforeLoading"), &Config.iClearBufBeforeLoad, 0 );
  IniFile.Read(_T("NeedVddBeforeRaisingMCLR"), &Config.iNeedPowerBeforeRaisingMCLR, 1 );

  IniFile.Read(_T("VerboseMessages"), &Config.iVerboseMessages, 0 );

  IniFile.SetPath(_T("/Config/PIC")); // PIC-specific stuff ...
  s.Empty();
  IniFile.Read(_T("PathToDevFiles"), &s );
  _tcsncpy(Config.sz255MplabDevDir, s.c_str(), 255); // path to MPLAB's 'DEVICE'-folder

  // old:  Config.pic_device_type = IniFile.ReadInteger(section, "DeviceType",Config.pic_device_type );
  // Since the device TYPE NUMBERS keep changing and never had a useful meaning,
  //       a full PIC DEVICE NAME is saved as a STRING in the ini file since Nov. 2002 .
  IniFile.Read(_T("DeviceType"), &s, wxEmptyString );
  wxCharBuffer DeviceName = s.mb_str(wxConvISO8859_1);
  strncpy(Config.sz40DeviceName, DeviceName, 40);
  IniFile.Read(_T("HasFlashMemory"),  &Config.iUnknownDevHasFlashMemory, Config.iUnknownDevHasFlashMemory);
  IniFile.Read(_T("UnknownCodeSize"), (long*)&Config.dwUnknownCodeMemorySize,   Config.dwUnknownCodeMemorySize );
  IniFile.Read(_T("UnknownDataSize"), (long*)&Config.dwUnknownDataMemorySize, Config.dwUnknownDataMemorySize );

  Config_changed = 0x0000;
}

void CFG_Save(wxConfigBase &IniFile) /* Saves the APPLICATION'S configuration in a file */
{
  IniFile.SetPath(_T("/Config/INTERFACE"));
  IniFile.Write(_T("InterfaceType"),    Config.pic_interface_type);
  IniFile.Write(_T("SupportFile"),      Config.sz255InterfaceSupportFile);
  IniFile.Write(_T("PluginDLL"),        Config.sz80InterfacePluginDLL );
  IniFile.Write(_T("ExtraRdDelay_us"),  Config.iExtraRdDelay_us);
  IniFile.Write(_T("ExtraClkDelay_us"), Config.iExtraClkDelay_us);
  IniFile.Write(_T("SlowClockPulses"),  Config.iSlowInterface );
  IniFile.Write(_T("IdleSupplyVoltage"),Config.iIdleSupplyVoltage );
//  IniFile.Write(_T("PortAccessDriver"), Config.iWhichPortAccessDriver );

//  IniFile.SetPath(_T("/PROGRAMMING_ALGORITHM"));
  // IniFile.Write(_T("ProgModeSequence"), PIC_dev_param.iProgModeSequence );

  IniFile.SetPath(_T("/Config/COM84_INTERFACE"));
  IniFile.Write(_T("ComPortNumber"), Config.iComPortNr );
  IniFile.Write(_T("UnusualIoAddress"), Config.iComIoAddress );

  IniFile.SetPath(_T("/Config/LPT_INTERFACE"));
  IniFile.Write(_T("LptPortNumber"),    Config.iLptPortNr );
  IniFile.Write(_T("UnusualIoAddress"), Config.iLptIoAddress );

  IniFile.SetPath(_T("/Config/SESSION"));
  IniFile.Write(_T("HexFileName"), Config.sz255HexFileName );

  IniFile.SetPath(_T("/Config/PROGRAMMER"));
  IniFile.Write(_T("ProgramWhat"), Config.iProgramWhat );
  IniFile.Write(_T("UseBulkErase"),Config.iUseCompleteChipErase );
  IniFile.Write(_T("Disconnect"),  Config.iDisconnectAfterProg);
  IniFile.Write(_T("VerifyDifferentVoltages"),Config.iVerifyAtDifferentVoltages);
  IniFile.Write(_T("DontCareForOsccal"),Config.iDontCareForOsccal);
  IniFile.Write(_T("DontCareForBGCalib"),Config.iDontCareForBGCalib);
  IniFile.Write(_T("ClearBufferBeforeLoading"), Config.iClearBufBeforeLoad);
  IniFile.Write(_T("NeedVddBeforeRaisingMCLR"), Config.iNeedPowerBeforeRaisingMCLR );
  IniFile.Write(_T("VerboseMessages"), Config.iVerboseMessages );

  IniFile.SetPath(_T("/Config/PIC")); // PIC-specific stuff ...
  IniFile.Write(_T("PathToDevFiles"), Config.sz255MplabDevDir );
  wxString s;
  IniFile.Write(_T("DeviceType"),     Iso8859_1_TChar(Config.sz40DeviceName, s) );
  IniFile.Write(_T("HasFlashMemory"), Config.iUnknownDevHasFlashMemory);
  IniFile.Write(_T("UnknownCodeSize"),(long)Config.dwUnknownCodeMemorySize);
  IniFile.Write(_T("UnknownDataSize"),(long)Config.dwUnknownDataMemorySize);

  Config_changed &= ~APPL_CALLER_SAVE_CFG;
}



/* EOF <config.c> */
