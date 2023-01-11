#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IRepresentationController.h>


namespace imtbase
{


class CObjectRepresentationControllerCompBase:
			public icomp::CComponentBase,
			virtual public IRepresentationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectRepresentationControllerCompBase)
		I_REGISTER_INTERFACE(IRepresentationController);
		I_ASSIGN(m_paramIdAttrPtr, "ObjectId", "Object-ID", false, "");
		I_ASSIGN(m_paramNameAttrPtr, "ObjectName", "Object Name", false, "");
		I_ASSIGN(m_qmlPathAttrPtr, "PathToQml", "Path to qml component for representation data", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (IRepresentationController)
	virtual QByteArray GetModelId() const override;
	virtual bool IsModelSupported(const istd::IChangeable &dataModel) const override;
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const override;

protected:
	I_ATTR(QByteArray, m_paramIdAttrPtr);
	I_TEXTATTR(m_paramNameAttrPtr);
	I_TEXTATTR(m_qmlPathAttrPtr);
};


} // namespace imtbase


