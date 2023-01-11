#pragma once


// ImtCore includes
#include <imtbase/CObjectRepresentationControllerCompBase.h>


namespace imtbase
{


class CSelectionParamRepresentationControllerComp: public imtbase::CObjectRepresentationControllerCompBase
{
public:
	typedef imtbase::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSelectionParamRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable &dataModel) const override;
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(const CTreeItemModel &representation, istd::IChangeable &dataModel) const override;
};


} // namespace imtbase


