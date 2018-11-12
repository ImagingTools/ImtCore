#include <imtbase/CSingleObjectProviderComp.h>


namespace imtbase
{


// public methods

CSingleObjectProviderComp::CSingleObjectProviderComp()
	:m_modelUpdateBridge(this)
{
}


// reimplemented (IObjectProvider)

int CSingleObjectProviderComp::GetObjectCount() const
{
	if (m_dataObjectCompPtr.IsValid()){
		return 1;
	}

	return 0;
}


const istd::IChangeable* CSingleObjectProviderComp::GetObject(int objectIndex) const
{
	Q_UNUSED(objectIndex);

	if (m_dataObjectCompPtr.IsValid()){
		return m_dataObjectCompPtr.GetPtr();
	}

	return NULL;
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


