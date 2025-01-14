#pragma once


// Qt includes
#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>

// ACF includes
#include <ilog/CLoggerBase.h>

// ImtCore includes
#include <imtmail/ISmtpMailSendRequest.h>


QT_REQUIRE_CONFIG(ssl);


namespace imtmail
{


class CSmtpMailSendRequest: public QObject, public imtmail::ISmtpMailSendRequest
{
	Q_OBJECT

public:
	typedef QObject BaseClass;

	CSmtpMailSendRequest();
	~CSmtpMailSendRequest();

	void SetVerboseEnabled(bool isVerboseEnabled);
	void SetLogLogger(const istd::ILogger& logger);

	// reimplemented (ISmtpMailSendRequest)
	void SendMail() const override;
	int GetState() const override;

	virtual const QString& GetUser() const override;
	virtual void SetUser(const QString& user) override;
	virtual const QString& GetPassword() const override;
	virtual void SetPassword(const QString& pass) override;
	virtual const QString& GetHost() const override;
	virtual void SetHost(const QString& host) override;
	virtual quint16 GetPort() const override;
	virtual void SetMailPort(quint16 port) override;
	virtual int GetTimeout() const override;
	virtual void SetTimeout(int timeout) override;
	virtual const QString& GetSubject() const override;
	virtual void SetSubject(const QString& newSubject) override;
	virtual const QString& GetBody() const override;
	virtual void SetBody(const QString& newBody) override;
	virtual const QStringList& GetFiles() const override;
	virtual void SetFiles(const QStringList& newFiles) override;
	virtual const QString& GetDestination() const override;
	virtual void SetDestination(const QString& newDestination) override;
	virtual const QString& GetSenderAddress() const override;
	virtual void SetSenderAddress(const QString& senderAddress) override;
	virtual bool IsIgnoreAllSslErrorsEnabled() const override;
	virtual void SetIgnoreAllSslErrors(bool enabled) override;
	virtual void SetBinaryData(const QString& name, const QByteArray& data) override;
	virtual void ClearBinaryData(const QString& name = QString())override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	bool CopyFrom(const IChangeable& object, CompatibilityMode mode) override;



signals:
	void StatusChanged(const QString & errorString);

protected:
	bool SendInfoMessage(const QString& message) const;
	bool SendWarningMessage(const QString& message) const;
	bool SendErrorMessage(const QString& message) const;
	bool SendCriticalMessage(const QString& message) const;
	bool SendVerboseMessage(const QString& message) const;

private slots:
	void OnStateChanged(QAbstractSocket::SocketState socketState);
	void OnErrorReceived(QAbstractSocket::SocketError socketError);
	void OnSslErrors(const QList<QSslError>& sslErrors);
	void OnDisconnected();
	void OnConnected();
	void OnReadyRead();
	void OnEcrypted();

private:
	void EnsureForReady();

private:
	enum States
	{
		Tls,
		HandShake,
		Auth,
		User,
		Pass,
		Rcpt,
		Mail,
		Data,
		Init,
		Body,
		Quit,
		Close
	};
	int m_timeout;
	mutable QString m_message;
	mutable QTextStream* m_testStream;
	mutable QSslSocket* m_sslSocket;
	QString m_senderAddress;
	mutable QString m_rcpt;
	QString m_response;
	QString m_user;
	QString m_password;
	QString m_host;
	QString m_subject;
	QString m_body;
	QString m_destination;
	QStringList m_files;
	quint16 m_port;
	bool m_isSslIgnoringEnabled;
	bool m_isVerboseEnabled;
	mutable int m_currentMailSendState;
	mutable int m_state;
	const istd::ILogger* m_loggerPtr;
	QHash<QString, QByteArray> m_binaryData;
};


} // namespace imtmail


