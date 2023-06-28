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


class CWebSocketRequest: public QObject, virtual public IRequest
{
	Q_OBJECT
public:
	class IDestroyObserver
	{
		public:
		virtual void OnRequestDestroyed(CWebSocketRequest* webSocketRequest) = 0;
	};

	enum MethodType
	{
		MT_UNKNOWN = 0,
		MT_CONNECTION_INIT,
		MT_CONNECTION_ASK,
		MT_KEEP_ALIVE,
		MT_START,
		MT_START_ASK,
		MT_ERROR,
		MT_DATA,
		MT_STOP,
		MT_COMPLETE
	};

	CWebSocketRequest(QObject& socket, const IRequestServlet& requestHandler, const IProtocolEngine& engine);
	~CWebSocketRequest();

	QByteArrayList GetHeaders() const;
	QByteArray GetHeaderValue(const QByteArray& headerType) const;
	QUrl GetUrl() const;
	QHostAddress GetRemoteAddress() const;
	void SetBody(const QByteArray& body);
	MethodType GetMethodType() const;
	QByteArray GetSubscriptionId() const;
	void RegisterDestroyObserver(IDestroyObserver* destroyObserver);

	// reimplemented (IRequest)
	virtual RequestState GetState() const override;
	virtual QByteArray GetCommandId() const override;
	virtual CommandParams GetCommandParams() const override;
	QByteArray GetBody() const;

	// reimplemented (INetworkObject)
	virtual const IProtocolEngine& GetProtocolEngine() const override;
	virtual QObject& GetSocketObject() const override;

	// reimplemented (istd::IChangeable)
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

// private Q_SLOTS:
// 	void OnWebSocketTextMessage(const QString& textMessage);
// 	void OnWebSocketBinaryMessage(const QByteArray& dataMessage);

private:
	QHostAddress m_remoteAddress;
	QUrl m_url;
	RequestState m_state;
	QByteArray m_body;
	QByteArray m_subscriptionId;
	MethodType m_type;

	const IRequestServlet& m_requestHandler;
	const IProtocolEngine& m_engine;
	QObject& m_socket;

	typedef QMap<QByteArray, QByteArray> HeaderMap;
	HeaderMap m_headers;

	QByteArray m_lastHeader;
	QList<IDestroyObserver*> m_destroyObserverList;
};


} // namespace imtrest


