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

	for (const InspectionInfo& inspection : inputData.inspections){
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

	i2d::CVector2d topLeft(15.0, 10.0);

	AddHeader(reportData, 60.0, QFont("Arial", 3), topLeft, *pagePtr);
	AddBody(reportData, topLeft, *pagePtr);
	AddFooter(reportData, 30.0, QFont("Arial", 3), topLeft, *pagePtr);

	return true;
}


bool CInspectionReportBuilderComp::CreateInspectionPage(const ReportInputData& reportData, const InspectionInfo& inspection, IReportDocument& reportDocument) const
{
	IReportPage* pagePtr = dynamic_cast<IReportPage*>(reportDocument.InsertPage());
	if (!pagePtr) {
		return false;
	}

	pagePtr->AddText("Date: " + reportData.time.date().toString(Qt::DateFormat::ISODate),
		i2d::CVector2d(30.0, 30.0));

	pagePtr->AddText("Time: " + reportData.time.time().toString(Qt::DateFormat::ISODate),
		i2d::CVector2d(30.0, 35.0));

	pagePtr->AddText("Product Date (Serial ID): " + reportData.partSerialNumber,
		i2d::CVector2d(30.0, 45.0));

	pagePtr->AddText("State: " + GetStatusText(inspection.status),
		i2d::CVector2d(120.0, 30.0));

	return true;
}


void CInspectionReportBuilderComp::AddHeader(const ReportInputData& reportData,
											 const double cellWidth,
											 const QFont& font,
											 i2d::CVector2d& topLeft,
											 IReportPage& page) const
{
	Table table(2);
	table[0] =
	{
		{ reportData.companyName, Qt::black },
		{ reportData.appVersion, Qt::black },
		{ reportData.productName, Qt::black }
	};
	table[1] =
	{
		{ reportData.time.toString(Qt::DateFormat::SystemLocaleShortDate), Qt::black },
		{ reportData.partSerialNumber, Qt::black },
		{ GetStatusText(reportData.partStatus), GetStatusColor(reportData.partStatus) }
	};

	AddTable(table, cellWidth, font, topLeft, page);
}


void CInspectionReportBuilderComp::AddBody(const ReportInputData& reportData, i2d::CVector2d& topLeft, IReportPage& page) const
{
	topLeft.SetY(topLeft.GetY() + 15.0);

	const QSize imageSize(180.0, 176.0);
	page.AddImage(reportData.imagePath, i2d::CRectangle(topLeft.GetX(), topLeft.GetY(), imageSize.width(), imageSize.height()));

	topLeft.SetY(topLeft.GetY() + imageSize.height());
}


void CInspectionReportBuilderComp::AddFooter(const ReportInputData& reportData,
											 const double cellWidth,
											 const QFont& font,
											 i2d::CVector2d& topLeft,
											 IReportPage& page) const
{
	topLeft.SetY(topLeft.GetY() + 15.0);

	if (reportData.inspections.empty() ||
		reportData.inspections[0].regions.empty() ||
		reportData.inspections[0].regions[0].results.empty()) {
		return;
	}

	const InspectionRegion& region = reportData.inspections[0].regions[0];
	int rows = qMin(region.results.size(), s_maxSummaryResults) + 1;

	Table table(rows);
	table[0] =
	{
		{ "Region",			Qt::black },
		{ "Error",			Qt::black },
		{ "Length",			Qt::black },
		{ "Value mm",		Qt::black },
		{ "Tolerance mm",	Qt::black },
		{ "Diff",			Qt::black }
	};

	for (int row = 1; row < rows; row++) {
		const InspectionRegionResult& regionResult = region.results[row - 1];
		table[row] =
		{
			{ region.name,                                     Qt::black },
			{ GetErrorClassText(regionResult.errorClass),      Qt::black },
			{ QString::number(regionResult.length, 'g', 2),    Qt::black },
			{ QString::number(regionResult.value, 'g', 2),     Qt::black },
			{ QString::number(regionResult.tolerance, 'g', 2), Qt::black },
			{ QString::number(regionResult.diff, 'g', 2),      Qt::black }
		};
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
			textLabelPtr->SetFillColor(table[row][col].second);

			page.AddRectangle(i2d::CRectangle(cellTopLeft.GetX(), cellTopLeft.GetY(), colWidth, cellHeight));
		}
	}

	topLeft.SetY(topLeft.GetY() + table.size() * cellHeight);
}


} // namespace imtreport


