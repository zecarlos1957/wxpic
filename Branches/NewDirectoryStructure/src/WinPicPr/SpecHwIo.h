/*****************************************************************************/
/*  SpecHwIo.h                                                               */
/*                                                                           */
/*  Special Hardware I/O-Routines for Spectrum Lab + Alert functions         */
/*   - coded by DL4YHF October '2000                                         */
/*****************************************************************************/


/*-----------  Low-Level access for PIC Programmer routines -----------------*/

bool SpecHw_OpenComPort(void);
bool SpecHw_CloseComPort(void);
uint16_t SpecHw_GetAlarmResetBit(void);
bool SpecHw_SetOutputLine( int output_line_nr, bool new_state_high );
bool SpecHw_Init(void);
void SpecHw_Close(void);


// EOF <SpecHwIo.h>
