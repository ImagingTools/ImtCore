// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QByteArray>


class QString;


namespace istd
{


class IChangeable;


}


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Strategy interface for validating document data before it is saved.

	Implementations are registered with \c CDocumentServiceCompBase via the
	\c "DocumentValidators" multi-reference slot (one validator per document
	type-ID, matched by position with \c "ObjectTypeIdList").

	The service calls \c ValidateDocumentData during \c DoSaveDocument before
	any write is attempted.  A failed validation aborts the save and returns
	\c IDocumentService::OS_INVALID_DOCUMENT_DATA to the caller together with
	the error message produced by the validator.
*/
class IDocumentValidator : virtual public istd::IPolymorphic
{
public:
	/**
		Validate document data.
		\return		true if document is valid, false otherwise.
		\param		objectId		ID of the object being validated (empty for new documents not yet saved).
		\param		document		document data to validate.
		\param		errorMessage	output parameter with validation error description when return value is false.
								When validation succeeds, errorMessage is left unchanged.
		\note		Implementations must be thread-safe.
	*/
	virtual bool ValidateDocumentData(const QByteArray& objectId, const istd::IChangeable& document, QString& errorMessage) const = 0;
};


} // namespace imtdoc


