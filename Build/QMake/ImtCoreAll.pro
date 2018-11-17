# Root of ImtCore project
include ($(ACFDIR)/Config/QMake/Solution.pri)

SUBDIRS += imtbase
imtbase.file = ../../Include/imtbase/QMake/imtbase.pro

SUBDIRS += imt3d
imt3d.file = ../../Include/imt3d/QMake/imt3d.pro

SUBDIRS += imt3proc
imt3proc.file = ../../Include/imt3proc/QMake/imt3proc.pro

SUBDIRS += imtgui
imtgui.file = ../../Include/imtgui/QMake/imtgui.pro

# Component packages
SUBDIRS += ImtCorePck
ImtCorePck.file = ../../Impl/ImtCorePck/QMake/ImtCorePck.pro
ImtCorePck.depends = imt3d

SUBDIRS += ImtCoreLoc
ImtCoreLoc.file = ../../Impl/ImtCoreLoc/QMake/ImtCoreLoc.pro

SUBDIRS += ImtAlgoPck
ImtAlgoPck.file = ../../Impl/ImtAlgoPck/QMake/ImtAlgoPck.pro
ImtAlgoPck.depends = imt3d imt3proc

SUBDIRS += ImtGuiPck
ImtGuiPck.file = ../../Impl/ImtGuiPck/QMake/ImtGuiPck.pro
ImtGuiPck.depends = imt3d imt3proc imtgui


