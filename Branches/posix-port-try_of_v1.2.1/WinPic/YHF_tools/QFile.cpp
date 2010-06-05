/****************************************************************************/
/* \cbproj\yhf_tools\QFile.c:  "Quick File" helper routines                 */
/*                                                                          */
/*               NO APPLICATION-SPECIFIC STUFF IN HERE !                    */
/*               This unit is used in a number of projects (see history).   */
/*                                                                          */
/* Author and Copyright by:                                                 */
/*   Wolfgang Buescher (DL4YHF)                                             */
/*   Use of this sourcecode for commercial purposes strictly forbidden !    */
/*                                                                          */
/* Revision History:                                                        */
/*  V1.0, 2002-03-13:  Created for DL4YHF's "Spectrum Lab".                 */
/*  V1.1, 2003-09-25:  Used on the QRL in the CAN-Logger-Utility .          */
/*  V1.2, 2003-12-15:  Copied ..LastErrorCodeToString() into this module .  */
/*  V1.3, 2004-11-21:  Also used in module "YHF_MultiLang.cpp" now .        */
/*  V1.4, 2005-07-22:  Fixed a bug in QFile_Seek()                          */
/*        2006-03-20:  Module used in project "GPS-Player" now .            */
/*        2006-03-23:  Added the "QFILE_SEEK_xxx"-macros in QFile.h .       */
/*  V1.5, 2006-04-02:  Optionally NOT using the buffer, if the file is read */
/*                     in large chunks (for example the GTOPO30 database) . */
/*  V1.6, 2006-09-04:  Added QFile_ParseInteger(), used in YHF_graf .       */
/*  V1.7, 2007-04-25:  Modified QFile_ReadLine() to support DOS- and UNIX-  */
/*                     textfiles: DOS uses CR+LF, UNIX only LF as line end. */
/*        2007-07-08:  Fixed a bug in QFile_ReadLine(): CR-LF at buffer     */
/*                     boundaries caused reading an additional empty line.  */
/*        2007-11-26:  Fixed another bug in QFile_Read() :                  */
/*                      Unneccessary attempt to read past the end-of-file,  */
/*                      if the file was read exactly in multiples of the    */
/*                      internal buffer size .                              */
/*        2008-03-20:  Fixed a bug in QFile_Flush(), and increased the      */
/*                     internal buffer size (to minimize the number of      */
/*                     OS calls when saving wave files in SpecLab) .        */
/*  WxPic 2009-06-04:  Ported to wxWidget by Philippe Chevrier              */
/****************************************************************************/


#include "QFile.h"        // header for this module
#include <wx/log.h>

//-------- Some red tape required in many windoze app's -----------


///***************************************************************************/
//void T_QFile::QFile_LastErrorCodeToString(uint32_t dwError, char *dest_str, int maxlen)
//{
//  LPVOID lpMsgBuf;
//  char *cp;
//
//  FormatMessage(
//    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//    NULL,
//    dwError,
//    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//    (LPTSTR) &lpMsgBuf,
//    0,
//    NULL      );
//  // Note : there is a disgusting NEW LINE character somewhere in the string !
//
//  // Copy the string to the caller's buffer
//  strncpy( /*dest:*/dest_str, /*source:*/(char*)lpMsgBuf,  maxlen );
//
//  // Remove that DISGUSTING CR+NL characters if we can find them:
//  cp = dest_str+strlen(dest_str); // dangerous !
//  if( (cp>dest_str+2) && (cp<dest_str+maxlen) )
//   { if(*(cp-1)=='\n') *(cp-1)='\0';   // cut off this trailing CR NL junk !
//     if(*(cp-2)=='\r') *(cp-2)='\0';
//   }
//
//
//  // Free the buffer.
//  LocalFree( lpMsgBuf );
//}
//


//----------------- File Input Routines  ------------------------------------


/***************************************************************************/
bool T_QFile::QFile_Open( wxString filename, wxFile::OpenMode mode)
  // Opens a file.  Parameters similar to _rtl_open (which in fact is used internally):
  //   QFILE_O_RDONLY , QFILE_O_WRONLY ,  or   QFILE_O_RDWR  .
{
//  dwLastError = 0;       // no error yet
    {
        wxLogNull logNo; //-- Suppress error dialog
        File.Open( filename, mode );
    }
  dwBufferStartPos = 0;  // disk file position of 1st byte in buffer
  dwPrevBufferStartPos = 0;
  if(File.IsOpened())
//   { fOpened = false;
//     dwLastError = GetLastError();  // added 2003-12-15
//   }
//  else
   { sz512PathAndName= filename;
//     fOpened = true;
   }
  return File.IsOpened();
} // end QFile_Open()


/***************************************************************************/
int T_QFile::QFile_Read( void *buf, uint32_t len)
  // Reads a few bytes from the file, or -if possible- from a buffer
{
 uint32_t di = 0;  // destination index like the good old 8086-register
 uint8_t *pbSrc;
 uint8_t *pbDst = (uint8_t*)buf;
 long left_in_buffer = (long)dwNumUsedBytesInBuffer - (long)dwBufferIndex;

  if( len>QFILE_BUFFER_SIZE )  // since 2006-04-02 : forget the buffer if chunk is too large !
   {
     // Though we don't use the buffer, keep track of the current file position..
     dwNumUsedBytesInBuffer = dwBufferIndex = 0;
     left_in_buffer = File.Read( buf, len );
     if(left_in_buffer <= 0)
      { return 0;   // no more data in file
      }
     else // successfully read something :
      { // keep "buffer start index" up-to-date ...
        // dwBufferStartPos is the disk file position of the 1st byte in the buffer.
        // Must add the number of bytes read IN THE PREVIOUS _read-call (!!)
        // ex: dwBufferStartPos += left_in_buffer; (wrong, dwBufferStartPos must be ZERO after very first read() !)
        dwBufferStartPos = dwPrevBufferStartPos;
        dwPrevBufferStartPos += left_in_buffer;
        return left_in_buffer;
      }
   }
  else // caller seems to read rather small chunks. Minimize OS calls by using own buffer:
   {
     do
      {

        if( left_in_buffer>0 )
         { // no need to call the slow '_rtl_read' .. do it slower ourself!!!
           pbSrc = bBuffer + dwBufferIndex;
           while(di<len && (left_in_buffer>0) )
            { *pbDst++ = *pbSrc++;
              ++di;
              --left_in_buffer;
              ++dwBufferIndex;
            }
           if(di==len)
              return len;    // bingo, everything from buffer
         }


        // Arrived here: no more data in the buffer. Must do the next READ :
        dwNumUsedBytesInBuffer = dwBufferIndex = 0;
        left_in_buffer = File.Read( (void*)bBuffer, QFILE_BUFFER_SIZE );
        if(left_in_buffer <= 0) // modified 2007-11-26, ex: if(left_in_buffer < 0)
           return di;   // no more data in buffer, return the "rest"
        else // >= 0 ..
         {
           // keep "buffer start index" up-to-date ...
           // dwBufferStartPos is the disk file position of 1st byte in buffer.
           // Must add the number of bytes read IN THE PREVIOUS _read-call (!!)
           // ex: dwBufferStartPos += left_in_buffer; (wrong, dwBufferStartPos must be ZERO after very first read() !)
           dwBufferStartPos = dwPrevBufferStartPos;
           dwPrevBufferStartPos += left_in_buffer;
           dwNumUsedBytesInBuffer = (uint32_t)left_in_buffer;
         }
      }while(left_in_buffer>0);
   } // end else <rather small chunks read>

 return -1;        // most likely : reached end-of-file

} // end QFile_Read()



/***************************************************************************/
int T_QFile::QFile_ReadLine( char *pszDest, int iMaxLen )
  // Reads a single text line from a file,  up to the CR / NL terminator.
  // The [CR /] NL terminator will be skipped but not entered in pszDest .
  //   Since 2007-04-25, works with DOS- *and* UNIX- textfiles !
  // Return value:  > 0 = Count of characters,
  //                  ZERO if an empty line has been read,
  //               or NEGATIVE if there was an access error or end of file.
{

 uint8_t  bCurrChar, bPrevChar;
 int  di = 0;  // destination index like the good old 8086-register
 uint8_t *pbSrc;
 uint8_t *pbDst = (uint8_t*)pszDest;
 long left_in_buffer = (long)dwNumUsedBytesInBuffer - (long)dwBufferIndex;

 if(iMaxLen>0)
    *pszDest = '\0';  // return empty string if no characters available

 do
  {

   if( left_in_buffer>0 )
    { // no need to call the slow '_rtl_read' ..
      pbSrc = bBuffer + dwBufferIndex;
      while(di<iMaxLen && (left_in_buffer>0) )
       { bCurrChar = *pbSrc++;
         bPrevChar = bPrevCharFromReadLine;
         bPrevCharFromReadLine = bCurrChar;
         ++dwBufferIndex;
         --left_in_buffer;
         if( (bCurrChar<=(uint8_t)13) && (bCurrChar!='\t') )
          { // Looks like a control character, may be the end of the line.
            // The CR and/or NL characters are *NOT* returned to the caller,
            // instead the C-string will be zero-terminated.
            // Note:
            // - Under UNIX, a text line is terminated with Linefeed ONLY (LF, #10).
            // - Under DOS, a text line is terminated with CR (#13) + LF (#10).
            // - Here, we accept ANY of the following combinations as line separator:
            //   CR+LF, only LF, only CR, and LF+CR (quite exotic but who knows..)
            if( (bCurrChar==(uint8_t)10) && (bPrevChar==(uint8_t)13) )
             { // the current character is the 2nd part of a DOS-line-end .
               // ( special case added 2007-07-08,
               //  to make this work independent from buffer boundaries)
               bCurrChar = 0;  // don't append this character to the string,
               // because it's the "rest" of a CR-LF sequence which began in the previous buffer part.
             }
            else
             {
               *pbDst = 0;              // terminate "C" string at the 1st ctrl char
               if( left_in_buffer < 0 ) // must read next buffer part to check NL after CR :
                { dwNumUsedBytesInBuffer = dwBufferIndex = 0;
                  left_in_buffer = File.Read( (void*)bBuffer, QFILE_BUFFER_SIZE );
                  pbSrc = bBuffer;
                  if(left_in_buffer < 0)
                     return di;   // no more data in buffer
                  else
                   { // Keep "buffer start index" up-to-date ...
                     // ex: dwBufferStartPos += left_in_buffer;  (WRONG !)
                     dwBufferStartPos = dwPrevBufferStartPos;
                     dwPrevBufferStartPos += left_in_buffer;
                   }
                }

               // If the NEXT character is also a control char, skip it as well...
               // ... but ONLY if the 2nd control char is different from the first,
               // because otherwise we would treat TWO EMPTY "UNIX-Textlines" as ONE .
               // 2007-07-08: this is IMPOSSIBLE if we just reached the end
               //             of the buffer (that's why bPrevCharFromReadLine was added !)
               if( (*pbSrc<13) && (*pbSrc!='\t') && (*pbSrc!=bCurrChar) && (left_in_buffer>0) )
                 { ++dwBufferIndex;
                 }
              return di;        // return value = string length
             } // end of line detected !
          } // end if (bCurrChar<=(BYTE)13)
         if( bCurrChar )
          { *pbDst++ = bCurrChar;
            ++di;
          }
       }
      if(di==iMaxLen)
        return di;    // caller's maximum string length exceeded
    } // end if( left_in_buffer>0 )

   // Arrived here: no more data in the buffer. Must do the next READ :
   dwNumUsedBytesInBuffer = dwBufferIndex = 0;
   left_in_buffer = File.Read( (void*)bBuffer, QFILE_BUFFER_SIZE );
   if(left_in_buffer <= 0)
     { // no more data in buffer
       return (di>0)?di:-1;    // maybe END-OF-FILE (important!)
     }
   else // >= 0 ..
     { dwNumUsedBytesInBuffer = (uint32_t)left_in_buffer;
       // keep "buffer start index" up-to-date ....
       // ex: dwBufferStartPos += left_in_buffer;  (WRONG !)
       dwBufferStartPos = dwPrevBufferStartPos;
       dwPrevBufferStartPos += left_in_buffer;
     }


  }while(left_in_buffer>0);

  return (di>0)?di:-1;
} // end QFile_ReadLine()


/***************************************************************************/
long T_QFile::QFile_Seek( long offset, int fromwhere)
   // Works like lseek on a 'buffered' file.
   // Avoids unnecessary calls to library/OS functions !
   // QFile_Seek(  pqFile, 0, SEEK_CUR )  returns the current file position .
{
  long lResult;
  uint32_t dwNewAbsFilePosition = 0;
  uint32_t dwOldAbsFilePosition = dwBufferStartPos + dwBufferIndex;
  switch( fromwhere )
   {
     case QFILE_SEEK_SET: /* Positionierung vom Dateianfang aus */
        dwNewAbsFilePosition = (uint32_t)offset;
        break;
     case QFILE_SEEK_CUR: /* Positionierung von der aktuellen Position aus */
        dwNewAbsFilePosition = (uint32_t)( (long)dwOldAbsFilePosition + offset );
        break;
     case QFILE_SEEK_END: /* Positionierung vom Dateiende aus */
        if(fBufferModified)
         {
           QFile_Flush();
           dwBufferIndex = dwNumUsedBytesInBuffer = 0;
         }
        lResult = File.Seek(offset, wxFromEnd );
        if( lResult != wxInvalidOffset)
         { dwNewAbsFilePosition = (uint32_t)lResult;
           dwNumUsedBytesInBuffer = dwBufferIndex = 0;
           dwBufferStartPos = dwNewAbsFilePosition;
           dwPrevBufferStartPos = dwNewAbsFilePosition;
           return lResult;  // finished here ("special case")
         }
        break;
     default :
        return -1L;  // error  (like lseek, but no fooling around with the global "errno")
   }

  // Is this just a QUERY or a SET-command ?
  if( (fromwhere==SEEK_CUR) && (offset==0) )
   { // it's just a QUERY for the current file position :
     return dwOldAbsFilePosition;
   }

  // Added 2008-04-26 :
  // It may be unnecessary to call the OS to "really" change the
  // file position, for example if the new position is already in the buffer:
  if( ( dwNewAbsFilePosition >= dwBufferStartPos )
    &&( dwNewAbsFilePosition < (dwBufferStartPos+dwNumUsedBytesInBuffer) ) )
   { // the "new" file position is covered by the current buffer contents.
     // No need to call a time-consuming OS function !
     dwBufferIndex = dwNewAbsFilePosition - dwBufferStartPos;
     return (long)dwNewAbsFilePosition;
   }

  // Arrived here, we know the seek-action will MODIFY the real file pointer.
  // If we're about to change the file position, and the file is WRITEABLE,
  //   we MAY have to flush the buffer back into the real file :
  if(fBufferModified)
   {
    QFile_Flush();
    dwBufferIndex = dwNumUsedBytesInBuffer = 0;
   }

  // Change the physical file position and forget everything about the buffer
  lResult = File.Seek(dwNewAbsFilePosition, wxFromStart );
    // > lseek liefert bei fehlerfreier Ausführung die neue Zeigerposition,
    // > gemessen in Bytes vom Dateianfang, zurück .

  // Because the current file position may have changed,
  //  the buffer is no longer valid :
  dwNumUsedBytesInBuffer = dwBufferIndex = 0;
  if(lResult>=0)
         dwBufferStartPos = lResult;
   else  dwBufferStartPos = 0;
  dwPrevBufferStartPos = dwBufferStartPos; // added 2005-07-22

  return lResult;
}



//----------------- File Output Routines  -----------------------------------


/***************************************************************************/
bool T_QFile::QFile_Create( wxString filename, int attrib)
  // Creates a new file (or truncates existing). Parameters similar to _rtl_creat .
{
//  dwLastError = 0;       // no error yet
  wxASSERT(attrib==0);
  File.Create(filename);
  dwBufferStartPos = 0;
  dwPrevBufferStartPos = 0;
  if(File.IsOpened())
//   { fOpened = false;
//     dwLastError = GetLastError();  // added 2003-12-15
//   }
//  else
   { _tcsncpy( sz512PathAndName, filename, 512 );
//     fOpened = true;
   }
  return File.IsOpened();
} // end QFile_Create()

/***************************************************************************/
bool T_QFile::QFile_Flush( )
{
 int nWritten = 0;
  if(fBufferModified && File.IsOpened() && dwNumUsedBytesInBuffer>0 )
   {
     nWritten = File.Write(bBuffer, dwNumUsedBytesInBuffer);
     fBufferModified = false;
     if(nWritten>0)
      { dwBufferStartPos += nWritten;  // keep "buffer start index" up-to-date
        dwPrevBufferStartPos = dwBufferStartPos;
      }
//     else
//      {
//        dwLastError = GetLastError();  // added 2003-12-15
//      }
     return (nWritten>0);
   }
  return true;
} // end QFile_Flush()

/***************************************************************************/
void T_QFile::QFile_Close( )
  // Closes a file.
{
  if(File.IsOpened())
   { if(fBufferModified && dwNumUsedBytesInBuffer>0 )
      {
       QFile_Flush();   // flush the very last part into the file
      }
     File.Close();
//     iHandle = -1;
   }
//  fOpened = false;
} // end QFile_Close()


/***************************************************************************/
bool T_QFile::QFile_Write( BYTE *pbSrc, long i32CountOfBytes )
{
 bool fOk = File.IsOpened();

  if( (pbSrc) && (i32CountOfBytes>0) )
   {
    // once here: fBufferModified
    while(i32CountOfBytes)
     { --i32CountOfBytes;
      if(dwBufferIndex >= QFILE_BUFFER_SIZE)
       { if(dwNumUsedBytesInBuffer < dwBufferIndex)
            dwNumUsedBytesInBuffer = dwBufferIndex;
         fBufferModified=true;   // 2008-03-23: moved HERE
         fOk &= QFile_Flush();
         dwBufferIndex = dwNumUsedBytesInBuffer = 0;
       }
      bBuffer[ dwBufferIndex++ ] = *pbSrc++;
     } // end while <more characters from input string>
    fBufferModified=true;  // maybe ONE character modified !

    // Update the "buffer usage" indicator; there may be something left
    //  in the buffer which must be flushed on CLOSING the file..
    if(dwNumUsedBytesInBuffer < dwBufferIndex)
       dwNumUsedBytesInBuffer = dwBufferIndex;

   } // end if < valid data >

  return fOk;
} // end QFile_Write()


/***************************************************************************/
bool T_QFile::QFile_WriteString( char *pszSrc )
   // Writes a zero-terminated string.
   // Counts the characters, but DOES NOT APPEND CR/NL by itself .
{
 bool fOk = File.IsOpened();

  // One could detect the string length first, and *then* call QFile_Write(),
  //  but doing everything here in a single loop is a bit FASTER ...
  if(pszSrc)
   {
    fBufferModified=true;
    while(*pszSrc)
     {
      if(dwBufferIndex >= QFILE_BUFFER_SIZE)
       { if(dwNumUsedBytesInBuffer < dwBufferIndex)
            dwNumUsedBytesInBuffer = dwBufferIndex;
         fOk &= QFile_Flush();
         dwBufferIndex = dwNumUsedBytesInBuffer = 0;
       }
      bBuffer[ dwBufferIndex++ ] = *pszSrc++;
     } // end while <more characters from input string>
    fBufferModified=true;  // maybe ONE character modified !

    // Update the "buffer usage" indicator; there may be something left
    //  in the buffer which must be flushed on CLOSING the file..
    if(dwNumUsedBytesInBuffer < dwBufferIndex)
       dwNumUsedBytesInBuffer = dwBufferIndex;

   } // end if(pszSrc)

  return fOk;
} // end QFile_WriteString()


//----------------- Universal parsing routines ------------------------------
// (often required when text files are read into structures)

/**************************************************************************/
bool QFile_SkipChar(char **ppcSource, char cCharToSkip )
  // String handling routine: Checks for a certain character,
  //   and skips it from the "sourcecode" if found .
{
 char *cp = *ppcSource;
  while(*cp==' ' || *cp=='\t') // skip SPACES and TABS (for reading "text data files")
   { ++cp;
   }
  if( *cp == cCharToSkip )
   { *ppcSource = (char*)cp+1; // skip the "expected" character
     return true;
   }
  else
   { return false;
   }
} // end QFile_SkipChar()

/**************************************************************************/
bool QFile_SkipString(char **ppcSource, char *pszStringToSkip)
  // String handling routine: Checks for a certain string,
  //   and skips it from the "sourcecode" if found there.
  //   Leading spaces and tab characters are SKIPPED !
  // Returns true when found, otherwise false .
  // Often used when reading configuration files, as a replacement
  // for the terribly slow windows INI-files .
{
  char *cp = *ppcSource;
  while(*cp==' ' || *cp=='\t') // skip SPACES and TABS (for reading "text data files")
   { ++cp;
   }
  while( (*pszStringToSkip!='\0') && (*pszStringToSkip==*cp) )
   { ++pszStringToSkip;
     ++cp;
   }
  if( *pszStringToSkip == '\0' )  // bingo, reached the end of the string-to-skip
   { *ppcSource = (char*)cp;
     return true;
   }
  else
   { return false;
   }
} // end QFile_SkipString()


/***************************************************************************/
long QFile_ParseInteger(char **ppcSource, int maxdigits, int radix, long deflt)
  // String handling routine: Parses an integer number from any sourcecode.
  // Also good for HEXADECIMAL NUMBERS WITH '0x' PREFIX since 2006-01 !
  // If the sourcecode doesn't contain a valid number,
  // the source pointer will not be moved, and ZERO will be returned .
{
 long ret=0;
 int  neg=0;
 bool valid=false;
 BYTE *bp = (BYTE*)*ppcSource;
 BYTE c;
  while(*bp==' ' || *bp=='\t')   // skip SPACES and TABS (for reading "text data files")
    { ++bp;
    }
  if(*bp=='-')
    { ++bp; neg=1; }
  else
  if(*bp=='+')
    { ++bp; }
  if( bp[0]=='0' && bp[1]=='x' ) // hexadecimal (C-style) ?
   { bp += 2;  // skip hex prefix
     radix = 16;
   }
  if( radix == 16 )
   { while(maxdigits>0)
      {
        c=*bp;
        if( c>='0' && c<='9' )
         { ++bp;
           --maxdigits;
           valid = true;
           ret = 16*ret + (c-'0');
         }
        else if(c>='a' && c<='f')
         { ++bp;
           --maxdigits;
           valid = true;
           ret = 160*ret + (c-'a'+10);
         }
        else if(c>='A' && c<='F')
         { ++bp;
           --maxdigits;
           valid = true;
           ret = 16*ret + (c-'A'+10);
         }
        else
           break;
      }
   }
  else // not hexadecimal but decimal :
   { while( (c=*bp)>='0' && (c<='9') && (maxdigits>0) )
      { ++bp;
        --maxdigits;
        valid = true;
        ret = 10*ret + (c-'0');
      }
   }
  *ppcSource = (char*)bp;
  if( valid )
       return neg ? -ret : ret;
  else return deflt;
} // end QFile_ParseInteger()



/* EOF < YHF_Tools\QFile.c > */
