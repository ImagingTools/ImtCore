#include <imthype/CJobTicketComp.h>


namespace imthype
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CJobTicketComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// Set up the params factory if available
	if (m_jobParamsFactPtr.IsValid()){
		SetParamsFactory([this](const QByteArray& contextId, const QByteArray& typeId) -> iprm::IParamsSetSharedPtr {
			Q_UNUSED(contextId);
			Q_UNUSED(typeId);
			return m_jobParamsFactPtr.CreateInstance();
		});
	}
}


void CJobTicketComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imthype


