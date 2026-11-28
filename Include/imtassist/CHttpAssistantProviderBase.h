// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// STL includes
#include <functional>
#include <memory>

// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>

// ImtCore includes
#include <imtassist/IHttpAssistantProvider.h>


namespace imtassist
{


/**
	Common part of the adapters to HTTP APIs of model providers.

	The base converts the provider-independent request into a list of turns, maps action
	IDs to tool names accepted by every provider, adds the clarification tool, sends the
	request through the transport and maps transport and HTTP errors. Derived classes
	create the request body and parse the response body of one API protocol.

	Credentials are read from environment variables at every call (so they can be rotated),
	or set explicitly, e.g. in tests. They are never part of the configuration files and
	never logged.

	\ingroup imtassist
*/
class CHttpAssistantProviderBase: virtual public IHttpAssistantProvider
{
public:
	/**
		Name of the built-in tool the model calls to ask the user back.
	*/
	static const QString s_clarificationToolName;

	enum
	{
		MAX_TOOL_NAME_LENGTH = 64,
		MAX_ERROR_FIELD_LENGTH = 64
	};

	CHttpAssistantProviderBase();

	/**
		The system prompt sent with every request.
	*/
	QString CreateSystemPrompt() const;

	/**
		Check whether the endpoint may be used: https, or http to the local machine only.
	*/
	static bool IsAllowedEndpoint(const QUrl& endpoint);

	// reimplemented (imtassist::IHttpAssistantProvider)
	virtual void SetSettings(const Settings& settings) override;
	virtual Settings GetSettings() const override;
	virtual void SetTransport(IAssistantHttpTransport* transportPtr) override;
	virtual void SetProblemHandler(const ProblemHandler& handler) override;

	// reimplemented (imtassist::IAssistantProvider)
	virtual bool IsConfigured() const override;
	virtual Reply Complete(const Request& request, const CCancellationToken& cancellationToken) override;

protected:
	/**
		Provider-independent conversation step.
	*/
	struct Turn
	{
		enum Kind
		{
			TK_USER_TEXT,
			TK_ASSISTANT_TEXT,
			TK_TOOL_CALL,
			TK_TOOL_RESULT
		};

		Kind kind = TK_USER_TEXT;
		QString text;
		QByteArray callId;
		QString toolName;
		QJsonObject data;
		QJsonObject providerState;
	};

	typedef QList<Turn> Turns;

	struct ToolNames
	{
		QMap<QByteArray, QString> actionToTool;
		QMap<QString, QByteArray> toolToAction;
	};

	/**
		Map action IDs to tool names of letters, digits, '_' and '-' (max. 64 characters), unique per request.
	*/
	static ToolNames CreateToolNames(const ToolDescriptions& tools);

	/**
		Convert the messages into turns. Every tool call is directly followed by its result;
		context items and results without a call are passed as user text marked as data.
	*/
	static Turns CreateTurns(const Request& request, const ToolNames& toolNames);

	/**
		Merged turns of the same role, for APIs with strictly alternating roles.
	*/
	struct RoleGroup
	{
		bool isAssistant = false;
		Turns turns;
	};

	typedef QList<RoleGroup> RoleGroups;

	/**
		Group the turns by role (user: user text and tool results; assistant: text and tool calls).
		The first group is always a user group.
	*/
	static RoleGroups GroupTurnsByRole(const Turns& turns);

	static QJsonObject CreateClarificationToolSchema();
	static QString GetClarificationToolDescription();

	/**
		Create the reply for a tool call of the model.
	*/
	static Reply CreateToolCallReply(
				const QString& toolName,
				const QJsonObject& arguments,
				const QByteArray& callId,
				const QJsonObject& providerState,
				const ToolNames& toolNames);

	static Reply CreateTextReply(const QString& text);
	static Reply CreateInvalidReply();

	/**
		Resolve a credential from its value or environment variable.
	*/
	static QByteArray ResolveSecret(const QByteArray& value, const QByteArray& variable);

	/**
		Add the API key header, if a key is available.
	*/
	void AddAuthHeader(const Settings& settings, IAssistantHttpTransport::Request& httpRequest) const;

	void ReportProblem(const QString& problem) const;

	// abstract methods

	/**
		Check protocol-specific credentials. The default requires an API key unless credentials are optional.
	*/
	virtual bool HasCredentials(const Settings& settings) const;

	/**
		Create the HTTP request including authentication.
	*/
	virtual bool CreateHttpRequest(
				const Settings& settings,
				const Request& request,
				const ToolNames& toolNames,
				const Turns& turns,
				IAssistantHttpTransport::Request& httpRequest) const = 0;

	/**
		Parse a successful response body.
	*/
	virtual Reply ParseResponseBody(const QByteArray& body, const ToolNames& toolNames) const = 0;

	/**
		Extract a short error type from an error body for the problem log; never the message text.
	*/
	virtual QString ExtractErrorType(const QByteArray& body) const;

private:
	mutable QMutex m_mutex;
	Settings m_settings;
	IAssistantHttpTransport* m_transportPtr = nullptr;
	std::shared_ptr<IAssistantHttpTransport> m_defaultTransportPtr;
	ProblemHandler m_problemHandler;
};


} // namespace imtassist


