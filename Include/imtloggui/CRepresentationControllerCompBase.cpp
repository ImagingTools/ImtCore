#include <imtloggui/CRepresentationControllerCompBase.h>


namespace imtloggui
{


// public methods

CRepresentationControllerCompBase::CRepresentationControllerCompBase()
	:m_eventProviderObserver(*this)
{
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CRepresentationControllerCompBase::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_representationCompPtr.IsValid() && m_eventProviderCompPtr.IsValid()){
		imtlog::CTimeRange timeRange;
		if (m_messageFilterParamsCompPtr.IsValid()){
			timeRange = m_messageFilterParamsCompPtr->GetFilterTimeRange();
		}

		imtlog::IEventProvider::EventContainerPtr containerPtr = m_eventProviderCompPtr->GetEvents(m_eventFilterCompPtr.GetPtr(), m_messageFilterParamsCompPtr.GetPtr());

		BuildRepresentation(*m_representationCompPtr, containerPtr, timeRange);
	}
}


// reimplemented (icomp::CComponentBase)

void CRepresentationControllerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_timeRangeProviderCompPtr.IsValid() && m_timeRangeProviderModelCompPtr.IsValid()){
		m_timeRangeProviderModelCompPtr->AttachObserver(this);
	}

	if (m_eventProviderCompPtr.IsValid() && m_eventProviderModelCompPtr.IsValid()){
		m_eventProviderModelCompPtr->AttachObserver(&m_eventProviderObserver);
	}
}


void CRepresentationControllerCompBase::OnComponentDestroyed()
{
	if (m_timeRangeProviderModelCompPtr.IsValid() && m_timeRangeProviderModelCompPtr->IsAttached(this)){
		m_timeRangeProviderModelCompPtr->DetachObserver(this);
	}

	if (m_eventProviderCompPtr.IsValid() && m_eventProviderModelCompPtr->IsAttached(this)){
		m_eventProviderModelCompPtr->DetachObserver(&m_eventProviderObserver);
	}

	BaseClass::OnComponentDestroyed();
}



CRepresentationControllerCompBase::EventProviderObserver::EventProviderObserver(CRepresentationControllerCompBase & parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CSingleModelObserverBase)

void CRepresentationControllerCompBase::EventProviderObserver::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_parent.m_representationCompPtr.IsValid() && m_parent.m_eventProviderCompPtr.IsValid()){
		imtlog::CTimeRange timeRange;
		if (m_parent.m_messageFilterParamsCompPtr.IsValid()){
			timeRange = m_parent.m_messageFilterParamsCompPtr->GetFilterTimeRange();
		}

		imtlog::IEventProvider::EventContainerPtr containerPtr = m_parent.m_eventProviderCompPtr->GetEvents(m_parent.m_eventFilterCompPtr.GetPtr(), m_parent.m_messageFilterParamsCompPtr.GetPtr());

		m_parent.BuildRepresentation(*m_parent.m_representationCompPtr,containerPtr, timeRange);
	}
}

} // namespace imtloggui


