// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/TDelPtr.h>

// ImtCore  includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QSslConfiguration>


namespace imtrest
{


class CMultiThreadServer;
class CSocketThread;


class CSocket: public QObject
{
	Q_OBJECT
public:
	CSocket(CSocketThread* rootSocket, imtrest::IRequest* request, bool secureConnection, const QSslConfiguration& sslConfiguration, qintptr socketDescriptor);
	~CSocket();

public Q_SLOTS:
	void HandleReadyRead();
	void Disconnected();
	void OnSendResponse(ConstResponsePtr response);
	void Abort();
	void TimeOut();
	void OnHandleSslErrors(QList<QSslError> errorList);

private:
	QTimer m_startTimer;
	CSocketThread* m_rootSocket;
	QPointer<QTcpSocket> m_socket;
	istd::TDelPtr<IRequest> m_requestPtr;
};


} // namespace imtrest


