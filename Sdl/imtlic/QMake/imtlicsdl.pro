TARGET = imtlicsdl

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# SDL
include($(IMTCOREDIR)/Config/QMake/SdlCodeGenerator.pri)

SDL_GENERATOR_SCHEME_PATH = $$PWD/../1.0/Features.sdl
QMAKE_EXTRA_COMPILERS += $$CreateSdlTarget(Features, $$SDL_GENERATOR_SCHEME_PATH)
QMAKE_EXTRA_COMPILERS += $$CreateQmlSdlTarget(Features, $$SDL_GENERATOR_SCHEME_PATH)

SDL_GENERATOR_SCHEME_PATH = $$PWD/../1.0/Licenses.sdl
QMAKE_EXTRA_COMPILERS += $$CreateSdlTarget(Licenses, $$SDL_GENERATOR_SCHEME_PATH)
QMAKE_EXTRA_COMPILERS += $$CreateQmlSdlTarget(Licenses, $$SDL_GENERATOR_SCHEME_PATH)

SDL_GENERATOR_SCHEME_PATH = $$PWD/../1.0/Products.sdl
QMAKE_EXTRA_COMPILERS += $$CreateSdlTarget(Products, $$SDL_GENERATOR_SCHEME_PATH)
QMAKE_EXTRA_COMPILERS += $$CreateQmlSdlTarget(Products, $$SDL_GENERATOR_SCHEME_PATH)
