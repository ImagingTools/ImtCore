// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


class QString;


namespace istd
{


class IChangeable;


}


namespace iprm
{


class IParamsSet;


}


namespace imtbase
{


class IOperationContext;


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
		\param		document		document data to validate.
		\param		errorMessage	output parameter with validation error description when return value is false.
								When validation succeeds, errorMessage is left unchanged.
		\param		paramsPtr		optional parameters of the document source, provided by the document service.
								Can be null if the document source is not described (e.g.\ new documents not yet saved).
								For collection-based documents use \c imtdoc::GetObjectIdFromParams to get the collection object ID
								and \c imtdoc::GetRevisionFromParams to get the object revision.
		\param		operationContextPtr		optional operation context providing information about who initiates the operation.
		\note		Implementations must be thread-safe.
	*/
	virtual bool ValidateDocumentData(
		const istd::IChangeable& document,
		QString& errorMessage,
		const iprm::IParamsSet* paramsPtr = nullptr,
		const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
};


} // namespace imtdoc


