/*-------------------------------------------------------------------------*/
/* PIC_PRG.h                                                               */
/*     Part of Wolfgang Buescher's  simple PIC-Programmer for Windows.     */
/*-------------------------------------------------------------------------*/

#include "Devices.h" // database with PIC device definitions + access routines

#ifdef __cplusplus      /* to call these routines from CPP modules : */
 #define CPROT extern "C"
 // Note: It's 'extern "C"' , not 'extern "c"' as stated in
 //       Borland's bugged help system ! (at least for BCB4)
#else
 #define CPROT          /* to call these routines from ordinary "C": */
#endif  /* nicht "cplusplus" */


#ifndef WORD
 #define WORD unsigned short
#endif


/*------------- Constants and other definitions ---------------------------*/


  // Constant definitions for the supported PIC DEVICE types.
  //   Be sure to update the items in the "PartName" combo of the
  //   user interface when you introduce new PIC device types here !!
#define PIC_DEV_TYPE_UNKNOWN 0
#define PIC_DEV_TYPE_16C84   1
#define PIC_DEV_TYPE_16C61   2
#define PIC_DEV_TYPE_16C71   3
#define PIC_DEV_TYPE_16C710  4
#define PIC_DEV_TYPE_16C711  5
#define PIC_DEV_TYPE_16C715  6
#define PIC_DEV_TYPE_12F629  7
#define PIC_DEV_TYPE_12F675  8
#define PIC_DEV_TYPE_16F73   9
#define PIC_DEV_TYPE_16F74   10
#define PIC_DEV_TYPE_16F76   11
#define PIC_DEV_TYPE_16F77   12
#define PIC_DEV_TYPE_16F84   13
#define PIC_DEV_TYPE_16F627  14
#define PIC_DEV_TYPE_16F628  15
#define PIC_DEV_TYPE_16F870  16
#define PIC_DEV_TYPE_16F871  17
#define PIC_DEV_TYPE_16F872  18
#define PIC_DEV_TYPE_16F873  19
#define PIC_DEV_TYPE_16F874  20
#define PIC_DEV_TYPE_16F876  21
#define PIC_DEV_TYPE_16F877  22

#define PIC_NR_OF_SUPPORTED_DEVICES 22

   // values for T_PicDeviceInfo.iCodeMemType :
#define PIC_MT_EPROM   0
#define PIC_MT_FLASH   1

   // possible values for iPicPrgAction (parameter in a some "new" routines)
   //  Same defs in PIC10F_PRG.H and PIC_PRG.H !
#define PIC_ACTION_NONE        0
#define PIC_ACTION_READ        1
#define PIC_ACTION_BLANK_CHECK 2
#define PIC_ACTION_VERIFY      4
#define PIC_ACTION_ERASE       8
#define PIC_ACTION_WRITE      16


   // Erase Options for routine 'PIC_PRG_Erase(int iEraseOptions)' :
#define PIC_ERASE_CODE       0x01
#define PIC_ERASE_DATA       0x02
#define PIC_ERASE_CONFIG     0x04
#define PIC_ERASE_ALL        0x07
#define PIC_SAVE_CALIBRATION 0x80

  // Different types of programming- & erase-algorithms for different PICs :
#define PIC_ALGO_UNKNOWN  0
#define PIC_ALGO_16FXX    1   /* CODE MEMORY programming algorithm for many older PICs  */
#define PIC_ALGO_12FXX    2   /* CODE MEMORY programming algorithm for 12F629, 12F675   */
#define PIC_ALGO_16F81X   3   /* CODE MEMORY programming algorithm for 16F818, 16F819   */
#define PIC_ALGO_16F87X   4   /* CODE MEMORY programming algorithm for 16F87x without A */
#define PIC_ALGO_16F87XA  5
#define PIC_ALGO_16CXX    6   /* old EPROM-based PICs (no FLASH code memory) */
#define PIC_ALGO_ERASE_16F62XA     7  /* added 2004-01-28; 16F628A is not compatible with 16F628 */
#define PIC_ALGO_ERASE_16F81X      8  /* ridiculously complicated algorithm to erase a 16F818    */
#define PIC_ALGO_ERASE_16F87X      9  /* similar, also ridiculous algorithm to erase a 16F87X    */
#define PIC_ALGO_ERASE_16F87XA     10 /* almost the same erase algorithm    for a 16F87XA        */
#define PIC_ALGO_DATA_EEPROM_16XXX 11 /* DATA EEPROM MEMORY programming algorithm for many PICs  */
#define PIC_ALGO_CONFIG_MEM_16XXX  12 /* CONFIGURATION(!) MEMORY programming algorithm for many older PICs */
#define PIC_ALGO_CONFIG_MEM_16F81X 13 /* CONFIGURATION(!) MEMORY programming algorithm for 16F818, 16F819  */
#define PIC_ALGO_CONFIG_MEM_16F87XA 14 /* CONFIGURATION(!) MEMORY programming algorithm for 16F87xA */
#define PIC_ALGO_16F630   15  /* added 2004-03-30, there are minor(!) differences between F628 and F630 !  */
#define PIC_ALGO_16F7X7   16  /* added 2004-07-31, again A NEW PROGRAMMING SPEC: TWO words per prog cycle  */
#define PIC_ALGO_dsPIC30F 17  /* added 2005-02-18, TOTALLY DIFFERENT programming methods for dsPIC30F      */
#define PIC_ALGO_PIC18F   18  /* added 2005-02-27, support for PIC18Fxxxx by M.v.d. Werff   */
#define PIC_ALGO_PIC18F_OLD 19 /* added 2007-01-25, because of trouble with erasing certain PIC18F's */
#define PIC_ALGO_PIC10F   20  /* added 2005-05-01, support for PIC10F20x by W.Buescher      */
#define PIC_ALGO_PIC16F7X 21  /* added 2005-08-21, because the PIC16F74 was too different.. */
#define PIC_ALGO_PIC16F716 22 /* added 2005-12-03, guess why ...  GRUMBLE ..                */
#define PIC_ALGO_ERASE_12F6XX 23 /* added 2006-03-01, no matter if really neccessary...     */
#define PIC_ALGO_MULTI_WORD    24   /* Added for 16F193X  */
#define PIC_ALGO_16FXX_OLD_ERASE    25   /* Kept in case the new algo would be buggy... else to be deleted  */

  // Different sequences to activate the "programming mode"  ?!?!?!?!?
  // Used in T_PicDeviceInfo.wVppVddSequence. A result of unprecise programming specs..
#define PROGMODE_VPP_THEN_VDD    0 /* Used in PIC12F675 and in NEW PIC16F628 */
#define PROGMODE_VDD_THEN_VPP    1 /* This was ONCE used for 16C84 (?!)      */
#define PROGMODE_VPP_VDD_UNKNOWN 2


  // Bitmasks for Config.iProgramWhat, telling certain routines "what to do":
#define PIC_PROGRAM_CODE     1
#define PIC_PROGRAM_DATA     2
#define PIC_PROGRAM_CONFIG   4
#define PIC_PROGRAM_ALL      7


  // States for "BATCH PROGRAMMING" ...
#define BATCH_PROG_OFF         0
#define BATCH_PROG_PREP_START  1
#define BATCH_PROG_WAIT_START  2
#define BATCH_PROG_WAIT_START2 3
#define BATCH_PROG_STARTED     4
#define BATCH_PROG_TERMINATE   9


/*------------- Data Types ------------------------------------------------*/

// impossible ?! : typedef bool(T_PicPrg_ReadWriteFunc)(uint32_t dwDeviceAddress, uint32_t *pdwData, uint32_t dwNrOfLocations );
typedef bool((T_PicPrg_ReadWriteFunc)(uint32_t dwDeviceAddress, uint32_t *pdwData, uint32_t dwNrOfLocations ));


/*------------- Variables  ------------------------------------------------*/
#undef EXTERN
#ifdef _I_AM_PIC_PRG_
 #define EXTERN
#else
 #define EXTERN extern
#endif

  // Current states for the three (?) memory types while "programming all"...
extern int PicPrg_iCodeMemErased  ;  // 0=no, 1=yes ...
extern int PicPrg_iCodeMemVerified;  // 0 = not verified yet, 1=verified "ok", -1=error(s)
extern int PicPrg_iDataMemErased  ;
extern int PicPrg_iDataMemVerified;
extern int PicPrg_iConfMemProgrammed;
extern int PicPrg_iConfMemErased  ;
extern int PicPrg_iConfMemVerified;    // verify-flags for ID/configuration memory range
extern int PicPrg_iConfWordProgrammed; // "classic" config word with readout protection
extern int PicPrg_iConfWordVerified;

  // other bits and pieces..
EXTERN long PIC_lBandgapCalibrationBits;
EXTERN long PIC_lOscillatorCalibrationWord;
EXTERN int  PIC_iHaveErasedCalibration;
EXTERN int  PIC_PRG_iBatchProgState;
extern int  PIC_PRG_iSimulateOnly;

 // ex: extern uint32_t dwChipWriteBufferSize; // located in M.v.d.Werff's PIC18F_PRG.CPP,
 // set to PIC_DeviceInfo.lCodeMemWriteLatchSize before calling code memory programming subroutines

/*------------- Prototypes ------------------------------------------------*/


/***************************************************************************/
CPROT void PIC_PRG_Init(void);
 /* Initializes all variables of the module.
  * Must be called before any other routine of this module.
  */


/***************************************************************************/
CPROT WORD PicPrg_GetConfigWordMask(void);
   // makes a 'mask' for programming and verification of the config word (ex:0x3FFF)



/*-----------  Second-Level  PIC Programmer routines -----------------*/

CPROT void PIC_PRG_Flash14ProgCycle(WORD wLoadCmd, WORD wData);
CPROT void PIC_PRG_Eprom14ProgCycle(WORD w);
CPROT bool PIC_PRG_Erase(int iEraseOptions);
CPROT void PIC_PRG_LoadConf(uint32_t dwDeviceAddress, WORD wDataAfterLoadCmd );
CPROT WORD PIC_PRG_ReadConf(void);
CPROT bool PIC_PRG_Program(uint32_t *pdwSourceData, int n, int pRowSize, uint32_t dwMask,
                     int ldcmd, int rdcmd, uint32_t dwDeviceAddress );
CPROT bool PicPrg_Verify( uint32_t dwDeviceAddress,
                    uint32_t *pdwSourceData,  // may be NULL since 2005-08 (calling buffer access routine then)
                    uint32_t dwNrOfLocations,
                    uint32_t dwMask, int rdcmd );
CPROT void PicPrg_SetVerifyResult( long i32DeviceBaseAddress, int iVerifyResultCode );

CPROT bool PIC_PRG_ReadAll(bool blank_check_only, bool *pfIsNotBlank );

CPROT bool PicPrg_WriteDataMemory(void);          // new since March 2005 ..
CPROT bool PicPrg_WriteConfigRegs( uint32_t dwDestAddress, uint32_t *pdwSourceData, uint32_t dwNrOfRegisters);
CPROT bool PicPrg_ReadExecutiveCodeMemory(void);  // for dsPIC30F only ! !

// char *HEX_PRG_ConfigWordToString(WORD cfg);

CPROT bool PIC_PRG_SetDeviceType(T_PicDeviceInfo *pDeviceInfo );

//-- Saving the calibration bits
void SaveBandgapCalBit (void);
void SaveOscCalWord    (void);

/* EOF <pic_prg.h> */
