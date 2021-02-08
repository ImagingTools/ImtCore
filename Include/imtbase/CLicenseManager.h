#pragma once


// ImtCore includes
#include <imtbase/ILicenseInfoProvider.h>
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtbase/CLicenseInfo.h>


namespace imtbase
{


/**
	License manager implementation.
*/
class CLicenseManager: public TAggergatedObjectCollectionWrap<imtbase::ILicenseInfoProvider, CLicenseInfo>
{
public:
	typedef TAggergatedObjectCollectionWrap<imtbase::ILicenseInfoProvider, CLicenseInfo> BaseClass;
	
	CLicenseManager();

	// reimplemented (imtbase::ILicenseInfoProvider)
	virtual const imtbase::ICollectionInfo& GetLicenseList() const override;
	virtual const imtbase::ILicenseInfo* GetLicenseInfo(const QByteArray & licenseId) const override;
};


} // namespace imtbase


