# Root of ImtCore project
include ($(ACFCONFIGDIR)/QMake/Solution.pri)

SUBDIRS += imtbase
imtbase.file = ../../Include/imtbase/QMake/imtbase.pro

SUBDIRS += imtwidgets
imtwidgets.file = ../../Include/imtwidgets/QMake/imtwidgets.pro

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

# Component packages
SUBDIRS += ImtCorePck
ImtCorePck.file = ../../Impl/ImtCorePck/QMake/ImtCorePck.pro
ImtCorePck.depends = imtbase

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

# Application
!linux{
	SUBDIRS += EagleView
	EagleView.file = ../../Impl/EagleView/QMake/EagleView.pro
	EagleView.depends = Imt3dViewPck Imt3dGuiPck
}
