// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdoc/IDocumentValidator.h>


namespace DocumentServiceIntegrationTestPck
{


/**
	Test validator for \c iprm::ITextParam documents.
	Validation fails if the document is not a text parameter or its text is equal to the configured invalid text.
*/
class CTextDocumentValidatorComp:
			public icomp::CComponentBase,
			virtual public imtdoc::IDocumentValidator
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTextDocumentValidatorComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator);
		I_ASSIGN(m_invalidTextAttrPtr, "InvalidText", "Document text treated as invalid", true, "invalid");
		I_ASSIGN(m_errorMessageAttrPtr, "ErrorMessage", "Error message returned if validation fails", false, "");
	I_END_COMPONENT

	// reimplemented (imtdoc::IDocumentValidator)
	virtual bool ValidateDocumentData(
				const QUrl& url,
				const istd::IChangeable& document,
				QString& errorMessage,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

private:
	I_ATTR(QString, m_invalidTextAttrPtr);
	I_ATTR(QString, m_errorMessageAttrPtr);
};


} // namespace DocumentServiceIntegrationTestPck
