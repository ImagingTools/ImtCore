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
            TestCollection,
            "Definition of all tests",
            "Tests collection");

I_EXPORT_COMPONENT(
            TestMetaInfoCreator,
            "Test metainfo creator",
            "Test Metainfo Creator");

I_EXPORT_COMPONENT(
            TestDatabaseDelegate,
            "Test object delegate for SQL table",
            "SQL Test Delegate");



} // namespace ImtTestPck


