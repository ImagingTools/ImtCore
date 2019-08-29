# Root of ImtCore project
include ($(ACFDIR)/Config/QMake/Solution.pri)

SUBDIRS += imtbase
imtbase.file = ../../Include/imtbase/QMake/imtbase.pro

SUBDIRS += imtrepo
imtrepo.file = ../../Include/imtrepo/QMake/imtrepo.pro

#SUBDIRS += imtrepogui
#imtrepogui.file = ../../Include/imtrepogui/QMake/imtrepogui.pro

SUBDIRS += imtreport
imtreport.file = ../../Include/imtreport/QMake/imtreport.pro

SUBDIRS += imtreportgui
imtreportgui.file = ../../Include/imtreportgui/QMake/imtreportgui.pro

SUBDIRS += imt3d
imt3d.file = ../../Include/imt3d/QMake/imt3d.pro

SUBDIRS += imtgui
imtgui.file = ../../Include/imtgui/QMake/imtgui.pro

# Component packages
SUBDIRS += ImtCorePck
ImtCorePck.file = ../../Impl/ImtCorePck/QMake/ImtCorePck.pro
ImtCorePck.depends = imtbase

SUBDIRS += Imt3dPck
Imt3dPck.file = ../../Impl/Imt3dPck/QMake/Imt3dPck.pro
Imt3dPck.depends = imt3d

SUBDIRS += ImtRepoPck
ImtRepoPck.file = ../../Impl/ImtRepoPck/QMake/ImtRepoPck.pro
ImtRepoPck.depends = imtrepo

#SUBDIRS += ImtRepoGuiPck
#ImtRepoGuiPck.file = ../../Impl/ImtRepoGuiPck/QMake/ImtRepoGuiPck.pro
#ImtRepoGuiPck.depends = imtrepo imtrepogui

SUBDIRS += ImtReportGuiPck
ImtReportGuiPck.file = ../../Impl/ImtReportGuiPck/QMake/ImtReportGuiPck.pro
ImtReportGuiPck.depends = imtreport imtreportgui

SUBDIRS += ImtReportPck
ImtReportPck.file = ../../Impl/ImtReportPck/QMake/ImtReportPck.pro
ImtReportPck.depends = imtreport imtbase

SUBDIRS += ImtCoreLoc
ImtCoreLoc.file = ../../Impl/ImtCoreLoc/QMake/ImtCoreLoc.pro

SUBDIRS += ImtGuiPck
ImtGuiPck.file = ../../Impl/ImtGuiPck/QMake/ImtGuiPck.pro
ImtGuiPck.depends = imt3d imt3proc imtgui
