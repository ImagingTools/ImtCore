// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CSelectionParamSummaryRepresentationControllerComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtbase
{


// protected methods

// reimplemented (imtbase::ISummaryRepresentationController)

bool CSelectionParamSummaryRepresentationControllerComp::CreateSummaryRepresentation(const istd::IChangeable& param, QString& textRepresentation) const
{

	const iprm::ISelectionParam* selectionParamPtr = dynamic_cast<const iprm::ISelectionParam*>(&param);
	Q_ASSERT(selectionParamPtr != nullptr);
	int selectedIndex = selectionParamPtr->GetSelectedOptionIndex();
	if (selectedIndex != -1){
		const iprm::IOptionsList* optionListPtr = selectionParamPtr->GetSelectionConstraints();
		if (optionListPtr != nullptr){
			QString optionName = optionListPtr->GetOptionName(selectedIndex);
			textRepresentation = optionName;
		}

		return true;
	}

	return false;
}


} // namespace imtbase


