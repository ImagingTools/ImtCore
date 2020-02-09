#include <imtreport/CInspectionReportBuilderComp.h>


// ACF includes
#include <i2d/CRectangle.h>

// ImtCore includes
#include <imtreport/TGraphicsElement.h>
#include <imtreport/CTextTable.h>
#include <imtreport/IReportDocument.h>


namespace imtreport
{


// reimplemented (IInspectionReportBuilder)

bool CInspectionReportBuilderComp::CreateReport(const ReportInputData& inputData, IReportDocument& reportDocument, const iprm::IParamsSet* paramsPtr /*= nullptr*/) const
{
	Q_UNUSED(paramsPtr);

	if (!CreateTitlePage(inputData, reportDocument)){
		return false;
	}

	for (const Inspection& inspection : inputData.inspections){
		if (!CreateInspectionPage(inputData, inspection, reportDocument)){
			return false;
		}
	}

	return true;
}


// reimplemented (icomp::CComponentBase)

void CInspectionReportBuilderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


//private static methods

QString CInspectionReportBuilderComp::GetStatusText(istd::IInformationProvider::InformationCategory status)
{
	switch(status){
		case istd::IInformationProvider::InformationCategory::IC_CRITICAL:
			return "SYSTEM ERROR";

		case istd::IInformationProvider::InformationCategory::IC_ERROR:
			return "NOT OK";

		case istd::IInformationProvider::InformationCategory::IC_INFO:
			return "OK";

		case istd::IInformationProvider::InformationCategory::IC_WARNING:
			return "WARNING";

		default:
			return QString();
	}
}


icmm::CRgb CInspectionReportBuilderComp::GetStatusColor(istd::IInformationProvider::InformationCategory status)
{
	switch (status){
	case istd::IInformationProvider::InformationCategory::IC_CRITICAL:
		return icmm::CRgb(0.8, 0, 0);

	case istd::IInformationProvider::InformationCategory::IC_ERROR:
		return icmm::CRgb(1.0, 0, 0);

	case istd::IInformationProvider::InformationCategory::IC_INFO:
		return icmm::CRgb(0, 0.8, 0);

	case istd::IInformationProvider::InformationCategory::IC_WARNING:
		return icmm::CRgb(0.5, 0.5, 0);

	default:
		return icmm::CRgb(0.5, 0.5, 0.5);
	}
}


QString CInspectionReportBuilderComp::GetErrorClassText(const ErrorClass errorClass)
{
	switch (errorClass){
	case ErrorClass::EC_GLUE_GAP:
		return QObject::tr("Gap");

	case ErrorClass::EC_GLUE_NARROW:
		return QObject::tr("Narrow");

	case ErrorClass::EC_GLUE_WIDE:
		return QObject::tr("Wide");

	case ErrorClass::EC_GLUE_BEYOND_AREA:
		return QObject::tr("Beyond area");

	default:
		return QString();
	}
}


// private methods

bool CInspectionReportBuilderComp::CreateTitlePage(const ReportInputData& reportData, IReportDocument& reportDocument) const
{
	IReportPage* pagePtr = dynamic_cast<IReportPage*>(reportDocument.InsertPage());
	if (!pagePtr){
		return false;
	}

	i2d::CVector2d topLeft(15.0, 10.0);

	AddHeader(reportData, NULL, topLeft, *pagePtr);
	AddBody(reportData.imagePath, topLeft, *pagePtr);
	AddTitlePageFooter(reportData, topLeft, *pagePtr);

	return true;
}


bool CInspectionReportBuilderComp::CreateInspectionPage(const ReportInputData& reportData, const Inspection& inspection, IReportDocument& reportDocument) const
{
	IReportPage* pagePtr = dynamic_cast<IReportPage*>(reportDocument.InsertPage());
	if (!pagePtr){
		return false;
	}

	i2d::CVector2d topLeft(15.0, 10.0);

	AddHeader(reportData, &inspection, topLeft, *pagePtr);
	AddBody(inspection.imagePath, topLeft, *pagePtr);
	AddInspectionPageFooter(inspection, topLeft, *pagePtr);

	return true;
}


void CInspectionReportBuilderComp::GetTitlePageRegionResults(const ReportInputData& reportData, Results& results) const
{
	for (int i = 0; i < reportData.inspections.size(); i++){
		const Inspection& inspection = reportData.inspections[i];

		for (int j = 0; j < inspection.regions.size(); j++){
			const InspectionRegion& region = inspection.regions[j];

			for (int k = 0; k < region.results.size(); k++){
				const InspectionRegionResult& result = region.results[k];
				if (result.flags & RF_SHOW_IN_SUMMARY)
					results.push_back(result);
			}
		}
	}
}


void CInspectionReportBuilderComp::GetInspectionPageRegionResults(const Inspection& inspection, Results& results) const
{
	for (const InspectionRegion& region : inspection.regions){
		results.append(region.results);
	}
}


void CInspectionReportBuilderComp::AddHeader(const ReportInputData& reportData,
											 const Inspection* inspectionPtr,
											 i2d::CVector2d& topLeft,
											 IReportPage& page) const
{
	QString productName = inspectionPtr ? inspectionPtr->name : reportData.productName;
	istd::IInformationProvider::InformationCategory status = inspectionPtr ? inspectionPtr->status : reportData.partStatus;

	const double tableWidth = 180.0;
	const double tableHeight = 16.0;

	QByteArray uuid = page.AddTextTable(i2d::CRectangle(topLeft.GetX(), topLeft.GetY(), tableWidth, tableHeight), 2, 3);

	CTextTable* tablePtr = dynamic_cast<CTextTable*>(page.GetPageElement(uuid));
	Q_ASSERT(tablePtr != NULL);

	tablePtr->ShowHorizontalHeader(false);
	tablePtr->ShowVerticalHeader(false);
	tablePtr->SetColumnWidths(QVector<double>(tablePtr->GetColumnCount(), tableWidth / tablePtr->GetColumnCount()));

	tablePtr->SetItem(0, 0, CTextTableItem(reportData.companyName));
	tablePtr->SetItem(0, 1, CTextTableItem(reportData.partSerialNumber));
	tablePtr->SetItem(0, 2, CTextTableItem(productName));

	tablePtr->SetItem(1, 0, CTextTableItem(reportData.appVersion));
	tablePtr->SetItem(1, 1, CTextTableItem(reportData.time.toString(Qt::DateFormat::SystemLocaleShortDate)));
	tablePtr->SetItem(1, 2, CTextTableItem(GetStatusText(status), Qt::AlignLeft, CFont("Arial", 10.0, CFont::FF_BOLD), GetStatusColor(status)));

	topLeft.SetY(topLeft.GetY() + tableHeight);
}


void CInspectionReportBuilderComp::AddBody(const QString& imagePath, i2d::CVector2d& topLeft, IReportPage& page) const
{
	topLeft.SetY(topLeft.GetY() + 15.0);

	const QSize imageSize(180.0, 120.0);
	page.AddImage(imagePath, i2d::CRectangle(topLeft.GetX(), topLeft.GetY(), imageSize.width(), imageSize.height()));

	topLeft.SetY(topLeft.GetY() + imageSize.height());
}


void CInspectionReportBuilderComp::AddTitlePageFooter(const ReportInputData& reportData, i2d::CVector2d& topLeft, IReportPage& page) const
{
	Results results;
	GetTitlePageRegionResults(reportData, results);

	if (results.isEmpty()){
		return;
	}

	topLeft.SetY(topLeft.GetY() + 15.0);

	const double tableWidth = 180.0;
	const double tableHeight = 100.0;

	QByteArray uuid = page.AddTextTable(i2d::CRectangle(topLeft.GetX(), topLeft.GetY(), tableWidth, tableHeight), results.size(), 7);

	CTextTable* tablePtr = dynamic_cast<CTextTable*>(page.GetPageElement(uuid));
	Q_ASSERT(tablePtr);

	tablePtr->SetHorizontalHeaderLabels({
				QObject::tr("Inspection"),
				QObject::tr("Region"),
				QObject::tr("Error"),
				QObject::tr("Length"),
				QObject::tr("Value mm"),
				QObject::tr("Tolerance mm"),
				QObject::tr("Diff") });
	tablePtr->ShowVerticalHeader(false);
	tablePtr->SetColumnWidths({40.0, 40.0, 20.0, 20.0, 20.0, 20.0, 20.0 });

	for (int i = 0; i < results.size(); i++){
		const InspectionRegionResult& result = results[i];

		tablePtr->SetItem(i, 0, CTextTableItem(result.inspectionName));
		tablePtr->SetItem(i, 1, CTextTableItem(result.regionName));
		tablePtr->SetItem(i, 2, CTextTableItem(GetErrorClassText(result.errorClass)));
		tablePtr->SetItem(i, 3, CTextTableItem(QString::number(result.length, 'f', 2), Qt::AlignRight));

		if (result.errorClass != IInspectionReportBuilder::EC_NONE){
			tablePtr->SetItem(i, 4, CTextTableItem(QString::number(result.value, 'f', 2), Qt::AlignRight));
			tablePtr->SetItem(i, 5, CTextTableItem(QString::number(result.tolerance, 'f', 2), Qt::AlignRight));
			tablePtr->SetItem(i, 6, CTextTableItem(QString::number(result.diff, 'f', 2), Qt::AlignRight));
		}
	}
}


void CInspectionReportBuilderComp::AddInspectionPageFooter(const Inspection& inspection, i2d::CVector2d& topLeft, IReportPage& page) const
{
	Results results;
	GetInspectionPageRegionResults(inspection, results);

	if (results.isEmpty()){
		return;
	}

	topLeft.SetY(topLeft.GetY() + 15.0);

	const double tableWidth = 180.0;
	const double tableHeight = 100.0;

	QByteArray uuid = page.AddTextTable(i2d::CRectangle(topLeft.GetX(), topLeft.GetY(), tableWidth, tableHeight), results.size(), 6);

	CTextTable* tablePtr = dynamic_cast<CTextTable*>(page.GetPageElement(uuid));
	Q_ASSERT(tablePtr);

	tablePtr->SetHorizontalHeaderLabels({
				QObject::tr("Region"),
				QObject::tr("Error"),
				QObject::tr("Length"),
				QObject::tr("Value mm"),
				QObject::tr("Tolerance mm"),
				QObject::tr("Diff")});
	tablePtr->ShowVerticalHeader(false);
	tablePtr->SetColumnWidths({55.0, 25.0, 25.0, 25.0, 25.0, 25.0});

	for (int i = 0; i < results.size(); i++){
		const InspectionRegionResult& result = results[i];

		tablePtr->SetItem(i, 0, CTextTableItem(result.regionName));
		tablePtr->SetItem(i, 1, CTextTableItem(GetErrorClassText(result.errorClass)));
		tablePtr->SetItem(i, 2, CTextTableItem(QString::number(result.length, 'f', 2), Qt::AlignRight));

		if (result.errorClass != IInspectionReportBuilder::EC_NONE){
			tablePtr->SetItem(i, 3, CTextTableItem(QString::number(result.value, 'f', 2), Qt::AlignRight));
			tablePtr->SetItem(i, 4, CTextTableItem(QString::number(result.tolerance, 'f', 2), Qt::AlignRight));
			tablePtr->SetItem(i, 5, CTextTableItem(QString::number(result.diff, 'f', 2), Qt::AlignRight));
		}
	}
}


} // namespace imtreport


