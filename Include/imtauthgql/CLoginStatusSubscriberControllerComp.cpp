// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CLoginStatusSubscriberControllerComp.h>


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QPointer>

// ImtCore includes
#include<imtrest/IProtocolEngine.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtservergql::CGqlPublisherCompBase)

bool CLoginStatusSubscriberControllerComp::RegisterSubscription(
			const QByteArray& subscriptionId,
			const imtgql::CGqlRequest& gqlRequest,
			const imtrest::IRequest& networkRequest,
			QString& errorMessage)
{
	if (!m_loginStatusProviderCompPtr.IsValid()){
		return false;
	}

	if (!m_commandIdsAttrPtr.IsValid() || m_commandIdsAttrPtr.GetCount() <= 0){
		return false;
	}

	const imtrest::CWebSocketRequest* webSocketRequestPtr = dynamic_cast<const imtrest::CWebSocketRequest*>(&networkRequest);
	QPointer<QObject> requestGuard(const_cast<imtrest::CWebSocketRequest*>(webSocketRequestPtr));

	bool result = BaseClass::RegisterSubscription(subscriptionId, gqlRequest, networkRequest, errorMessage);
	if (result){
		QByteArray status;

		int loginStatus = m_loginStatusProviderCompPtr->GetLoginStatus();
		if (loginStatus == 0){
			status = "Disconnected";
		}
		else if (loginStatus == imtauth::ILoginStatusProvider::LSF_LOGGED_IN){
			status = "Connected";
		}

		QString data = QString("{\"status\": \"%1\"}").arg(qPrintable(status));

		QMutexLocker locker(&m_mutex);
		if (requestGuard.isNull()){
			return result;
		}

		bool subscriptionActive = false;
		for (const BaseClass::RequestNetworks& entry : m_registeredSubscribers){
			if (entry.networkRequests.value(subscriptionId) == &networkRequest){
				subscriptionActive = true;
				break;
			}
		}
		if (!subscriptionActive){
			return result;
		}

		QByteArray commandId = m_commandIdsAttrPtr[0];
		PushDataToSubscriber(subscriptionId, commandId, data.toUtf8(), networkRequest);
	}

	return result;
}


// reimplemented (imod::CSingleModelObserverBase)

void CLoginStatusSubscriberControllerComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_requestManagerCompPtr.IsValid()){
		return;
	}

	if (!m_commandIdsAttrPtr.IsValid() || m_commandIdsAttrPtr.GetCount() <= 0){
		return;
	}

	QByteArray status;
	if (changeSet.Contains(0)){
		status = "Disconnected";
	}
	else if (changeSet.Contains(imtauth::ILoginStatusProvider::LSF_LOGGED_IN)){
		status = "Connected";
	}

	if (changeSet.Contains(0) || changeSet.Contains(imtauth::ILoginStatusProvider::LSF_LOGGED_IN)){
		QString data = QString("{\"status\": \"%1\"}").arg(qPrintable(status));

		PublishData(m_commandIdsAttrPtr[0], data.toUtf8());
	}
}


// reimplemented (icomp::CComponentBase)

void CLoginStatusSubscriberControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_loginStatusModelCompPtr.IsValid()){
		m_loginStatusModelCompPtr->AttachObserver(this);
	}
}


void CLoginStatusSubscriberControllerComp::OnComponentDestroyed()
{
	if (m_loginStatusModelCompPtr.IsValid()){
		m_loginStatusModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


} // namespace imtauthgql
