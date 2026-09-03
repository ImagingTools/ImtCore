// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CIdParamRepresentationController.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


// ACF includes
#include <iprm/IIdParam.h>


namespace imtserverapp
{


CIdParamRepresentationController::CIdParamRepresentationController(QByteArray modelId, QString name, QString description)
{
	m_modelId = modelId;
	m_name = name;
	m_description = description;
}


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerWrap<sdl::V1_0::imtbase::CParamsSet>)

QByteArray CIdParamRepresentationController::GetTypeId() const
{
	return sdl::V1_0::imtbase::CParamTypeIds::ParamTypeIdsFields::IdParam.toUtf8();
}


bool CIdParamRepresentationController::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::IIdParam* idParamPtr = dynamic_cast<const iprm::IIdParam*>(&dataModel);

	return idParamPtr != nullptr;
}


bool CIdParamRepresentationController::GetSdlRepresentationFromDataModel(
			sdl::V1_0::imtbase::CIdParam& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const iprm::IIdParam* idParamPtr = dynamic_cast<const iprm::IIdParam*>(&dataModel);
	if (idParamPtr == nullptr){
		return false;
	}

	sdlRepresentation.id = idParamPtr->GetId();

	return true;
}


bool CIdParamRepresentationController::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::V1_0::imtbase::CIdParam& sdlRepresentation) const
{
	iprm::IIdParam* idParamPtr = dynamic_cast<iprm::IIdParam*>(&dataModel);
	if (idParamPtr == nullptr){
		return false;
	}

	if (sdlRepresentation.id){
		idParamPtr->SetId(*sdlRepresentation.id);
		return true;
	}

	return false;
}


} // namespace imtserverapp


