// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QUrl>
#include <QtNetwork/QHostAddress>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/http_parser.h>


namespace imtrest
{


class CHttpRequest: public QObject, virtual public IRequest
{
	Q_OBJECT
public:
	enum MethodType
	{
		MT_UNKNOWN = 0,
		MT_GET = 1,
		MT_PUT = 2,
		MT_POST = 4,
		MT_DELETE = 8,
		MT_HEAD = 16,
		MT_OPTIONS = 32,
		MT_PATCH = 64
	};

	CHttpRequest(const IRequestServlet& requestHandler, const IProtocolEngine& engine);

	QByteArrayList GetHeaders() const;
	QByteArray GetHeaderValue(const QByteArray& headerType) const;
	QUrl GetUrl() const;
	virtual MethodType GetMethodType() const;
	virtual QByteArray GetBody() const override;
	QHostAddress GetRemoteAddress() const;

	void SetHeader(const QByteArray& headerId, const QByteArray& value);
	void SetMethodType(const MethodType& methodType);
	void SetBody(const QByteArray& body);
	void SetCommandId(const QByteArray& commandId);

	static int OnStartMessage(http_parser* httpParser);
	static int OnUrl(http_parser* httpParser, const char* at, size_t length);
	static int OnStatus(http_parser* httpParser, const char *at, size_t length);
	static int OnHeaderField(http_parser* httpParser, const char *at, size_t length);
	static int OnHeaderValue(http_parser* httpParser, const char *at, size_t length);
	static int OnHeadersComplete(http_parser* httpParser);
	static int OnBody(http_parser* httpParser, const char *at, size_t length);
	static int OnMessageComplete(http_parser* httpParser);
	static int OnChunkHeader(http_parser* httpParser);
	static int OnChunkComplete(http_parser* httpParser);

	// reimplemented (IRequest)
	virtual RequestState GetState() const override;
	virtual QByteArray GetCommandId() const override;
	virtual CommandParams GetCommandParams() const override;
	virtual QByteArray GetRequestId() const override;

	// reimplemented (INetworkObject)
	virtual const IProtocolEngine& GetProtocolEngine() const override;

	// reimplemented (istd::IChangeable)
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

public:
	virtual bool ParseDeviceData(QIODevice& device) override;
	virtual bool ExecuteHttpParser(const QByteArray& data, const QObject* socketObjectPtr);

private:
	static bool ParseUrl(const char *at, size_t length, bool connect, QUrl& url);

protected:
	http_parser m_httpParser;
	QHostAddress m_remoteAddress;
	QUrl m_url;
	RequestState m_state;
	QByteArray m_body;
	QByteArray m_data;

	const IRequestServlet& m_requestHandler;
	const IProtocolEngine& m_engine;
	QObject* m_socket;

	typedef QMap<QByteArray, QByteArray> HeaderMap;
	HeaderMap m_headers;

	QByteArray m_lastHeader;

	QByteArray m_requestId;
};


} // namespace imtrest


