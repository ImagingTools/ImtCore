// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTextDocumentValidatorComp.h"


// ACF includes
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtdoc/imtdoc.h>


namespace DocumentServiceIntegrationTestPck
{


// reimplemented (imtdoc::IDocumentValidator)

bool CTextDocumentValidatorComp::ValidateDocumentData(
			const istd::IChangeable& document,
			QString& errorMessage,
			const iprm::IParamsSet* validationParams,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	errorMessage.clear();

	const iprm::ITextParam* textPtr = dynamic_cast<const iprm::ITextParam*>(&document);
	if ((textPtr != nullptr) && (textPtr->GetText() != *m_invalidTextAttrPtr)){
		return true;
	}

	if (m_errorMessageAttrPtr.IsValid()){
		errorMessage = *m_errorMessageAttrPtr;

		// With a language the message shows what reached the validator: "[<language>|<object-ID>]"
		const QByteArray languageId = imtdoc::GetLanguageIdFromParams(validationParams);
		if (!languageId.isEmpty()){
			errorMessage += QStringLiteral(" [%1|%2]").arg(
						QString::fromUtf8(languageId),
						QString::fromUtf8(imtdoc::GetObjectIdFromParams(validationParams)));
		}
	}

	return false;
}


} // namespace DocumentServiceIntegrationTestPck
