// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtSdlGenQmlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtSdlGenQmlPck
{


I_EXPORT_PACKAGE(
			"ImtSdlGenQmlPck",
			"The Schema Definition Language (SDL) Generation package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") IM_AUTHOR("Sergey Zastrovnyh"));

I_EXPORT_COMPONENT(
			QmlProcessorsManager,
			"The manager of SDL processors for QML ",
			"Schema Definition Language SDL Parser Argument General Manager")


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
			QmlEnumGenerator,
			"The QML file with ENUM ids generator of SDL enums",
			"Schema Definition Language SDL QML Enum Generator")

I_EXPORT_COMPONENT(
			QmlCodeRequestGenerator,
			"The QML file with SDL request data",
			"Qml Request Generator")


} // namespace ImtSdlGenQmlPck


