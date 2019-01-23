#include <imtreport/CInspectionReportBuilderComp.h>


// Qt includes
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>

// ACF includes
#include <i2d/CRectangle.h>
#include <imtreport/TGraphicsElement.h>

// ImtCore includes
#include <imtreport/IReportDocument.h>
#include <imtreport/IReportPage.h>


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
	switch (status) {
	case istd::IInformationProvider::InformationCategory::IC_CRITICAL:
		return Qt::darkRed;

	case istd::IInformationProvider::InformationCategory::IC_ERROR:
		return Qt::red;

	case istd::IInformationProvider::InformationCategory::IC_INFO:
		return Qt::darkGreen;

	case istd::IInformationProvider::InformationCategory::IC_WARNING:
		return qRgb(200, 200, 0);

	default:
		return Qt::gray;
	}
}


// private methods

bool CInspectionReportBuilderComp::CreateTitlePage(const ReportInputData& reportData, IReportDocument& reportDocument) const
{
	IReportPage* pagePtr = dynamic_cast<IReportPage*>(reportDocument.InsertPage());
	if (!pagePtr) {
		return false;
	}

	double leftOffset = 15;
	double topOffset = 10.0;
	QFont headerFont("Arial", 3);
	double headerCellWidth = 60.0;
	int headerCellHeight = QFontMetrics(headerFont).height() + 2;

	// 1st header line
	int colIndex = 0;
	int rowIndex = 0;

	QByteArray uuid = pagePtr->AddText(reportData.companyName, i2d::CVector2d(colIndex * headerCellWidth + leftOffset, rowIndex * headerCellHeight + topOffset));
	CTextLabelElement* textLabelPtr = dynamic_cast<CTextLabelElement*>(pagePtr->GetPageElement(uuid));
	Q_ASSERT(textLabelPtr);
	textLabelPtr->SetFontSize(headerFont.pointSize());

	colIndex++;
	uuid = pagePtr->AddText(reportData.appVersion, i2d::CVector2d(colIndex * headerCellWidth + leftOffset, rowIndex * headerCellHeight + topOffset));
	textLabelPtr = dynamic_cast<CTextLabelElement*>(pagePtr->GetPageElement(uuid));
	Q_ASSERT(textLabelPtr);
	textLabelPtr->SetFontSize(headerFont.pointSize());

	colIndex++;
	uuid = pagePtr->AddText(reportData.productName, i2d::CVector2d(colIndex * headerCellWidth + leftOffset, rowIndex * headerCellHeight + topOffset));
	textLabelPtr = dynamic_cast<CTextLabelElement*>(pagePtr->GetPageElement(uuid));
	Q_ASSERT(textLabelPtr);
	textLabelPtr->SetFontSize(headerFont.pointSize());

	for (int i = 0; i < colIndex + 1; i++){
		pagePtr->AddRectangle(i2d::CRectangle(i * headerCellWidth + leftOffset, topOffset, headerCellWidth, headerCellHeight));
	}

	// 2nd header line
	rowIndex++;
	colIndex = 0;
	uuid = pagePtr->AddText(reportData.time.toString(Qt::DateFormat::SystemLocaleShortDate), i2d::CVector2d(colIndex * headerCellWidth + leftOffset, rowIndex * headerCellHeight + topOffset));
	textLabelPtr = dynamic_cast<CTextLabelElement*>(pagePtr->GetPageElement(uuid));
	Q_ASSERT(textLabelPtr);
	textLabelPtr->SetFontSize(headerFont.pointSize());

	colIndex++;
	uuid = pagePtr->AddText(reportData.partSerialNumber, i2d::CVector2d(colIndex * headerCellWidth + leftOffset, rowIndex * headerCellHeight + topOffset));
	textLabelPtr = dynamic_cast<CTextLabelElement*>(pagePtr->GetPageElement(uuid));
	Q_ASSERT(textLabelPtr);
	textLabelPtr->SetFontSize(headerFont.pointSize());

	colIndex++;
	uuid = pagePtr->AddText(GetStatusText(reportData.partStatus), i2d::CVector2d(colIndex * headerCellWidth + leftOffset, rowIndex * headerCellHeight + topOffset));
	textLabelPtr = dynamic_cast<CTextLabelElement*>(pagePtr->GetPageElement(uuid));
	Q_ASSERT(textLabelPtr);
	textLabelPtr->SetFontSize(headerFont.pointSize());
	textLabelPtr->SetFillColor(GetStatusColor(reportData.partStatus));
	textLabelPtr->SetStrokeColor(GetStatusColor(reportData.partStatus));

	for (int i = 0; i < colIndex + 1; i++) {
		pagePtr->AddRectangle(i2d::CRectangle(i * headerCellWidth + leftOffset, rowIndex * headerCellHeight + topOffset, headerCellWidth, headerCellHeight));
	}

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


} // namespace imtreport


