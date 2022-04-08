#include "ImtGuiGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtGuiGqlPck
{


I_EXPORT_PACKAGE(
			"ImtGuiGqlPck",
			"ImagingTools core framework GraphQL-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") "GraphQL");

I_EXPORT_COMPONENT(
			PagesDataController,
			"Pages data controller",
			"Pages Data Controller" IM_AUTHOR("\"Viktor Shcherbina\""));

I_EXPORT_COMPONENT(
			CommandsDataController,
			"Commands data controller",
			"Commands Data Controller" IM_AUTHOR("\"Viktor Shcherbina\""));


I_EXPORT_COMPONENT(
			GetStyleDataController,
			"Controller providing application style in GraphQL representation",
			"Style Data Controller" IM_AUTHOR("\"Viktor Shcherbina\""));

I_EXPORT_COMPONENT(
			SettingsController,
			"Provider of GraphQL data respresentation for the application settings",
			"Application Settings Controller" IM_TAG("Client GraphQL Server") IM_AUTHOR("\"Viktor Shcherbina\""));


} // namespace ImtGuiGqlPck


