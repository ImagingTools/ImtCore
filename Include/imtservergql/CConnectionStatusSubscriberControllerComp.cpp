// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CConnectionStatusSubscriberControllerComp.h>


namespace imtservergql
{


// protected methods

QString CConnectionStatusSubscriberControllerComp::CreateBodySubscription() const
{
	if (!m_connectionStatusProviderCompPtr.IsValid()){
		return QString();
	}

	QString data = QString("{\"status\": \"%1\"}");

	switch (m_connectionStatusProviderCompPtr->GetConnectionStatus())
	{
	case imtcom::IConnectionStatusProvider::CS_CONNECTED:
		data = data.arg("Connected");
		break;
	case imtcom::IConnectionStatusProvider::CS_DISCONNECTED:
		data = data.arg("Disconnected");
		break;
	case imtcom::IConnectionStatusProvider::CS_UNKNOWN:
		data = data.arg("Unknown");
		break;
	}

	return data;
}


// reimplemented (imtservergql::CGqlPublisherCompBase)

bool CConnectionStatusSubscriberControllerComp::RegisterSubscription(
			const QByteArray& subscriptionId,
			const imtgql::CGqlRequest& gqlRequest,
			const imtrest::IRequest& networkRequest,
			QString& errorMessage)
{
	if (m_commandIdsAttrPtr.GetCount() <= 0){
		return false;
	}

	bool result = BaseClass::RegisterSubscription(subscriptionId, gqlRequest, networkRequest, errorMessage);
	if (result){
		PushDataToSubscriber(subscriptionId, m_commandIdsAttrPtr[0], CreateBodySubscription().toUtf8(), networkRequest);
	}

	return result;
}


// reimplemented (imod::CSingleModelObserverBase)

void CConnectionStatusSubscriberControllerComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_commandIdsAttrPtr.GetCount() <= 0){
		return;
	}

	PublishData(m_commandIdsAttrPtr[0], CreateBodySubscription().toUtf8());
}


// reimplemented (icomp::CComponentBase)

void CConnectionStatusSubscriberControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_connectionStatusModelCompPtr.IsValid()){
		m_connectionStatusModelCompPtr->AttachObserver(this);
	}
}


void CConnectionStatusSubscriberControllerComp::OnComponentDestroyed()
{
	if (m_connectionStatusModelCompPtr.IsValid()){
		m_connectionStatusModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


} // namespace imtservergql


