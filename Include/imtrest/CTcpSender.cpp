// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CTcpSender.h>


// Qt includes
#include <QtCore/QDataStream>

// ImtCore includes
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// public methods

CTcpSender::CTcpSender(QAbstractSocket* tcpSocketPtr)
{
	m_tcpSocketPtr = tcpSocketPtr;
}


// reimplemented (IRequest)

bool CTcpSender::SendResponse(ConstResponsePtr& response) const
{
	if (!response.IsValid()){
		return false;
	}

	int protocolStatusCode = -1;
	QByteArray statusLiteral;

	bool retVal = response->GetProtocolEngine().GetProtocolStatusCode(response->GetStatusCode(), protocolStatusCode, statusLiteral);
	if (!retVal){
		return false;
	}

	if (m_tcpSocketPtr != nullptr){
		if (!m_tcpSocketPtr->isOpen()){
			return false;
		}

		const QByteArray& contentData = response->GetData();

		retVal = retVal && WriteBody(contentData, *m_tcpSocketPtr);

		return retVal;
	}

	return false;
}


bool imtrest::CTcpSender::SendRequest(ConstRequestPtr& /*reguest*/) const
{
	return false;
}


// protected methods

bool CTcpSender::WriteBody(const QByteArray& data, QAbstractSocket& socket) const
{
	socket.write(data);

	return true;
}


} // namespace imtrest


