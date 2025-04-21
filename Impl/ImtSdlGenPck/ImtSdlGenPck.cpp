#include "ImtSdlGenPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtSdlGenPck
{


I_EXPORT_PACKAGE(
			"ImtSdlGenPck",
			"The Schema Definition Language (SDL) Generation package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") IM_AUTHOR("Sergey Zastrovnyh"));

I_EXPORT_COMPONENT(
			SdlGeneralManager,
			"The manager of SDL processors and argument paresrs",
			"Schema Definition Language SDL Parser Argument General Manager")

I_EXPORT_COMPONENT(
			SdlProcessArgumentsParser,
			"The command line argument parser for processors",
			"Schema Definition Language SDL Argument Parser")

I_EXPORT_COMPONENT(
			SdlClassCodeGenerator,
			"The C++ class generator of SDL types",
			"Schema Definition Language SDL Class Generator")

I_EXPORT_COMPONENT(
			BaseClassExtender,
			"The C++ class extender adds inheritance to class files",
			"Schema Definition Language Class Generator Base Class")

I_EXPORT_COMPONENT(
			QmlCodeGenerator,
			"The QML generator of SDL types",
			"Schema Definition Language SDL QML Generator")

I_EXPORT_COMPONENT(
			QmlCodeMetaGenerator,
			"The QML meta info generator of SDL types",
			"Schema Definition Language SDL QML Generator")

I_EXPORT_COMPONENT(
			QmlCodeCommandIdGenerator,
			"The QML file with Command ids generator of SDL requests",
			"Schema Definition Language SDL QML Generator")

I_EXPORT_COMPONENT(
			QmldirFilePersistence,
			"A qmldir file persistence",
			"Qmldir QT QML File Persistence")

I_EXPORT_COMPONENT(
			QtResourceFilePersistence,
			"A Qt resource file persistence",
			"Qt Resource File Persistence")

I_EXPORT_COMPONENT(
			FileCleanupProcessor,
			"A processor that allows you to clear generated files from unnecessary lines",
			"SDL Class File Cleanup Generated")

I_EXPORT_COMPONENT(
			FileFinalizer,
			"A processor that finalizes files",
			"SDL Class File Finalize Generated")

} // namespace ImtSdlGenPck


