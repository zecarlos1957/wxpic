/**********************************************************************/
/*  LoadHex.H  =  Modul zum Importieren von Intel-Hex-Dateien         */
/*                 in W.B.'s HEX-Editor                               */
/*  Ermoeglicht das Einlesen von Dateien im 8-bit-Intel-Hex-Format.   */
/*  Die maximale Dateigroesse ist nur durch den von HEX_ED bereit-    */
/*  gestellten Puffer begrenzt.                                       */
/*                                                                    */
/*  Versions-Historie:                                                */
/*  2009-06-04:  Ported to wxWidget by Philippe Chevrier              */
/*  2005-03-07: Einsatz im "CAN-Tester für Windows" / FW-Update (QRL) */
/*  2005-02-23: Einsatz in DL4YHF's WinPic, mit Erweiterungen fuer    */
/*              die etwas merkwuerdigen HEX-Dateien von Microchip,    */
/*              speziell fuer dsPIC's mit "24-Bit-Instruktionen" .    */
/*  2000-03-29: Laderoutine fuer "Simple HEX Format" aus dem          */
/*               privaten Fundus von DL4YHF uebernommen.              */
/*                                                                    */
/**********************************************************************/


#ifdef __cplusplus
 #define CPROT extern "C"
#else
 #define CPROT
#endif



//--------------- Typen ---------------------------------------------
typedef void(* T_HexLoadCallback)(uint32_t dwHexSourceLine,uint32_t dwLinearByteAddress, uint8_t bData); // callback for 'load file'

//--------------- Prototypen ----------------------------------------
CPROT long LoadHexFile( const wxChar *fname, T_HexLoadCallback pvLoadCallback, wxChar *sz80ErrorMessage );
      // loads data from an INTEL-HEX-file


// EOF <LoadHex.h>
