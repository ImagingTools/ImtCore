// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtassist
{


/**
	Knowledge about the product for the assistant: concepts, rules and the meaning of the
	API, e.g. from the documentation of the product. The model searches it and reads sections
	of it. The content is data for the model, never instructions.

	\ingroup imtassist
*/
class IAssistantKnowledgeBase: virtual public istd::IPolymorphic
{
public:
	struct Hit
	{
		/**
			ID of the section, used to read it.
		*/
		QString sectionId;
		QString title;
		QString source;
		QString snippet;
		double score = 0;
	};

	typedef QList<Hit> Hits;

	/**
		Search the sections by words of the text, best first.
	*/
	virtual Hits Search(const QString& text, int maxHits) const = 0;

	/**
		Read a section.
		\return False if the section does not exist.
	*/
	virtual bool ReadSection(const QString& sectionId, QString& title, QString& source, QString& text) const = 0;

	/**
		Number of sections.
	*/
	virtual int GetSectionsCount() const = 0;
};


} // namespace imtassist


