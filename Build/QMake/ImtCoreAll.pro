# Root of ImtCore project
include ($(ACFDIR)/Config/QMake/Solution.pri)

SUBDIRS += imtbase
imtbase.file = ../../Include/imtbase/QMake/imtbase.pro

SUBDIRS += imtreports
imtreports.file = ../../Include/imtreports/QMake/imtreports.pro

SUBDIRS += imt3d
imt3d.file = ../../Include/imt3d/QMake/imt3d.pro

SUBDIRS += imt3dproc
imt3dproc.file = ../../Include/imt3dproc/QMake/imt3dproc.pro

SUBDIRS += imtgui
imtgui.file = ../../Include/imtgui/QMake/imtgui.pro

# Component packages
SUBDIRS += ImtCorePck
ImtCorePck.file = ../../Impl/ImtCorePck/QMake/ImtCorePck.pro
ImtCorePck.depends = imt3d

SUBDIRS += ImtCoreLoc
ImtCoreLoc.file = ../../Impl/ImtCoreLoc/QMake/ImtCoreLoc.pro

SUBDIRS += ImtProc3dPck
ImtProc3dPck.file = ../../Impl/ImtProc3dPck/QMake/ImtProc3dPck.pro
ImtProc3dPck.depends = imt3d imt3proc

SUBDIRS += ImtGuiPck
ImtGuiPck.file = ../../Impl/ImtGuiPck/QMake/ImtGuiPck.pro
ImtGuiPck.depends = imt3d imt3proc imtgui


