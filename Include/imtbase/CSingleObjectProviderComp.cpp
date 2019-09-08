#include <imtbase/CSingleObjectProviderComp.h>


namespace imtbase
{


// public methods

CSingleObjectProviderComp::CSingleObjectProviderComp()
	:m_modelUpdateBridge(this)
{
}


// reimplemented (IObjectProvider)

const istd::IChangeable* CSingleObjectProviderComp::GetDataObject() const
{
	return m_dataObjectCompPtr.GetPtr();
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CSingleObjectProviderComp::OnComponentCreated()
{
	if (m_dataObjectModelCompPtr.IsValid()){
		m_dataObjectModelCompPtr->AttachObserver(&m_modelUpdateBridge);
	}
}


void CSingleObjectProviderComp::OnComponentDestroyed()
{
	m_modelUpdateBridge.EnsureModelsDetached();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtbase


