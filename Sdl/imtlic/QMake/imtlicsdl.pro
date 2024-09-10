TARGET = imtlicsdl

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# SDL
SDL_GENERATOR_SCHEME_NAME = Features
SDL_GENERATOR_SCHEME_PATH = $$PWD/../1.0/Features.sdl
SDL_GENERATOR_CPP_OUT_SUBFOLDER = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET/SDL/CPP
SDL_GENERATOR_CPP_NAMESPACE = "sdl::imtlic::Features"
SDL_GENERATOR_QML_OUT_SUBFOLDER = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET/SDL/QML
SDL_GENERATOR_QML_MODULE_NAME = Features
include($(IMTCOREDIR)/Config/QMake/SdlCodeGenerator.pri)
