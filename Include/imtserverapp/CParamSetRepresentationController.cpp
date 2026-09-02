// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CParamSetRepresentationController.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/IParamsInfoProvider.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerWrap<sdl::V1_0::imtbase::CParamsSet>)

QByteArray CParamSetRepresentationController::GetTypeId() const
{
	return sdl::V1_0::imtbase::CParamTypeIds::ParamTypeIdsFields::ParamsSet.toUtf8();
}


bool CParamSetRepresentationController::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);

	return paramsSetPtr != nullptr;
}


bool CParamSetRepresentationController::GetSdlRepresentationFromDataModel(
			sdl::V1_0::imtbase::CParamsSet& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* paramsPtr) const
{
	const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);
	Q_ASSERT(paramsSetPtr != nullptr);
	if (paramsSetPtr == nullptr){
		return false;
	}

	iprm::IParamsSet::Ids paramSetIds = paramsSetPtr->GetParamIds();
	QByteArrayList parameterIds = paramSetIds.values();
	std::sort(parameterIds.begin(), parameterIds.end());

	imtsdl::TElementList<sdl::V1_0::imtbase::CParameter> parameterList;

	const iprm::IParamsInfoProvider* paramsInfoProviderPtr = paramsSetPtr->GetParamsInfoProvider();
	for (const QByteArray& parameterId : std::as_const(parameterIds)){
		if (parameterId.contains('/')){
			continue;
		}

		const iser::ISerializable* parameterPtr = paramsSetPtr->GetParameter(parameterId);
		if (parameterPtr == nullptr){
			continue;
		}

		const IJsonRepresentationController* subControllerPtr = GetRepresentationController(*parameterPtr);
		if (subControllerPtr == nullptr){
			continue;
		}

		QJsonObject parameterRepresentation;
		if (!subControllerPtr->GetRepresentationFromDataModel(*parameterPtr, parameterRepresentation, paramsPtr)){
			return false;
		}

		sdl::V1_0::imtbase::CParameter parameter;
		QJsonDocument jsonDocument(parameterRepresentation);

		parameter.data = jsonDocument.toJson(QJsonDocument::Compact);

		IJsonRepresentationController::RepresentationInfo representationInfo = subControllerPtr->GetRepresentationInfo();
		QByteArray typeId = subControllerPtr->GetTypeId();

		parameter.id = parameterId;
		parameter.typeId = typeId;

		if(paramsInfoProviderPtr){
			std::unique_ptr<iprm::IParamsInfoProvider::ParamInfo> paramInfoPtr = paramsInfoProviderPtr->GetParamInfo(parameterId);
			if(paramInfoPtr){
				parameter.name = paramInfoPtr->name;
				parameter.description = paramInfoPtr->description;
			}
		}

		parameterList << parameter;
	}

	sdlRepresentation.parameters = parameterList;

	return true;
}


bool CParamSetRepresentationController::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::V1_0::imtbase::CParamsSet& sdlRepresentation) const
{
	iprm::IParamsSet* paramsSetPtr = dynamic_cast<iprm::IParamsSet*>(&dataModel);
	Q_ASSERT(paramsSetPtr != nullptr);
	if (paramsSetPtr == nullptr){
		return false;
	}

	if (!sdlRepresentation.parameters){
		return false;
	}

	imtsdl::TElementList<sdl::V1_0::imtbase::CParameter> parameters = *sdlRepresentation.parameters;

	for (const istd::TNullableValue<sdl::V1_0::imtbase::CParameter>& parameter : parameters){
		if (!parameter->id){
			continue;
		}

		const QByteArray parameterId = *parameter->id;
		iser::ISerializable* parameterPtr = paramsSetPtr->GetEditableParameter(parameterId);
		if (parameterPtr == nullptr){
			continue;
		}

		QString parameterData;
		if (parameter->data){
			parameterData = *parameter->data;
		}

		QJsonDocument document = QJsonDocument::fromJson(parameterData.toUtf8());
		if (document.isNull()){
			return false;
		}

		if (!document.isObject()){
			return false;
		}

		const IJsonRepresentationController* subControllerPtr = GetRepresentationController(*parameterPtr);
		if (subControllerPtr == nullptr){
			return false;
		}

		if (!subControllerPtr->GetDataModelFromRepresentation(document.object(), *parameterPtr)){
			return false;
		}
	}

	return true;
}


// private methods

const IJsonRepresentationController* CParamSetRepresentationController::GetRepresentationController(
			const istd::IChangeable& dataModel) const
{
	for (const IJsonRepresentationController* controllerPtr : m_representationControllers){
		if (controllerPtr != nullptr && controllerPtr->IsModelSupported(dataModel)){
			return controllerPtr;
		}
	}

	return nullptr;
}

bool CParamSetRepresentationController::RegisterSubController(const imtserverapp::IJsonRepresentationController& controller) const
{
	if (m_representationControllers.contains(&controller)){
		return false;
	}

	m_representationControllers << &controller;

	return true;
}


} // namespace imtserverapp


