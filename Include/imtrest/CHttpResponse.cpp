#include <imtrest/CHttpResponse.h>


// ImtCore includes
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

CHttpResponse::CHttpResponse(
			int statusCode,
			const QByteArray& data,
			const QByteArray& dataTypeId,
			const IProtocolEngine& engine)
	:m_engine(engine),
	m_statusCode(statusCode),
	m_data(data),
	m_dataTypeId(dataTypeId)
{

}


void CHttpResponse::SetData(const QByteArray &data)
{
	m_data = data;
}

void CHttpResponse::SetHeaders(const Headers &headers)
{
	m_headers = headers;
}


// reimplemented (IResponse)

int CHttpResponse::GetStatusCode() const
{
	return m_statusCode;
}


const QByteArray& CHttpResponse::GetData() const
{
	return m_data;
}


QByteArray CHttpResponse::GetDataTypeId() const
{
	return m_dataTypeId;
}


IResponse::Headers CHttpResponse::GetHeaders() const
{
	return m_headers;
}


// reimplemented (INetworkObject)

const IProtocolEngine& CHttpResponse::GetProtocolEngine() const
{
	return m_engine;
}


// reimplemented (istd::IChangeable)

bool CHttpResponse::ResetData(CompatibilityMode /*mode*/)
{
	m_headers.clear();

	m_statusCode = IProtocolEngine::SC_NONE;

	return true;
}


} // namespace imtrest


