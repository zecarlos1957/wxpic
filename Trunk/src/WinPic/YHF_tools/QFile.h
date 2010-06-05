/****************************************************************************/
/* \cbproj\yhf_tools\QFile.c:  "Quick File" helper routines                 */
/*                                                                          */
/*               NO APPLICATION-SPECIFIC STUFF IN HERE !                    */
/*               This unit is used in a number of projects (see history).   */
/*                                                                          */
/* Author and Copyright by:                                                 */
/*   Wolfgang Buescher (DL4YHF)                                             */
/*                                                                          */
/* Revision History:                                                        */
/*        2009-06-04:  Ported to wxWidget by Philippe Chevrier              */
/*        2006-03-23:  Added the "QFILE_SEEK_xxx"-macros in QFile.h .       */
/*        2007-04-25:  Modified QFile_ReadLine() to support DOS- and UNIX-  */
/*                     textfiles: DOS uses CR+LF, UNIX only LF as line end. */
/*        2007-07-08:  Fixed a bug in QFile_ReadLine(): CR-LF at buffer     */
/*                     boundaries caused reading an additional empty line.  */
/*                                                                          */
/*  WxPic 2009-06-04:  Ported to wxWidget by Philippe Chevrier              */
/****************************************************************************/

#ifndef _QFILE_H_
#define _QFILE_H_

#include <wx/file.h>
//#include <io.h>         // some file DEFINITIONS

//---------- Constants ------------------------------------------------------

// Possible values for 'mode' in QFile_Open .
// Declared here so other modules don't need to include dozens of headers .
#define QFILE_O_RDONLY    wxFile::read
#define QFILE_O_WRONLY    wxFile::write
#define QFILE_O_RDWR      wxFile::read_write

#define QFILE_SEEK_SET  wxFromStart    /* Position from beginning of the file */
#define QFILE_SEEK_CUR  wxFromCurrent  /* Position relative to current position */
#define QFILE_SEEK_END  wxFromEnd      /* Position from end of the file */


//ex: #define QFILE_BUFFER_SIZE 2048  // must be a power of two !
#define QFILE_BUFFER_SIZE 16384 // must be a power of two !

//---------- Data types (no C++ classes!) -----------------------------------
class T_QFile // T_QFile (for "Quick file access")
{
public:

T_QFile(void)
: bPrevCharFromReadLine  (0)  // added to detect end of text lines properly
, fBufferModified        (false)
, dwBufferStartPos       (0)
, dwPrevBufferStartPos   (0)  // important for QFile_Seek()
, dwBufferIndex          (0)  // buffer index for the next read/write access
, dwNumUsedBytesInBuffer (0)  // count of "used" bytes in the buffer
{}

///***************************************************************************/
//void QFile_LastErrorCodeToString(uint32_t dwError, char *dest_str, int maxlen);
//

/***************************************************************************/
bool QFile_Open( const wxChar *filename, wxFile::OpenMode mode);
  // Opens a file.  Parameters similar to wxFile : wxFile::read, wxFile::write, wxFile::read_write
  // #defined to  QFILE_O_RDONLY , QFILE_O_WRONLY ,  or   QFILE_O_RDWR  .

/***************************************************************************/
void QFile_Close( );
  // Closes a file.

/***************************************************************************/
long QFile_Seek( long offset, int fromwhere);
  // Works like lseek on a 'buffered' file.
  // QFile_Seek(  pqFile, 0, SEEK_CUR )  returns the current file position .

/***************************************************************************/
int QFile_Read( void *buf, uint32_t len);
  // Reads a few bytes from the file, or -if possible- from a buffer

/***************************************************************************/
int  QFile_ReadLine( char *pszDest, int iMaxLen );
  // Reads a single text line from a file,  up to the CR / NL terminator.
  // The [CR /] NL terminator will be skipped but not entered in pszDest .
  // Return value:  > 0 = Count of characters,
  //                  ZERO if an empty line has been read,
  //               or NEGATIVE if there was an access error or end of file.


/***************************************************************************/
bool QFile_Create( const wxChar *filename, int attrib);
  // Creates a new file (or truncates existing). Parameters similar to _rtl_creat:
  // attrib=0 is a "normal" file (neither hidden nor something else)

/***************************************************************************/
bool QFile_Write( uint8_t *pbSrc, long i32CountOfBytes );


/***************************************************************************/
bool QFile_WriteString( char *pszSrc );
   // Writes a zero-terminated string.
   // Counts the characters, but DOES NOT APPEND CR/NL by itself .


//--------------------------------------------------------------------------
//  String handling routines .   Frequently used when reading TEXT FILES .
//--------------------------------------------------------------------------
static bool QFile_SkipChar(char **ppcSource, char cCharToSkip );
static bool QFile_SkipString(char **ppcSource, char *pszStringToSkip);
static long QFile_ParseInteger(char **ppcSource, int ndigits, int radix, long deflt);


private:

    bool QFile_Flush(void);

  wxFile   File;
  wxChar  sz512PathAndName[514];
  uint8_t  bBuffer[QFILE_BUFFER_SIZE];
  uint8_t  bPrevCharFromReadLine; // added 2007-07-08 to detect end of text lines properly
  bool  fBufferModified;
  uint32_t dwBufferStartPos,dwPrevBufferStartPos; // important for QFile_Seek()
  uint32_t dwBufferIndex;   // buffer index for the next read/write access
  uint32_t dwNumUsedBytesInBuffer;   // count of "used" bytes in the buffer
//  uint32_t dwLastError;     // if there was an error, the error code is here
//  bool  fOpened;
};


//------------ Prototypes -------------------------------------------------



#endif // _QFILE_H_
