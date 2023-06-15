#pragma once

// Qt includes
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QThread>

namespace imtrest {

class CMultiThreadServer;

class CThreadSocket : public QThread
{
	Q_OBJECT
  public:
	explicit CThreadSocket(qintptr ID, CMultiThreadServer *parent);
    QTcpSocket* GetSocket();

	void run();

  signals:
	void error(QTcpSocket::SocketError socketerror);

  public slots:
	void disconnected();

  private:
	QTcpSocket* m_socket;
	CMultiThreadServer* m_server;
	qintptr m_socketDescriptor;
};


class CMultiThreadServer : public QTcpServer
{
	Q_OBJECT
public:
	explicit CMultiThreadServer(QObject *parent = 0);

signals:
	void newThreadConnection(QTcpSocket *connection);

protected:
	void incomingConnection(qintptr socketDescriptor);
};


} // namespace imtrest
