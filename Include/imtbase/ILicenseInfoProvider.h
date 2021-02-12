#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class ILicenseInfo;
class ICollectionInfo;


/**
	Common interface for a provider of licenses.
	\ingroup LicenseManagement
*/
class ILicenseInfoProvider: virtual public istd::IChangeable
{
public:
	/**
		Get information about the list of available licenses.
	*/
	virtual const imtbase::ICollectionInfo& GetLicenseList() const = 0;

	/**
		Get license info for the given ID.
	*/
	virtual const imtbase::ILicenseInfo* GetLicenseInfo(const QByteArray& licenseId) const = 0;
};


} // namespace imtbase


