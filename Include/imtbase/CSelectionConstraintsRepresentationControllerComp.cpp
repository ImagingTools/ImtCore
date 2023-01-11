#include <imtbase/CSelectionConstraintsRepresentationControllerComp.h>


// ACF includes
#include <iprm/IOptionsList.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CSelectionConstraintsRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::IOptionsList* optionListPtr = dynamic_cast<const iprm::IOptionsList*>(&dataModel);
	if (optionListPtr != nullptr) {
		return true;
	}

	return false;
}


bool CSelectionConstraintsRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const iprm::IOptionsList* optionListPtr = dynamic_cast<const iprm::IOptionsList*>(&dataModel);
	if (optionListPtr != nullptr){
		for (int i = 0; i < optionListPtr->GetOptionsCount(); i++){
			QByteArray optionId = optionListPtr->GetOptionId(i);
			QString optionName = optionListPtr->GetOptionName(i);
			QString optionDescription = optionListPtr->GetOptionDescription(i);

			int index = representation.InsertNewItem();

			representation.SetData("Id", optionId, index);
			representation.SetData("Name", optionName, index);
			representation.SetData("Description", optionDescription, index);
		}
	}

	return true;
}


bool CSelectionConstraintsRepresentationControllerComp::GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	iprm::IOptionsList* optionListPtr = dynamic_cast<iprm::IOptionsList*>(&dataModel);
	if (optionListPtr != nullptr){

	}

	return false;
}


} // namespace imtbase


