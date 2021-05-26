#include <imtrest/CWebSocketResponse.h>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

CWebSocketResponse::CWebSocketResponse(
			int statusCode,
			const QByteArray& data,
			const QByteArray& dataTypeId,
			QObject& socketObject,
			const IProtocolEngine& engine)
	:m_engine(engine),
	m_socket(socketObject),
	m_statusCode(statusCode),
	m_data(data),
	m_dataTypeId(dataTypeId)
{
}


// reimplemented (IResponse)

int CWebSocketResponse::GetStatusCode() const
{
	return m_statusCode;
}


const QByteArray& CWebSocketResponse::GetData() const
{
	return m_data;
}


QByteArray CWebSocketResponse::GetDataTypeId() const
{
	return m_dataTypeId;
}


IResponse::Headers CWebSocketResponse::GetHeaders() const
{
	return m_headers;
}


// reimplemented (INetworkObject)

const IProtocolEngine& CWebSocketResponse::GetProtocolEngine() const
{
	return m_engine;
}


QObject& CWebSocketResponse::GetSocketObject() const
{
	return m_socket;
}


// reimplemented (istd::IChangeable)

bool CWebSocketResponse::ResetData(CompatibilityMode /*mode*/)
{
	m_headers.clear();

	m_statusCode = IProtocolEngine::SC_NONE;

	return true;
}


} // namespace imtrest


