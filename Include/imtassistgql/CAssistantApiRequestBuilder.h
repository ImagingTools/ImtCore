// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QStringList>

// ImtCore includes
#include <imtassistgql/IAssistantApiCatalog.h>


namespace imtassistgql
{


/**
	Builds GraphQL requests of the assistant from a command, its input and the wanted result
	fields, checked against the API catalog, and filters the results. The model never writes
	GraphQL text: its values are passed only as variables.

	\ingroup imtassistgql
*/
class CAssistantApiRequestBuilder
{
public:
	enum
	{
		MAX_FIELDS = 100,
		MAX_LIST_ITEMS = 100,
		MAX_INPUT_DEPTH = 10
	};

	/**
		Build the query text and the variables of a command.
		\param input	Value of the single argument of the command, or an object with the arguments by name.
		\param fields	Wanted result fields as paths, e.g. "items.id"; the scalar fields of the result when empty.
	*/
	static bool BuildRequest(
				const IAssistantApiCatalog& catalog,
				const QString& commandName,
				const QJsonValue& input,
				const QStringList& fields,
				QByteArray& query,
				QJsonObject& variables,
				QString& error);

	/**
		Refuse inputs with forbidden fields (e.g. passwords) or too deep nesting.
	*/
	static bool CheckInput(const IAssistantApiCatalog& catalog, const QJsonValue& input, QString& error, int depth = 0);

	/**
		Remove forbidden fields and shorten long lists.
		\param truncatedCount	Number of removed list items.
	*/
	static QJsonValue FilterResult(const IAssistantApiCatalog& catalog, const QJsonValue& value, int& truncatedCount);

private:
	struct SelectionNode
	{
		QMap<QString, SelectionNode> children;
	};

	static bool AddPath(
				const IAssistantApiCatalog& catalog,
				const QString& typeName,
				const QStringList& segments,
				SelectionNode& node,
				QString& error);
	static void AddScalarFields(const IAssistantApiCatalog& catalog, const QString& typeName, SelectionNode& node, int depth);
	static QString CreateSelectionText(const SelectionNode& node);
};


} // namespace imtassistgql


