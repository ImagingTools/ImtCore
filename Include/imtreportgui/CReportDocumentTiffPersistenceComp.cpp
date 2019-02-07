#include <imtreportgui/CReportDocumentTiffPersistenceComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtPrintSupport/QPrinter>
#include <QtGui/QPainter>
#include <QtGui/QImageWriter>

// ImtCore includes
#include <imtreport/CReportDocument.h>


namespace imtreportgui
{


// reimplemented (ifile::IFilePersistence)

bool CReportDocumentTiffPersistenceComp::IsOperationSupported(
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


int CReportDocumentTiffPersistenceComp::LoadFromFile(
	istd::IChangeable& /*data*/,
	const QString& /*filePath*/,
	ibase::IProgressManager* /*progressManagerPtr*/) const
{
	return OS_FAILED;
}


int CReportDocumentTiffPersistenceComp::SaveToFile(const istd::IChangeable& data, const QString& filePath, ibase::IProgressManager* /*progressManagerPtr*/) const
{
	const imtreport::IReportDocument* documentPtr = dynamic_cast<const imtreport::IReportDocument*>(&data);
	if (documentPtr == NULL){
		return OS_FAILED;
	}

	CReportSceneBuilder::ReportScenes scenes = CReportSceneBuilder::Build(*documentPtr);
	if (scenes.isEmpty()){
		SendWarningMessage(0, "Report is empty");
		return OS_OK;
	}
	
	OperationState retVal = SaveToFile(scenes, filePath);

	for (const QGraphicsScene* scenePtr : scenes){
		delete scenePtr;
	}

	return retVal;
}


ifile::IFilePersistence::OperationState CReportDocumentTiffPersistenceComp::SaveToFile(const CReportSceneBuilder::ReportScenes scenes, const QString& filePath) const
{
	Q_ASSERT(!scenes.isEmpty());

	QFileInfo fileInfo(filePath);

	for (int i = 0; i < scenes.size(); ++i){
		QGraphicsScene* scenePtr = scenes[i];
		Q_ASSERT(scenePtr != NULL);

		QImage image(scenePtr->sceneRect().size().toSize(), QImage::Format::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		scenePtr->render(&painter);

		QString imgFilePath = filePath;

		QString fileCounter;
		if (!*m_saveFirstPageOnlyAttrPtr){
			fileCounter = QString("%1").arg(i + 1, 2, 10, QChar('0'));
		}

		imgFilePath = fileInfo.path() + "/" +
					fileInfo.baseName() +
					fileCounter + "." +
					fileInfo.completeSuffix();

		QImageWriter writer(imgFilePath, "tiff");
		if (!writer.write(image)){
			SendErrorMessage(0, "Failed to export report\r\n" + writer.errorString());

			return OS_FAILED;
		}

		if (*m_saveFirstPageOnlyAttrPtr){
			break;
		}
	}

	SendInfoMessage(0, "Report has been exported successfully");

	return OS_OK;
}


// reimplemented (ifile::IFileTypeInfo)

bool CReportDocumentTiffPersistenceComp::GetFileExtensions(QStringList& result, const istd::IChangeable* /*dataObjectPtr*/, int /*flags*/, bool doAppend) const
{
	if (!doAppend){
		result.clear();
	}

	result.push_back("tiff");
	result.push_back("tif");

	return true;
}


QString CReportDocumentTiffPersistenceComp::GetTypeDescription(const QString* /*extensionPtr*/) const
{
	return "Report files";
}


} // namespace imt3d


