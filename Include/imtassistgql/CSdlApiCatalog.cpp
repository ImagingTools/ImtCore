// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassistgql/CSdlApiCatalog.h>


namespace imtassistgql
{


namespace
{


struct Token
{
	enum Kind
	{
		TK_NAME,
		TK_PUNCTUATION,
		TK_STRING,
		TK_COMMENT,
		TK_END
	};

	Kind kind = TK_END;
	QString text;
};


QList<Token> Tokenize(const QString& sdl)
{
	QList<Token> retVal;

	int position = 0;
	const int length = sdl.size();
	while (position < length){
		const QChar character = sdl.at(position);
		if (character.isSpace() || (character == QLatin1Char(','))){
			++position;

			continue;
		}

		Token token;
		if (character == QLatin1Char('#')){
			int end = sdl.indexOf(QLatin1Char('\n'), position);
			if (end < 0){
				end = length;
			}

			token.kind = Token::TK_COMMENT;
			token.text = sdl.mid(position + 1, end - position - 1).trimmed();
			position = end;
		}
		else if (character == QLatin1Char('"')){
			int end = position + 1;
			while ((end < length) && (sdl.at(end) != QLatin1Char('"'))){
				end += (sdl.at(end) == QLatin1Char('\\')) ? 2 : 1;
			}

			token.kind = Token::TK_STRING;
			token.text = sdl.mid(position + 1, end - position - 1);
			position = end + 1;
		}
		else if (character.isLetterOrNumber() || (character == QLatin1Char('_')) || (character == QLatin1Char('.')) || (character == QLatin1Char('-'))){
			int end = position;
			while ((end < length) && (sdl.at(end).isLetterOrNumber() || (sdl.at(end) == QLatin1Char('_')) || (sdl.at(end) == QLatin1Char('.')) || (sdl.at(end) == QLatin1Char('-')))){
				++end;
			}

			token.kind = Token::TK_NAME;
			token.text = sdl.mid(position, end - position);
			position = end;
		}
		else{
			token.kind = Token::TK_PUNCTUATION;
			token.text = character;
			++position;
		}

		retVal.append(token);
	}

	return retVal;
}


class CParser
{
public:
	explicit CParser(const QList<Token>& tokens)
		:m_tokens(tokens)
	{
	}

	bool IsEnd()
	{
		SkipComments();

		return m_position >= m_tokens.size();
	}

	// Comments since the last call, also those passed by a look-ahead, joined; they are consumed.
	QString TakeComment()
	{
		SkipComments();

		const QString retVal = m_pendingComments.join(QLatin1Char(' ')).trimmed();
		m_pendingComments.clear();

		return retVal;
	}

	Token Peek()
	{
		SkipComments();

		return (m_position < m_tokens.size()) ? m_tokens.at(m_position) : Token();
	}

	Token Next()
	{
		Token retVal = Peek();
		if (m_position < m_tokens.size()){
			++m_position;
		}

		return retVal;
	}

	bool Accept(const QString& punctuation)
	{
		const Token token = Peek();
		if ((token.kind == Token::TK_PUNCTUATION) && (token.text == punctuation)){
			Next();

			return true;
		}

		return false;
	}

	bool Expect(const QString& punctuation, QString& error)
	{
		if (Accept(punctuation)){
			return true;
		}

		error = QStringLiteral("'%1' expected before '%2'").arg(punctuation, Peek().text);

		return false;
	}

	bool ExpectName(QString& name, QString& error)
	{
		const Token token = Peek();
		if (token.kind != Token::TK_NAME){
			error = QStringLiteral("Name expected before '%1'").arg(token.text);

			return false;
		}

		name = Next().text;

		return true;
	}

	// Skip a block in braces, with nested blocks.
	bool SkipBlock(QString& error)
	{
		if (!Expect(QStringLiteral("{"), error)){
			return false;
		}

		int depth = 1;
		while ((depth > 0) && (m_position < m_tokens.size())){
			const Token token = m_tokens.at(m_position++);
			if (token.kind == Token::TK_PUNCTUATION){
				if (token.text == QLatin1String("{")){
					++depth;
				}
				else if (token.text == QLatin1String("}")){
					--depth;
				}
			}
		}

		if (depth > 0){
			error = QStringLiteral("Unterminated block");

			return false;
		}

		return true;
	}

	bool ParseTypeRef(CSdlApiCatalog::TypeRef& typeRef, QString& error)
	{
		if (Accept(QStringLiteral("["))){
			CSdlApiCatalog::TypeRef itemRef;
			if (!ParseTypeRef(itemRef, error) || !Expect(QStringLiteral("]"), error)){
				return false;
			}

			typeRef.name = itemRef.name;
			typeRef.isList = true;
		}
		else if (!ExpectName(typeRef.name, error)){
			return false;
		}

		typeRef.isRequired = Accept(QStringLiteral("!"));

		return true;
	}

	bool ParseFields(CSdlApiCatalog::Fields& fields, QString& error)
	{
		if (!Expect(QStringLiteral("{"), error)){
			return false;
		}

		while (true){
			const QString comment = TakeComment();
			if (Accept(QStringLiteral("}"))){
				return true;
			}

			if (IsEnd()){
				error = QStringLiteral("Unterminated type");

				return false;
			}

			CSdlApiCatalog::Field field;
			field.description = comment;
			if (!ExpectName(field.name, error)){
				return false;
			}

			if (Accept(QStringLiteral("("))){
				while (!Accept(QStringLiteral(")"))){
					QPair<QString, CSdlApiCatalog::TypeRef> argument;
					if (!ExpectName(argument.first, error) || !Expect(QStringLiteral(":"), error) || !ParseTypeRef(argument.second, error)){
						return false;
					}

					// A default value is not needed for the catalog.
					if (Accept(QStringLiteral("="))){
						Next();
					}

					field.arguments.append(argument);
				}
			}

			if (!Expect(QStringLiteral(":"), error) || !ParseTypeRef(field.type, error)){
				return false;
			}

			fields.append(field);
		}
	}

	bool ParseEnumValues(QStringList& values, QString& error)
	{
		if (!Expect(QStringLiteral("{"), error)){
			return false;
		}

		while (!Accept(QStringLiteral("}"))){
			QString value;
			if (!ExpectName(value, error)){
				return false;
			}

			values.append(value);
		}

		return true;
	}

	bool ParseUnion(QStringList& members, QString& error)
	{
		if (!Expect(QStringLiteral("="), error)){
			return false;
		}

		do{
			Accept(QStringLiteral("|"));

			QString member;
			if (!ExpectName(member, error)){
				return false;
			}

			members.append(member);
		}while (Peek().kind == Token::TK_PUNCTUATION && Peek().text == QLatin1String("|"));

		return true;
	}

private:
	void SkipComments()
	{
		while ((m_position < m_tokens.size()) && (m_tokens.at(m_position).kind == Token::TK_COMMENT)){
			m_pendingComments.append(m_tokens.at(m_position).text);
			++m_position;
		}
	}

	QList<Token> m_tokens;
	QStringList m_pendingComments;
	int m_position = 0;
};


bool IsBuiltInScalar(const QString& typeName)
{
	static const QStringList scalars = {
		"ID", "String", "Int", "Integer", "Long", "Float", "Double", "Boolean", "Bool", "Date", "DateTime", "Time", "Json", "JSON"
	};

	return scalars.contains(typeName);
}


} // anonymous namespace


// public methods of the embedded class TypeRef

QString CSdlApiCatalog::TypeRef::ToString() const
{
	QString retVal = isList ? QStringLiteral("[%1]").arg(name) : name;
	if (isRequired){
		retVal.append(QLatin1Char('!'));
	}

	return retVal;
}


// public methods of the embedded class TypeInfo

const CSdlApiCatalog::Field* CSdlApiCatalog::TypeInfo::FindField(const QString& fieldName) const
{
	for (const Field& field: fields){
		if (field.name == fieldName){
			return &field;
		}
	}

	return nullptr;
}


// public methods

bool CSdlApiCatalog::AddSchema(const QString& source, const QString& sdl, QString* errorPtr)
{
	CParser parser(Tokenize(sdl));
	QString error;

	while (!parser.IsEnd()){
		const QString comment = parser.TakeComment();

		QString keyword;
		if (!parser.ExpectName(keyword, error)){
			break;
		}

		if ((keyword == QLatin1String("schema")) || (keyword == QLatin1String("import")) || (keyword == QLatin1String("collectionSchema"))){
			if (!parser.SkipBlock(error)){
				break;
			}

			continue;
		}

		TypeInfo typeInfo;
		typeInfo.description = comment;
		typeInfo.source = source;
		if (!parser.ExpectName(typeInfo.name, error)){
			break;
		}

		bool isParsed = false;
		if ((keyword == QLatin1String("type")) || (keyword == QLatin1String("input"))){
			typeInfo.kind = (keyword == QLatin1String("input")) ? TK_INPUT : TK_OBJECT;
			isParsed = parser.ParseFields(typeInfo.fields, error);
		}
		else if (keyword == QLatin1String("enum")){
			typeInfo.kind = TK_ENUM;
			isParsed = parser.ParseEnumValues(typeInfo.values, error);
		}
		else if (keyword == QLatin1String("union")){
			typeInfo.kind = TK_UNION;
			isParsed = parser.ParseUnion(typeInfo.values, error);
		}
		else{
			error = QStringLiteral("Unknown definition '%1'").arg(keyword);
		}

		if (!isParsed){
			break;
		}

		const bool isQuery = (typeInfo.name == QLatin1String("Query"));
		const bool isMutation = (typeInfo.name == QLatin1String("Mutation"));
		if (isQuery || isMutation){
			for (const Field& field: typeInfo.fields){
				if (!m_commands.contains(field.name)){
					Command command;
					command.name = field.name;
					command.kind = isQuery ? CK_QUERY : CK_MUTATION;
					command.field = field;
					command.source = source;
					m_commands.insert(command.name, command);
				}
			}
		}
		else if ((typeInfo.name != QLatin1String("Subscription")) && !m_types.contains(typeInfo.name)){
			m_types.insert(typeInfo.name, typeInfo);
		}
	}

	if (!error.isEmpty()){
		if (errorPtr != nullptr){
			*errorPtr = QStringLiteral("%1: %2").arg(source, error);
		}

		return false;
	}

	return true;
}


const CSdlApiCatalog::TypeInfo* CSdlApiCatalog::FindType(const QString& typeName) const
{
	QMap<QString, TypeInfo>::const_iterator iter = m_types.constFind(typeName);

	return (iter != m_types.constEnd()) ? &iter.value() : nullptr;
}


const CSdlApiCatalog::Command* CSdlApiCatalog::FindCommand(const QString& commandName) const
{
	QMap<QString, Command>::const_iterator iter = m_commands.constFind(commandName);

	return (iter != m_commands.constEnd()) ? &iter.value() : nullptr;
}


QStringList CSdlApiCatalog::GetCommandNames() const
{
	return m_commands.keys();
}


bool CSdlApiCatalog::IsScalar(const QString& typeName) const
{
	return IsBuiltInScalar(typeName) || !m_types.contains(typeName);
}


QString CSdlApiCatalog::DescribeCommand(const QString& commandName, int maxDepth) const
{
	const Command* commandPtr = FindCommand(commandName);
	if (commandPtr == nullptr){
		return QString();
	}

	QStringList arguments;
	for (const QPair<QString, TypeRef>& argument: commandPtr->field.arguments){
		arguments.append(QStringLiteral("%1: %2").arg(argument.first, argument.second.ToString()));
	}

	QStringList lines;
	lines.append(QStringLiteral("%1 %2(%3): %4").arg(
				(commandPtr->kind == CK_QUERY) ? QStringLiteral("query") : QStringLiteral("mutation"),
				commandPtr->name,
				arguments.join(QStringLiteral(", ")),
				commandPtr->field.type.ToString()));
	if (!commandPtr->field.description.isEmpty()){
		lines.append(QStringLiteral("# %1").arg(commandPtr->field.description));
	}

	QStringList describedTypes;
	for (const QPair<QString, TypeRef>& argument: commandPtr->field.arguments){
		lines.append(QString());
		lines.append(QStringLiteral("Input %1:").arg(argument.first));
		DescribeType(argument.second.name, 1, maxDepth, describedTypes, lines);
	}

	lines.append(QString());
	lines.append(QStringLiteral("Result:"));
	DescribeType(commandPtr->field.type.name, 1, maxDepth, describedTypes, lines);

	return lines.join(QLatin1Char('\n'));
}


// private methods

void CSdlApiCatalog::DescribeType(const QString& typeName, int indentLevel, int remainingDepth, QStringList& describedTypes, QStringList& lines) const
{
	const QString indent(indentLevel * 2, QLatin1Char(' '));
	const TypeInfo* typePtr = FindType(typeName);
	if (typePtr == nullptr){
		lines.append(QStringLiteral("%1%2 (scalar)").arg(indent, typeName));

		return;
	}

	if (describedTypes.contains(typeName)){
		lines.append(QStringLiteral("%1%2 (see above)").arg(indent, typeName));

		return;
	}

	describedTypes.append(typeName);

	const QString description = typePtr->description.isEmpty() ? QString() : QStringLiteral("  # %1").arg(typePtr->description);
	if (typePtr->kind == TK_ENUM){
		lines.append(QStringLiteral("%1enum %2: %3%4").arg(indent, typeName, typePtr->values.join(QStringLiteral(" | ")), description));

		return;
	}

	if (typePtr->kind == TK_UNION){
		lines.append(QStringLiteral("%1union %2: %3%4").arg(indent, typeName, typePtr->values.join(QStringLiteral(" | ")), description));

		return;
	}

	lines.append(QStringLiteral("%1%2 {%3").arg(indent, typeName, description));
	for (const Field& field: typePtr->fields){
		QString line = QStringLiteral("%1  %2: %3").arg(indent, field.name, field.type.ToString());
		if (!field.description.isEmpty()){
			line += QStringLiteral("  # %1").arg(field.description);
		}

		lines.append(line);

		if (!IsScalar(field.type.name) && (remainingDepth > 1)){
			DescribeType(field.type.name, indentLevel + 2, remainingDepth - 1, describedTypes, lines);
		}
	}

	lines.append(QStringLiteral("%1}").arg(indent));
}


} // namespace imtassistgql


