; 自定义 Countdown 安装器脚本 - 强制卸载旧版
; 基于 Craft 官方 NullsoftInstaller.nsi 修改

; registry stuff
!define regkey "Software\${company}\${productname}"
!define uninstkey "Software\Microsoft\Windows\CurrentVersion\Uninstall\${productname}"
!define startmenu "$SMPROGRAMS\${productname}"
!define uninstaller "uninstall.exe"

;--------------------------------
XPStyle on
ShowInstDetails hide
ShowUninstDetails hide

Name "${productname}"
Caption "${productname} ${version}"
OutFile "${setupname}"

!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "x64.nsh"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_LANGUAGE "English"

SetDateSave on
SetDatablockOptimize on
CRCCheck on
SilentInstall normal

InstallDir "${defaultinstdir}\${productname}"
InstallDirRegKey HKLM "${regkey}" "Install_Dir"

Var /global ExistingInstallation

Function .onInit
    !if ${architecture} == "x64"
        ${IfNot} ${RunningX64}
            MessageBox MB_OK|MB_ICONEXCLAMATION "This installer can only be run on 64-bit Windows."
            Abort
        ${EndIf}
    !endif

    ; 读取旧版安装路径
    ReadRegStr $R0 HKLM "${regkey}" "Install_Dir"
    ${IfNot} $R0 == ""
        StrCpy $ExistingInstallation $R0
    ${EndIf}
FunctionEnd

;--------------------------------
AutoCloseWindow false

; 安装前先强制删除旧版安装目录
Section
    ; 如果检测到旧版安装路径，直接删除整个文件夹
    ${IfNot} $ExistingInstallation == ""
        DetailPrint "检测到旧版安装路径: $ExistingInstallation"
        ; 先尝试正常卸载（如果卸载器存在）
        IfFileExists "$ExistingInstallation\${uninstaller}" 0 +4
            ExecWait '"$ExistingInstallation\${uninstaller}" /S _?=$ExistingInstallation' $0
            DetailPrint "卸载器退出码: $0"
            Sleep 2000
        ${EndIf}
        ; 无论卸载是否成功，强制删除整个安装目录
        DetailPrint "强制删除旧版安装目录: $ExistingInstallation"
        RMDir /r "$ExistingInstallation"
    ${Else}
        DetailPrint "未检测到旧版安装，跳过卸载步骤"
    ${EndIf}

    ; 写入注册表
    WriteRegStr HKLM "${regkey}" "Install_Dir" "$INSTDIR"
    WriteRegStr HKLM "${uninstkey}" "DisplayName" "${productname}"
    WriteRegStr HKLM "${uninstkey}" "UninstallString" '"$INSTDIR\${uninstaller}"'
    WriteRegStr HKLM "${uninstkey}" "DisplayIcon" "$INSTDIR\${executable}"
    WriteRegStr HKLM "${uninstkey}" "URLInfoAbout" "${website}"
    WriteRegStr HKLM "${uninstkey}" "Publisher" "${company}"
    WriteRegStr HKLM "${uninstkey}" "DisplayVersion" "${version}"

    SetOutPath $INSTDIR

    ; 打包所有文件
    File /a /r /x "*.nsi" /x "${setupname}" "${srcdir}\*.*"

    WriteUninstaller "${uninstaller}"
SectionEnd

; 创建快捷方式
Section
    SetShellVarContext all
    CreateDirectory "${startmenu}"
    SetOutPath $INSTDIR
    CreateShortCut "${startmenu}\${productname}.lnk" "$INSTDIR\${executable}"
    CreateShortCut "${startmenu}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

; 卸载器
Section "Uninstall"
    SetShellVarContext all
    DeleteRegKey HKLM "${uninstkey}"
    DeleteRegKey HKLM "${regkey}"
    RMDir /r "$INSTDIR"
    Delete "${startmenu}\${productname}.lnk"
    Delete "${startmenu}\Uninstall.lnk"
    RMDir "${startmenu}"
SectionEnd
