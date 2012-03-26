!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "WinVer.nsh"
!include "X64.nsh"



;--------------------------------
;General

  ;Name and file
  Name "PortScope"
  OutFile "portscope-1.0.0.exe"

  ;Default installation folder
  InstallDir "$LOCALAPPDATA\PortScope"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\PortScope" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "gpl-3.0.txt"
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  
;--------------------------------
;Languages 
  !insertmacro MUI_LANGUAGE "English"

  
;------------------------------------------------------------------------------
;   Main Installation Section
;------------------------------------------------------------------------------
Section "Installation Files" SecInstallationFiles

    SetOutPath "$INSTDIR"
    SetShellVarContext all
    
    
    ; License
    File "gpl-3.0.txt"

    ; Application
	File /r ".\release\"

    ; Qt Libraries
	File $%QTDIR%\bin\QtCore4.dll
	File $%QTDIR%\bin\QtGui4.dll
	File $%QTDIR%\bin\QtXml4.dll
    
	CreateDirectory "$SMPROGRAMS\PortScope"
    CreateShortcut  "$SMPROGRAMS\PortScope\PortScope.lnk" $INSTDIR\portscope.exe
    CreateShortcut  "$SMPROGRAMS\PortScope\Uninstall PortScope.lnk" $INSTDIR\uninstall.exe
    
    ; Store installation folder
    WriteRegStr HKCU "Software\PortScope" "" $INSTDIR
  
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd



;------------------------------------------------------------------------------
;   Driver Installation Section
;------------------------------------------------------------------------------
Section "PortScope Driver" SecDriver


; Windows XP
${If} ${IsWinXP}

    DetailPrint "Installing Windows XP driver"
    CopyFiles $INSTDIR\drivers\winxp\portscope.sys $SYSDIR\drivers\portscope.sys

 ; Windows Vista
${ElseIf} ${IsWinVista}

  ${If} ${RunningX64}
    DetailPrint "Installing Windows Vista 64-bit driver"
    CopyFiles $INSTDIR\drivers\win7_64\portscope.sys $SYSDIR\drivers\portscope.sys

  ${Else}
    DetailPrint "Installing Windows Vista 32-bit driver"
    CopyFiles $INSTDIR\drivers\win7_32\portscope.sys $SYSDIR\drivers\portscope.sys

  ${EndIf}

 ; Windows 7
${ElseIf} ${IsWin7}

  ${If} ${RunningX64}
    DetailPrint "Installing Windows 7 64-bit driver"
    CopyFiles $INSTDIR\drivers\win7_64\portscope.sys $SYSDIR\drivers\portscope.sys

  ${Else}
    DetailPrint "Installing Windows 7 32-bit driver"
    CopyFiles $INSTDIR\drivers\win7_32\portscope.sys $SYSDIR\drivers\portscope.sys

  ${EndIf}
  
${Else}
    messageBox MB_OK "Operating System is not supported"
    Abort
    
${EndIf}


; Install the service
DetailPrint "Installing service"
nsSCM::Install /NOUNLOAD "PortScope" "PortScope Driver" 1 0 "$SYSDIR\drivers\portscope.sys" "" "" "" ""                               
pop $0

${If} "$0" != "success"
    messageBox MB_OK "Could not install service"
    Abort
${Else}
    DetailPrint "Service Successfully Installed"
    nsSCM::Start "PortScope"
${Endif}
  

SectionEnd



;------------------------------------------------------------------------------
;   Uninstaller
;------------------------------------------------------------------------------
Section "Uninstall"
  
    SetShellVarContext all  
    
    ; Stop and delete the service
    nsSCM::Stop "PortScope"
    nsSCM::Remove "PortScope"

    RMDir /r "$SMPROGRAMS\PortScope\"
    
    ; Delete the installation directory
    RMDir /r "$INSTDIR\plugins\"
    RMDir /r "$INSTDIR\drivers\"

    ; Delete individual files
	Delete $INSTDIR\portscope.exe
	Delete $INSTDIR\default.xml
	Delete $INSTDIR\libportscope.dll
	Delete $INSTDIR\QtCore4.dll
	Delete $INSTDIR\QtGui4.dll
	Delete $INSTDIR\QtXml4.dll
	Delete $INSTDIR\Uninstall.exe
     
    RMDir "$INSTDIR"

    ; Delete the registry keys
    DeleteRegKey /ifempty HKCU "Software\PortScope"
  
SectionEnd

