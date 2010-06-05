/*-------------------------------------------------------------------------*/
/*     CONFIG - module for DL4YHF's simple PIC-Programmer under Windows    */
/*-------------------------------------------------------------------------*/

#include <wx/string.h>
#include <wx/confbase.h>
#include <wx/app.h>


#include "../../Wx/Appl.h"      // call the APPLication to display message strings
#include "PIC_HW.h"   // interface types etc for default values
#include "PIC_PRG.h"

#define _I_AM_CONFIG_  /* for single-source-variables in CONFIG.H */
#include "Config.h"


/*---------------- Implementation -----------------------------------------*/
void CFG_Init(void)
{
  memset( &Config, 0, sizeof(Config) );
  Config.pic_interface_type = PIC_INTF_TYPE_COM84;
  Config.iComPortNr = 1;    // enter your favorite COM port number here ;-)
  Config.iComIoAddress = 0; // do NOT use a non-standard I/O address for the COM port
  Config.sz40DeviceName = wxT("PIC??????");
  Config.dwUnknownCodeMemorySize = 4096;  // used for PIC_DEV_TYPE_UNKNOWN..
  Config.dwUnknownDataMemorySize = 256;   // ..for a trial to program exotic types

  Config.sz255HexFileName = wxT( "test.hex" );
  Config.sz255InterfaceSupportFile = wxT( "MyProg.ini" );
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

  IniFile.SetPath(wxT("/Config/INTERFACE"));
  IniFile.Read(wxT("InterfaceType"), &Config.pic_interface_type, Config.pic_interface_type);
  IniFile.Read(wxT("SupportFile"), &s, wxEmptyString);
  Config.sz255InterfaceSupportFile =  s;
  IniFile.Read(wxT("PluginDLL"),   &s, wxEmptyString);
  Config.sz80InterfacePluginDLL  = s;;
  IniFile.Read(wxT("ExtraRdDelay_us"), &Config.iExtraRdDelay_us, Config.iExtraRdDelay_us);
  IniFile.Read(wxT("ExtraClkDelay_us"), &Config.iExtraClkDelay_us, Config.iExtraClkDelay_us);
  IniFile.Read(wxT("SlowClockPulses"), &Config.iSlowInterface, 0 );
  IniFile.Read(wxT("IdleSupplyVoltage"), &Config.iIdleSupplyVoltage, 1/*norm*/ );
//  IniFile.Read(wxT("PortAccessDriver"), &Config.iWhichPortAccessDriver, CFG_PORTACCESS_SMPORT );

//  IniFile.SetPath(wxT("/PROGRAMMING_ALGORITHM"));
//  IniFile.Read("ProgModeSequence", &PIC_dev_param.iProgModeSequence, 0); // 0=PROGMODE_VDD_THEN_VPP (usually except for DS41173b)

  IniFile.SetPath(wxT("/Config/COM84_INTERFACE"));
  IniFile.Read(wxT("ComPortNumber"), &Config.iComPortNr, Config.iComPortNr );
  IniFile.Read(wxT("UnusualIoAddress"), &Config.iComIoAddress, 0 );

  IniFile.SetPath(wxT("/Config/LPT_INTERFACE"));
  IniFile.Read(wxT("LptPortNumber"),    &Config.iLptPortNr,    Config.iLptPortNr );
  IniFile.Read(wxT("UnusualIoAddress"), &Config.iLptIoAddress, 0 );

  IniFile.SetPath(wxT("/Config/SESSION"));
  IniFile.Read(wxT("HexFileName"), &s,Config.sz255HexFileName );
  Config.sz255HexFileName = s;


  IniFile.SetPath(wxT("/Config/PROGRAMMER"));
  IniFile.Read(wxT("ProgramWhat"), &Config.iProgramWhat, PIC_PROGRAM_ALL );
  IniFile.Read(wxT("UseBulkErase"), &Config.iUseCompleteChipErase, 1 );
  IniFile.Read(wxT("Disconnect"), &Config.iDisconnectAfterProg, 1 );
  IniFile.Read(wxT("VerifyDifferentVoltages"), &Config.iVerifyAtDifferentVoltages, 1 );
  IniFile.Read(wxT("DontCareForOsccal"), &Config.iDontCareForOsccal, 0 );
  IniFile.Read(wxT("DontCareForBGCalib"), &Config.iDontCareForBGCalib, 0 );
  IniFile.Read(wxT("ClearBufferBeforeLoading"), &Config.iClearBufBeforeLoad, 0 );
  IniFile.Read(wxT("NeedVddBeforeRaisingMCLR"), &Config.iNeedPowerBeforeRaisingMCLR, 1 );

  IniFile.Read(wxT("VerboseMessages"), &Config.iVerboseMessages, 0 );

  IniFile.SetPath(wxT("/Config/PIC")); // PIC-specific stuff ...
  s.Empty();
  IniFile.Read(wxT("PathToDevFiles"), &s );
  Config.sz255MplabDevDir = s; // path to MPLAB's 'DEVICE'-folder

  // old:  Config.pic_device_type = IniFile.ReadInteger(section, "DeviceType",Config.pic_device_type );
  // Since the device TYPE NUMBERS keep changing and never had a useful meaning,
  //       a full PIC DEVICE NAME is saved as a STRING in the ini file since Nov. 2002 .
  IniFile.Read(wxT("DeviceType"), &s, wxEmptyString );
  Config.sz40DeviceName = s;
  IniFile.Read(wxT("HasFlashMemory"),  &Config.iUnknownDevHasFlashMemory, Config.iUnknownDevHasFlashMemory);
  IniFile.Read(wxT("UnknownCodeSize"), (long*)&Config.dwUnknownCodeMemorySize,   Config.dwUnknownCodeMemorySize );
  IniFile.Read(wxT("UnknownDataSize"), (long*)&Config.dwUnknownDataMemorySize, Config.dwUnknownDataMemorySize );

  Config_changed = 0x0000;
}

void CFG_Save(wxConfigBase &IniFile) /* Saves the APPLICATION'S configuration in a file */
{
  IniFile.SetPath(wxT("/Config/INTERFACE"));
  IniFile.Write(wxT("InterfaceType"),    Config.pic_interface_type);
  IniFile.Write(wxT("SupportFile"),      Config.sz255InterfaceSupportFile);
  IniFile.Write(wxT("PluginDLL"),        Config.sz80InterfacePluginDLL );
  IniFile.Write(wxT("ExtraRdDelay_us"),  Config.iExtraRdDelay_us);
  IniFile.Write(wxT("ExtraClkDelay_us"), Config.iExtraClkDelay_us);
  IniFile.Write(wxT("SlowClockPulses"),  Config.iSlowInterface );
  IniFile.Write(wxT("IdleSupplyVoltage"),Config.iIdleSupplyVoltage );
//  IniFile.Write(wxT("PortAccessDriver"), Config.iWhichPortAccessDriver );

//  IniFile.SetPath(wxT("/PROGRAMMING_ALGORITHM"));
  // IniFile.Write(wxT("ProgModeSequence"), PIC_dev_param.iProgModeSequence );

  IniFile.SetPath(wxT("/Config/COM84_INTERFACE"));
  IniFile.Write(wxT("ComPortNumber"), Config.iComPortNr );
  IniFile.Write(wxT("UnusualIoAddress"), Config.iComIoAddress );

  IniFile.SetPath(wxT("/Config/LPT_INTERFACE"));
  IniFile.Write(wxT("LptPortNumber"),    Config.iLptPortNr );
  IniFile.Write(wxT("UnusualIoAddress"), Config.iLptIoAddress );

  IniFile.SetPath(wxT("/Config/SESSION"));
  IniFile.Write(wxT("HexFileName"), Config.sz255HexFileName );

  IniFile.SetPath(wxT("/Config/PROGRAMMER"));
  IniFile.Write(wxT("ProgramWhat"), Config.iProgramWhat );
  IniFile.Write(wxT("UseBulkErase"),Config.iUseCompleteChipErase );
  IniFile.Write(wxT("Disconnect"),  Config.iDisconnectAfterProg);
  IniFile.Write(wxT("VerifyDifferentVoltages"),Config.iVerifyAtDifferentVoltages);
  IniFile.Write(wxT("DontCareForOsccal"),Config.iDontCareForOsccal);
  IniFile.Write(wxT("DontCareForBGCalib"),Config.iDontCareForBGCalib);
  IniFile.Write(wxT("ClearBufferBeforeLoading"), Config.iClearBufBeforeLoad);
  IniFile.Write(wxT("NeedVddBeforeRaisingMCLR"), Config.iNeedPowerBeforeRaisingMCLR );
  IniFile.Write(wxT("VerboseMessages"), Config.iVerboseMessages );

  IniFile.SetPath(wxT("/Config/PIC")); // PIC-specific stuff ...
  IniFile.Write(wxT("PathToDevFiles"), Config.sz255MplabDevDir );

  IniFile.Write(wxT("DeviceType"),     Config.sz40DeviceName );
  IniFile.Write(wxT("HasFlashMemory"), Config.iUnknownDevHasFlashMemory);
  IniFile.Write(wxT("UnknownCodeSize"),(long)Config.dwUnknownCodeMemorySize);
  IniFile.Write(wxT("UnknownDataSize"),(long)Config.dwUnknownDataMemorySize);

  Config_changed &= ~APPL_CALLER_SAVE_CFG;
}



/* EOF <config.c> */
