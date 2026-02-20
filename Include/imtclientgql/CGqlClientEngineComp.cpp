// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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

				QByteArray prefix;
				if (m_prefixServerAttrPtr.IsValid()){
					prefix  = *m_prefixServerAttrPtr;
				}

				if (m_applicationInfoCompPtr.IsValid()){
					prefix = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
				}

				if (!prefix.isEmpty()){
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
		QByteArray productId = contextPtr->GetProductId();
		if (!productId.isEmpty()){
			networkRequest->setRawHeader(imtbase::s_productIdHeaderId, productId);
		}

		QByteArray token = contextPtr->GetToken();
		if (!token.isEmpty()){
			networkRequest->setRawHeader(imtbase::s_authenticationTokenHeaderId, token);
		}
	}

	const QByteArray protocolVersion = request.GetProtocolVersion();
	if (!protocolVersion.isEmpty()){
		networkRequest->setRawHeader(imtbase::s_protocolVersionHeaderId, protocolVersion);
	}

	return networkRequest;
}


} // namespace imtclientgql


