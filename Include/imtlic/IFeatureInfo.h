#pragma once


// ACF includes
#include <istd/IChangeable.h>


class QByteArray;
class QByteArrayList;
class QString;


namespace imtlic
{


/**
	Common information about a feature.
*/
class IFeatureInfo: virtual public istd::IChangeable
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


