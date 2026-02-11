// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QSslSocket>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtmail/ISmtpClient.h>


namespace imtmail
{


class CSmtpClientComp: public QObject, public ilog::CLoggerComponentBase, virtual public ISmtpClient
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSmtpClientComp);
		I_REGISTER_INTERFACE(ISmtpClient);
		I_ASSIGN(m_hostNameAttrPtr, "Host", "The property holds connection's host name.", false, "smtp.gmail.com");
		I_ASSIGN(m_hostParamCompPtr, "HostParam", "The property holds connection's host name parameter", false, "");
		I_ASSIGN(m_portAttrPtr, "Port", "The property holds connection's port number", false, 465);
		I_ASSIGN(m_portParamCompPtr, "PortParam", "The property holds connection's port number parameter", false, "");
		I_ASSIGN(m_timeoutAttrPtr, "Timeout", "Timeout (in msecs)", false, 5000);
		I_ASSIGN(m_userAttrPtr, "User", "User", false, "");
		I_ASSIGN(m_userParamCompPtr, "UserParam", "User parameter", false, "");
		I_ASSIGN(m_passwordAttrPtr, "Password", "Password", false, "");
		I_ASSIGN(m_passwordParamCompPtr, "PasswordParam", "Password parameter", false, "");
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
	void ClearData();

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
	I_REF(iprm::ITextParam, m_userParamCompPtr);
	I_ATTR(QByteArray, m_passwordAttrPtr);
	I_REF(iprm::ITextParam, m_passwordParamCompPtr);
	I_ATTR(QString, m_hostNameAttrPtr);
	I_REF(iprm::ITextParam, m_hostParamCompPtr);
	I_ATTR(int, m_portAttrPtr);
	I_REF(iprm::ITextParam, m_portParamCompPtr);
	I_ATTR(int, m_timeoutAttrPtr);

private:
	mutable std::unique_ptr<QSslSocket> m_socketPtr;
	mutable istd::TUniqueInterfacePtr<ISmtpMessage> m_smtpMessagePtr;
	std::unique_ptr<QTextStream> m_textStreamPtr;
	mutable int m_currentMailSendState;
	mutable QString m_message;
};


} // namespace imtmail



