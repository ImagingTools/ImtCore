// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtColPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtColPck
{


I_EXPORT_PACKAGE(
			"ImtColvPck",
			"Collection components", 
			IM_PROJECT("ImtCore") IM_COMPANY("ImagingTools") "Collection");

I_EXPORT_COMPONENT(
			ObjectTypeIdFilter,
			"Object type ID filter",
			"Object Type Id Filter");

I_EXPORT_COMPONENT(
			TextFilter,
			"Text filter",
			"Text Filter");

I_EXPORT_COMPONENT(
			DocumentFilter,
			"Document filter",
			"Document Filter");

I_EXPORT_COMPONENT(
			CollectionHeadersProvider,
			"Collection headers provider",
			"Collection Headers Provider");


} // namespace ImtColPck


