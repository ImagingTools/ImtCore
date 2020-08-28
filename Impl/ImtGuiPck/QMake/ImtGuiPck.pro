TARGET = ImtGuiPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

LIBS += -L../../../Lib/$$COMPILER_DIR -limeas -liproc -liauth -liedge
LIBS += -limtgui -limtloggui -limtbase -limtwidgets -li2d -liqt2d

win32
{
	LIBS += -lUser32 -lAdvapi32
}

# Get build output directory of shadow build:
ACFSLNDIRBUILD = $$(ACFSLNDIR_BUILD)
!isEmpty(ACFSLNDIRBUILD){
	INCLUDEPATH += $$(ACFSLNDIR_BUILD)/$$AUXINCLUDEDIR
	LIBS += -L$$(ACFSLNDIR_BUILD)/Lib/$$COMPILER_DIR
}

# Get build output directory of shadow build:
ACFDIRBUILD = $$(ACFDIR_BUILD)
!isEmpty(ACFDIRBUILD){
	INCLUDEPATH += $$(ACFDIR_BUILD)/$$AUXINCLUDEDIR
	LIBS += -L$$(ACFDIR_BUILD)/Lib/$$COMPILER_DIR
}

include($(ACFCONFIGDIR)/QMake/AcfQt.pri)
include($(ACFCONFIGDIR)/QMake/AcfStd.pri)

