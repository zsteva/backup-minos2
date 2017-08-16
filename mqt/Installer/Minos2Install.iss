; Script for installing Minos 2

#define MyAppPublisher "G0GJV"
#define MyAppURL "http://minos.sourceforge.net"

; This requires that the ISS Preprocessor be installed
#define SourcePath="C:\temp\mqtInstaller\installFiles"

#define MyAppExename "MqtLogger.exe" 
#define MyAppMajor ""
#define MyAppMinor ""
#define MyAppRev ""
#define MyAppBuild ""
#define MyFullVersion ParseVersion(AddBackslash(SourcePath) + "Bin\" + MyAppExeName, MyAppMajor, MyAppMinor, MyAppRev, MyAppBuild)
#define MyAppVersion Str(MyAppMajor) + "." + Str(MyAppMinor) + "." + Str(MyAppRev)
#define MyAppName "Minos2"

#define MainBinaryName  "MqtLogger.exe"
#define SetupBaseName   "Minos2Install_"
#define AppVersion      GetFileVersion(AddBackslash(SourcePath) + "Bin\" + MainBinaryName)
#define AVF1            Copy(AppVersion, 1, Pos(".", AppVersion) - 1) + "_" + Copy(AppVersion, Pos(".", AppVersion) + 1)
#define AVF2            Copy(AVF1,       1, Pos(".", AVF1      ) - 1) + "_" + Copy(AVF1      , Pos(".", AVF1      ) + 1)
#define AppVersionFile  Copy(AVF2,       1, Pos(".", AVF2      ) - 1)

[Setup]
SourceDir={#SourcePath}
SetupLogging=true
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{617577AD-2218-40EF-8300-431EB77CD246}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName=C:\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
DirExistsWarning=yes
LicenseFile=C:\temp\mqtInstaller\Installer\gpl3.txt

OutputDir=..\InnoInstaller\
OutputBaseFilename={#SetupBaseName + AppVersionFile}
Compression=lzma
SolidCompression=yes
MinVersion=0,6.1

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\Bin\{#MyAppExeName}";WorkingDir: "{app}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\Bin\{#MyAppExeName}";WorkingDir: "{app}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\Bin\{#MyAppExeName}";WorkingDir: "{app}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\Bin\{#MyAppExeName}";WorkingDir: "{app}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
