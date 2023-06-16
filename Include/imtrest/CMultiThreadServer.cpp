#include <imtrest/CMultiThreadServer.h>


namespace imtrest {


CThreadSocket::CThreadSocket(qintptr ID, CMultiThreadServer *parent) : QThread(parent), m_socket(nullptr)
{
	this->m_socketDescriptor = ID;

	m_server = parent;
}


QTcpSocket *CThreadSocket::GetSocket() 
{
	return m_socket;
}


void CThreadSocket::run()
{
	if (m_server == nullptr){
		return;
	}
	
	// thread starts here
	qDebug() << " Thread started";

	m_socket = new QTcpSocket();

	// set the ID
	if (!m_socket->setSocketDescriptor(this->m_socketDescriptor)) {
		// something's wrong, we just emit a signal
		emit error(m_socket->error());
		return;
	}
	emit m_server->newThreadConnection(m_socket); 

	// connect socket and signal
	// note - Qt::DirectConnection is used because it's multithreaded
	//        This makes the slot to be invoked immediately, when the signal is emitted.

	// connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
	connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

	// We'll have multiple clients, we want to know which is which
	qDebug() << m_socketDescriptor << " Client connected";



	// make this thread a loop,
	// thread will stay alive so that signal/slot to function properly
	// not dropped out in the middle when thread dies

	exec();
}


void CThreadSocket::disconnected()
{
	qDebug() << m_socketDescriptor << " Disconnected";

//	m_socket->deleteLater();

	exit(0);
}


CMultiThreadServer::CMultiThreadServer(QObject *parent) : QTcpServer(parent)
{
}


// This function is called by QTcpServer when a new connection is available.
void CMultiThreadServer::incomingConnection(qintptr socketDescriptor)
{
	// We have a new connection
	qDebug() << socketDescriptor << " Connecting...";

	// Every new connection will be run in a newly created thread
	CThreadSocket *thread = new CThreadSocket(socketDescriptor, this);

	// connect signal/slot
	// once a thread is not needed, it will be beleted later
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

	thread->start();
}


} // namespace imtrest
