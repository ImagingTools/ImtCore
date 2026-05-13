// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtDdlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtDdlPck
{


I_EXPORT_PACKAGE(
			"ImtDdlPck",
			"Data Definition Language (DDL) package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
		DdlCodeCreatorProcessorsController,
		"DDL code creator processor controller",
		"DDL Creator Processor Controller");

I_EXPORT_COMPONENT(
		DdlCodeCreatorArgumentsParser,
		"DDL code creator arguments parser",
		"DDL Creator Parser");

I_EXPORT_COMPONENT(
		ConstVarCppCreatorProcessor,
		"Creates ddl for cpp code",
		"DDL Creator Cpp Processor");

I_EXPORT_COMPONENT(
		ConstVarHCreatorProcessor,
		"Creates ddl for h code",
		"DDL Creator Cpp Processor");

I_EXPORT_COMPONENT(
		ConstVarQmlCreatorProcessor,
		"Creates ddl for qml code",
		"DDL Creator Qml Processor");

I_EXPORT_COMPONENT(
		ConstVarQmldirCreatorProcessor,
		"Creates ddl for qmldir code",
		"DDL Creator Qmldir Processor");

I_EXPORT_COMPONENT(
		ConstVarQrcCreatorProcessor,
		"Creates qrc file for qml code",
		"DDL Creator Qrc Qml Processor");


} // namespace ImtDdlPck


