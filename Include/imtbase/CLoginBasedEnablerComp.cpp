#include <imtbase/CLoginBasedEnablerComp.h>


namespace imtbase
{


// public methods

CLoginBasedEnablerComp::CLoginBasedEnablerComp()
	:m_isEnabled(false)
{
}


// reimplemented (iprm::IEnableableParam)

bool CLoginBasedEnablerComp::IsEnabled() const
{
	return m_isEnabled;
}


bool CLoginBasedEnablerComp::IsEnablingAllowed() const
{
	return false;
}


bool CLoginBasedEnablerComp::SetEnabled(bool /*isEnabled*/)
{
	return false;
}


// reimplemented (iser::ISerializable)

bool CLoginBasedEnablerComp::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CLoginBasedEnablerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_loginModelCompPtr.IsValid()){
		m_loginModelCompPtr->AttachObserver(this);
	}
}


void CLoginBasedEnablerComp::OnComponentDestroyed()
{
	EnsureModelDetached();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::TSingleModelObserverBase)

void CLoginBasedEnablerComp::OnUpdate(const ChangeSet& /*changeSet*/)
{
	bool retVal = m_isEnabled;

	if (m_loginCompPtr.IsValid()){
		retVal = m_loginCompPtr->GetLoggedUser() != nullptr;
	}

	if (retVal != m_isEnabled){
		istd::CChangeNotifier changePtr(this);

		m_isEnabled = retVal;
	}
}


} // namespace imtbase


