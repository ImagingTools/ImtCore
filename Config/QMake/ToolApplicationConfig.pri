# The tool configuration file. Moves executables to a new directory. ONLY! created for tools, as Visual Studio generates projects for both Debug and Release simultaneously.

include($(ACFCONFIGDIR)/QMake/ApplicationConfig.pri)
DESTDIR = $$OUT_PWD/../../../Bin/TOOLS_Qt$${QT_MAJOR_VERSION}_$$COMPILER_CODE
