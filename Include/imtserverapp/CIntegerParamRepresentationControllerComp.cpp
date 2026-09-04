// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CIntegerParamRepresentationControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


// ACF includes
#include <iprm/ISelectionParam.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerWrap<sdl::V1_0::imtbase::CIntegerParam>)

QByteArray CIntegerParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::V1_0::imtbase::CParamTypeIds::ParamTypeIdsFields::IntegerParam.toUtf8();
}


bool CIntegerParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::ISelectionParam* selectionParamPtr = dynamic_cast<const iprm::ISelectionParam*>(&dataModel);

	return selectionParamPtr != nullptr;
}


bool CIntegerParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::V1_0::imtbase::CIntegerParam& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const iprm::ISelectionParam* selectionParamPtr = dynamic_cast<const iprm::ISelectionParam*>(&dataModel);
	Q_ASSERT(selectionParamPtr != nullptr);
	if (selectionParamPtr == nullptr){
		return false;
	}

	sdlRepresentation.value = selectionParamPtr->GetSelectedOptionIndex();

	return true;
}


bool CIntegerParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::V1_0::imtbase::CIntegerParam& sdlRepresentation) const
{
	iprm::ISelectionParam* selectionParamPtr = dynamic_cast<iprm::ISelectionParam*>(&dataModel);
	Q_ASSERT(selectionParamPtr != nullptr);
	if (selectionParamPtr == nullptr){
		return false;
	}

	if (!sdlRepresentation.value){
		return false;
	}

	selectionParamPtr->SetSelectedOptionIndex(*sdlRepresentation.value);

	return true;
}


} // namespace imtserverapp


