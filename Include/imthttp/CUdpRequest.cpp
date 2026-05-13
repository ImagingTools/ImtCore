// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CUdpRequest.h>


// Qt includes
#include <QtCore/QUrlQuery>
#include <QtCore/QUuid>


namespace imtrest
{


// public methods
CUdpRequest::CUdpRequest(const IRequestServlet& handler,
				const IProtocolEngine& engine,
				QUdpSocket *socket,
				QHostAddress address,
				unsigned int port)
	:CHttpRequest(handler, engine)
{
	m_socket = socket;
	m_address = address;
	m_port = port;
}


CUdpRequest::MethodType CUdpRequest::GetMethodType() const
{
	return MT_POST;
}


// reimplemented (IRequest)

IRequest::RequestState CUdpRequest::GetState() const
{
	return m_state;
}


// protected methods

bool CUdpRequest::ParseDeviceData(QIODevice& device)
{
	QByteArray data = device.readAll();

	m_body = data;

	m_state = RS_MESSAGE_COMPLETE;

	if (!data.isEmpty()){
		m_data += data;
	}

	return true;
}


} // namespace imtrest


