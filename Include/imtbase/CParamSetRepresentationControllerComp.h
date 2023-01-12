#pragma once


// ImtCore includes
#include <imtbase/CObjectRepresentationControllerCompBase.h>


namespace imtbase
{


class CParamSetRepresentationControllerComp: public imtbase::CObjectRepresentationControllerCompBase
{
public:
	typedef imtbase::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CParamSetRepresentationControllerComp)
		I_ASSIGN_MULTI_0(m_paramRepresentationControllersCompPtr, "ParamRepresentationControllers", "Sub parameters representation controllers", false);
		I_ASSIGN(m_paramNameAttrPtr, "ObjectName", "Object Name", false, "");
		I_ASSIGN(m_qmlPathAttrPtr, "PathToQml", "Path to qml component for representation data", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(const CTreeItemModel &representation, istd::IChangeable &dataModel) const override;

	const IRepresentationController* FindSubController(const QByteArray& parameterId) const;

protected:
	/**
		All needed sub-controllers according to the used data model types inside of the parameter set
	*/
	I_MULTIREF(IRepresentationController, m_paramRepresentationControllersCompPtr);

	I_TEXTATTR(m_paramNameAttrPtr);
	I_TEXTATTR(m_qmlPathAttrPtr);
};


} // namespace imtbase


