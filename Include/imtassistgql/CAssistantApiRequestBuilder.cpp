// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassistgql/CAssistantApiRequestBuilder.h>


// Qt includes
#include <QtCore/QJsonArray>


namespace imtassistgql
{


// public methods

bool CAssistantApiRequestBuilder::BuildRequest(
			const IAssistantApiCatalog& catalog,
			const QString& commandName,
			const QJsonValue& input,
			const QStringList& fields,
			QByteArray& query,
			QJsonObject& variables,
			QString& error)
{
	const CSdlApiCatalog& apiCatalog = catalog.GetCatalog();
	const CSdlApiCatalog::Command* commandPtr = apiCatalog.FindCommand(commandName);
	if ((commandPtr == nullptr) || !catalog.IsCommandAllowed(commandName)){
		error = QStringLiteral("The command '%1' is not available to the assistant").arg(commandName);

		return false;
	}

	if (!CheckInput(catalog, input, error)){
		return false;
	}

	// A change without input is never meant: the arguments of the mutations are optional in the schema only.
	if ((commandPtr->kind == CSdlApiCatalog::CK_MUTATION) && !commandPtr->field.arguments.isEmpty() && (input.isUndefined() || input.isNull())){
		error = QStringLiteral("The input of '%1' is missing").arg(commandName);

		return false;
	}

	// Variables: the single argument takes the input, several arguments take the fields of an object.
	variables = QJsonObject();
	QStringList declarations;
	QStringList arguments;
	const QList<QPair<QString, CSdlApiCatalog::TypeRef>>& commandArguments = commandPtr->field.arguments;
	for (const QPair<QString, CSdlApiCatalog::TypeRef>& argument: commandArguments){
		QJsonValue value;
		if (commandArguments.size() == 1){
			value = input;
		}
		else if (input.isObject()){
			value = input.toObject().value(argument.first);
		}

		if (value.isUndefined() || value.isNull()){
			if (argument.second.isRequired){
				error = QStringLiteral("The argument '%1' of '%2' is missing").arg(argument.first, commandName);

				return false;
			}

			continue;
		}

		variables.insert(argument.first, value);
		declarations.append(QStringLiteral("$%1: %2").arg(argument.first, argument.second.ToString()));
		arguments.append(QStringLiteral("%1: $%1").arg(argument.first));
	}

	SelectionNode root;
	const QString resultType = commandPtr->field.type.name;
	if (!apiCatalog.IsScalar(resultType)){
		if (fields.size() > MAX_FIELDS){
			error = QStringLiteral("Too many result fields");

			return false;
		}

		for (const QString& path: fields){
			if (!AddPath(catalog, resultType, path.split(QLatin1Char('.'), Qt::SkipEmptyParts), root, error)){
				return false;
			}
		}

		if (root.children.isEmpty()){
			AddScalarFields(catalog, resultType, root, 0);
		}

		if (root.children.isEmpty()){
			error = QStringLiteral("The result of '%1' has no readable fields").arg(commandName);

			return false;
		}
	}

	QString text = (commandPtr->kind == CSdlApiCatalog::CK_QUERY) ? QStringLiteral("query Assistant") : QStringLiteral("mutation Assistant");
	if (!declarations.isEmpty()){
		text += QStringLiteral("(%1)").arg(declarations.join(QStringLiteral(", ")));
	}

	text += QStringLiteral(" { %1").arg(commandName);
	if (!arguments.isEmpty()){
		text += QStringLiteral("(%1)").arg(arguments.join(QStringLiteral(", ")));
	}

	if (!root.children.isEmpty()){
		text += QLatin1Char(' ') + CreateSelectionText(root);
	}

	text += QStringLiteral(" }");
	query = text.toUtf8();

	return true;
}


bool CAssistantApiRequestBuilder::CheckInput(const IAssistantApiCatalog& catalog, const QJsonValue& input, QString& error, int depth)
{
	if (depth > MAX_INPUT_DEPTH){
		error = QStringLiteral("The input is nested too deeply");

		return false;
	}

	if (input.isObject()){
		const QJsonObject object = input.toObject();
		for (QJsonObject::const_iterator iter = object.constBegin(); iter != object.constEnd(); ++iter){
			if (catalog.IsFieldForbidden(iter.key())){
				error = QStringLiteral("The field '%1' cannot be set by the assistant").arg(iter.key());

				return false;
			}

			if (!CheckInput(catalog, iter.value(), error, depth + 1)){
				return false;
			}
		}
	}
	else if (input.isArray()){
		const QJsonArray array = input.toArray();
		for (const QJsonValue& item: array){
			if (!CheckInput(catalog, item, error, depth + 1)){
				return false;
			}
		}
	}

	return true;
}


QJsonValue CAssistantApiRequestBuilder::FilterResult(const IAssistantApiCatalog& catalog, const QJsonValue& value, int& truncatedCount)
{
	if (value.isObject()){
		QJsonObject retVal;
		const QJsonObject object = value.toObject();
		for (QJsonObject::const_iterator iter = object.constBegin(); iter != object.constEnd(); ++iter){
			if (!catalog.IsFieldForbidden(iter.key())){
				retVal.insert(iter.key(), FilterResult(catalog, iter.value(), truncatedCount));
			}
		}

		return retVal;
	}

	if (value.isArray()){
		QJsonArray retVal;
		const QJsonArray array = value.toArray();
		for (const QJsonValue& item: array){
			if (retVal.size() >= MAX_LIST_ITEMS){
				++truncatedCount;

				continue;
			}

			retVal.append(FilterResult(catalog, item, truncatedCount));
		}

		return retVal;
	}

	return value;
}


// private methods

bool CAssistantApiRequestBuilder::AddPath(
			const IAssistantApiCatalog& catalog,
			const QString& typeName,
			const QStringList& segments,
			SelectionNode& node,
			QString& error)
{
	if (segments.isEmpty()){
		return true;
	}

	const CSdlApiCatalog& apiCatalog = catalog.GetCatalog();
	const CSdlApiCatalog::TypeInfo* typePtr = apiCatalog.FindType(typeName);
	const QString& fieldName = segments.first();
	const CSdlApiCatalog::Field* fieldPtr = (typePtr != nullptr) ? typePtr->FindField(fieldName) : nullptr;
	if (fieldPtr == nullptr){
		QStringList fieldNames;
		if (typePtr != nullptr){
			for (const CSdlApiCatalog::Field& field: typePtr->fields){
				if (!catalog.IsFieldForbidden(field.name)){
					fieldNames.append(field.name);
				}
			}
		}

		error = QStringLiteral("Unknown field '%1' of %2; fields: %3").arg(fieldName, typeName, fieldNames.join(QStringLiteral(", ")));

		return false;
	}

	if (catalog.IsFieldForbidden(fieldName)){
		error = QStringLiteral("The field '%1' is not available to the assistant").arg(fieldName);

		return false;
	}

	SelectionNode& child = node.children[fieldName];
	if (apiCatalog.IsScalar(fieldPtr->type.name)){
		if (segments.size() > 1){
			error = QStringLiteral("The field '%1' has no sub-fields").arg(fieldName);

			return false;
		}

		return true;
	}

	if (segments.size() == 1){
		AddScalarFields(catalog, fieldPtr->type.name, child, 1);

		return true;
	}

	return AddPath(catalog, fieldPtr->type.name, segments.mid(1), child, error);
}


void CAssistantApiRequestBuilder::AddScalarFields(const IAssistantApiCatalog& catalog, const QString& typeName, SelectionNode& node, int depth)
{
	const CSdlApiCatalog& apiCatalog = catalog.GetCatalog();
	const CSdlApiCatalog::TypeInfo* typePtr = apiCatalog.FindType(typeName);
	if ((typePtr == nullptr) || (typePtr->kind != CSdlApiCatalog::TK_OBJECT)){
		return;
	}

	for (const CSdlApiCatalog::Field& field: typePtr->fields){
		if (catalog.IsFieldForbidden(field.name) || !field.arguments.isEmpty()){
			continue;
		}

		if (apiCatalog.IsScalar(field.type.name)){
			node.children[field.name];
		}
		else if (depth == 0){
			// One level of objects, e.g. the items of a list
			SelectionNode child;
			AddScalarFields(catalog, field.type.name, child, depth + 1);
			if (!child.children.isEmpty()){
				node.children.insert(field.name, child);
			}
		}
	}
}


QString CAssistantApiRequestBuilder::CreateSelectionText(const SelectionNode& node)
{
	QStringList parts;
	for (QMap<QString, SelectionNode>::const_iterator iter = node.children.constBegin(); iter != node.children.constEnd(); ++iter){
		parts.append(iter.value().children.isEmpty() ? iter.key() : QStringLiteral("%1 %2").arg(iter.key(), CreateSelectionText(iter.value())));
	}

	return QStringLiteral("{ %1 }").arg(parts.join(QLatin1Char(' ')));
}


} // namespace imtassistgql


