TARGET = ImtControlsGallery

include($(ACFCONFIGDIR)/QMake/ApplicationConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# Include
SUBDIRS += controlsgallerygql
controlsgallerygql.file = ../../Include/controlsgallerygql/QMake/controlsgallerygql.pro
controlsgallerygql.depends = controlsgallerysdl

# Impl
SUBDIRS += ControlsGallery
ControlsGallery.file = ../../Impl/ControlsGallery/QMake/ControlsGallery.pro
ControlsGallery.depends = controlsgallerysdl

SUBDIRS += ControlsGalleryGqlPck
ControlsGalleryGqlPck.file = ../../Impl/ControlsGalleryGqlPck/QMake/ControlsGalleryGqlPck.pro
ControlsGalleryGqlPck.depends = controlsgallerygql

# Qml
SUBDIRS += controlsgalleryqml
controlsgalleryqml.file = ../../Qml/controlsgalleryqml/QMake/controlsgalleryqml.pro
controlsgalleryqml.depends = controlsgallerysdl

# Sdl
SUBDIRS += controlsgallerysdl
controlsgallerysdl.file = ../../Sdl/controlsgallery/QMake/controlsgallerysdl.pro
controlsgallerysdl.depends = imtbasesdl



