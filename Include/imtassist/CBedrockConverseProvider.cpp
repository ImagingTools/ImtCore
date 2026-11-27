// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CBedrockConverseProvider.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtassist/CAwsSigV4Signer.h>


namespace imtassist
{


namespace
{


QJsonObject CreateToolSpec(const QString& name, const QString& description, const QJsonObject& schema)
{
	QJsonObject inputSchema;
	inputSchema["json"] = schema;

	QJsonObject toolSpec;
	toolSpec["name"] = name;
	toolSpec["description"] = description;
	toolSpec["inputSchema"] = inputSchema;

	QJsonObject retVal;
	retVal["toolSpec"] = toolSpec;

	return retVal;
}


} // namespace


// reimplemented (imtassist::CHttpAssistantProviderBase)

bool CBedrockConverseProvider::HasCredentials(const Settings& settings) const
{
	if (!ResolveSecret(settings.apiKey, settings.apiKeyVariable).isEmpty()){
		return true;
	}

	return
				!settings.awsRegion.isEmpty() &&
				!ResolveSecret(settings.awsAccessKeyId, settings.awsAccessKeyIdVariable).isEmpty() &&
				!ResolveSecret(settings.awsSecretAccessKey, settings.awsSecretAccessKeyVariable).isEmpty();
}


bool CBedrockConverseProvider::CreateHttpRequest(
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
				block["text"] = turn.text;
				break;

			case Turn::TK_TOOL_CALL:
				{
					QJsonObject toolUse;
					toolUse["toolUseId"] = QString::fromUtf8(turn.callId);
					toolUse["name"] = turn.toolName;
					toolUse["input"] = turn.data;
					block["toolUse"] = toolUse;
				}
				break;

			case Turn::TK_TOOL_RESULT:
				{
					QJsonObject jsonContent;
					jsonContent["json"] = turn.data;

					QJsonObject toolResult;
					toolResult["toolUseId"] = QString::fromUtf8(turn.callId);
					toolResult["content"] = QJsonArray() << jsonContent;
					block["toolResult"] = toolResult;
				}
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
		tools.append(CreateToolSpec(toolNames.actionToTool.value(tool.actionId), tool.description, tool.parameterSchema));
	}

	tools.append(CreateToolSpec(s_clarificationToolName, GetClarificationToolDescription(), CreateClarificationToolSchema()));

	QJsonObject toolChoice;
	toolChoice["auto"] = QJsonObject();

	QJsonObject toolConfig;
	toolConfig["tools"] = tools;
	toolConfig["toolChoice"] = toolChoice;

	QJsonObject systemBlock;
	systemBlock["text"] = CreateSystemPrompt();

	QJsonObject body;
	body["system"] = QJsonArray() << systemBlock;
	body["messages"] = messages;
	body["toolConfig"] = toolConfig;

	if (settings.maxOutputTokens > 0){
		QJsonObject inferenceConfig;
		inferenceConfig["maxTokens"] = settings.maxOutputTokens;
		body["inferenceConfig"] = inferenceConfig;
	}

	httpRequest.body = QJsonDocument(body).toJson(QJsonDocument::Compact);

	// A Bedrock API key is a bearer token; otherwise the request is signed with the access key.
	QByteArray apiKey = ResolveSecret(settings.apiKey, settings.apiKeyVariable);
	if (!apiKey.isEmpty()){
		AddAuthHeader(settings, httpRequest);

		return true;
	}

	CAwsSigV4Signer::Credentials credentials;
	credentials.accessKeyId = ResolveSecret(settings.awsAccessKeyId, settings.awsAccessKeyIdVariable);
	credentials.secretAccessKey = ResolveSecret(settings.awsSecretAccessKey, settings.awsSecretAccessKeyVariable);
	credentials.sessionToken = ResolveSecret(settings.awsSessionToken, settings.awsSessionTokenVariable);
	if (credentials.accessKeyId.isEmpty() || credentials.secretAccessKey.isEmpty() || settings.awsRegion.isEmpty()){
		return false;
	}

	CAwsSigV4Signer::Sign(
				"POST",
				httpRequest.url,
				httpRequest.headers,
				httpRequest.body,
				settings.awsRegion,
				settings.awsService.isEmpty() ? QByteArray("bedrock") : settings.awsService,
				credentials,
				QDateTime::currentDateTimeUtc());

	return true;
}


IAssistantProvider::Reply CBedrockConverseProvider::ParseResponseBody(const QByteArray& body, const ToolNames& toolNames) const
{
	QJsonParseError parseError;
	QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
	if ((parseError.error != QJsonParseError::NoError) || !document.isObject()){
		return CreateInvalidReply();
	}

	QJsonValue contentValue = document.object().value("output").toObject().value("message").toObject().value("content");
	if (!contentValue.isArray()){
		return CreateInvalidReply();
	}

	QString text;
	for (const QJsonValue& blockValue: contentValue.toArray()){
		QJsonObject block = blockValue.toObject();

		if (block.contains("toolUse")){
			QJsonObject toolUse = block.value("toolUse").toObject();
			QString name = toolUse.value("name").toString();
			QJsonValue input = toolUse.value("input");
			if (name.isEmpty() || !input.isObject()){
				return CreateInvalidReply();
			}

			return CreateToolCallReply(name, input.toObject(), toolUse.value("toolUseId").toString().toUtf8(), QJsonObject(), toolNames);
		}

		if (block.value("text").isString()){
			text += block.value("text").toString();
		}
	}

	if (text.trimmed().isEmpty()){
		return CreateInvalidReply();
	}

	return CreateTextReply(text);
}


} // namespace imtassist


