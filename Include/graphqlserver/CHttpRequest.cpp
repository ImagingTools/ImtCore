#include <graphqlserver/CHttpRequest.h>

namespace graphqlserver {


void CHttpRequest::SetHeaders(const Headers& headers)
{
	m_headers = headers;
}


void CHttpRequest::SetUrl(const QUrl& url)
{
	m_url = url;
}


void CHttpRequest::SetMethodType(const MethodType& methodType)
{
	m_methodType = methodType;
}


void CHttpRequest::SetCommandId(const QByteArray& commandId)
{
	m_commandId = commandId;
}


void CHttpRequest::SetCommandParams(const CommandParams& commandParams)
{
	m_commandParams = commandParams;
}


void CHttpRequest::SetBody(const QByteArray& body)
{
	m_body = body;
}


void CHttpRequest::SetRequestId(const QByteArray& requestId)
{
	m_requestId = requestId;
}

// reimplemented (graphqlserver::IHttpRequest)
QByteArrayList CHttpRequest::GetHeaders() const
{
	return m_headers.keys();
}

QByteArray CHttpRequest::GetHeaderValue(const QByteArray& headerType) const
{
	return m_headers.value(headerType);
}

QUrl CHttpRequest::GetUrl() const
{
	return m_url;
}

CHttpRequest::MethodType CHttpRequest::GetMethodType() const
{
	return m_methodType;
}


QByteArray CHttpRequest::GetCommandId() const
{
	return m_commandId;
}

CHttpRequest::CommandParams CHttpRequest::GetCommandParams() const
{
	return m_commandParams;
}

QByteArray CHttpRequest::GetBody() const
{
	return m_body;
}

QByteArray CHttpRequest::GetRequestId() const
{
	return m_requestId;
}



}


