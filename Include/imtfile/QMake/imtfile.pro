TARGET = imtfile

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

# The owner and the access list of the private files (CPrivateFilePersistenceComp)
win32 {
	LIBS += -ladvapi32
}

