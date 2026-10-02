// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CTcpSender.h>


// Qt includes
#include <QtCore/QDataStream>


namespace imtrest
{


// public methods

CTcpSender::CTcpSender(QAbstractSocket* tcpSocketPtr)
{
	m_tcpSocketPtr = tcpSocketPtr;
}


// reimplemented (ITransport)

bool CTcpSender::SendData(QByteArray& data) const
{
	if (m_tcpSocketPtr == nullptr || !m_tcpSocketPtr->isOpen()){
		return false;
	}

	return WriteBody(data, *m_tcpSocketPtr);
}


// protected methods

bool CTcpSender::WriteBody(const QByteArray& data, QAbstractSocket& socket) const
{
	socket.write(data);

	return true;
}


} // namespace imtrest


