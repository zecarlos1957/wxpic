// PIC programming routines for PIC18F devices

// Device families within the PIC18F range

void PIC18F_SetDeviceFamily(uint32_t dwFamily,
               uint32_t dwWriteBufSize_wd, uint32_t dwEraseBufSize_wd);

#define PIC18_FAMILY_458   10
#define PIC18_FAMILY_4580  20
#define PIC18_FAMILY_4550  21
#define PIC18_FAMILY_4680  22


// Meaning of the dwAddress parameter
//
// The address argument (dwAddress) passed to the read/write subroutines is
// always the "device address" or the first byte / word / 24-bit location / etc.
//
// For the PIC18F range of devices this means:
//
// Code memory
//
//   One entry in the uint32_t-array contains one 16-bit WORD
//     1st instruction word: dwAddress = 0x000000, to be programmed with pdwSourceData[0]
//     2nd instruction word: dwAddress = 0x000002, to be programmed with pdwSourceData[1]
//     3rd instruction word: dwAddress = 0x000004, to be programmed with pdwSourceData[2]
//
// Data memory (EEPROM)
//
//   One entry in the uint32_t-array contains one BYTE
//     1st byte: dwAddress = 0xF00000, to be programmed with pdwSourceData[0]
//     2nd byte: dwAddress = 0xF00001, to be programmed with pdwSourceData[1]
//
//   For "historic" reasons, WinPic treats the data EEPROM as byte-wide memory.
//
// Configuration Registers
//
//   One entry in the uint32_t-array contains one 16-bit WORD
//     1st Config WORD: dwAddress = 0x300000 ("CONFIG1H:CONFIG1L") in pdwSourceData[0]
//     2nd Config WORD: dwAddress = 0x300002 ("CONFIG2H:CONFIG2L") in pdwSourceData[1]
//
// Device ID Register
//
//   One entry in the uint32_t-array contains one 16-bit WORD
//     dwAddress = 0x3FFFE (contains "DEVID2" in bits 15..8 and "DEVID1" in bits 7..0)


extern bool PIC18F_fUseOldBulkEraseCommands; // flag for PIC18F_EraseChip_4550(), added 2007-01-25


// Generic operations

bool PIC18F_EraseChip();

uint32_t PIC18F_ReadDeviceID();

// Code memory routines

bool PIC18F_WriteCodeMemory(
  uint32_t dwAddress,
  uint32_t *pdwSourceData,      // for 16-bit core, simply ignore the upper WORD
  uint32_t dwNumberOfCodeWords  // count of 'code memory words', not bytes !
);

bool PIC18F_ReadCodeMemory(
  uint32_t dwAddress,
  uint32_t *pdwDestBuffer,      // for 16-bit core, simply ignore the upper WORD
  uint32_t dwNumberOfCodeWords  // count of 'code memory words', not bytes !
);

// Data memory (EEPROM) routines

bool PIC18F_WriteDataMemory(
  uint32_t dwAddress,
  uint32_t *pdwSourceData ,    // only lower 8 bit in each uint32_t used here
  uint32_t dwNumberOfBytes     // for EEPROM, count of bytes(!) to be written
);

bool PIC18F_ReadDataMemory(
  uint32_t dwAddress,
  uint32_t *pdwDestBuffer,     // only lower 8 bit in each uint32_t used here
  uint32_t dwNumberOfBytes     // for EEPROM, count of bytes(!) to be read
);

// Configuration word routines

bool PIC18F_ReadConfigRegs(
  uint32_t dwSourceAddress,
  uint32_t *pdwDestBuffer,
  uint32_t dwNrOfRegisters
);

bool PIC18F_WriteConfigRegs(
  uint32_t dwDestAddress,
  uint32_t *pdwSourceData,
  uint32_t dwNrOfRegisters
);


