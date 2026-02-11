// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IStatusManager.h>


namespace imtbase
{


/**
	Common status manager implementation.
*/
class CSimpleStatus: virtual public imtbase::IStatusManager
{
public:
	CSimpleStatus();

	// reimplemented (iinsp::IInformationProvider)
	virtual QDateTime GetInformationTimeStamp() const override;
	virtual InformationCategory GetInformationCategory() const override;
	virtual int GetInformationId() const override;
	virtual QString GetInformationDescription() const override;
	virtual QString GetInformationSource() const override;
	virtual int GetInformationFlags() const override;

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

protected:
	QDateTime m_timeStamp;
	InformationCategory m_category;
	int m_id;
	QString m_description;
	QString m_source;
	int m_flags;
};


} // namespace imtbase


