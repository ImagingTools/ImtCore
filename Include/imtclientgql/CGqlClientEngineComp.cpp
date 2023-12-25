#include <imtclientgql/CGqlClientEngineComp.h>


// Qt includes
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


namespace imtclientgql
{


// public methods

CGqlClientEngineComp::CGqlClientEngineComp()
	:m_urlParamObserver(*this)
{
}


// reimplemented (imtclientgql::IClientProtocolEngine)

QNetworkRequest* CGqlClientEngineComp::CreateNetworkRequest(const imtgql::IGqlRequest& request) const
{
	QNetworkRequest* networkRequest = new QNetworkRequest();
	networkRequest->setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/graphql"));

	m_workingUrl.setScheme("http");
	QString urlString = m_workingUrl.toString();

	if (m_prefixServerAttrPtr.IsValid()){
		QByteArray prefix = *m_prefixServerAttrPtr;
		if (!prefix.startsWith('/')){
			prefix.prepend('/');
		}

		urlString.append(prefix);
	}

	urlString.append("/graphql");

	networkRequest->setUrl(QUrl(urlString));

	imtgql::IGqlContext* contextPtr = request.GetRequestContext();
	if (contextPtr != nullptr){
		QByteArray token = contextPtr->GetToken();
		if (!token.isEmpty()){
			networkRequest->setRawHeader("X-authentication-token", token);
		}
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


