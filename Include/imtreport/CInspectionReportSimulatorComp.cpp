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
	IInspectionReportBuilder::InspectionRegion ir1;

	IInspectionReportBuilder::InspectionRegionResult ir1r1;
	ir1r1.regionName = "ir1";
	ir1r1.errorClass = IInspectionReportBuilder::ErrorClass::EC_GLUE_GAP;
	ir1r1.status = istd::IInformationProvider::IC_ERROR;
	ir1r1.length = 0.0;
	ir1r1.value = 85.0;
	ir1r1.tolerance = 50.0;
	ir1r1.diff = 35.0;

	IInspectionReportBuilder::InspectionRegionResult ir1r2;
	ir1r2.regionName = "ir1";
	ir1r2.errorClass = IInspectionReportBuilder::ErrorClass::EC_GLUE_WIDE;
	ir1r2.status = istd::IInformationProvider::IC_ERROR;
	ir1r2.length = 15.0;
	ir1r2.value = 85.0;
	ir1r2.tolerance = 80.0;
	ir1r2.diff = 5.0;

	IInspectionReportBuilder::InspectionRegionResult ir1r3;
	ir1r3.regionName = "ir1";
	ir1r3.errorClass = IInspectionReportBuilder::ErrorClass::EC_GLUE_NARROW;
	ir1r3.status = istd::IInformationProvider::InformationCategory::IC_ERROR;
	ir1r3.length = 10.0;
	ir1r3.value = 50.0;
	ir1r3.tolerance = 80.0;
	ir1r3.diff = 30.0;

	IInspectionReportBuilder::InspectionRegionResult ir1r4;
	ir1r4.regionName = "ir1";
	ir1r4.errorClass = IInspectionReportBuilder::ErrorClass::EC_GLUE_NARROW;
	ir1r4.status = istd::IInformationProvider::InformationCategory::IC_ERROR;
	ir1r4.length = 11.0;
	ir1r4.value = 53.0;
	ir1r4.tolerance = 80.0;
	ir1r4.diff = 13.0;

	IInspectionReportBuilder::InspectionRegionResult ir1r5;
	ir1r5.regionName = "ir1";
	ir1r5.errorClass = IInspectionReportBuilder::ErrorClass::EC_GLUE_NARROW;
	ir1r5.status = istd::IInformationProvider::InformationCategory::IC_ERROR;
	ir1r5.length = 12.0;
	ir1r5.value = 40.0;
	ir1r5.tolerance = 80.0;
	ir1r5.diff = 40.0;

	ir1.name = "ir1";
	ir1.results.push_back(ir1r1);
	ir1.results.push_back(ir1r2);
	ir1.results.push_back(ir1r3);
	ir1.results.push_back(ir1r4);
	ir1.results.push_back(ir1r5);

	IInspectionReportBuilder::Inspection i1;
	i1.name = "Inspection 1";
	i1.imagePath = "c:\\work\\Inspection.png";
	i1.status = istd::IInformationProvider::IC_ERROR;

	reportData.companyName = "Company name";
	reportData.appVersion = "1.0.0.0";
	reportData.productName = "Product name";
	reportData.time = QDateTime::currentDateTime();
	reportData.imagePath = "c:\\work\\Inspection.png";
	reportData.partSerialNumber = "1234 5678";
	reportData.partStatus = istd::IInformationProvider::IC_WARNING;

	i1.regions.push_back(ir1);

	reportData.inspections.push_back(i1);
}


} // namespace imtreport


