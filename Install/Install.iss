#define QTDIR GetEnv("QTDIR")

[Setup]
AppVerName=$AcfProductName$ $AcfVersion:1$
AppVersion=$AcfVersion:1$
AppPublisher=ImagingTools GmbH
AppPublisherURL=www.imagingtools.cos
AppName= ImagingTools Core SDK
OutputBaseFilename=Setup_$AcfProductName$_$LICENSE_TYPE$_v$AcfVersion:1$_$CompilerName$

DefaultDirName={pf}\ImagingTools\$AcfProductName$_v$AcfVersion:1$
WizardImageFile=..\Resources\WizardImageFile.bmp
WizardSmallImageFile=..\Resources\WizardSmallImageFile.bmp
SetupIconFile=..\..\Docs\Images\Icons\Logo.ico
AlwaysShowDirOnReadyPage=True
DefaultGroupName=ImagingTools\ACF-Solutions
DisableProgramGroupPage=True
UninstallDisplayIcon=..\..\Docs\Images\Icons\Logo.ico
UninstallDisplayName=$AcfProductName$ $AcfVersion:1$
OutputDir=..\Setup
LicenseFile=$LICENSE_INSTALL_PATH$
VersionInfoDescription=$AcfProductName$ v. $AcfVersion:1$ ($AcfRawVersion:1$) by $AcfCompanyName$
AppComments=Compiled using $CompilerName$, based on ACF v. $AcfVersion:0$ ($AcfRawVersion:0$) and QT $AcfVersion:1023$, see www.imagingtools.co to find out more about ImagingTools Core SDK
AppCopyright=Copyright (C) 2017-2021 ImagingTools GmbH
VersionInfoVersion=$AcfVersion:1$
ChangesEnvironment=yes

[Files]
Source: ..\Temp\Bin\*.exe; DestDir: {app}\Bin; Flags: recursesubdirs; Components: binaryComp
Source: ..\Temp\Bin\*.arp; DestDir: {app}\Bin; Flags: recursesubdirs; Components: binaryComp
Source: ..\Temp\Bin\*.pdb; DestDir: {app}\Bin; Flags: recursesubdirs; Components: binaryComp
Source: ..\Temp\Lib\*.lib; DestDir: {app}\Lib; Flags: recursesubdirs; Components: binaryComp
Source: ..\Temp\Lib\*.pdb; DestDir: {app}\Lib; Flags: recursesubdirs; Components: binaryComp

Source: ..\Temp\Config\*; DestDir: {app}\Config; Flags: recursesubdirs; Components: standardComp
Source: ..\Temp\Partitura\*; DestDir: {app}\Partitura; Flags: recursesubdirs; Components: standardComp
Source: ..\Temp\*; DestDir: {app}; Components: standardComp
Source: ..\Temp\Docs\*.txt; DestDir: {app}\Docs; Components: standardComp
Source: ..\Temp\Include\*; Excludes: "*.cpp,*.qrc,*.ui,\Resources"; DestDir: {app}\Include; Flags: recursesubdirs; Components: standardComp
Source: ..\Temp\Impl\*; Excludes: "*.cpp"; DestDir: {app}\Impl; Flags: recursesubdirs; Components: standardComp

Source: ..\Temp\Docs\*; DestDir: {app}\Docs; Flags: recursesubdirs; Components: docuComp

Source: ..\Temp\Include\*; DestDir: {app}\Include; Flags: recursesubdirs; Components: sourceComp
Source: ..\Temp\Impl\*; DestDir: {app}\Impl; Flags: recursesubdirs; Components: sourceComp

Source: ..\Temp\Bin\*.dll; DestDir: {app}\Lib; Flags: recursesubdirs; Components: qtBinaryComp

[Languages]
Name: en; MessagesFile: compiler:Default.isl
Name: de; MessagesFile: compiler:Languages\German.isl
Name: pl; MessagesFile: compiler:Languages\Polish.isl

[CustomMessages]
en.StandardType=Typical installation
de.StandardType=Standardinstallation
pl.StandardType=Instalacja standardowa

en.FullType=Full installation
de.FullType=Komplete Installation
pl.FullType=Instalacja kompletna

en.CustomType=Custom installation
de.CustomType=Ausgewählte Komponenten
pl.CustomType=Wybrane komponenty

en.Binaries=Compilated binaries
de.Binaries=Kompilierte binäre Dateien
pl.Binaries=Skompilowane pliki binarne

en.Documentation=Documentation
de.Documentation=Dokumentation
pl.Documentation=Dokumentacja

en.Sources=Source files
de.Sources=Source-Dateien
pl.Sources=Pliki zródlowe

en.QtBinaries=Qt binaries
de.QtBinaries=Qt binäre Dateien
pl.QtBinaries=Pliki binarne Qt

en.AddExtPath=Set ACFSLNDIR to environmental variables
de.AddExtPath=Die Umgebungsvariable ACFSLNDIR setzten
pl.AddExtPath=Ustaw ACFSLNDIR w zmiennych systemowych

en.RegisterExtensions=Register ACF file extensions
de.RegisterExtensions=Die ACF-Dateiereiterungen registrieren
pl.RegisterExtensions=Zarejestruj rozszerzenia plików ACF

[Types]
Name: standardType; Description: {cm:StandardType}
Name: fullType; Description: {cm:FullType}
Name: customType; Description: {cm:CustomType}; Flags: iscustom

[Components]
Name: standardComp; Description: ACF-Solutions; Types: standardType fullType customType
Name: binaryComp; Description: {cm:Binaries}; Types: standardType fullType customType
Name: docuComp; Description: {cm:Documentation}; Types: standardType fullType customType
Name: sourceComp; Description: {cm:Sources}; Types: fullType
Name: qtBinaryComp; Description: {cm:QtBinaries}; Types: standardType fullType customType

[Icons]
Name: "{group}\Compositor"; Filename: {app}\Bin\Release$CompilerName$\Compositor.exe; Components: binaryComp

[Tasks]
Name: AddExtPath; Description: {cm:AddExtPath}; Flags: checkedonce; Components: standardComp
Name: RegisterExtensions; Description: {cm:RegisterExtensions}; Flags: checkedonce; Components: binaryComp

[Registry]
Root: HKCU; Subkey: "Environment"; ValueName: "ACFSLNDIR"; ValueData: "{app}"; ValueType: string; Flags: uninsdeletevalue; Tasks: AddExtPath
Root: HKLM; Subkey: "SOFTWARE\\Classes\\.arx"; ValueType: string; ValueData: arx_auto_file; Tasks: RegisterExtensions
Root: HKLM; Subkey: "SOFTWARE\\Classes\\arx_auto_file"; ValueData: "ACF Registry"; ValueType: string; Flags: uninsdeletevalue; Tasks: RegisterExtensions
Root: HKLM; Subkey: "SOFTWARE\\Classes\\arx_auto_file"; ValueName: EditFlags; ValueData: 0; ValueType: dword; Flags: uninsdeletevalue; Tasks: RegisterExtensions
Root: HKLM; Subkey: "SOFTWARE\\Classes\\arx_auto_file"; ValueName: BrowserFlags; ValueData: 8; ValueType: dword; Flags: uninsdeletevalue; Tasks: RegisterExtensions
Root: HKLM; Subkey: "SOFTWARE\\Classes\\arx_auto_file\\shell\\edit\\command"; ValueData: "{app}\\Bin\\Release$CompilerName$\\Compositor.exe %22%251%22"; ValueType: string; Flags: uninsdeletevalue; Tasks: RegisterExtensions

Root: HKLM; Subkey: "SOFTWARE\\Classes\\.xpc"; ValueType: string; ValueData: xpc_auto_file; Tasks: AddExtPath
Root: HKLM; Subkey: "SOFTWARE\\Classes\\xpc_auto_file"; ValueData: "ACF Configuration"; ValueType: string; Flags: uninsdeletevalue; Tasks: RegisterExtensions
Root: HKLM; Subkey: "SOFTWARE\\Classes\\xpc_auto_file"; ValueName: EditFlags; ValueData: 0; ValueType: dword; Flags: uninsdeletevalue; Tasks: RegisterExtensions
Root: HKLM; Subkey: "SOFTWARE\\Classes\\xpc_auto_file"; ValueName: BrowserFlags; ValueData: 8; ValueType: dword; Flags: uninsdeletevalue; Tasks: RegisterExtensions
Root: HKLM; Subkey: "SOFTWARE\\Classes\\xpc_auto_file\\shell\\edit\\command"; ValueData: "{app}\\Bin\\Release$CompilerName$\\Compositor.exe %22%251%22"; ValueType: string; Flags: uninsdeletevalue; Tasks: RegisterExtensions
