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
	// inspection 1 region 1 results
	IInspectionReportBuilder::InspectionRegionResult i1r1r1;
	i1r1r1.regionName = "i1r1";
	i1r1r1.errorClass = IInspectionReportBuilder::ErrorClass::EC_GLUE_GAP;
	i1r1r1.status = istd::IInformationProvider::IC_ERROR;
	i1r1r1.length = 0.0;
	i1r1r1.value = 85.0;
	i1r1r1.tolerance = 50.0;
	i1r1r1.diff = 35.0;
	i1r1r1.flags = IInspectionReportBuilder::RF_SHOW_IN_SUMMARY;

	IInspectionReportBuilder::InspectionRegionResult i1r1r2;
	i1r1r2.regionName = "i1r1";
	i1r1r2.errorClass = IInspectionReportBuilder::ErrorClass::EC_NONE;
	i1r1r2.status = istd::IInformationProvider::IC_ERROR;
	i1r1r2.length = 15.0;
	i1r1r2.value = 85.0;
	i1r1r2.tolerance = 80.0;
	i1r1r2.diff = 5.0;
	i1r1r2.flags = IInspectionReportBuilder::RF_NONE;

	IInspectionReportBuilder::InspectionRegionResult i1r1r3;
	i1r1r3.regionName = "i1r1";
	i1r1r3.errorClass = IInspectionReportBuilder::ErrorClass::EC_GLUE_NARROW;
	i1r1r3.status = istd::IInformationProvider::InformationCategory::IC_ERROR;
	i1r1r3.length = 10.0;
	i1r1r3.value = 50.0;
	i1r1r3.tolerance = 80.0;
	i1r1r3.diff = 30.0;
	i1r1r3.flags = IInspectionReportBuilder::RF_SHOW_IN_SUMMARY;

	IInspectionReportBuilder::InspectionRegionResult i1r1r4;
	i1r1r4.regionName = "i1r1";
	i1r1r4.errorClass = IInspectionReportBuilder::ErrorClass::EC_GLUE_NARROW;
	i1r1r4.status = istd::IInformationProvider::InformationCategory::IC_ERROR;
	i1r1r4.length = 11.0;
	i1r1r4.value = 53.0;
	i1r1r4.tolerance = 80.0;
	i1r1r4.diff = 13.0;
	i1r1r4.flags = IInspectionReportBuilder::RF_NONE;

	// inspectrion 1 region 2 results
	IInspectionReportBuilder::InspectionRegionResult i1r2r1;
	i1r2r1.regionName = "i1r2";
	i1r2r1.errorClass = IInspectionReportBuilder::ErrorClass::EC_GLUE_NARROW;
	i1r2r1.status = istd::IInformationProvider::InformationCategory::IC_ERROR;
	i1r2r1.length = 12.0;
	i1r2r1.value = 40.0;
	i1r2r1.tolerance = 80.0;
	i1r2r1.diff = 40.0;
	i1r2r1.flags = IInspectionReportBuilder::RF_SHOW_IN_SUMMARY;

	IInspectionReportBuilder::InspectionRegionResult i1r2r2;
	i1r2r2.regionName = "i1r2";
	i1r2r2.errorClass = IInspectionReportBuilder::ErrorClass::EC_GLUE_GAP;
	i1r2r2.status = istd::IInformationProvider::InformationCategory::IC_CRITICAL;
	i1r2r2.length = 13.0;
	i1r2r2.value = 41.0;
	i1r2r2.tolerance = 80.0;
	i1r2r2.diff = 39.0;
	i1r2r2.flags = IInspectionReportBuilder::RF_NONE;

	// inspection 1 region 1
	IInspectionReportBuilder::InspectionRegion i1r1;
	i1r1.name = "i1r1";
	i1r1.results.push_back(i1r1r1);
	i1r1.results.push_back(i1r1r2);
	i1r1.results.push_back(i1r1r3);
	i1r1.results.push_back(i1r1r4);

	// inspection 1 region 2
	IInspectionReportBuilder::InspectionRegion i1r2;
	i1r2.name = "i1r2";
	i1r2.results.push_back(i1r2r1);
	i1r2.results.push_back(i1r2r2);

	IInspectionReportBuilder::Inspection i1;
	i1.name = "Inspection 1";
	i1.imagePath = "c:\\work\\Inspection.png";
	i1.status = istd::IInformationProvider::IC_ERROR;
	i1.regions.push_back(i1r1);
	i1.regions.push_back(i1r2);

	// inspection 2

	reportData.companyName = "Company name";
	reportData.appVersion = "1.0.0.0";
	reportData.productName = "Product name";
	reportData.time = QDateTime::currentDateTime();
	reportData.imagePath = "c:\\work\\Inspection.png";
	reportData.partSerialNumber = "1234 5678";
	reportData.partStatus = istd::IInformationProvider::IC_WARNING;

	reportData.inspections.push_back(i1);
}


} // namespace imtreport


