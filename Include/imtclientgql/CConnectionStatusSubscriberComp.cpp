// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CConnectionStatusSubscriberComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


namespace imtclientgql
{


// public methods

CConnectionStatusSubscriberComp::CConnectionStatusSubscriberComp()
	:m_connectionStatus(imtcom::IConnectionStatusProvider::CS_UNKNOWN)
{
}


// protected methods

// reimplemented (imtgql::IGqlSubscriptionClient)

void CConnectionStatusSubscriberComp::OnResponseReceived(
			const QByteArray& subscriptionId,
			const QByteArray& subscriptionData)
{
	if (!m_subscriptionIds.contains(subscriptionId)){
		return;
	}

	if (m_commandAttrPtr.GetCount() <= 0){
		return;
	}

	QJsonDocument document = QJsonDocument::fromJson(subscriptionData);
	if (document.isNull() || !document.isObject()){
		return;
	}

	QJsonObject subscriptionObject = document.object();

	if (subscriptionObject.contains(m_commandAttrPtr[0])){
		QJsonObject dataObject = subscriptionObject.value(m_commandAttrPtr[0]).toObject();
		if (dataObject.contains("status")){
			istd::CChangeNotifier changeNotifier(this);

			QString status = dataObject.value("status").toString();

			if (status == "Connected"){
				m_connectionStatus = imtcom::IConnectionStatusProvider::CS_CONNECTED;
			}
			else if (status == "Disconnected"){
				m_connectionStatus = imtcom::IConnectionStatusProvider::CS_DISCONNECTED;
			}
			else{
				m_connectionStatus = imtcom::IConnectionStatusProvider::CS_UNKNOWN;
			}
		}
	}
}


// reimplemented (imtcom::IConnectionStatusProvider)

imtcom::IConnectionStatusProvider::ConnectionStatus CConnectionStatusSubscriberComp::GetConnectionStatus() const
{
	return m_connectionStatus;
}


} // namespace imtclientgql


