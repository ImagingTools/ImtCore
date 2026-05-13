// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtTestPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtTestPck
{


I_EXPORT_PACKAGE(
            "ImtTestPck",
            "Database-related test component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
            TestInfo,
            "Test information",
            "Test Information");

I_EXPORT_COMPONENT(
            TestMetaInfoCreator,
            "Test metainfo creator",
            "Test Metainfo Creator");

I_EXPORT_COMPONENT(
            TestDatabaseDelegate,
            "Test object delegate for SQL table",
            "SQL Test Delegate");



} // namespace ImtTestPck


