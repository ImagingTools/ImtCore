#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtserverapp/IJsonRepresentationController.h>


namespace imtserverapp
{


template<class SdlClass>
class TJsonRepresentationControllerCompWrap:
			public ilog::CLoggerComponentBase,
			virtual public imtserverapp::IJsonRepresentationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	
	I_BEGIN_BASE_COMPONENT(TJsonRepresentationControllerCompWrap);
		I_REGISTER_INTERFACE(imtserverapp::IJsonRepresentationController);
		I_ASSIGN(m_modelIdAttrPtr, "ModelId", "Model-ID", false, "");
		I_ASSIGN(m_nameAttrPtr, "Name", "Name", false, "");
		I_ASSIGN(m_descriptionAttrPtr, "Description", "Description", false, "");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;
		
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
	I_ATTR(QByteArray, m_modelIdAttrPtr);
	I_TEXTATTR(m_nameAttrPtr);
	I_TEXTATTR(m_descriptionAttrPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


template<class SdlClass>
imtserverapp::IJsonRepresentationController::RepresentationInfo TJsonRepresentationControllerCompWrap<SdlClass>::GetRepresentationInfo() const
{
	imtserverapp::IJsonRepresentationController::RepresentationInfo representationInfo;
	representationInfo.modelId = m_modelIdAttrPtr.IsValid() ? *m_modelIdAttrPtr : QByteArray();
	representationInfo.name = m_nameAttrPtr.IsValid() ? *m_nameAttrPtr : QString();
	representationInfo.description = m_descriptionAttrPtr.IsValid() ? *m_descriptionAttrPtr : QString();
	
	return representationInfo;
}


template<class SdlClass>
bool TJsonRepresentationControllerCompWrap<SdlClass>::GetRepresentationFromDataModel(
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
bool TJsonRepresentationControllerCompWrap<SdlClass>::GetDataModelFromRepresentation(
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


