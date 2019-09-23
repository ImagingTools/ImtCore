#pragma once


// Qt includes
#include <QtCore/QUrlQuery>
#include <QtNetwork/QHostAddress>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IRequestHandler.h>
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

	CHttpRequest(const QAbstractSocket& socket, const IRequestHandler& requestHandler, const IProtocolEngine& engine);

	QByteArray GetHeaderValue(const QByteArray& headerType) const;
	QUrl GetUrl() const;
	QUrlQuery GetQuery() const;
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
	virtual QByteArray GetRequestId() const override;
	virtual bool ReadFromDevice(QIODevice& device) override;
	virtual const IProtocolEngine& GetProtocolEngine() const override;

	// reimplemented (istd::IChangeable)
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private Q_SLOTS:
	void HandleReadyRead();

private:
	static bool ParseUrl(const char *at, size_t length, bool connect, QUrl& url);

private:
	http_parser m_httpParser;
	QHostAddress m_remoteAddress;
	QUrl m_url;
	RequestState m_state;
	QByteArray m_body;

	const IRequestHandler& m_requestHandler;
	const IProtocolEngine& m_engine;

	typedef QMap<QByteArray, QByteArray> HeaderMap;
	HeaderMap m_headers;

	QByteArray m_lastHeader;
};


} // namespace imtrest


