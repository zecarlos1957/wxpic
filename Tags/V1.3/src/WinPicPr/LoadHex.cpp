/**********************************************************************/
/*  LoadHex.C  =  Modul zum Importieren von Intel-Hex-Dateien         */
/*                 in W.B.'s HEX-Editor (und anderen Projekten) .     */
/*  Ermoeglicht das Einlesen von Dateien im 8-bit-Intel-Hex-Format.   */
/*  Die maximale Dateigroesse ist nur durch den vom Caller bereit-    */
/*  gestellten Puffer begrenzt.                                       */
/*                                                                    */
/*  Author:   Wolfgang Buescher (DL4YHF)                              */
/*     Use of this sourcecode for commercial purposes is forbidden !  */
/*                                                                    */
/*                                                                    */
/*  Versions-Historie:                                                */
/*  2009-06-04: Ported to wxWidget by Philippe Chevrier               */
/*  2005-03-07: Einsatz im "CAN-Tester für Windows" / FW-Update (QRL) */
/*  2005-02-23: Einsatz in DL4YHF's WinPic, mit Erweiterungen fuer    */
/*              die etwas merkwuerdigen HEX-Dateien von Microchip,    */
/*              speziell fuer dsPIC's mit "24-Bit-Instruktionen" .    */
/*  2000-03-29: Laderoutine fuer "Simple HEX Format" aus dem          */
/*               privaten Fundus von DL4YHF uebernommen.              */
/*                                                                    */
/**********************************************************************/

//#include <stdio.h>
//#include <alloc.h>
//#include <string.h>
//#include <io.h>
//#include <fcntl.h>
#include <wx/intl.h>
#include "Appl.h"
#include "QFile.h"        // W.B.'s "Quick-File" module

#include "LoadHex.h"      // W.B.'s Hex-File-Import [ LoadHexFile() by DL4YHF ]

char Hex_sz80ErrorMessage[84];


/*************************************************************************/
uint32_t Hex_ParseHexNumber(char *cp, int digits)
  /* Wandelt einen ASCII-String in einen Zahlenwert um (hexadezimal).
   *
   * Aenderungen:
   *  04.04.2000: Datentyp des Ergebnisses auf 32 Bit erweitert,
   *              um auch "lange" Adressen konvertieren zu koennen.
   */
{
  uint32_t ret;
  char ch;

  ret = 0;
  if (cp[0]=='\'' && cp[1]>0)
   { return cp[1];            /* Code eines  ASCII-Zeichens liefern ! */
   }

  while(digits--)
   {
     ch  = *cp++;
     if (ch==':')
      { /* vorzeitig erreichtes Ende der Hex-Zahl (z.B. nach Adresse) ! */
       return ret;
      }
     ret <<= 4;
     if (ch>='0' && ch<='9')  ret += (ch-'0');
     else
     if (ch>='a' && ch<='f')  ret += (ch-'a'+10);
     else
     if (ch>='A' && ch<='F')  ret += (ch-'A'+10);
     else  /* error: attempted to convert a non-hex-number */
         break;
   }
  return ret;
}


/*************************************************************************/
long LoadHexFile( const wxChar *fname,
                  T_HexLoadCallback pvLoadCallback,
                  wxChar *sz80ErrorMessage )
  /* Laedt ein Intel-Hex-File in den Puffer des HEX-Editors.
   * Wird automatisch aus HexBuf_LoadFile() aufgerufen,
   * wenn die zu ladende Datei die Erweiterung ".HEX" hat.
   * Speicherzellen, deren Inhalt im Hex-File nicht enthalten ist,
   * werden hier NICHT ueberschrieben
   *    (auch nicht auf "00" oder "FF" gesetzt, sonst wuerde das
   *     Laden mehrerer Teile eines Programms mit "Code Banking"
   *     schiefgehen !!)
   *
   * Return :  0 = OK,
   *           >0 = line number of error (failure when parsing a line)
   *           <0 = other "error codes"
   *
   */
{
  T_QFile myQFile;
  char sz255Line[256];

//  boolEAN ok;
  int  i;
  long iCurrentLineNr;
  long iErrorInLine;
  int  iLineLength;
  int  iRecordLength;
  uint16_t wLoadOffset;    /* funny name from Intel's hex file specification  */
  int  iRecordType;
  int  iSegmentedAddr; /* Flag ob "segmentierte" oder "lineare" Adressen  */
  int  iGotEOFRecord;  /* wird gesetzt wenn "End of File"-Record erreicht */
  uint8_t ip_checksum;
//  uint8_t ip_checksum_ok;
  uint8_t bData;
  char  *bp;
  uint32_t dwAddressBase;
  uint32_t dwLinearByteAddress;



   iErrorInLine  = 0;  /* Noch keine Fehler-Zeile bekannt                     */
   iCurrentLineNr= 1;  /* aktuell bearbeitete Zeilennummer in Datei, 1..N (!) */
   iGotEOFRecord = 0;  /* noch kein End-Of-File-Record gefunden               */
   iSegmentedAddr=0;  /* Adressierungsmodell "linear" (default)               */
   dwAddressBase= 0; /* aktuelle Bits 16..31 der Ladeadresse fuer Puffer      */
   /*   HexBuffer.bytes = 0;  gestrichen wegen "mehrteiligem Laden"           */

  if( sz80ErrorMessage )
      sz80ErrorMessage[0] = '\0';

  //  Open the file to see what's in it ...
  if( myQFile.QFile_Open( fname, QFILE_O_RDONLY ) )
   {  // read all lines from the file ...
     while( (iLineLength=myQFile.QFile_ReadLine( sz255Line, 255 )) >= 0 )
       { // Parse the input line ...
         // Check if this looks like an  INTEL-HEX-FILE, which means ..
         // at least 9 characters "Prefix" + 2 characters checksum, 1st char =  ":"
        if ( (iLineLength>=11) && (sz255Line[0]==':') )
         { /* sieht ganz verdaechtig nach "INTEL HEX"-Format aus !!  */

           // Einige "immer benoetigte" Parameter aus input-Zeilen-String analysieren:
           iRecordLength = Hex_ParseHexNumber(sz255Line+1, 2);  /* record length (2 digits) */
           wLoadOffset   = Hex_ParseHexNumber(sz255Line+3, 4);  /* "offset"      (4 digits) */
           iRecordType   = Hex_ParseHexNumber(sz255Line+7, 2);  /* record type   (2 digits) */
           ip_checksum = 0;   /* einfache Zeilen-Pruefsumme, muss Null ergeben...*/
           for (i=0;i<iRecordLength+5;++i) /*..indem die Pruefsumme mit addiert wird */
              ip_checksum += Hex_ParseHexNumber(sz255Line+1+2*i, 2);
           if (ip_checksum == 0)
            {  /* nur wenn die Zeilen-Pruefsumme passt: */
             /* das hoeherwertige Digit aus dem "record type" muss immer '0' sein,
              * das niederwertige Digit aus dem "record type" definiert den Typ.
              */
             switch(iRecordType)
              {
               // Aus: "Intel Hexadecimal Object File Format Specification",
               //      Revision A, 1/6/88
               // > '00' Data Record
               // > '0l' End of File Record
               // > '02' Extended Segment Address Record
               // > '03' Start Segment Address Record
               // > '04' Extended Linear Address Record
               // > '05' Start Linear Address Record
               case 0:   /* record type == "Data"----------------------------*/
                // Data Record :
                //  > LOAD OFFSET
                //  >  This field contains four ASCII hexadecimal digits
                //  >  representing the offset from the LBA (see
                //  >  Extended Linear Address Record)
                //  >  or SBA (see Extended Segment Address Record) defining the
                //  >  address which the first byte of the data is to be placed.
                for (i=0;i<iRecordLength;++i)
                 {
                   bData = Hex_ParseHexNumber( sz255Line+9+2*i, 2 );
                   if (iSegmentedAddr)
                    {      /* Segmentierung, alles innerhalb eines 64k-Blocks.*/
                           /* Siehe "Formel" in Intel-Hex-Format-Beschreibung */
                      dwLinearByteAddress = dwAddressBase + wLoadOffset + i;
                    }
                   else    /* nicht segmentiert sondern linear,            */
                    {      /* 64k-Segment-Grenzen existieren hier nicht !! */
                           /* Darum erfolgt die Addition mit 32 Bit Breite.*/
                      dwLinearByteAddress=dwAddressBase + (long)wLoadOffset + (long)i;
                    }
                   // Pass the loaded byte to the application via callback :
                   (*pvLoadCallback)(iCurrentLineNr,dwLinearByteAddress, bData );  // here from "Data" record
                 }
                break;
               case 1:   /* record type == "End of File"---------------------*/
                iGotEOFRecord = 1;
                break;
               case 2:   /* record type == "Extended segment address"--------*/
                /* Nach diesem Record berechnet sich die Zieladresse aus:
                 *    address[in adress record, addressbits 4..19]
                 *  + (  ( offset[in data record]
                 *         +index[in data record] ) MODULO 64k )
                 */
                 if(iRecordLength!=2) /* dieser Record muss 2 Nutzdatenbytes haben */
                  {
                   if (iErrorInLine==0)
                    { iErrorInLine=iCurrentLineNr;
                      _stprintf(sz80ErrorMessage,_("bad length(%d)"),(int)iErrorInLine);
                    }
                  }
                 else
                  {
                   iSegmentedAddr=1; /*Adressierungsmodell jetzt "segmentiert"*/
                   dwAddressBase = (long)Hex_ParseHexNumber(sz255Line+9,4) << 4;
                  }

                break;
               case 3:   /* record type == "Start segment address"-----------*/
                break;
               case 4:   /* record type == "Extended linear address"---------*/
                /* Nach diesem Record berechnet sich die Zieladresse aus:
                 *    address(in adress record)
                 *  + offset(in data record)
                 *  + index(in data record)
                 */
                 if(iRecordLength!=2) /* dieser Record muss 2 Nutzdatenbytes haben */
                  {
                   if (iErrorInLine==0)
                    { iErrorInLine=iCurrentLineNr;
                      _stprintf(sz80ErrorMessage,_("bad length(%d)"),(int)iErrorInLine);
                    }
                  }
                 else
                  {
                   iSegmentedAddr=0;  /* Adressierungsmodell jetzt "linear" */
                   dwAddressBase = (long)Hex_ParseHexNumber(sz255Line+9,4) << 16;
                  }
                break;
               case 5:     /* record type == "Start linear address"----------*/
                break;
               default:    /* unbekannte "record types" werden ignoriert !!! */
                _stprintf(sz80ErrorMessage,_("HEX-record%d??"), iRecordType);
                break;
              } /* end switch(sz255Line[7])           */
            }   /* end if <Pruefsumme ok>      */
           else
            {   /* Pruefsummenfehler: melden in welcher Zeile */
             if (iErrorInLine==0)
              {
                iErrorInLine=iCurrentLineNr;
                _stprintf(sz80ErrorMessage,_("ChkSum (l%d)?"),(int)iErrorInLine);
              }
            }
         } /* end if <gueltige INTEL-HEX-File-Zeile> */
        else
          /* Die Zeile sieht nicht nach "Intel-Hex-Zeile" aus..........
           * Vielleicht ist es eine "Hex-Dump-Zeile" (DL4YHF's Simple Hex Format)
           * mit "linearer" 32-Bit-Adresse am Anfang ?
           *  (Nicht die elende Fuddelei mit "Segment" und "Offset",
           *   die bei vielen Targets sowieso keine sinnvolle Funktion hat
           *   solange man nicht genau definiert was ein "Segment" eigentlich ist..)
           */
        if ( (strlen(sz255Line)>=10)
          && (sz255Line[0]>='0') && (sz255Line[0]<='9') /* hoechstes Digit der Adresse */
          && (sz255Line[8]==':') ) /* Trenner zwischen Adresse und Nutzdaten    */
         {
           /* HEX-Dump-Zeile analysieren:  */
           /* Hex-Dump-Zeile hat IMMER eine lineare Zieladresse am Zeilenanfang:  */
           dwAddressBase = Hex_ParseHexNumber(sz255Line+0, 8);  /* (8 digits) */
           /* Das "Nutzdatenfeld" aus der HEX-DUMP-Zeile uebernehmen.     */
           /* Die ANZAHL Nutzdatenbytes ergibt sich aus der Analyse.      */
           bp = sz255Line+9;     /* Pointer auf erstes ZEICHEN (hinter ":")      */
           i  = 0;        /* Index fuer Nutzdatenarray                    */
           while( *bp>0 && i<32)
            {
             if (*bp==' ') /* EIN Trenn-Space wird akzeptiert, aber nicht mehr */
                  bp++;
             if ( bp[0]>' ' && bp[1]>' ' )
                {
                  bData = Hex_ParseHexNumber( bp , 2 );
                  dwLinearByteAddress=dwAddressBase + (long)i;
                  // Pass the loaded byte to the application via callback :
                  (*pvLoadCallback)(iCurrentLineNr, dwLinearByteAddress, bData );  // here from "HEX DUMP" line
                  ++i;  /* Naechstes Datenbyte */
                  bp+=2;
                }
             else  /* da kommt nix mehr: */
                 break;
            } /* end while */
         } /* end if <gueltige HEX-DUMP(!)-Zeile> */
        else
         { /* Zeile ist weder "Intel-HEX" noch DL4YHF's "Simple Hex Format": */
          if(sz255Line[0]==';')
           { /* Kommentarzeile: einfach nur ignorieren !! */
             // Der SCC-Compiler produziert z.B. Folgendes am Ende eines HEX-Files:
             //  :02400E00F93F78
             //  :00000001FF
             //  ;PIC16F84
             if(sz255Line[1]=='E' && sz255Line[2]=='O'  && sz255Line[3]=='F')
              { iGotEOFRecord = 1;
              }
             else // must be some other (funny) comment :
              {   // NOTE: this is STRANGE (like some compilers) but not an ERROR ! !
                if( sz80ErrorMessage[0]=='\0' )
                 { _stprintf(sz80ErrorMessage,_("Strange line in HEX-file (%d): %40hs"),(int)iCurrentLineNr, sz255Line);
                 }
              }
           }
         }

        ++iCurrentLineNr;  /* Zeilenzaehler (zum Lokalisieren fehlerhafter Zeilen) */
      } // end while
     myQFile.QFile_Close();  // close the hex-file again
    }

   /* Meckern, wenn im Hex-File kein EOF-Record vorhanden ist.          */
   /*  (dies deutet darauf hin, dass die geladene Datei einen Bug hat)  */
   if ( (iErrorInLine==0) && (!iGotEOFRecord) )
     { iErrorInLine=iCurrentLineNr;
       _tcscpy(sz80ErrorMessage,_("No EOF-record"));
     }

   return iErrorInLine;

} // end LoadHexFile()

// EOF <LoadHex.C>
