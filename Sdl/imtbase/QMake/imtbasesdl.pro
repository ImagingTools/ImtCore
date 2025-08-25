TARGET = imtbasesdl

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# SDL
SDL_SCHEMES_LIST = $$PWD/../1.0/ImtBaseTypes.sdl \
					$$PWD/../1.0/ComplexCollectionFilter.sdl \
					$$PWD/../1.0/ImtCollection.sdl \
					$$PWD/../1.0/DocumentRevision.sdl \
					$$PWD/../1.0/MimeType.sdl \
					$$PWD/../1.0/DocumentCollectionFilter.sdl \
					$$PWD/../1.0/Search.sdl \
					$$PWD/../1.0/Commands.sdl \
					$$PWD/../1.0/Settings.sdl \
					$$PWD/../1.0/CollectionImport.sdl \
					$$PWD/../1.0/ProgressManager.sdl

include($(IMTCOREDIR)/Config/QMake/SdlConfiguration.pri)
