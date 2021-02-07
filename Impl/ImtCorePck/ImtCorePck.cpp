#include "ImtCorePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtCorePck
{


I_EXPORT_PACKAGE(
			"ImtCorePck",
			"ImagingTools core framework component package",
			IM_PROJECT("\"ImagingTools Core\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			SupplierFactory,
			"Supplier factory",
			"Processing Supplier Factory");

I_EXPORT_COMPONENT(
			ObjectCollection,
			"General collection of the data objects",
			"Object Data Model Collection Container Composition");

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

I_EXPORT_COMPONENT(
			OptionsListAdapter,
			"ICollectionInfo-To-IOptionsList adapter",
			"Collection List Options Adapter");

I_EXPORT_COMPONENT(
			LocalizedHelpPathProvider,
			"Localization-dependant help path provider",
			"Help Path Documentation Localization");

I_EXPORT_COMPONENT(
			CompositeObjectPersistence,
			"Localization-dependant help path provider",
			"Object Collection Composite Persistence Bundle");

I_EXPORT_COMPONENT(
			ParamsManagerAdapter,
			"Adapter of object collection to the parameter manager interface",
			"Object Collection Params Manager Adapter");

I_EXPORT_COMPONENT(
			CollectionObjectExtractor,
			"Collection-related object extractor",
			"Object Collection Data Extractor");

I_EXPORT_COMPONENT(
			LicenseInfo,
			"Single license information",
			"License Information");

I_EXPORT_COMPONENT(
			LicenseManager,
			"Management of licenses",
			"License Manager Container Collection");


} // namespace ImtCorePck


