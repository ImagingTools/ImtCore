#include "ImtSdlGenV2Pck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtSdlGenV2Pck
{


I_EXPORT_PACKAGE(
			"ImtSdlGenV2Pck",
			"The Schema Definition Language (SDL) Generation package. 2nd generation",
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


} // namespace ImtSdlGenV2Pck


