// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtServicePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtServicePck
{


I_EXPORT_PACKAGE(
			"ImtServicePck",
			"ImagingTools communication framework component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			ObjectCollectionFactory,
			"ObjectCollection factory",
			"Processing ObjectCollection Factory");

I_EXPORT_COMPONENT(
			ConnectionCollectionFactory,
			"ConnectionCollection factory",
			"Processing Connection Collection Factory");

I_EXPORT_COMPONENT(
			ConnectionCollection,
			"ConnectionCollection",
			"Connection Collection");

I_EXPORT_COMPONENT(
			UrlConnectionParamInfo,
			"Url connection param",
			"Url Connection Param");


} // namespace ImtServicePck


