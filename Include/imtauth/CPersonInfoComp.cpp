#include <imtauth/CPersonInfoComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CPersonInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_addressProviderCompPtr.IsValid()){
		m_addressProviderPtr = m_addressProviderCompPtr.GetPtr();
	}
}


} // namespace imtauth


