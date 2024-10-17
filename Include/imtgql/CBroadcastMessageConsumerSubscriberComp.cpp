#include <imtgql/CBroadcastMessageConsumerSubscriberComp.h>


namespace imtgql
{


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

bool CBroadcastMessageConsumerSubscriberComp::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePtr*/) const
{
	return true;
}


void CBroadcastMessageConsumerSubscriberComp::AddMessage(const MessagePtr& messagePtr)
{
	if (!m_commandIdsAttrPtr.IsValid() || m_commandIdsAttrPtr.GetCount() <= 0){
		return;
	}

	QString source = messagePtr->GetInformationSource();
	QString message = messagePtr->GetInformationDescription();

	QString data = QString("{\"source\": \"%1\", \"message\": \"%2\"}").arg(source).arg(message);

	SetAllSubscriptions(m_commandIdsAttrPtr[0], data.toUtf8());
}


} // namespace imtgql


