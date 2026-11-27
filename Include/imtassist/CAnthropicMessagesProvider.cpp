// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAnthropicMessagesProvider.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>


namespace imtassist
{


namespace
{


QJsonObject CreateTool(const QString& name, const QString& description, const QJsonObject& inputSchema)
{
	QJsonObject retVal;
	retVal["name"] = name;
	retVal["description"] = description;
	retVal["input_schema"] = inputSchema;

	return retVal;
}


} // namespace


// reimplemented (imtassist::CHttpAssistantProviderBase)

bool CAnthropicMessagesProvider::CreateHttpRequest(
			const Settings& settings,
			const Request& request,
			const ToolNames& toolNames,
			const Turns& turns,
			IAssistantHttpTransport::Request& httpRequest) const
{
	QJsonArray messages;
	for (const RoleGroup& group: GroupTurnsByRole(turns)){
		QJsonArray content;
		for (const Turn& turn: group.turns){
			QJsonObject block;
			switch (turn.kind){
			case Turn::TK_USER_TEXT:
			case Turn::TK_ASSISTANT_TEXT:
				block["type"] = QStringLiteral("text");
				block["text"] = turn.text;
				break;

			case Turn::TK_TOOL_CALL:
				block["type"] = QStringLiteral("tool_use");
				block["id"] = QString::fromUtf8(turn.callId);
				block["name"] = turn.toolName;
				block["input"] = turn.data;
				break;

			case Turn::TK_TOOL_RESULT:
				block["type"] = QStringLiteral("tool_result");
				block["tool_use_id"] = QString::fromUtf8(turn.callId);
				block["content"] = QString::fromUtf8(QJsonDocument(turn.data).toJson(QJsonDocument::Compact));
				break;
			}

			content.append(block);
		}

		QJsonObject message;
		message["role"] = group.isAssistant ? QStringLiteral("assistant") : QStringLiteral("user");
		message["content"] = content;
		messages.append(message);
	}

	QJsonArray tools;
	for (const ToolDescription& tool: request.tools){
		tools.append(CreateTool(toolNames.actionToTool.value(tool.actionId), tool.description, tool.parameterSchema));
	}

	tools.append(CreateTool(s_clarificationToolName, GetClarificationToolDescription(), CreateClarificationToolSchema()));

	QJsonObject toolChoice;
	toolChoice["type"] = QStringLiteral("auto");
	toolChoice["disable_parallel_tool_use"] = true;

	QJsonObject body;
	if (settings.includeModelInBody){
		body["model"] = QString::fromUtf8(settings.model);
	}

	if (!settings.anthropicVersionBodyValue.isEmpty()){
		body["anthropic_version"] = QString::fromUtf8(settings.anthropicVersionBodyValue);
	}

	// The Messages API requires a token limit.
	body["max_tokens"] = (settings.maxOutputTokens > 0) ? settings.maxOutputTokens : 1024;
	body["system"] = CreateSystemPrompt();
	body["messages"] = messages;
	body["tools"] = tools;
	body["tool_choice"] = toolChoice;

	httpRequest.body = QJsonDocument(body).toJson(QJsonDocument::Compact);
	AddAuthHeader(settings, httpRequest);

	return true;
}


IAssistantProvider::Reply CAnthropicMessagesProvider::ParseResponseBody(const QByteArray& body, const ToolNames& toolNames) const
{
	QJsonParseError parseError;
	QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
	if ((parseError.error != QJsonParseError::NoError) || !document.isObject()){
		return CreateInvalidReply();
	}

	QJsonValue contentValue = document.object().value("content");
	if (!contentValue.isArray()){
		return CreateInvalidReply();
	}

	QString text;
	for (const QJsonValue& blockValue: contentValue.toArray()){
		QJsonObject block = blockValue.toObject();
		QString type = block.value("type").toString();

		if (type == "tool_use"){
			QString name = block.value("name").toString();
			QJsonValue input = block.value("input");
			if (name.isEmpty() || !input.isObject()){
				return CreateInvalidReply();
			}

			return CreateToolCallReply(name, input.toObject(), block.value("id").toString().toUtf8(), QJsonObject(), toolNames);
		}

		if (type == "text"){
			text += block.value("text").toString();
		}
	}

	if (text.trimmed().isEmpty()){
		return CreateInvalidReply();
	}

	return CreateTextReply(text);
}


} // namespace imtassist


