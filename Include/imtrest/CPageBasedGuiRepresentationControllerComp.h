#pragma once


// ImtCore includes
#include <imtrest/CGuiElementRepresentationControllerComp.h>


namespace imtrest
{


class CPageBasedGuiRepresentationControllerComp: public imtrest::CGuiElementRepresentationControllerComp
{
public:
	typedef imtrest::CGuiElementRepresentationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CPageBasedGuiRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::CGuiElementRepresentationControllerComp)
	virtual bool SetupItemModel(
				const imtrest::IGuiElementModel& guiElementModel,
				imtbase::CTreeItemModel& representation,
				int index,
				const iprm::IParamsSet* paramsPtr) const override;
};


} // namespace imtrest


