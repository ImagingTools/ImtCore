#include <imtserverapp/CParamSetRepresentationControllerComp.h>


// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <iqt/iqt.h>
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
	
	QByteArrayList paramTypeIds;
	QByteArrayList paramIds;
	QStringList paramNames;
	QStringList paramDescriptions;
	QStringList parameters;
	
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
					QJsonDocument jsonDocument(parameterRepresentation);
					parameters << jsonDocument.toJson(QJsonDocument::Compact);
					
					IJsonRepresentationController::RepresentationInfo representationInfo = subControllerPtr->GetRepresentationInfo();
					QByteArray typeId = subControllerPtr->GetTypeId();
					
					paramTypeIds << typeId;
					paramIds << representationInfo.modelId;
					
					QString name = representationInfo.name;
					QString description = representationInfo.description;
					
					if (m_translationManagerCompPtr.IsValid()){
						name = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), name.toUtf8(), languageId, "Attribute");
						description = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), description.toUtf8(), languageId, "Attribute");
					}
					
					paramNames << name;
					paramDescriptions << description;
				}
			}
		}
	}
	
	sdlRepresentation.paramIds = paramIds;
	sdlRepresentation.paramTypeIds = paramTypeIds;
	sdlRepresentation.paramNames = paramNames;
	sdlRepresentation.paramDescriptions = paramDescriptions;
	sdlRepresentation.parameters = parameters;
	
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
	
	if (!sdlRepresentation.paramIds.has_value()){
		return false;
	}
	
	if (!sdlRepresentation.parameters.has_value()){
		return false;
	}
	
	QList<QByteArray> parameterIds = *sdlRepresentation.paramIds;
	QList<QString> parameters = *sdlRepresentation.parameters;
	
	if (parameterIds.size() != parameters.size()){
		return false;
	}
	
	for (int i = 0; i < parameterIds.size(); i++){
		const QByteArray parameterId = parameterIds[i];
		iser::ISerializable* parameterPtr = paramsSetPtr->GetEditableParameter(parameterId);
		if (parameterPtr == nullptr){
			continue;
		}

		QJsonDocument document = QJsonDocument::fromJson(parameters[i].toUtf8());
		if (document.isNull()){
			return false;
		}
		
		if (!document.isObject()){
			return false;
		}
		
		const IJsonRepresentationController* jsonRepresentationControllerPtr = FindSubController(parameterId);
		if (jsonRepresentationControllerPtr == nullptr){
			return false;
		}
		
		if (!jsonRepresentationControllerPtr->GetDataModelFromRepresentation(document.object(), *parameterPtr)){
			return false;
		}
	}

	return true;
}


} // namespace imtserverapp


