TARGET = imtlicsdl

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

SDL_SCHEMES_LIST = $$PWD/../1.0/Features.sdl \
					$$PWD/../1.0/Licenses.sdl \
					$$PWD/../1.0/Products.sdl

include($(IMTCOREDIR)/Config/QMake/SdlConfiguration.pri)