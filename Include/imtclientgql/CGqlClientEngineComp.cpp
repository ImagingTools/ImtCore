#include <imtclientgql/CGqlClientEngineComp.h>


// Qt includes
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include<imtbase/imtbase.h>


namespace imtclientgql
{


// public methods

CGqlClientEngineComp::CGqlClientEngineComp()
{
}


// reimplemented (imtclientgql::IClientProtocolEngine)

QNetworkRequest* CGqlClientEngineComp::CreateNetworkRequest(const imtgql::IGqlRequest& request, imtbase::IUrlParam* urlParam) const
{
	QNetworkRequest* networkRequest = new QNetworkRequest();
	networkRequest->setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/graphql"));

	QUrl url;

	if (urlParam != nullptr){
		url = urlParam->GetUrl();
	}
	else{
		if (m_serverConnectionCompPtr.IsValid()){
			if (m_serverConnectionCompPtr->GetUrl(imtcom::IServerConnectionInterface::PT_HTTP, url)){
				QString urlString = url.toString();

				if (m_prefixServerAttrPtr.IsValid()){
					QByteArray prefix = *m_prefixServerAttrPtr;
					if (!prefix.startsWith('/')){
						prefix.prepend('/');
					}

					urlString.append(prefix);
				}

				urlString.append("/graphql");
				url = QUrl(urlString);
			}
			else{
				Q_ASSERT(false);

				return nullptr;
			}
		}
		else{
			return nullptr;
		}
	}

	networkRequest->setUrl(url);

	const imtgql::IGqlContext* contextPtr = request.GetRequestContext();
	if (contextPtr != nullptr){
		imtgql::IGqlContext::Headers headersMap = contextPtr->GetHeaders();
		QByteArray productId = headersMap.value("productid");
		if (!productId.isEmpty()){
			networkRequest->setRawHeader("productid", productId);
		}

		QByteArray token = contextPtr->GetToken();
		if (!token.isEmpty()){
			networkRequest->setRawHeader("x-authentication-token", token);
		}
	}

	const QByteArray protocolVersion = request.GetProtocolVersion();
	if (!protocolVersion.isEmpty()){
		networkRequest->setRawHeader(imtbase::s_protocolVersionHeaderId, protocolVersion);
	}

	return networkRequest;
}


} // namespace imtclientgql


