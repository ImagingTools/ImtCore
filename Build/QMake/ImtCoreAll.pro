# Root of ImtCore project
include ($(ACFDIR)/Config/QMake/Solution.pri)

SUBDIRS += imt3d
imt3d.file = ../../Include/imt3d/QMake/imt3d.pro


# Component packages
SUBDIRS += ImtCorePck
ImtCorePck.file = ../../Impl/ImtCorePck/QMake/ImtCorePck.pro
ImtCorePck.depends = imt3d

SUBDIRS += ImtCoreGuiPck
ImtCoreGuiPck.file = ../../Impl/ImtCoreGuiPck/QMake/ImtCoreGuiPck.pro
ImtCoreGuiPck.depends = imt3d

SUBDIRS += ImtCoreLoc
ImtCoreLoc.file = ../../Impl/ImtCoreLoc/QMake/ImtCoreLoc.pro





