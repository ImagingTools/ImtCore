// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CParamSetRepresentationController.h>


// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CParamsSet>)

QByteArray CParamSetRepresentationController::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::ParamsSet.toUtf8();
}


bool CParamSetRepresentationController::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);

	return paramsSetPtr != nullptr;
}


bool CParamSetRepresentationController::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* paramsPtr) const
{
	const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);
	Q_ASSERT(paramsSetPtr != nullptr);
	if (paramsSetPtr == nullptr){
		return false;
	}

	QByteArray languageId;
	if (paramsPtr != nullptr){
		iprm::TParamsPtr<iprm::IIdParam> languageParamPtr(paramsPtr, "LanguageParam");
		if (languageParamPtr.IsValid()){
			languageId = languageParamPtr->GetId();
		}
	}

	iprm::IParamsSet::Ids paramSetIds = paramsSetPtr->GetParamIds();
	QByteArrayList parameterIds = paramSetIds.values();
	std::sort(parameterIds.begin(), parameterIds.end());

	imtsdl::TElementList<sdl::imtbase::ImtBaseTypes::CParameter::V1_0> parameterList;

	for (const QByteArray& parameterId : parameterIds){
		if (!parameterId.contains("/")){
			const iser::ISerializable* parameterPtr = paramsSetPtr->GetParameter(parameterId);
			if (parameterPtr == nullptr){
				continue;
			}

			if (!m_representationControllersMap.contains(parameterId)){
				continue;
			}

			const IJsonRepresentationController* subControllerPtr = m_representationControllersMap[parameterId];
			if (subControllerPtr == nullptr){
				return false;
			}

			QJsonObject parameterRepresentation;
			if (!subControllerPtr->GetRepresentationFromDataModel(*parameterPtr, parameterRepresentation, paramsPtr)){
				return false;
			}

			sdl::imtbase::ImtBaseTypes::CParameter::V1_0 parameter;
			QJsonDocument jsonDocument(parameterRepresentation);

			parameter.data = jsonDocument.toJson(QJsonDocument::Compact);

			IJsonRepresentationController::RepresentationInfo representationInfo = subControllerPtr->GetRepresentationInfo();
			QByteArray typeId = subControllerPtr->GetTypeId();

			parameter.id = representationInfo.modelId;
			parameter.typeId = typeId;

			QString name = representationInfo.name;
			QString description = representationInfo.description;

			if (m_translationManagerPtr != nullptr){
				name = iqt::GetTranslation(m_translationManagerPtr, name.toUtf8(), languageId, "Attribute");
				description = iqt::GetTranslation(m_translationManagerPtr, description.toUtf8(), languageId, "Attribute");
			}

			parameter.name = name;
			parameter.description = description;

			parameterList << parameter;
		}
	}

	sdlRepresentation.parameters = parameterList;

	return true;
}


bool CParamSetRepresentationController::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdlRepresentation) const
{
	iprm::IParamsSet* paramsSetPtr = dynamic_cast<iprm::IParamsSet*>(&dataModel);
	Q_ASSERT(paramsSetPtr != nullptr);
	if (paramsSetPtr == nullptr){
		return false;
	}

	if (!sdlRepresentation.parameters){
		return false;
	}

	imtsdl::TElementList<sdl::imtbase::ImtBaseTypes::CParameter::V1_0> parameters = *sdlRepresentation.parameters;

	for (const istd::TSharedNullable<sdl::imtbase::ImtBaseTypes::CParameter::V1_0>& parameter : parameters){
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

		if (!m_representationControllersMap.contains(parameterId)){
			return false;
		}

		const IJsonRepresentationController* subControllerPtr = m_representationControllersMap[parameterId];
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

bool CParamSetRepresentationController::RegisterSubController(const imtserverapp::IJsonRepresentationController& controller) const
{
	imtserverapp::IJsonRepresentationController::RepresentationInfo representationInfo = controller.GetRepresentationInfo();
	if (m_representationControllersMap.contains(representationInfo.modelId)){
		return false;
	}

	m_representationControllersMap[representationInfo.modelId] = &controller;

	return true;
}


} // namespace imtserverapp


