// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QVector>

// ImtCore includes
#include <imtassist/IAssistantKnowledgeBase.h>


namespace imtassist
{


/**
	Knowledge base from Markdown documents. A document is split into sections at its
	headings; a long section is split into parts. The search ranks sections by the words of
	the query in their title and text (case-insensitive, any language).

	\ingroup imtassist
*/
class CMarkdownKnowledgeBase: virtual public IAssistantKnowledgeBase
{
public:
	enum
	{
		MAX_SECTION_LENGTH = 6000,
		SNIPPET_LENGTH = 240
	};

	/**
		Add a document.
		\param source	Name of the document shown to the model, e.g. the file name.
		\return Number of sections added.
	*/
	int AddDocument(const QString& source, const QString& markdown);
	void Clear();

	// reimplemented (imtassist::IAssistantKnowledgeBase)
	virtual Hits Search(const QString& text, int maxHits) const override;
	virtual bool ReadSection(const QString& sectionId, QString& title, QString& source, QString& text) const override;
	virtual int GetSectionsCount() const override;

	/**
		Split a text into lower-case words of at least two letters or digits.
	*/
	static QStringList SplitWords(const QString& text);

private:
	struct Section
	{
		QString id;
		QString title;
		QString source;
		QString text;
		QStringList titleWords;
		QStringList textWords;
	};

	void AddSection(const QString& source, const QString& title, const QString& text);
	static QString CreateSnippet(const QString& text, const QStringList& queryWords);

	QVector<Section> m_sections;
	QMap<QString, int> m_sectionIndexes;
	QMap<QString, int> m_sectionsPerSource;
};


} // namespace imtassist


