// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QByteArray>


namespace istd
{


class IChangeable;


}


namespace imtbase
{


class IObjectCollection;
class IOperationContext;


}


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Delegate interface for writing a saved document over an existing collection object.

	Implementations are registered with \c CDocumentServiceCompBase via the
	\c "DocumentSaveDelegates" multi-reference slot (one delegate per document
	type-ID, matched by position with \c "ObjectTypeIdList").

	The service calls \c UpdateCollectionObject during \c DoSaveDocument instead of
	\c IObjectCollection::SetObjectData when the document updates the object it was
	opened from.  This allows the delegate to keep server-owned data of the stored
	object that the opened document may hold in an outdated state.

	\note Implementations must be thread-safe.
*/
class IDocumentSaveDelegate : virtual public istd::IPolymorphic
{
public:
	/**
		Write the document data over the collection object.
		\param collection			collection containing the object.
		\param objectId				ID of the object to be updated.
		\param document				snapshot of the document data; the delegate may change it before it is written.
		\param operationContextPtr	optional operation context providing information about who initiates the operation.
		\return true if the object was updated, false otherwise.
	*/
	virtual bool UpdateCollectionObject(
				imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				istd::IChangeable& document,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
};


} // namespace imtdoc


