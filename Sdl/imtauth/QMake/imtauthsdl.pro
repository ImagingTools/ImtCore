TARGET = imtauthsdl

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)


# SDL
include($(IMTCOREDIR)/Config/QMake/SdlCodeGenerator.pri)

SDL_GENERATOR_SCHEME_PATH = $$PWD/../1.0/Roles.sdl
QMAKE_EXTRA_COMPILERS += $$CreateSdlTarget(Roles, $$SDL_GENERATOR_SCHEME_PATH)
QMAKE_EXTRA_COMPILERS += $$CreateQmlSdlTarget(Roles, $$SDL_GENERATOR_SCHEME_PATH)

SDL_GENERATOR_SCHEME_PATH = $$PWD/../1.0/Users.sdl
QMAKE_EXTRA_COMPILERS += $$CreateSdlTarget(Users, $$SDL_GENERATOR_SCHEME_PATH)
QMAKE_EXTRA_COMPILERS += $$CreateQmlSdlTarget(Users, $$SDL_GENERATOR_SCHEME_PATH)

SDL_GENERATOR_SCHEME_PATH = $$PWD/../1.0/Groups.sdl
QMAKE_EXTRA_COMPILERS += $$CreateSdlTarget(Groups, $$SDL_GENERATOR_SCHEME_PATH)
QMAKE_EXTRA_COMPILERS += $$CreateQmlSdlTarget(Groups, $$SDL_GENERATOR_SCHEME_PATH)
