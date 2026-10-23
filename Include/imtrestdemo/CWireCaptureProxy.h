// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>


namespace imtrestdemo
{


/**
	Transparent TCP proxy that shows the exact bytes that are exchanged between a client and a server: it listens on a local port,
	connects every incoming connection to the target server, passes the bytes in both directions without any change and reports them.
	It is used by the demo client for the debug preview of the traffic: a client library does not show what it really sends
	(the added headers, the framing of the chunks, \c Expect: 100-continue and the interim responses), the proxy does.

	The proxy does not buffer more than a bounded part of the data: it stops reading from a side while the other side
	does not take the data, so a slow client slows the server down as without the proxy.
	It works in the thread of its creation.
*/
class CWireCaptureProxy: public QObject
{
	Q_OBJECT
public:
	enum Direction
	{
		D_CLIENT_TO_SERVER = 0,
		D_SERVER_TO_CLIENT = 1
	};

	explicit CWireCaptureProxy(QObject* parentPtr = nullptr);
	~CWireCaptureProxy();

	/**
		Start to listen on a free port of the local host. The connections are passed to the target.
	*/
	bool Start(const QString& targetHost, int targetPort);
	void Stop();

	bool IsRunning() const;

	/**
		Get the port to which the client connects instead of the port of the server.
	*/
	int GetPort() const;

	QString GetErrorString() const;

Q_SIGNALS:
	void ConnectionOpened(int connectionId);

	/**
		Bytes that are passed to the other side. \c direction is a value of Direction.
	*/
	void DataCaptured(int connectionId, int direction, const QByteArray& data);

	/**
		The connection is closed by one of the sides, \c closedBy is a value of Direction that tells who closed it (the side that is the
		source of the closing: D_SERVER_TO_CLIENT is the closing by the server).
	*/
	void ConnectionClosed(int connectionId, int closedBy);

private Q_SLOTS:
	void OnNewConnection();

private:
	struct Link
	{
		QPointer<QTcpSocket> clientPtr;
		QPointer<QTcpSocket> serverPtr;
		bool isClosed = false;
	};

	void Pump(int connectionId, Direction direction);
	void OnSideClosed(int connectionId, Direction direction);

private:
	QTcpServer m_server;
	QString m_targetHost;
	int m_targetPort = 0;
	QString m_errorString;
	int m_lastConnectionId = 0;
	QHash<int, Link> m_links;
};


} // namespace imtrestdemo


