#pragma once


#include <imtlic/IFeatureDependenciesProvider.h>


namespace imtlic
{


/**
	Common interface for managing of dependencies for a feature.
*/
class IFeatureDependenciesManager: virtual public IFeatureDependenciesProvider
{
public:
	/**
		Set list of features the given feature depends on.
	*/
	virtual void SetFeatureDependencies(const QByteArray& featureId, const QByteArrayList& dependentIds) = 0;
};


} // namespace imtlic


