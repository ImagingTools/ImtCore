#pragma once


// ACF includes
#include <iser/IVersionInfo.h>

// ImtCore includes
#include <imthttp/IRequest.h>
#include <imthttp/IResponse.h>


namespace imtrest
{
	class IRequestServlet;
}


namespace imthttp
{


/**
	Interface for core-level protocol definition.
	Supports multiple communication patterns for different protocols.
*/
class IProtocolEngine: virtual public istd::IPolymorphic
{
public:
	/**
		Communication pattern supported by the protocol.
	*/
	enum ProtocolPattern
	{
		/**
			Request-response pattern (HTTP, TCP with request/response).
		*/
		PP_REQUEST_RESPONSE,
		
		/**
			Publish-subscribe pattern (MQTT, AMQP).
		*/
		PP_PUBLISH_SUBSCRIBE,
		
		/**
			Bidirectional streaming (WebSocket, gRPC streams).
		*/
		PP_STREAMING,
		
		/**
			Datagram/message-based (UDP, ZeroMQ).
		*/
		PP_DATAGRAM
	};

	enum StatusCode
	{
		/**
			No status is available.
		*/
		SC_NONE,

		/**
			Operation could be successfull processed.
		*/
		SC_OK,

		/**
			Requested operation is not available or implemented.
		*/
		SC_OPERATION_NOT_AVAILABLE,

		/**
			Requested resource is not available.
		*/
		SC_RESOURCE_NOT_AVAILABLE,

		/**
			Requested resource was not modified since the last call.
			The caller should use the last version of the resource on the client side.
		*/
		SC_NOT_MODIFIED,

		/**
			Operation is not authorized.
		*/
		SC_UNAUTHORIZED,

		/**
			Internal server error.
		*/
		SC_INTERNAL_ERROR,

		SC_CONTINUE,
		SC_SWITCHING_PROTOCOLS,
		SC_PROCESSING,
		SC_CREATED,
		SC_ACCEPTED,
		SC_NON_AUTHORITATIVE_INFORMATION,
		SC_NO_CONTENT,
		SC_RESET_CONTENT,
		SC_PARTIAL_CONTENT,
		SC_MULTI_STATUS,
		SC_ALREADY_REPORTED,
		SC_IM_USED,
		SC_MULTIPLE_CHOICES,
		SC_MOVED_PERMANENTLY,
		SC_MOVED_TEMPORARILY,
		SC_SEE_OTHER,
		SC_USE_PROXY,
		SC_RESERVE,
		SC_TEMPORARY_REDIRECT,
		SC_PERMANENT_REDIRECT,
		SC_BAD_REQUEST,
		SC_PAYMENT_REQUIRED,
		SC_FORBIDDEN,
		SC_NOT_FOUND,
		SC_METHOD_NOT_ALLOWED,
		SC_NOT_ACCEPTABLE,
		SC_PROXY_AUTHENTICATION_REQUIRED,
		SC_REQUEST_TIMEOUT,
		SC_CONFLICT,
		SC_GONE,
		SC_LENGTH_REQUIRED,
		SC_PRECONDITION_FAILED,
		SC_PAYLOAD_TOO_LARGE,
		SC_URI_TOO_LONG,
		SC_UNSUPPORTED_MEDIA_TYPE,
		SC_RANGE_NOT_SATISFIABLE,
		SC_EXPECTATION_FAILED,
		SC_I_AM_A_TEAPOT,
		SC_MISDIRECTED_REQUEST,
		SC_UNPROCESSABLE_ENTITY,
		SC_LOCKED,
		SC_FAILED_DEPENDENCY,
		SC_UPGRADE_REQUIRED,
		SC_PRECONDITION_REQUIRED,
		SC_TOO_MANY_REQUESTS,
		SC_REQUEST_HEADER_FIELDS_TOO_LARGE,
		SC_RETRY_WITH,
		SC_UNAVAILABLE_FOR_LEGAL_REASONS,
		SC_INTERNAL_SERVER_ERROR,
		SC_NOT_IMPLEMENTED,
		SC_BAD_GATEWAY,
		SC_SERVICE_UNAVAILABLE,
		SC_GATEWAY_TIMEOUT,
		SC_HTTP_VERSION_NOT_SUPPORTED,
		SC_VARIANT_ALSO_NEGOTIATES,
		SC_INSUFFICIENT_STORAGE,
		SC_LOOP_DETECTED,
		SC_BANDWIDTH_LIMIT_EXCEEDED,
		SC_NOT_EXTENDED,
		SC_NETWORK_AUTHENTICATION_REQUIRED,
		SC_UNKNOWN_ERROR,
		SC_WEB_SERVER_IS_DOWN,
		SC_CONNECTION_TIMED_OUT,
		SC_ORIGIN_IS_UNREACHABLE,
		SC_A_TIMEOUT_OCCURRED,
		SC_SSL_HANDSHAKE_FAILED,
		SC_INVALID_SSL_CERTIFICATE
	};

	/**
		Get type-ID of the used protocol.
		\return Protocol identifier (e.g., "http", "mqtt", "websocket", "tcp", "udp")
	*/
	virtual QByteArray GetProtocolTypeId() const = 0;

	/**
		Get the protocol version.
	*/
	virtual const iser::IVersionInfo* GetProtocolVersion() const = 0;

	/**
		Get the communication pattern supported by this protocol.
		\return The protocol pattern (request-response, pub-sub, streaming, or datagram)
	*/
	virtual ProtocolPattern GetProtocolPattern() const = 0;

	/**
		Get the protocol-specific code for the engine's status.
		Maps generic status codes to protocol-specific codes.
		\param statusCode Generic status code
		\param protocolStatusCode Output: protocol-specific status code
		\param statusCodeLiteral Output: human-readable status description
		\return true if mapping was successful, false otherwise
	*/
	virtual bool GetProtocolStatusCode(
				int statusCode,
				int& protocolStatusCode,
				QByteArray& statusCodeLiteral) const = 0;

	/**
		Create request based on the incomming data.
		\param socketPtr		Socket instance for reading the incommming data
		\param requestHandler	Instance to process the request after all data has been read.
	*/
	virtual imthttp::IRequestUniquePtr CreateRequest(const imtrest::IRequestServlet& requestHandler) const = 0;

	/**
		Create request for sending data.
		\param data		The response data.
		\param request	Related request.
	*/
	virtual imthttp::IRequestUniquePtr CreateRequestForSend(
				const imtrest::IRequestServlet& requestHandler,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const = 0;

	/**
		Create reponse for a given request.
		\param data		The response data.
		\param request	Related request.
	*/
	virtual imthttp::IResponseUniquePtr CreateResponse(
				const IRequest& request,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const = 0;
};


} // namespace imthttp


