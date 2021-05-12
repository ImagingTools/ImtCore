TARGET = Imt3dGuiPck

include($(ACFCONFIGDIR)/QMake/ComponentConfig.pri)
include($(ACFCONFIGDIR)/QMake/QtBaseConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

INCLUDEPATH += $(IMTCOREDIR)/Include

# ACF-Solutions libs
LIBS += -L$(ACFSLNDIR)/Lib/$$COMPILER_DIR -limeas

LIBS += -L../../../Lib/$$COMPILER_DIR -limt3d -limt3dgui -limt3dview -limtgui -limtwidgets -limtstyle

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

