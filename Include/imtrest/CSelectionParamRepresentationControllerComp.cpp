#include <imtrest/CSelectionParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>


namespace imtrest
{


// protected methods

// reimplemented (imtrest::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0>)

QByteArray CSelectionParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::SelectionParam.toUtf8();
}


bool CSelectionParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::ISelectionParam* selectionParamPtr = dynamic_cast<const iprm::ISelectionParam*>(&dataModel);
	if (selectionParamPtr != nullptr){
		return true;
	}
	
	return false;
}


bool CSelectionParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const iprm::ISelectionParam* selectionParamPtr = dynamic_cast<const iprm::ISelectionParam*>(&dataModel);
	Q_ASSERT(selectionParamPtr != nullptr);
	if (selectionParamPtr == nullptr){
		return false;
	}
	
	QList<sdl::imtbase::ImtBaseTypes::COption::V1_0> optionList;
	
	const iprm::IOptionsList* optionListPtr = selectionParamPtr->GetSelectionConstraints();
	if (optionListPtr != nullptr){
		for (int i = 0; i < optionListPtr->GetOptionsCount(); ++i){
			sdl::imtbase::ImtBaseTypes::COption::V1_0 option;
			option.id = optionListPtr->GetOptionId(i);
			option.name = optionListPtr->GetOptionName(i);
			option.description = optionListPtr->GetOptionDescription(i);
			option.enabled = optionListPtr->IsOptionEnabled(i);
			
			optionList << option;
		}
	}
	
	sdl::imtbase::ImtBaseTypes::COptionsList::V1_0 optionsList;
	optionsList.options = optionList;
	
	int selectedIndex = selectionParamPtr->GetSelectedOptionIndex();
	sdlRepresentation.selectedIndex = selectedIndex;
	sdlRepresentation.constraints = optionsList;
	
	return true;
}


bool CSelectionParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CSelectionParam::V1_0& sdlRepresentation) const
{
	iprm::ISelectionParam* selectionParamPtr = dynamic_cast<iprm::ISelectionParam*>(&dataModel);
	Q_ASSERT(selectionParamPtr != nullptr);
	if (selectionParamPtr == nullptr){
		return false;
	}
	
	if (!sdlRepresentation.selectedIndex){
		return false;
	}
	
	int selectedIndex = *sdlRepresentation.selectedIndex;
	selectionParamPtr->SetSelectedOptionIndex(selectedIndex);
	
	return true;
}


} // namespace imtrest


