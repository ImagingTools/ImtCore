#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtlic/CLicenseInfo.h>
#include <imtlic/ILicenseInfoProvider.h>


namespace imtlic
{


/**
	Common implementation of a license manager.
	\ingroup LicenseManagement
*/
class CLicenseInfoManager: public imtbase::TAggergatedObjectCollectionWrap<imtlic::ILicenseInfoProvider, CLicenseInfo>
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<imtlic::ILicenseInfoProvider, CLicenseInfo> BaseClass;
	
	CLicenseInfoManager();

	// reimplemented (imtbase::ILicenseInfoProvider)
	virtual const imtbase::ICollectionInfo& GetLicenseList() const override;
	virtual const imtlic::ILicenseInfo* GetLicenseInfo(const QByteArray & licenseId) const override;
};


} // namespace imtlic


