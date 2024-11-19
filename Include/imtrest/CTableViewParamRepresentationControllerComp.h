#pragma once


// ImtCore includes
#include <imtrest/CGuiElementRepresentationControllerComp.h>


namespace imtrest
{


class CTableViewParamRepresentationControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtrest::IRepresentationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTableViewParamRepresentationControllerComp)
		I_REGISTER_INTERFACE(imtrest::IRepresentationController);
		I_ASSIGN(m_modelIdAttrPtr, "ModelId", "Model-ID", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::IRepresentationController)
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
};


} // namespace imtbase


