// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotify/NotificationMessageFormat.h>


namespace imtnotify
{


namespace NotificationMessageFormat
{


namespace
{


const QSet<QString> s_pluralCategories = {
			QStringLiteral("zero"),
			QStringLiteral("one"),
			QStringLiteral("two"),
			QStringLiteral("few"),
			QStringLiteral("many"),
			QStringLiteral("other")};


bool IsWordCharacter(QChar character)
{
	const char16_t code = character.unicode();

	return (code >= u'a' && code <= u'z') || (code >= u'A' && code <= u'Z') || (code >= u'0' && code <= u'9') || code == u'_';
}


class CParser
{
public:
	CParser(const QString& message, QSet<QByteArray>& argumentIds, QString& errorMessage)
		:m_message(message),
		m_argumentIds(argumentIds),
		m_errorMessage(errorMessage)
	{
	}

	bool Parse()
	{
		return ParseText(false);
	}

private:
	bool Fail(const QString& reason)
	{
		m_errorMessage = QStringLiteral("%1 at %2").arg(reason).arg(m_position);

		return false;
	}

	bool IsAt(QChar character) const
	{
		return m_position < m_message.size() && m_message[m_position] == character;
	}

	void SkipSpaces()
	{
		while (m_position < m_message.size() && m_message[m_position].isSpace()){
			++m_position;
		}
	}

	QString ReadWord()
	{
		const int start = m_position;
		while (m_position < m_message.size() && IsWordCharacter(m_message[m_position])){
			++m_position;
		}

		return m_message.mid(start, m_position - start);
	}

	// At an apostrophe: '' is one, before a special character it quotes up to the next one
	void SkipQuote()
	{
		const QChar next = m_position + 1 < m_message.size() ? m_message[m_position + 1] : QChar();
		if (next == QLatin1Char('\'')){
			m_position += 2;

			return;
		}

		++m_position;
		if (next != QLatin1Char('{') && next != QLatin1Char('}') && next != QLatin1Char('#')){
			return;
		}

		while (m_position < m_message.size()){
			if (IsAt(QLatin1Char('\''))){
				if (m_position + 1 < m_message.size() && m_message[m_position + 1] == QLatin1Char('\'')){
					m_position += 2;

					continue;
				}

				++m_position;

				return;
			}

			++m_position;
		}
	}

	// A text up to the end or, nested, up to the closing brace of its branch
	bool ParseText(bool isNested)
	{
		while (m_position < m_message.size()){
			if (IsAt(QLatin1Char('\''))){
				SkipQuote();
			}
			else if (IsAt(QLatin1Char('{'))){
				++m_position;
				if (!ParseArgument()){
					return false;
				}
			}
			else if (IsAt(QLatin1Char('}'))){
				return isNested ? true : Fail(QStringLiteral("Unexpected '}'"));
			}
			else{
				++m_position;
			}
		}

		return isNested ? Fail(QStringLiteral("Missing '}'")) : true;
	}

	bool ParseArgument()
	{
		SkipSpaces();
		const QString argumentId = ReadWord();
		if (argumentId.isEmpty()){
			return Fail(QStringLiteral("Missing argument id"));
		}

		m_argumentIds.insert(argumentId.toUtf8());

		SkipSpaces();
		if (IsAt(QLatin1Char('}'))){
			++m_position;

			return true;
		}

		if (!IsAt(QLatin1Char(','))){
			return Fail(QStringLiteral("Expected ',' or '}'"));
		}

		++m_position;
		SkipSpaces();
		const QString argumentType = ReadWord();
		const bool isPlural = argumentType == QStringLiteral("plural");
		if (!isPlural && argumentType != QStringLiteral("select")){
			return Fail(QStringLiteral("Unknown argument type '%1'").arg(argumentType));
		}

		SkipSpaces();
		if (!IsAt(QLatin1Char(','))){
			return Fail(QStringLiteral("Expected ','"));
		}

		++m_position;

		bool hasOther = false;
		for (;;){
			SkipSpaces();
			if (IsAt(QLatin1Char('}'))){
				++m_position;
				break;
			}

			QString selector;
			if (IsAt(QLatin1Char('='))){
				++m_position;
				selector = ReadWord();
				bool isNumber = false;
				selector.toInt(&isNumber);
				if (!isPlural || !isNumber){
					return Fail(QStringLiteral("'=' needs a number of a plural"));
				}
			}
			else{
				selector = ReadWord();
				if (selector.isEmpty() || (isPlural && !s_pluralCategories.contains(selector))){
					return Fail(QStringLiteral("Invalid branch '%1'").arg(selector));
				}
			}

			SkipSpaces();
			if (!IsAt(QLatin1Char('{'))){
				return Fail(QStringLiteral("Expected '{'"));
			}

			++m_position;
			if (!ParseText(true)){
				return false;
			}

			++m_position;
			hasOther = hasOther || selector == QStringLiteral("other");
		}

		return hasOther ? true : Fail(QStringLiteral("Missing branch 'other' of '%1'").arg(argumentId));
	}

private:
	const QString& m_message;
	QSet<QByteArray>& m_argumentIds;
	QString& m_errorMessage;
	int m_position = 0;
};


} // namespace


bool GetArgumentIds(const QString& message, QSet<QByteArray>& argumentIds, QString& errorMessage)
{
	errorMessage.clear();

	return CParser(message, argumentIds, errorMessage).Parse();
}


} // namespace NotificationMessageFormat


} // namespace imtnotify


