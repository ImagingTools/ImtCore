include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
include($(IACFDIR)/Config/QMake/Iacf.pri)

INCLUDEPATH += $(IMTCOREDIR)/Include
INCLUDEPATH += $(IMTCOREDIR)/Impl
INCLUDEPATH += $(IMTCOREDIR)/$$AUXINCLUDEDIR

LIBS += -L$$PWD/../../Lib/$$COMPILER_DIR

win32{
	LIBS += -ladvapi32 -luser32
}


