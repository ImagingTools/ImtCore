#include <imtclientgql/CLoginStatusSubscriberComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>


namespace imtclientgql
{


// public methods

CLoginStatusSubscriberComp::CLoginStatusSubscriberComp()
	:m_loginStatus(imtauth::ILoginStatusProvider::LSF_CACHED)
{
}


// protected methods

// reimplemented (imtgql::IGqlSubscriptionClient)

void CLoginStatusSubscriberComp::OnResponseReceived(
			const QByteArray& subscriptionId,
			const QByteArray& subscriptionData)
{
	if (m_commandAttrPtr.GetCount() <= 0){
		return;
	}

	if (subscriptionId == m_subscriptionId){
		QJsonDocument document = QJsonDocument::fromJson(subscriptionData);
		QJsonObject subscriptionObject = document.object();

		if (subscriptionObject.contains(m_commandAttrPtr[0])){
			QJsonObject dataObject = subscriptionObject.value(m_commandAttrPtr[0]).toObject();
			if (dataObject.contains("status")){
				istd::CChangeNotifier changeNotifier(this);

				QString status = dataObject.value("status").toString();

				if (status == "Connected"){
					m_loginStatus = imtauth::ILoginStatusProvider::LSF_LOGGED_IN;
				}
				else if (status == "Disconnected"){
					m_loginStatus = (imtauth::ILoginStatusProvider::LoginStatusFlags)0;
				}
				else{
					m_loginStatus = imtauth::ILoginStatusProvider::LSF_CACHED;
				}
			}
		}
	}
}


// reimplemented (imtauth::ILoginStatusProvider)

int CLoginStatusSubscriberComp::GetLoginStatus(const QByteArray& /*clientId*/) const
{
	return m_loginStatus;
}


} // namespace gmgaws


