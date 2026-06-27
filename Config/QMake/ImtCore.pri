include($(ACFDIR)/Config/QMake/GeneralConfig.pri)
exists($(ACFSLNDIR)/Config/QMake/AcfSln.pri){
	include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
	DEFINES += IMT_HAS_ACFSLN_LOC
}
include($(IACFDIR)/Config/QMake/Iacf.pri)

INCLUDEPATH += $$PWD/../../Include
INCLUDEPATH += $$PWD/../../Impl
INCLUDEPATH += $$PWD/../../Sdl
INCLUDEPATH += $$PWD/../../$$AUXINCLUDEDIR
INCLUDEPATH += $$PWD/../../$$AUXINCLUDEDIR/GeneratedFiles

win32{
	LIBS += -ladvapi32 -luser32
}

IMTCOREDIRBUILD = $$(IMTCOREDIR_BUILD)

!isEmpty(IMTCOREDIRBUILD){
	INCLUDEPATH += $$(IMTCOREDIRBUILD)/$$AUXINCLUDEDIR
	LIBS += -L$$(IMTCOREDIRBUILD)/Lib/$$COMPILER_DIR
	message("Use shadow build in: ");
	message($$IMTCOREDIRBUILD);
}
else{
	LIBS += -L$$PWD/../../Lib/$$COMPILER_DIR
}
