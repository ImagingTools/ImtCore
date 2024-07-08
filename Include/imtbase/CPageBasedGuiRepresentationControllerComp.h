#pragma once


// ImtCore includes
#include <imtbase/CGuiElementRepresentationControllerComp.h>


namespace imtbase
{


class CPageBasedGuiRepresentationControllerComp: public CGuiElementRepresentationControllerComp
{
public:
	typedef imtbase::CGuiElementRepresentationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CPageBasedGuiRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::CGuiElementRepresentationControllerComp)
	virtual bool SetupItemModel(
				const imtgui::IGuiElementModel& guiElementModel,
				CTreeItemModel& representation,
				int index,
				const iprm::IParamsSet* paramsPtr) const override;
};


} // namespace imtbase


