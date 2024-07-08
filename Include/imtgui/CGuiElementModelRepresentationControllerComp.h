#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtbase/CGuiElementRepresentationControllerComp.h>
#include <imtbase/IRepresentationController.h>


namespace imtgui
{


class CGuiElementModelRepresentationControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::IRepresentationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	I_BEGIN_COMPONENT(CGuiElementModelRepresentationControllerComp)
		I_REGISTER_INTERFACE(imtbase::IRepresentationController);
		I_ASSIGN(m_representationControllerCompPtr, "ContainerRepresentationController", "Representation controller for the sub elements container", false, "ContainerRepresentationController");
		I_ASSIGN(m_modelIdAttrPtr, "ModelId", "Model-ID", false, "");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IRepresentationController)
	virtual QByteArray GetModelId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetRepresentationFromDataModel(
				const istd::IChangeable& dataModel,
				imtbase::CTreeItemModel& representation,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(
				const imtbase::CTreeItemModel& representation,
				istd::IChangeable& dataModel) const override;

protected:
	I_ATTR(QByteArray, m_modelIdAttrPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
	I_REF(imtbase::IRepresentationController, m_representationControllerCompPtr);
};


} // namespace imtgui


