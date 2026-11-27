// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpClientRequest.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtrest
{


CHttpClientRequest::CHttpClientRequest()
	:m_requestType(QNetworkAccessManager::GetOperation),
	m_state(RS_CREATED),
	m_errorType(QNetworkReply::NoError)
{
}


void CHttpClientRequest::SetState(int state)
{
	if (m_state != state){
		istd::CChangeNotifier notifier(this);

		m_state = state;
	}
}


void CHttpClientRequest::SetError(QNetworkReply::NetworkError errorType, const QString& errorString)
{
	istd::CChangeNotifier notifier(this);

	m_errorType = errorType;
	m_errorString = errorString;
}


// reimplemented (imtrest::IHttpClientRequest)

QVariant CHttpClientRequest::GetAttribute(QNetworkRequest::Attribute code) const
{
	return m_replyAttributes.value(code);
}


QString CHttpClientRequest::GetErrorString() const
{
	return m_errorString;
}


QNetworkReply::NetworkError CHttpClientRequest::GetErrorType() const
{
	return m_errorType;
}


QByteArray CHttpClientRequest::GetHeader(const QByteArray& headerName) const
{
	return m_replyHeaders.value(headerName.toLower());
}


QList<QByteArray> CHttpClientRequest::GetHeaderList() const
{
	return m_replyHeaderNames;
}


QNetworkReply* CHttpClientRequest::GetReply() const
{
	return m_replyPtr.data();
}


QByteArray CHttpClientRequest::GetReplyBody() const
{
	return m_replyBody;
}


QNetworkRequest CHttpClientRequest::GetRequest() const
{
	return m_request;
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
	return m_request.url();
}


void CHttpClientRequest::SetReply(QNetworkReply* reply)
{
	istd::CChangeNotifier notifier(this);

	m_replyPtr = reply;
	m_replyBody.clear();
	m_replyAttributes.clear();
	m_replyHeaderNames.clear();
	m_replyHeaders.clear();
	m_errorType = QNetworkReply::NoError;
	m_errorString.clear();

	if (reply == nullptr){
		return;
	}

	if (!reply->isFinished()){
		m_state = RS_PROCESSING;

		return;
	}

	static const QNetworkRequest::Attribute attributes[] = {
		QNetworkRequest::HttpStatusCodeAttribute,
		QNetworkRequest::HttpReasonPhraseAttribute,
		QNetworkRequest::RedirectionTargetAttribute
	};

	for (QNetworkRequest::Attribute attribute: attributes){
		QVariant value = reply->attribute(attribute);
		if (value.isValid()){
			m_replyAttributes.insert(attribute, value);
		}
	}

	for (const QByteArray& headerName: reply->rawHeaderList()){
		m_replyHeaderNames.append(headerName);
		m_replyHeaders.insert(headerName.toLower(), reply->rawHeader(headerName));
	}

	m_replyBody = reply->readAll();
	m_errorType = reply->error();
	m_errorString = (m_errorType == QNetworkReply::NoError) ? QString() : reply->errorString();
	m_state = RS_FINISHED | ((m_errorType == QNetworkReply::NoError) ? RS_NO_ERROR : RS_ERROR);
}


void CHttpClientRequest::SetRequest(const QNetworkRequest& request)
{
	istd::CChangeNotifier notifier(this);

	m_request = request;

	UpdateReadyState();
}


void CHttpClientRequest::SetRequestBody(const QByteArray& body)
{
	istd::CChangeNotifier notifier(this);

	m_requestBody = body;
}


void CHttpClientRequest::SetRequestType(QNetworkAccessManager::Operation requestType)
{
	istd::CChangeNotifier notifier(this);

	m_requestType = requestType;

	UpdateReadyState();
}


// reimplemented (istd::IChangeable)

int CHttpClientRequest::GetSupportedOperations() const
{
	return SO_COPY | SO_RESET;
}


bool CHttpClientRequest::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CHttpClientRequest* sourcePtr = dynamic_cast<const CHttpClientRequest*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_request = sourcePtr->m_request;
	m_requestBody = sourcePtr->m_requestBody;
	m_requestType = sourcePtr->m_requestType;
	m_state = sourcePtr->m_state;
	m_replyPtr = sourcePtr->m_replyPtr;
	m_replyBody = sourcePtr->m_replyBody;
	m_replyAttributes = sourcePtr->m_replyAttributes;
	m_replyHeaderNames = sourcePtr->m_replyHeaderNames;
	m_replyHeaders = sourcePtr->m_replyHeaders;
	m_errorType = sourcePtr->m_errorType;
	m_errorString = sourcePtr->m_errorString;

	return true;
}


bool CHttpClientRequest::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_request = QNetworkRequest();
	m_requestBody.clear();
	m_requestType = QNetworkAccessManager::GetOperation;
	m_state = RS_CREATED;
	m_replyPtr = nullptr;
	m_replyBody.clear();
	m_replyAttributes.clear();
	m_replyHeaderNames.clear();
	m_replyHeaders.clear();
	m_errorType = QNetworkReply::NoError;
	m_errorString.clear();

	return true;
}


// private methods

void CHttpClientRequest::UpdateReadyState()
{
	if ((m_state == RS_CREATED) && m_request.url().isValid()){
		m_state = RS_READY_TO_PROCESSING;
	}
}


} // namespace imtrest


