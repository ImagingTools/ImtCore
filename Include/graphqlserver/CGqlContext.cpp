#include <graphqlserver/CGqlContext.h>

namespace graphqlserver {


QByteArray CGqlContext::GetProductId() const
{
	return m_productId;
}


void CGqlContext::SetProductId(const QByteArray& productId)
{
	m_productId = productId;
}


QByteArray CGqlContext::GetLanguageId() const
{
	return m_languageId;
}


void CGqlContext::SetLanguageId(const QByteArray& languageId)
{
	m_languageId = languageId;
}


QByteArray CGqlContext::GetDesignScheme() const
{
	return m_designScheme;
}


void CGqlContext::SetDesignScheme(const QByteArray& designScheme)
{
	m_designScheme = designScheme;
}


QByteArray CGqlContext::GetToken() const
{
	return m_token;
}


void CGqlContext::SetToken(const QByteArray& token)
{
	m_token = token;
}


CGqlContext::Headers CGqlContext::GetHeaders() const
{
	return m_headers;
}


void CGqlContext::SetHeaders(const Headers& headers)
{
	m_headers = headers;
}



} // graphqlserver


