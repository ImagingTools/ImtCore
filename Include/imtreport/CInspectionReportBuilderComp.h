#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <icmm/CRgb.h>

// ImtCore includes
#include <imtreport/IInspectionReportBuilder.h>
#include <imtreport/IReportPage.h>


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
	static icmm::CRgb GetStatusColor(istd::IInformationProvider::InformationCategory status);
	static QString GetErrorClassText(const ErrorClass errorClass);

	bool CreateTitlePage(const ReportInputData& reportData, IReportDocument& reportDocument) const;
	bool CreateInspectionPage(const ReportInputData& reportData, const Inspection& inspection, IReportDocument& reportDocument) const;

	void GetTitlePageRegionResults(const ReportInputData& reportData, Results& results) const;
	void GetInspectionPageRegionResults(const Inspection& inspection, Results& results) const;

	void AddHeader(const ReportInputData& reportData,
				   const Inspection* inspectionPtr,
				   i2d::CVector2d& topLeft,
				   IReportPage& page) const;
	void AddBody(const QString& imagePath, i2d::CVector2d& topLeft, IReportPage& page) const;
	void AddFooter(const Results& regions, i2d::CVector2d& topLeft, IReportPage& page) const;
};


} // namespace imtreport


