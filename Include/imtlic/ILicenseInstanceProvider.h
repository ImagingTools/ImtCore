#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtlic
{


class ILicenseInstance;


/**
	Common interface for a provider of licenses.
	\ingroup LicenseManagement
*/
class ILicenseInstanceProvider: virtual public istd::IChangeable
{
public:
	/**
		Get information about the list of available license instances.
	*/
	virtual const imtbase::ICollectionInfo& GetLicenseInstances() const = 0;

	/**
		Get license info for the given ID.
	*/
	virtual const imtlic::ILicenseInstance* GetLicenseInstance(const QByteArray& licenseId) const = 0;
};


} // namespace imtlic


