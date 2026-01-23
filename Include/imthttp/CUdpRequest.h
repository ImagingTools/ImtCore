#pragma once


// Qt includes
#include <QtCore/QUrl>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>

// ImtCore includes
#include <imthttp/IRequest.h>

#include <imthttp/http_parser.h>
#include <imthttp/CHttpRequest.h>


namespace imthttp
{


class CUdpRequest: public CHttpRequest
{
	Q_OBJECT
public:
	CUdpRequest(const imtrest::IRequestServlet& requestHandler,
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


} // namespace imthttp


