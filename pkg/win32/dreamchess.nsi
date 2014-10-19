!define VERSION "0.2.1"

!include "MUI2.nsh"
!include "Library.nsh"

!ifndef SDL_ROOT
!error "Please set SDL_ROOT"
!endif

!ifndef SDL_IMAGE_ROOT
!error "Please set SDL_IMAGE_ROOT"
!endif

!ifndef SDL_MIXER_ROOT
!error "Please set SDL_MIXER_ROOT"
!endif

!ifndef GLEW_ROOT
!error "Please set GLEW_ROOT"
!endif

!ifndef MINGW_ROOT
!error "Please set MINGW_ROOT"
!endif

Name "DreamChess ${VERSION}"
OutFile "dreamchess-${VERSION}-win32.exe"
InstallDir "$PROGRAMFILES\DreamChess"
RequestExecutionLevel admin

Function unix2dos
    ; strips all CRs
    ; and then converts all LFs into CRLFs
    ; (this is roughly equivalent to "cat file | dos2unix | unix2dos")
    ;
    ; usage:
    ;    Push "infile"
    ;    Push "outfile"
    ;    Call unix2dos
    ;
    ; beware that this function destroys $0 $1 $2

    ClearErrors

    Pop $2
    FileOpen $1 $2 w

    Pop $2
    FileOpen $0 $2 r

    Push $2 ; save name for deleting

    IfErrors unix2dos_done

    ; $0 = file input (opened for reading)
    ; $1 = file output (opened for writing)

unix2dos_loop:
    ; read a byte (stored in $2)
    FileReadByte $0 $2
    IfErrors unix2dos_done ; EOL
    ; skip CR
    StrCmp $2 13 unix2dos_loop
    ; if LF write an extra CR
    StrCmp $2 10 unix2dos_cr unix2dos_write

unix2dos_cr:
    FileWriteByte $1 13

unix2dos_write:
    ; write byte
    FileWriteByte $1 $2
    ; read next byte
    Goto unix2dos_loop

unix2dos_done:

    ; close files
    FileClose $0
    FileClose $1

    ; delete original
    Pop $0
    Delete $0

FunctionEnd

!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp" 
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange-uninstall.bmp" 
!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING

Var MUI_TEMP
Var STARTMENU_FOLDER
Var ALREADY_INSTALLED

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "../../COPYING"

!insertmacro MUI_PAGE_DIRECTORY

!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\DreamChess"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "StartMenuFolder"
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "DreamChess"

!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_TEXT "Create Desktop Icon"
!define MUI_FINISHPAGE_RUN_FUNCTION "DesktopShortcut"

Function .onInit
 
  ReadRegStr $R0 HKLM \
  "Software\Microsoft\Windows\CurrentVersion\Uninstall\DreamChess" \
  "UninstallString"
  StrCmp $R0 "" done
 
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
  "DreamChess is already installed. $\n$\nClick 'OK' to remove the \
  previous version or 'Cancel' to cancel this upgrade." \
  IDOK uninst
  Abort
  
;Run the uninstaller
uninst:
  ClearErrors
  Exec $R0
  
done:
 
FunctionEnd

Function "DesktopShortcut"
CreateShortCut "$DESKTOP\DreamChess.lnk" "$INSTDIR\DreamChess.exe"
FunctionEnd

!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Section
  SetOutPath "$INSTDIR"
  File /oname=DreamChess.exe ..\..\build\src\dreamchess.exe
  File /oname=Dreamer.exe ..\..\build\src\dreamer\dreamer.exe

  File /oname=Readme.txt ..\..\README
  File /oname=Authors.txt ..\..\AUTHORS
  File /oname=Copying.txt ..\..\COPYING
  File /oname=Copyright.txt ..\..\COPYRIGHT
 
  ; DLLs
  File "${SDL_ROOT}\SDL.dll"
  File "${SDL_IMAGE_ROOT}\*.dll"
  File "${SDL_MIXER_ROOT}\*.dll"
  File "${MINGW_ROOT}\bin\libwinpthread-1.dll"

  ; Licenses
  SetOutPath "$INSTDIR\Licenses"
  File /oname=LICENSE.SDL.txt "${SDL_ROOT}\README-SDL.txt"
  File "${SDL_IMAGE_ROOT}\LICENSE.*.txt"
  File /oname=LICENSE.SDL_image.txt "${SDL_IMAGE_ROOT}\README.txt"
  File "${SDL_MIXER_ROOT}\LICENSE*.txt"
  File /oname=LICENSE.SDL_mixer.txt "${SDL_MIXER_ROOT}\README.txt"
  File /oname=LICENSE.winpthreads "${MINGW_ROOT}\share\licenses\winpthreads\COPYING"
  Push "$INSTDIR\Licenses\LICENSE.winpthreads"
  Push "$INSTDIR\Licenses\LICENSE.winpthreads.txt"
  Call unix2dos
  File /oname=LICENSE.glew.txt "${GLEW_ROOT}\LICENSE.txt"

  SetOutPath "$INSTDIR\Data"
  File /r /x .* ..\..\data\*

  WriteUninstaller "$INSTDIR\Uninstall.exe"

  IfFileExists "$INSTDIR\DreamChess.exe" 0 new_installation
    StrCpy $ALREADY_INSTALLED 1
  new_installation:

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\DreamChess.lnk" "$INSTDIR\DreamChess.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Readme.lnk" "$INSTDIR\Readme.txt"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DreamChess" "DisplayName" "DreamChess ${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DreamChess" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DreamChess" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DreamChess" "DisplayIcon" "$INSTDIR\DreamChess.exe"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DreamChess" "NoModify" "1"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DreamChess" "NoRepair" "1"
SectionEnd

Section "Uninstall"
  Delete "$INSTDIR\DreamChess.exe"
  Delete "$INSTDIR\Dreamer.exe"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\Readme.txt"
  Delete "$INSTDIR\Authors.txt"
  Delete "$INSTDIR\Copying.txt"
  Delete "$INSTDIR\Copyright.txt"

  RMDir /r "$INSTDIR\Data"
  RMDir /r "$INSTDIR\Licenses"

  Delete "$INSTDIR\Uninstall.exe"
  RMDir "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP

  Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\DreamChess.lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\Readme.lnk"

  Delete "$DESKTOP\DreamChess.lnk"

  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
 
  startMenuDeleteLoop:
    ClearErrors
    RMDir $MUI_TEMP
    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."

    IfErrors startMenuDeleteLoopDone

    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
  startMenuDeleteLoopDone:

  DeleteRegKey HKCU "Software\DreamChess"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DreamChess"
SectionEnd
