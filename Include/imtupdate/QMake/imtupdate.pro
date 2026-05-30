TARGET = imtupdate

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)

INCLUDEPATH += $$PWD/../../../../3rdParty

android{
	QT += androidextras
}
