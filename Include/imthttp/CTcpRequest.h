#pragma once


// Qt includes
#include <QtCore/QUrl>
#include <QtNetwork/QHostAddress>

// ImtCore includes
#include <imthttp/IRequest.h>

#include <imthttp/http_parser.h>
#include <imthttp/CHttpRequest.h>


namespace imtrest
{
class IRequestServlet;
}


namespace imthttp
{


class CTcpRequest: public CHttpRequest
{
	Q_OBJECT
public:
    CTcpRequest(const imtrest::IRequestServlet& requestHandler, const IProtocolEngine& engine);

	// reimplemented (IRequest)
	virtual RequestState GetState() const override;
    virtual MethodType GetMethodType() const override;
public:
	virtual bool ParseDeviceData(QIODevice& device) override;
};


} // namespace imthttp


