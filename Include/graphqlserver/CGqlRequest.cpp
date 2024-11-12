#include <graphqlserver/CGqlRequest.h>

namespace graphqlserver {


void CGqlRequest::SetCommandId(const QByteArray& commandId)
{
	m_commandId = commandId;
}


void CGqlRequest::SetRequestType(const RequestType& requestType)
{
	m_requestType = requestType;
}


void CGqlRequest::SetQuery(const QByteArray& query)
{
	m_query = query;
}

// reimplemented (graphqlserver::IHttpRequest)
QByteArray CGqlRequest::GetCommandId() const
{
	return m_commandId;
}


CGqlRequest::RequestType CGqlRequest::GetRequestType() const
{
	return m_requestType;
}


QByteArray CGqlRequest::GetQuery() const
{
	return m_query;
}


const IGqlContext* CGqlRequest::GetRequestContext() const
{
	return &m_gqlContext;
}


} // graphqlserver


