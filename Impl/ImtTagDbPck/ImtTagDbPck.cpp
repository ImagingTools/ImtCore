// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtTagDbPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtTagDbPck
{


I_EXPORT_PACKAGE(
			"ImtTagDbPck",
			"Tags database delegate component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			TagDbDelegate,
			"Database delegate of the tag catalog",
			"Tag Database Delegate");

I_EXPORT_COMPONENT(
			TagAssignmentDbDelegate,
			"Database delegate of tag assignments",
			"Tag Assignment Database Delegate");


} // namespace ImtTagDbPck


