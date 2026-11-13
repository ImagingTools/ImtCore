// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmail/CSmtpClientComp.h>


// Qt includes
#include <QtCore/QDeadlineTimer>
#include <QtCore/QMimeDatabase>
#include <QtCore/QUuid>


namespace imtmail
{


// public methods

CSmtpClientComp::CSmtpClientComp()
{
	m_threadPool.setMaxThreadCount(1);
}


// protected methods

QString CSmtpClientComp::GetUser() const
{
	if (m_userParamCompPtr.IsValid()){
		return m_userParamCompPtr->GetText();
	}

	if (m_userAttrPtr.IsValid()){
		return *m_userAttrPtr;
	}

	return QString();
}


QByteArray CSmtpClientComp::BuildMimeMessage(const ISmtpMessage& message, const QString& sender) const
{
	const QByteArray lineBreak = QByteArrayLiteral("\r\n");
	const QByteArray boundary = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QByteArray retVal;
	retVal.append(QByteArrayLiteral("From: ") + sender.toUtf8() + lineBreak);
	retVal.append(QByteArrayLiteral("To: ") + message.GetTo().toUtf8() + lineBreak);
	retVal.append(QByteArrayLiteral("Subject: ") + message.GetSubject().toUtf8() + lineBreak);
	retVal.append(QByteArrayLiteral("MIME-Version: 1.0") + lineBreak);
	retVal.append(QByteArrayLiteral(R"(Content-Type: multipart/mixed; boundary=")") + boundary + QByteArrayLiteral(R"(")") + lineBreak);
	retVal.append(lineBreak);

	retVal.append(QByteArrayLiteral("--") + boundary + lineBreak);
	retVal.append(QByteArrayLiteral("Content-Type: text/plain; charset=UTF-8") + lineBreak);
	retVal.append(QByteArrayLiteral("Content-Transfer-Encoding: base64") + lineBreak + lineBreak);
	retVal.append(ToBase64Lines(message.GetBody().toUtf8()));

	const QMimeDatabase mimeDatabase;
	const ISmtpMessage::Attachments attachments = message.GetAttachments();
	for (const ISmtpMessage::Attachment& attachment : attachments){
		const QByteArray mimeType = mimeDatabase.mimeTypeForFileNameAndData(attachment.fileName, attachment.data).name().toLatin1();

		// In a quoted parameter a backslash and a quote are escaped with a backslash.
		QString fileName = attachment.fileName;
		fileName.remove('\r').remove('\n');
		fileName.replace('\\', QStringLiteral(R"(\\)")).replace('"', QStringLiteral(R"(\")"));

		const QByteArray quotedName = QByteArrayLiteral(R"(")") + fileName.toUtf8() + QByteArrayLiteral(R"(")");

		retVal.append(QByteArrayLiteral("--") + boundary + lineBreak);
		retVal.append(QByteArrayLiteral("Content-Type: ") + mimeType + QByteArrayLiteral("; name=") + quotedName + lineBreak);
		retVal.append(QByteArrayLiteral("Content-Disposition: attachment; filename=") + quotedName + lineBreak);
		retVal.append(QByteArrayLiteral("Content-Transfer-Encoding: base64") + lineBreak + lineBreak);
		retVal.append(ToBase64Lines(attachment.data));
	}

	retVal.append(QByteArrayLiteral("--") + boundary + QByteArrayLiteral("--") + lineBreak);

	return retVal;
}


QByteArray CSmtpClientComp::ToBase64Lines(const QByteArray& data) const
{
	const QByteArray encoded = data.toBase64();

	QByteArray retVal;
	for (int position = 0; position < encoded.size(); position += 76){
		retVal.append(encoded.mid(position, 76));
		retVal.append(QByteArrayLiteral("\r\n"));
	}

	return retVal;
}


QString CSmtpClientComp::ExtractAddress(const QString& address) const
{
	const int begin = address.indexOf('<');
	const int end = address.indexOf('>', begin);
	if (begin < 0 || end <= begin){
		return address.trimmed();
	}

	return address.mid(begin + 1, end - begin - 1);
}


void CSmtpClientComp::DeliverMessage(const QString& from, const QString& to, const QByteArray& data) const
{
	QString error;
	for (int attempt = 1; attempt <= *m_maxAttemptsAttrPtr; ++attempt){
		if (m_isStopped.loadRelaxed() != 0){
			SendWarningMessage(0, QStringLiteral("The message to '%1' was not sent, the client is stopped").arg(to), "CSmtpClientComp");

			return;
		}

		if (RunSession(from, to, data, error)){
			SendInfoMessage(0, QStringLiteral("The message to '%1' is sent").arg(to), "CSmtpClientComp");

			return;
		}

		SendWarningMessage(0, QStringLiteral("Attempt %1 to send a message to '%2' failed. %3").arg(attempt).arg(to, error), "CSmtpClientComp");
	}

	SendErrorMessage(0, QStringLiteral("Failed to send a message to '%1'").arg(to), "CSmtpClientComp");
}


bool CSmtpClientComp::RunSession(
			const QString& from,
			const QString& to,
			const QByteArray& data,
			QString& error) const
{
	const QByteArray user = GetUser().toUtf8();

	QByteArray password;
	if (m_passwordParamCompPtr.IsValid()){
		password = m_passwordParamCompPtr->GetText().toUtf8();
	}
	else if (m_passwordAttrPtr.IsValid()){
		password = *m_passwordAttrPtr;
	}

	QSslSocket socket;

	// The certificate of the server is not checked, as before.
	socket.setPeerVerifyMode(QSslSocket::VerifyNone);
	socket.connectToHostEncrypted(GetHost(), static_cast<quint16>(GetPort()));
	if (!socket.waitForEncrypted(*m_timeoutAttrPtr)){
		error = socket.errorString();

		return false;
	}

	bool retVal = Exchange(socket, QByteArray(), '2', error);
	retVal = retVal && Exchange(socket, QByteArrayLiteral("EHLO localhost"), '2', error);
	if (!user.isEmpty()){
		retVal = retVal && Exchange(socket, QByteArrayLiteral("AUTH LOGIN"), '3', error);
		retVal = retVal && Exchange(socket, user.toBase64(), '3', error);
		retVal = retVal && Exchange(socket, password.toBase64(), '2', error);
	}

	retVal = retVal && Exchange(socket, QByteArrayLiteral("MAIL FROM:<") + from.toUtf8() + '>', '2', error);
	retVal = retVal && Exchange(socket, QByteArrayLiteral("RCPT TO:<") + to.toUtf8() + '>', '2', error);
	retVal = retVal && Exchange(socket, QByteArrayLiteral("DATA"), '3', error);
	retVal = retVal && Exchange(socket, data + '.', '2', error);
	if (retVal){
		Exchange(socket, QByteArrayLiteral("QUIT"), '2', error);
	}

	return retVal;
}


bool CSmtpClientComp::Exchange(
			QSslSocket& socket,
			const QByteArray& command,
			char expectedReplyClass,
			QString& error) const
{
	if (!command.isEmpty()){
		socket.write(command + QByteArrayLiteral("\r\n"));
	}

	/**
		The lines of a reply have a dash after the code, except the last one. The whole reply has one deadline, so a
		server that sends it slowly cannot hold the worker.
	*/
	const QDeadlineTimer deadline(*m_timeoutAttrPtr);
	QByteArray line;
	do{
		while (!socket.canReadLine()){
			if (deadline.hasExpired() || !socket.waitForReadyRead(static_cast<int>(deadline.remainingTime()))){
				error = QStringLiteral("The server did not answer: %1").arg(socket.errorString());

				return false;
			}
		}

		line = socket.readLine();
	} while (line.size() > 3 && line.at(3) == '-');

	if (!line.startsWith(expectedReplyClass)){
		error = QStringLiteral("Unexpected reply of the server: %1").arg(line.trimmed());

		return false;
	}

	return true;
}


// reimplemented (icomp::CComponentBase)

void CSmtpClientComp::OnComponentDestroyed()
{
	m_isStopped.storeRelaxed(1);
	m_threadPool.clear();
	m_threadPool.waitForDone();

	BaseClass::OnComponentDestroyed();
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


bool CSmtpClientComp::SendEmail(const ISmtpMessage& message) const
{
	// The user of the server is the sender of a message that has none.
	const QString sender = message.GetFrom().isEmpty() ? GetUser() : message.GetFrom();
	const QString from = ExtractAddress(sender);
	const QString to = ExtractAddress(message.GetTo());
	const QString headers = sender + message.GetTo() + message.GetSubject();
	if (from.isEmpty() || to.isEmpty() || headers.contains('\r') || headers.contains('\n')){
		SendErrorMessage(0, QStringLiteral("The message to '%1' has no sender or recipient, or has a line break in a header").arg(message.GetTo()), "CSmtpClientComp");

		return false;
	}

	if (GetHost().isEmpty() || GetPort() <= 0){
		SendErrorMessage(0, QStringLiteral("The server of the message to '%1' is not set").arg(to), "CSmtpClientComp");

		return false;
	}

	const QByteArray data = BuildMimeMessage(message, sender);

	m_threadPool.start([this, from, to, data](){
		DeliverMessage(from, to, data);
	});

	return true;
}


} // namespace imtmail

