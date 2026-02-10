// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
			ParamsSetFactory,
			"ParamsSet factory",
			"Processing ParamsSet Factory");

I_EXPORT_COMPONENT(
			ObjectCollection,
			"General collection of the data objects",
			"Object Data Model Collection Container Composition");

I_EXPORT_COMPONENT(
			CachedObjectCollection,
			"Cached collection of the data objects",
			"Object Data Model Cached Collection Container Composition");

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
			ParameterLinkController,
			"Parameter link controller",
			"Parameter Controller");

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

I_EXPORT_COMPONENT(
			UrlParam,
			"URL parameter",
			"URL Parameter");

I_EXPORT_COMPONENT(
			MessageFilter,
			"Filter for messages",
			"Filter Message");

I_EXPORT_COMPONENT(
			LoginBasedEnabler,
			"Enabler based on login state",
			"Login Enabler");

I_EXPORT_COMPONENT(
			CommandLineParamsAdapter,
			"Adapter from command line parameters to ParamSet",
			"Adapter ParamSet Command Parameter");

I_EXPORT_COMPONENT(
			TransactionableUndoManager,
			"Undo manager that supports merging notifications into transactions",
			"Undo Manager Transaction");

I_EXPORT_COMPONENT(
			Selection,
			"Ids based selection",
			"Selection Id");

I_EXPORT_COMPONENT(
			SelectionAdapter,
			"Selection adapter",
			"Selection SelectionParam Adapter");

I_EXPORT_COMPONENT(
			SelectionParamAdapter,
			"Selection param adapter",
			"Selection SelectionParam Adapter");

I_EXPORT_COMPONENT(
			SystemStatus,
			"System status",
			"System Status");

I_EXPORT_COMPONENT(
			SelectionIdBinder,
			"Combined list of selected ids",
			"Selection Param Id Ids Bind");

I_EXPORT_COMPONENT(
			TableViewParam,
			"Table view param",
			"Table View Param");

I_EXPORT_COMPONENT(
			ModelUpdateBridge,
			"Model update bridge",
			"Model Update Bridge");

I_EXPORT_COMPONENT(
			DelegatedUrlParam,
			"Delegated url param",
			"Delegated Url Param");
			
I_EXPORT_COMPONENT(
			SelectionParamSummaryRepresentationController,
			"Selection param Summary representation controller",
			"Selection param Summary representation controller");

I_EXPORT_COMPONENT(
			ComplexCollectionFilter,
			"Complex collection filter",
			"Complex Collection Filter");

I_EXPORT_COMPONENT(
			CollectionFilter,
			"Collection filter",
			"Collection Filter");

I_EXPORT_COMPONENT(
			DelegatedParamsSet,
			"Delegated params set",
			"Delegated Params Set");

I_EXPORT_COMPONENT(
			ApplicationInfoController,
			"Application info controller",
			"Application Info Controller");

I_EXPORT_COMPONENT(
			ApplicationInfoDelegator,
			"Application info delegator",
			"Application Info Delegator");


} // namespace ImtCorePck


