TARGET = ImtGui3dPck

include($(ACFDIR)/Config/QMake/ComponentConfig.pri)
include($(ACFDIR)/Config/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

INCLUDEPATH += $(IMTCOREDIR)/Include

# ACF-Solutions libs
LIBS += -L$(ACFSLNDIR)/Lib/$$COMPILER_DIR -limeas

# imtview3d libs
LIBS += -L../../../Lib/$$COMPILER_DIR -limtgui3d -limtview3d

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

include($(ACFDIR)/Config/QMake/AcfQt.pri)
include($(ACFDIR)/Config/QMake/AcfStd.pri)
