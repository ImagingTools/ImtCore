#include <imtauthgql/CLoginStatusSubscriberControllerComp.h>


// ImtCore includes
#include<imtrest/IProtocolEngine.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::CGqlSubscriberControllerCompBase)

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

		QByteArray commandId = m_commandIdsAttrPtr[0];

		SetData(subscriptionId, commandId, data.toUtf8(), networkRequest);
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

		SetAllSubscriptions(m_commandIdsAttrPtr[0], data.toUtf8());
	}
}


// reimplemented (icomp::CComponentBase)

void CLoginStatusSubscriberControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_loginStatusModelCompPtr.IsValid()) {
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


