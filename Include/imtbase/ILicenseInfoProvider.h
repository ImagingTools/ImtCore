#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class ILicenseInfo;
class ICollectionInfo;


class ILicenseInfoProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetLicenseList() const = 0;

	/**
		Get license info for the given ID.
	*/
	virtual const imtbase::ILicenseInfo* GetLicenseInfo(const QByteArray& licenseId) const = 0;
};


} // namespace imtbase


