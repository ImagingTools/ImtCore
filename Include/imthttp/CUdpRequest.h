// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QUrl>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/http_parser.h>
#include <imtrest/CHttpRequest.h>


namespace imtrest
{


class CUdpRequest: public CHttpRequest
{
	Q_OBJECT
public:
	CUdpRequest(const IRequestServlet& requestHandler,
				const IProtocolEngine& engine,
				QUdpSocket *socket,
				QHostAddress address,
				unsigned int port);

	// reimplemented (IRequest)
	virtual RequestState GetState() const override;
	virtual MethodType GetMethodType() const override;
public:
	virtual bool ParseDeviceData(QIODevice& device) override;
	QUdpSocket *m_socket;
	QHostAddress m_address;
	unsigned int m_port;
};


} // namespace imtrest


