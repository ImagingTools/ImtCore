#pragma once


// ImtCore includes
#include <imtbase/CObjectRepresentationControllerCompBase.h>


namespace imtbase
{


class CTextParamRepresentationControllerComp: public imtbase::CObjectRepresentationControllerCompBase
{
public:
	typedef imtbase::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTextParamRepresentationControllerComp)
		I_ASSIGN(m_paramNameAttrPtr, "ObjectName", "Object Name", false, "");
		I_ASSIGN(m_qmlPathAttrPtr, "PathToQml", "Path to qml component for representation data", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable &dataModel) const override;
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(const CTreeItemModel &representation, istd::IChangeable &dataModel) const override;

protected:
	I_TEXTATTR(m_paramNameAttrPtr);
	I_TEXTATTR(m_qmlPathAttrPtr);
};


} // namespace imtbase


