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
			QAbstractSocket& socket,
			const IProtocolEngine& engine)
	:m_engine(engine),
	m_socket(socket),
	m_statusCode(statusCode),
	m_data(data),
	m_dataTypeId(dataTypeId)
{
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


QAbstractSocket& CHttpResponse::GetSocket() const
{
	return m_socket;
}


// reimplemented (istd::IChangeable)

bool CHttpResponse::ResetData(CompatibilityMode /*mode*/)
{
	m_headers.clear();

	m_statusCode = IProtocolEngine::SC_NONE;

	return true;
}


} // namespace imtrest


