TARGET = imt2dsdl

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# SDL
SDL_SCHEMES_LIST = $$PWD/../1.0/Geometry.sdl \
					$$PWD/../1.0/PaintSystem.sdl

include($(IMTCOREDIR)/Config/QMake/SdlConfiguration.pri)
