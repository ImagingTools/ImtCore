#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IStatusManager.h>


namespace imtbase
{


/**
	Common status manager component implementation.
*/
class CStatusManagerComp:
			public icomp::CComponentBase,
			virtual public imtbase::IStatusManager
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStatusManagerComp);
		I_REGISTER_INTERFACE(imtbase::IStatusManager);
		I_REGISTER_INTERFACE(istd::IInformationProvider);
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

	// reimplemented (imtbase::IStatusManager)
	virtual void SetInformationTimeStamp(const QDateTime& timestamp) override;
	virtual void SetInformationCategory(InformationCategory category) override;
	virtual void SetInformationId(int id) override;
	virtual void SetInformationDescription(const QString& description) override;
	virtual void SetInformationSource(const QString& source) override;
	virtual void SetInformationFlags(int flags) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

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


