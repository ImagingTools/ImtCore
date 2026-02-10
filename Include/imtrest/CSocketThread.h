// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QMutex>

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imtrest/ISender.h>
#include <imtrest/CSocket.h>


namespace imtrest
{

class CMultiThreadServer;

class CSocketThread:
			public QThread,
			virtual public IRequestServlet,
			virtual public ISender
{
	Q_OBJECT
public:
	enum Status
	{
		ST_START,
		ST_PROCESS,
		ST_CLOSE
	};

	explicit CSocketThread(qintptr ID, bool secureConnection, const QSslConfiguration& sslConfiguration, CMultiThreadServer *parent);
	void SetSocketDescriptor(qintptr socketDescriptor);
	qintptr GetSocketDescriptor();
	void SetSocketStatus(Status socketStatus);
	Status GetSocketStatus();
	QByteArray GetRequestId();
	imtrest::IRequestServlet* GetRequestServlet();

	[[nodiscard]] bool IsSecureConnection() const;
	void EnableSecureConnection(bool isSecureConnection = true);

	virtual imtrest::IRequestUniquePtr CreateRequest() const;

	// reimplemented (QThread)
	virtual void run() override;

	// reimplemented (IRequestHandler)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

	// reimplemented (ISender)
	virtual bool SendResponse(ConstResponsePtr& response) const override;
	virtual bool SendRequest(ConstRequestPtr& reguest) const override;

Q_SIGNALS:
	void Error(QTcpSocket::SocketError socketerror);
	void SocketDisconnected(QByteArray requestId);
	void OnSendResponse(ConstResponsePtr response) const;
	void Abort();

private:
	CMultiThreadServer* m_server;
	qintptr m_socketDescriptor;
	const imtrest::IProtocolEngine* m_enginePtr;
	imtrest::IRequestServlet* m_requestHandlerPtr;
	mutable QMutex m_socketDescriptorMutex;
	mutable QMutex m_statusMutex;
	Status m_status;
	istd::TDelPtr<CSocket> m_socket;
	bool m_isSecureConnection;
	const QSslConfiguration& m_sslConfiguration;

	mutable QByteArray m_requestId;
};


} // namespace imtrest


