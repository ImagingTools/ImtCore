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
}


} // namespace imtlic


