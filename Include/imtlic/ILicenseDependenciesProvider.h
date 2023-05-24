#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtlic
{


/**
	Common interface for providing of dependencies for a license.
*/
class ILicenseDependenciesProvider: virtual public istd::IChangeable
{
public:
	/**
		Get list of licenses the given license depends on.
	*/
	virtual QByteArrayList GetLicenseDependencies(const QByteArray& licenseId) const = 0;
};


} // namespace imtlic


