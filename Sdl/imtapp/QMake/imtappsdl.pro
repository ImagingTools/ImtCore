TARGET = imtappsdl

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# SDL
include($(IMTCOREDIR)/Config/QMake/SdlCodeGenerator.pri)

SDL_GENERATOR_APPLICATION_SCHEME_PATH = $$PWD/../1.0/Application.sdl
SDL_CPP_APPLICATION_COMPILER = $$CreateCppQmlSdlTarget(Application, $$SDL_GENERATOR_APPLICATION_SCHEME_PATH)
$${SDL_CPP_APPLICATION_COMPILER}.input = SDL_GENERATOR_APPLICATION_SCHEME_PATH
QMAKE_EXTRA_COMPILERS += $${SDL_CPP_APPLICATION_COMPILER}

