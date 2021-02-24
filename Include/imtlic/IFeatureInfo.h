#pragma once


// ACF includes
#include <istd/IChangeable.h>


class QByteArray;
class QByteArrayList;


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
		List of licenses this license depends on.
	*/
	virtual QByteArrayList GetDependencyFeatureList() const = 0;
};


} // namespace imtlic


