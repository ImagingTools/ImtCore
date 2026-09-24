// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtTagGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtTagGqlPck
{


I_EXPORT_PACKAGE(
			"ImtTagGqlPck",
			"Tags GraphQL component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			TagCollectionController,
			"GraphQL controller of the tag catalog",
			"Tag Collection Controller");

I_EXPORT_COMPONENT(
			TagAssignmentController,
			"GraphQL controller of tag assignments",
			"Tag Assignment Controller");

I_EXPORT_COMPONENT(
			EntityTagsChangeNotifier,
			"Publisher of tag assignment changes",
			"Entity Tags Change Notifier");

I_EXPORT_COMPONENT(
			TagPermissionsProvider,
			"Provider of the tag permissions",
			"Tag Permissions Provider");


} // namespace ImtTagGqlPck


