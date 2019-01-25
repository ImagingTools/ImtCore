#include <imtreport/CInspectionReportBuilderComp.h>


// Qt includes
#include <QtGui/QFontMetrics>

// ACF includes
#include <i2d/CRectangle.h>

// ImtCore includes
#include <imtreport/TGraphicsElement.h>
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


QColor CInspectionReportBuilderComp::GetStatusColor(istd::IInformationProvider::InformationCategory status)
{
	switch (status){
	case istd::IInformationProvider::InformationCategory::IC_CRITICAL:
		return Qt::darkRed;

	case istd::IInformationProvider::InformationCategory::IC_ERROR:
		return Qt::red;

	case istd::IInformationProvider::InformationCategory::IC_INFO:
		return Qt::darkGreen;

	case istd::IInformationProvider::InformationCategory::IC_WARNING:
		return qRgb(128, 128, 0);

	default:
		return Qt::gray;
	}
}


QString CInspectionReportBuilderComp::GetErrorClassText(const ErrorClass errorClass)
{
	switch (errorClass){
	case ErrorClass::EC_GLUE_GAP:
		return "Gap";

	case ErrorClass::EC_GLUE_NARROW:
		return "Narrow";

	case ErrorClass::EC_GLUE_WIDE:
		return "Wide";

	default:
		return QString();
	}
}


// private methods

bool CInspectionReportBuilderComp::CreateTitlePage(const ReportInputData& reportData, IReportDocument& reportDocument) const
{
	IReportPage* pagePtr = dynamic_cast<IReportPage*>(reportDocument.InsertPage());
	if (!pagePtr) {
		return false;
	}

	Results results;
	GetTitlePageRegionResults(reportData, results);

	i2d::CVector2d topLeft(15.0, 10.0);

	AddHeader(reportData, NULL, 60.0, QFont("Arial", 3), topLeft, *pagePtr);
	AddBody(reportData.imagePath, topLeft, *pagePtr);
	AddFooter(results, 30.0, QFont("Arial", 3), topLeft, *pagePtr);

	return true;
}


bool CInspectionReportBuilderComp::CreateInspectionPage(const ReportInputData& reportData, const Inspection& inspection, IReportDocument& reportDocument) const
{
	IReportPage* pagePtr = dynamic_cast<IReportPage*>(reportDocument.InsertPage());
	if (!pagePtr) {
		return false;
	}

	Results results;
	GetInspectionPageRegionResults(inspection, results);

	i2d::CVector2d topLeft(15.0, 10.0);

	AddHeader(reportData, &inspection, 60.0, QFont("Arial", 3), topLeft, *pagePtr);
	AddBody(inspection.imagePath, topLeft, *pagePtr);
	AddFooter(results, 30.0, QFont("Arial", 3), topLeft, *pagePtr);

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
	for (const InspectionRegion& region : inspection.regions) {
		results.append(region.results);
	}
}


void CInspectionReportBuilderComp::AddHeader(const ReportInputData& reportData,
											 const Inspection* inspectionPtr,
											 const double cellWidth,
											 const QFont& font,
											 i2d::CVector2d& topLeft,
											 IReportPage& page) const
{
	QString productName = inspectionPtr ? inspectionPtr->name : reportData.productName;
	istd::IInformationProvider::InformationCategory status = inspectionPtr ? inspectionPtr->status : reportData.partStatus;

	Table table(2);
	table[0] =
	{
		{ reportData.companyName, Qt::black },
		{ reportData.appVersion, Qt::black },
		{ inspectionPtr->name, Qt::black }
	};
	table[1] =
	{
		{ reportData.time.toString(Qt::DateFormat::SystemLocaleShortDate), Qt::black },
		{ reportData.partSerialNumber, Qt::black },
		{ GetStatusText(status), GetStatusColor(status) }
	};

	AddTable(table, cellWidth, font, topLeft, page);
}


void CInspectionReportBuilderComp::AddBody(const QString& imagePath, i2d::CVector2d& topLeft, IReportPage& page) const
{
	topLeft.SetY(topLeft.GetY() + 15.0);

	const QSize imageSize(180.0, 120.0);
	page.AddImage(imagePath, i2d::CRectangle(topLeft.GetX(), topLeft.GetY(), imageSize.width(), imageSize.height()));

	topLeft.SetY(topLeft.GetY() + imageSize.height());
}


void CInspectionReportBuilderComp::AddFooter(const Results& results,
											 const double cellWidth,
											 const QFont& font,
											 i2d::CVector2d& topLeft,
											 IReportPage& page) const
{
	if (results.isEmpty())
		return;

	topLeft.SetY(topLeft.GetY() + 15.0);

	Table table;
	table.push_back(
	{
		{ "Region",			Qt::black },
		{ "Error",			Qt::black },
		{ "Length",			Qt::black },
		{ "Value mm",		Qt::black },
		{ "Tolerance mm",	Qt::black },
		{ "Diff",			Qt::black }
	});

	for (const InspectionRegionResult& result : results){
		table.push_back(
		{
			{ result.regionName,                         Qt::black },
			{ GetErrorClassText(result.errorClass),      Qt::black },
			{ QString::number(result.length, 'f', 2),    Qt::black },
			{ QString::number(result.value, 'f', 2),     Qt::black },
			{ QString::number(result.tolerance, 'f', 2), Qt::black },
			{ QString::number(result.diff, 'f', 2),      Qt::black }
		});
	}

	AddTable(table, cellWidth, font, topLeft, page);
}


void CInspectionReportBuilderComp::AddTable(const Table& table, const double colWidth, const QFont& font, i2d::CVector2d& topLeft, IReportPage& page) const
{
	int cellHeight = QFontMetrics(font).height() + 2;

	for (int row = 0; row < table.size(); row++){
		for (int col = 0; col < table[row].size(); col++){
			i2d::CVector2d cellTopLeft(topLeft.GetX() + col * colWidth, topLeft.GetY() + row * cellHeight);
			QByteArray uuid = page.AddText(table[row][col].first, cellTopLeft);

			CTextLabelElement* textLabelPtr = dynamic_cast<CTextLabelElement*>(page.GetPageElement(uuid));
			Q_ASSERT(textLabelPtr);
			textLabelPtr->SetFontName(font.family());
			textLabelPtr->SetFontSize(font.pointSize());
			textLabelPtr->SetFillColor(table[row][col].second);

			page.AddRectangle(i2d::CRectangle(cellTopLeft.GetX(), cellTopLeft.GetY(), colWidth, cellHeight));
		}
	}

	topLeft.SetY(topLeft.GetY() + table.size() * cellHeight);
}


} // namespace imtreport


