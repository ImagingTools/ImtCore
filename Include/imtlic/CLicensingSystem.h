#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtlic/IProductLicensingInfoProvider.h>
#include <imtlic/CProductLicensingInfo.h>


namespace imtlic
{


/**
	Common implementation of a license manager.
	\ingroup LicenseManagement
*/
class CLicensingSystem: public imtbase::TAggergatedObjectCollectionWrap<imtlic::IProductLicensingInfoProvider, CProductLicensingInfo>
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<imtlic::IProductLicensingInfoProvider, CProductLicensingInfo> BaseClass;
	
	CLicensingSystem();

	// reimplemented (imtbase::IProductLicensingInfoProvider)
	virtual const imtbase::ICollectionInfo& GetProductList() const override;
	virtual const imtlic::IProductLicensingInfo* GetProductLicenses(const QByteArray& productId) const override;
};


} // namespace imtlic


