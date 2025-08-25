TARGET = imtauthsdl

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtGuiBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)


# SDL
SDL_SCHEMES_LIST = $$PWD/../1.0/Roles.sdl \
					$$PWD/../1.0/Users.sdl \
					$$PWD/../1.0/Groups.sdl \
					$$PWD/../1.0/Authorization.sdl \
					$$PWD/../1.0/Profile.sdl \
					$$PWD/../1.0/Sessions.sdl \
					$$PWD/../1.0/AuthorizationOptions.sdl

include($(IMTCOREDIR)/Config/QMake/SdlConfiguration.pri)