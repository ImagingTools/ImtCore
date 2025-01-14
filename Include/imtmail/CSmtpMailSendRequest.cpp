#include <imtmail/CSmtpMailSendRequest.h>


// Qt includes
#include <QtCore/QMetaEnum>
#include <QtNetwork>

//Acf includes
#include <istd/CChangeNotifier.h>

namespace imtmail
{


template <typename EnumClass>
static QString ConvertEnumValue(int value)
{
	return QMetaEnum::fromType<EnumClass>().valueToKey(value);
}


CSmtpMailSendRequest::CSmtpMailSendRequest()
{
	m_sslSocket = nullptr;
	m_testStream = nullptr;
	m_state = ISmtpMailSendRequest::RequestState::RS_CREATED;
	m_isVerboseEnabled = false;
	m_loggerPtr = nullptr;
}


void CSmtpMailSendRequest::SendMail() const
{
	m_state = ISmtpMailSendRequest::RequestState::RS_PROCESSING;
	SendVerboseMessage("Begin smtp Transmission");
	QString _F = this->m_user;
	m_message = "To: " + m_destination + "\n";
	m_message.append("From: " + _F + "\n");
	m_message.append("Subject: " + m_subject + "\n");

	//Let's intitiate multipart MIME with cutting boundary "frontier"
	m_message.append("MIME-Version: 1.0\n");
	m_message.append("Content-Type: multipart/mixed; boundary=frontier\n\n");

	m_message.append( "--frontier\n" );
	//message.append( "Content-Type: text/html\n\n" );  //Uncomment this for HTML formating, coment the line below
	m_message.append( "Content-Type: text/plain\n\n" );
	m_message.append(m_body);
	m_message.append("\n\n");

	if(!m_files.isEmpty())
	{
		SendVerboseMessage(QString("Files to be sent: %1").arg(ConvertEnumValue<QAbstractSocket::SocketError>(m_files.size())));
		foreach(QString filePath, m_files)
		{
			QFile file(filePath);
			if(file.exists())
			{
				if (!file.open(QIODevice::ReadOnly))
				{
					SendErrorMessage("Couldn't open the file");
					return ;
				}
				QByteArray bytes = file.readAll();
				m_message.append( "--frontier\n" );
				m_message.append( "Content-Type: application/octet-stream\nContent-Disposition: attachment; filename="+ QFileInfo(file.fileName()).fileName() +";\nContent-Transfer-Encoding: base64\n\n" );
				m_message.append(bytes.toBase64());
				m_message.append("\n");
			}
		}
	}
	if(!m_binaryData.isEmpty())
	{
		SendVerboseMessage(QString("Files to be sent: %1").arg(ConvertEnumValue<QAbstractSocket::SocketError>(m_binaryData.size())));
		foreach(QString key, m_binaryData.keys())
		{


			QByteArray bytes = m_binaryData[key];
			m_message.append( "--frontier\n" );
			m_message.append( "Content-Type: application/octet-stream\nContent-Disposition: attachment; filename="+ key +";\nContent-Transfer-Encoding: base64\n\n" );
			m_message.append(bytes.toBase64());
			m_message.append("\n");

		}
	}
	if(m_binaryData.isEmpty() && m_files.isEmpty()){
		SendVerboseMessage("No attachments found");
	}

	m_message.append( "--frontier--\n" );

	m_message.replace( QString::fromLatin1( "\n" ), QString::fromLatin1( "\r\n" ) );
	m_message.replace( QString::fromLatin1( "\r\n.\r\n" ),QString::fromLatin1( "\r\n..\r\n" ) );

	m_rcpt = m_destination;
	m_currentMailSendState = Init;
	qDebug() << m_host << "PORT" << m_port;
	m_sslSocket = new QSslSocket();
	connect(m_sslSocket, &QSslSocket::readyRead, this, &CSmtpMailSendRequest::OnReadyRead, Qt::DirectConnection);
	connect(m_sslSocket, &QSslSocket::connected, this, &CSmtpMailSendRequest::OnConnected, Qt::DirectConnection);
	connect(m_sslSocket, &QSslSocket::encrypted, this, &CSmtpMailSendRequest::OnEcrypted, Qt::DirectConnection);
	connect(m_sslSocket, &QSslSocket::stateChanged, this, &CSmtpMailSendRequest::OnStateChanged, Qt::DirectConnection);
	connect(m_sslSocket, &QSslSocket::disconnected, this,&CSmtpMailSendRequest::OnDisconnected, Qt::DirectConnection);
	connect(m_sslSocket, &QSslSocket::errorOccurred, this,&CSmtpMailSendRequest::OnErrorReceived, Qt::DirectConnection);
	connect(m_sslSocket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors), this, &CSmtpMailSendRequest::OnSslErrors, Qt::DirectConnection);
	connect(this, &CSmtpMailSendRequest::StatusChanged, [&](const QString& errorString){
		SendVerboseMessage(QString("SMTP status: ") + qPrintable(errorString));
	});
	m_sslSocket->connectToHostEncrypted(m_host, m_port); //"smtp.gmail.com" and 465 for gmail TLS

	if (!m_sslSocket->waitForConnected(m_timeout)) {
		SendErrorMessage(QString("Error during the connection establishing: %1").arg(m_sslSocket->errorString()));
	}
	m_testStream = new QTextStream( m_sslSocket );

}


const QString& CSmtpMailSendRequest::GetUser() const
{
	return m_user;
}


void CSmtpMailSendRequest::SetUser(const QString& user)
{
	this->m_user = user;
	this->EnsureForReady();
}


const QString& CSmtpMailSendRequest::GetPassword() const
{
	return m_password;
}


void CSmtpMailSendRequest::SetPassword(const QString &pass)
{
	m_password = pass;
	this->EnsureForReady();
}


const QString& CSmtpMailSendRequest::GetHost() const
{
	return m_host;
}


void CSmtpMailSendRequest::SetHost(const QString& host)
{
	m_host = host;
	this->EnsureForReady();
}


quint16 CSmtpMailSendRequest::GetPort() const
{
	return m_port;
}


void CSmtpMailSendRequest::SetMailPort(quint16 port)
{
	m_port = port;
	this->EnsureForReady();
}


int CSmtpMailSendRequest::GetTimeout() const
{
	return m_timeout;
}


void CSmtpMailSendRequest::SetTimeout(int timeout)
{
	m_timeout = timeout;
	this->EnsureForReady();
}


const QString& CSmtpMailSendRequest::GetSubject() const
{
	return m_subject;
}


void CSmtpMailSendRequest::SetSubject(const QString& newSubject)
{
	m_subject = newSubject;
	this->EnsureForReady();
}


const QString& CSmtpMailSendRequest::GetBody() const
{
	return m_body;
}


void CSmtpMailSendRequest::SetBody(const QString& newBody)
{
	m_body = newBody;
	this->EnsureForReady();
}


const QStringList& CSmtpMailSendRequest::GetFiles() const
{
	return m_files;
}


void CSmtpMailSendRequest::SetFiles(const QStringList& newFiles)
{
	m_files = newFiles;
	this->EnsureForReady();
}


const QString& CSmtpMailSendRequest::GetDestination() const
{
	return m_destination;
}

void CSmtpMailSendRequest::SetDestination(const QString& newDestination)
{
	m_destination = newDestination;
	this->EnsureForReady();
}


const QString& CSmtpMailSendRequest::GetSenderAddress() const
{
	return m_senderAddress;
}

void CSmtpMailSendRequest::SetSenderAddress(const QString& senderAddress)
{
	m_senderAddress = senderAddress;
}


bool CSmtpMailSendRequest::IsIgnoreAllSslErrorsEnabled() const
{
	return m_isSslIgnoringEnabled;
}


void CSmtpMailSendRequest::SetIgnoreAllSslErrors(bool enabled)
{
	m_isSslIgnoringEnabled = enabled;
}


void CSmtpMailSendRequest::SetBinaryData(const QString& name, const QByteArray& data)
{
	if(m_binaryData.keys().contains(name)){
		I_CRITICAL();

		return;
	}
	m_binaryData[name] = data;
}


void CSmtpMailSendRequest::ClearBinaryData(const QString& name)
{
	if(!name.isEmpty()){
		if(m_binaryData.keys().contains(name)){
			m_binaryData.remove(name);
		}
	}
	else{
		m_binaryData.clear();
	}
}


int CSmtpMailSendRequest::GetState() const
{
	return m_state;
}


// reimplemented (istd::IChangeable)

int CSmtpMailSendRequest::GetSupportedOperations() const
{
	return int(istd::IChangeable::SupportedOperations::SO_COPY);
}


bool CSmtpMailSendRequest::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	if (bool (this->GetSupportedOperations() & istd::IChangeable::SupportedOperations::SO_COPY)){
		const CSmtpMailSendRequest* otherCSmtpMailSendRequestPtr = dynamic_cast<const CSmtpMailSendRequest*>(&object);
		if(otherCSmtpMailSendRequestPtr != nullptr){
			m_user = (otherCSmtpMailSendRequestPtr->m_user);
			m_password = (otherCSmtpMailSendRequestPtr->m_password);
			m_host = (otherCSmtpMailSendRequestPtr->m_host);
			m_port = (otherCSmtpMailSendRequestPtr->m_port);
			m_timeout = (otherCSmtpMailSendRequestPtr->m_timeout);
			m_subject = (otherCSmtpMailSendRequestPtr->m_subject);
			m_body = (otherCSmtpMailSendRequestPtr->m_body);
			m_files = (otherCSmtpMailSendRequestPtr->m_files);
			m_senderAddress = (otherCSmtpMailSendRequestPtr->m_senderAddress);
			m_destination = (otherCSmtpMailSendRequestPtr->m_destination);
			m_loggerPtr = (otherCSmtpMailSendRequestPtr->m_loggerPtr);
			m_isVerboseEnabled = (otherCSmtpMailSendRequestPtr->m_isVerboseEnabled);
			m_binaryData = (otherCSmtpMailSendRequestPtr->m_binaryData);
			this->EnsureForReady();

			return true;
		}
	}

	return false;
}


CSmtpMailSendRequest::~CSmtpMailSendRequest()
{
	if(m_testStream != nullptr){
		delete m_testStream;
	}
	if(m_sslSocket != nullptr){
		m_sslSocket->deleteLater();
	}
	SendVerboseMessage("destructor CSmtpMailSender");
}


void CSmtpMailSendRequest::SetVerboseEnabled(bool isVerboseEnabled)
{
	m_isVerboseEnabled = isVerboseEnabled;
}


void CSmtpMailSendRequest::SetLogLogger(const istd::ILogger& logger)
{
	m_loggerPtr = &logger;
}


void CSmtpMailSendRequest::OnStateChanged(QAbstractSocket::SocketState socketState)
{
	SendVerboseMessage(QString("Socket state: %1").arg(ConvertEnumValue<QAbstractSocket::SocketState>(socketState)));
}


void CSmtpMailSendRequest::OnErrorReceived(QAbstractSocket::SocketError socketError)
{
	SendVerboseMessage("send error socket");
	SendErrorMessage(QString("Socket error occured: %1").arg(ConvertEnumValue<QAbstractSocket::SocketError>(socketError)));
}


void CSmtpMailSendRequest::OnSslErrors(const QList<QSslError>& sslErrors)
{
	QString sslErrorsString;
	for(const QSslError& sslError: sslErrors){
		sslErrorsString += sslError.errorString() + "; ";
	}
	SendErrorMessage(QString("Ssl errors occured: %1").arg(sslErrorsString));
	if (m_isSslIgnoringEnabled){
		m_sslSocket->ignoreSslErrors(sslErrors);
		SendWarningMessage("SSL errors ignored");
	}
}


void CSmtpMailSendRequest::OnDisconnected()
{
	SendVerboseMessage("disconneted");
	SendVerboseMessage(QString("Error: %1").arg(m_sslSocket->errorString()));
}


void CSmtpMailSendRequest::OnConnected()
{
	SendVerboseMessage("Connected ");
}


void CSmtpMailSendRequest::OnReadyRead()
{
	SendVerboseMessage("readyRead");
	// SMTP is line-oriented

	QString responseLine;
	do
	{
		responseLine = m_sslSocket->readLine();
		m_response += responseLine;
	}
	while ( m_sslSocket->canReadLine() && responseLine[3] != ' ' );

	responseLine.truncate( 3 );

	SendVerboseMessage(QString("Server response code: %1").arg(responseLine));
	SendVerboseMessage(QString("Server response: %1").arg(qPrintable(m_response)));

	if ( m_currentMailSendState == Init && responseLine == "220" )
	{
		// banner was okay, let's go on
		*m_testStream << "EHLO localhost" <<"\r\n";
		m_testStream->flush();

		m_currentMailSendState = HandShake;
	}
	//No need, because I'm using socket->startClienEncryption() which makes the SSL handshake for you
	/*else if (state == Tls && responseLine == "250")
	{
		// Trying AUTH
		*t << "STARTTLS" << "\r\n";
		t->flush();
		state = HandShake;
	}*/
	else if (m_currentMailSendState == HandShake && responseLine == "250")
	{
		m_sslSocket->startClientEncryption();
		if(!m_sslSocket->waitForEncrypted(m_timeout))
		{
			SendWarningMessage(QString("Error during secure the connection: %1").arg( m_sslSocket->errorString()));
			m_currentMailSendState = Close;
		}


		//Send EHLO once again but now encrypted

		*m_testStream << "EHLO localhost" << "\r\n";
		m_testStream->flush();
		m_currentMailSendState = Auth;
	}
	else if (m_currentMailSendState == Auth && responseLine == "250")
	{
		// Trying AUTH
		SendVerboseMessage("Auth");
		*m_testStream << "AUTH LOGIN" << "\r\n";
		m_testStream->flush();
		m_currentMailSendState = User;
	}
	else if (m_currentMailSendState == User && responseLine == "334")
	{
		//Trying User
		SendVerboseMessage("Username");
		//GMAIL is using XOAUTH2 protocol, which basically means that password and username has to be sent in base64 coding
		//https://developers.google.com/gmail/xoauth2_protocol
		// *m_testStream << QByteArray()append(m_user).toBase64()  << "\r\n";
		*m_testStream << QByteArray().append(m_user.toUtf8()).toBase64()  << "\r\n";
		m_testStream->flush();

		m_currentMailSendState = Pass;
	}
	else if (m_currentMailSendState == Pass && responseLine == "334")
	{
		//Trying pass
		SendVerboseMessage("Passw");
		*m_testStream << QByteArray().append(m_password.toUtf8()).toBase64() << "\r\n";
		m_testStream->flush();

		m_currentMailSendState = Mail;
	}
	else if ( m_currentMailSendState == Mail && responseLine == "235" )
	{
		// HELO response was okay (well, it has to be)

		//Apperantly for Google it is mandatory to have MAIL FROM and RCPT email formated the following way -> <email@gmail.com>		
		*m_testStream << "MAIL FROM:<" << m_senderAddress << ">\r\n";
		m_testStream->flush();
		m_currentMailSendState = Rcpt;
	}
	else if ( m_currentMailSendState == Rcpt && responseLine == "250" )
	{
		//Apperantly for Google it is mandatory to have MAIL FROM and RCPT email formated the following way -> <email@gmail.com>
		*m_testStream << "RCPT TO:<" << m_rcpt << ">\r\n"; //r
		m_testStream->flush();
		m_currentMailSendState = Data;
	}
	else if ( m_currentMailSendState == Data && responseLine == "250" )
	{

		*m_testStream << "DATA\r\n";
		m_testStream->flush();
		m_currentMailSendState = Body;
	}
	else if ( m_currentMailSendState == Body && responseLine == "354" )
	{

		*m_testStream << m_message << "\r\n.\r\n";
		m_testStream->flush();
		m_currentMailSendState = Quit;
	}
	else if ( m_currentMailSendState == Quit && responseLine == "250" )
	{

		*m_testStream << "QUIT\r\n";
		m_testStream->flush();
		// here, we just close.
		m_currentMailSendState = Close;
		SendInfoMessage("Message sent");
		emit StatusChanged( tr( "Сообщение отправлено" ) );
		m_state = RS_FINISHED | RS_NO_ERROR;
	}
	else if ( m_currentMailSendState == Close )
	{
		return;
	}
	else
	{
		// something broke.
		SendErrorMessage(QString("Failed to send message %1").arg(qPrintable(m_response)));
		m_currentMailSendState = Close;
		emit StatusChanged( tr( "Ошибка при отправке сообщения: ") + m_response );
		m_state = RS_FINISHED | RS_ERROR;
	}
	m_response = "";
}


void CSmtpMailSendRequest::OnEcrypted()
{
	qDebug() << "is encrypted";
}


void CSmtpMailSendRequest::EnsureForReady()
{
	if (m_user.length() &&
				m_password.length() &&
				m_host.length() &&
				m_port > 0 &&
				m_body.length() &&
				m_senderAddress.length() &&
				m_destination.length()){
		IChangeable::ChangeSet changeSet;
		istd::CChangeNotifier changePtr(this, &changeSet);
		m_state = ISmtpMailSendRequest::RequestState::RS_READY_TO_PROCESSING;
		SendVerboseMessage("Smtp ready for transmission");
	}
}


bool CSmtpMailSendRequest::SendInfoMessage(const QString& message) const
{
	if (m_loggerPtr == nullptr){
		return false;
	}

	return m_loggerPtr->SendLogMessage(istd::IInformationProvider::IC_INFO, 0, QString("%1 :").arg(QString::number(reinterpret_cast<qint64>(this))) + message, "CSmtpMailSendRequest");
}


bool CSmtpMailSendRequest::SendWarningMessage(const QString& message) const
{	
	if (m_loggerPtr == nullptr){
		return false;
	}

	return m_loggerPtr->SendLogMessage(istd::IInformationProvider::IC_WARNING, 0, QString("%1 :").arg(QString::number(reinterpret_cast<qint64>(this))) + message, "CSmtpMailSendRequest");
}


bool CSmtpMailSendRequest::SendErrorMessage(const QString& message) const
{
	if (m_loggerPtr == nullptr){
		return false;
	}

	return m_loggerPtr->SendLogMessage(istd::IInformationProvider::IC_ERROR, 0, QString("%1 :").arg(QString::number(reinterpret_cast<qint64>(this))) + message, "CSmtpMailSendRequest");
}


bool CSmtpMailSendRequest::SendCriticalMessage(const QString& message) const
{
	if (m_loggerPtr == nullptr){
		return false;
	}

	return m_loggerPtr->SendLogMessage(istd::IInformationProvider::IC_CRITICAL, 0, QString("%1 :").arg(QString::number(reinterpret_cast<qint64>(this))) + message, "CSmtpMailSendRequest");
}


bool CSmtpMailSendRequest::SendVerboseMessage(const QString& message) const
{
	if (m_loggerPtr == nullptr){
		return false;
	}

	if (m_isVerboseEnabled){
		return m_loggerPtr->SendLogMessage(istd::IInformationProvider::IC_NONE, 0, QString("%1 :").arg(QString::number(reinterpret_cast<qint64>(this))) + message, "CSmtpMailSendRequest");
	}

	return false;
}


} // namespace imtmail
