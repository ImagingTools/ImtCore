// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


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
		\param		errorMessage	output parameter with validation error description when return value is false.
	*/
	virtual bool ValidateDocumentData(const istd::IChangeable& document, QString& errorMessage) const = 0;
};


} // namespace imtdoc
