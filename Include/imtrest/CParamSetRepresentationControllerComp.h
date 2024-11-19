#pragma once


// ImtCore includes
#include <imtrest/CObjectRepresentationControllerCompBase.h>


namespace imtrest
{


class CParamSetRepresentationControllerComp: public imtrest::CObjectRepresentationControllerCompBase
{
public:
	typedef imtrest::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CParamSetRepresentationControllerComp)
		I_ASSIGN_MULTI_0(m_paramRepresentationControllersCompPtr, "ParamRepresentationControllers", "Sub parameters representation controllers", false);
	I_END_COMPONENT;

protected:
	const IRepresentationController* FindSubController(const QByteArray& parameterId) const;

	// reimplemented (imtrest::CObjectRepresentationControllerCompBase)
	virtual bool GetRepresentationFromValue(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;

	// reimplemented (imtrest::IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetDataModelFromRepresentation(const imtbase::CTreeItemModel &representation, istd::IChangeable &dataModel) const override;

protected:
	/**
		All needed sub-controllers according to the used data model types inside of the parameter set
	*/
	I_MULTIREF(IRepresentationController, m_paramRepresentationControllersCompPtr);
};


} // namespace imtrest


