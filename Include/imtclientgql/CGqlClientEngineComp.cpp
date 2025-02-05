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
	:m_urlParamObserver(*this)
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
		QString urlString = m_workingUrl.toString();

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

	networkRequest->setUrl(url);

	const imtgql::IGqlContext* contextPtr = request.GetRequestContext();
	if (contextPtr != nullptr){
		imtgql::IGqlContext::Headers headersMap = contextPtr->GetHeaders();

		QByteArray productId = headersMap.value("ProductId");
		if (!productId.isEmpty()){
			networkRequest->setRawHeader("ProductId", productId);
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


// protected methods

void CGqlClientEngineComp::OnUrlParamChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtbase::IUrlParam* urlParamPtr)
{
	Q_ASSERT(urlParamPtr != nullptr);
	if (urlParamPtr != nullptr){
		QUrl url = urlParamPtr->GetUrl();

		m_workingUrl = url;
	}
}


// reimplemented (icomp::CComponentBase)

void CGqlClientEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_urlParamCompPtr.IsValid()) {
		m_urlParamObserver.RegisterObject(m_urlParamCompPtr.GetPtr(), &CGqlClientEngineComp::OnUrlParamChanged);

		m_workingUrl = m_urlParamCompPtr->GetUrl();
	}
}


void CGqlClientEngineComp::OnComponentDestroyed()
{
	m_urlParamObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtclientgql


