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
#include <imtbase/CPluginStatusMonitor.h>
#include <imtbase/CObjectMetaInfoUniquenessValidatorComp.h>
#include <imtbase/CCollectionObjectContext.h>
#include <imtbase/CFileBasedTranslationManagerComp.h>
#include <imtbase/CTextBasedParamSetRepresentationControllerComp.h>
#include <imtbase/CNumericParamRepresentationControllerComp.h>
#include <imtbase/CTextParamRepresentationControllerComp.h>


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
typedef imtbase::CTextBasedParamSetRepresentationControllerComp TextBasedParamSetRepresentationController;
typedef imtbase::CNumericParamRepresentationControllerComp NumericParamRepresentationController;
typedef imtbase::CTextParamRepresentationControllerComp TextParamRepresentationController;


} // namespace ImtCorePck


