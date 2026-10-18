// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/TDelPtr.h>

// ImtCore  includes
#include <imtrest/CConnectionTimeoutBehavior.h>
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>


// Qt includes
#include <QtCore/QPointer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QSslConfiguration>


namespace imtrest
{


class CMultiThreadServer;
class CSocketThread;
class CSocketTimeoutWatcher;


class CSocket: public QObject
{
	Q_OBJECT
public:
	CSocket(CSocketThread* rootSocket, imtrest::IRequest* request, bool secureConnection, const QSslConfiguration& sslConfiguration, qintptr socketDescriptor);
	~CSocket();

public Q_SLOTS:
	void HandleReadyRead();
	void Disconnected();
	void OnSendResponse(QByteArray data);
	void Abort();
	void OnHandleSslErrors(QList<QSslError> errorList);

private:
	/**
		The data of the socket are not a valid request: the client gets the message of the protocol (for HTTP \c 400) and
		the connection is closed.
	*/
	void RejectRequest();

private:
	CSocketThread* m_rootSocket;
	QPointer<QTcpSocket> m_socket;
	istd::TDelPtr<IRequest> m_requestPtr;

	// Time limits of the connection, it is a child of this object.
	CSocketTimeoutWatcher* m_timeoutWatcherPtr = nullptr;

	CConnectionTimeoutBehavior m_behavior;

	// A request is passed to the handler and its response is not written yet. The next request is not parsed meanwhile,
	// its data wait in the socket (only for the protocols with the cycle of a request and a response).
	bool m_isRequestInProgress = false;
};


} // namespace imtrest


