#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>


namespace imtbase
{
	class ICollectionInfo;
	class IObjectCollection;
}


namespace imtlic
{


class IFeatureInfo;
class IFeatureDependenciesProvider;


/**
	Common interface for providing of features.
*/
class IFeatureInfoProvider: virtual public istd::IChangeable
{
public:
	enum MetaInfoTypes
	{
		/**
			List of license infos given as QStringList.
		*/
		MIT_FEATURE_INFO_LIST = idoc::IDocumentMetaInfo::MIT_USER + 1,
	};

	/**
		Get list of all available feature packages.
	*/
	virtual const imtbase::IObjectCollection* GetFeaturePackages() const = 0;

	/**
		Get feature info list.
	*/
	virtual const imtbase::ICollectionInfo& GetFeatureList() const = 0;

	/**
		Get feature info.
	*/
	virtual const IFeatureInfo* GetFeatureInfo(const QByteArray& featureId) const = 0;

	/**
		Get access to the feature dependency information.
	*/
	virtual const IFeatureDependenciesProvider* GetDependenciesInfoProvider() const = 0;

	/**
		Get parent feature info provider list.
	*/
	virtual const imtbase::ICollectionInfo* GetParentFeatureInfoProviderList() const = 0;

	/**
		Get parent feature info provider.
	*/
	virtual const IFeatureInfoProvider* GetParentFeatureInfoProvider(const QByteArray& parentId) const = 0;
};


} // namespace imtlic


