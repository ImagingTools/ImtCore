// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/IChangeable.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Strategy interface for converting document changes to notification change IDs.

	Implementations map a document-specific \c ChangeSet and current document data
	to a list of stable IDs that are used by clients to refresh only affected parts
	of their representations.
*/
class IDocumentChangeNotificationConverter: virtual public istd::IPolymorphic
{
public:
	/**
		\brief Convert a document change into notification change IDs.
		\param changeSet Change mask received from the document model update.
		\param document  Current in-memory document object.
		\return          Document-controller-specific change IDs for notification payloads.
	*/
	virtual QByteArrayList GetChangeNotificationIds(
				const istd::IChangeable::ChangeSet& changeSet,
				const istd::IChangeable& document) const = 0;
};


} // namespace imtdoc
