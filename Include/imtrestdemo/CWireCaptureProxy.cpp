// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CWireCaptureProxy.h>


// Qt includes
#include <QtNetwork/QHostAddress>


namespace imtrestdemo
{


namespace
{


// The unsent data of a side: the other side is not read while there are more of them
const qint64 s_unsentLimit = 1024 * 1024;

// The data that a socket reads ahead of the proxy
const qint64 s_readBufferSize = 256 * 1024;

const qint64 s_partSize = 64 * 1024;


}


CWireCaptureProxy::CWireCaptureProxy(QObject* parentPtr)
	:QObject(parentPtr),
	m_server(this)
{
	connect(&m_server, &QTcpServer::newConnection, this, &CWireCaptureProxy::OnNewConnection);
}


CWireCaptureProxy::~CWireCaptureProxy()
{
	Stop();
}


bool CWireCaptureProxy::Start(const QString& targetHost, int targetPort)
{
	Stop();

	m_errorString.clear();
	m_targetHost = targetHost;
	m_targetPort = targetPort;

	if (!m_server.listen(QHostAddress::LocalHost, 0)){
		m_errorString = m_server.errorString();

		return false;
	}

	return true;
}


void CWireCaptureProxy::Stop()
{
	m_server.close();

	const QList<Link> links = m_links.values();
	m_links.clear();

	for (const Link& link : links){
		if (link.clientPtr != nullptr){
			link.clientPtr->abort();
			link.clientPtr->deleteLater();
		}

		if (link.serverPtr != nullptr){
			link.serverPtr->abort();
			link.serverPtr->deleteLater();
		}
	}
}


bool CWireCaptureProxy::IsRunning() const
{
	return m_server.isListening();
}


int CWireCaptureProxy::GetPort() const
{
	return m_server.isListening() ? int(m_server.serverPort()) : 0;
}


QString CWireCaptureProxy::GetErrorString() const
{
	return m_errorString;
}


// private slots

void CWireCaptureProxy::OnNewConnection()
{
	while (m_server.hasPendingConnections()){
		QTcpSocket* clientPtr = m_server.nextPendingConnection();
		QTcpSocket* serverPtr = new QTcpSocket(this);

		const int connectionId = ++m_lastConnectionId;

		Link link;
		link.clientPtr = clientPtr;
		link.serverPtr = serverPtr;
		m_links.insert(connectionId, link);

		clientPtr->setParent(this);
		clientPtr->setReadBufferSize(s_readBufferSize);
		serverPtr->setReadBufferSize(s_readBufferSize);

		// The client is not read until the server is connected: its bytes wait in the socket
		connect(clientPtr, &QTcpSocket::readyRead, this, [this, connectionId](){
			Pump(connectionId, D_CLIENT_TO_SERVER);
		});
		connect(serverPtr, &QTcpSocket::readyRead, this, [this, connectionId](){
			Pump(connectionId, D_SERVER_TO_CLIENT);
		});
		connect(serverPtr, &QTcpSocket::bytesWritten, this, [this, connectionId](){
			Pump(connectionId, D_CLIENT_TO_SERVER);
		});
		connect(clientPtr, &QTcpSocket::bytesWritten, this, [this, connectionId](){
			Pump(connectionId, D_SERVER_TO_CLIENT);
		});
		connect(clientPtr, &QTcpSocket::disconnected, this, [this, connectionId](){
			OnSideClosed(connectionId, D_CLIENT_TO_SERVER);
		});
		connect(serverPtr, &QTcpSocket::disconnected, this, [this, connectionId](){
			OnSideClosed(connectionId, D_SERVER_TO_CLIENT);
		});
		connect(serverPtr, &QTcpSocket::connected, this, [this, connectionId](){
			Pump(connectionId, D_CLIENT_TO_SERVER);
		});
		connect(serverPtr, &QTcpSocket::errorOccurred, this, [this, connectionId, serverPtr](){
			// The server is not reachable: the client sees that the connection is closed
			if (serverPtr->state() == QAbstractSocket::UnconnectedState){
				OnSideClosed(connectionId, D_SERVER_TO_CLIENT);
			}
		});

		serverPtr->connectToHost(m_targetHost, quint16(m_targetPort));

		Q_EMIT ConnectionOpened(connectionId);
	}
}


// private methods

void CWireCaptureProxy::Pump(int connectionId, Direction direction)
{
	QHash<int, Link>::ConstIterator iter = m_links.constFind(connectionId);
	if (iter == m_links.constEnd()){
		return;
	}

	QTcpSocket* fromPtr = direction == D_CLIENT_TO_SERVER ? iter->clientPtr.data() : iter->serverPtr.data();
	QTcpSocket* toPtr = direction == D_CLIENT_TO_SERVER ? iter->serverPtr.data() : iter->clientPtr.data();
	if (fromPtr == nullptr || toPtr == nullptr || toPtr->state() != QAbstractSocket::ConnectedState){
		return;
	}

	while (fromPtr->bytesAvailable() > 0 && toPtr->bytesToWrite() < s_unsentLimit){
		const QByteArray data = fromPtr->read(s_partSize);
		if (data.isEmpty()){
			break;
		}

		Q_EMIT DataCaptured(connectionId, int(direction), data);

		toPtr->write(data);
	}
}


void CWireCaptureProxy::OnSideClosed(int connectionId, Direction direction)
{
	QHash<int, Link>::Iterator iter = m_links.find(connectionId);
	if (iter == m_links.end() || iter->isClosed){
		return;
	}

	iter->isClosed = true;

	// What the closed side has sent before is passed, then the other side is closed: the same as without the proxy
	QPointer<QTcpSocket> clientPtr = iter->clientPtr;
	QPointer<QTcpSocket> serverPtr = iter->serverPtr;

	if (direction == D_CLIENT_TO_SERVER){
		Pump(connectionId, D_CLIENT_TO_SERVER);
		if (serverPtr != nullptr){
			serverPtr->disconnectFromHost();
		}
	}
	else{
		Pump(connectionId, D_SERVER_TO_CLIENT);
		if (clientPtr != nullptr){
			clientPtr->disconnectFromHost();
		}
	}

	QPointer<QTcpSocket> closedPtr = direction == D_CLIENT_TO_SERVER ? clientPtr : serverPtr;
	QPointer<QTcpSocket> otherPtr = direction == D_CLIENT_TO_SERVER ? serverPtr : clientPtr;

	m_links.erase(iter);

	Q_EMIT ConnectionClosed(connectionId, int(direction));

	if (closedPtr != nullptr){
		closedPtr->deleteLater();
	}

	// The other socket is deleted after it has sent what is left: deleting earlier would cut the end of the data
	if (otherPtr != nullptr){
		if (otherPtr->state() == QAbstractSocket::UnconnectedState){
			otherPtr->deleteLater();
		}
		else{
			connect(otherPtr, &QTcpSocket::disconnected, otherPtr, &QObject::deleteLater);
		}
	}
}


} // namespace imtrestdemo
