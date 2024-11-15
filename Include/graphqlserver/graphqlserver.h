#pragma once


// Qt includes
#include <QtCore/QSharedPointer>
#include <QtCore/QUrl>
#include <QtCore/QMap>


namespace graphqlserver {

enum ServerStatus
{
	SS_NOT_STARTED,
	SS_LISTENING
};


struct ServerSettings
{
	quint16 tcpPort;
	quint16 webSocketPort;
	QByteArray productId;
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


class ResultKeys
{
public:
	QByteArrayList GetKeys() const { return m_simpleKes + m_childs.keys(); }
	const ResultKeys* GetChild(const QByteArray& key)
	{
		if (m_childs.contains(key)){
			return &m_childs[key];
		}

		return nullptr;
	};
	void AppendSimpleKey(const QByteArray& key) { m_simpleKes.append(key); }
	void InsertChild(const QByteArray& key, const ResultKeys& child) { m_childs.insert(key, child); }
	bool IsChild(const QByteArray& key) const { return m_childs.contains(key); }

private:
	QByteArrayList m_simpleKes;
	QMap<QByteArray, ResultKeys> m_childs;
};

void CreateResultData(const ResultKeys* resultKeys, QByteArray& resultData);


class IGqlRequestHandler
{
public:
	/**
		Create response for a GraphQL-based request.
	*/
	virtual QJsonObject CreateResponse(
				const QByteArray& commandId,
				const RequestType requestType,
				const QJsonObject& inputParams,
				const ResultKeys& resultKeys,
				QString& errorMessage) const = 0;
};


class IGqlPublisher
{
public:

	/**
		Register subscriprion for a GraphQL-based request.
	*/
	virtual bool RegisterSubscription(
				const QByteArray& subscriptionId,
				const ResultKeys& resultKeys,
				QString& errorMessage) = 0;
	virtual bool UnRegisterSubscription(const QByteArray& subscriptionId, QString& errorMessage) = 0;
};


bool StartServer(const ServerSettings &serverSettings, QString& errorMessage);
bool StopServer(QString& errorMessage);
ServerStatus GetServerStatus();

bool RegisterHttpHandler(const QByteArray& commandId, IHttpRequestHandler& httpRequestHandler, QString& errorMessage);
bool RegisterGqlHandler(const QByteArray& commandId, graphqlserver::IGqlRequestHandler& gqlRequestHandler, QString& errorMessage);
bool RegisterGqlPublisher(const QByteArray& commandId, IGqlPublisher& gqlPublisher, QString& errorMessage);
bool PublishData(const QByteArray& subscriptionId, const QJsonObject& subscriptionData);


} //namespace graphqlserver
