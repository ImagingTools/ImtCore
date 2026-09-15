// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CUdpSender.h>


// Qt includes
#include <QtCore/QDataStream>


namespace imtrest
{


// public methods

CUdpSender::CUdpSender(CUdpRequest* request)
{
	m_socket = request->m_socket;
	m_address = request->m_address;
	m_port = request->m_port;
    m_requestId = request->GetRequestId();
}


// reimplemented (ITransport)

bool CUdpSender::SendData(QByteArray& data) const
{
    bool result = m_socket->writeDatagram(data, m_address, m_port);
    Q_EMIT sended(m_requestId);

    return result;
}


} // namespace imtrest


