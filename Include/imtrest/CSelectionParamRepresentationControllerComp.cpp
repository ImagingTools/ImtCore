#include <imtrest/CSelectionParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <iqt/iqt.h>


namespace imtrest
{


// protected methods

// reimplemented (imtrest::CObjectRepresentationControllerCompBase)

bool CSelectionParamRepresentationControllerComp::GetRepresentationFromValue(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const iprm::ISelectionParam* selectionParamPtr = dynamic_cast<const iprm::ISelectionParam*>(&dataModel);
	Q_ASSERT(selectionParamPtr != nullptr);

	const iprm::IOptionsList* optionListPtr = selectionParamPtr->GetSelectionConstraints();
	if (optionListPtr != nullptr){
		imtbase::CTreeItemModel* parametersModelPtr = representation.AddTreeModel("Parameters");
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

	return true;
}


// reimplemented (IRepresentationController)

bool CSelectionParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::ISelectionParam* selectionParamPtr = dynamic_cast<const iprm::ISelectionParam*>(&dataModel);
	if (selectionParamPtr != nullptr){
		return true;
	}

	return false;
}


bool CSelectionParamRepresentationControllerComp::GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	iprm::ISelectionParam* selectionParamPtr = dynamic_cast<iprm::ISelectionParam*>(&dataModel);
	Q_ASSERT(selectionParamPtr != nullptr);

	int selectedIndex = -1;
	if (representation.ContainsKey("Value")){
		selectedIndex = representation.GetData("Value").toInt();
	}

	if (selectedIndex >= 0){
		selectionParamPtr->SetSelectedOptionIndex(selectedIndex);

		return true;
	}

	return false;
}


} // namespace imtrest


