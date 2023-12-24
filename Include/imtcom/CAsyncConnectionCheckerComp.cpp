#include <imtcom/CAsyncConnectionCheckerComp.h>


// Qt includes
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlRequest.h>


namespace imtcom
{


// public methods

CAsyncConnectionCheckerComp::CAsyncConnectionCheckerComp()
	:m_status(CS_UNKNOWN),
	m_managerPtr(nullptr)
{
}


void CAsyncConnectionCheckerComp::SendRequest()
{
	if (m_managerPtr != nullptr){
		QString url;
		if (m_urlParamCompPtr.IsValid()){
			url = m_urlParamCompPtr->GetUrl().toString();
		}

		if (m_prefixServer.IsValid()){
			url.append(*m_prefixServer);
		}

		url.append("/graphql");

		QUrl requestUrl(url);
		QNetworkRequest networkRequest = QNetworkRequest(requestUrl);

		imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, "GetDatabaseStatus");

		QByteArray requestBody = request.GetQuery();
		QNetworkReply* reply = m_managerPtr->post(networkRequest, requestBody);

		connect(reply, &QNetworkReply::finished, this, &CAsyncConnectionCheckerComp::ReplyFinished);
	}
}


void CAsyncConnectionCheckerComp::SetStatus(ConnectionStatus status)
{
	if (m_status != status){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_status = status;
	}
}


// reimplemented (icomp::CComponentBase)

void CAsyncConnectionCheckerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_managerPtr = new QNetworkAccessManager();

	connect(&m_timer, &QTimer::timeout, this, &CAsyncConnectionCheckerComp::OnTimeout);

	m_timer.setSingleShot(true);

	int interval = m_checkIntervalAttrPtr.IsValid() ? *m_checkIntervalAttrPtr * 1000 : 60000;
	m_timer.setInterval(interval);

	m_timer.start();
}


void CAsyncConnectionCheckerComp::OnComponentDestroyed()
{
	m_timer.stop();

	if (m_managerPtr != nullptr){
		m_managerPtr->deleteLater();
		m_managerPtr = nullptr;
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (IConnectionStatusProvider)

IConnectionStatusProvider::ConnectionStatus CAsyncConnectionCheckerComp::GetConnectionStatus() const
{
	return m_status;
}


void CAsyncConnectionCheckerComp::ReplyFinished()
{
	QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
	if(reply){
		QByteArray representationData = reply->readAll();

		imtbase::CTreeItemModel representationModel;

		qDebug() << "representationData" << representationData;

		bool result = representationModel.CreateFromJson(representationData);
		if (result){
			SetStatus(CS_CONNECTED);
		}
		else{
			SetStatus(CS_DISCONNECTED);
		}
	}

	m_timer.start();
}


void CAsyncConnectionCheckerComp::OnTimeout()
{
	SendRequest();
}


} // namespace imtcom


