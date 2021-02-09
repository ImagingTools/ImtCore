#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtbase/TComponentFactoryComp.h>
#include <imtbase/CObjectCollectionComp.h>
#include <imtbase/CDelegatedObjectContainerSupplierComp.h>
#include <imtbase/CMultiStatusManagerComp.h>
#include <imtbase/CStatusManagerComp.h>
#include <imtbase/COptionsListAdapterComp.h>
#include <imtbase/CLocalizedHelpPathProviderComp.h>
#include <imtbase/CCompositeObjectPersistenceComp.h>
#include <imtbase/CParamsManagerAdapterComp.h>
#include <imtbase/CCollectionObjectExtractorComp.h>
#include <imtbase/CLicenseInfo.h>
#include <imtbase/CLicenseManager.h>
#include <imtbase/CLicenseBasedRightsProviderComp.h>
#include <imtbase/CPluginStatusMonitor.h>


/**
	ImtCorePck package
*/
namespace ImtCorePck
{


typedef imtbase::TComponentFactoryComp<iinsp::ISupplier> SupplierFactory;
typedef icomp::TModelCompWrap<imtbase::CObjectCollectionComp> ObjectCollection;
typedef icomp::TModelCompWrap<imtbase::CDelegatedObjectContainerSupplierComp> DelegatedObjectContainerSupplier;
typedef icomp::TModelCompWrap<imtbase::CMultiStatusManagerComp> MultiStatusManager;
typedef icomp::TModelCompWrap<imtbase::CStatusManagerComp> StatusManager;
typedef icomp::TModelCompWrap<imtbase::COptionsListAdapterComp> OptionsListAdapter;
typedef imtbase::CLocalizedHelpPathProviderComp LocalizedHelpPathProvider;
typedef imtbase::CCompositeObjectPersistenceComp CompositeObjectPersistence;
typedef icomp::TModelCompWrap<imtbase::CParamsManagerAdapterComp> ParamsManagerAdapter;
typedef imtbase::CCollectionObjectExtractorComp CollectionObjectExtractor;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtbase::CLicenseInfo,
						imtbase::ILicenseInfo,
						iser::ISerializable>> LicenseInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtbase::CLicenseManager,
						imtbase::ILicenseInfoProvider,
						imtbase::IObjectCollection,
						imtbase::IObjectCollectionInfo,
						imtbase::ICollectionInfo,
						iser::ISerializable>> LicenseManager;
typedef imtbase::CLicenseBasedRightsProviderComp LicenseBasedRightsProvider;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtbase::CPluginStatusMonitor,
						imtbase::IPluginStatusMonitor>> PluginStatusMonitor;

} // namespace ImtCorePck


