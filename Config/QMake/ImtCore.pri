include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
include($(IACFDIR)/Config/QMake/Iacf.pri)

INCLUDEPATH += $(IMTCOREDIR)/Include
INCLUDEPATH += $(IMTCOREDIR)/Impl
INCLUDEPATH += $(IMTCOREDIR)/$$AUXINCLUDEDIR

LIBS += -L$(IMTCOREDIR)/Lib/$$COMPILER_DIR -limtbase -limtgui -lImtCoreLoc -limt3d -limt3dview -limt3dgui
LIBS += -L$(ACFSLNDIR)/Lib/$$COMPILER_DIR -liauth -liqtauth

