// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CBroadcastMessageConsumerSubscriberComp.h>


namespace imtservergql
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

	PublishData(m_commandIdsAttrPtr[0], data.toUtf8());
}


} // namespace imtservergql


