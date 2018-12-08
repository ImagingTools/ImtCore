include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

INCLUDEPATH += $(IMTCOREDIR)/Include
INCLUDEPATH += $(IMTCOREDIR)/Impl
INCLUDEPATH += $(IMTCOREDIR)/$$AUXINCLUDEDIR

LIBS += -L$(IMTCOREDIR)/Lib/$$COMPILER_DIR
LIBS += -limtbase -limtgui -lImtCoreLoc -liauth -liqtauth

