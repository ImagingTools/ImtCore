TARGET = imtappsdl

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# SDL
SDL_SCHEMES_LIST = $$PWD/../1.0/Application.sdl

include($(IMTCOREDIR)/Config/QMake/SdlConfiguration.pri)

