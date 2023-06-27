#include <imtgql/CGqlClientEngineComp.h>


// Qt includes
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


namespace imtgql
{


// public methods

CGqlClientEngineComp::CGqlClientEngineComp()
	:m_textParamObserver(*this)
{
}


// reimplemented (icomp::CComponentBase)

void CGqlClientEngineComp::OnComponentCreated(){
	BaseClass::OnComponentCreated();

	if (m_urlParamCompPtr.IsValid()){
		m_textParamObserver.RegisterObject(m_urlParamCompPtr.GetPtr(), &CGqlClientEngineComp::OnUrlParamChanged);
	}
	else{
		m_workingUrl = m_urlAttrPtr->GetValue();
	}
}


void CGqlClientEngineComp::OnComponentDestroyed(){
	m_textParamObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imtgql::IClientProtocolEngine)

QNetworkRequest* CGqlClientEngineComp::CreateNetworkRequest(const imtgql::IGqlRequest& request) const
{
	QNetworkRequest* networkRequest = new QNetworkRequest();
	networkRequest->setUrl(QUrl(m_workingUrl));
	imtgql::IGqlContext* contextPtr = request.GetGqlContext();
	if (contextPtr != nullptr){
		QByteArray token = contextPtr->GetToken();
		if (!token.isEmpty()){
			networkRequest->setRawHeader("X-authentication-token", token);
		}
	}

	return networkRequest;
}


// protected methods

void CGqlClientEngineComp::OnUrlParamChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const iprm::ITextParam* textParamPtr){
	Q_ASSERT(textParamPtr != nullptr);
	if (textParamPtr != nullptr){
		QString textParam = textParamPtr->GetText();

		m_workingUrl = textParam.toUtf8();
	}
}


} // namespace imtgql


