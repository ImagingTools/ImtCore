#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtlic
{


/**
	Common information about a feature.
*/
class IFeatureInfo: virtual public iser::ISerializable
{
public:
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


