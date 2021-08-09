#include <imtrest/CHttpClientRequest.h>


namespace imtrest
{


// reimplemented (IHttpClientRequest)


CHttpClientRequest::CHttpClientRequest()
{
	m_state				= IHttpClientRequest::RequestState::CREATED;
	m_networkReply		= nullptr;
	m_networkRequest	= nullptr;
	m_requestType		= QNetworkAccessManager::Operation::UnknownOperation;
}

CHttpClientRequest::CHttpClientRequest(const QNetworkRequest& request, QNetworkAccessManager::Operation requestType): CHttpClientRequest()
{
	QNetworkRequest* newRequest = new QNetworkRequest(request);
	m_networkRequest	= newRequest;
	m_requestType		= requestType;
	m_state				= requestType != QNetworkAccessManager::Operation::PostOperation ? IHttpClientRequest::RequestState::READY_TO_PROCESSING : IHttpClientRequest::RequestState::CREATED;
}

QVariant CHttpClientRequest::GetAttribute(QNetworkRequest::Attribute code) const
{
	return (m_networkReply == nullptr ? QVariant() : m_networkReply->attribute(code));
}


QString CHttpClientRequest::GetErrorString() const
{
	return (m_networkReply == nullptr ? QString() : m_networkReply->errorString());
}


QNetworkReply::NetworkError CHttpClientRequest::GetErrorType() const
{
	return (m_networkReply == nullptr ? QNetworkReply::NetworkError::UnknownNetworkError : m_networkReply->error());
}


QByteArray CHttpClientRequest::GetHeader(const QByteArray& headerName) const
{
	return (m_networkReply == nullptr ? QByteArray() : m_networkReply->rawHeader(headerName));
}


QList<QByteArray> CHttpClientRequest::GetHeaderList() const
{
	return (m_networkReply == nullptr ? QList<QByteArray>() : m_networkReply->rawHeaderList());
}


QNetworkReply* CHttpClientRequest::GetReply() const
{
	return m_networkReply;
}


QByteArray CHttpClientRequest::GetReplyBody() const
{
	return (m_networkReply == nullptr ? QByteArray() : m_networkReply->readAll());
}


QNetworkRequest CHttpClientRequest::GetRequest() const
{
	return *const_cast<const QNetworkRequest*>(m_networkRequest);
}


QByteArray CHttpClientRequest::GetRequestBody() const
{
	return m_requestBody;
}


QNetworkAccessManager::Operation CHttpClientRequest::GetRequestType() const
{
	return m_requestType;
}


int CHttpClientRequest::GetState() const
{
	return m_state;
}


QUrl CHttpClientRequest::GetUrl() const
{
	return (m_networkReply == nullptr ? QUrl() : m_networkReply->url());
}


void CHttpClientRequest::SetReply(QNetworkReply* reply)
{
	if(reply != nullptr){

	istd::CChangeNotifier changeNotifier(this);
	m_networkReply = reply;
	QObject::connect(m_networkReply, &QNetworkReply::finished, [&](){this->OnReplyFinished();});
	m_state = IHttpClientRequest::RequestState::PROCESSING;

	}
}


void CHttpClientRequest::SetRequest(const QNetworkRequest& request)
{
	istd::CChangeNotifier changeNotifier(this);

	QNetworkRequest* newRequest = new QNetworkRequest(request);
	m_networkRequest	= newRequest;

	this->CheckForReady();

}


void CHttpClientRequest::SetRequestBody(const QByteArray& body)
{
	istd::CChangeNotifier changeNotifier(this);
	m_requestBody = body;

	this->CheckForReady();

}


void CHttpClientRequest::SetRequestType(QNetworkAccessManager::Operation requestType)
{
	istd::CChangeNotifier changeNotifier(this);
	m_requestType = requestType;
	this->CheckForReady();
}



// reimplemented (istd::IChangeable)

int CHttpClientRequest::GetSupportedOperations() const
{
	return int(istd::IChangeable::SupportedOperations::SO_COPY);
}


bool CHttpClientRequest::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	Q_UNUSED(mode)

	bool retval = false;

	if (bool (this->GetSupportedOperations() & istd::IChangeable::SupportedOperations::SO_COPY)){

		const CHttpClientRequest* otherHttpClientRequest = dynamic_cast<const CHttpClientRequest*>(&object);

		if(otherHttpClientRequest != nullptr){

			this->SetRequest(otherHttpClientRequest->GetRequest());
			this->SetRequestBody(otherHttpClientRequest->GetRequestBody());
			this->SetRequestType(otherHttpClientRequest->GetRequestType());

			retval = true;

		}
	}
	return retval;
}


// private methods
bool CHttpClientRequest::CheckForReady()
{
	bool retval = false;
	retval = m_requestType != QNetworkAccessManager::Operation::UnknownOperation;
	retval = retval && m_networkRequest != nullptr;

	if (retval){
		if(m_requestType == QNetworkAccessManager::Operation::PostOperation){
			retval = retval && m_requestBody.length();
		}
	}

	if(retval){
		m_state = IHttpClientRequest::RequestState::READY_TO_PROCESSING;
	}

	return retval;
}


void CHttpClientRequest::OnReplyFinished()
{
	istd::CChangeNotifier changeNotifier(this);

	if (m_networkReply != nullptr){

		m_state = IHttpClientRequest::RequestState::FINISED;

		if (m_networkReply->error() == QNetworkReply::NetworkError::NoError){
			m_state |= IHttpClientRequest::RequestState::NO_ERROR;
		}
		else{
			m_state |= IHttpClientRequest::RequestState::ERROR;
		}
	}
}


// private methods







} // namespace imtrest

