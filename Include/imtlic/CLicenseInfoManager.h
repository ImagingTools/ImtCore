#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtlic/ILicenseInfoProvider.h>
#include <imtlic/CLicenseDefinition.h>


namespace imtlic
{


/**
	Common implementation of a license container.
	\ingroup LicenseManagement
*/
class CLicenseInfoManager: public imtbase::TAggergatedObjectCollectionWrap<imtlic::ILicenseInfoProvider, CLicenseDefinition>
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<imtlic::ILicenseInfoProvider, CLicenseDefinition> BaseClass;
	
	CLicenseInfoManager();

	// reimplemented (imtbase::ILicenseInfoProvider)
	virtual const imtbase::ICollectionInfo& GetLicenseList() const override;
	virtual const imtlic::ILicenseDefinition* GetLicenseInfo(const QByteArray& licenseId) const override;
};


} // namespace imtlic


