TARGET = controlsgallerysdl

include($(ACFDIR)/Config/QMake/StaticConfig.pri)
include($(IMTCOREDIR)/Config/QMake/ImtCore.pri)

SDL_SCHEMES_LIST = $$files($$_PRO_FILE_PWD_/../1.0/*.sdl, false)

include($(IMTCOREDIR)/Config/QMake/SdlConfiguration.pri)

