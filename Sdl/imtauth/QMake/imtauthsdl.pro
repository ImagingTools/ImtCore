TARGET = imtauthsdl

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# SDL
SDL_GENERATOR_SCHEME_NAME = Roles
SDL_GENERATOR_SCHEME_PATH = $$PWD/../1.0/Roles.sdl
SDL_GENERATOR_CPP_OUT_SUBFOLDER = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET/SDL/CPP
SDL_GENERATOR_CPP_NAMESPACE = "sdl::imtauth::Roles"
SDL_GENERATOR_QML_OUT_SUBFOLDER = $$OUT_PWD/$$AUXINCLUDEPATH/GeneratedFiles/$$TARGET/SDL/QML
SDL_GENERATOR_QML_MODULE_NAME = imtauthRolesSdl
include($(IMTCOREDIR)/Config/QMake/SdlCodeGenerator.pri)
