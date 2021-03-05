#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/CProductLicensingInfo.h>


namespace imtlic
{


/**
	Component implementation of IProductLicensingInfo interface with reference to the list of all defined products.
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
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product database containing this object", false, "ProductCollection");
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_REF(imtbase::ICollectionInfo, m_productCollectionCompPtr);
};


} // namespace imtlic


