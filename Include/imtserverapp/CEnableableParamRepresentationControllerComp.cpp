// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CEnableableParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/IEnableableParam.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CEnableableParam::V1_0>)

QByteArray CEnableableParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::EnableableParam.toUtf8();
}


bool CEnableableParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable &dataModel) const
{
	const iprm::IEnableableParam* enableableParamPtr = dynamic_cast<const iprm::IEnableableParam*>(&dataModel);

	return enableableParamPtr != nullptr;
}


bool CEnableableParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CEnableableParam::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const iprm::IEnableableParam* enableableParamPtr = dynamic_cast<const iprm::IEnableableParam*>(&dataModel);
	Q_ASSERT(enableableParamPtr != nullptr);
	if (enableableParamPtr == nullptr){
		return false;
	}

	bool isEnabled = enableableParamPtr->IsEnabled();
	sdlRepresentation.value = isEnabled;

	return true;
}


bool CEnableableParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CEnableableParam::V1_0& sdlRepresentation) const
{
	iprm::IEnableableParam* enableableParamPtr = dynamic_cast<iprm::IEnableableParam*>(&dataModel);
	Q_ASSERT(enableableParamPtr != nullptr);
	if (enableableParamPtr == nullptr){
		return false;
	}

	if (!sdlRepresentation.value.has_value()){
		return false;
	}

	enableableParamPtr->SetEnabled(*sdlRepresentation.value);

	return true;
}


} // namespace imtserverapp


