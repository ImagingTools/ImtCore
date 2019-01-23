#include <imtreport/CInspectionReportSimulatorComp.h>


// ImtCore includes
#include <imtreport/IReportPage.h>


namespace imtreport
{


// reimplemented (icomp::CComponentBase)

void CInspectionReportSimulatorComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_inspectionReportBuilderCompPtr.IsValid() && m_reportDocumentCompPtr.IsValid()){
		IInspectionReportBuilder::ReportInputData reportInputData;
		PrepareReportInputData(reportInputData);

		bool result = m_inspectionReportBuilderCompPtr->CreateReport(reportInputData, *m_reportDocumentCompPtr.GetPtr());
		Q_ASSERT(result);
	}
}


// private methods

void CInspectionReportSimulatorComp::PrepareReportInputData(IInspectionReportBuilder::ReportInputData& reportData) const
{
	IInspectionReportBuilder::InspectionRegionResult ir1r1;
	ir1r1.errorClass = IInspectionReportBuilder::ErrorClass::EC_GAP;
	ir1r1.status = istd::IInformationProvider::IC_ERROR;
	ir1r1.length = 0.0;
	ir1r1.value = 85.0;
	ir1r1.tolerance = 50.0;
	ir1r1.diff = 35.0;

	IInspectionReportBuilder::InspectionRegionResult ir1r2;
	ir1r2.errorClass = IInspectionReportBuilder::ErrorClass::EC_GLUE_WIDE;
	ir1r2.status = istd::IInformationProvider::IC_ERROR;
	ir1r2.length = 15.0;
	ir1r2.value = 66.0;
	ir1r2.tolerance = 80.0;
	ir1r2.diff = 14.0;

	IInspectionReportBuilder::InspectionRegionResult ir1r3;
	ir1r3.errorClass = IInspectionReportBuilder::ErrorClass::EC_GLUE_NARROW;
	ir1r3.status = istd::IInformationProvider::InformationCategory::IC_ERROR;
	ir1r3.length = 10.0;
	ir1r3.value = 52.0;
	ir1r3.tolerance = 40.0;
	ir1r3.diff = 12.0;

	IInspectionReportBuilder::InspectionRegion ir1;
	ir1.name = "ir1";
	ir1.results.push_back(ir1r1);
	ir1.results.push_back(ir1r2);
	ir1.results.push_back(ir1r3);

	IInspectionReportBuilder::InspectionInfo i1;
	i1.name = "Inspection 1";
	i1.image.load("c:\\work\\Develop\\Inspection.png");
	i1.status = istd::IInformationProvider::IC_ERROR;

	reportData.companyName = "Company name";
	reportData.appVersion = "1.0.0.0";
	reportData.productName = "Product name";
	reportData.time = QDateTime::currentDateTime();
	reportData.partSerialNumber = "1234 5678";
	reportData.partStatus = istd::IInformationProvider::IC_WARNING;

	//ir1.results.push_back(irr1);

	i1.regions.push_back(ir1);

	reportData.inspections.push_back(i1);

	// inspection 2
}


} // namespace imtreport


