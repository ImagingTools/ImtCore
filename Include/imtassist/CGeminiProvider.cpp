// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CGeminiProvider.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>


namespace imtassist
{


namespace
{


/**
	Gemini accepts an OpenAPI subset of JSON Schema without "additionalProperties".
*/
QJsonValue RemoveUnsupportedKeywords(const QJsonValue& value)
{
	if (value.isArray()){
		QJsonArray retVal;
		for (const QJsonValue& item: value.toArray()){
			retVal.append(RemoveUnsupportedKeywords(item));
		}

		return retVal;
	}

	if (!value.isObject()){
		return value;
	}

	QJsonObject object = value.toObject();
	QJsonObject retVal;
	for (QJsonObject::const_iterator iter = object.constBegin(); iter != object.constEnd(); ++iter){
		if (iter.key() == "additionalProperties"){
			continue;
		}

		if (iter.key() == "properties" && iter.value().isObject()){
			QJsonObject properties;
			QJsonObject sourceProperties = iter.value().toObject();
			for (QJsonObject::const_iterator propertyIter = sourceProperties.constBegin(); propertyIter != sourceProperties.constEnd(); ++propertyIter){
				properties[propertyIter.key()] = RemoveUnsupportedKeywords(propertyIter.value());
			}
			retVal[iter.key()] = properties;
			continue;
		}

		retVal[iter.key()] = RemoveUnsupportedKeywords(iter.value());
	}

	return retVal;
}


QJsonObject CreateFunctionDeclaration(const QString& name, const QString& description, const QJsonObject& parameters)
{
	QJsonObject retVal;
	retVal["name"] = name;
	retVal["description"] = description;
	retVal["parameters"] = RemoveUnsupportedKeywords(parameters).toObject();

	return retVal;
}


} // namespace


// reimplemented (imtassist::CHttpAssistantProviderBase)

bool CGeminiProvider::CreateHttpRequest(
			const Settings& settings,
			const Request& request,
			const ToolNames& toolNames,
			const Turns& turns,
			IAssistantHttpTransport::Request& httpRequest) const
{
	QJsonArray contents;
	for (const RoleGroup& group: GroupTurnsByRole(turns)){
		QJsonArray parts;
		for (const Turn& turn: group.turns){
			QJsonObject part;
			switch (turn.kind){
			case Turn::TK_USER_TEXT:
			case Turn::TK_ASSISTANT_TEXT:
				part["text"] = turn.text;
				break;

			case Turn::TK_TOOL_CALL:
				{
					QJsonObject functionCall;
					functionCall["name"] = turn.toolName;
					functionCall["args"] = turn.data;
					if (turn.providerState.value("id").isString()){
						functionCall["id"] = turn.providerState.value("id");
					}

					part["functionCall"] = functionCall;

					// The signature of the model must be sent back with its function call.
					if (turn.providerState.value("thoughtSignature").isString()){
						part["thoughtSignature"] = turn.providerState.value("thoughtSignature");
					}
				}
				break;

			case Turn::TK_TOOL_RESULT:
				{
					QJsonObject response;
					response["result"] = turn.data;

					QJsonObject functionResponse;
					functionResponse["name"] = turn.toolName;
					functionResponse["response"] = response;

					part["functionResponse"] = functionResponse;
				}
				break;
			}

			parts.append(part);
		}

		QJsonObject content;
		content["role"] = group.isAssistant ? QStringLiteral("model") : QStringLiteral("user");
		content["parts"] = parts;
		contents.append(content);
	}

	QJsonArray declarations;
	for (const ToolDescription& tool: request.tools){
		declarations.append(CreateFunctionDeclaration(toolNames.actionToTool.value(tool.actionId), tool.description, tool.parameterSchema));
	}

	declarations.append(CreateFunctionDeclaration(s_clarificationToolName, GetClarificationToolDescription(), CreateClarificationToolSchema()));

	QJsonObject tools;
	tools["functionDeclarations"] = declarations;

	QJsonObject systemPart;
	systemPart["text"] = CreateSystemPrompt();

	QJsonObject systemInstruction;
	systemInstruction["parts"] = QJsonArray() << systemPart;

	QJsonObject functionCallingConfig;
	functionCallingConfig["mode"] = QStringLiteral("AUTO");

	QJsonObject toolConfig;
	toolConfig["functionCallingConfig"] = functionCallingConfig;

	QJsonObject body;
	body["systemInstruction"] = systemInstruction;
	body["contents"] = contents;
	body["tools"] = QJsonArray() << tools;
	body["toolConfig"] = toolConfig;

	if (settings.maxOutputTokens > 0){
		QJsonObject generationConfig;
		generationConfig["maxOutputTokens"] = settings.maxOutputTokens;
		body["generationConfig"] = generationConfig;
	}

	httpRequest.body = QJsonDocument(body).toJson(QJsonDocument::Compact);
	AddAuthHeader(settings, httpRequest);

	return true;
}


IAssistantProvider::Reply CGeminiProvider::ParseResponseBody(const QByteArray& body, const ToolNames& toolNames) const
{
	QJsonParseError parseError;
	QJsonDocument document = QJsonDocument::fromJson(body, &parseError);
	if ((parseError.error != QJsonParseError::NoError) || !document.isObject()){
		return CreateInvalidReply();
	}

	QJsonArray candidates = document.object().value("candidates").toArray();
	if (candidates.isEmpty()){
		return CreateInvalidReply();
	}

	QJsonArray parts = candidates.at(0).toObject().value("content").toObject().value("parts").toArray();

	QString text;
	for (const QJsonValue& partValue: parts){
		QJsonObject part = partValue.toObject();

		if (part.contains("functionCall")){
			QJsonObject functionCall = part.value("functionCall").toObject();
			QString name = functionCall.value("name").toString();
			QJsonValue args = functionCall.value("args");
			if (name.isEmpty() || (!args.isObject() && !args.isUndefined())){
				return CreateInvalidReply();
			}

			QJsonObject providerState;
			if (part.value("thoughtSignature").isString()){
				providerState["thoughtSignature"] = part.value("thoughtSignature");
			}

			QByteArray callId;
			if (functionCall.value("id").isString()){
				providerState["id"] = functionCall.value("id");
				callId = functionCall.value("id").toString().toUtf8();
			}

			return CreateToolCallReply(name, args.toObject(), callId, providerState, toolNames);
		}

		// Thought summaries are not part of the answer.
		if (part.value("text").isString() && !part.value("thought").toBool()){
			text += part.value("text").toString();
		}
	}

	if (text.trimmed().isEmpty()){
		return CreateInvalidReply();
	}

	return CreateTextReply(text);
}


} // namespace imtassist

