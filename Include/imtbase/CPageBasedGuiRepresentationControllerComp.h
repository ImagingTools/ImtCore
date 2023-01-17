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
	// reimplemented (IRepresentationController)
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;
};


} // namespace imtbase


