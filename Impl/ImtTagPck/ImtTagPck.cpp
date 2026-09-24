// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtTagPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtTagPck
{


I_EXPORT_PACKAGE(
			"ImtTagPck",
			"Tags domain model component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			Tag,
			"Tag",
			"Tag");

I_EXPORT_COMPONENT(
			TagAssignment,
			"Assignment of a tag to an entity",
			"Tag Assignment");

I_EXPORT_COMPONENT(
			TagEvent,
			"Tagged/untagged event",
			"Tag Event");

I_EXPORT_COMPONENT(
			TagAssignmentManager,
			"Manager of tag assignments",
			"Tag Assignment Manager");

I_EXPORT_COMPONENT(
			SystemTagSeeder,
			"Seeder of the default system tags",
			"System Tag Seeder");

I_EXPORT_COMPONENT(
			TaggableEntityType,
			"Registration of a collection whose entities can be tagged",
			"Taggable Entity Type");

I_EXPORT_COMPONENT(
			TagHistoryEventProvider,
			"Shows tag events in the document history",
			"Tag History Event Provider");


} // namespace ImtTagPck


