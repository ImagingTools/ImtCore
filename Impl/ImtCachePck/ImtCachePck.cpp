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


I_EXPORT_COMPONENT(
			CacheCollectionChangeNotifier,
			"Publishes the changes of a collection mirrored into the cache once the cache has them",
			"Cache Collection Change Notifier");


I_EXPORT_COMPONENT(
			MaterializedTableBuilder,
			"Keeps a table joined from other cache tables, recomputing only the rows a change touched",
			"Materialized Table Builder");


I_EXPORT_COMPONENT(
			ViewBuilder,
			"Keeps a view over cache tables, recreated from a script on every update",
			"View Builder");


} // namespace ImtCachePck
