#pragma once


// ImtCore includes
#include <imtrest/CObjectRepresentationControllerCompBase.h>
#include <imtrest/CCommandRepresentationController.h>
#include <imtrest/IGuiElementModel.h>


namespace imtrest
{


class CGuiElementRepresentationControllerComp: public imtrest::CObjectRepresentationControllerCompBase
{
public:
	typedef imtrest::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGuiElementRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	virtual bool SetupItemModel(const imtrest::IGuiElementModel& guiElementModel, imtbase::CTreeItemModel& representation, int index, const iprm::IParamsSet* paramsPtr) const;

	// reimplemented (imtrest::CObjectRepresentationControllerCompBase)
	virtual bool GetRepresentationFromValue(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;

	// reimplemented (IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const override;

	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;

private:
	istd::TSmartPtr<imtrest::CCommandRepresentationController> m_commandRepresentationControllerPtr;
};


} // namespace imtbase


