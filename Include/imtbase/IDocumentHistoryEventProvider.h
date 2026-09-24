// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QList>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


/**
	Source of document history entries that are not revisions of the document,
	e.g. tags being added or removed. The entries are shown between the revisions
	in the document history.
	\ingroup Collection
*/
class IDocumentHistoryEventProvider: virtual public istd::IPolymorphic
{
public:
	struct HistoryEvent
	{
		QDateTime timestamp;
		QString user;
		QString description;

		/**
			Revision of the document at the moment of the event, or -1 if unknown.
		*/
		qlonglong revision = -1;

		/**
			Kind of the entry shown to the client, e.g. "TagEvent".
		*/
		QByteArray kind;
	};

	typedef QList<HistoryEvent> HistoryEvents;

	virtual HistoryEvents GetHistoryEvents(
				const QByteArray& collectionId,
				const QByteArray& documentId,
				const QByteArray& languageId) const = 0;
};


} // namespace imtbase


