#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/CProductLicensingInfo.h>


namespace imtlic
{


/**
	Common implementation of IProductLicensingInfo interface.
	\sa ILicenseInfo
	\ingroup LicenseManagement
*/
class CProductLicensingInfoComp:
			public icomp::CComponentBase,
			virtual public CProductLicensingInfo
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CProductLicensingInfo BaseClass2;


	I_BEGIN_COMPONENT(CProductLicensingInfoComp)
		I_REGISTER_INTERFACE(INameParam);
		I_REGISTER_INTERFACE(ILicenseInfoProvider);
		I_REGISTER_INTERFACE(IProductInfo);
		I_REGISTER_INTERFACE(IProductLicensingInfo);
		I_ASSIGN(m_collectionInfoCompPtr, "ProductCollectionInfo", "Product collection info", true, "ProductCollection");
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_REF(imtbase::ICollectionInfo, m_collectionInfoCompPtr);
};


} // namespace imtlic


