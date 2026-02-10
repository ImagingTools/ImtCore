// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QUdpSocket>

// ImtCore includes
#include <imtrest/ISender.h>
#include <imtrest/CUdpRequest.h>


namespace imtrest
{


class CUdpSender: public QObject, virtual public ISender
{
	Q_OBJECT
public:
	CUdpSender(CUdpRequest* request);

	// reimplemented (ISender)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& reguest) const override;

Q_SIGNALS:
    void sended(QByteArray requestId) const;
private:
	QUdpSocket* m_socket;
	QHostAddress m_address;
	unsigned int m_port = 0;
    QByteArray m_requestId;
};


} // namespace imtrest


