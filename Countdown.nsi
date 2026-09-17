!include "MUI2.nsh"
!include "LogicLib.nsh"

; ============ 配置 ============
!define REGKEY "Software\@{company}\@{productname}"
!define UNINSTKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\@{productname}"

Name "@{productname}"
Caption "@{productname} @{version}"
OutFile "@{setupname}"
InstallDir "$PROGRAMFILES64\@{productname}"
RequestExecutionLevel admin

; 安装器和卸载器的图标
!define MUI_ICON "@{icon}"
!define MUI_UNICON "@{unicon}"

; ============ 界面 ============
!insertmacro MUI_PAGE_DIRECTORY

Var StartMenuFolder

!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${REGKEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "English"   ; 默认语言
!insertmacro MUI_LANGUAGE "SimpChinese"

Var OldInstallDir

Function .onInit
  System::Call "kernel32::GetUserDefaultUILanguage() i .r0"
  StrCpy $LANGUAGE $0

  SetRegView 32
  ReadRegStr $OldInstallDir HKLM "${REGKEY}" "Install_Dir"
FunctionEnd

; ============ 安装 ============
Section
  SetRegView 32
  SetShellVarContext all

  ${If} $OldInstallDir != ""
    RMDir /r "$OldInstallDir"
    DeleteRegKey HKLM "${UNINSTKEY}"
    DeleteRegKey HKLM "${REGKEY}"
  ${EndIf}

  SetOutPath $INSTDIR
  File /a "@{dataPath}"
  File /a "@{7za}"
  File /a "@{icon}"
  nsExec::ExecToLog '"$INSTDIR\7za.exe" x -r -y "$INSTDIR\@{dataName}" -o"$INSTDIR"'
  Delete "$INSTDIR\7za.exe"
  Delete "$INSTDIR\@{dataName}"

  WriteRegStr HKLM "${REGKEY}" "Install_Dir" "$INSTDIR"
  WriteRegStr HKLM "${UNINSTKEY}" "DisplayName" "@{productname}"
  WriteRegStr HKLM "${UNINSTKEY}" "DisplayVersion" "@{version}"
  WriteRegStr HKLM "${UNINSTKEY}" "Publisher" "@{company}"
  WriteRegStr HKLM "${UNINSTKEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "${UNINSTKEY}" "DisplayIcon" "$INSTDIR\@{iconname}"
  WriteRegDWORD HKLM "${UNINSTKEY}" "EstimatedSize" "@{estimated_size}"

  WriteUninstaller "$INSTDIR\uninstall.exe"

  CreateShortCut "$DESKTOP\@{productname}.lnk" "$INSTDIR\bin\@{productname}.exe"
SectionEnd

; 快捷方式由 Craft 宏生成
@{shortcuts}

; ============ 卸载 ============
Section "Uninstall"
  SetRegView 32
  SetShellVarContext all

  DeleteRegKey HKLM "${UNINSTKEY}"
  DeleteRegKey HKLM "${REGKEY}"

!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  RMDir /r "$SMPROGRAMS\$StartMenuFolder"
  Delete "$DESKTOP\@{productname}.lnk"

  RMDir /r "$INSTDIR"
SectionEnd
