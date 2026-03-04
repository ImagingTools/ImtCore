// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CTextParamSummaryRepresentationControllerComp.h>


// ACF includes
#include <iprm/ITextParam.h>


namespace imtbase
{


// public methods

// reimplemented (ISummaryRepresentationController)

bool CTextParamSummaryRepresentationControllerComp::CreateSummaryRepresentation(const istd::IChangeable& param, QString& textRepresentation) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&param);
	if (textParamPtr != nullptr){
		textRepresentation = textParamPtr->GetText();

		return true;
	}

	return false;
}


} // namespace imtbase


