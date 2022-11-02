#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
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
#include <imtbase/CPluginStatusMonitor.h>
#include <imtbase/CObjectMetaInfoUniquenessValidatorComp.h>
#include <imtbase/CCollectionObjectContext.h>
#include <imtbase/CFileBasedTranslationManagerComp.h>
#include <imtbase/CParamSetSummaryRepresentationControllerComp.h>
#include <imtbase/CNumericParamSummaryRepresentationControllerComp.h>
#include <imtbase/CTextParamSummaryRepresentationControllerComp.h>
#include <imtbase/CBinaryDataProviderComp.h>
#include <imtbase/CParameterLinkControllerComp.h>
#include <imtbase/CObjectCollectionBasedPersistenceComp.h>
#include <imtbase/CTimeFilterParam.h>
#include <imtbase/CCollectionDataControllerComp.h>
#include <imtbase/CQtResourceLoaderComp.h>
#include <imtbase/CUrlParamComp.h>
#include <imtbase/CMessageFilterComp.h>
#include <imtbase/CLoginBasedEnablerComp.h>


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
						imtbase::CPluginStatusMonitor,
						imtbase::IObjectCollection,
						imtbase::IPluginStatusMonitor>> PluginStatusMonitor;
typedef imtbase::CObjectMetaInfoUniquenessValidatorComp ObjectMetaInfoUniquenessValidator;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtbase::CCollectionObjectContext,
						imtbase::ICollectionObjectContext>> CollectionObjectContext;
typedef icomp::TModelCompWrap<imtbase::CFileBasedTranslationManagerComp> FileBasedTranslationManager;
typedef imtbase::CParamSetSummaryRepresentationControllerComp ParamSetSummaryRepresentationController;
typedef imtbase::CNumericParamSummaryRepresentationControllerComp NumericParamSummaryRepresentationController;
typedef imtbase::CTextParamSummaryRepresentationControllerComp TextParamSummaryRepresentationController;
typedef imtbase::CBinaryDataProviderComp BinaryDataProvider;
typedef ibase::TModelObserverCompWrap<imtbase::CParameterLinkControllerComp> ParameterLinkController;
typedef imtbase::CObjectCollectionBasedPersistenceComp ObjectCollectionBasedPersistence;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						imtbase::CTimeFilterParam,
						imtbase::ITimeFilterParam,
						iser::ISerializable>> TimeFilterParam;
typedef imtbase::CCollectionDataControllerComp CollectionDataController;
typedef imtbase::CQtResourceLoaderComp QtResourceLoaderComp;
typedef icomp::TModelCompWrap<imtbase::CUrlParamComp> UrlParam;
typedef imtbase::CMessageFilterComp MessageFilter;
typedef icomp::TModelCompWrap<imtbase::CLoginBasedEnablerComp> LoginBasedEnabler;


} // namespace ImtCorePck


