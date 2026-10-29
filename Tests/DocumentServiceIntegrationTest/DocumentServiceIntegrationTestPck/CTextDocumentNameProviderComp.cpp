// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTextDocumentNameProviderComp.h"


// ACF includes
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtdoc/imtdoc.h>


namespace DocumentServiceIntegrationTestPck
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CTextDocumentNameProviderComp::GetDefaultDocumentName(const istd::IChangeable& document, const iprm::IParamsSet* paramsPtr) const
{
	const iprm::ITextParam* textPtr = dynamic_cast<const iprm::ITextParam*>(&document);
	if ((textPtr == nullptr) || textPtr->GetText().isEmpty()){
		return QString();
	}

	QString retVal = textPtr->GetText();
	if (m_namePrefixAttrPtr.IsValid()){
		retVal.prepend(*m_namePrefixAttrPtr);
	}

	if (!*m_appendSourceAttrPtr){
		return retVal;
	}

	QString source = imtdoc::GetObjectIdFromParams(paramsPtr);
	const int revision = imtdoc::GetRevisionFromParams(paramsPtr);
	if (revision >= 0){
		source += '/' + QString::number(revision);
	}

	return retVal + QStringLiteral(" [%1]").arg(source);
}


} // namespace DocumentServiceIntegrationTestPck
