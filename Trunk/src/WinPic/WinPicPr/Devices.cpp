/*-------------------------------------------------------------------------*/
/* Devices.cpp                                                             */
/*                                                                         */
/*  Purpose:                                                               */
/*       Database of programmable PIC devices.                             */
/*       Contains everything which the programmer must know                */
/*       about a particular device.                                        */
/*   Since 2005-03-12: In addition to the built-in tables,                 */
/*       information can also be loaded from a *.dev-file,                 */
/*       which can be "borrowed" from Microchip's MPLAB IDE .              */
/*       ( Questionable if these files will remain format-compatible,      */
/*         so don' rely entirely on that. See ..\devices\readme_dev.txt )  */
/*                                                                         */
/*  Author:   Wolfgang Buescher (DL4YHF)                                   */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*                                                                         */
/*  Revision: 2005-07-01  (YYYY-MM-DD)                                     */
/*                                                                         */
/*  ToDo :  put i32CalibAddr[] + i32CalibMask[] to work !                  */
/*          ( required for some new chips where they scattered             */
/*            calibration bits all over the chip ... grrrr )               */
/*                                                                         */
/* Last changes:                                                           */
/*  2009-06-04:  Ported to wxWidget by Philippe Chevrier                   */
/*  2005-08-20:  Now using PIC_ALGO_16F7X7 to program PIC16F74 etc         */
/*               (never tested due to lack of a sample chip)               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

//#include <stdio.h>      // don't panic.. just required for sprintf !

#define _I_AM_DEVICES_
#include "devices.h"   // header for THIS module
#include "config.h"    // permanently saved Config-structure
#include <Wx/Appl.h>      // call the APPLication to display message strings
//#include "YHF_MultiLang.h" // helper routines for multi-language support ( TE(str) )
#include <WinPic/YHF_tools/QFile.h>     // W.B.'s "Quick-File" module to access text files

#include "PIC_HEX.h"   // HEX-file import, export  and buffers
#include "PIC_HW.h"   // Routines to drive the programmer hardware interface
#include "PIC_PRG.h"  // PIC programmer routines (really needed here ?)

/*----------- INTERNAL definitions -----------------------------------------*/


const wxChar C_DEVLIST_FILE_NAME[] = _T("devices.ini");

/*----------- Tables -------------------------------------------------------*/

// Combo List Items  for "single-bit-functions" in any configuration register..
const T_PicConfigBitSetting PicDev_BitCombi_OneOrZero[] =  // suited for "N" bits !
{  { _T("1"), 0xFFFFFFFF, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_OneOrZero+1 },
    { _T("0"), 0,          0xFFFFFFFF, NULL }
};

const T_PicConfigBitSetting PicDev_BitCombi_HighActiveOnOff[] =
{  { _("ON"),  0xFFFFFFFF,0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveOnOff+1 },
    { _("off"), 0,         0xFFFFFFFF, NULL }
};

const T_PicConfigBitSetting PicDev_BitCombi_LowActiveOnOff[] =
{  { _("ON"),  0,         0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_LowActiveOnOff+1 },
    { _("off"), 0xFFFFFFFF,0xFFFFFFFF, NULL }
};

const T_PicConfigBitSetting PicDev_BitCombi_HighActiveEnable[] =
{  { _("ENABLED"),  0xFFFFFFFF,0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable+1 },
    { _("disabled"), 0,         0xFFFFFFFF, NULL }
};

const T_PicConfigBitSetting PicDev_BitCombi_LowActiveEnable[] =
{  { _("ENABLED"),  0,         0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_LowActiveEnable+1 },
    { _("disabled"), 0xFFFFFFFF,0xFFFFFFFF, NULL }
};

const T_PicConfigBitSetting PicDev_BitCombi_LowActiveProtected[] =
{  { _("PROTECTED"),     0,         0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_LowActiveProtected+1 },
    { _("not protected"), 0xFFFFFFFF,0xFFFFFFFF, NULL }
};


const T_PicConfigBitSetting PicDev_BitCombi_Osc03[] =
{
    // char sz60ComboText,  dwCombiValue, dwExtraMask, *pNext
    { _("LP (low power)"),  0, 3, (P_PicConfigBitSetting)PicDev_BitCombi_Osc03+1 },
    { _("XT (crystal)"),    1, 3, (P_PicConfigBitSetting)PicDev_BitCombi_Osc03+2 },
    { _("HS (high speed)"), 2, 3, (P_PicConfigBitSetting)PicDev_BitCombi_Osc03+3 },
    { _("RC (res + cap)"),  3, 3, NULL/*end of list*/ }
};

const T_PicConfigBitSetting PicDev_BitCombi_Osc07[] = // oscillator selection bits with mask 0x0007, like PIC12F629
    // Note: There are different THREE-BIT-OSCILLATOR-CONFIGS (mask 0x0007, mask 0x0013),
    //       but the 3-bit-indices also have SLIGHTLY DIFFERENT MEANINGS:
    //              [111] = "RC-oscillator with CLKOUT" for PIC12F6xx (mask 0x0007),
    //        but   [111] = "ER occ with CLKOUT" for PIC16F628 (only an external "R", but no "C" !)
{
    // char sz60ComboText,   dwCombiValue, dwExtraMask, *pNext
    { _("LP (low power)   [000]"),     0, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc07+1 },
    { _("XT (crystal)     [001]"),     1, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc07+2 },
    { _("HS (high speed)  [010]"),     2, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc07+3 },
    { _("ExtClk via CLKIN [011]"),     3, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc07+4 },
    { _("INTOSC, no CLKOUT [100]"),    4, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc07+5 },
    { _("INTOSC, with CLKOUT [101]"),  5, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc07+6 },
    { _("RC osc, no CLKOUT [110]"),    6, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc07+7 },
    { _("RC occ, with CLKOUT [111]"),  7, 0xFFFFFFFF, NULL/*end of list*/ }
};

const T_PicConfigBitSetting PicDev_BitCombi_Osc13[] = // oscillator selection bits with mask 0x0013, like PIC16F628
{
    // char sz60ComboText,              dwCombiValue, dwExtraMask, *pNext
    { _("LP (low power)   [000]"),         0x00, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc13+1 },
    { _("XT (crystal)     [001]"),         0x01, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc13+2 },
    { _("HS (high speed)  [010]"),         0x02, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc13+3 },
    { _("ExtClk via CLKIN [011]"),         0x03, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc13+4 },
    { _("IntRC, no CLKOUT [100]"),         0x10, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc13+5 },
    { _("IntRC, with CLKOUT [101]"),       0x11, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc13+6 },
    { _("ER osc, no CLKOUT [110]"),  /*!*/ 0x12, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_Osc13+7 },
    { _("ER osc, with CLKOUT [111]"),/*!*/ 0x13, 0xFFFFFFFF,  NULL/*end of list*/ }
};

const T_PicConfigBitInfo BuiltInConfigBitInfo_PIC16F[] =
{
    // here: most parameters for PIC16F628, used as "default" in PicDev_FillConfigBitInfoTable()...
// sz60Text[44]               iFunction,     dwAddress   dwBitmask  pBitCombinations fIsBuiltIn
    {  _("Oscillator"),              CFGBIT_OSC_SEL,0x002007,   0x0000/*mask filled at runtime*/,   NULL, true  },
    {  _("Code Protection"),         CFGBIT_CP,     0x002007,   0x0000,      NULL, true  },
    {  _("Code Write Enable"),       CFGBIT_WRCODE, 0x002007,   0x0000,      NULL, true  },
    {  _("Data Protection"),         CFGBIT_CPD,    0x002007,   0x0000,      NULL, true  },
    {  _("Power-Up timer"),          CFGBIT_PWRTEN, 0x002007,   0x0000,      NULL, true  },
    {  _("Watchdog Enable"),         CFGBIT_WDTEN,  0x002007,   0x0000,      NULL, true  },
    {  _("Brown-Out Reset Enable"),  CFGBIT_BOREN,  0x002007,   0x0000,      NULL, true  },
    {  _("MCLR Input Enable"),       CFGBIT_MCLREN, 0x002007,   0x0000,      NULL, true  },
    {  _("Low Voltage Programming"), CFGBIT_LVPEN,  0x002007,   0x0000,      NULL, true  },
    {  _("Debug Enable"),            CFGBIT_DDEBUG, 0x002007,   0x0000,      NULL, true  },
    {  _("Bandgap Calibration 0"),   CFGBIT_BGCAL0, 0x002007,   0x0000,      NULL, true  },
    {  _("Bandgap Calibration 1"),   CFGBIT_BGCAL1, 0x002007,   0x0000,      NULL, true  },
    {  _("Capture/Compare MUX 0"),   CFGBIT_CCPMX0, 0x002007,   0x0000,      NULL, true  },
    {  _("Brown-Out Voltage Select"),CFGBIT_BORV,   0x002007,   0x0000,      NULL, true  },
    {  NULL,                         0,             0x000000,   0x0000,      NULL, true  }
}; // end BuiltInConfigBitInfo_PIC16F[]


//--- dsPIC30F --------------------------------------------------------------
const T_PicConfigBitSetting PicDev_BitCombi_dsPICOscSel[] = // oscillator selection bits for dsPIC30F...
{
    // char sz60ComboText,   dwCombiValue, dwExtraMask, *pNext
    // 123456789012345678901234567890
    { _("Primary Oscillator    [11]"),    3, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscSel+1 },
    { _("Internal Low-Power RC [10]"),    2, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscSel+2 },
    { _("Internal Fast RC Osc. [10]"),    1, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscSel+3 },
    { _("Low-Power 32 kHz Osc, [00]"),    0, 0xFFFFFFFF,  NULL/*end of list*/ }
};

const T_PicConfigBitSetting PicDev_BitCombi_dsPICOscCsm[] = // dsPIC30F Clock Switching Mode
{
    // char sz60ComboText,        dwCombiValue, dwExtraMask(used here!), *pNext
    // 1234567890123456789012345678901234567
    { _("Clk Switch disabled, Monitor disabled"), 2, 2, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscCsm+1 },
    { _("Clk Switch ENABLED,  Monitor disabled"), 1, 3, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscCsm+2 },
    { _("Clk Switch ENABLED,  Monitor ENABLED"),  0, 3, NULL/*end of list*/ }
};

const T_PicConfigBitSetting PicDev_BitCombi_dsPICOscPri[] = // dsPIC30F Primary Oscillator Modes
{
    // char sz60ComboText,   dwCombiValue, dwExtraMask, *pNext
    // 123456789012345678901234567890
    { _("ECIO * 16, no CLKOUT on OSC2"),         0x0F, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+1 },
    { _("ECIO * 8 , no CLKOUT on OSC2"),         0x0E, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+2 },
    { _("ECIO * 4 , no CLKOUT on OSC2"),         0x0D, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+3 },
    { _("ECIO , no CLKOUT on OSC2"),             0x0C, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+4 },
    { _("EC External Clock with CLKOUT on OSC2"),0x0B, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+5 },
    { _("FRC * 8, Internal fast osc, no CLKOUT"),0x0A, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+6 },
    { _("ERC External RC osc with CLKOUT"),      0x09, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+7 },
    { _("ERCIO External RC osc, I/O on OSC2"),   0x08, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+8 },
    { _("XT * 16, Crystal osc with PLL"),        0x07, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+9  },
    { _("XT * 8 , Crystal osc with PLL"),        0x06, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+10 },
    { _("XT * 4 , Crystal osc with PLL"),        0x05, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+11 },
    { _("XT Crystal osc, 4-10 MHz, no PLL"),     0x04, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+12 },
    { _("FRC * 16,Internal fast osc w/ PLL"),    0x03, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+13 },
    { _("HS  High speed xtal, 10-25 MHz"),       0x02, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+14 },
    { _("FRC * 4, Internal fast osc w/ PLL"),    0x01, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri+15 },
    { _("XTL Crystal Oscillator 0.2-4 MHz"),     0x00, 0xFFFFFFFF, NULL/*end of list*/ }
};

const T_PicConfigBitSetting PicDev_BitCombi_dsPICFPWRT[] = // dsPIC30F Power-on Reset Timer Value Select
{
    // char sz60ComboText, dwCombiValue, dwExtraMask, *pNext
    { _("PWRT = 64 ms  [11]"), 3, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICFPWRT+1 },
    { _("PWRT = 16 ms  [10]"), 2, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICFPWRT+2 },
    { _("PWRT = 4 ms   [01]"), 1, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICFPWRT+3 },
    { _("PWRT disabled [00]"), 0, 0xFFFFFFFF, NULL/*end of list*/ }
};


const T_PicConfigBitSetting PicDev_BitCombi_dsPICWdtPreA[] = // dsPIC30F Watchdog Prescaler A
{
    // char sz60ComboText, dwCombiValue, dwExtraMask, *pNext
    { _T("1 : 512 [11]"), 3, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreA+1 },
    { _T("1 : 64  [10]"), 2, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreA+2 },
    { _T("1 : 8   [01]"), 1, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreA+3 },
    { _T("1 : 1   [00]"), 0, 0xFFFFFFFF,  NULL/*end of list*/ }
};

const T_PicConfigBitSetting PicDev_BitCombi_dsPICWdtPreB[] = // dsPIC30F Watchdog Prescaler B
{
    // char sz60ComboText, dwCombiValue, dwExtraMask, *pNext
    { _T("1 : 16 [1111]"), 0x0F, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+1 },
    { _T("1 : 15 [1110]"), 0x0E, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+2 },
    { _T("1 : 14 [1101]"), 0x0D, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+3 },
    { _T("1 : 13 [1100]"), 0x0C, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+4 },
    { _T("1 : 12 [1011]"), 0x0B, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+5 },
    { _T("1 : 11 [1010]"), 0x0A, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+6 },
    { _T("1 : 10 [1001]"), 0x09, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+7 },
    { _T("1 : 9  [1000]"), 0x08, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+8 },
    { _T("1 : 8  [0111]"), 0x07, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+9 },
    { _T("1 : 7  [0110]"), 0x06, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+10 },
    { _T("1 : 6  [0101]"), 0x05, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+11 },
    { _T("1 : 5  [0100]"), 0x04, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+12 },
    { _T("1 : 4  [0011]"), 0x03, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+13 },
    { _T("1 : 3  [0010]"), 0x02, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+14 },
    { _T("1 : 2  [0001]"), 0x01, 0xFFFFFFFF, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB+15 },
    { _T("1 : 1  [0000]"), 0x00, 0xFFFFFFFF,  NULL/*end of list*/ }
};


const T_PicConfigBitInfo BuiltInConfigBitInfo_dsPIC30F[] =
{
    // here: a few parameters for dsPIC30Fxxxx, used as "default" in PicDev_FillConfigBitInfoTable()...
// sz60Text[44]               iFunction,     dwAddress dwBitmask  pBitCombinations fIsBuiltIn
//  1234567890123456789012345
    {  _("Oscillator source select"),CFGBIT_CUSTOM, 0xF80000, 0x0300, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscSel, true },
    {  _("Clock Switching Mode"),    CFGBIT_CUSTOM, 0xF80000, 0xC000, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscCsm, true },
    {  _("Primary Oscillator Mode"), CFGBIT_CUSTOM, 0xF80000, 0x000F, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscPri, true },
// NOT FOR dsPIC30F2010: {  "Alternate Oscillator Mode",CFGBIT_CUSTOM,0xF80000, 0x000F, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICOscAlt },
//     Note: "Primary" and "Alternate" osc mode are mapped to the same control bits !

// dsPIC30F2010 Register "FWDT" ...
    {  _("FWPSA Watchdog prescaler A"), CFGBIT_CUSTOM, 0xF80002, 0x0030, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreA, true },
    {  _("FWPSB Watchdog prescaler B"), CFGBIT_CUSTOM, 0xF80002, 0x000F, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICWdtPreB, true },
    {  _("FWDTEN Watchdog Enable")     ,CFGBIT_CUSTOM, 0xF80002, 0x0003, (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable, true },

// dsPIC30F2010 Register "FBORPOR" ...
    {  _("MCLREN Master Clear Enable") ,CFGBIT_CUSTOM, 0xF80004, 0x8000, (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable, true },
    {  _("PWMPIN Motor PWM Pin Mode"),  CFGBIT_CUSTOM, 0xF80004, 0x0400, (P_PicConfigBitSetting)PicDev_BitCombi_OneOrZero, true },
    {  _("HPOL PWM High Side Polarity"),CFGBIT_CUSTOM, 0xF80004, 0x0200, (P_PicConfigBitSetting)PicDev_BitCombi_OneOrZero, true },
    {  _("LPOL PWM Low Side Polarity"), CFGBIT_CUSTOM, 0xF80004, 0x0100, (P_PicConfigBitSetting)PicDev_BitCombi_OneOrZero, true },
    {  _("BOREN Brown-out Reset Enable"),CFGBIT_CUSTOM, 0xF80004,0x0080, (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable, true },
    {  _("BORV Brown-out Voltage Select"),CFGBIT_CUSTOM,0xF80004,0x0030, (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable, true },
    {  _("FPWRT Power-on Reset Timer"), CFGBIT_CUSTOM,0xF80004,  0x0003, (P_PicConfigBitSetting)PicDev_BitCombi_dsPICFPWRT, true },

// dsPIC30Fxxxx Register "FGS" ...
    {  _("GCP General Code Protect"), CFGBIT_CUSTOM, 0xF8000A,  0x0002, (P_PicConfigBitSetting)PicDev_BitCombi_LowActiveProtected, true },
    {  _("GWRP Code Write Protect"),  CFGBIT_CUSTOM, 0xF8000A,  0x0001, (P_PicConfigBitSetting)PicDev_BitCombi_LowActiveProtected, true },

// dsPIC30Fxxxx Register "FICD" ...
    {  _("BKBUG Debugger/Emulator Enable"),CFGBIT_CUSTOM,0xF8000C,0x8000, (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable, true },
    {  _("COE Clip-on Emulation Mode"),  CFGBIT_CUSTOM, 0xF8000C, 0x4000, (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable, true },
    {  _("ICS ICD Comm Channel Select"), CFGBIT_CUSTOM, 0xF8000C, 0x0003, (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable, true },

    {  NULL,                      0,             0x000000, 0x0000,  NULL, true  }
}; // end BuiltInConfigBitInfo_dsPIC30F[]



const T_PicDeviceInfo BuiltInPicDeviceInfo[] =
{

// sz40DeviceName, iCodeMemType, iBitsPerInstruction,  code mem size, data EEPROM size, internal RAM size,
    { "PIC10F200",     PIC_MT_FLASH, 12,                   256/*WORDS*/,  0,                16 ,
        // sz80ConfigRegisterInfoFile[] :
        _T("PIC10F200.dev"),
        // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
        1, 1,
        // lConfMemBase,lConfWordAdr,
        0x0FFF,      0x0FFF,   // note: this is the address of the CONFIG WORD in the HEX FILE,
        // but in the TARGET(!) for a PIC10F20x, the config word is located at 0x03FF  !
        // Even Microchip's PIC10F206.DEV file contains address 0x0FFF instead of 0x03FF !
        // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
        0x0001 , 0x0000,       // only ONE Config Word here !
        // lDataMemBase,lAddressOscCal,
        -1,          0x00FF,   // note: there is another OSCCAL BACKUP VALUE at addr 0x104 for 10F200/10F204 !
        // lIdMemBase,  lIdMemSize,
        0x0100,       5,  // FIVE ID locations to cover the PIC10F20x's "Backup OSCCAL value" too
        // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
        -1,           0,              0,  // note: there is no DEVICE ID in this stupid piece of silicon !
        // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
        {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
        // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
        {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
        // wCanRead:  1 = can read while programming,   0 = can NOT read in between
        1,
        // wEraseAlgo,      wCodeProgAlgo,   wConfigProgAlgo,
        PIC_ALGO_PIC10F, PIC_ALGO_PIC10F, PIC_ALGO_PIC10F,
        // wDataProgAlgo,          wVppVddSequence,
        PIC_ALGO_PIC10F,        PROGMODE_VDD_THEN_VPP,
        // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
        10,          10000,       10000,
        // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
        -1,              -1,      -1,       -1,      -1,       -1,     -1,       -1,     -1,       -1,
        // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices, -1=not supported) :
        -1,             -1,
        // wCfgmask_..unused,unknown,cpbits,  pwrte,inv_pwrte, wdte,   :
        0x0E00,           0x0000, 0x0080,  0x0000,0x0000,   0x0004,
        // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
        0x0000,   0x0000, 0x0000,0x0000,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
        // wCfgmask_..cpd,lvp,  boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
        0x0000, 0x0000,0x0000, 0x0010, 0x0000, 0x0000, 0x0000,  0x0000,
        // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
        0x0000,
        // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
        0x0000,

        73 // iCheck73 (to check for missing table entries)
    }, // end PIC10F200

// sz40DeviceName, iCodeMemType, iBitsPerInstruction,  code mem size, data EEPROM size, internal RAM size,
    { "PIC10F206",     PIC_MT_FLASH, 12,                   512,           0,                24 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC10F206.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x0FFF,      0x0FFF,   // note: this is the address of the CONFIG WORD in the HEX FILE,
      // but in the TARGET(!) for a PIC10F20x, the config word is located at 0x03FF  !
      // Even Microchip's PIC10F206.DEV file contains address 0x0FFF instead of 0x03FF !
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x0001 , 0x0000,       // only ONE Config Word here !
      // lDataMemBase,lAddressOscCal,
      -1,          0x01FF,   // note: there is another OSCCAL BACKUP VALUE at addr 0x204 (0x104 for 10F200/10F204)
      // lIdMemBase,  lIdMemSize,
      0x0200,       5,  // FIVE ID locations to cover the PIC10F20x's "Backup OSCCAL value" too
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      -1,           0,              0,  // note: there is no DEVICE ID in this stupid piece of silicon !
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      1,
      // wEraseAlgo,      wCodeProgAlgo,   wConfigProgAlgo,
      PIC_ALGO_PIC10F, PIC_ALGO_PIC10F, PIC_ALGO_PIC10F,
      // wDataProgAlgo,          wVppVddSequence,
      PIC_ALGO_PIC10F,        PROGMODE_VDD_THEN_VPP,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,          10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      -1,              -1,      -1,       -1,      -1,       -1,     -1,       -1,     -1,       -1,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices, -1=not supported) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits,  pwrte,inv_pwrte, wdte,   :
      0x0E00,           0x0000, 0x0080,  0x0000,0x0000,   0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0000,   0x0000, 0x0000,0x0000,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
      // wCfgmask_..cpd,lvp,  boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000, 0x0000,0x0000, 0x0010, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC10F206


// sz40DeviceName, iCodeMemType, iBitsPerInstruction,  code mem size, data EEPROM size, internal RAM size,
    { "PIC12F629",     PIC_MT_FLASH, 14,                   1024,           128,             64 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC12F629.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       0x03FF,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0F80,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_12FXX, PIC_ALGO_12FXX,PIC_ALGO_12FXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,          10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices, -1=not supported) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits,  pwrte,inv_pwrte, wdte,   :
      0x0E00,           0x0000, 0x0080,  0x0010,0x0010,   0x0008,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0007,   0x0007, 0x0002,0x0001,0x0000, 0x0003, 0x0004,       0x0005,      0x0006,
      // wCfgmask_..cpd,lvp,  boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100, 0x0000,0x0040, 0x0020, 0x0000, 0x0000, 0x3000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC12F629

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC12F675",     PIC_MT_FLASH, 14,                  1024,           128,             64 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC12F675.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       0x03FF,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0FC0,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_12FXX, PIC_ALGO_12FXX,PIC_ALGO_12FXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,          10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices, -1=not supported) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits,  pwrte,inv_pwrte, wdte,   :
      0x0E00,           0x0000, 0x0080,  0x0010,0x0010,   0x0008,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0007,   0x0007, 0x0002,0x0001,0x0000, 0x0003, 0x0004,       0x0005,      0x0006,
      // wCfgmask_..cpd,lvp,  boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100, 0x0000,0x0040, 0x0020, 0x0000, 0x0000, 0x3000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC12F675

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16C61",      PIC_MT_EPROM, 14,                  1024,           0,               -1 ,
      // sz80ConfigRegisterInfoFile[] :
      _T(""),      // no longer supported by MPLAB ?
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0060/*found by experiment!*/ ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_16CXX, PIC_ALGO_16CXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VDD_THEN_VPP, // here for 16C61
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,          10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x3FE0,           0x0000, 0x0010, 0x0008,0x0000,   0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,   0x0003, 0x0002,0x0001,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
      // wCfgmask_..cpd,lvp, boden, mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000,   0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16C61

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16C71", PIC_MT_EPROM,      14,                  1024,             0,         -1 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16C71.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0000 /*0=unknown*/ ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_16CXX, PIC_ALGO_16CXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VDD_THEN_VPP, // here for 16C71
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,          10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices, -1=not supported) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x3FE0,           0x0000, 0x0010, 0x0008,0x0000,   0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,   0x0003, 0x0002,0x0001,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
      // wCfgmask_..cpd,lvp, boden, mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000,   0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16C71


// sz40DeviceName, iCodeMemType, iBitsPerInstruction,  code mem size, data EEPROM size, internal RAM size,
    { "PIC16C84", PIC_MT_FLASH,      14,                   1024,            64,         -1 ,
      // sz80ConfigRegisterInfoFile[] :
      _T(""),    // no dev-file for this chip !
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0000 /*0=unknown*/ ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      1,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_16FXX, PIC_ALGO_16FXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VDD_THEN_VPP, // here for 16C84 (stoneage)
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte,      wdte,   :
      0x3FE0,           0x0000, 0x0010, 0x0008,0x0000/*!*/,   0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,   0x0003, 0x0002,0x0001,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
      // wCfgmask_..cpd,lvp, boden, mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000,   0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16C84


// sz40DeviceName, iCodeMemType, iBitsPerInstruction,  code mem size, data EEPROM size, internal RAM size,
    { "PIC16C710",     PIC_MT_EPROM, 14,                   512,           0,                64 ,
      // sz80ConfigRegisterInfoFile[] :
      _T(""),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0000 /*0=unknown*/ ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_16CXX, PIC_ALGO_16CXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_VDD_UNKNOWN,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices, -1=not supported) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0200,           0x0000, 0x3FB0, 0x0008,0x0008,   0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,   0x0003, 0x0002,0x0001,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
      // wCfgmask_..cpd,lvp, boden, mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000,   0x0000,  0x0040, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16C710

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16C711",     PIC_MT_EPROM, 14,                  1024,          0,                64 ,
      // sz80ConfigRegisterInfoFile[] :
      _T(""),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0000 /*0=unknown*/ ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_16CXX, PIC_ALGO_16CXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_VDD_UNKNOWN,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices, -1=not supported) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0200,           0x0000, 0x3FB0, 0x0008,0x0008,   0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,   0x0003, 0x0002,0x0001,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
      // wCfgmask_..cpd,lvp, boden, mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000,   0x0000,  0x0040, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16C710

#if(0)  //  16F715 DOES NOT WORK, see note from FENG3:
// > PIC16C715 has 16-bit core.
// > WinPic can read, but cannot program this device.
// > Moreover, because config bit shifts, the code
// > protection is set as for PIC16C715 that fails
// > in programming.
// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16C715",     PIC_MT_EPROM, 16/*!*/,             2048,          0,                128 ,
      // sz80ConfigRegisterInfoFile[] :
      _T(""),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0000 /*0=unknown*/ ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_16CXX, PIC_ALGO_16CXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_VDD_UNKNOWN,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices, -1=not supported) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0200,           0x0000, 0x3F30, 0x0008,0x0008,   0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,   0x0003, 0x0002,0x0001,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
      // wCfgmask_..cpd,lvp, boden, mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000,   0x0000,  0x0040, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16C715
#endif // (0),  16F715 not supported

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F73",      PIC_MT_FLASH, 14,                  4096,             0 ,            192 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F73.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0600  ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      1,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_PIC16F7X, PIC_ALGO_PIC16F7X, PIC_ALGO_PIC16F7X,  // new since 2005-08-21 !
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_UNKNOWN/*no EEPROM*/,     PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us,     lTi_Erase_us :
      10,         1000 /*1ms!*/,   30000 /* 30ms!*/,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x3FE0,          0x0000, 0x0010, 0x0008,0x0000,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,   0x0003, 0x0002,0x0001,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
      // wCfgmask_..cpd,lvp, boden, mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000,   0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F73


// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F737",     PIC_MT_FLASH, 14,                  4096,             0 ,            368 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F737.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0BA0  ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,      wCodeProgAlgo,   wConfigProgAlgo,
      PIC_ALGO_16F7X7, PIC_ALGO_16F7X7, PIC_ALGO_16F7X7,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_UNKNOWN/*no EEPROM!*/,    PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us,       lTi_Erase_us :
      10,         1000,/*yes,1ms!*/   30000/*yes,30ms!*/,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   -1  ,    -1  ,    0x09,     -1  ,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0900,          0x0800, 0x2000, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0013,    0x0013,0x0002,0x0001,0x0000,0x0003,    0x0010,      0x0011,     0x0012,
      // wCfgmask_..cpd,lvp, boden(later: "BOREN") mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000,   0x0000,  0x0040,                0x0020, 0x0000, 0x0000, 0x0000,  0x1000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0180,
      // wCfgmask2_used (introduced for PIC16F88, also exists in the 16F7x7 ) :
      0x0043,           // in 16F7x7: bit6="BORSEN", bit1="IESO", bit0="FCMEN" .

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F737


// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F74",      PIC_MT_FLASH, 14,                  4096,          0 ,               192 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F74.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0620 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      1,
      // wEraseAlgo,     wCodeProgAlgo,  wConfigProgAlgo,
      PIC_ALGO_PIC16F7X, PIC_ALGO_PIC16F7X, PIC_ALGO_PIC16F7X,  // new since 2005-08-21 !
      // ex (before 2005-08-20):  PIC_ALGO_16FXX, PIC_ALGO_16FXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                  wVppVddSequence,
      PIC_ALGO_UNKNOWN/*no EEPROM*/,  PROGMODE_VDD_THEN_VPP, // Vdd before Vpp reported by Stefan Toftevall
      // lTi_Clk_us, lTi_Prog_us,     lTi_Erase_us :
      10,         1000 /*1ms!*/,   30000 /* 30ms!*/,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x3FE0,          0x0000, 0x0010, 0x0008,0x0000,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,   0x0003, 0x0002,0x0001,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
      // wCfgmask_..cpd,lvp, boden, mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000,   0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F74

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F76",      PIC_MT_FLASH, 14,                  8192,             0 ,            368 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F76.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0640 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      1,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_PIC16F7X, PIC_ALGO_PIC16F7X, PIC_ALGO_PIC16F7X,  // new since 2005-08-21 !
      // ex (before 2005-08-20):  PIC_ALGO_16FXX, PIC_ALGO_16FXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us,     lTi_Erase_us :
      10,         1000 /*1ms!*/,   30000 /* 30ms!*/,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x3FE0,          0x0000, 0x0010, 0x0008,0x0000,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,   0x0003, 0x0002,0x0001,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
      // wCfgmask_..cpd,lvp, boden, mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000,   0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F76

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F77",      PIC_MT_FLASH, 14,                  8192,           0 ,              368 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F77.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0660 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      1,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_PIC16F7X, PIC_ALGO_PIC16F7X, PIC_ALGO_PIC16F7X,  // new since 2005-08-21 !
      // ex (before 2005-08-20):  PIC_ALGO_16FXX, PIC_ALGO_16FXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us,     lTi_Erase_us :
      10,         1000 /*1ms!*/,   30000 /* 30ms!*/,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x3FE0,          0x0000, 0x0010, 0x0008,0x0000,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,   0x0003, 0x0002,0x0001,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
      // wCfgmask_..cpd,lvp, boden, mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000,   0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F77

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F84", PIC_MT_FLASH,      14,                  1024,          64,               64,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F84.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0000 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      1,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_16FXX, PIC_ALGO_16FXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits,  pwrte,inv_pwrte,      wdte,   :
      0x0000,           0x0000, 0x3FF0,  0x0008,0x0008/*!*/,   0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,   0x0003, 0x0002,0x0001,0x0000, 0x0000, 0x0000,       0x0000,      0x0000,
      // wCfgmask_..cpd,lvp, boden, mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0000,   0x0000,  0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F84

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F627",     PIC_MT_FLASH, 14,                  1024,          128,              224 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F627.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x07A0,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      1,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_16FXX, PIC_ALGO_16FXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0200,          0x0000, 0x3C00, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0013,    0x0013,0x0002,0x0001,0x0000,0x0003,    0x0010,      0x0011,     0x0012,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100,    0x0080, 0x0040, 0x0020, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F627

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F628",     PIC_MT_FLASH, 14,                  2048,           128,             224 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F628.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x07C0 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      1,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_16FXX, PIC_ALGO_16FXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,      0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,        -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0200,          0x0000, 0x3C00, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0013,    0x0013,0x0002,0x0001,0x0000,0x0003,    0x0010,      0x0011,     0x0012,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100,    0x0080, 0x0040, 0x0020, 0x0000, 0x0000, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F628

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F630",     PIC_MT_FLASH, 14,                  1024,           128,             64 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F630.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       0x03FF,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x07C0 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      1,
      // wEraseAlgo,      wCodeProgAlgo,   wConfigProgAlgo,
      PIC_ALGO_16F630, PIC_ALGO_16F630, PIC_ALGO_16F630,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us,                    lTi_Erase_us :
      10,         6000/*int'l timed, EEPROM!*/,   6000,
      // Grrrr: DATA- and CODE- programming times are different now !
      //        Tprog1=max 2.5ms for CODE, 6ms for DATA memory. Fixed time in CODE PROGGING STUFF !
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,  LoadData,ReadData,EraseProg,EraseData,
      0x00,      0x02,    0x04,    0x06,    0x08,   0x0A/*!*/,    0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (there is no such thing for the 16F630) :
      -1,        -1,
      // wCfgmask_..unused,unknown, cpbits, pwrte,  inv_pwrte, wdte,   :
      0x800+0x400+0x200, 0x0000, 0x0080, 0x0010, 0x0010,    0x0008,
      // wCfgmask_..osc,.rc, hs,     xt,    lp,  extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0007,    0x0007,0x0002,0x0001,0x0000,0x0003,    0x0004,      0x0005,     0x0006,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100,    0x0000, 0x0040, 0x0020, 0x0000, 0x0000, 0x3000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88, unused in 16F630)..
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F630


// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F818",     PIC_MT_FLASH, 14,                  1024,          128 ,             128 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F818.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FF0,        0x04C0,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,   wConfigProgAlgo,
      PIC_ALGO_ERASE_16F81X, PIC_ALGO_16F81X, PIC_ALGO_CONFIG_MEM_16F81X,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_16F81X,                   PROGMODE_VDD_THEN_VPP, // 16F818, see "long story" in PIC_HW.cpp !
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         2000,       8000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr, BeginProg,  EndProg,
      0x00,            0x02,    0x04,    0x06,     0x18/*!*/,  0x17/*!*/,
      // LoadData, ReadData, EraseProg, EraseData,
      0x03,    0x05,     0x09,      0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      0x1F,              0x18,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0000,  0x0000, 0x2000, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc, .rc,  hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0013, 0x0013,0x0002,0x0001,0x0000,0x0003,    0x0010,      0x0011,     0x0012,
      // wCfgmask_..cpd, lvp,  boden/"boren"(boring!),  mclre,  ddebug, wrcode, bandgap, CCPMX,
      0x0100, 0x0080, 0x0040,                0x0020, 0x0800, 0x0600, 0x0000,  0x1000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F818

// sz40DeviceName, iCodeMemType,  iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F819",     PIC_MT_FLASH,  14,                  2048,          256 ,             128 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F819.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FF0,        0x04E0,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,   wConfigProgAlgo,
      PIC_ALGO_ERASE_16F81X, PIC_ALGO_16F81X, PIC_ALGO_CONFIG_MEM_16F81X,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_16F81X,                   PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         2000,       8000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr, BeginProg,  EndProg,
      0x00,            0x02,    0x04,    0x06,     0x18/*!*/,  0x17/*!*/,
      // LoadData, ReadData, EraseProg, EraseData,
      0x03,    0x05,     0x09,      0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      0x1F,              0x18,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0000,  0x0000, 0x2000, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc, .rc,  hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0013, 0x0013,0x0002,0x0001,0x0000,0x0003,    0x0010,      0x0011,     0x0012,
      // wCfgmask_..cpd, lvp,  boden/"boren"(boring!),  mclre,  ddebug, wrcode, bandgap, CCPMX,
      0x0100, 0x0080, 0x0040,                0x0020, 0x0800, 0x0600, 0x0000,  0x1000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F819

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F870", PIC_MT_FLASH,     14,                  2048,            64 ,        -1  ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F870.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0D00,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,    wConfigProgAlgo,
      PIC_ALGO_ERASE_16F87X, PIC_ALGO_16F87X,  PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0400,          0x0000, 0x3030, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,    0x0003,0x0002,0x0001,0x0000,0x0000,    0x0000,      0x0000,     0x0000,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100,    0x0080, 0x0040, 0x0000, 0x0800, 0x0200, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F870

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F871", PIC_MT_FLASH,     14,                  2048,            64 ,        -1  ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F871.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0D20 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,    wConfigProgAlgo,
      PIC_ALGO_ERASE_16F87X, PIC_ALGO_16F87X,  PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0400,          0x0000, 0x3030, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,    0x0003,0x0002,0x0001,0x0000,0x0000,    0x0000,      0x0000,     0x0000,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100,    0x0080, 0x0040, 0x0000, 0x0800, 0x0200, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F871

// sz40DeviceName, iCodeMemType, iBitsPerInstruction,  code mem size, data EEPROM size, internal RAM size,
    { "PIC16F872",     PIC_MT_FLASH, 14                 ,  2048,            64 ,        -1 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F872.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x08E0 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,    wConfigProgAlgo,
      PIC_ALGO_ERASE_16F87X, PIC_ALGO_16F87X,  PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0400,          0x0000, 0x3030, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,    0x0003,0x0002,0x0001,0x0000,0x0000,    0x0000,      0x0000,     0x0000,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100,    0x0080, 0x0040, 0x0000, 0x0800, 0x0200, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F872

// sz40DeviceName, iCodeMemType, iBitsPerInstruction,  code mem size, data EEPROM size, internal RAM size,
    { "PIC16F873", PIC_MT_FLASH,     14,                   4096,            128,        -1 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F873.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0960 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,    wConfigProgAlgo,
      PIC_ALGO_ERASE_16F87X, PIC_ALGO_16F87X,  PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0400,          0x0000, 0x3030, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,    0x0003,0x0002,0x0001,0x0000,0x0000,    0x0000,      0x0000,     0x0000,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100,    0x0080, 0x0040, 0x0000, 0x0800, 0x0200, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F873

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F874", PIC_MT_FLASH,     14,                  4096,            128,        -1 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F874.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0920 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,    wConfigProgAlgo,
      PIC_ALGO_ERASE_16F87X, PIC_ALGO_16F87X,  PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0400,          0x0000, 0x3030, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,    0x0003,0x0002,0x0001,0x0000,0x0000,    0x0000,      0x0000,     0x0000,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100,    0x0080, 0x0040, 0x0000, 0x0800, 0x0200, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F874 (nobody knows what happened to the 16F875 ;o)

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F876",     PIC_MT_FLASH, 14,                  8192,            256,        -1 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F876.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x09E0 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,    wConfigProgAlgo,
      PIC_ALGO_ERASE_16F87X, PIC_ALGO_16F87X,  PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0400,          0x0000, 0x3030, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,    0x0003,0x0002,0x0001,0x0000,0x0000,    0x0000,      0x0000,     0x0000,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100,    0x0080, 0x0040, 0x0000, 0x0800, 0x0200, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F876

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC16F877", PIC_MT_FLASH,     14,                  8192,            256,        -1 ,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC16F877.dev"),
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 1,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,  lIdMemSize,
      0x2000,       4,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x09A0 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,    wConfigProgAlgo,
      PIC_ALGO_ERASE_16F87X, PIC_ALGO_16F87X,  PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_THEN_VDD,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,          10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0400,          0x0000, 0x3030, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,    0x0003,0x0002,0x0001,0x0000,0x0000,    0x0000,      0x0000,     0x0000,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100,    0x0080, 0x0040, 0x0000, 0x0800, 0x0200, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used  :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC16F877

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC18F252",     PIC_MT_FLASH, 16/*!*/,             16384/*instr!*/,  256/*byte!*/,  1536/*byte!*/,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC18F252.dev"),  // <<< file must be "borrowed" from MLPAB IDE/device !
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      8, 64,  // 8 *BYTES* for WRITING,  64 *BYTES* for ERASING !
      // lConfMemBase,lConfWordAdr,
      0x300000,     0x300000/*no "classic" config word*/,
      // dwConfMemUsedLocations (first 32 address locations !!)
      0x0000FFFE,  /* 0x30000D...0x300001 = 7 and a half config WORDS(!) in PIC18F252*/
      // dwConfMemUsedLocations (next 32, all of them unused in a PIC18F252 )
      0x00000000,
      // lDataMemBase, lAddressOscCal,
      0xF00000L,    -1,  // Note: 0xF00000 is EEPROM-"pseudo"-address in PIC18F, but DEVICE ID in dsPIC30!
      // lIdMemBase,      lIdMemSize,
      0x2000000/*!*/,  8,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue (here: DEVID1 + DEVID2 combined in one WORD)
      0x3FFFFEL,     0x0FFE0/*strip lower 5 bits*/,   0x0400 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,     wConfigProgAlgo,
      PIC_ALGO_PIC18F,       PIC_ALGO_PIC18F,   PIC_ALGO_PIC18F,
      // wDataProgAlgo,         wVppVddSequence,
      PIC_ALGO_PIC18F,       PROGMODE_VDD_THEN_VPP /*?*/ ,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      1,          10000,     10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      //  (all these are "don't care" for PIC18F, which use TOTALLY DIFFERENT algoritms, see prog spec.. )
      -1,             -1,      -1,      -1,       -1,      -1,      -1,     -1,      -1,       -1,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,
      0x0000,          0,      0,       0,    0,        0,     // <<< old stuff, not applicable for PIC18F etc
      // (all DON'T CARE for dsPIC's because they have SEVEN(!) config words)
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,  intrc_noclkout,intrc_clkout,extrc_noclkout
      0,         0,    0,    0,    0,          0,           0,      0,          0,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0,         0,    0,         0,       0,      0,       0,  0,
      // wCfgmask_..borv (for PICs with selectable BROWN-OUT DETECTION VOLTAGE) :
      0,
      // wCfgmask2_used :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC18F252


// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC18F2550",    PIC_MT_FLASH, 16/*!*/,             16384/*instr!*/,  256/*byte!*/,  2048/*byte!*/,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC18F2550.dev"),  // <<< file must be "borrowed" from MLPAB IDE/device !
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      32, 64,  // 32 *BYTES* for WRITING,  64 *BYTES* for ERASING (PIC18F2420, 2520, 4420, 4520, 2455, 2550, 4455, 4550, ..??)
      // lConfMemBase,lConfWordAdr,
      0x300000,     0x300000,
      // dwConfMemUsedLocations (first 32 WORDS!!) , dwConfMemUsedLocations (next 32)
      0x007F/*only 7 config WORDS in PIC18F*/   , 0x0000,
      // lDataMemBase, lAddressOscCal,
      0xF00000L,    -1,  // Note: 0xF00000 is EEPROM-"pseudo"-address in PIC18F, but DEVICE ID in dsPIC30!
      // lIdMemBase,      lIdMemSize,
      0x2000000/*!*/,  8,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue (here: DEVID1 + DEVID2 combined in one WORD)
      0x3FFFFEL,     0x0FFE0/*strip lower 5 bits*/,   0x1240 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,     wConfigProgAlgo,
      PIC_ALGO_PIC18F,       PIC_ALGO_PIC18F,   PIC_ALGO_PIC18F,
      // wDataProgAlgo,         wVppVddSequence,
      PIC_ALGO_PIC18F,       PROGMODE_VDD_THEN_VPP /*?*/ ,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      1,          10000,     10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      //  (all these are "don't care" for PIC18F, which use TOTALLY DIFFERENT algoritms, see prog spec.. )
      -1,             -1,      -1,      -1,       -1,      -1,      -1,     -1,      -1,       -1,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,
      0x0000,          0,      0,       0,    0,        0,     // <<< old stuff, not applicable for PIC18F etc
      // (all DON'T CARE for dsPIC's because they have SEVEN(!) config words)
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,  intrc_noclkout,intrc_clkout,extrc_noclkout
      0,         0,    0,    0,    0,          0,           0,      0,          0,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0,         0,    0,         0,       0,      0,       0,  0,
      // wCfgmask_..borv (for PICs with selectable BROWN-OUT DETECTION VOLTAGE) :
      0,
      // wCfgmask2_used :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC18F2550

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC18F4455",    PIC_MT_FLASH, 16/*!*/,             12288/*instr!*/,  256/*byte!*/,  2048/*byte!*/,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC18F4455.dev"),  // <<< file must be "borrowed" from MLPAB IDE/device !
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      32, 64,  // 32 *BYTES* for WRITING,  64 *BYTES* for ERASING (PIC18F2420, 2520, 4420, 4520, 2455, 2550, 4455, 4550, ..??)
      // lConfMemBase,lConfWordAdr,
      0x300000,     0x300000,
      // dwConfMemUsedLocations (first 32 WORDS!!) , dwConfMemUsedLocations (next 32)
      0x007F/*only 7 config WORDS in PIC18F*/   , 0x0000,
      // lDataMemBase, lAddressOscCal,
      0xF00000L,    -1,  // Note: 0xF00000 is EEPROM-"pseudo"-address in PIC18F, but DEVICE ID in dsPIC30!
      // lIdMemBase,      lIdMemSize,
      0x2000000/*!*/,  8,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue (here: DEVID1 + DEVID2 combined in one WORD)
      0x3FFFFEL,     0x0FFE0/*strip lower 5 bits*/,   0x1240 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,     wConfigProgAlgo,
      PIC_ALGO_PIC18F,       PIC_ALGO_PIC18F,   PIC_ALGO_PIC18F,
      // wDataProgAlgo,         wVppVddSequence,
      PIC_ALGO_PIC18F,       PROGMODE_VDD_THEN_VPP /*?*/ ,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      1,          10000,     10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      //  (all these are "don't care" for PIC18F, which use TOTALLY DIFFERENT algoritms, see prog spec.. )
      -1,             -1,      -1,      -1,       -1,      -1,      -1,     -1,      -1,       -1,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,
      0x0000,          0,      0,       0,    0,        0,     // <<< old stuff, not applicable for PIC18F etc
      // (all DON'T CARE for dsPIC's because they have SEVEN(!) config words)
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,  intrc_noclkout,intrc_clkout,extrc_noclkout
      0,         0,    0,    0,    0,          0,           0,      0,          0,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0,         0,    0,         0,       0,      0,       0,  0,
      // wCfgmask_..borv (for PICs with selectable BROWN-OUT DETECTION VOLTAGE) :
      0,
      // wCfgmask2_used :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC18F4455

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "PIC18F4550",    PIC_MT_FLASH, 16/*!*/,             16384/*instr!*/,  256/*byte!*/,  2048/*byte!*/,
      // sz80ConfigRegisterInfoFile[] :
      _T("PIC18F4550.dev"),  // <<< file must be "borrowed" from MLPAB IDE/device !
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      32, 64,  // 32 *BYTES* for WRITING,  64 *BYTES* for ERASING (PIC18F2420, 2520, 4420, 4520, 2455, 2550, 4455, 4550, ..??)
      // lConfMemBase,lConfWordAdr,
      0x300000,     0x300000,
      // dwConfMemUsedLocations (first 32 WORDS!!) , dwConfMemUsedLocations (next 32)
      0x007F/*only 7 config WORDS in PIC18F*/   , 0x0000,
      // lDataMemBase, lAddressOscCal,
      0xF00000L,    -1,  // Note: 0xF00000 is EEPROM-"pseudo"-address in PIC18F, but DEVICE ID in dsPIC30!
      // lIdMemBase,      lIdMemSize,
      0x2000000/*!*/,  8,
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue (here: DEVID1 + DEVID2 combined in one WORD)
      0x3FFFFEL,     0x0FFE0/*strip lower 5 bits*/,   0x1240 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,     wConfigProgAlgo,
      PIC_ALGO_PIC18F,       PIC_ALGO_PIC18F,   PIC_ALGO_PIC18F,
      // wDataProgAlgo,         wVppVddSequence,
      PIC_ALGO_PIC18F,       PROGMODE_VDD_THEN_VPP /*?*/ ,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      1,          10000,     10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      //  (all these are "don't care" for PIC18F, which use TOTALLY DIFFERENT algoritms, see prog spec.. )
      -1,             -1,      -1,      -1,       -1,      -1,      -1,     -1,      -1,       -1,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,
      0x0000,          0,      0,       0,    0,        0,     // <<< old stuff, not applicable for PIC18F etc
      // (all DON'T CARE for dsPIC's because they have SEVEN(!) config words)
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,  intrc_noclkout,intrc_clkout,extrc_noclkout
      0,         0,    0,    0,    0,          0,           0,      0,          0,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0,         0,    0,         0,       0,      0,       0,  0,
      // wCfgmask_..borv (for PICs with selectable BROWN-OUT DETECTION VOLTAGE) :
      0,
      // wCfgmask2_used :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end PIC18F4550


// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "dsPIC30F2010",  PIC_MT_FLASH, 24/*!*/,             4096/*instr!*/, 1024,            512,
      // sz80ConfigRegisterInfoFile[] :
      _T("dsPIC30F2010.dev"), // <<< file must be "borrowed" from MLPAB IDE/device !
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      32, 0,  // dsPIC: 32 instruction words (!!!!!!!!!) = 32*3 bytes for dsPIC30F
      // lConfMemBase,lConfWordAdr,
      0xF80000,    0xF80000/*caution, no "classic" config word in a dsPIC*/,
      // dwConfMemUsedLocations (first 32)                      , dwConfMemUsedLocations (next 32)
      0x0027/*only 4 of 8  dsPIC config regs used in -2010*/ , 0x0000/*none*/,
      // lDataMemBase,                        lAddressOscCal,
      0x7FFC00/*not 0x7FF000L ! */,        -1/*none*/,
      // Note on dsPIC's "data memory base address" :
      //  IT VARIES WITH THE SIZE OF THE EEPROM ! ! ! !
      //  Instead the END address is *always* (?) 0x7FFFFE,
      //  regardless of the EEPROM size .  Ummm...
      // lIdMemBase,      lIdMemSize,
      0xFF00000/*!*/,  0,       // NO "ID LOCATIONS" IN dsPIC30F ?!
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0xFF0000L,     0xFFFF,        (0x01<<6)+0x00 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,     wConfigProgAlgo,
      PIC_ALGO_dsPIC30F,     PIC_ALGO_dsPIC30F, PIC_ALGO_dsPIC30F,
      // wDataProgAlgo,         wVppVddSequence,
      PIC_ALGO_dsPIC30F,     PROGMODE_VDD_THEN_VPP,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      1,          10000,     10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      //  (all these are "don't care" for dsPICs, which use TOTALLY DIFFERENT algoritms, see prog spec.. )
      -1,             -1,      -1,      -1,       -1,      -1,      -1,     -1,      -1,       -1,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,
      0x0000,          0,      0,       0,    0,        0,     // <<< old stuff, not applicable for dsPIC etc
      // (all DON'T CARE for dsPIC's because they have SEVEN(!) config words)
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,  intrc_noclkout,intrc_clkout,extrc_noclkout
      0,         0,    0,    0,    0,          0,           0,      0,          0,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0,         0,    0,         0,       0,      0,       0,  0,
      // wCfgmask_..borv (for PICs with selectable BROWN-OUT DETECTION VOLTAGE) :
      0,
      // wCfgmask2_used :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end dsPIC30F2010

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "dsPIC30F4013",  PIC_MT_FLASH, 24/*!*/,             16384/*instr!*/, 1024,            512,
      // sz80ConfigRegisterInfoFile[] :
      _T("dsPIC30F4013.dev"), // <<< file must be "borrowed" from MLPAB IDE/device !
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      32, 0,  // dsPIC: 32 instruction words (!!!!!!!!) = 32*3 bytes for dsPIC30F
      // lConfMemBase,lConfWordAdr,
      0xF80000,    0xF80000/*caution, no "classic" config word in a dsPIC*/,
      // dwConfMemUsedLocations (first 32)                      , dwConfMemUsedLocations (next 32)
      0x0027/*only 4 of 8  dsPIC config regs used in -2010*/ , 0x0000/*none*/,
      // lDataMemBase,                        lAddressOscCal,
      0x7FFC00/*not 0x7FF000L ! */,        -1/*none*/,
      // Note on dsPIC's "data memory base address" :
      //  IT VARIES WITH THE SIZE OF THE EEPROM ! ! ! !
      //  Instead the END address is *always* (?) 0x7FFFFE,
      //  regardless of the EEPROM size .  Ummm...
      // lIdMemBase,      lIdMemSize,
      0xFF00000/*!*/,  0,       // NO "ID LOCATIONS" IN dsPIC30F ?!
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0xFF0000L,     0xFFFF,        (0x05<<6)+0x01/* "mask" + "variant"*/ ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,     wConfigProgAlgo,
      PIC_ALGO_dsPIC30F,     PIC_ALGO_dsPIC30F, PIC_ALGO_dsPIC30F,
      // wDataProgAlgo,         wVppVddSequence,
      PIC_ALGO_dsPIC30F,     PROGMODE_VDD_THEN_VPP,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      1,          10000,     10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      //  (all these are "don't care" for dsPICs, which use TOTALLY DIFFERENT algoritms, see prog spec.. )
      -1,             -1,      -1,      -1,       -1,      -1,      -1,     -1,      -1,       -1,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,
      0x0000,          0,      0,       0,    0,        0,     // <<< old stuff, not applicable for dsPIC etc
      // (all DON'T CARE for dsPIC's because they have SEVEN(!) config words)
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,  intrc_noclkout,intrc_clkout,extrc_noclkout
      0,         0,    0,    0,    0,          0,           0,      0,          0,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0,         0,    0,         0,       0,      0,       0,  0,
      // wCfgmask_..borv (for PICs with selectable BROWN-OUT DETECTION VOLTAGE) :
      0,
      // wCfgmask2_used :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end dsPIC30F4013

// sz40DeviceName, iCodeMemType, iBitsPerInstruction, code mem size, data EEPROM size, internal RAM size,
    { "dsPIC30F6010",  PIC_MT_FLASH, 24/*!*/,             49152/*instr!*/, 4096,            512,
      // sz80ConfigRegisterInfoFile[] :
      _T("dsPIC30F6010.dev"), // <<< file must be "borrowed" from MLPAB IDE/device !
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      32, 0,  // dsPIC: 32 instruction words (!!!!!!) = 32*3 bytes for dsPIC30F
      // lConfMemBase,lConfWordAdr,
      0xF80000,    0xF80000/*caution, no "classic" config word in a dsPIC*/,
      // dwConfMemUsedLocations (first 32)                      , dwConfMemUsedLocations (next 32)
      0x0027/*only 4 of 8  dsPIC config regs used in -2010*/ , 0x0000/*none*/,
      // lDataMemBase,                        lAddressOscCal,
      0x7FF000/*not 0x7FFC00L ! */,        -1/*none*/,
      // Note on dsPIC's "data memory base address" :
      //  IT VARIES WITH THE SIZE OF THE EEPROM ! ! ! !
      //  Instead the END address is *always* (?) 0x7FFFFE,
      //  regardless of the EEPROM size .  Ummm...
      // lIdMemBase,      lIdMemSize,
      0xFF00000/*!*/,  0,       // NO "USER ID LOCATIONS" IN dsPIC30F ?!
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0xFF0000L,     0xFFFF,        (0x06<<6)+0x08 ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      0,
      // wEraseAlgo,            wCodeProgAlgo,     wConfigProgAlgo,
      PIC_ALGO_dsPIC30F,     PIC_ALGO_dsPIC30F, PIC_ALGO_dsPIC30F,
      // wDataProgAlgo,         wVppVddSequence,
      PIC_ALGO_dsPIC30F,     PROGMODE_VDD_THEN_VPP,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      1,          10000,     10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      //  (all these are "don't care" for dsPICs, which use TOTALLY DIFFERENT algoritms, see prog spec.. )
      -1,             -1,      -1,      -1,       -1,      -1,      -1,     -1,      -1,       -1,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,
      0x0000,          0,      0,       0,    0,        0,     // <<< old stuff, not applicable for dsPIC etc
      // (all DON'T CARE for dsPIC's because they have SEVEN(!) config words)
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,  intrc_noclkout,intrc_clkout,extrc_noclkout
      0,         0,    0,    0,    0,          0,           0,      0,          0,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0,         0,    0,         0,       0,      0,       0,  0,
      // wCfgmask_..borv (for PICs with selectable BROWN-OUT DETECTION VOLTAGE) :
      0,
      // wCfgmask2_used :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    }, // end dsPIC30F6010


// sz40DeviceName, iCodeMemType, iBitsPerInstruction,  code mem size, data EEPROM size, internal RAM size,
    { "PIC??????", PIC_MT_FLASH,       14,                   8192,          256,              -1 ,
      // sz80ConfigRegisterInfoFile[] :
      _T(""),  // no "dev"-file for unknown devices !
      // lCodeMemWriteLatchSize, lCodeMemEraseLatchSize (only important for PIC18Fxxxx and dsPIC) :
      1, 0,
      // lConfMemBase,lConfWordAdr,
      0x2000,      0x2007,
      // dwConfMemUsedLocations 0x2000..0x201F, dwConfMemUsedLocations 0x2020..0x203F
      0x008F                               , 0x0000,
      // lDataMemBase,lAddressOscCal,
      0x2100,       -1,
      // lIdMemBase,      lIdMemSize,
      0x2007,           0,       // NO "ID LOCATIONS" in this "unknown" device
      // lDeviceIdAddr, lDeviceIdMask, lDeviceIdValue,
      0x2006,        0x3FE0,        0x0000 /*0=unknown*/ ,
      // i32CalibAddr[8] = a list of ADDRESSES which contain calibration stuff :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // i32CalibMask[8] = bitmasks (up to 32 bit) for each of the above locations :
      {  -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1       },
      // wCanRead:  1 = can read while programming,   0 = can NOT read in between
      1,
      // wEraseAlgo,     wCodeProgAlgo, wConfigProgAlgo,
      PIC_ALGO_16FXX, PIC_ALGO_16FXX,PIC_ALGO_CONFIG_MEM_16XXX,
      // wDataProgAlgo,                     wVppVddSequence,
      PIC_ALGO_DATA_EEPROM_16XXX,        PROGMODE_VPP_VDD_UNKNOWN,
      // lTi_Clk_us, lTi_Prog_us, lTi_Erase_us :
      10,         10000,       10000,
      // iCmd_..LoadConfig,LoadProg,ReadProg,IncrAddr,BeginProg,EndProg,LoadData,ReadData,EraseProg,EraseData,
      0x00,            0x02,    0x04,    0x06,    0x08,     0x0E,   0x03,    0x05,    0x09,     0x0B,
      // iCmd_..EraseChip, Cmd_BeginProgNoErase (only for 'newer' devices) :
      -1,             -1,
      // wCfgmask_..unused,unknown,cpbits, pwrte,inv_pwrte, wdte,   :
      0x0000,          0x3FFF, 0x0000, 0x0008,0x0008,  0x0004,
      // wCfgmask_..osc,.rc, hs,   xt,   lp,     extclk,intrc_noclkout,intrc_clkout,extrc_noclkout
      0x0003,    0x0003,0x0002,0x0001,0x0000,0x0000,    0x0000,      0x0000,     0x0000,
      // wCfgmask_..cpd,lvp, boden,  mclre,  ddebug, wrcode, bandgap, CCPMX, :
      0x0100,    0x0080, 0x0040, 0x0000, 0x0800, 0x0200, 0x0000,  0x0000,
      // wCfgmask_..borv (introduced for PIC16F7x7, where the BROWN-OUT DETECTION VOLTAGE is selectable) :
      0x0000,
      // wCfgmask2_used (introduced in 2003-12 for PIC16F88) :
      0x0000,

      73 // iCheck73 (to check for missing table entries)
    } // end "unknown" PIC

}; // end BuiltInPicDeviceInfo[]


T_PicConfigBitSetting PicDev_CfgBitSettings[PICDEV_MAX_CONFIG_BIT_SETTINGS];


/*----------- Implementation of routines -----------------------------------*/

//---------------------------------------------------------------------------
int PicDev_BinaryCrossTotal( uint32_t dwBitmask )
// Calculates the "binary cross total" (DL: binäre Quersumme)
{
    int iResult=0;
    while (dwBitmask)
    {
        if (dwBitmask&1) ++iResult;
        dwBitmask >>= 1;
    }
    return iResult;
}


/***************************************************************************/
const char *PicDev_AlgorithmCodeToString(int iAlgorithm)
{
    static char sz8Other[9];
    switch (iAlgorithm)
    {
    case PIC_ALGO_16FXX:
        return  "16Fxx";
    case PIC_ALGO_16FXX_OLD_ERASE:
        return  "16Fxx_OLD_ERASE";
    case PIC_ALGO_12FXX:
        return  "12Fxx";
    case PIC_ALGO_16F81X:
        return  "16F81x";
    case PIC_ALGO_16F87X:
        return  "16F87x";
    case PIC_ALGO_16F87XA:
        return  "16F87xA";
    case PIC_ALGO_16CXX:
        return  "16Cxx";             // also for some 16Cxxx types !

    case PIC_ALGO_ERASE_16F62XA: // added 2004-01-28; 16F628A is not compatible with 16F628
        return  "ERASE_16F62xA";
    case PIC_ALGO_ERASE_16F81X:  // ridiculously complicated way to erase everything in a 16F818
        return  "ERASE_16F81x";
    case PIC_ALGO_ERASE_16F87X:  // similar but still different for a 16F87X .. etc (dont wonder why)
        return  "ERASE_16F87x";
    case PIC_ALGO_ERASE_16F87XA:
        return  "ERASE_16F87xA";
    case PIC_ALGO_ERASE_12F6XX:  // added 2006-03-01 (due to problems with  PIC12F683, reported by Edward Schlunder)
        return  "ERASE_12F6xx";

    case PIC_ALGO_DATA_EEPROM_16XXX:
        return "DATA_EEPROM_16xxx";  // DATA EEPROM programmin algo for all PICs I know so far

    case PIC_ALGO_CONFIG_MEM_16XXX:
        return "CONFIG_MEM_16xxx";   // CONFIGURATION MEMORY programming for many PICs like 16F84
    case PIC_ALGO_CONFIG_MEM_16F81X:
        return "CONFIG_MEM_16F81x";   // CONFIGURATION MEMORY programming algorithm for 16F818, 16F819
    case PIC_ALGO_CONFIG_MEM_16F87XA:
        return "CONFIG_MEM_16F87xA";  // CONFIGURATION MEMORY programming algorithm for 16F87xA

    case PIC_ALGO_16F630:        // added 2004-03-30; 16F630 is NOT compatible with 16F628 ! !
        return  "16F630";

    case PIC_ALGO_PIC16F7X:     // added 2005-08-21, since the prog spec looked quite different
        return  "16F7x";

    case PIC_ALGO_16F7X7:       // added 2004-07-31, again A NEW PROGRAMMING SPEC: TWO words per prog cycle
        return  "16F7x7";

    case PIC_ALGO_PIC16F716:    // added 2005-12-03, because PIC16F716 caused it's own little nightmares..
        return  "16F716";

    case PIC_ALGO_dsPIC30F:     // added 2005-02-19, but not fully functionable yet, VERY STRANGE
        return  "dsPIC30F";

    case PIC_ALGO_PIC18F:       // added 2005-03-04, PIC18F support by M.v.d.Werff ..
        return  "PIC18F";
    case PIC_ALGO_PIC18F_OLD:   // added 2007-01-25, look into PIC18F_EraseChip_4550 for details
        return  "PIC18F_OLD";

    case PIC_ALGO_PIC10F:       // added 2005-07-14 for PIC10F200 ... PIC10F206
        return  "PIC10F20x";

    case PIC_ALGO_MULTI_WORD:
        return  "MULTI_WORD";

    case PIC_ALGO_UNKNOWN:      // added 2004-12-16, this may be the 16F737 DATA-EEPROM-PROGRAMMING algorithm ;-)
        return "NONE";

    default:
        sprintf(sz8Other,"%d",iAlgorithm);
        return  sz8Other;            // something missing, use numeric code
    }
} // end PicDev_AlgorithmCodeToString()

/***************************************************************************/
int PicDev_StringToAlgorithmCode(const char * pszAlgorithmName)
{
    wxChar sz80Temp[81];

    if (stricmp(pszAlgorithmName, "16Fxx")==0)
        return PIC_ALGO_16FXX;
    if (stricmp(pszAlgorithmName, "16Fxx_OLD_ERASE")==0)
        return PIC_ALGO_16FXX_OLD_ERASE;
    if (stricmp(pszAlgorithmName, "12Fxx")==0)
        return PIC_ALGO_12FXX;
    if (stricmp(pszAlgorithmName, "16F630")==0)
        return PIC_ALGO_16F630;
    if (stricmp(pszAlgorithmName, "16F7x7")==0)
        return PIC_ALGO_16F7X7;
    if (stricmp(pszAlgorithmName, "16F7x")==0)
        return PIC_ALGO_PIC16F7X;
    if (stricmp(pszAlgorithmName, "16F716")==0)
        return PIC_ALGO_PIC16F716;

    if (stricmp(pszAlgorithmName, "16F81x")==0)
        return PIC_ALGO_16F81X;
    if (stricmp(pszAlgorithmName, "16F87xA")==0)
        return PIC_ALGO_16F87XA;
    if (stricmp(pszAlgorithmName, "16F87x")==0)
        return PIC_ALGO_16F87X;
    if (stricmp(pszAlgorithmName, "16Cxx")==0)
        return PIC_ALGO_16CXX;
    if (stricmp(pszAlgorithmName, "dsPIC30F")==0)
        return PIC_ALGO_dsPIC30F;
    if (stricmp(pszAlgorithmName, "PIC18F")==0)
        return PIC_ALGO_PIC18F;
    if (stricmp(pszAlgorithmName, "PIC18F_OLD")==0)
        return PIC_ALGO_PIC18F_OLD;
    if (stricmp(pszAlgorithmName, "PIC10F20x")==0)
        return PIC_ALGO_PIC10F;
    if (stricmp(pszAlgorithmName, "ERASE_16F62xA")==0)
        return PIC_ALGO_ERASE_16F62XA;
    if (stricmp(pszAlgorithmName, "ERASE_16F81x")==0)
        return PIC_ALGO_ERASE_16F81X;
    if (stricmp(pszAlgorithmName, "ERASE_16F87xA")==0)
        return PIC_ALGO_ERASE_16F87XA;
    if (stricmp(pszAlgorithmName, "ERASE_16F87x")==0)
        return PIC_ALGO_ERASE_16F87X;
    if (stricmp(pszAlgorithmName, "ERASE_12F6xx")==0)
        return PIC_ALGO_ERASE_12F6XX;

    if (stricmp(pszAlgorithmName, "DATA_EEPROM_16xxx")==0)
        return PIC_ALGO_DATA_EEPROM_16XXX;

    if (stricmp(pszAlgorithmName, "CONFIG_MEM_16xxx")==0)
        return PIC_ALGO_CONFIG_MEM_16XXX;

    if (stricmp(pszAlgorithmName, "CONFIG_MEM_16F81x")==0)
        return PIC_ALGO_CONFIG_MEM_16F81X;

    if (stricmp(pszAlgorithmName, "CONFIG_MEM_16F87xA")==0)
        return PIC_ALGO_CONFIG_MEM_16F87XA;

    if (stricmp(pszAlgorithmName, "MULTI_WORD")==0)
        return PIC_ALGO_MULTI_WORD;

    if (stricmp(pszAlgorithmName, "NONE")==0)
        return PIC_ALGO_UNKNOWN;

    _stprintf(sz80Temp, _T("Unknown algorithm name : \"%hs\" !"), pszAlgorithmName);
    APPL_ShowMsg( APPL_CALLER_PIC_PRG, 0, sz80Temp );
    return  PIC_ALGO_UNKNOWN;
} // end PicDev_StringToAlgorithmCode()


/***************************************************************************/
int PicDev_StringToVppVddSequenceCode( char *pszVppVddSequence )
{
    if (stricmp(pszVppVddSequence, "Vdd_before_Vpp")==0)
        return PROGMODE_VDD_THEN_VPP;
    if (stricmp(pszVppVddSequence, "Vpp_before_Vdd")==0)
        return PROGMODE_VPP_THEN_VDD;

    return PROGMODE_VPP_VDD_UNKNOWN;
} // end PicDev_StringToVppVddSequenceCode()


//---------------------------------------------------------------------------
const char *PicDev_VppVddSequenceCodeToString(int iVppVddSequence)
{
    switch ( iVppVddSequence )
    {
    case PROGMODE_VPP_THEN_VDD :
        return "Vpp_before_Vdd";
    case PROGMODE_VDD_THEN_VPP :
        return "Vdd_before_Vpp";
    default:
        return "unknown";
    }
} // end PicDev_VppVddSequenceCodeToString()


//---------------------------------------------------------------------------
int PicDev_GetNrOfBitsPerIdLocation(void)
{
    if ( PIC_DeviceInfo.iBitsPerInstruction==16 || PIC_DeviceInfo.iBitsPerInstruction==24 )
        return 16;  // 16 bit per ID location for dsPIC30F, and for PIC18F(?)
    else if ( PIC_DeviceInfo.iBitsPerInstruction>=14 )
        return 14;     // 14 bit per ID location for PIC16Fxxx and many others
    else
        return 12;     // 12 bit per ID location for PIC10F20x (since 2005)
} // end PicDev_GetNrOfBitsPerIdLocation()

//---------------------------------------------------------------------------
int PicDev_GetConfMemSize(void)
// Find out how many WORDs(!!) must be read by looking at the BITS
// in PIC_DeviceInfo.dwConfMemUsedLocations .
// THIS INCLUDES POSSIBLE GAPS BETWEEN TWO "IMPLEMENTED" LOCATIONS !
// Caller: Configuration-read-  and write- routines, to find out
//         how many WORD LOCATIONS(!!) must be read in one block .
// The same info can be found in PIC_DeviceInfo_iConfMemSize since 2005-03-05 .
{
    int i, iNWordsToRead;

    for ( iNWordsToRead=0,i=0;  i<=31;  i++ )
    {
        if ( (PIC_DeviceInfo.dwConfMemUsedLocations0 & (1L<<i)) != 0 )
            iNWordsToRead = i+1; // don't "count" bits, instead use number of highest H-bit
        // Example for dsPIC: PIC_DeviceInfo.dwConfMemUsedLocations = 0x0027,
        //     which means only FOUR words are really implemented,
        //     but there are 2 gaps ("RESERVED") between them so must read SIX words
        //     to retrieve the last too (here: FGS @ 0xF8000A) .
        // Similar oddities may be found in PIC18Fxxxx too !
        // Caution: Every bit in PIC_DeviceInfo.dwConfMemUsedLocations stands for
        //          one address step. Examples:
        //  PIC16Fxx: dwConfMemUsedLocations0 = address range 0x2000(->bit0) ... 0x201F(->bit31)
        //            dwConfMemUsedLocations2 = address range 0x2020(->bit0) ... 0x203F(->bit31)
        //  PIC18Fxx: dwConfMemUsedLocations0 = address range 0x300000(->bit0)...0x30001F(->bit31)
        //            dwConfMemUsedLocations2 = address range 0x300000(->bit0)...0x30003F(->bit31)
        //            dwConfMemUsedLocations0 = 0x0000007F means SEVEN
        //
    }
    return iNWordsToRead;
} // end PicDev_GetNrOfImplementedConfigWordLocations()


//---------------------------------------------------------------------------
int   PicDev_IsConfigMemLocationValid(uint32_t dwCfgMemAddress)
{
    long i32Offset = (long)dwCfgMemAddress - (long)PIC_DeviceInfo.lConfMemBase;

    if ( PIC_DeviceInfo.iBitsPerInstruction==24  || PIC_DeviceInfo.iBitsPerInstruction==16 )
    {
        // 24 bit per instruction in code memory .. looks like a dsPIC : only EVEN addresses useable
        i32Offset /= 2;
    }
    else
    {
        // most likely PIC12Fxxx or 16Fxxx, no need to divide address offset by two !
    }

    if ( i32Offset>=0 && i32Offset<=31)
        return (PIC_DeviceInfo.dwConfMemUsedLocations0 & ((uint32_t)1 << (uint32_t)i32Offset)) != 0;
    i32Offset-=32;
    if ( i32Offset>=0 && i32Offset<=31)
        return (PIC_DeviceInfo.dwConfMemUsedLocations2 & ((uint32_t)1 << (uint32_t)i32Offset)) != 0;

    return 0;  // otherwise "invalid"

} // end PicDev_IsConfigMemLocationValid()


//---------------------------------------------------------------------------
uint32_t PicDev_GetVerifyMaskForAddress(uint32_t dwTargetAddress)
// Called by half a dozen of different "verify"-subroutines , if an
//   "individual" bitmask for AND-combinig read and written value is required.
//   This is essential when verifying CONFIG-memory,
//   with lots of "unimplemented" and "reserved" bits which must be ignored
//   to avoid false verify-errors !
{
    bool  fFoundConfigMemInfo;
    uint32_t dwVerifyMask;
    T_PicConfigBitInfo *pConfigBitInfo;

    // What's this, CONFIGURATION MEMORY ?
    if (   ( (long)(dwTargetAddress) >= PIC_DeviceInfo.lConfMemBase)
            && ( (long)(dwTargetAddress) <  PIC_DeviceInfo.lConfMemBase + PIC_DeviceInfo_iConfMemSize * PicDevInfo_i32ConfMemAddressFactor ) )
    {

        if ( !  PicDev_IsConfigMemLocationValid(dwTargetAddress) )
        {
            // this is an "unimplemented" location for the VERIFY process:
            return 0x0000000L;  // ignore this location when verifying
        }

        if ( (long)dwTargetAddress == PIC_DeviceInfo.lDeviceIdAddr )
        {
            // The "device ID location" cannot be programmed, and shall be ignored for that reason
            // (furthermore, the device ID value is often not contained in the hex file).
            return 0x0000000L;  // ignore this location when verifying
        }

        // Search info from Microchip's DEV-file for info on this memory location.
        //  If no info found in there, continue with something else (further down)
        pConfigBitInfo = PicDev_ConfigBitInfo;  // info parsed from Microchip's DEV-file
        fFoundConfigMemInfo = false; // no information found for this config memory location yet
        dwVerifyMask   = 0L;
        while ( pConfigBitInfo->szText && pConfigBitInfo->szText[0] ) // Check all entries in the config bit table..
        {
            fFoundConfigMemInfo |=  // check if there are informations about the config memory AT ALL(*)
                ( ( (long)dwTargetAddress >= PIC_DeviceInfo.lConfMemBase)
                  && ( (long)dwTargetAddress < (PIC_DeviceInfo.lConfMemBase + PIC_DeviceInfo_iConfMemSize * PicDevInfo_i32ConfMemAddressFactor) ) );

            // With PIC12F629 the reserved bits are defined as a field named Reserved.
            // So it is nececessary to exclude those fields.
            // It will be supposed that reserved fields will not have more than one allowed value
            if (( dwTargetAddress == pConfigBitInfo->dwAddress ) // address match: OR this bitmask..
            &&  (pConfigBitInfo->pBitCombinations != NULL)          //-- Issue #0000031
            &&  (pConfigBitInfo->pBitCombinations->pNext != NULL))
                dwVerifyMask |= pConfigBitInfo->dwBitmask;  // bitmask
            // if dwTargetAddress is an EVEN address, and PicDevInfo_i32ConfMemAddressFactor==2
            //  (which means "every LOCATION in the config memory consists of TWO BYTES WITH INDIVIDUAL ADDRESSES),
            // or the bitmask of the next ODD address into the higher byte of the verify-mask :
            if ( (PicDevInfo_i32ConfMemAddressFactor==2) // 2 for PIC18F ("words", but each BYTE has its own address)
                    &&((dwTargetAddress&0x0001)==0)           // target address is an EVEN address..
                    &&(pConfigBitInfo->dwAddress == (dwTargetAddress+1) ) )
            {
                dwVerifyMask |= (pConfigBitInfo->dwBitmask<<8);  // bitmask for 'upper byte'
            }
            ++pConfigBitInfo;
        } // end while( pConfigBitInfo->sz60Text[0] )
        if ( fFoundConfigMemInfo ) // (*)ex: "if (dwVerifyMask!=0)" but not all locations are used at all !
        {
            return dwVerifyMask;   // found info in Microchip's dev-file -> USE IT !
        }

        if ( (long)dwTargetAddress == PIC_DeviceInfo.lConfWordAdr  )
        {
            // It's what they called "Configuration Word" for PIC16Fxxx, but which device ?

            if ( PIC_DeviceInfo.iBitsPerInstruction==14 )
            {
                // it's a PIC16Fxx,  PIC16Fxxx,  or similar, and the  "CONFIG WORD" at 0x2007..
                return
                    PIC_DeviceInfo.wCfgmask_unknown  // "unknown" but "existing" bits may be programmed + verified !
                    | PIC_DeviceInfo.wCfgmask_cpbits
                    | PIC_DeviceInfo.wCfgmask_pwrte
                    | PIC_DeviceInfo.wCfgmask_wdte
                    | PIC_DeviceInfo.wCfgmask_oscillator
                    | PIC_DeviceInfo.wCfgmask_cpd      // ... new devices ...
                    | PIC_DeviceInfo.wCfgmask_lvp
                    | PIC_DeviceInfo.wCfgmask_boden
                    | PIC_DeviceInfo.wCfgmask_mclre
                    | PIC_DeviceInfo.wCfgmask_ddebug   // ... and even newer devices
                    | PIC_DeviceInfo.wCfgmask_wrcode
                    | PIC_DeviceInfo.wCfgmask_bandgap  // bandgap calibration bits, 0x3000 for 12F675
                    | PIC_DeviceInfo.wCfgmask_ccpmx;   // capture/compare output pin control bit (PIC16F818 et al)
            } // end if < "Config Word" for PIC's with 14-bit core >

        } // end if( dwCfgMemAddress == PIC_DeviceInfo.lConfWordAdr  )

        if ( PIC_DeviceInfo.iBitsPerInstruction==24 )
        {
            // a few hard-coded CONFIGURATION REGISTER verify-bitmasks for dsPIC30F,
            // based on Microchip's "dsPIC30F Flash Programming Specification" DS70102D,
            // Table 5-8 (Config regs for dsPIC30F2010, 4011, 4012, 5011, 5013, 6010..14, no "-A" )
            // Footnote from table 5-8 : "Reserved bits read as '1' and must be programmed as '1'"
            switch ( dwTargetAddress )
            {
            case 0xF80000 :
                return 0xC30F;  // "FOSC"  (here: FOS<1:0> only)
            case 0xF80002 :
                return 0x803F;  // "FWDT"
            case 0xF80004 :
                return 0x87B3;  // "FBORPOR"
            case 0xF80006 :
                return 0x0000;  // "RESERVED1"
            case 0xF80008 :
                return 0x0000;  // "RESERVED2"
            case 0xF8000A :
                return 0x0003;  // "FGS"
            case 0xF8000C :
                return 0xC002;  // "FICD"
            } // end switch( dwCfgMemAddress ) for dsPIC30F
        } // end < devices with 24 bits per core instruction = dsPIC >

        // Arrived here: Assume it's a 'normal' CONFIGURATION word location with NO missing bits ...
        switch ( PIC_DeviceInfo.iBitsPerInstruction )
        {
        case 12:
        case 14:
            return 0x00003FFFL;   // PIC's with 14-bit core have 14-bit config memory locations

        case 16:
            return 0x0000FFFFL;   // PIC's with 16-bit core have 16-bit config memory locations
            // (though they may look byte-wise organized, looking at some data sheets)

        case 24:
            return 0x0000FFFFL;   // drums please .. PIC's with 24-bit core have 16(!)-bit config REGISTERS (!)

        default:
            return 0x0000FFFFL;   // otherwise, assume a hypothetical machine with 16-bit registers
        }
    }  // end if  <  it's configuration memory >
    else  // ... or is it DATA EEPROM MEMORY ... ?
        if (   ( (long)(dwTargetAddress) >= PIC_DeviceInfo.lDataMemBase)
                && ( (long)(dwTargetAddress) <  PIC_DeviceInfo.lDataMemBase + PIC_DeviceInfo.lDataEEPROMSizeInByte ) )
        {
            return 0x000000FFL;  // WinPic always treats DATA EEPROM as BYTE-array !
        }

    // Arrived here: Assume it's a 'normal' CODE MEMORY location  ...
    switch ( PIC_DeviceInfo.iBitsPerInstruction )
    {
    case  8:
        return 0x000000FFL;
    case 12:
        return 0x00000FFFL;
    case 14:
        return 0x00003FFFL;
    case 16:
        return 0x0000FFFFL;
    case 24:
        return 0x00FFFFFFL;
    case 32:
        return 0xFFFFFFFFL;
    default:
        return 0x0000FFFFL;
    }

} // end PicDev_GetVerifyMaskForAddress()


//---------------------------------------------------------------------------
const wxChar *PicDev_GetInfoOnConfigMemory(uint32_t dwCfgMemAddress)
{
    if ( PIC_DeviceInfo.iBitsPerInstruction == 24 )
    {
        // 24 bit per instruction in code memory .. looks like a dsPIC :
        switch ( dwCfgMemAddress ) // note: only EVEN addresses accessable here
        {
        case 0xF80000:
            return _T("FOSC");  // note : these are MICROCHIP's names !
        case 0xF80002:
            return _T("FWDT");
        case 0xF80004:
            return _T("FBORPOR");
        case 0xF80006:
            return _T("RESERVED1");
        case 0xF80008:
            return _T("RESERVED2");
        case 0xF8000A:
            return _T("FGS");
        case 0xF8000C:
            return _T("FICD");
        default:
            return _T("");  // no info about this location !
        }
    }
    else // not a dsPIC, but .. ?
        if ( PIC_DeviceInfo.iBitsPerInstruction == 16 )
        {
            // 16 bit per instruction in code memory .. looks like a PIC18F :
            switch ( dwCfgMemAddress )
            {
            case 0x300000:
                return _T("FOSC,CLKDIV");  // note : these are MICROCHIP's names !
            case 0x300002:
                return _T("BORV,WDT");     // part at 0x200003
            case 0x300004:
                return _T("MCLRE,CCP2MX"); // most at 0x300005, but 16 bit here
            case 0x300006:
                return _T("DEBUG,STVREN");
            case 0x300008:
                return _T("CPx");
            case 0x30000A:
                return _T("WRTx");
            case 0x30000C:
                return _T("EBTRx");
            default:
                return _T("");  // no info about this location !
            }
        }
        else // neither 24 nor 16 bit core, but .. 14 ?
            if ( PIC_DeviceInfo.iBitsPerInstruction == 14 )
            {
                switch ( dwCfgMemAddress )
                {
                case 0x2000:
                case 0x2001:
                case 0x2002:
                case 0x2003:
                    return _("User ID");

                case 0x2006:
                    return _("Device ID");

                case 0x2007:
                    return _("Config Word");
                case 0x2008:
                    return _("Config2 /Cal");
                case 0x2009:
                    return _("Calib Word 2");
                default:
                    return _T("");
                }
            } // end if < 14-bit core >
            else // neither 24, 16, nor 14- bit core, but .. ?
                if ( PIC_DeviceInfo.iBitsPerInstruction == 12 )
                {
                    // 12-bit core, must be PIC10F20x (since no other 12-bit devices supported)
                    // Caution: The special address locations depend on the code memory size !!
                    if ( PIC_DeviceInfo.lCodeMemSize <= 256 )
                    {
                        switch ( dwCfgMemAddress )
                        {
                        case 0x00FF:
                            return _("Reset/OSCCAL");
                        case 0x0100:
                        case 0x0101:
                        case 0x0102:
                        case 0x0103:
                            return _("User ID");
                        case 0x0104:
                            return _("Backup OSCCAL");
                        case 0x01FF:
                            return _("Config Word");
                        case 0x0FFF:
                            return _("Config Word(f)");
                        default:
                            return _T("");
                        }
                    }
                    // ex: else // here for PIC10F202 / 206 (512 words code memory)
                    // Modified 2006-04-13 by L.L to support 12F508 (509?) too :
                    else if ( PIC_DeviceInfo.lCodeMemSize <= 512 )
                        // here for PIC10F202 / 206 (512 words code memory) :
                    {
                        switch ( dwCfgMemAddress )
                        {
                        case 0x01FF:
                            return _("Reset/OSCCAL");
                        case 0x0200:
                        case 0x0201:
                        case 0x0202:
                        case 0x0203:
                            return _("User ID");
                        case 0x0204:
                            return _("Backup OSCCAL");
                        case 0x03FF:
                            return _("Config Word");
                        case 0x0FFF:
                            return _("Config Word(f)");
                        default:
                            return _T("");
                        }
                    }
                    else // (1024 words code memory) :
                    {
                        switch ( dwCfgMemAddress )
                        {
                        case 0x03FF:
                            return _("Reset/OSCCAL");
                        case 0x0400:
                        case 0x0401:
                        case 0x0402:
                        case 0x0403:
                            return _("User ID");
                        case 0x0404:
                            return _("Backup OSCCAL");
                        case 0x07FF:
                            return _("Config Word");
                        case 0x0FFF:
                            return _("Config Word(f)");
                        default:
                            return  _T("");
                        }
                    }
                } // end if < 12-bit core >


    return _T("??");

} // end PicDev_GetInfoOnConfigMemory()

//---------------------------------------------------------------------------
const wxChar * PicDev_ConfigBitValueToString( T_PicConfigBitInfo *pConfigBitInfo, uint32_t dwConfigRegisterContent )
{
    P_PicConfigBitSetting pBitCombiInfo;
    uint32_t dwValue, dwBitmask, dwExtraBitmask;
    int iEmgyBrake=100;
    // First try to find the VALUE in the chain of bit-combinations,
    //   if it exists, return a pointer to the STRING (text), needless to copy..
    dwBitmask = pConfigBitInfo->dwBitmask;
    pBitCombiInfo = pConfigBitInfo->pBitCombinations;

    // Shift the "bit group value" RIGHT until the group's LSB is in bit zero.
    // Example: dwBitmask=0x8000  ->  value must be shifted RIGHT 15 times
    dwBitmask = pConfigBitInfo->dwBitmask;
    dwValue = dwConfigRegisterContent & dwBitmask;
    if (dwBitmask)
    {
        while ( (dwBitmask & 1L) == 0)
        {
            dwValue   >>= 1;
            dwBitmask >>= 1;
        }
    } // note: after this loop, both "value" and "bitmask" are right-aligned !

    if ( pBitCombiInfo )
    {
        do
        {
            dwExtraBitmask = pBitCombiInfo->dwExtraMask;
            // Note: This "extra bitmask" is called "req" in Microchip's DEV-files.
            // Look at the dsPIC30F2010's "Clock Switching Mode" bit group (FCKSM)
            //   to understand why certain bit combinations need this extra mask !
            if ( (dwValue & dwBitmask & dwExtraBitmask) == ( pBitCombiInfo->dwCombiValue & dwBitmask & dwExtraBitmask) )
            {
                return pBitCombiInfo->szComboText;
            }
            pBitCombiInfo = pBitCombiInfo->pNext;
        }
        while (pBitCombiInfo && (iEmgyBrake--)>0 );
    }

    // 123456789012345678901234567890
    return _("-invalid combination-");  // max 40 characters !
} // end PicDev_ConfigBitValueToString()

//---------------------------------------------------------------------------
bool PicDev_ConfigBitStringToValue( T_PicConfigBitInfo *pConfigBitInfo, const wxChar *pszComboText, uint32_t *pdwDest )
{
    bool fResult=false;
    P_PicConfigBitSetting pBitCombiInfo;
    uint32_t dwValue, dwBitmask;
    // First try to find the string in the chain of possible bit-combinations...
    pBitCombiInfo = pConfigBitInfo->pBitCombinations;
    dwValue = 0;
    if ( pBitCombiInfo )
    {
        do
        {
            if (_tcscmp( pszComboText, pBitCombiInfo->szComboText) == 0)
            {
                dwValue = pBitCombiInfo->dwCombiValue;
                fResult = true;
                break;
            }
            pBitCombiInfo = pBitCombiInfo->pNext;
        }
        while (pBitCombiInfo);
    }
    else // if there is NO bit-combination-list, could it be a NUMERIC value ?
    {
//     dwValue = PicHex_ParseInteger( pszComboText, 0/*default*/ );
//     fResult = false;
        fResult = wxString(pszComboText).ToLong((long int*)&dwValue);
    }

    // Shift the "bit group value" LEFT until the group's LSB is at the right position.
    // Example: dwBitmask=0x8000  ->  value must be shifted LEFT 15 times
    dwBitmask = pConfigBitInfo->dwBitmask;
    if (dwBitmask)
    {
        while ( (dwBitmask & 1L) == 0)
        {
            dwValue   <<= 1;
            dwBitmask >>= 1;
        }
    }

    // Combine value with bitmask.  See PicDev_BitCombi_HighActiveOnOff for reason:
    // the "ON"-value is 0xFFFFFFFF there, not 0x00000001, so we can use that
    // combination table (PicDev_BitCombi_HighActiveOnOff) also for "uniform multi-bit groups",
    //  for example the code protect feature in some very old devices .
    dwBitmask = pConfigBitInfo->dwBitmask;
    if ( pdwDest )
        *pdwDest = dwValue & pConfigBitInfo->dwBitmask;

    return fResult;
} // end PicDev_ConfigBitStringToValue()


//---------------------------------------------------------------------------
// Access to the PIC-device database ....
//  Note: Totally rewritten in 2005-10 !
//        The device-database was once a real INI-file,
//        but Win98 cannot handle INI-files larger than 64 kByte - grrr - ...
//        So when the size of DEVICES.INI exceeded 64kBytes in autumn 2005,
//        the old INI-file crap was replaced by DL4YHF's QFile access .
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
const wxChar * PicDev_GetDeviceFileName(void)
{
    static wxChar sz511FileName[512];
//  wxString s = ExtractFilePath(Application->ExeName)+C_DEVLIST_FILE_NAME;
    wxFileName  DeviceListFilename (wxStandardPaths::Get().GetExecutablePath());
    DeviceListFilename.SetFullName(C_DEVLIST_FILE_NAME);
    wxString s = DeviceListFilename.GetFullPath();
    _tcsncpy( sz511FileName, s.c_str(), 511 );
    sz511FileName[511]=0;
    return sz511FileName;
}

//---------------------------------------------------------------------------
int PicDev_SearchDeviceDefInFile(const char * pszDeviceName) // input: name of a PIC DEVICE (not a filename)
// Checks if a certain device is contained in the PIC definition file.
// Returns -1 if the device is NOT contained in that file,
//  other wise an index (0...n) .
// If pszDeviceName is NULL, this function returns the NUMBER OF PIC DEVICES
// found in the database.
{
// int  i;
    int iCountDevs=0, iResult=-1;
    T_QFile myQFile;  // "Quick File Access"-module to handle text files line-by-line
    char *pszSection;
    char sz255Line[256];
    char *cp;


    if ( myQFile.QFile_Open( PicDev_GetDeviceFileName(), QFILE_O_RDONLY ) )
    {
        // Check all section headers until the section for this PIC was found.
        while ( (iResult<0) && (myQFile.QFile_ReadLine( sz255Line, 254 ) >= 0) )
        {
            if ( (sz255Line[0]=='[') && (strncmp(sz255Line,"[Devices]",9)!=0) )
            {
                pszSection = sz255Line+1;
                cp=strchr(pszSection,']');
                if ( cp ) // only if this looks like a SECTION HEADER..
                {
                    *cp='\0';   // remove ']' from the section name
                    if ( pszDeviceName )
                    {
                        if ( strcmp( pszSection, pszDeviceName ) == 0 )
                        {
                            iResult = iCountDevs;
                            break;
                        }
                    }
                    ++iCountDevs;
                }
            }
        } // end while
        myQFile.QFile_Close();  // close the device database again
    } // end if < file opened >

    if ( pszDeviceName )
        return iResult;
    else return iCountDevs;

} // end PicDev_SearchDeviceDefInFile()

//---------------------------------------------------------------------------
bool PicDev_GetDeviceNameFromFileByIndex( int index,
        char * pszDstDeviceName, int iMaxLen )  // output: name of a PIC DEVICE
{
    int  count=0;
    bool found=false;
    T_QFile myQFile;  // "Quick File Access"-module to handle text files line-by-line
    char *pszSection, *cp;
    char sz255Line[256];


    if ( myQFile.QFile_Open( PicDev_GetDeviceFileName(), QFILE_O_RDONLY ) )
    {
        // Check all section headers until the section for this PIC was found.
        while ( (!found) && (myQFile.QFile_ReadLine( sz255Line, 254 ) >= 0) )
        {
            if ( (sz255Line[0]=='[') && (strncmp(sz255Line,"[Devices]",9)!=0) )
            {
                pszSection = sz255Line+1;
                cp=strchr(pszSection,']');
                if ( cp ) // only if this looks like a SECTION HEADER..
                {
                    *cp='\0';   // remove ']' from the section name
                    if ( index == count )
                    {
                        strncpy( pszDstDeviceName, pszSection, iMaxLen );
                        found = true;
                        break;
                    }
                    ++count;
                }
            }
        } // end while
        myQFile.QFile_Close();  // close the device database again
    } // end if < file opened >
    return found;
} // end PicDev_GetDeviceNameFromFileByIndex()


//---------------------------------------------------------------------------
bool PicDev_LoadPicDeviceDefinitionFromFile(
    const char * pszDeviceName,       // name of a new PIC DEVICE (not a filename!)
    T_PicDeviceInfo *pDevInfo,  // destination structure (to be filled)
    bool fMayShowInfo )         // true=may show info from section header
// Last revisions:
//  2005-10-24: Got rid of the poor INI-file stuff, because DEVICES.INI
//              grew too large, and Win98 only supports a max (INI-)file size
//              of 64 kBytes !
{

// int     i;
    T_QFile myQFile;  // "Quick File Access"-module to handle text files line-by-line
    bool fFoundSection;
    char sz80Section[84];
    char sz255Line[256];
    char *pszValue, *pszKey, *cp;
    long i32Value;

    PicDev_FillDefaultDeviceInfo(pDevInfo);  // just in case something is missing

    if (pszDeviceName==NULL) return false;
    if (pszDeviceName[0]==0) return false;


    //  Open the file to see what's in it ...
    if ( ! myQFile.QFile_Open( PicDev_GetDeviceFileName(), QFILE_O_RDONLY ) )
    {
        APPL_ShowMsg( APPL_CALLER_PIC_PRG, 0, _("ERROR: Cannot find %s ."), PicDev_GetDeviceFileName() );
        return false;
    }

    // Skip all lines until the section for this PIC was found
    fFoundSection = false;
    sprintf(sz80Section, "[%s]", pszDeviceName );
    while ( (!fFoundSection) && (myQFile.QFile_ReadLine( sz255Line, 254 ) >= 0) )
    {
        if ( sz255Line[0]=='[' )
        {
            cp=strchr(sz255Line,']');
            if ( cp ) // to remove trailing junk after a section header..
            {
                cp[1]='\0';
                if ( strcmp( sz80Section, sz255Line ) == 0 )
                    fFoundSection = true;
            }
        }
    }

    if ( fFoundSection )
    {
        while ( myQFile.QFile_ReadLine( sz255Line, 254 ) >= 0 )
        {
            if ( sz255Line[0]=='[' ) // end of the current section !
                break;
            pszKey   = sz255Line;
            pszValue = strchr(sz255Line, '=' );
            if ( (sz255Line[0]!=';') && (pszValue!=NULL) )
            {
                *pszValue = '\0'; // terminate KEY-name (overwrite '=')
                ++pszValue;       // advance VALUE-pointer...
                while ( *pszValue==' ')
                    ++pszValue;
                cp = pszValue+strlen(pszValue);  // remove trailing spaces..
                while ( (cp>pszValue) && (cp[-1]==' ') )
                {
                    *(--cp)='\0';
                }
                i32Value = PicHex_ParseInteger(pszValue,0);  // convert the VALUE (string) into a number already
                if ( strncmp(pszKey, "info", 4) == 0 )
                {
                    if ( pszValue[0] && fMayShowInfo )
                        APPL_ShowMsg(APPL_CALLER_MAIN,0,_("Info: %hs"), pszValue );
                }
                else if ( strcmp(pszKey, "DeviceName")== 0 )
                {
                    strncpy(pDevInfo->sz40DeviceName, pszValue, 40);
                }
                else if ( strcmp(pszKey, "CodeMemType")== 0 )
                {
                    pDevInfo->iCodeMemType = i32Value;
                }
                else if ( strcmp(pszKey, "CodeMemBitsPerInst")== 0 )
                {
                    pDevInfo->iBitsPerInstruction = i32Value;
                }
                else if ( strcmp(pszKey, "CodeMemSize")== 0 )
                {
                    pDevInfo->lCodeMemSize = i32Value;
                }
                else if ( strcmp(pszKey, "DataEEPROMSize")== 0 )
                {
                    pDevInfo->lDataEEPROMSizeInByte= i32Value;
                }
                else if ( strcmp(pszKey, "InternalRAMSize")== 0 )
                {
                    pDevInfo->lInternalRAMSize=i32Value;
                }
                else if ( strcmp(pszKey, "DeviceInfoFileName")== 0 )
                {
                    CopyIso8859_1_TChar(pDevInfo->sz80ConfigRegisterInfoFile, pszValue, 80);
//                _tcsncpy(pDevInfo->sz80ConfigRegisterInfoFile, pszValue, 80);
                }
                else if ( strcmp(pszKey, "CodeMemWriteLatchSize")== 0 )
                {
                    pDevInfo->lCodeMemWriteLatchSize = i32Value; /* here: write-buffer-size in BYTES ! */
                }
                else if ( strcmp(pszKey, "CodeMemEraseLatchSize")== 0 )
                {
                    pDevInfo->lCodeMemEraseLatchSize = i32Value; /* here: erase-buffer-size in BYTES ! */
                }
                else if ( strcmp(pszKey, "AddrConfigMem")== 0 )
                {
                    pDevInfo->lConfMemBase = i32Value;
                }
                else if ( strcmp(pszKey, "AddrConfigWord")== 0 )
                {
                    pDevInfo->lConfWordAdr = i32Value;
                    if (pDevInfo->lConfWordAdr==0x2007) // should be true in over 99.9 percent of all cases :
                        pDevInfo->dwConfMemUsedLocations0 |= (0x0001 << 0x07);  // mark config word as "existing"
                }
                else if ( strcmp(pszKey, "ConfigMemUsedLocs00_1F")== 0 )
                {
                    pDevInfo->dwConfMemUsedLocations0 = i32Value;
                }
                else if ( strcmp(pszKey, "ConfigMemUsedLocs20_3F")== 0 )
                {
                    pDevInfo->dwConfMemUsedLocations2 = i32Value;
                }
                else if ( strcmp(pszKey, "AddrDataMem")== 0 )
                {
                    pDevInfo->lDataMemBase = i32Value;
                }
                else if ( strcmp(pszKey, "AddrOscCalib")== 0 )
                {
                    pDevInfo->lAddressOscCal= i32Value;
                }
                else if ( strcmp(pszKey, "IdMemoryBase")== 0 )
                {
                    pDevInfo->lIdMemBase= i32Value;
                }
                else if ( strcmp(pszKey, "IdMemorySize")== 0 )
                {
                    pDevInfo->lIdMemSize= i32Value;
                }
                else if ( strcmp(pszKey, "DeviceIdAddr")== 0 )
                {
                    pDevInfo->lDeviceIdAddr= i32Value;
                }
                else if ( strcmp(pszKey, "DeviceIdMask")== 0 )
                {
                    pDevInfo->lDeviceIdMask= i32Value;
                }
                else if ( strcmp(pszKey, "DeviceIdValue")== 0 )
                {
                    pDevInfo->lDeviceIdValue= i32Value;
                }
                else if ( strcmp(pszKey, "CalibAddrs")== 0 )
                {
                    // An additional list of ADDRESSES with calibration-stuff
                    // which may be scattered all over the chip...
                    //  Example: addr 0x2008 for PIC12F683 .
                    pDevInfo->i32CalibAddr[0] = i32Value;
                }
                else if ( strcmp(pszKey, "CalibMasks")== 0 )
                {
                    // Closely related to CalibAddrs, here one BITMASK
                    //  for every ADDRESS with calibration-bits .
                    pDevInfo->i32CalibMask[0] = i32Value;
                }
                else if ( strcmp(pszKey, "CanRead")== 0 )
                {
                    pDevInfo->wCanRead =(uint16_t)i32Value;
                }
                else if ( strcmp(pszKey, "EraseAlgo")== 0 )
                {
                    pDevInfo->wEraseAlgo = PicDev_StringToAlgorithmCode(pszValue);
                }
                else if ( strcmp(pszKey, "CodeProgAlgo")== 0 )
                {
                    pDevInfo->wCodeProgAlgo= PicDev_StringToAlgorithmCode(pszValue);
                }
                else if ( strcmp(pszKey, "ConfigProgAlgo")== 0 )
                {
                    pDevInfo->wConfigProgAlgo=PicDev_StringToAlgorithmCode(pszValue);
                }
                else if ( strcmp(pszKey, "DataProgAlgo")== 0 )
                {
                    pDevInfo->wDataProgAlgo= PicDev_StringToAlgorithmCode(pszValue);
                }
                else if ( strcmp(pszKey, "VppVddSequence")== 0 )
                {
                    pDevInfo->wVppVddSequence= PicDev_StringToVppVddSequenceCode(pszValue);
                    // Note: the GUI uses .wVppVddSequence to check the state of the option
                    //       "raise Vdd before MCLR=Vpp" on the "Other Options" panel .
                    //    If does *NOT* change the checkmark automatically, but shows a
                    //    warning if the settings may be incompatible. This is the result
                    //    of a "long story", partially told in PIC_HW_ProgMode() !
                }
                else if ( strcmp(pszKey, "Ti_Clock_us")== 0 )
                {
                    pDevInfo->lTi_Clock_us =i32Value;
                }
                else if ( strcmp(pszKey, "Ti_Prog_us")== 0 )
                {
                    pDevInfo->lTi_Prog_us  = i32Value;
                }
                else if ( strcmp(pszKey, "Ti_Erase_us")== 0 )
                {
                    pDevInfo->lTi_Erase_us = i32Value;
                }
                else if ( strcmp(pszKey, "CmdLoadConfig")== 0 )
                {
                    pDevInfo->iCmd_LoadConfig = i32Value;
                }
                else if ( strcmp(pszKey, "CmdLoadProg")== 0 )
                {
                    pDevInfo->iCmd_LoadProg  = i32Value;
                }
                else if ( strcmp(pszKey, "CmdReadProg")== 0 )
                {
                    pDevInfo->iCmd_ReadProg = i32Value;
                }
                else if ( strcmp(pszKey, "CmdIncrAddr")== 0 )
                {
                    pDevInfo->iCmd_IncrAddr = i32Value;
                }
                else if ( strcmp(pszKey, "CmdBeginProg")== 0 )
                {
                    pDevInfo->iCmd_BeginProg = i32Value;
                }
                else if ( strcmp(pszKey, "CmdEndProg")== 0 )
                {
                    pDevInfo->iCmd_EndProg = i32Value;
                }
                else if ( strcmp(pszKey, "CmdLoadData")== 0 )
                {
                    pDevInfo->iCmd_LoadDataDM = i32Value;
                }
                else if ( strcmp(pszKey, "CmdReadData")== 0 )
                {
                    pDevInfo->iCmd_ReadDataDM = i32Value;
                }
                else if ( strcmp(pszKey, "CmdEraseProg")== 0 )
                {
                    pDevInfo->iCmd_EraseProg = i32Value;
                }
                else if ( strcmp(pszKey, "CmdEraseData")== 0 )
                {
                    pDevInfo->iCmd_EraseData = i32Value;
                }
                else if ( strcmp(pszKey, "CmdEraseChip")== 0 )
                {
                    pDevInfo->iCmd_EraseChip = i32Value;
                }
                else if ( strcmp(pszKey, "CmdBeginProgNoErase")== 0 )
                {
                    pDevInfo->iCmd_BeginProgNoErase = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_unused")== 0 )
                {
                    pDevInfo->wCfgmask_unused = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_unknown")== 0 )
                {
                    pDevInfo->wCfgmask_unknown = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_cpbits")== 0 )
                {
                    pDevInfo->wCfgmask_cpbits = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_cpd")== 0 )
                {
                    pDevInfo->wCfgmask_cpd = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_pwrte")== 0 )
                {
                    pDevInfo->wCfgmask_pwrte = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_inv_pwrte")== 0 )
                {
                    pDevInfo->wCfgmask_inv_pwrte = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_wdte")== 0 )
                {
                    pDevInfo->wCfgmask_wdte = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_oscillator")== 0 )
                {
                    pDevInfo->wCfgmask_oscillator = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_osc_rc")== 0 )
                {
                    pDevInfo->wCfgmask_osc_rc = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_osc_hs")== 0 )
                {
                    pDevInfo->wCfgmask_osc_hs = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_osc_xt")== 0 )
                {
                    pDevInfo->wCfgmask_osc_xt = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_osc_lp")== 0 )
                {
                    pDevInfo->wCfgmask_osc_lp = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_osc_extclk")== 0 )
                {
                    pDevInfo->wCfgmask_osc_extclk = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_osc_intrc_noclkout")== 0 )
                {
                    pDevInfo->wCfgmask_osc_intrc_noclkout = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_osc_intrc_clkout")== 0 )
                {
                    pDevInfo->wCfgmask_osc_intrc_clkout = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_osc_extrc_noclkout")== 0 )
                {
                    pDevInfo->wCfgmask_osc_extrc_noclkout = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_lvp")== 0 )
                {
                    pDevInfo->wCfgmask_lvp = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_boden")== 0 )
                {
                    pDevInfo->wCfgmask_boden = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_mclre")== 0 )
                {
                    pDevInfo->wCfgmask_mclre = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_ddebug")== 0 )
                {
                    pDevInfo->wCfgmask_ddebug = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_wrcode")== 0 )
                {
                    pDevInfo->wCfgmask_wrcode = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_bandgap")== 0 )
                {
                    pDevInfo->wCfgmask_bandgap = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask_ccpmx")== 0 )
                {
                    pDevInfo->wCfgmask_ccpmx = i32Value;
                }
                else if ( strcmp(pszKey, "Cfgmask2_used_bits")== 0 )
                {
                    pDevInfo->wCfgmask2_used = i32Value;
                    // bitmasks for the SECOND config word since 2003-12 (for PIC16F88)
                }
                else
                {
                    APPL_ShowMsg( APPL_CALLER_PIC_PRG, 0,
                                  _("ERROR: Unknown key \"%hs\" in section %hs ."), pszKey, sz80Section  );
                }
            } // end if( pszValue )
        } // end while < more lines in this section >
    } // end if ( fFoundSection )
    else
    {
        APPL_ShowMsg( APPL_CALLER_PIC_PRG, 0,
                      _("ERROR: Section %hs not found in device database ."), sz80Section );
    }


    myQFile.QFile_Close();  // close the device database again

    return fFoundSection;

}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//  Parser for Microchip's "DEV"-files   (since 2005-03, for PIC18F + dsPIC)
//---------------------------------------------------------------------------

// Counters for LOADING *.DEV - FILE :
uint16_t PicDev_wCfgBitInfoCount;     // counts "cfgbits" "field"    definitions
uint16_t PicDev_wCfgBitInfoIndex;     // current index into PicDev_ConfigBitInfo[]
uint16_t PicDev_wCfgBitSettingCount;  // counts "cfgbits" "setting"  definitions
uint16_t PicDev_wCfgBitSettingIndex;  // current index into PicDev_CfgBitSettings[]
uint16_t PicDev_wNrOfSettingsInField; // number of "bit settings" in the current "field"

enum McDev_Token   // not "Mäc Dev", but Microchip DEV-file tokens :
{ McDev_TOK_NONE,  // token for "end of table" or "not recognized"

  // "Top Level" tokens .
  McDev_TOK_vpp,      McDev_TOK_vdd,
  McDev_TOK_pgming,   McDev_TOK_pgmmem,
  McDev_TOK_eedata,   McDev_TOK_testmem,  McDev_TOK_emulreg,
  McDev_TOK_cfgmem,   McDev_TOK_devid,    McDev_TOK_bkbgvectmem,
  McDev_TOK_NumBanks, McDev_TOK_xymem,    McDev_TOK_ymem,
  McDev_TOK_sfr,      McDev_TOK_UnusedRegs,
  McDev_TOK_cfgbits,  // <<< this is the most interesting top-level section for us
  McDev_TOK_peripheral,

  // "Second Level" Tokens ..
  McDev_TOK_wait,                                             // in "pgming"
  McDev_TOK_vectors,                                          // in "pgmmem"
  McDev_TOK_ver,      McDev_TOK_variant,                      // in "devid"
  McDev_TOK_reset,    McDev_TOK_bit,      McDev_TOK_stimulus, // in "sfr"
  McDev_TOK_field,                                            // in "cfgbits"
  McDev_TOK_pinfunc,  McDev_TOK_interrupt,McDev_TOK_iopin,    // in "peripheral"..
  McDev_TOK_timers,


  // "Third Level" Tokens ...
  McDev_TOK_setting, McDev_TOK_conflict,

  // "Formal variables" in argument list ....
  McDev_TOK_range,   McDev_TOK_dflt,  McDev_TOK_dfltrange,  McDev_TOK_nominal,
  McDev_TOK_memtech, McDev_TOK_tries, McDev_TOK_lvpthresh,

  // in "pgming","wait()" : may be interesting TIMING parameters (microseconds?)
  McDev_TOK_pgm,    McDev_TOK_lvpgm, /*McDev_TOK_eedata,*/ McDev_TOK_cfg,
  McDev_TOK_userid, McDev_TOK_erase, McDev_TOK_lverase,
  // in "pgmmem" / "vectors" / "testmem" :
  McDev_TOK_region, McDev_TOK_altregion,  McDev_TOK_appregion,
  // in "devid"
  McDev_TOK_idmask, McDev_TOK_id,

  McDev_TOK_key,  McDev_TOK_addr,  McDev_TOK_unused,
  McDev_TOK_names,
  McDev_TOK_mask, McDev_TOK_desc,
  McDev_TOK_req,  McDev_TOK_value, McDev_TOK_val,
  McDev_TOK_size, McDev_TOK_width, McDev_TOK_access,
  McDev_TOK_type, McDev_TOK_scl ,
  McDev_TOK_por , McDev_TOK_mclr

};

typedef struct
{
    McDev_Token tok;
    char *pszSym;
} T_PicDevSymTable;
T_PicDevSymTable McDev_TopLevelSymbols[] =
{ { McDev_TOK_vpp,        "vpp"    },   { McDev_TOK_vdd,        "vdd"    },
    { McDev_TOK_pgming,     "pgming" },   { McDev_TOK_pgmmem,     "pgmmem" },
    { McDev_TOK_eedata,     "eedata" },   { McDev_TOK_testmem,    "testmem"},
    { McDev_TOK_emulreg,    "emulreg"},   { McDev_TOK_cfgmem,     "cfgmem" },
    { McDev_TOK_devid,      "devid"  },   { McDev_TOK_bkbgvectmem,"bkbgvectmem" },
    { McDev_TOK_NumBanks,   "NumBanks"},  { McDev_TOK_xymem,      "xymem"  },
    { McDev_TOK_sfr,        "sfr"    },   { McDev_TOK_UnusedRegs, "UnusedRegs" },
    { McDev_TOK_cfgbits,    "cfgbits"},   { McDev_TOK_peripheral, "peripheral" },
    { McDev_TOK_NONE, ""  } // <<< End Of Table
}; // end McDev_TopLevelSymbols[]

T_PicDevSymTable McDev_SubLevelSymbols[] =
{ { McDev_TOK_wait,       "wait"      },   // in "pgming"
    { McDev_TOK_vectors,    "vectors"   },   // in "pgmmem"
    { McDev_TOK_ver,        "ver"       },   // in "devid"
    { McDev_TOK_variant,    "variant"   },   // in "devid"
    { McDev_TOK_reset,      "reset"     },   // in "sfr"
    { McDev_TOK_bit,        "bit"       },   // in "sfr"
    { McDev_TOK_stimulus,   "stimulus"  },   // in "sfr"
    { McDev_TOK_field,      "field"     },   // in "cfgbits"
    { McDev_TOK_setting,    "setting"   },   // in "cfgbits field"
    { McDev_TOK_conflict,   "conflict"  },   // in "cfgbits field" (16F628)
    { McDev_TOK_pinfunc,    "pinfunc"   },   // in "peripheral"
    { McDev_TOK_interrupt,  "interrupt" },   // in "peripheral"
    { McDev_TOK_iopin,      "iopin"     },   // in "peripheral"
    { McDev_TOK_timers,     "timers"    },   // in "peripheral"
    { McDev_TOK_NONE, ""  } // <<< End Of Table
}; // end McDev_SubLevelSymbols[]

T_PicDevSymTable McDev_ArglistSymbols[] =
{ { McDev_TOK_range,      "range"     },   // in "vpp"
    { McDev_TOK_dflt,       "dflt"      },   // in "vpp"
    { McDev_TOK_dfltrange,  "dfltrange" },   // in "vdd"
    { McDev_TOK_nominal,    "nominal"   },   // in "vdd"
    { McDev_TOK_memtech,    "memtech"   },   // in "pgming"
    { McDev_TOK_tries,      "tries"     },   // in "pgming"
    { McDev_TOK_lvpthresh,  "lvpthresh" },   // in "pgming"
    { McDev_TOK_pgm,        "pgm"       },   // in "pgming wait"
    { McDev_TOK_lvpgm,      "lvpgm"     },   // in "pgming wait"
    { McDev_TOK_eedata,     "eedata"    },   // in "pgming wait"
    { McDev_TOK_cfg,        "cfg"       },   // in "pgming wait"
    { McDev_TOK_pgm,        "pgm"       },   // in "pgming wait"
    { McDev_TOK_userid,     "userid"    },   // in "pgming wait"
    { McDev_TOK_pgm,        "pgm"       },   // in "pgming wait"
    { McDev_TOK_erase,      "erase"     },   // in "pgming wait"
    { McDev_TOK_lverase,    "lverase"   },   // in "pgming wait"

    { McDev_TOK_region,     "region"    },   // in "pgmmem"
    { McDev_TOK_altregion,  "altregion" },   // in "pgmmem vectors"

    { McDev_TOK_idmask,     "idmask"    },   // in "devid"
    { McDev_TOK_id,         "id"        },   // in "devid"

    { McDev_TOK_desc,       "desc"      },   // in other sections ..
    { McDev_TOK_key ,       "key"       },
    { McDev_TOK_names,      "names"     },
    { McDev_TOK_addr,       "addr"      },
    { McDev_TOK_val ,       "val"       },
    { McDev_TOK_ymem,       "ymem"      },
    { McDev_TOK_size,       "size"      },
    { McDev_TOK_width,      "width"     },
    { McDev_TOK_access,     "access"    },
    { McDev_TOK_type,       "type"      },
    { McDev_TOK_scl ,       "scl"       },
    { McDev_TOK_por   ,     "por"       },
    { McDev_TOK_mclr  ,     "mclr"      },

    { McDev_TOK_unused,     "unused"    },    // in "cfgbits"
    { McDev_TOK_mask  ,     "mask"      },
    { McDev_TOK_req   ,     "req"       },
    { McDev_TOK_value ,     "value"     },

    { McDev_TOK_NONE, ""  } // <<< End Of Table
}; // end McDev_SubLevelSymbols[]


/***************************************************************************/
int PicDev_SkipSpaces( char **ppszSource )
{
    int iNrOfSpaces = 0;
    while (**ppszSource==' ' || **ppszSource=='\t' )
    {
        ++*ppszSource;
        ++iNrOfSpaces;
    }
    return iNrOfSpaces;
}

/***************************************************************************/
bool PicDev_SkipChar( char **ppszSource , char c )
{
    PicDev_SkipSpaces( ppszSource );
    if ( **ppszSource == c )
    {
        ++*ppszSource;
        return true;
    }
    else
    {
        return false;
    }
}

/***************************************************************************/
void PicDev_ParseString( char **ppszSource , char *pszDest, int iMaxLen )
{
    char *cp = *ppszSource;
    PicDev_SkipSpaces( &cp );
    if ( *cp=='"' )
    {
        // string in double quotes .. may include space :
        ++cp;
        while (*cp!=0 && *cp!='"')
        {
            if (iMaxLen>0)
            {
                *pszDest++ = *cp;
                --iMaxLen;
            }
            ++cp;  // important: skip the COMPLETE source string even if it's too long for the destination !
        }
        if (*cp=='"')
            ++cp;
    }
    else // stríng w/o double quotes .. space is delimiter:
    {
        while (*cp!=0 && *cp!=' ' && *cp!='\t' )
        {
            if (iMaxLen>0)
            {
                *pszDest++ = *cp;
                --iMaxLen;
            }
            ++cp;
        }
    }
    *pszDest = '\0';
    *ppszSource = cp;
} // PicDev_ParseString()

/***************************************************************************/
uint32_t PicDev_ParseDWordValue( char **ppszSource )
{
    char *cp;
    char c;
    uint32_t dwValue  = 0;
    int is_hex = 0;

    cp = *ppszSource;

    while (*cp==' ' || *cp=='\t' )
        ++cp;

    if (cp[0]=='0' && cp[1]=='x')
    {
        is_hex = 1;
        cp+=2;
    }

    if ( is_hex )
    {
        while ( ((c=*cp)>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F') )
        {
            ++cp;
            dwValue <<= 4;
            if (c>='0' && c<='9') dwValue |= (uint32_t)(c-'0');
            else if (c>='a' && c<='f') dwValue |= (uint32_t)(c-'a'+10);
            else if (c>='A' && c<='F') dwValue |= (uint32_t)(c-'A'+10);
        }
    }
    else // not hex but decimal..
    {
        while ( ((c=*cp)>='0' && c<='9'))
        {
            ++cp;
            dwValue *= 10;
            if (c>='0' && c<='9') dwValue |= (uint32_t)(c-'0');
            else if (c>='a' && c<='f') dwValue |= (uint32_t)(c-'a'+10);
            else if (c>='A' && c<='F') dwValue |= (uint32_t)(c-'A'+10);
        }
    }
    *ppszSource = cp;
    return dwValue;
} // PicDev_ParseDWordValue()

/***************************************************************************/
void PicDev_SkipStringOrNumber( char **ppszSource )
{
    char sz40Trash[44];
    PicDev_ParseString( ppszSource , sz40Trash, 40 );
}

/***************************************************************************/
McDev_Token PicDev_ParseToken( T_PicDevSymTable *pSymTab, char **ppSrc )
{
    int iSymLen;
    char *cp = *ppSrc;
    char nextchr;

    PicDev_SkipSpaces( &cp );
    while ( (pSymTab->tok != McDev_TOK_NONE ) && ((iSymLen=strlen(pSymTab->pszSym))>0 ) )
    {
        if ( strncmp( cp, pSymTab->pszSym, iSymLen ) == 0 )
        {
            nextchr = cp[iSymLen];
            if ( (nextchr>='a' && nextchr>='z') || (nextchr>='A' && nextchr>='Z') )
            {
                // no valid delimiter, don't acceppt "ex" in "example" !
            }
            else
            {
                // bingo, found a symbol...
                *ppSrc = cp + iSymLen;
                return pSymTab->tok;
            }
        }
        ++pSymTab;
    }
    return McDev_TOK_NONE;
} // end PicDev_ParseToken()


/***************************************************************************/
McDev_Token PicDev_ParseTokenAndAssignOp( T_PicDevSymTable *pSymTab, char **ppSrc )
{
    char *cp = *ppSrc;
    McDev_Token token = PicDev_ParseToken( pSymTab, &cp );
    if ( token != McDev_TOK_NONE )
    {
        PicDev_SkipSpaces( &cp );
        if ( PicDev_SkipChar( &cp,'=') )
        {
            *ppSrc = cp;
            return token;
        }
    }
    return McDev_TOK_NONE;
} // end PicDev_ParseTokenAndAssignOp()


/***************************************************************************/
uint32_t PicDev_ShiftRightForMask( uint32_t dwValue, uint32_t dwBitmask )
{
    if (dwBitmask)
    {
        while ( (dwBitmask & 1L) == 0) // don't try this loop with mask=ZERO ;-)
        {
            dwValue   >>= 1;
            dwBitmask >>= 1;
        }
    }
    return dwValue;
}


/***************************************************************************/
bool PicDev_McDev_ParseCfgbits( char **ppSrc )
// Parses everything after a "cfgbits" token .
{
    char * cp = *ppSrc;
    McDev_Token token;
    uint32_t dw;
    T_PicConfigBitInfo    * pBitGroupInfo;
    T_PicConfigBitSetting * pBitSetting;
    char sz20Key[24], sz60Descr[64];
    char sz60Text[60];
    static uint32_t dwRegAddr=0;

    sz20Key[0]=0;
    sz60Descr[0]=0;

    // Get pointers for the currently filled structures :
    if ( PicDev_wCfgBitInfoIndex >= PICDEV_MAX_CONFIG_BIT_INFOS )
        PicDev_wCfgBitInfoIndex = PICDEV_MAX_CONFIG_BIT_INFOS-1;
    pBitGroupInfo = &PicDev_ConfigBitInfo[PicDev_wCfgBitInfoIndex];
    if ( PicDev_wCfgBitSettingIndex >= PICDEV_MAX_CONFIG_BIT_SETTINGS )
        PicDev_wCfgBitSettingIndex = PICDEV_MAX_CONFIG_BIT_SETTINGS-1;
    pBitSetting = &PicDev_CfgBitSettings[PicDev_wCfgBitSettingIndex];

    // Parse everything which belongs to a "cfgbits" section.   Example:
    // cfgbits (key=FOSC addr=0xF80000 unused=0x0000)
    //     field (key=FCKSMEN mask=0xC000 desc="Clock Switching and Monitor")
    //         setting (req=0x8000 value=0x8000 desc="Sw Disabled, Mon Disabled")
    //         setting (req=0xC000 value=0x4000 desc="Sw Enabled, Mon Disabled")
    //         setting (req=0xC000 value=0x0000 desc="Sw Enabled, Mon Enabled")
    //     field (key=FOS mask=0x0300 desc="Oscillator Source")
    //        setting (req=0x0300 value=0x0300 desc="Primary Oscillator")
    //  .....
    PicDev_SkipSpaces(&cp);
    if ( PicDev_SkipChar(&cp,'(') ) // cfgbits (key=FOSC addr=0xF80000 unused=0x0000)
    {
        // Note: "cfgbits" already skipped by caller .
        // Now parse the argument list after the "cfgbits"-token :
        while ( (token=PicDev_ParseTokenAndAssignOp( McDev_ArglistSymbols,&cp)) != McDev_TOK_NONE)
        {
            switch ( token  )
            {
            case McDev_TOK_key :    // here: name of the CONFIGURATION REGISTER
                PicDev_ParseString( &cp, sz20Key, 20 );
                break;
            case McDev_TOK_addr:    // here: address of the register
                dwRegAddr = PicDev_ParseDWordValue( &cp );
                break;
            case McDev_TOK_unused:  // unused bits in this register
                PicDev_ParseDWordValue( &cp );
                break;
            default:                // something we don't want to know -> ignore
                break;
            }
        } // end while < all parameters in the argument list >
        PicDev_SkipChar(&cp, ')' );  // skip closing parenthesis of argument list
        *ppSrc = cp;
        return true;
    }
    else switch ( PicDev_ParseToken( McDev_SubLevelSymbols, &cp ) )
        {
        case McDev_TOK_field   :
            if ( PicDev_SkipChar(&cp,'(') ) // "field (key=FCKSMEN mask=0xC000 ..)"
            {
                PicDev_wNrOfSettingsInField = 0;
                PicDev_wCfgBitInfoIndex = PicDev_wCfgBitInfoCount; // OLD "count" = NEW "index"
                if ( PicDev_wCfgBitInfoIndex >= PICDEV_MAX_CONFIG_BIT_INFOS )
                    PicDev_wCfgBitInfoIndex = PICDEV_MAX_CONFIG_BIT_INFOS-1;
                pBitGroupInfo = &PicDev_ConfigBitInfo[PicDev_wCfgBitInfoIndex];
                while ( (token=PicDev_ParseTokenAndAssignOp( McDev_ArglistSymbols,&cp)) != McDev_TOK_NONE)
                {
                    switch ( token  )
                    {
                    case McDev_TOK_key :  // here: Register which contains this "bit group"
                        PicDev_ParseString( &cp, sz20Key, 20 );
                        break;
                    case McDev_TOK_desc:  // description of this "bit group"
                        PicDev_ParseString( &cp, sz60Descr, 60 );
                        break;
                    case McDev_TOK_mask:  // mask of this "bit group"
                        pBitGroupInfo->dwBitmask = PicDev_ParseDWordValue( &cp );
                        break;
                    default:
                        PicDev_SkipStringOrNumber( &cp );
                        break;
                    } // end switch < token in "field" -arglist >
                } // end while
                memset(sz60Text, 0, 60 );
                strncpy(sz60Text, sz20Key, 10 );
//              sz60Text[10] = 0;
                strcat(sz60Text," ");
                strncat(sz60Text, sz60Descr, 50 );
                pBitGroupInfo->szText = DupIso8859_1_TChar(sz60Text);
                pBitGroupInfo->dwAddress = dwRegAddr;
                pBitGroupInfo->iFunction = CFGBIT_CUSTOM;
                pBitGroupInfo->pBitCombinations = NULL; // NO IDEA if a "setting" entry will follow !
                pBitGroupInfo->fIsBuiltIn = false; // Read from file
                ++PicDev_wCfgBitInfoCount; // count for NEXT "field" definition
                // (but don't change PicDev_wCfgBitInfoIndex yet, because the
                //  following "setting" entries need to access this pBitGroupInfo too)
            }
            *ppSrc=cp;           // skip the parsed substring
            return true; // end case "cfgbits field"

        case McDev_TOK_setting :
            if ( PicDev_SkipChar(&cp,'(') )
            {
                // OLD "count" = NEW "index" :
                PicDev_wCfgBitSettingIndex = PicDev_wCfgBitSettingCount;
                if ( PicDev_wCfgBitSettingIndex >= PICDEV_MAX_CONFIG_BIT_SETTINGS )
                    PicDev_wCfgBitSettingIndex = PICDEV_MAX_CONFIG_BIT_SETTINGS-1;
                // Link from the previous to this (new) entry ?
                if ( PicDev_wNrOfSettingsInField>0
                        && PicDev_wCfgBitSettingIndex<PICDEV_MAX_CONFIG_BIT_SETTINGS)
                {
                    pBitSetting->pNext = &PicDev_CfgBitSettings[PicDev_wCfgBitSettingIndex];
                }
                pBitSetting = &PicDev_CfgBitSettings[PicDev_wCfgBitSettingIndex];
                pBitSetting->pNext = NULL;   // no link to the next "setting" yet!
                // Link from the "field"-struct to the first "setting" ?
                if ( PicDev_wNrOfSettingsInField==0)
                {
                    pBitGroupInfo->pBitCombinations = pBitSetting;
                }
                ++PicDev_wNrOfSettingsInField;

                // Now parse the argument list (in parenthesis) after the "setting" token:
                while ( (token=PicDev_ParseTokenAndAssignOp( McDev_ArglistSymbols,&cp)) != McDev_TOK_NONE)
                {
                    switch ( token  )
                    {
                    case McDev_TOK_req :  // mask for THIS config bit combination
                        dw = PicDev_ParseDWordValue( &cp );
                        dw = PicDev_ShiftRightForMask( dw, pBitGroupInfo->dwBitmask );
                        pBitSetting->dwExtraMask = dw;
                        break;
                    case McDev_TOK_value: // value for THIS config bit combination
                        dw = PicDev_ParseDWordValue( &cp );
                        dw = PicDev_ShiftRightForMask( dw, pBitGroupInfo->dwBitmask );
                        pBitSetting->dwCombiValue = dw;
                        break;
                    case McDev_TOK_desc:  // description for THIS config bit combination
                        PicDev_ParseString( &cp, sz60Text, 60 );
                        pBitSetting->szComboText = DupIso8859_1_TChar(sz60Text);
                        break;
                    default:       // ignore everything else..
                        PicDev_SkipStringOrNumber( &cp );
                        break;
                    } // end switch < token in parameter list after "setting" >
                } // end while
                ++PicDev_wCfgBitSettingCount; // count for NEXT "setting" definition
                PicDev_SkipChar(&cp,')');
            }
            *ppSrc=cp;           // skip the parsed substring
            return true; // end case "cfgbits setting"

        case  McDev_TOK_conflict:   // not clear what this means yet .
            // Judging from the indentation in PIC16F628.dev ,
            //  the "conflict" data seem to apply to the previous "setting" :
            // >cfgbits (key=CONFIG addr=0x2007 unused=0x200)
            // >  field (key=PUT mask=0x8 desc="Power Up Timer")
            // >    setting (req=0x8 value=0x8 desc="Off")
            // >	     conflict (addr=0x2007 mask=0x40 value=0x40 cfmsg=2)
            // >    setting (req=0x8 value=0x0 desc="On")
            if ( PicDev_SkipChar(&cp,'(') )
            {
                PicDev_SkipChar(&cp,')');
            }
            return true; // ignore this token

        default: // oops.. wrong token, ignore it
            // NO: *ppSrc=cp;
            break;
        }

    // NO: *ppSrc=cp;
    return false;
} // end PicDev_McDev_ParseCfgbits( char *pszSource )


/***************************************************************************/
bool PicDev_LoadMcDevFile( const wxChar *pszDevFileName )
// TRIES TO load all we have to know ;-) from one of Microchip's DEV-files .
//  ( Caution, those files are copyrighted material from Microchip,
//    their structure is not officially documented,
//    so this file parser entirely based on guesswork.. )
{
    bool fResult = false;
    T_QFile myQFile;  // "Quick File Access"-module to handle text files line-by-line
//  int iParseOptions;
    int iCurrentLineNr = 0;
    char szLine[1024];
    char *cp;
    wxString Diag;
    McDev_Token top_level_token = McDev_TOK_NONE;


    //  Open the file to see what's in it ...
    if ( myQFile.QFile_Open( pszDevFileName, QFILE_O_RDONLY ) )
    {
        // prepare import of "cfgbits".."setting" :
        PicDev_wCfgBitInfoCount = 0;   // counts "cfgbits field"    definitions
        PicDev_wCfgBitInfoIndex = 0;   // current index into PicDev_ConfigBitInfo[]
        PicDev_wCfgBitSettingCount=0;  // counts "cfgbits" "setting"  definitions
        PicDev_wCfgBitSettingIndex=0;  // current index into PicDev_CfgBitSettings[]
        PicDev_wNrOfSettingsInField=0; // number of "bit settings" in the current "field"

        // read all lines from the file ...
        while ( myQFile.QFile_ReadLine( szLine, 1020 ) >= 0 )
        {
            // Parse the input line .  First check for "top-level" tokens (in column 0)
            cp = szLine;
            if ( (cp[0]>='a' && cp[0]<='z') || (cp[0]>='A' && cp[0]<='Z') )
            {
                top_level_token = PicDev_ParseToken( McDev_TopLevelSymbols, &cp );
            }
            // Note: one TOP-LEVEL section may extend over several lines,
            //  which begin with SPACES or TAB characters then ....
            switch (top_level_token)
            {
            case McDev_TOK_vpp    :
            case McDev_TOK_vdd    :
            case McDev_TOK_pgming :
            case McDev_TOK_pgmmem :
            case McDev_TOK_eedata :
            case McDev_TOK_testmem:
            case McDev_TOK_emulreg:
            case McDev_TOK_cfgmem :
            case McDev_TOK_devid  :
            case McDev_TOK_bkbgvectmem:
            case McDev_TOK_NumBanks:
            case McDev_TOK_xymem  :
            case McDev_TOK_sfr    :
            case McDev_TOK_UnusedRegs:
            case McDev_TOK_peripheral:
                break; // don't care for all these sections !

            case McDev_TOK_cfgbits:
                PicDev_McDev_ParseCfgbits( &cp );

            default:    // also don't care for all "unknown" sections
                break;
            } // end switch(token)


            ++iCurrentLineNr;
        }
        myQFile.QFile_Close();  // close the hex-file again
        // Only "success" if at least ONE config bit info was loaded:
        fResult = (PicDev_wCfgBitInfoCount > 0);

        if ( fResult )
        {
            Diag.Printf(_("Parsed \"%s\" : found %d bit combinations in %d configuration bit groups ."),
                        pszDevFileName, (int)PicDev_wCfgBitSettingCount, (int)PicDev_wCfgBitInfoCount );
        }
        else
        {
            Diag.Printf(_("Couldn't parse Microchip's \"%s\", incompatible format ?"), pszDevFileName );
        }
    } // end if < file could be opened >
    else
    {
        Diag.Printf(_("Couldn't find \"%s\", please borrow/copy from MPLAB !"), pszDevFileName );
    }

    APPL_ShowMsg( APPL_CALLER_PIC_PRG, 0, Diag.c_str() );

    return fResult;
} // end PicDev_LoadMcDevFile()


/***************************************************************************/
bool PicDev_FillConfigBitInfoTable( T_PicDeviceInfo *psrcPicDeviceInfo )
// Results are placed in PicDev_ConfigBitInfo[] .
// Return value: 0=problem, using DEFAULT (built-in table)
//               1=ok, found the specified info file and loaded it.
{
    int i, iFunction;
    bool table_loaded = false;
//    wxChar sz355DevFileName[356];
    wxFileName DevFilename;
    wxChar *cp;


    // First clear the old "config bit info table" :
    for (i=0;i<PICDEV_MAX_CONFIG_BIT_INFOS;++i)
    {
        if (PicDev_ConfigBitInfo[i].szText != NULL)
        {
            if (!PicDev_ConfigBitInfo[i].fIsBuiltIn)
                free ((void*)PicDev_ConfigBitInfo[i].szText);
            PicDev_ConfigBitInfo[i].szText = NULL;
        }
    }

    // If one of Microchip's device-definition files (*.DEV) exists,
    //  use the config bit definitions from that file (instead of "our own").
    // First look at the file extension to find out how it can be loaded..
    if ( psrcPicDeviceInfo->sz80ConfigRegisterInfoFile[0] > 32 )
    {
        cp = _tcsrchr(psrcPicDeviceInfo->sz80ConfigRegisterInfoFile,'.');
        if ( cp )
        {
            if (_tcsicmp(cp, _T(".dev")) == 0)
            {
                // it MAY be one of Microchip's "dev"-files...
                // WinPic once expected these files IN ITS OWN SUB-FOLDER "DEVICES",
                //   but since 2005-10 it is possible to specify the path into
                //   Microchip's MPLAB DEVICE folder (so the dev-files don't necessarily
                //   have to be copied, though I recommend copying them because who knows
                //   if they don't change the file formats in future ! )
                if ( Config.sz255MplabDevDir[0] != 0 )
                    DevFilename.AssignDir(Config.sz255MplabDevDir);
                else
                {
                    DevFilename.Assign(wxStandardPaths::Get().GetExecutablePath());
                    DevFilename.AppendDir(_T("Devices"));
                }
                DevFilename.SetFullName(psrcPicDeviceInfo->sz80ConfigRegisterInfoFile);
                wxString DevFilenameText = DevFilename.GetFullPath();
                table_loaded = PicDev_LoadMcDevFile(DevFilenameText.c_str());
            }
        }
    } // end if < Config-Register info file specified in DEVICE-TABLE >

    // If no *.DEV-file could be loaded for this PIC device,
    //  use one of the "built-in" info tables
    if ( ! table_loaded )
    {
        if ( psrcPicDeviceInfo->lConfWordAdr == 0x002007 )
        {
            // here for PIC16Fxx(x) & Co ...
            for (i=0;i<PICDEV_MAX_CONFIG_BIT_INFOS;++i)
            {
                PicDev_ConfigBitInfo[i] = BuiltInConfigBitInfo_PIC16F[i];
                if ( BuiltInConfigBitInfo_PIC16F[i].szText == NULL )
                    break;
            }
        }
        else if ( psrcPicDeviceInfo->lConfWordAdr == 0x0F80000 )
        {
            // here for dsPIC30F ...
            for (i=0;i<PICDEV_MAX_CONFIG_BIT_INFOS;++i)
            {
                PicDev_ConfigBitInfo[i] = BuiltInConfigBitInfo_dsPIC30F[i];
                if ( BuiltInConfigBitInfo_dsPIC30F[i].szText == NULL )
                    break;
            }
        }
        // For PIC16F family only :  Try to convert info from "old" tables into "new" structs:
        if ( psrcPicDeviceInfo->lConfWordAdr == 0x002007 )
        {
            // Copy all device-specific bitmasks for the config word
            // into the "built-in" info table, to stay compatible
            // with the built-in support for old PICs in older versions of WinPic
            // (where no extra CONFIG REGISTER INFO DATABASE was used) .
            // These values MAY be "overloaded" (from a file) in a later version of WinPic.
            for (i=0;i<PICDEV_MAX_CONFIG_BIT_INFOS;++i)
            {
                if ( PicDev_ConfigBitInfo[i].szText != NULL )
                {
                    iFunction = PicDev_ConfigBitInfo[i].iFunction;
                    PicDev_ConfigBitInfo[i].pBitCombinations = NULL;
                    switch (iFunction)
                    {
                    case CFGBIT_OSC_SEL : // oscillator selection bits, various "schemes"...
                        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_oscillator;
                        if ( PicDev_ConfigBitInfo[i].dwBitmask == 0x0003 )
                            PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_Osc03;
                        else if ( PicDev_ConfigBitInfo[i].dwBitmask == 0x0007 )
                            PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_Osc07;
                        else if ( PicDev_ConfigBitInfo[i].dwBitmask == 0x0013 )
                            PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_Osc13;
                        else
                            PicDev_ConfigBitInfo[i].pBitCombinations = NULL;
                        break;
                    case CFGBIT_CP    :  // Code protection, general
                        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_cpbits;
                        PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_LowActiveOnOff;
                        break;
                    case CFGBIT_CPD   :  // Code protection for "data"
                        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_cpd;
                        PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_LowActiveOnOff;
                        break;
                    case CFGBIT_PWRTEN:  // power-up timer ENable
                        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_pwrte;
                        if ( psrcPicDeviceInfo->wCfgmask_inv_pwrte )
                            PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_LowActiveEnable;
                        else
                            PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable;
                        break;
                    case CFGBIT_WDTEN :  // watchdog timer ENable
                        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_wdte;
                        PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable;
                        break;
                    case CFGBIT_LVPEN :  // low voltage programming enable
                        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_lvp;
                        PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable;
                        // Note: For 16F628, 0x2007, bit 7: LVP: Low Voltage Programming Enable bit
                        // > 1 = RB4/PGM pin has PGM function, Low Voltage Programming enabled
                        // > 0 = RB4/PGM is digital I/O, HV on MCLR must be used for programming
                        break;
                    case CFGBIT_BOREN :  // brown-out reset enable
                        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_boden;
                        PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable;
                        break;
                    case CFGBIT_BORV  :  // brown-out voltage select
                        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_borv;
                        break;
                    case CFGBIT_MCLREN:  // memory clear enable  ("Reset-input enable")
                        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_mclre;
                        PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_HighActiveEnable;
                        break;
                    case CFGBIT_DDEBUG:  // debugging enable
                        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_ddebug;
                        PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_LowActiveOnOff;
                        break;
                    case CFGBIT_WRCODE:  // code write enable
                        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_wrcode;
                        break;
                        //   case CFGBIT_BGCAL0:  // bandgap calibration bit #0 (TWO bits in PIC12F629)
                        //      PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_bandgap;
                        //        break;
                        //   case CFGBIT_BGCAL1:  // bandgap calibration bit #1
                        //        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo-> ? ;
                        //        break;
                    case CFGBIT_CCPMX0:  // Capture/Compare pin selection (used for PIC16F818)
                        PicDev_ConfigBitInfo[i].dwBitmask = psrcPicDeviceInfo->wCfgmask_ccpmx;
                        break;
                    default:  // special function, must be very device-specific !
                        break;
                    } // end switch(iFunction)

                    if ( PicDev_ConfigBitInfo[i].pBitCombinations == NULL )
                    {
                        // If no suitable COMBO LIST found yet, and it's a single bit,
                        // use the universal "0" / "1" - combo for this thingy (whatever it is):
                        if ( PicDev_BinaryCrossTotal( PicDev_ConfigBitInfo[i].dwBitmask) == 1 )
                        {
                            PicDev_ConfigBitInfo[i].pBitCombinations = (P_PicConfigBitSetting)PicDev_BitCombi_OneOrZero;
                        }
                    }
                }
                else // PicDev_ConfigBitInfo[i].sz60Text[0] == 0   -> end of the list !
                    break;
            } // end for(i=0;i<PICDEV_MAX_CONFIG_BIT_INFOS;++i) to copy the old CONFIG WORD BITMASKS
        } // end if( psrcPicDeviceInfo->lConfWordAdr == 0x002007 )  ~~~ for PIC16F family only !
    } // end else < no *.DEV-file loaded >

    return table_loaded;
} // end PicDev_FillConfigBitInfoTable()

/***************************************************************************/
int PicDev_GetBuiltInDeviceInfoByIndex(int iTableIndex, T_PicDeviceInfo *pDstDeviceInfo )
// return : >= 0 means "ok",   <0 means "error"
{
    int iCountOfBuiltInDevices = sizeof(BuiltInPicDeviceInfo) / sizeof(T_PicDeviceInfo);

    if (iTableIndex<0)
        return -1;   // complete nonsense, dear caller !

    if (APPL_i32CustomizeOptions & APPL_CUST_NO_INTERNAL_DEVICES)
        return -1;   // shall not use the internal device list (by order of the prophet)


    if (iTableIndex<iCountOfBuiltInDevices)
    {
        *pDstDeviceInfo = BuiltInPicDeviceInfo[iTableIndex];
        return iTableIndex;  // returns info from built-in PIC device table
    }

    return -2;        // not in list
} // end PicDev_GetBuiltInDeviceInfoByIndex()

/***************************************************************************/
int PicDev_GetDeviceInfoByIndex(int iTableIndex,
                                T_PicDeviceInfo    *pDstDeviceInfo )
// return : >= 0 means "ok",   <0 means "error"
// Side effect: PicDev_ConfigBitInfo[] may be loaded
//              depending on the new device .
{
    int iResult;
    char sz80DeviceName[84];
// T_PicDeviceInfo MyDeviceInfo;
// char sz80[81];
// int  i;
    int iFileTableIndex;
    int iCountOfBuiltInDevices = sizeof(BuiltInPicDeviceInfo) / sizeof(T_PicDeviceInfo);


    if (APPL_i32CustomizeOptions & APPL_CUST_NO_INTERNAL_DEVICES)
        iCountOfBuiltInDevices = 0;  // shall not use the internal device list ?

    if (iTableIndex < iCountOfBuiltInDevices)
    {
        iResult = PicDev_GetBuiltInDeviceInfoByIndex(iTableIndex, pDstDeviceInfo );
        // T_PicConfigBitInfo
        return iResult;
    }

    iFileTableIndex = iTableIndex-iCountOfBuiltInDevices;  // secondary index to load from FILE..


    // Retrieve the device name from the device database file
    //  (which actually was an INI-file once, which grew too large .. > 64 kByte)
    if ( PicDev_GetDeviceNameFromFileByIndex( iFileTableIndex,
            sz80DeviceName, 80 ) ) // output: name of a PIC DEVICE
    {
        // definitions for this device exist in the device file -> load them...
        if ( PicDev_LoadPicDeviceDefinitionFromFile(
                    sz80DeviceName,  // [in] name of a PIC DEVICE (not a filename!)
                    pDstDeviceInfo,  // [out] destination structure (to be filled)
                    false) ) // [in] fMayShowInfo ? true=may show info from section header
        {
            return iTableIndex;
        }
    }

    return -2;        // not in list
} // end PicDev_GetDeviceInfoByIndex()



/***************************************************************************/
int PicDev_GetDeviceInfoByName(const char *pszDeviceName,
                               T_PicDeviceInfo     *pDstDeviceInfo)
// returns a table index (0..n) if successfull, negative value otherwise
{
    int i,iFileTableIndex;
    int iCountOfBuiltInDevices = sizeof(BuiltInPicDeviceInfo) / sizeof(T_PicDeviceInfo);

    if (APPL_i32CustomizeOptions & APPL_CUST_NO_INTERNAL_DEVICES)
        iCountOfBuiltInDevices = 0;  // shall not use the internal device list ?

    // FIRST try device definition file. Reason: If there are wrong entries in the
    //  built-in device table, the user of WinPic may put things right by adding the
    //  correct definitions to the database in DEVICES.INI  .
    i =  PicDev_SearchDeviceDefInFile( pszDeviceName );  // returns < 0 if not found, index otherwise
    if ( i>=0 )
    {
        iFileTableIndex = i+iCountOfBuiltInDevices;
        APPL_ShowMsg(APPL_CALLER_MAIN,0,
                     _("Info: Loading definitions for \"%hs\" from %s .") ,
                     pszDeviceName, PicDev_GetDeviceFileName() );
        if ( PicDev_LoadPicDeviceDefinitionFromFile(
                    pszDeviceName, // name of a new PIC DEVICE (not a filename!)
                    pDstDeviceInfo, // destination structure (to be filled)
                    true ) ) // fMayShowInfo ? true=may show info from section header
        {
            return iFileTableIndex;   // returns a 'logic' device table index
        }
    }

    if (APPL_i32CustomizeOptions & APPL_CUST_NO_INTERNAL_DEVICES)
    {
        // shall not use the internal device list ?
        return -1;  // greetings to Danmark !
    }


    // Not found in the file-based device list. Try the built-in devices:
    for (i=0; i<iCountOfBuiltInDevices; ++i)
    {
        if (BuiltInPicDeviceInfo[i].iCheck73!=73)
        {
            APPL_ShowMsg(APPL_CALLER_MAIN, 0, _("Error in built-in PIC device list, \"%hs\"."),
                         BuiltInPicDeviceInfo[i].sz40DeviceName );
        }

        if (strcmp(BuiltInPicDeviceInfo[i].sz40DeviceName, pszDeviceName)==0)
        {
            *pDstDeviceInfo = BuiltInPicDeviceInfo[i];
            return i;   // device table index
        }
    }

    return -1;  // not found
} // end PicDev_GetDeviceInfoByName()

/***************************************************************************/
const char *PicDev_GetDeviceNameByIdWord( uint16_t wDeviceIdWord, int iBitsPerInstruction )
// returns an EMPTY string if unknown
// Because of a device ID collosion between certain PIC16F's and PIC18F's
// (like PIC16F630 and PIC18F4220), this routine also needs to know
// the "core family" (=number of bits per instruction word; 14,16,24, ..).
{
    static T_PicDeviceInfo MyDeviceInfo;
    int i=0;
    if (   wDeviceIdWord==0x0000
            || wDeviceIdWord==0xFFFF
            || wDeviceIdWord==0x3FFF )
        return "";    // not "unknown" but "nothing read"

    // Does the CURRENTLY LOADED device info match the device ID ?
    //  (if so, don't waste any time to search the lists)
    if ( (wDeviceIdWord & PIC_DeviceInfo.lDeviceIdMask) ==
            (PIC_DeviceInfo.lDeviceIdValue & PIC_DeviceInfo.lDeviceIdMask) )
        return PIC_DeviceInfo.sz40DeviceName;

    // Arrived here, it must be "some other chip" ... which one ?
    while ( PicDev_GetDeviceInfoByIndex( i, &MyDeviceInfo ) >= 0 )
    {
        if ( ( (wDeviceIdWord & MyDeviceInfo.lDeviceIdMask) ==
                (MyDeviceInfo.lDeviceIdValue & MyDeviceInfo.lDeviceIdMask) )
                && (iBitsPerInstruction == MyDeviceInfo.iBitsPerInstruction) )
        {
            if (MyDeviceInfo.lDeviceIdValue != 0)
                return MyDeviceInfo.sz40DeviceName;
        }
        ++i;
    } // end while

    return "";

} // end PicDev_GetDeviceNameByIdWord()

/***************************************************************************/
void PicDev_FillDefaultDeviceInfo(T_PicDeviceInfo *pDeviceInfo)
{
    // Set "default" values which MAY be usable for MOST PICs,
    //     (may be overwritten later by reading from file, etc)
    memset( pDeviceInfo, 0, sizeof(T_PicDeviceInfo) );  // cleanup
    pDeviceInfo->iCodeMemType = PIC_MT_FLASH;
    pDeviceInfo->iBitsPerInstruction = 14;
    pDeviceInfo->lCodeMemSize = 1024;
    pDeviceInfo->lDataEEPROMSizeInByte = 64;
    pDeviceInfo->lInternalRAMSize= 64;
    pDeviceInfo->lTi_Clock_us = 1;      // 1 microsecond per clock pulse
    pDeviceInfo->lTi_Prog_us  = 10000;  // 10 ms after "begin programming"
    pDeviceInfo->lTi_Erase_us = 10000;  // 10 ms after "begin programming"
    pDeviceInfo->lAddressOscCal = -1; // address of the oscillator calibration word: none
    pDeviceInfo->lDeviceIdAddr  = 0x2006;
    pDeviceInfo->lDeviceIdMask  = 0x3FE0;
    pDeviceInfo->lDeviceIdValue = 0;  // device ID value unknown
    pDeviceInfo->wCfgmask_bandgap= 0x0000;  // no bandgap calibration bits available
    pDeviceInfo->wEraseAlgo   = PIC_ALGO_16FXX;
    pDeviceInfo->wCodeProgAlgo= PIC_ALGO_16FXX;
    pDeviceInfo->wConfigProgAlgo=PIC_ALGO_16FXX;
    pDeviceInfo->wDataProgAlgo= PIC_ALGO_16FXX;

    // Set default Serial mode commands. Originally only used for PIC16C84 + 16F84,
    // but seemed to be valid for 16F628, 12F629/675 also .
    pDeviceInfo->iCmd_LoadConfig = 0;  // once 0
    pDeviceInfo->iCmd_LoadProg   = 2;  // once 2
    pDeviceInfo->iCmd_ReadProg   = 4;  // once 4
    pDeviceInfo->iCmd_IncrAddr   = 6;  // once 6
    pDeviceInfo->iCmd_BeginProg  = 8;  // once 8
    // The following commands only apply to PICs with EPROM code memory,
    //   like the 16C71, 16C711 etc
    pDeviceInfo->iCmd_EndProg   = 14;  //  14  for 16C71, 16C711 etc

    // The following commands only apply to PICs with DATA EEPROM Memory,
    //  (so not the 16C71, 16C711..) :
    pDeviceInfo->iCmd_LoadDataDM = 3;  //  3
    pDeviceInfo->iCmd_ReadDataDM = 5;  //  5
    // The following commands only apply to PICs with FLASH CODE Memory,
    //  (which is all 16Fxxx-types, but no 16Cxx(x)'s except for the 16C84):
    pDeviceInfo->iCmd_EraseProg  = 9;   //  9, also called "bulk erase"
    pDeviceInfo->iCmd_EraseData  = 11;  // 11
} // end PicDev_FillDefaultDeviceInfo()


void WriteValue (wxFile &File, const char *Key, const char *Value)
{
    File.Write (Key, strlen(Key));
    File.Write ("=", 1);
    File.Write (Value, strlen(Value));
    File.Write ("\n", 1);
}
void WriteIntValue (wxFile &File, const char *Key, int Value)
{
    char Buf[10];
    _itoa(Value, Buf, 10);
    File.Write (Key, strlen(Key));
    File.Write ("=", 1);
    File.Write (Buf, strlen(Buf));
    File.Write ("\n", 1);
}
void WriteHeader (wxFile &File, const char *Header)
{
    File.Write ("[", 1);
    File.Write (Header, strlen(Header));
    File.Write ("]\n", 2);
}

//---------------------------------------------------------------------------
void PicDev_DumpDeviceListToFile( const wxChar *pszDumpFileName )
{
// originally just a "test function" but may be interesting for the user..
    wxString s;
    T_PicDeviceInfo MyDeviceInfo;
    char sz80[81];
    int  i;

    wxFileName DumpFilename(wxStandardPaths::Get().GetExecutablePath());
    DumpFilename.SetFullName(pszDumpFileName);
    s = DumpFilename.GetFullPath();
    DeleteFile( s );  // make sure we write into a "clean" file. No old scrap !
    APPL_ShowMsg(APPL_CALLER_MAIN,0,_("Device list dumped to \"%s\""),s.c_str());
    wxFile IniFile(s, wxFile::write);
    WriteHeader(IniFile, "Info");
    WriteValue(IniFile,"i1","Dump of built-in device info table");
    sprintf(sz80,"Generated by WinPic, compiled %s",__DATE__);
    WriteValue(IniFile,"i2",sz80);
    WriteValue(IniFile,"i3"," You may copy & paste a single section");
    WriteValue(IniFile,"i4"," to produce a new PIC definition in the");
    WriteValue(IniFile,"i5"," file DEVICES.INI, which must be placed");
    WriteValue(IniFile,"i6"," in the WinPic directory.");
    WriteValue(IniFile,"i7"," Dont forget the entry under [Devices].");
    WriteValue(IniFile,"i8","");
    WriteValue(IniFile,"i9","");
    WriteHeader(IniFile, "Devices");                 // produce an index of all devices

    for (i=0; PicDev_GetBuiltInDeviceInfoByIndex( i/*iTableIndex*/, &MyDeviceInfo ) >= 0; ++i)
    {
        char sz7[8];
        sprintf(sz7, "dev%d", i);
        WriteValue(IniFile, sz7, MyDeviceInfo.sz40DeviceName);
    }


    for (i=0; PicDev_GetBuiltInDeviceInfoByIndex( i/*iTableIndex*/, &MyDeviceInfo ) >= 0; ++i)
    {
        WriteHeader(IniFile, MyDeviceInfo.sz40DeviceName);                 // produce an index of all devices
        WriteValue(IniFile,"DeviceName",MyDeviceInfo.sz40DeviceName);
        wxCharBuffer FileName = wxString(MyDeviceInfo.sz80ConfigRegisterInfoFile).mb_str(wxConvISO8859_1);
        WriteValue(IniFile,"DeviceInfoFileName", FileName);
        WriteIntValue(IniFile,"CodeMemType",MyDeviceInfo.iCodeMemType);
        WriteIntValue(IniFile,"CodeMemBitsPerInst",MyDeviceInfo.iBitsPerInstruction);
        WriteIntValue(IniFile,"CodeMemSize",MyDeviceInfo.lCodeMemSize);
        WriteIntValue(IniFile,"CodeMemWriteLatchSize",MyDeviceInfo.lCodeMemWriteLatchSize);
        WriteIntValue(IniFile,"CodeMemEraseLatchSize",MyDeviceInfo.lCodeMemEraseLatchSize);
        WriteIntValue(IniFile,"DataEEPROMSize",MyDeviceInfo.lDataEEPROMSizeInByte);
        WriteIntValue(IniFile,"InternalRAMSize",MyDeviceInfo.lInternalRAMSize);

        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.lConfMemBase);
        WriteValue(IniFile,"AddrConfigMem",sz80);
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.lConfWordAdr);
        WriteValue(IniFile,"AddrConfigWord",sz80);
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.lDataMemBase);
        WriteValue(IniFile,"AddrDataMem",sz80);
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.lAddressOscCal);
        WriteValue(IniFile,"AddrOscCalib",sz80);

        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.dwConfMemUsedLocations0);
        WriteValue(IniFile,"ConfigMemUsedLocs00_1F",sz80);
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.dwConfMemUsedLocations2);
        WriteValue(IniFile,"ConfigMemUsedLocs20_3F",sz80);

        sprintf(sz80,"0x%06lX",(long)MyDeviceInfo.lIdMemBase);
        WriteValue(IniFile,"IdMemoryBase",sz80);
        WriteIntValue(IniFile,"IdMemorySize",MyDeviceInfo.lIdMemSize);

        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.lDeviceIdAddr);
        WriteValue(IniFile,"DeviceIdAddr",sz80);
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.lDeviceIdMask);
        WriteValue(IniFile,"DeviceIdMask",sz80);
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.lDeviceIdValue);
        WriteValue(IniFile,"DeviceIdValue",sz80);
        WriteIntValue(IniFile,"CanRead", (int)MyDeviceInfo.wCanRead);  // modified 2004-02-18

        WriteValue(IniFile,"EraseAlgo",     PicDev_AlgorithmCodeToString(MyDeviceInfo.wEraseAlgo) );
        WriteValue(IniFile,"CodeProgAlgo",  PicDev_AlgorithmCodeToString(MyDeviceInfo.wCodeProgAlgo) );
        WriteValue(IniFile,"ConfigProgAlgo",PicDev_AlgorithmCodeToString(MyDeviceInfo.wConfigProgAlgo) );
        WriteValue(IniFile,"DataProgAlgo",  PicDev_AlgorithmCodeToString(MyDeviceInfo.wDataProgAlgo) );

        WriteValue(IniFile,"VppVddSequence",PicDev_VppVddSequenceCodeToString(MyDeviceInfo.wVppVddSequence) );

        WriteIntValue(IniFile,"Ti_Clock_us",MyDeviceInfo.lTi_Clock_us);
        WriteIntValue(IniFile,"Ti_Prog_us",MyDeviceInfo.lTi_Prog_us);
        WriteIntValue(IniFile,"Ti_Erase_us",MyDeviceInfo.lTi_Erase_us);

        WriteIntValue(IniFile,"CmdLoadConfig",MyDeviceInfo.iCmd_LoadConfig );
        WriteIntValue(IniFile,"CmdLoadProg",MyDeviceInfo.iCmd_LoadProg );
        WriteIntValue(IniFile,"CmdReadProg",MyDeviceInfo.iCmd_ReadProg );
        WriteIntValue(IniFile,"CmdIncrAddr",MyDeviceInfo.iCmd_IncrAddr );
        WriteIntValue(IniFile,"CmdBeginProg",MyDeviceInfo.iCmd_BeginProg );
        WriteIntValue(IniFile,"CmdEndProg",MyDeviceInfo.iCmd_EndProg );
        WriteIntValue(IniFile,"CmdLoadData",MyDeviceInfo.iCmd_LoadDataDM );
        WriteIntValue(IniFile,"CmdReadData",MyDeviceInfo.iCmd_ReadDataDM );
        WriteIntValue(IniFile,"CmdEraseProg",MyDeviceInfo.iCmd_EraseProg );
        WriteIntValue(IniFile,"CmdEraseData",MyDeviceInfo.iCmd_EraseData );
        WriteIntValue(IniFile,"CmdEraseChip",MyDeviceInfo.iCmd_EraseChip );
        WriteIntValue(IniFile,"CmdBeginProgNoErase",MyDeviceInfo.iCmd_BeginProgNoErase);

        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_unused);
        WriteValue(IniFile,"Cfgmask_unused",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_unknown);
        WriteValue(IniFile,"Cfgmask_unknown",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_cpbits);
        WriteValue(IniFile,"Cfgmask_cpbits",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_cpd);
        WriteValue(IniFile,"Cfgmask_cpd",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_pwrte);
        WriteValue(IniFile,"Cfgmask_pwrte",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_inv_pwrte);
        WriteValue(IniFile,"Cfgmask_inv_pwrte",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_wdte);
        WriteValue(IniFile,"Cfgmask_wdte",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_oscillator);
        WriteValue(IniFile,"Cfgmask_oscillator",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_osc_rc);
        WriteValue(IniFile,"Cfgmask_osc_rc",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_osc_hs);
        WriteValue(IniFile,"Cfgmask_osc_hs",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_osc_xt);
        WriteValue(IniFile,"Cfgmask_osc_xt",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_osc_lp);
        WriteValue(IniFile,"Cfgmask_osc_lp",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_osc_extclk);
        WriteValue(IniFile,"Cfgmask_osc_extclk",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_osc_intrc_noclkout);
        WriteValue(IniFile,"Cfgmask_osc_intrc_noclkout",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_osc_intrc_clkout);
        WriteValue(IniFile,"Cfgmask_osc_intrc_clkout",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_osc_extrc_noclkout);
        WriteValue(IniFile,"Cfgmask_osc_extrc_noclkout",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_lvp);
        WriteValue(IniFile,"Cfgmask_lvp",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_boden);
        WriteValue(IniFile,"Cfgmask_boden",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_mclre);
        WriteValue(IniFile,"Cfgmask_mclre",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_ddebug);
        WriteValue(IniFile,"Cfgmask_ddebug",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_wrcode);
        WriteValue(IniFile,"Cfgmask_wrcode",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_bandgap);
        WriteValue(IniFile,"Cfgmask_bandgap",sz80 );
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask_ccpmx);
        WriteValue(IniFile,"Cfgmask_ccpmx",sz80 );  // speciality 16F818

        // bitmasks for the SECOND config word since 2003-12 (for PIC16F88)
        sprintf(sz80,"0x%4.4lX",(long)MyDeviceInfo.wCfgmask2_used);
        WriteValue(IniFile,"Cfgmask2_used_bits",sz80 );

    } // end while

}
//---------------------------------------------------------------------------


