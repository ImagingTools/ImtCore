#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtlic
{


class IFeatureInfo;
class IFeatureDependenciesProvider;


/**
	Common information about a feature group.
*/
class IFeatureInfoProvider: virtual public iser::ISerializable
{
public:
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


