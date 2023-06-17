#include <imtbase/CSelectionParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CSelectionParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::ISelectionParam* selectionParamPtr = dynamic_cast<const iprm::ISelectionParam*>(&dataModel);
	if (selectionParamPtr != nullptr) {
		return true;
	}

	return false;
}


bool CSelectionParamRepresentationControllerComp::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			CTreeItemModel& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const iprm::ISelectionParam* selectionParamPtr = dynamic_cast<const iprm::ISelectionParam*>(&dataModel);
	if (selectionParamPtr != nullptr){
		if (m_paramIdAttrPtr.IsValid()){
			representation.SetData("Id", *m_paramIdAttrPtr);
		}

		if (m_paramNameAttrPtr.IsValid()){
			representation.SetData("Name", *m_paramNameAttrPtr);
		}

		if (m_qmlPathAttrPtr.IsValid()){
			representation.SetData("Source", *m_qmlPathAttrPtr);
		}

		const iprm::IOptionsList* optionListPtr = selectionParamPtr->GetSelectionConstraints();
		if (optionListPtr != nullptr){
			CTreeItemModel* parametersModelPtr = representation.AddTreeModel("Parameters");
			Q_ASSERT(parametersModelPtr != nullptr);

			for (int i = 0; i < optionListPtr->GetOptionsCount(); ++i){
				QByteArray optionId = optionListPtr->GetOptionId(i);
				QString optionName = optionListPtr->GetOptionName(i);
				QString optionDescription = optionListPtr->GetOptionDescription(i);

				int index = parametersModelPtr->InsertNewItem();

				parametersModelPtr->SetData("Id", optionId, index);
				parametersModelPtr->SetData("Name", optionName, index);
				parametersModelPtr->SetData("Description", optionDescription, index);
			}
		}

		int selectedIndex = selectionParamPtr->GetSelectedOptionIndex();
		representation.SetData("Value", selectedIndex);
	}

	return true;
}


bool CSelectionParamRepresentationControllerComp::GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	iprm::ISelectionParam* selectionParamPtr = dynamic_cast<iprm::ISelectionParam*>(&dataModel);
	if (selectionParamPtr != nullptr){
		int selectedIndex = -1;
		if (representation.ContainsKey("Value")){
			selectedIndex = representation.GetData("Value").toInt();
		}

		if (selectedIndex >= 0){
			selectionParamPtr->SetSelectedOptionIndex(selectedIndex);

			return true;
		}
	}

	return false;
}


} // namespace imtbase


