#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/CProductInstanceInfo.h>


namespace imtlic
{


/**
	Component implementation of IProductInstanceInfo interface with reference to the list of all defined products and customers.
	\ingroup LicenseManagement
*/
class CProductInstanceInfoComp:
			public icomp::CComponentBase,
			public CProductInstanceInfo
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CProductInstanceInfo BaseClass2;

	I_BEGIN_COMPONENT(CProductInstanceInfoComp)
		I_REGISTER_INTERFACE(imtlic::IProductInstanceInfo);
		I_REGISTER_INTERFACE(imtlic::ILicenseInstanceProvider);
		I_REGISTER_INTERFACE(iser::IObject);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product database", false, "ProductCollection");
		I_ASSIGN(m_customerCollectionCompPtr, "CustomerCollection", "Customer database", false, "CustomerCollection");
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_customerCollectionCompPtr);
};


} // namespace imtlic


