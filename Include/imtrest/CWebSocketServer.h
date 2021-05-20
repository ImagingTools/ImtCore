#pragma once


// Qt includes
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

namespace imtrest
{


/**
	WebSocketServer from QWebSocketServer with default constructor
*/

class CWebSocketServer: public QWebSocketServer
{
	Q_OBJECT
public:
	CWebSocketServer(QObject *parent) :
		QWebSocketServer(QString(),QWebSocketServer::NonSecureMode,parent)
	{

	}
};


} // namespace imtrest


