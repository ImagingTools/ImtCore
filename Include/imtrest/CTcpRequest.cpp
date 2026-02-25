// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CTcpRequest.h>


// Qt includes
#include <QtCore/QUrlQuery>
#include <QtCore/QUuid>
#if QT_CONFIG(ssl)
#include <QtNetwork/QSslSocket>
#endif
#include <QtWebSockets/QWebSocket>


namespace imtrest
{


// public methods
CTcpRequest::CTcpRequest(const IRequestServlet& handler, const IProtocolEngine& engine)
    :CHttpRequest(handler, engine)
{

}


CTcpRequest::MethodType CTcpRequest::GetMethodType() const
{
    return MT_POST;
}

// reimplemented (IRequest)

IRequest::RequestState CTcpRequest::GetState() const
{
	return m_state;
}

// protected methods

bool CTcpRequest::ParseDeviceData(QIODevice& device)
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


