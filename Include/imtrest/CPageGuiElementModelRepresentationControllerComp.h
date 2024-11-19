#pragma once


// ImtCore includes
#include <imtrest/CGuiElementModelRepresentationControllerComp.h>


namespace imtrest
{


class CPageGuiElementModelRepresentationControllerComp: public imtrest::CGuiElementModelRepresentationControllerComp
{
public:
	typedef CGuiElementModelRepresentationControllerComp BaseClass;
	I_BEGIN_COMPONENT(CPageGuiElementModelRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetRepresentationFromDataModel(
				const istd::IChangeable& dataModel,
				imtbase::CTreeItemModel& representation,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(
				const imtbase::CTreeItemModel& representation,
				istd::IChangeable& dataModel) const override;
};


} // namespace imtrest


