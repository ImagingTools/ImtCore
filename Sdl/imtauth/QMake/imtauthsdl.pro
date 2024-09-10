TARGET = imtauthsdl

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# SDL
SDL_SOURCES = $$PWD/../1.0/Roles.sdl
SDL_SOURCES += $$PWD/../1.0/Users.sdl
SDL_SOURCES += $$PWD/../1.0/Groups.sdl
include($(IMTCOREDIR)/Config/QMake/SdlCodeGenerator.pri)
