; =============================================================================
; Inno Setup Script for SORTED Desktop File Organizer
; Windows 11 64-bit Installer Specification
; =============================================================================

#define MyAppName "SORTED"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "SORTED Project"
#define MyAppURL "https://github.com/satya/SORTED"
#define MyAppExeName "SORTED.exe"

[Setup]
AppId={{E581A051-419F-4CB0-9DC7-855734A1B29E}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=..\..\LICENSE
OutputDir=..\..\dist\windows
OutputBaseFilename=SORTED-{#MyAppVersion}-windows-x64-Setup
SetupIconFile=..\..\resources\icons\SORTED.ico
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Distribute only the deployed production directory
Source: "..\..\dist\windows\SORTED\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\resources\icons\SORTED.ico"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; IconFilename: "{app}\resources\icons\SORTED.ico"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
