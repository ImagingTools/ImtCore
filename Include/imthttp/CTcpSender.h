// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QAbstractSocket>

// ImtCore includes
#include <imtrest/ITransport.h>

namespace imtrest
{


class CTcpSender: public QObject, virtual public ITransport
{
	Q_OBJECT
public:
    CTcpSender(QAbstractSocket* tcpSocketPtr);
	// reimplemented (ITransport)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& request) const override;

protected:
    virtual bool WriteBody(const QByteArray& data, QAbstractSocket& socket) const;

	QAbstractSocket* m_tcpSocketPtr;
};


} // namespace imtrest


