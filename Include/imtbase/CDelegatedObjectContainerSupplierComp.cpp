#include <imtbase/CDelegatedObjectContainerSupplierComp.h>


namespace imtbase
{


// reimplemented (imtbase::IObjectCollectionProvider)

const imtbase::IObjectCollection* CDelegatedObjectContainerSupplierComp::GetObjectCollection() const
{
	if (m_objectProviderCompPtr.IsValid()){
		return m_objectProviderCompPtr->GetObjectCollection();
	}

	if (m_objectCompPtr.IsValid()){
		return m_objectCompPtr.GetPtr();
	}

	return nullptr;
}


} // namespace imtbase


