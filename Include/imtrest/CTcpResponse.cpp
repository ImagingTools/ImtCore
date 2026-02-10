// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CTcpResponse.h>


// ImtCore includes
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

CTcpResponse::CTcpResponse(
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


void CTcpResponse::SetData(const QByteArray &data)
{
	m_data = data;
}

void CTcpResponse::SetHeaders(const Headers &headers)
{
	m_headers = headers;
}


// reimplemented (IResponse)

int CTcpResponse::GetStatusCode() const
{
	return m_statusCode;
}


const QByteArray& CTcpResponse::GetData() const
{
	return m_data;
}


QByteArray CTcpResponse::GetDataTypeId() const
{
	return m_dataTypeId;
}


IResponse::Headers CTcpResponse::GetHeaders() const
{
	return m_headers;
}


// reimplemented (INetworkObject)

const IProtocolEngine& CTcpResponse::GetProtocolEngine() const
{
	return m_engine;
}


// reimplemented (istd::IChangeable)

bool CTcpResponse::ResetData(CompatibilityMode /*mode*/)
{
	m_headers.clear();

	m_statusCode = IProtocolEngine::SC_NONE;

	return true;
}


} // namespace imtrest


