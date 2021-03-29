#include <imtlic/CProductLicensingInfoComp.h>


namespace imtlic
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CProductLicensingInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_productCollectionCompPtr.IsValid()){
		m_productListPtr = m_productCollectionCompPtr.GetPtr();
	}

	if (m_accountCollectionCompPtr.IsValid()){
		m_accountListPtr = m_accountCollectionCompPtr.GetPtr();
	}
}


} // namespace imtlic


