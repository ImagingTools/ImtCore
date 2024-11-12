#pragma once


// Qt includes
#include <QtCore/QSharedPointer>
#include <QtCore/QUrl>
#include <QtCore/QMap>


namespace graphqlserver {

enum ServerStatus
{
	SS_NOT_STARTED,
	SS_LISTEN
};


struct ServerSettings
{
	quint16 tcpPort;
	quint16 webSocketPort;
	QUrl pathToLog;
	QByteArray ProductName;
};


class IHttpRequest
{
public:
	typedef QMap<QByteArray, QByteArray> CommandParams;
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

	virtual QByteArrayList GetHeaders() const = 0;
	virtual QByteArray GetHeaderValue(const QByteArray& headerType) const = 0;
	virtual QUrl GetUrl() const = 0;
	virtual MethodType GetMethodType() const = 0;
	// virtual QHostAddress GetRemoteAddress() const = 0;

	virtual QByteArray GetCommandId() const = 0;
	virtual CommandParams GetCommandParams() const = 0;
	virtual QByteArray GetBody() const = 0;
	virtual QByteArray GetRequestId() const = 0;
};

/**
	Common interface for the server's response.
	Each response consists of following sections:
	* Status code.
	* List of headers given as key-value pair in textual form.
	* Reponse data, if provided.
*/
class IHttpResponse
{
public:
	/**
		Get the status code of the response.
		\sa IProtocolEngine::StatusCode
	*/
	virtual int GetStatusCode() const = 0;

	/**
		Get the data of the response.
	*/
	virtual const QByteArray& GetData() const = 0;

	/**
		Get the type-ID of the response data.
	*/
	virtual QByteArray GetDataTypeId() const = 0;

	/**
		Get headers of the response object.
	*/
	virtual QByteArrayList GetHeaders() const = 0;
};

typedef QSharedPointer<const IHttpResponse>  ConstResponsePtr;


class IHttpRequestHandler
{
public:
	/**
		Returns \c true if the command (also a hierachical command) is supported by the servlet.
		Only supported commands can be processed and are supported in \c ProcessRequest.
	*/
	virtual bool IsCommandSupported(const QByteArray& commandId) const = 0;

	/**
		Process a given request.
	*/
	virtual ConstResponsePtr ProcessRequest(const IHttpRequest& request, const QByteArray& subCommandId = QByteArray()) const = 0;
};


/**
	Common interface for a GraphQL context.
	\todo Move to imtrest. This is a general interface, rename it to IClientRequestContext
*/
class IGqlContext
{
public:
	typedef QMap<QByteArray,QByteArray> Headers;

	virtual QByteArray GetProductId() const = 0;
	virtual void SetProductId(const QByteArray& productId) = 0;

	virtual QByteArray GetLanguageId() const = 0;
	virtual void SetLanguageId(const QByteArray& languageId) = 0;

	virtual QByteArray GetDesignScheme() const = 0;
	virtual void SetDesignScheme(const QByteArray& designScheme) = 0;

	virtual QByteArray GetToken() const = 0;
	virtual void SetToken(const QByteArray& token) = 0;

	virtual Headers GetHeaders() const = 0;
	virtual void SetHeaders(const Headers& headers) = 0;
};

/**
	Common interface for a GraphQL request.
*/
class IGqlRequest
{
public:
	enum RequestType
	{
		/**
			Query request.
		*/
		RT_QUERY,

		/**
			Mutation request.
		*/
		RT_MUTATION,

		/**
			Subscription request.
		*/
		RT_SUBSCRIPTION
	};

	virtual QByteArray GetCommandId() const = 0;
	virtual RequestType GetRequestType() const = 0;
	virtual QByteArray GetQuery() const = 0;
	virtual const IGqlContext* GetRequestContext() const = 0;
};


class IGqlRequestHandler
{
public:
	/**
		Get IDs of the supported commands.
	*/
	virtual bool IsRequestSupported(const IGqlRequest& gqlRequest) const = 0;

	/**
		Create response for a GraphQL-based request.
	*/
	virtual QJsonObject CreateResponse(const IGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};



// /**
// 	Common interface for a subscriber object.
// */
// class ISubscriber
// {
// public:
// 	enum SubscriberState
// 	{
// 		SS_NON_AUTHORIZED,
// 		SS_AUTHORIZED
// 	};

// 	virtual SubscriberState GetState() const = 0;
// 	virtual void SetState(const SubscriberState& subscriberState) = 0;
// 	virtual QByteArray GetTokenId() const = 0;
// 	virtual QByteArray GetSubscriberId() const = 0;
// 	virtual const IRequestServlet& GetRequestHandler() const = 0;
// };

// IPublisher

class IGqlPublisher
{
public:
	/**
		Get IDs of the supported commands.
	*/
	virtual bool IsRequestSupported(const IGqlRequest& gqlRequest) const = 0;

	/**
		Register subscriprion for a GraphQL-based request.
	*/
	virtual bool RegisterSubscription(
				const QByteArray& subscriptionId,
				const IGqlRequest& gqlRequest,
				QString& errorMessage) = 0;
	virtual bool UnRegisterSubscription(const QByteArray& subscriptionId, QString& errorMessage) = 0;
};


bool InitServer(const ServerSettings& serverSettings, QString& errorMessage);
bool StartServer(QString& errorMessage);
bool StopServer(QString& errorMessage);
ServerStatus GetServerStatus();

bool RegisterHttpHandler(const QByteArray& commandId, IHttpRequestHandler& httpRequestHandler, QString& errorMessage);
bool RegisterGqlHandler(const QByteArray& commandId, graphqlserver::IGqlRequestHandler& gqlRequestHandler, QString& errorMessage);
bool RegisterGqlPublisher(const QByteArray& commandId, IGqlPublisher& gqlPublisher, QString& errorMessage);
bool SendSubscription(const QByteArray& subscriptionId, const QJsonObject& subscriptionData);


} //namespace graphqlserver
