# Root of ImtCore project
include ($(ACFDIR)/Config/QMake/Solution.pri)

SUBDIRS += imtbase
imtbase.file = ../../Include/imtbase/QMake/imtbase.pro

SUBDIRS += imtrest
imtrest.file = ../../Include/imtrest/QMake/imtrest.pro

SUBDIRS += imtreport
imtreport.file = ../../Include/imtreport/QMake/imtreport.pro

SUBDIRS += imtreportgui
imtreportgui.file = ../../Include/imtreportgui/QMake/imtreportgui.pro

SUBDIRS += imt3d
imt3d.file = ../../Include/imt3d/QMake/imt3d.pro

SUBDIRS += imtgui
imtgui.file = ../../Include/imtgui/QMake/imtgui.pro

SUBDIRS += imtview3d
imtview3d.file = ../../Include/imtview3d/QMake/imtview3d.pro

SUBDIRS += imtgui3d
imtgui3d.file = ../../Include/imtgui3d/QMake/imtgui3d.pro

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

SUBDIRS += ImtView3dPck
ImtView3dPck.file = ../../Impl/ImtView3dPck/QMake/ImtView3dPck.pro
ImtView3dPck.depends = imtview3d

SUBDIRS += ImtGui3dPck
ImtGui3dPck.file = ../../Impl/ImtGui3dPck/QMake/ImtGui3dPck.pro
ImtGui3dPck.depends = imtview3d imtgui3d

# Application
SUBDIRS += EagleView
EagleView.file = ../../Impl/EagleView/QMake/EagleView.pro
EagleView.depends = ImtView3dPck ImtGui3dPck
