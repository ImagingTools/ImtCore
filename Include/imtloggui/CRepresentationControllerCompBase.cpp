#include <imtloggui/CRepresentationControllerCompBase.h>


namespace imtloggui
{


// public methods


// reimplemented (imtlog::IMessageIdFilter)

QSet<int> CRepresentationControllerCompBase::GetMessageFilterIds() const
{
	return QSet<int>();
}


imtlog::IMessageIdFilter::FilterMode CRepresentationControllerCompBase::GetMessageFilterMode() const
{
	return FM_INCLUDE;
}


void CRepresentationControllerCompBase::SetMessageFilterIds(const QSet<int>& ids)
{

}


void CRepresentationControllerCompBase::SetMessageFilterMode(imtlog::IMessageIdFilter::FilterMode mode)
{

}


// private methods

void CRepresentationControllerCompBase::OnTimeRangeChanged()
{

}


// public methods of the embedded class TimeRangeObserver

CRepresentationControllerCompBase::TimeRangeObserver::TimeRangeObserver()
	:m_parent(nullptr)
{
}

void CRepresentationControllerCompBase::TimeRangeObserver::SetParent(CRepresentationControllerCompBase* parent)
{
	m_parent = parent;
}


// protected methods of the embedded class TimeRangeObserver

// reimplemented (imod::CSingleModelObserverBase)

void CRepresentationControllerCompBase::TimeRangeObserver::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_parent != nullptr){
		m_parent->OnTimeRangeChanged();
	}
}


} // namespace imtloggui

