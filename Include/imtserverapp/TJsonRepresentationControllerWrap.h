// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtserverapp/IJsonRepresentationController.h>


namespace imtserverapp
{


template<class SdlClass>
class TJsonRepresentationControllerWrap: virtual public IJsonRepresentationController
{
public:
	virtual RepresentationInfo GetRepresentationInfo() const override;
	virtual bool GetRepresentationFromDataModel(
				const istd::IChangeable& dataModel,
				QJsonObject& representation,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(
				const QJsonObject& representation,
				istd::IChangeable& dataModel) const override;
	
protected:
	virtual bool GetSdlRepresentationFromDataModel(SdlClass& sdlRepresentation, const istd::IChangeable& dataModel, const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
	virtual bool GetDataModelFromSdlRepresentation(istd::IChangeable& dataModel, const SdlClass& sdlRepresentation) const = 0;

protected:
	QByteArray m_modelId;
	QString m_name;
	QString m_description;
	iqt::ITranslationManager* m_translationManagerPtr = nullptr;
};


template<class SdlClass>
IJsonRepresentationController::RepresentationInfo TJsonRepresentationControllerWrap<SdlClass>::GetRepresentationInfo() const
{
	RepresentationInfo representationInfo;
	representationInfo.modelId = m_modelId;
	representationInfo.name = m_name;
	representationInfo.description = m_description;

	return representationInfo;
}


template<class SdlClass>
bool TJsonRepresentationControllerWrap<SdlClass>::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			QJsonObject& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	SdlClass sdlClass;
	if (!GetSdlRepresentationFromDataModel(sdlClass, dataModel, paramsPtr)){
		return false;
	}

	return sdlClass.WriteToJsonObject(representation);
}


template<class SdlClass>
bool TJsonRepresentationControllerWrap<SdlClass>::GetDataModelFromRepresentation(
			const QJsonObject& representation,
			istd::IChangeable& dataModel) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	SdlClass sdlClass;
	if (!sdlClass.ReadFromJsonObject(representation)){
		return false;
	}

	return GetDataModelFromSdlRepresentation(dataModel, sdlClass);
}


} // namespace imtserverapp


