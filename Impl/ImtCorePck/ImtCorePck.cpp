#include "ImtCorePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtCorePck
{


I_EXPORT_PACKAGE(
			"ImtCorePck",
			"ImagingTools core framework component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

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
			PluginStatusMonitor,
			"Plugin status monitor",
			"Plugin Status Monitor");

I_EXPORT_COMPONENT(
			ObjectMetaInfoUniquenessValidator,
			"Object Metainfo Uniqueness Validator",
			"Object Metainfo Uniqueness Validator");

I_EXPORT_COMPONENT(
			CollectionObjectContext,
			"Collection object context",
			"Collection Object Context");

I_EXPORT_COMPONENT(
			FileBasedTranslationManager,
			"File based text localization manager",
			"Translation" IM_TAG("Localization"));

I_EXPORT_COMPONENT(
			ParamSetSummaryRepresentationController,
			"Summary representation controller for ParamSet",
			"Summary Representation Controller ParamSet ");

I_EXPORT_COMPONENT(
			NumericParamSummaryRepresentationController,
			"Summary representation controller for numeric param",
			"Summary Representation Controller Numeric Param");

I_EXPORT_COMPONENT(
			TextParamSummaryRepresentationController,
			"Summary representation controller for text param",
			"Summary Representation Controller Text Param");

I_EXPORT_COMPONENT(
			BinaryDataProvider,
			"Binary data provider from folder",
			"Binary data provider");

I_EXPORT_COMPONENT(
			ParameterLinkController,
			"Parameter link controller",
			"Parameter Controller");

I_EXPORT_COMPONENT(
			ObjectCollectionBasedPersistence,
			"Object collection based persistence",
			"Object Collection Persistence");

I_EXPORT_COMPONENT(
			TimeFilterParam,
			"Time-based filter parameter",
			"Time Filter Parameter");

I_EXPORT_COMPONENT(
			CollectionDataController,
			"Controller for export and import of data in a collection",
			"Controller Export Import Collection");

I_EXPORT_COMPONENT(
            QtResourceLoaderComp,
            "Controller for load data from resource file in a collection",
            "Controller Resource Loader");


} // namespace ImtCorePck


