#pragma once


// ImtCore includes
#include <imtbase/CObjectRepresentationControllerCompBase.h>


namespace imtbase
{


class CDatabaseAccessSettingsRepresentationControllerComp: public imtbase::CObjectRepresentationControllerCompBase
{
public:
	typedef imtbase::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDatabaseAccessSettingsRepresentationControllerComp)
		I_ASSIGN(m_dbNamePathAttrPtr, "DatabaseNameSource", "Path to qml database name component for representation data", false, "");
		I_ASSIGN(m_hostPathAttrPtr, "HostSource", "Path to qml host component for representation data", false, "");
		I_ASSIGN(m_passwordPathAttrPtr, "PasswordSource", "Path to qml password component for representation data", false, "");
		I_ASSIGN(m_portAttrPtr, "PortSource", "Path to qml port component for representation data", false, "");
		I_ASSIGN(m_usernamePathAttrPtr, "UsernameSource", "Path to qml username component for representation data", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::CObjectRepresentationControllerCompBase)
	virtual bool GetRepresentationFromValue(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;

	// reimplemented (IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable &dataModel) const override;
	virtual bool GetDataModelFromRepresentation(const CTreeItemModel &representation, istd::IChangeable &dataModel) const override;

protected:
	I_ATTR(QByteArray, m_dbNamePathAttrPtr);
	I_ATTR(QByteArray, m_hostPathAttrPtr);
	I_ATTR(QByteArray, m_passwordPathAttrPtr);
	I_ATTR(QByteArray, m_portAttrPtr);
	I_ATTR(QByteArray, m_usernamePathAttrPtr);
};


} // namespace imtbase


