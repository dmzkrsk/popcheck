;
; This script is based on example1.nsi, but adds uninstall support
; and (optionally) start menu shortcuts.
;
; It will install notepad.exe into a directory that the user selects,
;

!define MUI_PRODUCT "Свежая Почта" ;Define your own software name here
!define MUI_VERSION "1.0" ;Define your own software version here

!include "${NSISDIR}\Contrib\Modern UI\System.nsh"
; The name of the installer
Name "Свежая почта 1.0"

; The file to write
OutFile "pc1-setup.exe"
Caption "Установщик Свежая почта v1.0"
Icon "setup.ico"
LicenseText "Ознакомтесь с условиями лицензии"
LicenseData lic.txt

;--------------------------------
;Language Strings

  ;Description
  LangString DESC_SecFiles ${LANG_RUSSIAN} "Установка основных фалов программы."
  LangString DESC_SecStartUp ${LANG_RUSSIAN} "Запускает программу при каждом старте Windows."
  LangString DESC_SecStartMenu ${LANG_RUSSIAN} "Помещает ярлык программы в меню Пуск."
  LangString DESC_SecDesktop ${LANG_RUSSIAN} "Помещает ярлык программы на рабочий стол."
  LangString DESC_SecUninstall ${LANG_RUSSIAN} "Создаёт программы для удаления."

;--------------------------------
; The default installation directory
InstallDir $PROGRAMFILES\KSPU\PopCheck10

; The text to prompt the user to enter a directory
ComponentText "Выберите компоненты для установки"
; The text to prompt the user to enter a directory
DirText "Выберите каталог для установки"

  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\PhysGluck\PopChecker" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"
;--------------------------------
;Modern UI Configuration

  !define MUI_LICENSEPAGE
  !define MUI_COMPONENTSPAGE
  !define MUI_DIRECTORYPAGE
  !define MUI_STARTMENUPAGE
  
  !define MUI_ABORTWARNING
  
  !define MUI_UNINSTALLER
  !define MUI_UNCONFIRMPAGE
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "Russian"

  !insertmacro MUI_RESERVEFILE_WELCOMEFINISHPAGE
; The stuff to install
Section "Програмные файлы" SecFiles
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  ; Put file there
  File "nmenu.dll"
  File "pop3.dll"
  File "reg.dll"
  File "ticon.dll"
  File "popcheck.exe"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TA_PopCheck10" "DisplayName" "Свежая почта v1.0 (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TA_PopCheck10" "UninstallString" '"$INSTDIR\uninstall.exe"'
SectionEnd

Section "Запускать при старте Windows" SecStartUp
  CreateShortCut "$SMSTARTUP\Свежая почта 1.0.lnk" "$INSTDIR\IEextension.exe"
SectionEnd

Section "Добавить ярлык на рабочий стол" SecDesktop
  CreateShortCut "$DESKTOP\Свежая почта 1.0.lnk" "$INSTDIR\IEextension.exe"
SectionEnd

; optional section
Section "Ярлыки в главном меню" SecStartMenu
  CreateDirectory "$SMPROGRAMS\Свежая почта"
  CreateShortCut "$SMPROGRAMS\Свежая почта\Удалить программу с компьютера.lnk" "$INSTDIR\uninstall.exe" 
  CreateShortCut "$SMPROGRAMS\Свежая почта\Свежая почта 1.0" "$INSTDIR\IEextension.exe"
SectionEnd

UninstallText "Программа Свежая Почта удаляется с вашего компьютера"
Section "Деинсталятор" SecUninstall
  WriteUninstaller "uninstall.exe"
SectionEnd

!insertmacro MUI_FUNCTIONS_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenu} $(DESC_SecStartMenu)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecUnistall} $(DESC_SecUnistall)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} $(DESC_SecDesktop)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecStartUp} $(DESC_SecStartUp)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecFiles} $(DESC_SecFiles)
!insertmacro MUI_FUNCTIONS_DESCRIPTION_END

; special uninstall section.
Section "Uninstall"
  ; remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TA_PopCheck10"
  DeleteRegKey HKCU "Software\PhysGluck\PopChecker"
  ; remove files
  Delete $INSTDIR\pop3.dll
  Delete $INSTDIR\reg.dll
  Delete $INSTDIR\ticon.dll
  Delete $INSTDIR\popcheck.exe
  Delete $INSTDIR\nmenu.dll
  ; MUST REMOVE UNINSTALLER, too
  Delete $INSTDIR\uninstall.exe
  ; remove shortcuts, if any.
  RMDir "$INSTDIR"
  Delete "$SMPROGRAMS\Свежая почта\*.*"
  RMDir "$SMPROGRAMS\Свежая почта"
  Delete "$SMSTARTUP\Свежая почта 1.0.lnk"
  Delete "$DESKTOP\Свежая почта 1.0.lnk"
SectionEnd

; eof
