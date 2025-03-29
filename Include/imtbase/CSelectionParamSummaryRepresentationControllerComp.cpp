#include <imtbase/CSelectionParamSummaryRepresentationControllerComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>


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


