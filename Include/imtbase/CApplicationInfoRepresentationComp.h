#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IApplicationInfoRepresentation.h>


namespace imtbase
{


class CApplicationInfoRepresentationComp: public imtbase::IApplicationInfoRepresentation, public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CApplicationInfoRepresentationComp)
		I_REGISTER_INTERFACE(IApplicationInfoRepresentation);
		I_ASSIGN(m_paramIdAttrPtr, "ParamId", "ID of the param", true, "");
		I_ASSIGN(m_paramNameAttrPtr, "ParamName", "Name of the param", false, "");
		I_ASSIGN(m_sourceAttrPtr, "Source", "Source for component representation", false, "");
	I_END_COMPONENT;

	// reimplemented (imtbase::IApplicationInfoRepresentation)
	virtual bool GetRepresentationFromApplicationInfo(const ibase::IApplicationInfo& applicationInfo, CTreeItemModel& representation) const override;

protected:
	I_ATTR(QByteArray, m_paramIdAttrPtr);
	I_TEXTATTR(m_paramNameAttrPtr);
	I_TEXTATTR(m_sourceAttrPtr);
};


} // namespace imtbase


