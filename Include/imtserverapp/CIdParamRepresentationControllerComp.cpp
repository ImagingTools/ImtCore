// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CIdParamRepresentationControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


// ACF includes
#include <iprm/IIdParam.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::V1_0::imtbase::CParamsSet>)

QByteArray CIdParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::V1_0::imtbase::CParamTypeIds::ParamTypeIdsFields::IdParam.toUtf8();
}


bool CIdParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::IIdParam* idParamPtr = dynamic_cast<const iprm::IIdParam*>(&dataModel);

	return idParamPtr != nullptr;
}


bool CIdParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
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


bool CIdParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
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


