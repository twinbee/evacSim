SetCompress off

!define setup "evacsim_release_0.1.exe"
 
!define company "USM School of Computing"
 
!define prodname "EVacSim"
 
; text file to open in notepad after installation
; linked as "release notes"
!define notefile "todo.txt"
 
; license text file
!define licensefile license.txt
 
; icons must be Microsoft .ICO files
!define icon "eagle.ico"
 
; installer background screen
!define screenimage sportlogo.bmp
 
; file containing list of file-installation commands
!define files "files.nsi"
 
; file containing list of file-uninstall commands
!define unfiles "unfiles.nsi"
 
; registry stuff
 
!define regkey "Software\${company}\${prodname}"
!define uninstkey "Software\Microsoft\Windows\CurrentVersion\Uninstall\${prodname}"
 
!define startmenu "$SMPROGRAMS\${company}\${prodname}"
!define uninstaller "uninstall.exe"

!define exec "Evacuation_Renderer.exe"
 
;--------------------------------
 
XPStyle on
ShowInstDetails show
ShowUninstDetails show
 
Name "${prodname}"
Caption "${prodname}"
 
!ifdef icon
Icon "${icon}"
!endif
 
OutFile "${setup}"
 
SetDateSave on
SetDatablockOptimize on
CRCCheck on

 
InstallDir "$PROGRAMFILES\${company}\${prodname}"
InstallDirRegKey HKLM "${regkey}" ""
 
!ifdef licensefile
LicenseText "License"
LicenseData "${licensefile}"
!endif
 
; pages
; we keep it simple - leave out selectable installation types
 
!ifdef licensefile
Page license
!endif
 
; Page components
Page directory
Page instfiles
 
UninstPage uninstConfirm
UninstPage instfiles
 
;--------------------------------
 
AutoCloseWindow false
ShowInstDetails show
 
 
!ifdef screenimage
 
; set up background image
; uses BgImage plugin
 
Function .onGUIInit
	; extract background BMP into temp plugin directory
	InitPluginsDir
	File /oname=$PLUGINSDIR\1.bmp "${screenimage}"
 
	BgImage::SetBg /NOUNLOAD /FILLSCREEN $PLUGINSDIR\1.bmp
	BgImage::Redraw /NOUNLOAD
FunctionEnd
 
Function .onGUIEnd
	; Destroy must not have /NOUNLOAD so NSIS will be able to unload and delete BgImage before it exits
	BgImage::Destroy
FunctionEnd
 
!endif

; These are the programs that are needed
Section -Prerequisites
  SetOutPath $INSTDIR\Prerequisites
  MessageBox MB_YESNO "Install XVid Codec? Needed for video compression" /SD IDYES IDNO endXvid
    File "XviD-1.1.3-28062007.exe"
    ExecWait "$INSTDIR\Prerequisites\XviD-1.1.3-28062007.exe"
    Goto endXVid
  endXVid:
  MessageBox MB_YESNO "Install ImageMagick? Needed for multi-pane videos" /SD IDYES IDNO endim
    File "ImageMagick-6.4.1-0-Q16-windows-static.exe"
    ExecWait "$INSTDIR\Prerequisites\ImageMagick-6.4.1-0-Q16-windows-static.exe"
    Goto endim
  endim:

SectionEnd
 
; beginning (invisible) section
Section
 
  WriteRegStr HKLM "${regkey}" "Install_Dir" "$INSTDIR"
  ; write uninstall strings
  WriteRegStr HKLM "${uninstkey}" "DisplayName" "${company} ${prodname} (remove only)"
  WriteRegStr HKLM "${uninstkey}" "UninstallString" '"$INSTDIR\${uninstaller}"'
 
!ifdef filetype
  WriteRegStr HKCR "${filetype}" "" "${prodname}"
!endif
 
  WriteRegStr HKCR "${prodname}\Shell\open\command\" "" '"$INSTDIR\${exec} "%1"'
 
!ifdef icon
  WriteRegStr HKCR "${prodname}\DefaultIcon" "" "$INSTDIR\${icon}"
!endif
 
  SetOutPath $INSTDIR
 
 
; package all files, recursively, preserving attributes
; assume files are in the correct places
;File /a "${exec}"
 
!ifdef licensefile
File /a "${licensefile}"
!endif
 
!ifdef notefile
File /a "${notefile}"
!endif
 
!ifdef icon
File /a "${icon}"
!endif
 
; any application-specific files
!ifdef files
!include "${files}"
!endif
 
  WriteUninstaller "${uninstaller}"
  
SectionEnd
 
; create shortcuts
Section
  
  CreateDirectory "${startmenu}"
  SetOutPath "$INSTDIR\renderer" ; for working directory

!ifdef icon
  CreateShortCut "${startmenu}\${prodname}.lnk" "$INSTDIR\renderer\${exec}" "" "$INSTDIR\${icon}" 
!else
  CreateShortCut "${startmenu}\${prodname}.lnk" "$INSTDIR\renderer\${exec}"
!endif
 
!ifdef notefile
  CreateShortCut "${startmenu}\Quick Start Guide.lnk" "$INSTDIR\docs\quick_start_guide.txt"
!endif

!ifdef notefile
  CreateShortCut "${startmenu}\README.lnk" "$INSTDIR\readme.txt"
!endif

!ifdef notefile
  CreateShortCut "${startmenu}\Videos.lnk "$INSTDIR\videos"
!endif

!ifdef notefile
  CreateShortCut "${startmenu}\Scripts.lnk "$INSTDIR\scripts"
!endif

!ifdef notefile
  CreateShortCut "${startmenu}\Maps.lnk "$INSTDIR\obj"
!endif
 
!ifdef helpfile
  CreateShortCut "${startmenu}\Documentation.lnk "$INSTDIR\${helpfile}"
!endif
 
!ifdef website
WriteINIStr "${startmenu}\web site.url" "InternetShortcut" "URL" ${website}
 ; CreateShortCut "${startmenu}\Web Site.lnk "${website}" "URL"
!endif
 
!ifdef notefile
;ExecShell "open" "$INSTDIR\${notefile}"
!endif
 
SectionEnd
 
; Uninstaller
; All section names prefixed by "Un" will be in the uninstaller
 
UninstallText "This will uninstall ${prodname}."
 
!ifdef icon
UninstallIcon "${icon}"
!endif
 
Section "Uninstall"
 
  DeleteRegKey HKLM "${uninstkey}"
  DeleteRegKey HKLM "${regkey}"
  
  Delete "${startmenu}\*.*"
  Delete "${startmenu}"
 
!ifdef licensefile
Delete "$INSTDIR\${licensefile}"
!endif
 
!ifdef notefile
Delete "$INSTDIR\${notefile}"
!endif
 
!ifdef icon
Delete "$INSTDIR\${icon}"
!endif
 
Delete "$INSTDIR\${exec}"
 
!ifdef unfiles
!include "${unfiles}"
!endif
 
SectionEnd