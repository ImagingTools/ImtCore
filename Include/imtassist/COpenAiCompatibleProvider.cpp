// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/COpenAiCompatibleProvider.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>


namespace imtassist
{


namespace
{


QJsonObject CreateFunctionTool(const QString& name, const QString& description, const QJsonObject& parameters)
{
	QJsonObject function;
	function["name"] = name;
	function["description"] = description;
	function["parameters"] = parameters;

	QJsonObject retVal;
	retVal["type"] = QStringLiteral("function");
	retVal["function"] = function;

	return retVal;
}


} // namespace


// reimplemented (imtassist::CHttpAssistantProviderBase)

bool COpenAiCompatibleProvider::CreateHttpRequest(
			const Settings& settings,
			const Request& request,
			const ToolNames& toolNames,
			const Turns& turns,
			IAssistantHttpTransport::Request& httpRequest) const
{
	QJsonArray messages;

	QJsonObject systemMessage;
	systemMessage["role"] = QStringLiteral("system");
	systemMessage["content"] = CreateSystemPrompt();
	messages.append(systemMessage);

	for (const Turn& turn: turns){
		QJsonObject message;
		switch (turn.kind){
		case Turn::TK_USER_TEXT:
			message["role"] = QStringLiteral("user");
			message["content"] = turn.text;
			break;

		case Turn::TK_ASSISTANT_TEXT:
			message["role"] = QStringLiteral("assistant");
			message["content"] = turn.text;
			break;

		case Turn::TK_TOOL_CALL:
			{
				QJsonObject function;
				function["name"] = turn.toolName;
				function["arguments"] = QString::fromUtf8(QJsonDocument(turn.data).toJson(QJsonDocument::Compact));

				QJsonObject toolCall;
				toolCall["id"] = QString::fromUtf8(turn.callId);
				toolCall["type"] = QStringLiteral("function");
				toolCall["function"] = function;

				message["role"] = QStringLiteral("assistant");
				message["content"] = QJsonValue(QJsonValue::Null);
				message["tool_calls"] = QJsonArray() << toolCall;
			}
			break;

		case Turn::TK_TOOL_RESULT:
			message["role"] = QStringLiteral("tool");
			message["tool_call_id"] = QString::fromUtf8(turn.callId);
			message["content"] = QString::fromUtf8(QJsonDocument(turn.data).toJson(QJsonDocument::Compact));
			break;
		}

		messages.append(message);
	}

	QJsonArray tools;
	for (const ToolDescription& tool: request.tools){
		tools.append(CreateFunctionTool(toolNames.actionToTool.value(tool.actionId), tool.description, tool.parameterSchema));
	}

	tools.append(CreateFunctionTool(s_clarificationToolName, GetClarificationToolDescription(), CreateClarificationToolSchema()));

	QJsonObject body;
	if (settings.includeModelInBody){
		body["model"] = QString::fromUtf8(settings.model);
	}

	body["messages"] = messages;
	body["tools"] = tools;
	body["tool_choice"] = QStringLiteral("auto");

	if (settings.sendParallelToolCallsFlag){
		body["parallel_tool_calls"] = false;
	}

	if ((settings.maxOutputTokens > 0) && !settings.maxTokensField.isEmpty()){
		body[QString::fromUtf8(settings.maxTokensField)] = settings.maxOutputTokens;
	}

	httpRequest.body = QJsonDocument(body).toJson(QJsonDocument::Compact);
	AddAuthHeader(settings, httpRequest);

	return true;
}


IAssistantProvider::Reply COpenAiCompatibleProvider::ParseResponseBody(const QByteArray& body, const ToolNames& toolNames) const
{
	QJsonParseError parseError;
	QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
	if ((parseError.error != QJsonParseError::NoError) || !document.isObject()){
		return CreateInvalidReply();
	}

	QJsonArray choices = document.object().value("choices").toArray();
	if (choices.isEmpty() || !choices.at(0).isObject()){
		return CreateInvalidReply();
	}

	QJsonObject message = choices.at(0).toObject().value("message").toObject();

	QJsonArray toolCalls = message.value("tool_calls").toArray();
	if (!toolCalls.isEmpty()){
		QJsonObject toolCall = toolCalls.at(0).toObject();
		QJsonObject function = toolCall.value("function").toObject();
		QString name = function.value("name").toString();

		// Most servers send the arguments as JSON text, some as an object.
		QJsonObject arguments;
		QJsonValue argumentsValue = function.value("arguments");
		if (argumentsValue.isObject()){
			arguments = argumentsValue.toObject();
		}
		else if (argumentsValue.isString()){
			QString argumentsText = argumentsValue.toString().trimmed();
			if (!argumentsText.isEmpty()){
				QJsonDocument argumentsDocument = QJsonDocument::fromJson(argumentsText.toUtf8(), &parseError);
				if ((parseError.error != QJsonParseError::NoError) || !argumentsDocument.isObject()){
					return CreateInvalidReply();
				}

				arguments = argumentsDocument.object();
			}
		}
		else if (!argumentsValue.isUndefined() && !argumentsValue.isNull()){
			return CreateInvalidReply();
		}

		if (name.isEmpty()){
			return CreateInvalidReply();
		}

		return CreateToolCallReply(name, arguments, toolCall.value("id").toString().toUtf8(), QJsonObject(), toolNames);
	}

	QJsonValue content = message.value("content");
	QString text;
	if (content.isString()){
		text = content.toString();
	}
	else if (content.isArray()){
		for (const QJsonValue& part: content.toArray()){
			QJsonObject partObject = part.toObject();
			if (partObject.value("type").toString() == "text"){
				text += partObject.value("text").toString();
			}
		}
	}

	if (text.trimmed().isEmpty()){
		return CreateInvalidReply();
	}

	return CreateTextReply(text);
}


} // namespace imtassist


