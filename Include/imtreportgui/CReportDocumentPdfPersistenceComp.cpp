#include <imtreportgui/CReportDocumentPdfPersistenceComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtPrintSupport/QPrinter>
#include <QtGui/QPainter>

// ImtCore includes
#include <imtreport/CReportDocument.h>
#include <imtreportgui/CReportSceneBuilder.h>


namespace imtreportgui
{


// reimplemented (ifile::IFilePersistence)

bool CReportDocumentPdfPersistenceComp::IsOperationSupported(
			const istd::IChangeable* dataObjectPtr,
			const QString* filePathPtr,
			int flags,
			bool /*beQuiet*/) const
{
	if ((filePathPtr == NULL) || (flags & QF_LOAD)){
		return false;
	}

	if (dataObjectPtr != NULL){
		const imtreport::CReportDocument* documentPtr = dynamic_cast<const imtreport::CReportDocument*>(dataObjectPtr);
		if (documentPtr == NULL){
			return false;
		}
	}

	return true;
}


int CReportDocumentPdfPersistenceComp::LoadFromFile(
			istd::IChangeable& /*data*/,
			const QString& /*filePath*/,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	return OS_FAILED;
}


int CReportDocumentPdfPersistenceComp::SaveToFile(
			const istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* /*progressManagerPtr*/) const
{
	const imtreport::IReportDocument* documentPtr = dynamic_cast<const imtreport::IReportDocument*>(&data);
	if (documentPtr == NULL){
		return OS_FAILED;
	}

	CReportSceneBuilder::ReportScenes scenes = CReportSceneBuilder::Build(*documentPtr, *m_resolutionDpiAttrPtr);
	if (scenes.isEmpty()){
		SendWarningMessage(0, "Report is empty");
		return OS_OK;
	}

	QPrinter printer(QPrinter::HighResolution);
	printer.setPageSize(QPrinter::A4);
	printer.setOrientation(QPrinter::Portrait);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName(filePath);
	printer.setPageMargins({0.0, 0.0, 0.0, 0.0});

	QPainter painter(&printer);

	bool retVal = true;

	for (int i = 0; i < scenes.count(); ++i){
		QGraphicsScene* scenePtr = scenes[i];
		Q_ASSERT(scenePtr != NULL);

		scenePtr->render(&painter);

		if (i < scenes.count() - 1){ // initially printer already has one default page so don't add a new one on the last iteration
			if (!printer.newPage()){
				SendErrorMessage(0, "Failed to create a new report page");
				retVal = false;
				break;
			}
		}
	}

	for (const QGraphicsScene* scenePtr : scenes)
		delete scenePtr;

	if (retVal){
		SendInfoMessage(0, "Report has been exported successfully");
		return OS_OK;
	}
	else{
		return OS_FAILED;
	}
}


// reimplemented (ifile::IFileTypeInfo)

bool CReportDocumentPdfPersistenceComp::GetFileExtensions(QStringList& result, const istd::IChangeable* /*dataObjectPtr*/, int /*flags*/, bool doAppend) const
{
	if (!doAppend){
		result.clear();
	}

	result.push_back("pdf");

	return true;
}


QString CReportDocumentPdfPersistenceComp::GetTypeDescription(const QString* /*extensionPtr*/) const
{
	return "Report files";
}


} // namespace imt3d


