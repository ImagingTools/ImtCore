#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtlic
{


class QByteArray;
class QByteArrayList;

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
		Get dependents feature list.
	*/
	virtual QByteArrayList GetDependencyFeatureList() const = 0;
};


} // namespace imtlic


