# MDBX library configuration for QMake

INCLUDEPATH += $$PWD/../../3rdParty/libmdbx/include

win32|unix:!macx {
    LIBS += -L$$PWD/../../3rdParty/libmdbx/lib/$$COMPILER_DIR -lmdbx
}

macx {
    contains(QT_ARCH, arm64) {
        LIBS += -L$$PWD/../../3rdParty/libmdbx/lib/ClangOSX_arm64 -lmdbx
    } else {
        LIBS += -L$$PWD/../../3rdParty/libmdbx/lib/ClangOSX_x64 -lmdbx
    }
}
