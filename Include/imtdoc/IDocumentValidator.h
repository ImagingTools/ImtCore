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
