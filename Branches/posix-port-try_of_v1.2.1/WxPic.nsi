; WxPic Installer Generator Script
;
; BUILD may be defined with /DBUILD=DEBUG to use the Debug executable in the generation
; Else by default the Release executable is used

;--------------------------------

XPStyle on

!define PRODUCT_NAME "WxPic"
!define EXE_NAME "${PRODUCT_NAME}.exe"
!define UNINSTALL "uninstall-${EXE_NAME}"

Name "${PRODUCT_NAME}"

RequestExecutionLevel highest

InstallDir $PROGRAMFILES\${PRODUCT_NAME}

SetCompressor /SOLID lzma

;--------------------------------
; If the build is not defined, then define it to Release
!ifndef ${BUILD}
   !define BUILD Release
!else
	!if ${BUILD} != "Debug"
		!if ${BUILD} != "Release"
			!error "The build '${BUILD}' is not a valid build name"
		!endif
	!endif
!endif
!if ${BUILD} == "Debug"
	!define SHORT_BUILD Dbg
!else
	!define SHORT_BUILD
!endif

; Read the version number in version.autogen file
!searchparse /file version.autogen '#define SVN_DATE (_T("' SVN_DATE '"'
!searchparse /file version.autogen '#define SVN_REVISION (' SVN_REVISION ")" 
!searchparse /file version.autogen '#define SVN_VERSION (_T("' SVN_VERSION '"'
!searchparse /file version.autogen '#define FULL_SVNVER (_T("' FULL_VERSION '"'
!searchparse /file version.autogen '#define SVN_MANAGED (' SVN_MANAGED ")"

!echo "SVN_DATE = ${SVN_DATE}"
!echo "SVN_REVISION = ${SVN_REVISION}"
!echo "SVN_VERSION = ${SVN_VERSION}"
!echo "FULL_SVNVER = ${FULL_VERSION}"
!echo "SVN_MANAGED = ${SVN_MANAGED}"

!if "${SVN_MANAGED}" == "true"
	OutFile "${PRODUCT_NAME}-${SVN_VERSION}R${SVN_REVISION}${SHORT_BUILD}-installer.exe"
	!define VERSION_TEXT "${SVN_VERSION}R${SVN_REVISION}${SHORT_BUILD}"
	!define VERSION "${FULL_VERSION}"
;	!searchparse /noerrors ${SVN_VERSION} "V" VER1 "." VER2 "." VER3 
;	!ifdef VER2
;		!if "${VER3}" == ""
;			!undef VER3 
;		!endif
;		!ifndef VER3
;			!define VER3 0
;		!endif
;	!else
;		!ifdef VER1
;			!undef VER1
;		!endif
;		!define VER1 0
;		!define VER2 0
;		!define VER3 0
;	!endif
;	!define VERSION "${VER1}.${VER2}.${VER3}.${SVN_REVISION}"
!else
	OutFile "${PRODUCT_NAME}${SHORT_BUILD}-setup.exe"
	!define VERSION_TEXT "Unknown.${SHORT_BUILD}"
	!define VERSION 0.0.0.0
!endif
!echo "   ==> VERSION_TEXT = ${VERSION_TEXT} / ${VERSION}"

VIProductVersion "${VERSION}"

BrandingText "${PRODUCT_NAME} ${VERSION_TEXT}"

;--------------------------------
; Include all the language
!include "Lang\English.nsh"  ; This is the default
!include "Lang\fr\French.nsh"
; Add include for additional language here

LicenseData $(LicenseData)

;--------------------------------

Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------
var CONTEXT

!macro INIT_CONTEXT
	ClearErrors
	UserInfo::GetAccountType
	IfErrors Win9x
	Pop $CONTEXT
	StrCmp $CONTEXT "Admin" Admin
	SetShellVarContext current	
	Goto ContextDone
Admin:
	SetShellVarContext all	
	Goto ContextDone
	
Win9x:
	MessageBox MB_OK $(Win9xError)
	Return
	
ContextDone:
!macroend


Function .onInit
	!insertmacro INIT_CONTEXT
	
	Push ""
	Push ${LANG_ENGLISH}
	Push English
	;------------------
	Push ${LANG_FRENCH}
	Push French
	;-- Add next languages following the same pattern
	Push A ; A means auto count languages
	       ; for the auto count to work the first empty push (Push "") must remain
	LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

	Pop $LANGUAGE
	StrCmp $LANGUAGE "cancel" 0 +2
		Abort
		
FunctionEnd


Function un.onInit
	!insertmacro INIT_CONTEXT
FunctionEnd

;--------------------------------

Section "${PRODUCT_NAME}"

	SectionIn RO
  
	; Set output path to the installation directory.
	SetOutPath $INSTDIR
  
	; Put executable file there
	File "bin\${Build}\${EXE_NAME}"
	
	; Put the additinal files (parameters, DLL, ...)
	File "Install\devices.ini"
	File "Install\DisablePolling.reg"
	File "Install\EnablePollingBack.reg"
	File "Install\SampleInterfaceOnSerialPort.xml"
	File "Install\SampleInterfaceOnLptPort.xml"
	File "Install\WinRing0.vxd"
	File "Install\WinRing0.sys"
	File "Install\WinRing0.dll"
	File "Install\WinRing0.COPYRIGHT.txt"

	CreateDirectory "$INSTDIR\Devices"
	
	; Write the uninstall keys for Windows
	WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayName" "${PRODUCT_NAME} ${VERSION_TEXT}"
	WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString" '"$INSTDIR\uninstall-${PRODUCT_NAME}.exe"'
	WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoModify" 1
	WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoRepair" 1
	WriteUninstaller "uninstall-${PRODUCT_NAME}.exe"
  
SectionEnd

Section "$(ShortCutSectionName)"
  
	SetOutPath $INSTDIR  ;-- Define Current directory for Shortcuts
	CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall ${PRODUCT_NAME}.lnk" "$INSTDIR\${UNINSTALL}"
	CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${EXE_NAME}" 

SectionEnd

;-- One section per optional language (the default language - English - is managed in the main section)
SectionGroup "$(LangSectionName)"

Section "English"
	;-- English is default Language and can't be deactivated
	SectionIn RO
	; Put The default Help and license files
	SetOutPath "$INSTDIR\Help"
	File "bin\${Build}\Help\*.*"
	SetOutPath "$INSTDIR\Lang"
	File "bin\${Build}\Lang\License.txt"
	File "bin\${Build}\Lang\Author.txt"
SectionEnd

!macro LANGUAGE_SECTION LANG_NAME LANG_TAG
Section "${LANG_NAME}"
	; Copy the translation file
	SetOutPath "$INSTDIR\Lang\${LANG_TAG}"
	File "bin\${Build}\Lang\${LANG_TAG}\WxPic.mo"
	File "bin\${Build}\Lang\${LANG_TAG}\License.txt"
	File "bin\${Build}\Lang\${LANG_TAG}\Author.txt"
	
	; Copy the Help files if any
	SetOutPath "$INSTDIR\Help\${LANG_TAG}"
	File /nonfatal "bin\${Build}\Help\${LANG_TAG}\*.*"
SectionEnd
!macroend

!insertmacro LANGUAGE_SECTION "French" "fr"

;-- Add more LANGUAGE_SECTION here to add new Languages
;-- The only data to adapt from one optional language to another
;-- are the values assigned to $LANGUAGE_SECTION parameters

SectionGroupEnd


;--------------------------------

; Uninstaller

UninstallText $(UninstallWarning)
UninstallIcon "${NSISDIR}\Contrib\Graphics\Icons\nsis1-uninstall.ico"

Section "Uninstall"

	;ReadRegStr "$INSTDIR SHCTX SOFTWARE\${PRODUCT_NAME}" "Install_Dir"
	
RetryDeleteExe:
	ClearErrors
	Delete "$INSTDIR\${EXE_NAME}"
	IfErrors 0 ExeDeleted
	MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION $(CantDeleteExe) /SD IDCANCEL IDRETRY RetryDeleteExe
	Abort
ExeDeleted:	

	; Uninstall the additinal files
	Delete "$INSTDIR\devices.ini"
	Delete /REBOOTOK "$INSTDIR\WinRing0.sys"
	Delete /REBOOTOK "$INSTDIR\WinRing0.vxd"
	Delete /REBOOTOK "$INSTDIR\WinRing0.dll"
	Delete "$INSTDIR\SampleInterfaceOnSerialPort.xml"
	Delete "$INSTDIR\SampleInterfaceOnLptPort.xml"
	Delete "$INSTDIR\WinRing0.COPYRIGHT.txt"
	Delete "$INSTDIR\DisablePolling.reg"
	Delete "$INSTDIR\EnablePollingBack.reg"
   
	IfFileExists "$INSTDIR\Devices\*.dev" +1 NoDeviceFile
		MessageBox MB_YESNO|MB_ICONQUESTION $(DeviceFilesExist) /SD IDYES IDYES KeepDeviceFiles 
		RMDir /r "$INSTDIR\Devices"
		goto EndOfDeviceFile
NoDeviceFile:
	RMDir "$INSTDIR\Devices"
KeepDeviceFiles:
EndOfDeviceFile:
   
	RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"
	
	RMDir /r "$INSTDIR\Lang"
	RMDir /r "$INSTDIR\Help"
	
	Delete /REBOOTOK "${UNINSTALL}"
  
	DeleteRegKey SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
	;DeleteRegKey SHCTX "SOFTWARE\${PRODUCT_NAME}"
	
SectionEnd
