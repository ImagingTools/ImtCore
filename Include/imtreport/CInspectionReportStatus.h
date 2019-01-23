#pragma once


// ACF includes
#include <istd/IInformationProvider.h>


namespace imtreport
{


class CInspectionReportStatus : public virtual istd::IInformationProvider
{
public:
	CInspectionReportStatus() = default;
	explicit CInspectionReportStatus(const InformationCategory status);

	QDateTime GetInformationTimeStamp() const override { return QDateTime(); }
	InformationCategory GetInformationCategory() const override;
	int GetInformationId() const override { return -1; }
	QString GetInformationDescription() const override;
	QString GetInformationSource() const override { return QString(); }
	int GetInformationFlags() const override { return -1; }

private:
	InformationCategory m_status = InformationCategory::IC_NONE;
};


} // namespace imtreport


