#include <imtgql/CGqlClientEngineComp.h>


// Qt includes
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


namespace imtgql
{


// public methods

// reimplemented (imtgql::IClientProtocolEngine)

QNetworkRequest* CGqlClientEngineComp::CreateNetworkRequest(const imtgql::IGqlRequest& request) const
{
	QNetworkRequest* networkRequest = new QNetworkRequest();

	networkRequest->setUrl(QUrl(m_urlAttrPtr->GetValue()));
	imtgql::IGqlContext* contextPtr = request.GetGqlContext();
	if (contextPtr != nullptr){
		QByteArray token = contextPtr->GetToken();
		if (!token.isEmpty()){
			networkRequest->setRawHeader("X-authentication-token", token);
		}
	}

	return networkRequest;
}


} // namespace imtgql


