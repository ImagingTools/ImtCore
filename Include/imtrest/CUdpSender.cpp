// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CUdpSender.h>


// Qt includes
#include <QtCore/QDataStream>

// ImtCore includes
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>


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


// reimplemented (IRequest)

bool CUdpSender::SendResponse(ConstResponsePtr& response) const
{
	if (!response.IsValid()){
		return false;
	}

	const QByteArray& contentData = response->GetData();
    bool result =m_socket->writeDatagram(contentData,m_address,m_port);
    Q_EMIT sended(m_requestId);

    return result;
}


bool imtrest::CUdpSender::SendRequest(ConstRequestPtr& /*reguest*/) const
{
	return false;
}


} // namespace imtrest


