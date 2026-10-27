// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTextDocumentNameProviderComp.h"


// ACF includes
#include <iprm/ITextParam.h>


namespace CollectionDocumentServiceIntegrationTestPck
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CTextDocumentNameProviderComp::GetDefaultDocumentName(const QByteArray& /*objectId*/, const istd::IChangeable& document) const
{
	const iprm::ITextParam* textPtr = dynamic_cast<const iprm::ITextParam*>(&document);
	if ((textPtr == nullptr) || textPtr->GetText().isEmpty()){
		return QString();
	}

	if (!m_namePrefixAttrPtr.IsValid()){
		return textPtr->GetText();
	}

	return *m_namePrefixAttrPtr + textPtr->GetText();
}


} // namespace CollectionDocumentServiceIntegrationTestPck
