// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <ImtCachePck/ImtCachePck.h>


// ACF includes
#include <icomp/export.h>


namespace ImtCachePck
{


I_EXPORT_PACKAGE(
			"ImtCachePck",
			"DuckDB cache building component package",
			IM_PROJECT(R"("ImagingTools Core Framework")") IM_COMPANY("ImagingTools"));


I_EXPORT_COMPONENT(
			CacheBuilder,
			"Runs the registered cache table builders in dependency order",
			"Cache Builder");


} // namespace ImtCachePck
