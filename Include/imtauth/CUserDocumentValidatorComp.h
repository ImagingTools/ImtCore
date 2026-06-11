// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdoc/IDocumentValidator.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	Validates User documents before save.
	Ensures username is specified and unique (case-insensitive),
	and that the user name is not empty.
*/
class CUserDocumentValidatorComp:
			public icomp::CComponentBase,
			virtual public imtdoc::IDocumentValidator
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserDocumentValidatorComp)
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection used for uniqueness checks", true, "UserCollection");
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator)
	I_END_COMPONENT

	// imtdoc::IDocumentValidator
	virtual bool ValidateDocumentData(const QByteArray& objectId, const istd::IChangeable& document, QString& errorMessage, const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

private:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauth
