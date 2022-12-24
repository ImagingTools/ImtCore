#pragma once


// Qt includes
#include <QtCore/QUrl>
#include <QtNetwork/QHostAddress>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

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

	CHttpRequest(QObject& socket, const IRequestServlet& requestHandler, const IProtocolEngine& engine);

	QByteArrayList GetHeaders() const;
	QByteArray GetHeaderValue(const QByteArray& headerType) const;
	QUrl GetUrl() const;
	MethodType GetMethodType() const;
	QByteArray GetBody() const;
	QHostAddress GetRemoteAddress() const;

	static int OnStartMessage(http_parser* httpParser);
	static int OnUrl(http_parser* httpParser, const char *at, size_t length);
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

	// reimplemented (INetworkObject)
	virtual const IProtocolEngine& GetProtocolEngine() const override;
	virtual QObject& GetSocketObject() const override;

	// reimplemented (istd::IChangeable)
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	virtual bool ParseDeviceData(QIODevice& device);
	virtual bool ExecuteHttpParser(const QByteArray& data, const QObject* socketObjectPtr);

private Q_SLOTS:
	void HandleReadyRead();
	void OnWebSocketTextMessage(const QString& textMessage);
	void OnWebSocketBinaryMessage(const QByteArray& dataMessage);

private:
	static bool ParseUrl(const char *at, size_t length, bool connect, QUrl& url);

private:
	http_parser m_httpParser;
    QHostAddress m_remoteAddress;
    QByteArray m_body;
    QUrl m_url;
    RequestState m_state;
    QObject& m_socket;
    const IRequestServlet& m_requestHandler;
    const IProtocolEngine& m_engine;

	typedef QMap<QByteArray, QByteArray> HeaderMap;
	HeaderMap m_headers;

	QByteArray m_lastHeader;
};


} // namespace imtrest


