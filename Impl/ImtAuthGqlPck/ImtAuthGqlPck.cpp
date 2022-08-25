#include "ImtAuthGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAuthGqlPck
{


I_EXPORT_PACKAGE(
			"ImtAuthGqlPck",
			"ImagingTools core framework GraphQL-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools") "GraphQL");

I_EXPORT_COMPONENT(
			AuthorizationController,
			"AuthorizationController",
			"AuthorizationController");

I_EXPORT_COMPONENT(
			RoleCollectionController,
			"RoleCollectionController",
			"Role Collection Controller");

I_EXPORT_COMPONENT(
			RoleController,
			"RoleController",
			"Role Controller");



} // namespace ImtAuthGqlPck


