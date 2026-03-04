// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtSdlGenCppPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtSdlGenCppPck
{


I_EXPORT_PACKAGE(
			"ImtSdlGenCppPck",
			"The Schema Definition Language (SDL) Generation package. 2nd generation",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") IM_AUTHOR("Sergey Zastrovnyh"));

I_EXPORT_COMPONENT(
			CxxProcessorsManager,
			"The manager of SDL processors for C++",
			"Schema Definition Language SDL Parser Argument General Manager")

I_EXPORT_COMPONENT(
			SdlClassCodeGenerator,
			"The C++ class generator of SDL types",
			"Schema Definition Language SDL Class Generator")

I_EXPORT_COMPONENT(
			SdlClassTreeModelModificator,
			"The C++ class modificator, adds saving and loading generated SDL types to TreeItemModel",
			"Schema Definition Language SDL Class Generator Tree Item Model")

I_EXPORT_COMPONENT(
			SdlClassJsonModificator,
			"The C++ class modificator, adds loading generated SDL types from JSON value",
			"Schema Definition Language SDL Class Generator JSON")

I_EXPORT_COMPONENT(
			SdlClassGqlModificator,
			"The C++ class modificator, adds loading generated SDL types from GraphQL request",
			"Schema Definition Language SDL Class Generator GraphQL GQL")

I_EXPORT_COMPONENT(
			GqlWrapClassCodeGenerator,
			"The C++ class generator of GraphQL wrap for SDL requests",
			"Schema Definition Language SDL Class Generator GraphQL GQL")

I_EXPORT_COMPONENT(
			GqlHandlerBaseClassGenerator,
			"A C++ class generator of GraphQL handlers for SDL requests",
			"Schema Definition Language SDL Class Generator JSON GraphQL GQL Request")

I_EXPORT_COMPONENT(
			GqlCollectionControllerBaseClassGenerator,
			"A C++ class generator of GraphQL Collection controllers for SDL requests",
			"Schema Definition Language SDL Class Generator JSON GraphQL GQL Request")

I_EXPORT_COMPONENT(
			SdlEnumGenerator,
			"A processor that creating C++ enums from SDL schema",
			"Schema Definition Language SDL Class Generator Enum")

I_EXPORT_COMPONENT(
			SdlUnionGenerator,
			"A processor that creating C++ Union from SDL schema",
			"Schema Definition Language SDL Class Generator Union")

I_EXPORT_COMPONENT(
			QmlRegisterGenerator,
			"A processor that creating C++ QML registration functions",
			"Schema Definition Language SDL QML Class Generator")

I_EXPORT_COMPONENT(
			BaseClassExtender,
			"The C++ class extender adds inheritance to class files",
			"Schema Definition Language Class Generator Base Class")

I_EXPORT_COMPONENT(
			SdlQObjectGenerator,
			"A C++ class generator of QObject implementations",
			"Schema Definition Language SDL Class Generator QObject")

I_EXPORT_COMPONENT(
			SdlUnionObjectGenerator,
			"A C++ class generator of union QObject implementations",
			"Schema Definition Language SDL Class Generator QObject Union")

} // namespace ImtSdlGenCppPck


