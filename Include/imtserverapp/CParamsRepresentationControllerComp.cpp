// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CParamsRepresentationControllerComp.h>


// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/IParamsInfoProvider.h>
#include <iprm/TParamsPtr.h>
#include <iqt/iqt.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0>)

IJsonRepresentationController::RepresentationInfo CParamsRepresentationControllerComp::GetRepresentationInfo() const
{
	static QByteArray id = QByteArrayLiteral("imtserverapp::CParamsRepresentationControllerComp");

	return {QByteArrayLiteral("")};
}


QByteArray CParamsRepresentationControllerComp::GetTypeId() const
{
	return QByteArrayLiteral("imtserverapp::CParamsRepresentationControllerComp");
}


bool CParamsRepresentationControllerComp::IsModelSupported(const istd::IChangeable &dataModel) const
{
	auto paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);
	auto slaveControllerPtr = GetRepresentationController(dataModel);

	return paramsSetPtr != nullptr || slaveControllerPtr != nullptr;
}


bool CParamsRepresentationControllerComp::GetRepresentationFromDataModel(
	const istd::IChangeable& dataModel,
	QJsonObject& representation,
	const iprm::IParamsSet* paramsPtr) const
{
	const iprm::IParamsSet* sourcePtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);
	if (sourcePtr == nullptr){
		auto controllerPtr = GetRepresentationController(dataModel);
		if (controllerPtr == nullptr){
			return false;
		}

		return controllerPtr->GetRepresentationFromDataModel(dataModel, representation, paramsPtr);
	}

	sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0 sdlParamsSet;
	if (ParamsSetModelToSdl(*sourcePtr, sdlParamsSet, paramsPtr)){
		return sdlParamsSet.WriteToJsonObject(representation);
	}

	return false;
}


bool CParamsRepresentationControllerComp::GetDataModelFromRepresentation(const QJsonObject& representation, istd::IChangeable& dataModel) const
{
	const iprm::IParamsSet* sourcePtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);
	if (sourcePtr == nullptr){
		auto controllerPtr = GetRepresentationController(dataModel);
		if (controllerPtr == nullptr){
			return false;
		}

		return controllerPtr->GetDataModelFromRepresentation(representation, dataModel);
	}

	//sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0 sdlParamsSet;
	//if (!sdlParamsSet.ReadFromJsonObject(representation)){
	//	return false;
	//}

	return true;
}


// reimplemented (icomp::CComponentBase)

void CParamsRepresentationControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	//if (m_slaveControllersCompPtr.IsValid()){
	//	for (int i = 0; i < m_slaveControllersCompPtr.GetCount(); i++){
	//		if (m_slaveControllersCompPtr[i] != nullptr){
	//			Q_ASSERT(m_slaveControllersCompPtr[i]->IsModelSupported(dummyParamsSet));

	//			m_paramsSetController.RegisterSubController(*m_slaveControllersCompPtr[i]);
	//		}
	//	}
	//}

	//m_paramsSetController.RegisterSubController(*this);
}


// private methods

IJsonRepresentationController* CParamsRepresentationControllerComp::GetRepresentationController(const istd::IChangeable& dataModel) const
{
	if (m_slaveControllersCompPtr.IsValid()){
		for (int i = 0; i < m_slaveControllersCompPtr.GetCount(); i++){
			if (m_slaveControllersCompPtr[i] != nullptr){
				if (m_slaveControllersCompPtr[i]->IsModelSupported(dataModel)){
					return m_slaveControllersCompPtr[i];
				}
			}
		}
	}

	return nullptr;
}


bool CParamsRepresentationControllerComp::ParamsSetModelToSdl(
			const iprm::IParamsSet& model,
			sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdl,
			const iprm::IParamsSet* paramsPtr) const
{
	QByteArray languageId;
	if (paramsPtr != nullptr){
		iprm::TParamsPtr<iprm::IIdParam> languageParamPtr(paramsPtr, "LanguageParam");
		if (languageParamPtr.IsValid()){
			languageId = languageParamPtr->GetId();
		}
	}

	const iprm::IParamsInfoProvider* paramsInfoProviderPtr = model.GetParamsInfoProvider();

	iprm::IParamsSet::Ids paramSetIds = model.GetParamIds();
	QByteArrayList parameterIds = paramSetIds.values();
	std::sort(parameterIds.begin(), parameterIds.end());

	imtsdl::TElementList<sdl::imtbase::ImtBaseTypes::CParameter::V1_0> parameterList;

	for (const QByteArray& parameterId : parameterIds){
		if (!parameterId.contains("/")){
			const iser::ISerializable* parameterPtr = model.GetParameter(parameterId);
			if (parameterPtr == nullptr){
				return false;
			}

			QByteArray typeId;
			QString parameterName;
			QJsonObject parameterRepresentation;

			const iprm::IParamsSet* subParamsSetPtr = dynamic_cast<const iprm::IParamsSet*>(parameterPtr);
			if (subParamsSetPtr != nullptr){
				typeId = GetTypeId();

				if (!GetRepresentationFromDataModel(*subParamsSetPtr, parameterRepresentation, paramsPtr)){
					return false;
				}
			}
			else{
				const IJsonRepresentationController* subControllerPtr = GetRepresentationController(*parameterPtr);
				if (subControllerPtr == nullptr){
					return false;
				}

				typeId = subControllerPtr->GetTypeId();

				if (!subControllerPtr->GetRepresentationFromDataModel(*parameterPtr, parameterRepresentation, paramsPtr)){
					return false;
				}
			}

			Q_ASSERT(!typeId.isEmpty());

			sdl::imtbase::ImtBaseTypes::CParameter::V1_0 parameter;
			QJsonDocument jsonDocument(parameterRepresentation);

			parameter.id = parameterId;
			parameter.typeId = typeId;
			parameter.data = jsonDocument.toJson(QJsonDocument::Compact);
			parameter.name.emplace();
			parameter.description.emplace();

			if (paramsInfoProviderPtr != nullptr){
				auto infoPtr = paramsInfoProviderPtr->GetParamInfo(parameterId);
				if (infoPtr != nullptr){
					QString name = infoPtr->name;
					QString description = infoPtr->description;

					if (m_translationManagerCompPtr.IsValid()){
						name = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), name.toUtf8(), languageId, "Attribute");
						description = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), description.toUtf8(), languageId, "Attribute");
					}

					parameter.name = name;
					parameter.description = description;
				}
			}

			parameterList << parameter;
		}
	}

	sdl.parameters = parameterList;

	return true;
}


bool CParamsRepresentationControllerComp::ParamsSetSdlToModel(
			const sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdl,
			iprm::IParamsSet& model,
			const iprm::IParamsSet* paramsPtr) const
{
	return false;
}


} // namespace imtserverapp


