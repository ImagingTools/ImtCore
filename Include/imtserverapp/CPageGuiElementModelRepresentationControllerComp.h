#pragma once


// ImtCore includes
#include <imtserverapp/CGuiElementModelRepresentationControllerComp.h>


namespace imtserverapp
{


class CPageGuiElementModelRepresentationControllerComp: public imtserverapp::CGuiElementModelRepresentationControllerComp
{
public:
	typedef CGuiElementModelRepresentationControllerComp BaseClass;
	I_BEGIN_COMPONENT(CPageGuiElementModelRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (imtserverapp::IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetRepresentationFromDataModel(
				const istd::IChangeable& dataModel,
				imtbase::CTreeItemModel& representation,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(
				const imtbase::CTreeItemModel& representation,
				istd::IChangeable& dataModel) const override;
};


} // namespace imtserverapp


