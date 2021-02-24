#pragma once


// ACF includes
#include <istd/IChangeable.h>


class QByteArray;
class QString;


namespace imtlic
{


/**
	Common interface for providing of dependencies for a feature.
*/
class IFeatureDependenciesProvider: virtual public istd::IChangeable
{
public:
	/**
		Get list of licenses the given license depends on.
	*/
	virtual QByteArrayList GetFeatureDependencies(const QByteArray& featureId) const = 0;
};


} // namespace imtlic


