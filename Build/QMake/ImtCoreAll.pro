# Root of ImtCore project
include ($(ACFDIR)/Config/QMake/Solution.pri)

SUBDIRS += imtauth
imtauth.file = ../../Include/imtauth/QMake/imtauth.pro

SUBDIRS += imtauthgui
imtauthgui.file = ../../Include/imtauthgui/QMake/imtauthgui.pro

SUBDIRS += imtbase
imtbase.file = ../../Include/imtbase/QMake/imtbase.pro

SUBDIRS += imtcrypt
imtcrypt.file = ../../Include/imtcrypt/QMake/imtcrypt.pro

SUBDIRS += imtwidgets
imtwidgets.file = ../../Include/imtwidgets/QMake/imtwidgets.pro

SUBDIRS += imtstyle
imtstyle.file = ../../Include/imtstyle/QMake/imtstyle.pro

SUBDIRS += imtrest
imtrest.file = ../../Include/imtrest/QMake/imtrest.pro

SUBDIRS += imtrepo
imtrepo.file = ../../Include/imtrepo/QMake/imtrepo.pro

SUBDIRS += imtreport
imtreport.file = ../../Include/imtreport/QMake/imtreport.pro

SUBDIRS += imtrepogui
imtrepogui.file = ../../Include/imtrepogui/QMake/imtrepogui.pro

SUBDIRS += imtreportgui
imtreportgui.file = ../../Include/imtreportgui/QMake/imtreportgui.pro

SUBDIRS += imt3d
imt3d.file = ../../Include/imt3d/QMake/imt3d.pro

SUBDIRS += imtgui
imtgui.file = ../../Include/imtgui/QMake/imtgui.pro
imtgui.depends = ../../Include/imtwidgets/QMake/imtwidgets.pro

SUBDIRS += imtloggui
imtloggui.file = ../../Include/imtloggui/QMake/imtloggui.pro

SUBDIRS += imtlog
imtlog.file = ../../Include/imtlog/QMake/imtlog.pro

SUBDIRS += imt3dview
imt3dview.file = ../../Include/imt3dview/QMake/imt3dview.pro

SUBDIRS += imt3dgui
imt3dgui.file = ../../Include/imt3dgui/QMake/imt3dgui.pro

SUBDIRS += imtfile
imtfile.file = ../../Include/imtfile/QMake/imtfile.pro

SUBDIRS += imtzip
imtzip.file = ../../Include/imtzip/QMake/imtzip.pro

SUBDIRS += imtpy
imtpy.file = ../../Include/imtpy/QMake/imtpy.pro

SUBDIRS += imtqml
imtqml.file = ../../Include/imtqml/QMake/imtqml.pro

SUBDIRS += imtlic
imtlic.file = ../../Include/imtlic/QMake/imtlic.pro

SUBDIRS += imtlicgui
imtlicgui.file = ../../Include/imtlicgui/QMake/imtlicgui.pro

## Component packages

SUBDIRS += ImtCorePck
ImtCorePck.file = ../../Impl/ImtCorePck/QMake/ImtCorePck.pro
ImtCorePck.depends = imtbase

SUBDIRS += ImtStylePck
ImtStylePck.file = ../../Impl/ImtStylePck/QMake/ImtStylePck.pro
ImtStylePck.depends = imtstyle

SUBDIRS += ImtAuthPck
ImtAuthPck.file = ../../Impl/ImtAuthPck/QMake/ImtAuthPck.pro
ImtAuthPck.depends = imtbase imtauth

SUBDIRS += ImtAuthGuiPck
ImtAuthGuiPck.file = ../../Impl/ImtAuthGuiPck/QMake/ImtAuthGuiPck.pro
ImtAuthGuiPck.depends = imtbase imtauth imtauthgui imtrepogui

SUBDIRS += ImtLicensePck
ImtLicensePck.file = ../../Impl/ImtLicensePck/QMake/ImtLicensePck.pro
ImtLicensePck.depends = imtbase

SUBDIRS += ImtLicenseGuiPck
ImtLicenseGuiPck.file = ../../Impl/ImtLicenseGuiPck/QMake/ImtLicenseGuiPck.pro
ImtLicenseGuiPck.depends = imtbase imtlic imtlicgui

SUBDIRS += ImtCryptPck
ImtCryptPck.file = ../../Impl/ImtCryptPck/QMake/ImtCryptPck.pro
ImtCryptPck.depends = imtbase imtcrypt

SUBDIRS += ImtRestPck
ImtRestPck.file = ../../Impl/ImtRestPck/QMake/ImtRestPck.pro
ImtRestPck.depends = imtbase imtrest

SUBDIRS += Imt3dPck
Imt3dPck.file = ../../Impl/Imt3dPck/QMake/Imt3dPck.pro
Imt3dPck.depends = imt3d

SUBDIRS += ImtRepoGuiPck
ImtRepoGuiPck.file = ../../Impl/ImtRepoGuiPck/QMake/ImtRepoGuiPck.pro
ImtRepoGuiPck.depends = imtrepo imtrepogui

SUBDIRS += ImtReportGuiPck
ImtReportGuiPck.file = ../../Impl/ImtReportGuiPck/QMake/ImtReportGuiPck.pro
ImtReportGuiPck.depends = imtreport imtreportgui

SUBDIRS += ImtRepoPck
ImtRepoPck.file = ../../Impl/ImtRepoPck/QMake/ImtRepoPck.pro
ImtRepoPck.depends = imtbase

SUBDIRS += ImtReportPck
ImtReportPck.file = ../../Impl/ImtReportPck/QMake/ImtReportPck.pro
ImtReportPck.depends = imtreport imtbase

SUBDIRS += ImtCoreLoc
ImtCoreLoc.file = ../../Impl/ImtCoreLoc/QMake/ImtCoreLoc.pro

SUBDIRS += ImtGuiPck
ImtGuiPck.file = ../../Impl/ImtGuiPck/QMake/ImtGuiPck.pro
ImtGuiPck.depends = imt3d imt3proc imtgui

SUBDIRS += ImtQmlPck
ImtQmlPck.file = ../../Impl/ImtQmlPck/QMake/ImtQmlPck.pro

SUBDIRS += ImtLogPck
ImtLogPck.file = ../../Impl/ImtLogPck/QMake/ImtLogPck.pro
ImtLogPck.depends = imtbase

SUBDIRS += ImtLogGuiPck
ImtLogGuiPck.file = ../../Impl/ImtLogGuiPck/QMake/ImtLogGuiPck.pro
ImtLogGuiPck.depends = imt3d imt3proc imtgui

SUBDIRS += Imt3dViewPck
Imt3dViewPck.file = ../../Impl/Imt3dViewPck/QMake/Imt3dViewPck.pro
Imt3dViewPck.depends = imt3dview

SUBDIRS += Imt3dGuiPck
Imt3dGuiPck.file = ../../Impl/Imt3dGuiPck/QMake/Imt3dGuiPck.pro
Imt3dGuiPck.depends = imt3dview imt3dgui

SUBDIRS += ImtZipPck
ImtZipPck.file = ../../Impl/ImtZipPck/QMake/ImtZipPck.pro
ImtZipPck.depends = imtzip

SUBDIRS += ImtPyPck
ImtPyPck.file = ../../Impl/ImtPyPck/QMake/ImtPyPck.pro
ImtPyPck.depends = imtpy

# Application
!linux{
	SUBDIRS += EagleView
	EagleView.file = ../../Impl/EagleView/QMake/EagleView.pro
	EagleView.depends = Imt3dViewPck Imt3dGuiPck
}

## Tutorials
#	SUBDIRS += imtservletdemo
#	imtservletdemo.file = ../../Docs/Tutorials/Include/imtservletdemo/QMake/imtservletdemo.pro

#	SUBDIRS += ServletDemoPck
#	ServletDemoPck.file = ../../Docs/Tutorials/Impl/ServletDemoPck/QMake/ServletDemoPck.pro
#	ServletDemoPck.depends = ImtRestPck ImtCorePck imtservletdemo

#	SUBDIRS += ServerDemo
#	ServerDemo.file = ../../Docs/Tutorials/Impl/ServerDemo/QMake/ServerDemo.pro
#	ServerDemo.depends = ServletDemoPck

