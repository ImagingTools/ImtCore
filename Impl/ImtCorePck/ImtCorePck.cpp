#include "ImtCorePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtCorePck
{


I_EXPORT_PACKAGE(
			"ImtCorePck",
			"ImagingTools core component package",
			IM_PROJECT("\"Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			ObjectCollection,
			"General container of the data objects",
			"Object Data Model Composition");

I_EXPORT_COMPONENT(
			DelegatedObjectContainerSupplier,
			"Delegated provider of a composite object (object container)",
			"Delegator Object Data Model Composition Supplier Provider");

I_EXPORT_COMPONENT(
			MultiStatusManager,
			"Multiple information status manager",
			"Status Information Multiple");

I_EXPORT_COMPONENT(
			StatusManager,
			"Information status manager",
			"Information Status Manager");


} // namespace ImtCorePck


