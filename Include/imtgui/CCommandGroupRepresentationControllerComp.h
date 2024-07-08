#pragma once


// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgui/CGuiElementModelRepresentationControllerComp.h>


namespace imtgui
{


class CCommandGroupRepresentationControllerComp: public imtgui::CGuiElementModelRepresentationControllerComp
{
public:
	typedef imtgui::CGuiElementModelRepresentationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CCommandGroupRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetRepresentationFromDataModel(
				const istd::IChangeable& dataModel,
				imtbase::CTreeItemModel& representation,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const override;
};


} // namespace imtgui


