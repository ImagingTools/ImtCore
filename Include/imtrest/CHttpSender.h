// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QAbstractSocket>

// ImtCore includes
#include <imtrest/ITransport.h>
#include <imtrest/IResponse.h>

namespace imtrest
{


class CHttpSender: public QObject, virtual public ITransport
{
	Q_OBJECT
public:
	CHttpSender(QAbstractSocket* tcpSocketPtr);
	// reimplemented (ITransport)
	virtual bool SendData(QByteArray& data) const override;

	/*!
		Assemble the full HTTP wire bytes (status line, headers, Content-Length/Content-Type, body)
		for the given response. The result can be passed to SendData().
	*/
	static bool BuildResponseData(const IResponse& response, QByteArray& data);

protected:
	QAbstractSocket* m_tcpSocketPtr;
};


} // namespace imtrest


