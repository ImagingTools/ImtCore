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
	
	bool retVal = SaveToFile(scenes, filePath);

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


bool CReportDocumentTiffPersistenceComp::SaveToFile(const CReportSceneBuilder::ReportScenes scenes, const QString& filePath) const
{
	Q_ASSERT(!scenes.isEmpty());

	const double interMargin = 10.0;

	QSize sceneSize = scenes[0]->sceneRect().size().toSize();

	int imageWidth = sceneSize.width();
	int imageHeigth = scenes.size() * (sceneSize.height() + interMargin);

	QRectF targetRect(0.0, 0.0, sceneSize.width(), sceneSize.height());

	QImage image(imageWidth, imageHeigth, QImage::Format::Format_ARGB32);
	image.fill(Qt::transparent);

	QPainter painter(&image);

	for (QGraphicsScene* scenePtr : scenes){
		Q_ASSERT(scenePtr != NULL);

		scenePtr->render(&painter, targetRect);

		QImageWriter writer(filePath, "tiff");
		if (!writer.write(image)){
			SendErrorMessage(0, "Failed to export report\r\n" + writer.errorString());
			return false;
		}

		targetRect.setTop(targetRect.top() + targetRect.height() + interMargin);
		targetRect.setHeight(sceneSize.height());
	}

	return true;
}


// reimplemented (ifile::IFileTypeInfo)

bool CReportDocumentTiffPersistenceComp::GetFileExtensions(QStringList& result, const istd::IChangeable* /*dataObjectPtr*/, int /*flags*/, bool doAppend) const
{
	if (!doAppend){
		result.clear();
	}

	result.push_back("tiff");

	return true;
}


QString CReportDocumentTiffPersistenceComp::GetTypeDescription(const QString* /*extensionPtr*/) const
{
	return "Report files";
}


} // namespace imt3d


