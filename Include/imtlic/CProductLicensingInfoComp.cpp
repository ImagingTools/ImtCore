#include <imtlic/CProductLicensingInfoComp.h>


namespace imtlic
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CProductLicensingInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_collectionInfoCompPtr.IsValid()){
		m_productListPtr = m_collectionInfoCompPtr.GetPtr();
	}
}


} // namespace imtlic


