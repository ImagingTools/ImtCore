// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CMarkdownKnowledgeBase.h>


// STL includes
#include <algorithm>

namespace imtassist
{


// public methods

int CMarkdownKnowledgeBase::AddDocument(const QString& source, const QString& markdown)
{
	const int countBefore = m_sections.size();

	// Heading path, e.g. "Users > Locking"; code blocks are not searched for headings
	QStringList headings;
	QString title = source;
	QStringList lines;
	bool isInCodeBlock = false;

	const QStringList documentLines = QString(markdown).replace(QStringLiteral("\r\n"), QStringLiteral("\n")).split(QLatin1Char('\n'));
	for (const QString& line: documentLines){
		if (line.trimmed().startsWith(QStringLiteral("```"))){
			isInCodeBlock = !isInCodeBlock;
			lines.append(line);

			continue;
		}

		int level = 0;
		while (!isInCodeBlock && (level < line.size()) && (line.at(level) == QLatin1Char('#'))){
			++level;
		}

		const bool isHeading = (level > 0) && (level <= 6) && (line.size() > level) && (line.at(level) == QLatin1Char(' '));
		if (!isHeading){
			lines.append(line);

			continue;
		}

		AddSection(source, title, lines.join(QLatin1Char('\n')));
		lines.clear();

		while (headings.size() >= level){
			headings.removeLast();
		}

		while (headings.size() < level - 1){
			headings.append(QString());
		}

		headings.append(line.mid(level + 1).trimmed());

		QStringList path;
		for (const QString& heading: headings){
			if (!heading.isEmpty()){
				path.append(heading);
			}
		}

		title = path.join(QStringLiteral(" > "));
	}

	AddSection(source, title, lines.join(QLatin1Char('\n')));

	return m_sections.size() - countBefore;
}


void CMarkdownKnowledgeBase::Clear()
{
	m_sections.clear();
	m_sectionIndexes.clear();
	m_sectionsPerSource.clear();
}


// reimplemented (imtassist::IAssistantKnowledgeBase)

IAssistantKnowledgeBase::Hits CMarkdownKnowledgeBase::Search(const QString& text, int maxHits) const
{
	Hits retVal;

	const QStringList words = SplitWords(text);
	QStringList queryWords;
	for (const QString& word: words){
		if (!queryWords.contains(word)){
			queryWords.append(word);
		}
	}

	if (queryWords.isEmpty() || (maxHits <= 0)){
		return retVal;
	}

	for (const Section& section: m_sections){
		double score = 0;
		int matchedWords = 0;
		for (const QString& word: queryWords){
			int titleCount = 0;
			int textCount = 0;
			for (const QString& titleWord: section.titleWords){
				if (titleWord.startsWith(word)){
					++titleCount;
				}
			}

			for (const QString& textWord: section.textWords){
				if (textWord.startsWith(word)){
					++textCount;
				}
			}

			if ((titleCount + textCount) > 0){
				++matchedWords;
				score += 3.0 * titleCount + std::min(textCount, 10);
			}
		}

		if (matchedWords == 0){
			continue;
		}

		// Sections that contain more of the words come first.
		score *= double(matchedWords) / queryWords.size();

		Hit hit;
		hit.sectionId = section.id;
		hit.title = section.title;
		hit.source = section.source;
		hit.score = score;
		hit.snippet = CreateSnippet(section.text, queryWords);
		retVal.append(hit);
	}

	std::stable_sort(retVal.begin(), retVal.end(), [](const Hit& first, const Hit& second){
		return first.score > second.score;
	});

	return retVal.mid(0, maxHits);
}


bool CMarkdownKnowledgeBase::ReadSection(const QString& sectionId, QString& title, QString& source, QString& text) const
{
	QMap<QString, int>::const_iterator iter = m_sectionIndexes.constFind(sectionId);
	if (iter == m_sectionIndexes.constEnd()){
		return false;
	}

	const Section& section = m_sections.at(iter.value());
	title = section.title;
	source = section.source;
	text = section.text;

	return true;
}


int CMarkdownKnowledgeBase::GetSectionsCount() const
{
	return m_sections.size();
}


QStringList CMarkdownKnowledgeBase::SplitWords(const QString& text)
{
	QStringList retVal;
	QString word;

	const QString lowerText = text.toLower();
	for (const QChar& character: lowerText){
		if (character.isLetterOrNumber()){
			word.append(character);
		}
		else{
			if (word.size() >= 2){
				retVal.append(word);
			}

			word.clear();
		}
	}

	if (word.size() >= 2){
		retVal.append(word);
	}

	return retVal;
}


// private methods

void CMarkdownKnowledgeBase::AddSection(const QString& source, const QString& title, const QString& text)
{
	const QString trimmedText = text.trimmed();
	if (trimmedText.isEmpty()){
		return;
	}

	// Long sections are split at paragraphs into parts of limited size.
	QStringList parts;
	QString part;
	const QStringList paragraphs = trimmedText.split(QStringLiteral("\n\n"));
	for (const QString& paragraph: paragraphs){
		if (!part.isEmpty() && ((part.size() + paragraph.size() + 2) > MAX_SECTION_LENGTH)){
			parts.append(part);
			part.clear();
		}

		QString rest = paragraph;
		while (rest.size() > MAX_SECTION_LENGTH){
			parts.append(rest.left(MAX_SECTION_LENGTH));
			rest = rest.mid(MAX_SECTION_LENGTH);
		}

		part += part.isEmpty() ? rest : QStringLiteral("\n\n") + rest;
	}

	if (!part.isEmpty()){
		parts.append(part);
	}

	for (int i = 0; i < parts.size(); ++i){
		const int number = ++m_sectionsPerSource[source];

		Section section;
		section.id = QStringLiteral("%1#%2").arg(source).arg(number);
		section.title = (parts.size() > 1) ? QStringLiteral("%1 (%2/%3)").arg(title).arg(i + 1).arg(parts.size()) : title;
		section.source = source;
		section.text = parts.at(i);
		section.titleWords = SplitWords(title);
		section.textWords = SplitWords(section.text);

		m_sectionIndexes.insert(section.id, m_sections.size());
		m_sections.append(section);
	}
}


QString CMarkdownKnowledgeBase::CreateSnippet(const QString& text, const QStringList& queryWords)
{
	const QString lowerText = text.toLower();
	int position = -1;
	for (const QString& word: queryWords){
		const int wordPosition = lowerText.indexOf(word);
		if ((wordPosition >= 0) && ((position < 0) || (wordPosition < position))){
			position = wordPosition;
		}
	}

	const int start = std::max(0, position - SNIPPET_LENGTH / 3);
	QString snippet = text.mid(start, SNIPPET_LENGTH).simplified();
	if (start > 0){
		snippet.prepend(QStringLiteral("..."));
	}

	if ((start + SNIPPET_LENGTH) < text.size()){
		snippet.append(QStringLiteral("..."));
	}

	return snippet;
}


} // namespace imtassist


