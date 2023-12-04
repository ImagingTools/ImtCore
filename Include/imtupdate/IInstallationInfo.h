#pragma once


// Acf includes
#include <imtupdate/IProductReleaseInfo.h>


namespace imtupdate
{


class IInstallationInfo: virtual public IProductReleaseInfo
{

public:
	virtual QString GetReleaseDownloadLink() const = 0;
	virtual QString GetInstallationHashType() const = 0;
	virtual QString GetInstallationHashValue() const = 0;
};


} // namespace imtupdate

