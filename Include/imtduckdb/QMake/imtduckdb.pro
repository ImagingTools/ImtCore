TARGET = imtduckdb

QT += sql

include($(ACFCONFIGDIR)/QMake/StaticConfig.pri)
include($$PWD/../../../Config/QMake/ImtCore.pri)

INCLUDEPATH += $$PWD/../../../3rdParty/libduckdb/include

macx {
	contains(QMAKE_HOST.arch, arm64) {
		LIBS += $$PWD/../../../3rdParty/libduckdb/lib/ClangOSX_arm64/libduckdb.dylib
	} else {
		LIBS += $$PWD/../../../3rdParty/libduckdb/lib/ClangOSX_x64/libduckdb.dylib
	}
}
