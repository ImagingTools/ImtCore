#include <imtserverapp/CParamSetRepresentationControllerComp.h>


// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtserverapp
{


// protected methods

const imtserverapp::IJsonRepresentationController* CParamSetRepresentationControllerComp::FindSubController(const QByteArray& modelId) const
{
	for (int i = 0; i < m_paramRepresentationControllersCompPtr.GetCount(); i++){
		const IJsonRepresentationController* subControllerPtr = m_paramRepresentationControllersCompPtr[i];
		if (subControllerPtr != nullptr){
			RepresentationInfo representationInfo = subControllerPtr->GetRepresentationInfo();
			if (representationInfo.modelId == modelId){
				return subControllerPtr;
			}
		}
	}

	return nullptr;
}


// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CParamsSet>)

QByteArray CParamSetRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::ParamsSet.toUtf8();
}


bool CParamSetRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);
	if (paramsSetPtr != nullptr){
		return true;
	}

	return false;
}


bool CParamSetRepresentationControllerComp::GetSdlRepresentationFromDataModel(
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

	QList<sdl::imtbase::ImtBaseTypes::CParameter::V1_0> parameterList;

	for (const QByteArray& parameterId : parameterIds){
		if (!parameterId.contains("/")){
			const iser::ISerializable* parameterPtr = paramsSetPtr->GetParameter(parameterId);
			if (parameterPtr == nullptr){
				continue;
			}

			const IJsonRepresentationController* subControllerPtr = FindSubController(parameterId);
			if (subControllerPtr != nullptr){
				QJsonObject parameterRepresentation;
				if (subControllerPtr->GetRepresentationFromDataModel(*parameterPtr, parameterRepresentation, paramsPtr)){
					sdl::imtbase::ImtBaseTypes::CParameter::V1_0 parameter;
					QJsonDocument jsonDocument(parameterRepresentation);
					
					parameter.data = jsonDocument.toJson(QJsonDocument::Compact);
					
					IJsonRepresentationController::RepresentationInfo representationInfo = subControllerPtr->GetRepresentationInfo();
					QByteArray typeId = subControllerPtr->GetTypeId();
					
					parameter.id = representationInfo.modelId;
					parameter.typeId = typeId;
					
					QString name = representationInfo.name;
					QString description = representationInfo.description;
					
					if (m_translationManagerCompPtr.IsValid()){
						name = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), name.toUtf8(), languageId, "Attribute");
						description = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), description.toUtf8(), languageId, "Attribute");
					}
					
					parameter.name = name;
					parameter.description = description;
					
					parameterList << parameter;
				}
			}
		}
	}

	sdlRepresentation.parameters = parameterList;

	return true;
}


bool CParamSetRepresentationControllerComp::GetDataModelFromSdlRepresentation(
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

	QList<sdl::imtbase::ImtBaseTypes::CParameter::V1_0> parameters = *sdlRepresentation.parameters;

	for (const sdl::imtbase::ImtBaseTypes::CParameter::V1_0& parameter : parameters){
		if (!parameter.id){
			continue;
		}

		const QByteArray parameterId = *parameter.id;
		iser::ISerializable* parameterPtr = paramsSetPtr->GetEditableParameter(parameterId);
		if (parameterPtr == nullptr){
			continue;
		}

		QString parameterData;
		if (parameter.data){
			parameterData = *parameter.data;
		}

		QJsonDocument document = QJsonDocument::fromJson(parameterData.toUtf8());
		if (document.isNull()){
			return false;
		}

		if (!document.isObject()){
			return false;
		}

		const IJsonRepresentationController* subControllerPtr = FindSubController(parameterId);
		if (subControllerPtr == nullptr){
			return false;
		}

		if (!subControllerPtr->GetDataModelFromRepresentation(document.object(), *parameterPtr)){
			return false;
		}
	}

	return true;
}


} // namespace imtserverapp


