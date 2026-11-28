// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// STL includes
#include <functional>

// ImtCore includes
#include <imtassist/IAssistantProvider.h>
#include <imtassist/IAssistantHttpTransport.h>
#include <imtrest/HttpHeaderIds.h>


namespace imtassist
{


/**
	Model provider reached over the HTTP API of one protocol, configured by settings.

	\ingroup imtassist
*/
class IHttpAssistantProvider: virtual public IAssistantProvider
{
public:
	struct Settings
	{
		/**
			Complete URL of the API call.
		*/
		QUrl endpoint;

		/**
			Model ID; sent in the body if \c includeModelInBody is set.
		*/
		QByteArray model;
		bool includeModelInBody = true;

		/**
			Header that carries the API key, e.g. "Authorization", "x-api-key", "api-key".
		*/
		QByteArray authHeaderName = imtrest::HttpHeaderIds::s_authorization;

		/**
			Prefix of the header value, e.g. "Bearer ".
		*/
		QByteArray authValuePrefix = "Bearer ";

		/**
			API key, or the name of the environment variable that contains it.
		*/
		QByteArray apiKey;
		QByteArray apiKeyVariable;

		/**
			Allow requests without credentials, e.g. to a local model server.
		*/
		bool allowsNoCredentials = false;

		IAssistantHttpTransport::Headers extraHeaders;

		/**
			Maximum number of generated tokens; 0 to let the provider decide (where allowed).
		*/
		int maxOutputTokens = 1024;

		/**
			Additional instructions of the application, appended to the system prompt.
		*/
		QString additionalInstructions;

		// Protocol specific options

		/**
			OpenAI-compatible: send "parallel_tool_calls": false.
		*/
		bool sendParallelToolCallsFlag = false;

		/**
			OpenAI-compatible: name of the token limit field, "max_tokens" or "max_completion_tokens".
		*/
		QByteArray maxTokensField = "max_tokens";

		/**
			Anthropic on Vertex AI: value of "anthropic_version" in the body.
		*/
		QByteArray anthropicVersionBodyValue;

		/**
			AWS Signature Version 4 (Bedrock) if no API key is used.
		*/
		QByteArray awsRegion;
		QByteArray awsService = "bedrock";
		QByteArray awsAccessKeyId;
		QByteArray awsAccessKeyIdVariable;
		QByteArray awsSecretAccessKey;
		QByteArray awsSecretAccessKeyVariable;
		QByteArray awsSessionToken;
		QByteArray awsSessionTokenVariable;
	};

	/**
		Receiver of technical problems, e.g. HTTP status and error type. Never gets content or credentials.
	*/
	typedef std::function<void(const QString& problem)> ProblemHandler;

	virtual void SetSettings(const Settings& settings) = 0;
	virtual Settings GetSettings() const = 0;

	/**
		Set the transport. Not owned; nullptr restores the built-in transport.
	*/
	virtual void SetTransport(IAssistantHttpTransport* transportPtr) = 0;

	virtual void SetProblemHandler(const ProblemHandler& handler) = 0;
};


} // namespace imtassist


