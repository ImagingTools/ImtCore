include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
include($(IACFDIR)/Config/QMake/Iacf.pri)

INCLUDEPATH += $$PWD/../../Include
INCLUDEPATH += $$PWD/../..//Impl
INCLUDEPATH += $$PWD/../../$$AUXINCLUDEDIR

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

