#pragma once


// ACF includes
#include <iser/ISerializable.h>
#include <imtlic/IFeatureInfoProvider.h>


namespace imtlic
{


/**
	Common information about a feature.
*/
class IFeatureInfo: virtual public iser::ISerializable
{
public:
	/**
		Get the feature package for this feature.
	*/
	virtual const imtlic::IFeatureInfoProvider* GetFeaturePackage() const = 0;

	/**
		Get the feature ID.
	*/
	virtual QByteArray GetFeatureId() const = 0;

	/**
		Get the feature name.
	*/
	virtual QString GetFeatureName() const = 0;
};


} // namespace imtlic


