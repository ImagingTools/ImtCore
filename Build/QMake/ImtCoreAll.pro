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

SUBDIRS += imt3dview
imt3dview.file = ../../Include/imt3dview/QMake/imt3dview.pro

SUBDIRS += imt3dgui
imt3dgui.file = ../../Include/imt3dgui/QMake/imt3dgui.pro

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

SUBDIRS += Imt3dViewPck
Imt3dViewPck.file = ../../Impl/Imt3dViewPck/QMake/Imt3dViewPck.pro
Imt3dViewPck.depends = imt3dview

SUBDIRS += Imt3dGuiPck
Imt3dGuiPck.file = ../../Impl/Imt3dGuiPck/QMake/Imt3dGuiPck.pro
Imt3dGuiPck.depends = imt3dview imt3dgui

# Application
SUBDIRS += EagleView
EagleView.file = ../../Impl/EagleView/QMake/EagleView.pro
EagleView.depends = Imt3dViewPck Imt3dGuiPck
