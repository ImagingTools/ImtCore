#pragma once


// Qt includes
#include <QtNetwork/QSslSocket>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtmail/ISmtpClient.h>


namespace imtmail
{


class CSmtpClientComp: public QObject, public icomp::CComponentBase, virtual public ISmtpClient
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSmtpClientComp);
		I_REGISTER_INTERFACE(ISmtpClient);
		I_ASSIGN(m_hostNameAttrPtr, "HostName", "The property holds connection's host name.", true, "smtp.gmail.com");
		I_ASSIGN(m_portAttrPtr, "Port", "The property holds connection's port number", true, 465);
		I_ASSIGN(m_timeoutAttrPtr, "Timeout", "Timeout", true, 100);
		I_ASSIGN(m_userAttrPtr, "User", "User", true, "");
		I_ASSIGN(m_passwordAttrPtr, "Password", "Password", true, "");
	I_END_COMPONENT;

	CSmtpClientComp();

protected:
	enum ResponseCode
	{
		RC_UNKNOWN = -1,
		RC_SERVICE_READY = 220,
		RC_OK = 250,
		RC_AUTHENTIFICATION_REQUEST = 334,
		RC_AUTHENTIFICATION_SUCCESSFUL = 235,
		RC_START_MAIL_INPUT = 354,
		RC_SERVICE_CLOSING = 221
	};

	enum MailStatus
	{
		MS_TLS,
		MS_HANDSHAKE,
		MS_AUTH,
		MS_USER,
		MS_PASS,
		MS_RCPT,
		MS_MAIL,
		MS_DATA,
		MS_INIT,
		MS_BODY,
		MS_QUIT,
		MS_CLOSE
	};

	ResponseCode ExtractResponseCode(const QByteArray& response);
	void SendCommand(const QString& command);

	// reimplemented (imtmail::ISmtpClient)
	virtual QString GetHost() const override;
	virtual int GetPort() const override;
	virtual bool SendEmail(const ISmtpMessage& message) const override;

private slots:
	void OnStateChanged(QAbstractSocket::SocketState socketState);
	void OnErrorReceived(QAbstractSocket::SocketError socketError);
	void OnSslErrors(const QList<QSslError>& sslErrors);
	void OnDisconnected();
	void OnConnected();
	void OnReadyRead();
	void OnEcrypted();

private:
	I_ATTR(QByteArray, m_userAttrPtr);
	I_ATTR(QByteArray, m_passwordAttrPtr);
	I_ATTR(QString, m_hostNameAttrPtr);
	I_ATTR(int, m_portAttrPtr);
	I_ATTR(int, m_timeoutAttrPtr);

private:
	mutable std::unique_ptr<QSslSocket> m_socketPtr;
	std::unique_ptr<QTextStream> m_textStreamPtr;
	mutable int m_currentMailSendState;
	mutable QString m_message;
	mutable QString m_rcpt;
	mutable QString m_senderAddress;
};


} // namespace imtmail



