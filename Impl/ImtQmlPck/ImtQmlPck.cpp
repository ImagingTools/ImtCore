// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtQmlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtQmlPck
{


I_EXPORT_PACKAGE(
			"ImtQmlPck",
			"Package for QML-related components",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			QuickApplication,
			"Main entry point for Qt Quick applications using GUI object as main window",
			"Application Main Quick QML");

I_EXPORT_COMPONENT(
			QuickObject,
			"Base graphics object for QuickApplication",
			"Quick QML");

I_EXPORT_COMPONENT(
			GuiQuickWrap,
			"Container object for QuickObject",
			"Quick QML");

I_EXPORT_COMPONENT(
			ObserverQml,
			"Observer QuickObject",
			"Observer qml quickobject");

I_EXPORT_COMPONENT(
			ClientUserContext,
			"Client-specific context for control global properties of QML engine",
			"Client Localization QML-Engine");

I_EXPORT_COMPONENT(
			PageGuiElementModel,
			"Page based model",
			"Page Based Model");

I_EXPORT_COMPONENT(
			CommandsObserverQml,
			"Commands observer qml",
			"Commands Observer Qml QML Provider");

I_EXPORT_COMPONENT(
			QmlObjectCollection,
			"Qml Object Collection",
			"Qml Object Collection");

I_EXPORT_COMPONENT(
			StaticQmlTypeRegistrator,
			"This component registrates all types of the imtqml module for QQmlEngine.",
			"QML Type Registrator QQmlEngine" IM_AUTHOR("Sergey Zastrovnyh"));


I_EXPORT_COMPONENT(
			StaticQmlModuleRegistrator,
			"This component registers dummy Qt modules in order to ensure QML compatibility with Qt5 and Qt6 applications simultaneously.",
			"QML Module Registrator QQmlEngine" IM_AUTHOR("Sergey Zastrovnyh"));


I_EXPORT_COMPONENT(
			StaticQmlBaseTypeRegistrator,
			"This component registrates all types of the imtbase module for QQmlEngine.",
			"QML Type Registrator QQmlEngine" IM_AUTHOR("Sergey Zastrovnyh"));

} // namespace ImtQmlPck


