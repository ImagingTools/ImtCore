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
			"General collection of the data objects",
			"Object Data Model Collection Container Composition");

I_EXPORT_COMPONENT(
			FileCollection,
			"File-based container of the objects",
			"Object Collection File Data Model Repository");

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


