To create installation file you need to set system path for following tools:

vcbuild	-	installed with VisualStudio 2005 in {VCInstallationPath}\VC\vcpackages
doxygen -	to download from http://www.doxygen.org
Inno Setup 5 with third party components -	to download (QUICK START PACK!) from http://www.jrsoftware.org/isdl.php#qsp
moc	-	typically to find in $(QTDIR)
uic	-	typically to find in $(QTDIR)
rcc	-	typically to find in $(QTDIR)

Additionally installation of Qt is needed and QTDIR path must be set.
All environment variables from ExtLib must be registered using RegisterEnvVariables.js or RegisterEnvVariablesLocal.js.

Running of installation script:

_Create_VC9_Installation.bat to create of installation with Visual Studio 2008, or
_Create_VC9_OpenSource.bat to create open source version (no binaries will be included) using ACF tools compiled with Visual Studio 2008.
