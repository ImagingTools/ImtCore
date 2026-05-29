// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CSelectionParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::V1_0::imtbase::CSelectionParam>)

QByteArray CSelectionParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::V1_0::imtbase::CParamTypeIds::ParamTypeIdsFields::SelectionParam.toUtf8();
}


bool CSelectionParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::ISelectionParam* selectionParamPtr = dynamic_cast<const iprm::ISelectionParam*>(&dataModel);

	return selectionParamPtr != nullptr;
}


bool CSelectionParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::V1_0::imtbase::CSelectionParam& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const iprm::ISelectionParam* selectionParamPtr = dynamic_cast<const iprm::ISelectionParam*>(&dataModel);
	Q_ASSERT(selectionParamPtr != nullptr);
	if (selectionParamPtr == nullptr){
		return false;
	}

	imtsdl::TElementList<sdl::V1_0::imtbase::COption> optionList;

	const iprm::IOptionsList* optionListPtr = selectionParamPtr->GetSelectionConstraints();
	if (optionListPtr != nullptr){
		for (int i = 0; i < optionListPtr->GetOptionsCount(); ++i){
			sdl::V1_0::imtbase::COption option;
			option.id = optionListPtr->GetOptionId(i);
			option.name = optionListPtr->GetOptionName(i);
			option.description = optionListPtr->GetOptionDescription(i);
			option.enabled = optionListPtr->IsOptionEnabled(i);

			optionList << option;
		}
	}

	sdl::V1_0::imtbase::COptionsList optionsList;
	optionsList.options = optionList;

	int selectedIndex = selectionParamPtr->GetSelectedOptionIndex();
	sdlRepresentation.selectedIndex = selectedIndex;
	sdlRepresentation.constraints = optionsList;

	return true;
}


bool CSelectionParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::V1_0::imtbase::CSelectionParam& sdlRepresentation) const
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


} // namespace imtserverapp


