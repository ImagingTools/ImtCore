// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTextDocumentValidatorComp.h"


// ACF includes
#include <iprm/ITextParam.h>


namespace CollectionDocumentServiceIntegrationTestPck
{


// reimplemented (imtdoc::IDocumentValidator)

bool CTextDocumentValidatorComp::ValidateDocumentData(
			const QUrl& /*url*/,
			const istd::IChangeable& document,
			QString& errorMessage,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	errorMessage.clear();

	const iprm::ITextParam* textPtr = dynamic_cast<const iprm::ITextParam*>(&document);
	if ((textPtr != nullptr) && (textPtr->GetText() != *m_invalidTextAttrPtr)){
		return true;
	}

	if (m_errorMessageAttrPtr.IsValid()){
		errorMessage = *m_errorMessageAttrPtr;
	}

	return false;
}


} // namespace CollectionDocumentServiceIntegrationTestPck
