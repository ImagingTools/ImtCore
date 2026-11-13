// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QAtomicInt>
#include <QtCore/QThreadPool>
#include <QtNetwork/QSslSocket>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtmail/ISmtpClient.h>


namespace imtmail
{


/**
	SMTP client over an implicit TLS connection.

	SendEmail() puts the message into a queue and returns at once. The messages are sent one after another on a
	thread of the component. A message that could not be sent is tried again, up to the number of attempts of the
	component. The result is logged.
*/
class CSmtpClientComp: public ilog::CLoggerComponentBase, virtual public ISmtpClient
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSmtpClientComp);
		I_REGISTER_INTERFACE(ISmtpClient);
		I_ASSIGN(m_hostNameAttrPtr, "Host", "The property holds connection's host name.", false, "smtp.gmail.com");
		I_ASSIGN(m_hostParamCompPtr, "HostParam", "The property holds connection's host name parameter", false, "");
		I_ASSIGN(m_portAttrPtr, "Port", "The property holds connection's port number", false, 465);
		I_ASSIGN(m_portParamCompPtr, "PortParam", "The property holds connection's port number parameter", false, "");
		I_ASSIGN(m_timeoutAttrPtr, "Timeout", "Timeout of one step of the conversation with the server (in msecs)", false, 5000);
		I_ASSIGN(m_userAttrPtr, "User", "User", false, "");
		I_ASSIGN(m_userParamCompPtr, "UserParam", "User parameter", false, "");
		I_ASSIGN(m_passwordAttrPtr, "Password", "Password", false, "");
		I_ASSIGN(m_passwordParamCompPtr, "PasswordParam", "Password parameter", false, "");
		I_ASSIGN(m_maxAttemptsAttrPtr, "MaxAttempts", "Number of attempts to send a message", false, 3);
	I_END_COMPONENT;

	CSmtpClientComp();

protected:
	QByteArray BuildMimeMessage(const ISmtpMessage& message) const;
	QByteArray ToBase64Lines(const QByteArray& data) const;
	QString ExtractAddress(const QString& address) const;
	void DeliverMessage(const QString& from, const QString& to, const QByteArray& data) const;
	bool RunSession(
				const QString& from,
				const QString& to,
				const QByteArray& data,
				QString& error) const;
	bool Exchange(
				QSslSocket& socket,
				const QByteArray& command,
				char expectedReplyClass,
				QString& error) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentDestroyed() override;

	// reimplemented (imtmail::ISmtpClient)
	virtual QString GetHost() const override;
	virtual int GetPort() const override;
	virtual bool SendEmail(const ISmtpMessage& message) const override;

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
	I_ATTR(int, m_maxAttemptsAttrPtr);

private:
	QAtomicInt m_isStopped;
	mutable QThreadPool m_threadPool;
};


} // namespace imtmail

