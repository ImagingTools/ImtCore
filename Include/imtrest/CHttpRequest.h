// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QUrl>
#include <QtNetwork/QHostAddress>

// STL includes
#include <functional>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IHttpRequestBodyPolicy.h>
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

	/**
		Set the policy that decides about the body after the headers are received (see IHttpRequestBodyPolicy). Without a policy
		the body is received with the limit of SetDefaultMaxBodySize().
		\warning	The request does not own the policy and does not check that it is alive: the request keeps a reference to it.
					The policy must live longer than the request, otherwise the call of the destroyed policy is undefined behavior
					(most likely a crash). The requests live in the threads of the connections, so the owner of the policy must
					stop the connections (or call ResetBodyPolicy() of every request) before it destroys the policy.
		\warning	The policy is called from the thread of the connection, it must be thread-safe and must not change its state.
		\note		It has an effect only before the headers are received, the decision of the request that is already received
					is not changed.
		\sa			ResetBodyPolicy()
	*/
	void SetBodyPolicy(const IHttpRequestBodyPolicy& policy);

	/**
		Return to the policy that is used by default: all bodies are received with the limit of SetDefaultMaxBodySize().
		It should be called if the policy that was set by SetBodyPolicy() is going to be destroyed before the request.
	*/
	void ResetBodyPolicy();

	/**
		Set the limit of the body in bytes that is used if the policy does not set its own limit, 0 - no limit.
	*/
	void SetDefaultMaxBodySize(qint64 maxBodySize);

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

	/**
		The connection is closed after the response if the request is \c HTTP/1.0 without \c Connection: \c keep-alive
		or has \c Connection: \c close (RFC 9112, section 9.3). It is known after the headers are received.
	*/
	virtual bool IsConnectionPersistent() const override;

	/**
		The response to \c HEAD has no content (RFC 9110, section 9.3.2).
	*/
	virtual bool IsResponseBodyOmitted() const override;

	/**
		For \c HA_REJECT the message is the final response (the status of the policy, \c 413 for a too big body or \c 417 for
		an unknown expectation) with \c Connection: \c close. For \c HA_SEND_INTERIM the message is \c 100 \c Continue (RFC 9110,
		section 10.1.1): the client expects it if the request has a body, the version is \c HTTP/1.1 (RFC 9110, section 15.2) and
		none of the body is received yet.
	*/
	virtual HeadAction TakeHeadAction(QByteArray& message) override;

	// reimplemented (INetworkObject)
	virtual const IProtocolEngine& GetProtocolEngine() const override;

	// reimplemented (istd::IChangeable)
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

public:
	/**
		Parse the data of the device. Only the bytes of this request are taken from the device: if the data continue with
		the next request, they stay in the device.
	*/
	virtual bool ParseDeviceData(QIODevice& device) override;

	/**
		Parse the data of a request.
		\param	consumedCount	Number of the bytes of the data that belong to this request. It is less than the size of
								the data if the request is complete (the state is RS_MESSAGE_COMPLETE) and the data continue
								with the next message, the parsing stops at the end of the request. The next call of the complete
								request takes nothing, until ResetData().
		\return \c false if the data are not a valid request. The parser does not recover.
	*/
	virtual bool ExecuteHttpParser(const QByteArray& data, const QObject* socketObjectPtr, qsizetype& consumedCount);

	/**
		The same, the number of the bytes that belong to the request is not needed.
	*/
	bool ExecuteHttpParser(const QByteArray& data, const QObject* socketObjectPtr);

private:
	static bool ParseUrl(const char *at, size_t length, bool connect, QUrl& url);

	/**
		Decide how to receive the body, when the headers are complete.
	*/
	void DecideBody();

	/**
		Refuse the request: the parser stops, the body is not received.
	*/
	void Reject(int statusCode, const IResponse::Headers& headers);

	QByteArray CreateRejectMessage() const;

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
	// http_parser reports a header name, a header value or the URL in several parts if they span several reads.
	bool m_isHeaderFieldParsing = false;
	QByteArray m_urlData;

	// Not owned, it must live longer than the request (see SetBodyPolicy()).
	std::reference_wrapper<const IHttpRequestBodyPolicy> m_bodyPolicy;
	qint64 m_defaultMaxBodySize = 0;
	qint64 m_maxBodySize = 0;

	// The request is refused with this status (a value of IProtocolEngine::StatusCode), 0 - it is not refused.
	int m_rejectStatusCode = 0;
	IResponse::Headers m_rejectHeaders;
	bool m_isContinueExpected = false;

	QByteArray m_requestId;
};


} // namespace imtrest


