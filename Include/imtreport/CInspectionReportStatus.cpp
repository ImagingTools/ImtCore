#include <imtreport/CInspectionReportStatus.h>


namespace imtreport
{


// reimplemented (istd::IInformationProvider)


CInspectionReportStatus::CInspectionReportStatus(const InformationCategory status) : m_status(status)
{
}


istd::IInformationProvider::InformationCategory CInspectionReportStatus::GetInformationCategory() const
{
	return m_status;
}


QString CInspectionReportStatus::GetInformationDescription() const
{
	switch (m_status)
	{
	case IC_INFO:
		return "OK";
	case IC_WARNING:
		return "WARNING";
	case IC_ERROR:
		return "ERROR";
	default:
		return "";
	}
}


} // namespace imtreport


