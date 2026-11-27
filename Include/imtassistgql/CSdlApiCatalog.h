// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QStringList>


namespace imtassistgql
{


/**
	Catalog of the public GraphQL API of the application, read from its SDL files:
	types, inputs, enums and the commands of \c Query and \c Mutation with their comments.
	The assistant uses it to find, explain and call commands.

	\ingroup imtassistgql
*/
class CSdlApiCatalog
{
public:
	struct TypeRef
	{
		QString name;
		bool isList = false;
		bool isRequired = false;

		QString ToString() const;
	};

	struct Field
	{
		QString name;
		TypeRef type;
		QList<QPair<QString, TypeRef>> arguments;
		QString description;
	};

	typedef QList<Field> Fields;

	enum TypeKind
	{
		TK_OBJECT,
		TK_INPUT,
		TK_ENUM,
		TK_UNION
	};

	struct TypeInfo
	{
		QString name;
		TypeKind kind = TK_OBJECT;
		Fields fields;
		QStringList values;
		QString description;
		QString source;

		const Field* FindField(const QString& fieldName) const;
	};

	enum CommandKind
	{
		CK_QUERY,
		CK_MUTATION
	};

	struct Command
	{
		QString name;
		CommandKind kind = CK_QUERY;
		Field field;
		QString source;
	};

	/**
		Read an SDL document and add its definitions. Definitions already known are kept.
		\return False if the document could not be parsed; definitions read before the error are kept.
	*/
	bool AddSchema(const QString& source, const QString& sdl, QString* errorPtr = nullptr);

	const TypeInfo* FindType(const QString& typeName) const;
	const Command* FindCommand(const QString& commandName) const;
	QStringList GetCommandNames() const;

	/**
		Check whether a type name is a scalar: a built-in scalar or a name without definition.
	*/
	bool IsScalar(const QString& typeName) const;

	/**
		Text that explains a command: signature, comment, the input type expanded
		(nested inputs and enums up to \c maxDepth) and the fields of the result.
	*/
	QString DescribeCommand(const QString& commandName, int maxDepth = 3) const;

private:
	void DescribeType(const QString& typeName, int indentLevel, int remainingDepth, QStringList& describedTypes, QStringList& lines) const;

	QMap<QString, TypeInfo> m_types;
	QMap<QString, Command> m_commands;
};


} // namespace imtassistgql


