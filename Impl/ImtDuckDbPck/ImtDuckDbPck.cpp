// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <ImtDuckDbPck/ImtDuckDbPck.h>


// ACF includes
#include <icomp/export.h>


namespace ImtDuckDbPck
{


I_EXPORT_PACKAGE(
			"ImtDuckDbPck",
			"DuckDB-based database component package",
			IM_PROJECT(R"("ImagingTools Core Framework")") IM_COMPANY("ImagingTools"));


I_EXPORT_COMPONENT(
			DuckDatabaseEngine,
			"DuckDB-based database engine",
			"SQL Database DuckDB");


} // namespace ImtDuckDbPck
