#pragma once


// ImtCore includes
#include <imtbase/CObjectRepresentationControllerCompBase.h>
#include <imtbase/CCommandRepresentationController.h>
#include <imtgui/IGuiElementModel.h>


namespace imtbase
{


class CGuiElementRepresentationControllerComp: public imtbase::CObjectRepresentationControllerCompBase
{
public:
	typedef imtbase::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGuiElementRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	virtual bool SetupItemModel(const imtgui::IGuiElementModel& guiElementModel, CTreeItemModel& representation, int index, const iprm::IParamsSet* paramsPtr) const;

	// reimplemented (imtbase::CObjectRepresentationControllerCompBase)
	virtual bool GetRepresentationFromValue(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;

	// reimplemented (IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const override;

	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;

private:
	istd::TSmartPtr<imtbase::CCommandRepresentationController> m_commandRepresentationControllerPtr;
};


} // namespace imtbase


