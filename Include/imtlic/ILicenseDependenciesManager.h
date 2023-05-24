#pragma once


#include <imtlic/ILicenseDependenciesProvider.h>


namespace imtlic
{


/**
	Common interface for managing of dependencies for a license.
*/
class ILicenseDependenciesManager: virtual public ILicenseDependenciesProvider
{
public:
	/**
		Set list of licenses the given license depends on.
	*/
	virtual void SetLicenseDependencies(const QByteArray& licenseId, const QByteArrayList& dependentIds) = 0;
};


} // namespace imtlic


