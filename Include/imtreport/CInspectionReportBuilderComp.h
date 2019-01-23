#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtreport/IInspectionReportBuilder.h>


namespace imtreport
{


class CInspectionReportBuilderComp : public icomp::CComponentBase, virtual public IInspectionReportBuilder
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CInspectionReportBuilderComp);
		I_REGISTER_INTERFACE(imtreport::IInspectionReportBuilder);
	I_END_COMPONENT;

	// reimplemented (imtreport::IInspectionReportBuilder)
	bool CreateReport(const ReportInputData& inputData, IReportDocument& reportDocument, const iprm::IParamsSet* paramsPtr = nullptr) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;

private:
	static QString GetStatusText(istd::IInformationProvider::InformationCategory status);
	static QColor GetStatusColor(istd::IInformationProvider::InformationCategory status);

	bool CreateTitlePage(const ReportInputData& reportData, IReportDocument& reportDocument) const;
	bool CreateInspectionPage(const ReportInputData& reportData, const InspectionInfo& inspection, IReportDocument& reportDocument) const;
};


} // namespace imtreport


