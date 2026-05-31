// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>


namespace istd
{
class IChangeable;
}


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Strategy interface for computing a default document name from the document's content.

	Implementations are registered with \c CDocumentServiceCompBase via the
	\c "DocumentNameProviders" multi-reference slot (one provider per document
	type-ID, matched by position with \c "ObjectTypeIdList").

	When a document is saved or created, the service calls
	\c GetDefaultDocumentName so that the collection element can receive a
	meaningful name derived from the document data itself (e.g.\ a patient
	name stored inside the document).
*/
class IDocumentNameProvider: virtual public istd::IPolymorphic
{
public:
	/**
		\brief Compute the default display name for a document.

		\param objectId  Persistent ID of the collection element (empty for
		                 documents not yet saved to the collection).
		\param document  Current in-memory state of the document.
		\return          The name to use, or an empty string if no name can be
		                 derived.
	*/
	virtual QString GetDefaultDocumentName(const QByteArray& objectId, const istd::IChangeable& document) const = 0;
};


} // namespace imtdoc


