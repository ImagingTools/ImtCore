TARGET = imtcolorsdl

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

SDL_SCHEMES_LIST = $$files($$_PRO_FILE_PWD_/../1.0/*.sdl, false)

include($(IMTCOREDIR)/Config/QMake/SdlConfiguration.pri)

