// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QUrl>
#include <QtNetwork/QHostAddress>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/http_parser.h>
#include <imtrest/CHttpRequest.h>


namespace imtrest
{


class CTcpRequest: public CHttpRequest
{
	Q_OBJECT
public:
    CTcpRequest(const IRequestServlet& requestHandler, const IProtocolEngine& engine);

	// reimplemented (IRequest)
	virtual RequestState GetState() const override;
    virtual MethodType GetMethodType() const override;
public:
	virtual bool ParseDeviceData(QIODevice& device) override;
};


} // namespace imtrest


