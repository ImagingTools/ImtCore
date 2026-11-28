// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassistgql/CAssistantApiActionComp.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtassist/CMarkdownKnowledgeBase.h>
#include <imtassistgql/CAssistantApiRequestBuilder.h>


namespace imtassistgql
{


namespace
{


imtassist::CParameterSchema::Field CreateField(
			const QString& name,
			imtassist::CParameterSchema::FieldType type,
			bool isRequired,
			const QString& description,
			int maxLength = imtassist::CParameterSchema::DEFAULT_MAX_STRING_LENGTH)
{
	imtassist::CParameterSchema::Field retVal;
	retVal.name = name;
	retVal.type = type;
	retVal.isRequired = isRequired;
	retVal.description = description;
	retVal.maxLength = maxLength;
	retVal.maxItems = CAssistantApiRequestBuilder::MAX_FIELDS;

	return retVal;
}


} // anonymous namespace


CAssistantApiActionComp::CAssistantApiActionComp()
	:m_listParameterSchema("api.listCommands.parameters", 1),
	m_listResultSchema("api.listCommands.result", 1),
	m_describeParameterSchema("api.describeCommand.parameters", 1),
	m_describeResultSchema("api.describeCommand.result", 1),
	m_queryParameterSchema("api.query.parameters", 1),
	m_queryResultSchema("api.query.result", 1)
{
	m_listParameterSchema.AddField(CreateField("text", imtassist::CParameterSchema::FT_STRING, false, "Words that the command name or comment contains, e.g. 'group user'", 200));

	std::shared_ptr<imtassist::CParameterSchema> commandSchemaPtr = std::make_shared<imtassist::CParameterSchema>("api.listCommands.command", 1);
	commandSchemaPtr->AddString("name", true, 200);
	commandSchemaPtr->AddEnum("kind", true, QStringList() << "query" << "mutation");
	commandSchemaPtr->AddString("description", false, 1000);
	m_listResultSchema.AddObjectList("commands", true, commandSchemaPtr, MAX_COMMANDS);
	m_listResultSchema.AddBoolean("hasMore", true);

	m_describeParameterSchema.AddField(CreateField("command", imtassist::CParameterSchema::FT_STRING, true, "Name of a command from api.listCommands", 200));
	m_describeResultSchema.AddString("description", true, MAX_DESCRIPTION_LENGTH);

	m_queryParameterSchema.AddField(CreateField("command", imtassist::CParameterSchema::FT_STRING, true, "Name of a query from api.listCommands", 200));
	m_queryParameterSchema.AddField(CreateField("inputJson", imtassist::CParameterSchema::FT_STRING, false, "Value of the argument of the command as JSON (for several arguments: an object with them by name), as described by api.describeCommand", MAX_INPUT_LENGTH));
	m_queryParameterSchema.AddField(CreateField("fields", imtassist::CParameterSchema::FT_STRING_LIST, false, "Wanted result fields as paths, e.g. [\"items.id\", \"items.name\"]; the simple fields when empty"));
	m_queryResultSchema.AddString("resultJson", true, MAX_RESULT_LENGTH + 100);
	m_queryResultSchema.AddInteger("omittedItems", true, 0, std::numeric_limits<qint32>::max());
}


// reimplemented (imtassist::IAssistantAction)

QByteArray CAssistantApiActionComp::GetActionId() const
{
	switch (GetKind()){
	case K_DESCRIBE:
		return "api.describeCommand";
	case K_QUERY:
		return "api.query";
	default:
		return "api.listCommands";
	}
}


QString CAssistantApiActionComp::GetDescription() const
{
	switch (GetKind()){
	case K_DESCRIBE:
		return "Describe a command of the public API of the product: arguments, input type with nested types and enums, result type, comments. Read it before calling a command.";
	case K_QUERY:
		return "Execute a query of the public API with the rights of the user and return the result as JSON. Use it to read data, e.g. lists and items, also to find IDs. Values never go into the query text, only into inputJson.";
	default:
		return "List the commands of the public API of the product that the assistant may use (queries read, mutations change). Use words to narrow the list.";
	}
}


imtassist::IAssistantAction::ActionKind CAssistantApiActionComp::GetActionKind() const
{
	return AK_READ;
}


const imtassist::CParameterSchema& CAssistantApiActionComp::GetParameterSchema() const
{
	switch (GetKind()){
	case K_DESCRIBE:
		return m_describeParameterSchema;
	case K_QUERY:
		return m_queryParameterSchema;
	default:
		return m_listParameterSchema;
	}
}


const imtassist::CParameterSchema& CAssistantApiActionComp::GetResultSchema() const
{
	switch (GetKind()){
	case K_DESCRIBE:
		return m_describeResultSchema;
	case K_QUERY:
		return m_queryResultSchema;
	default:
		return m_listResultSchema;
	}
}


bool CAssistantApiActionComp::CheckAccess(const imtassist::CAssistantRequestContext& context, const QJsonObject& parameters) const
{
	if (!context.IsValid() || !m_apiCatalogCompPtr.IsValid()){
		return false;
	}

	if (GetKind() != K_QUERY){
		return true;
	}

	// The handlers check the permissions of the command; here only the context and the policy.
	const QString commandName = parameters.value("command").toString();
	const CSdlApiCatalog::Command* commandPtr = m_apiCatalogCompPtr->GetCatalog().FindCommand(commandName);

	return
				(GetGqlContext(context) != nullptr) &&
				(commandPtr != nullptr) &&
				(commandPtr->kind == CSdlApiCatalog::CK_QUERY) &&
				m_apiCatalogCompPtr->IsCommandAllowed(commandName);
}


imtassist::IAssistantAction::ExecutionStatus CAssistantApiActionComp::Execute(
			const imtassist::CAssistantRequestContext& context,
			const QJsonObject& parameters,
			Result& result,
			QString& userError)
{
	const CSdlApiCatalog& catalog = m_apiCatalogCompPtr->GetCatalog();
	const Kind kind = GetKind();

	if (kind == K_LIST){
		const QStringList words = imtassist::CMarkdownKnowledgeBase::SplitWords(parameters.value("text").toString());

		QJsonArray commands;
		bool hasMore = false;
		const QStringList commandNames = catalog.GetCommandNames();
		for (const QString& commandName: commandNames){
			if (!m_apiCatalogCompPtr->IsCommandAllowed(commandName)){
				continue;
			}

			const CSdlApiCatalog::Command* commandPtr = catalog.FindCommand(commandName);
			const QString searchText = (commandName + QLatin1Char(' ') + commandPtr->field.description).toLower();
			bool isMatching = words.isEmpty();
			for (const QString& word: words){
				isMatching = isMatching || searchText.contains(word);
			}

			if (!isMatching){
				continue;
			}

			if (commands.size() >= MAX_COMMANDS){
				hasMore = true;

				break;
			}

			QJsonObject command;
			command["name"] = commandName;
			command["kind"] = (commandPtr->kind == CSdlApiCatalog::CK_QUERY) ? QStringLiteral("query") : QStringLiteral("mutation");
			if (!commandPtr->field.description.isEmpty()){
				command["description"] = commandPtr->field.description.left(1000);
			}

			commands.append(command);
		}

		result.data["commands"] = commands;
		result.data["hasMore"] = hasMore;

		return ES_SUCCEEDED;
	}

	const QString commandName = parameters.value("command").toString();
	if (!m_apiCatalogCompPtr->IsCommandAllowed(commandName)){
		userError = QStringLiteral("The command is not available to the assistant.");

		return ES_FAILED;
	}

	if (kind == K_DESCRIBE){
		result.data["description"] = catalog.DescribeCommand(commandName).left(MAX_DESCRIPTION_LENGTH);

		return ES_SUCCEEDED;
	}

	QJsonValue input;
	const QString inputJson = parameters.value("inputJson").toString().trimmed();
	if (!inputJson.isEmpty()){
		QJsonParseError parseError;
		const QJsonDocument document = QJsonDocument::fromJson(inputJson.toUtf8(), &parseError);
		if (parseError.error != QJsonParseError::NoError){
			userError = QStringLiteral("inputJson is not valid JSON.");

			return ES_FAILED;
		}

		input = document.isObject() ? QJsonValue(document.object()) : QJsonValue(document.array());
	}

	QStringList fields;
	const QJsonArray fieldsArray = parameters.value("fields").toArray();
	for (const QJsonValue& field: fieldsArray){
		fields.append(field.toString());
	}

	QByteArray query;
	QJsonObject variables;
	QString error;
	if (!CAssistantApiRequestBuilder::BuildRequest(*m_apiCatalogCompPtr, commandName, input, fields, query, variables, error)){
		// The text names fields and arguments of the schema only; the model corrects the call with it.
		userError = error;

		return ES_FAILED;
	}

	QJsonObject data;
	if (!ExecuteGql(context, query, variables, data, error)){
		SendWarningMessage(0, QStringLiteral("Assistant API query failed: %1").arg(error), "AssistantApi");
		userError = QStringLiteral("The query was refused or failed.");

		return ES_FAILED;
	}

	int omittedItems = 0;
	const QJsonValue filtered = CAssistantApiRequestBuilder::FilterResult(*m_apiCatalogCompPtr, data, omittedItems);
	QString resultJson = QString::fromUtf8(QJsonDocument(filtered.toObject()).toJson(QJsonDocument::Compact));
	if (resultJson.size() > MAX_RESULT_LENGTH){
		resultJson = resultJson.left(MAX_RESULT_LENGTH) + QStringLiteral("...(truncated; ask for fewer fields or use a filter)");
	}

	result.data["resultJson"] = resultJson;
	result.data["omittedItems"] = omittedItems;

	return ES_SUCCEEDED;
}


// private methods

CAssistantApiActionComp::Kind CAssistantApiActionComp::GetKind() const
{
	const QByteArray kind = m_kindAttrPtr.IsValid() ? *m_kindAttrPtr : QByteArray();
	if (kind == "describe"){
		return K_DESCRIBE;
	}

	if (kind == "query"){
		return K_QUERY;
	}

	return K_LIST;
}


} // namespace imtassistgql


