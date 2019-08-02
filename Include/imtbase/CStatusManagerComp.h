#pragma once


// ACF includes
#include <istd/IInformationProvider.h>
#include <icomp/CComponentBase.h>


namespace imtbase
{


/**
	Common status manager component implementation.
*/
class CStatusManagerComp:
			public icomp::CComponentBase,
			virtual public istd::IInformationProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStatusManagerComp);
		I_REGISTER_INTERFACE(IInformationProvider);
		I_ASSIGN(m_defaultDescriptionAttrPtr, "DefaultDescription", "Default information description used if no information description is set", false, "");
		I_ASSIGN(m_defaultSourceAttrPtr, "DefaultSource", "Default information source used if no information source is set", false, "Unknown");
	I_END_COMPONENT;

	CStatusManagerComp();

	// reimplemented (iinsp::IInformationProvider)
	virtual QDateTime GetInformationTimeStamp() const;
	virtual InformationCategory GetInformationCategory() const;
	virtual int GetInformationId() const;
	virtual QString GetInformationDescription() const;
	virtual QString GetInformationSource() const;
	virtual int GetInformationFlags() const;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS);

private:
	QDateTime m_timeStamp;
	InformationCategory m_category;
	int m_id;
	QString m_description;
	QString m_source;
	int m_flags;

	I_ATTR(QString, m_defaultDescriptionAttrPtr);
	I_ATTR(QString, m_defaultSourceAttrPtr);
};


} // namespace imtbase


