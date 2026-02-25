// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmail/CSmtpClientComp.h>


namespace imtmail
{


// public methods

CSmtpClientComp::CSmtpClientComp()
	:m_socketPtr(nullptr),
	m_currentMailSendState(MS_CLOSE)
{
}


// protected methods

CSmtpClientComp::ResponseCode CSmtpClientComp::ExtractResponseCode(const QByteArray& response)
{
	int code = response.left(3).toInt();
	switch (code){
	case 220:
		return RC_SERVICE_READY;
	case 250:
		return RC_OK;
	case 334:
		return RC_AUTHENTIFICATION_REQUEST;
	case 235:
		return RC_AUTHENTIFICATION_SUCCESSFUL;
	case 354:
		return RC_START_MAIL_INPUT;
	case 221:
		return RC_SERVICE_CLOSING;
	default:
		return RC_UNKNOWN;
	}
}


void CSmtpClientComp::SendCommand(const QString& command)
{
	if (m_textStreamPtr != nullptr){
		*m_textStreamPtr << command << "\r\n";
		m_textStreamPtr->flush();
	}
}


void CSmtpClientComp::ClearData()
{
	if (m_socketPtr){
		m_socketPtr->disconnect();
	}

	m_socketPtr.reset();
	m_textStreamPtr.reset();
	m_smtpMessagePtr.Reset();

	m_message.clear();
}


// reimplemented (imtmail::ISmtpClient)

QString CSmtpClientComp::GetHost() const
{
	if (m_hostParamCompPtr.IsValid()){
		return m_hostParamCompPtr->GetText();
	}
	
	if (m_hostNameAttrPtr.IsValid()){
		return *m_hostNameAttrPtr;
	}

	return QString();
}


int CSmtpClientComp::GetPort() const
{
	if (m_portParamCompPtr.IsValid()){
		QString portStr = m_portParamCompPtr->GetText();
		
		bool ok = false;
		int port = portStr.toInt(&ok);
		if (ok){
			return port;
		}
	}
	
	if (m_portAttrPtr.IsValid()){
		return *m_portAttrPtr;
	}

	return -1;
}


bool CSmtpClientComp::SendEmail(const ISmtpMessage& smtpMessage) const
{
	if (m_currentMailSendState != MS_CLOSE){
		return false;
	}

	m_message = "To: " + smtpMessage.GetTo() + "\n";
	m_message.append("From: " + smtpMessage.GetFrom() + "\n");
	m_message.append("Subject: " + smtpMessage.GetSubject() + "\n");

	//Let's intitiate multipart MIME with cutting boundary "frontier"
	m_message.append("MIME-Version: 1.0\n");
	m_message.append("Content-Type: multipart/mixed; boundary=frontier\n\n");

	m_message.append( "--frontier\n" );
	m_message.append( "Content-Type: text/plain\n\n" );
	m_message.append(smtpMessage.GetBody());
	m_message.append("\n\n");
	m_message.append( "--frontier--\n" );

	m_message.replace( QString::fromLatin1( "\n" ), QString::fromLatin1( "\r\n" ) );
	m_message.replace( QString::fromLatin1( "\r\n.\r\n" ),QString::fromLatin1( "\r\n..\r\n" ) );

	m_currentMailSendState = MS_INIT;

	m_smtpMessagePtr.MoveCastedPtr(smtpMessage.CloneMe());

	QString host = GetHost();
	int port = GetPort();

	if (m_socketPtr){
		m_socketPtr->disconnect();
	}
	m_socketPtr.reset(new QSslSocket());

	connect(m_socketPtr.get(), &QSslSocket::readyRead, this, &CSmtpClientComp::OnReadyRead, Qt::DirectConnection);
	connect(m_socketPtr.get(), &QSslSocket::connected, this, &CSmtpClientComp::OnConnected, Qt::DirectConnection);
	connect(m_socketPtr.get(), &QSslSocket::encrypted, this, &CSmtpClientComp::OnEcrypted, Qt::DirectConnection);
	connect(m_socketPtr.get(), &QSslSocket::stateChanged, this, &CSmtpClientComp::OnStateChanged, Qt::DirectConnection);
	connect(m_socketPtr.get(), &QSslSocket::disconnected, this,&CSmtpClientComp::OnDisconnected, Qt::DirectConnection);

	m_socketPtr->connectToHostEncrypted(host, port);

	if (!m_socketPtr->waitForConnected(*m_timeoutAttrPtr)){
		qDebug() << QString("Error during the connection establishing: %1").arg(m_socketPtr->errorString());
	}

	return true;
}


void CSmtpClientComp::OnStateChanged(QAbstractSocket::SocketState socketState)
{
	qDebug() << "OnStateChanged" << socketState;
}


void CSmtpClientComp::OnErrorReceived(QAbstractSocket::SocketError socketError)
{
	qDebug() << "OnErrorReceived" << socketError;
}


void CSmtpClientComp::OnSslErrors(const QList<QSslError>& sslErrors)
{
	qDebug() << "OnSslErrors" << sslErrors;

	QString sslErrorsString;
	for(const QSslError& sslError: sslErrors){
		sslErrorsString += sslError.errorString() + "; ";
	}

	m_socketPtr->ignoreSslErrors(sslErrors);
}


void CSmtpClientComp::OnDisconnected()
{
	qDebug() << "OnDisconnected";
}


void CSmtpClientComp::OnConnected()
{
	qDebug() << "OnConnected";
}


void CSmtpClientComp::OnReadyRead()
{
	if (m_socketPtr == nullptr){
		qDebug() << "Unable to handle 'OnReadyRead' socket is invalid";
		return;
	}

	QByteArray response = m_socketPtr->readAll();
	qDebug() << "Server response:" << response;

	ResponseCode code = ExtractResponseCode(response);

	if (m_currentMailSendState == MS_INIT && code == RC_SERVICE_READY)
	{
		m_textStreamPtr.reset(new QTextStream(m_socketPtr.get()));

		SendCommand("EHLO localhost");

		m_currentMailSendState = MS_HANDSHAKE;
	}
	else if (m_currentMailSendState == MS_HANDSHAKE && code == RC_OK)
	{
		m_socketPtr->startClientEncryption();
		if(!m_socketPtr->waitForEncrypted(*m_timeoutAttrPtr)){
			m_currentMailSendState = MS_CLOSE;
		}

		SendCommand("EHLO localhost");

		m_currentMailSendState = MS_AUTH;
	}
	else if (m_currentMailSendState == MS_AUTH && code == RC_OK)
	{
		SendCommand("AUTH LOGIN");
		m_currentMailSendState = MS_USER;
	}
	else if (m_currentMailSendState == MS_USER && code == RC_AUTHENTIFICATION_REQUEST)
	{
		QString user;
		
		if (m_userAttrPtr.IsValid()){
			user = *m_userAttrPtr;
		}
		
		if (m_userParamCompPtr.IsValid()){
			user = m_userParamCompPtr->GetText();
		}
		
		SendCommand(QByteArray().append(user.toUtf8()).toBase64());
		m_currentMailSendState = MS_PASS;
	}
	else if (m_currentMailSendState == MS_PASS && code == RC_AUTHENTIFICATION_REQUEST)
	{
		QString password;
		if (m_passwordAttrPtr.IsValid()){
			password = *m_passwordAttrPtr;
		}
		
		if (m_passwordParamCompPtr.IsValid()){
			password = m_passwordParamCompPtr->GetText();
		}

		SendCommand(QByteArray().append(password.toUtf8()).toBase64());
		m_currentMailSendState = MS_MAIL;
	}
	else if (m_currentMailSendState == MS_MAIL && code == RC_AUTHENTIFICATION_SUCCESSFUL)
	{
		if (m_smtpMessagePtr.IsValid()){
			SendCommand(QString("MAIL FROM:<%1>").arg(m_smtpMessagePtr->GetFrom()));
			m_currentMailSendState = MS_RCPT;
		}
	}
	else if (m_currentMailSendState == MS_RCPT && code == RC_OK)
	{
		if (m_smtpMessagePtr.IsValid()){
			SendCommand(QString("RCPT TO:<%1>").arg(m_smtpMessagePtr->GetTo()));
			m_currentMailSendState = MS_DATA;
		}
	}
	else if (m_currentMailSendState == MS_DATA && code == RC_OK)
	{
		SendCommand("DATA");
		m_currentMailSendState = MS_BODY;
	}
	else if (m_currentMailSendState == MS_BODY && code == RC_START_MAIL_INPUT)
	{
		SendCommand(QString("%1%2").arg(m_message).arg("\r\n."));

		m_currentMailSendState = MS_QUIT;
	}
	else if (m_currentMailSendState == MS_QUIT && code == RC_OK)
	{
		SendCommand("QUIT");
		m_currentMailSendState = MS_CLOSE;
		SendInfoMessage(0, QString("Message  succesfully sended"), "CSmtpClientComp");
		m_message.clear();
	}
	else if (m_currentMailSendState == MS_CLOSE)
	{
		m_message.clear();
	}
	else
	{
		SendErrorMessage(0, QString("Failed to send message"), "CSmtpClientComp");
		m_currentMailSendState = MS_CLOSE;
		m_message.clear();
	}
}


void CSmtpClientComp::OnEcrypted()
{
	qDebug() << "OnEcrypted";
}


} // namespace imtmail
