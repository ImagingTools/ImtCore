#include <imtqml/CRemoteFileController.h>


// Qt includes
#include <QtCore/QUrlQuery>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtQml/QQmlEngine>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QFileInfo>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QtAndroid>
#else
#include <QtGui/QDesktopServices>
#endif


namespace imtqml
{


CRemoteFileController::CRemoteFileController(QObject *parent) : QObject(parent), m_isAutoOpen(true)
{

}


CRemoteFileController::~CRemoteFileController()
{

}


bool CRemoteFileController::OpenFile(const QString& filePath) const
{
	QFileInfo fileInfo(filePath.length() > 2 ? filePath : m_downloadedFilePath);
	if(!fileInfo.exists()){
        qWarning() << "Attempting to aquire a non-existent file" << fileInfo.absoluteFilePath();

		return false;
	}
	if(!fileInfo.isReadable()){
        qWarning() << "Attempting to aquire an unreadable file" << fileInfo.absoluteFilePath();
	}
#ifdef Q_OS_ANDROID
	QString type = "*/*";
	if (filePath.contains(".doc") || filePath.contains(".docx")) {
		// Word document
		type = "application/msword";
	}
	else if (filePath.contains(".pdf")) {
		// PDF file
		type = "application/pdf";
	}
	else if (filePath.contains(".ppt") || filePath.contains(".pptx")) {
		// Powerpoint file
		type = "application/vnd.ms-powerpoint";
	}
	else if (filePath.contains(".xls") || filePath.contains(".xlsx")) {
		// Excel file
		type = "application/vnd.ms-excel";
	}
	else if (filePath.contains(".zip") || filePath.contains(".rar")) {
		// WAV audio file
		type = "application/x-wav";
	}
	else if (filePath.contains(".rtf")) {
		// RTF file
		type = "application/rtf";
	}
	else if (filePath.contains(".wav") || filePath.contains(".mp3")) {
		// WAV audio file
		type = "audio/x-wav";
	}
	else if (filePath.contains(".gif")) {
		// GIF file
		type = "image/gif";
	}
	else if (filePath.contains(".jpg") || filePath.contains(".jpeg") || filePath.contains(".png")) {
		// JPG file
		type = "image/jpeg";
	}
	else if (filePath.contains(".txt")) {
		// Text file
		type = "text/plain";
	}
	else if (filePath.contains(".3gp") || filePath.contains(".mpg") || filePath.contains(".mpeg") || filePath.contains(".mpe") || filePath.contains(".mp4") || filePath.contains(".avi")) {
		// Video files
		type = "video/*";
	}

	QString absoluteFilePath = QUrl::fromLocalFile(fileInfo.absoluteFilePath()).toString();
	QAndroidJniObject jniUri = QAndroidJniObject::callStaticObjectMethod(
				"android/net/Uri",
				"parse",
				"(Ljava/lang/String;)Landroid/net/Uri;",
				QAndroidJniObject::fromString(absoluteFilePath).object());

	QAndroidJniObject jniIntent = QAndroidJniObject("android/content/Intent");
	QAndroidJniObject jniIntentAction = QAndroidJniObject::getStaticObjectField("android/content/Intent","ACTION_VIEW", "Ljava/lang/String;");
	jint jniIntentFlags = QAndroidJniObject::getStaticField<jint>("android/content/Intent","FLAG_ACTIVITY_NEW_TASK");
	jint jniIntentFlags2 = QAndroidJniObject::getStaticField<jint>("android/content/Intent","FLAG_GRANT_READ_URI_PERMISSION");
	jniIntent.callObjectMethod("setAction", "(Ljava/lang/String;)Landroid/content/Intent;", jniIntentAction.object());
	jniIntent.callObjectMethod("addFlags", "(I)Landroid/content/Intent;", jniIntentFlags);
	jniIntent.callObjectMethod("addFlags", "(I)Landroid/content/Intent;", jniIntentFlags2);
	jniIntent.callObjectMethod("setDataAndType",
							   "(Landroid/net/Uri;Ljava/lang/String;)Landroid/content/Intent;",
							   jniUri.object(),
							   QAndroidJniObject::fromString(type).object());
	QtAndroid::androidActivity().callMethod<void>("startActivity", "(Landroid/content/Intent;)V", jniIntent.object());
	return true;
#else
	return QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
#endif
}


bool CRemoteFileController::DeleteFile(const QString& fileId)
{
	QQmlEngine* engine = qmlEngine(this);
	if (engine != nullptr){
		SetState("Loading");
		QNetworkAccessManager* accessManager = engine->networkAccessManager();
		QUrl requestUrl = engine->baseUrl();

		QString path = requestUrl.path();
		path.append(m_prefix).append(fileId);
		requestUrl.setPath(path);

		QNetworkReply* reply = accessManager->deleteResource(QNetworkRequest(requestUrl));
		connect(reply, &QNetworkReply::finished, this, &CRemoteFileController::OnFileDeleted);

		return true;
	}

	return false;
}


bool CRemoteFileController::GetFile(const QString& fileId, const QString& fileName)
{
	m_preferredFileNameForSave = fileName;
	QQmlEngine* engine = qmlEngine(this);
	if (engine != nullptr){
		SetState("Loading");
		QNetworkAccessManager* accessManager = engine->networkAccessManager();
		qDebug() << "baseUrl" << engine->baseUrl();
		QUrl requestUrl = engine->baseUrl();

		QString path = requestUrl.path();
		path.append(m_prefix).append(fileName);
		requestUrl.setPath(path);

		QUrlQuery urlQuery;
		urlQuery.addQueryItem("FileId", fileId);
		requestUrl.setQuery(urlQuery);

		QNetworkReply* reply = accessManager->get(QNetworkRequest(requestUrl));
		connect(reply, &QNetworkReply::downloadProgress, this, &CRemoteFileController::OnProgressChanged);
		connect(reply, &QNetworkReply::finished, this, &CRemoteFileController::OnFileDownloaded);

		return true;
	}

	return false;
}


bool CRemoteFileController::SendFile(const QString& fileUrl)
{
	QQmlEngine* engine = qmlEngine(this);
	if (engine != nullptr){
		SetState("Loading");
		QNetworkAccessManager* accessManager = engine->networkAccessManager();
		qDebug() << "baseUrl" << engine->baseUrl();
		QUrl requestUrl = engine->baseUrl();

		QFileInfo uploadingFileInfo(fileUrl);
		if (!uploadingFileInfo.exists()){
			return false;
		}

		QString fileName = uploadingFileInfo.fileName();

		QFile uploadingFile(uploadingFileInfo.absoluteFilePath());
		uploadingFile.open(QFile::ReadOnly);
		QByteArray payload = uploadingFile.readAll();
		uploadingFile.close();

		QString path = requestUrl.path();
		path.append(m_prefix).append(fileName);
		requestUrl.setPath(path);

		QNetworkRequest request(requestUrl);
		request.setHeader(QNetworkRequest::ContentLengthHeader, payload.size());

		QNetworkReply* reply = accessManager->post(request, payload);
		connect(reply, &QNetworkReply::uploadProgress, this, &CRemoteFileController::OnProgressChanged);
		connect(reply, &QNetworkReply::finished, this, &CRemoteFileController::OnFileUploaded);
		connect(reply, &QNetworkReply::finished, this, &CRemoteFileController::OnFileExists);

		return true;
	}

	return false;
}

void CRemoteFileController::OnFileExists()
{
/*	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if(reply){
		QByteArray representationData = reply->readAll();
		qDebug() << representationData;
		setJson(representationData);
		SetState("Ready");
		if (!reply->error()){
			Q_EMIT fileExists();
		}

		reply->deleteLater();
	}*/
}

void CRemoteFileController::OnFileDeleted()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if(reply){
		QByteArray representationData = reply->readAll();
		qDebug() << representationData;
		setJson(representationData);
		SetState("Ready");
		if (!reply->error()){
			Q_EMIT fileDeleted();
		}
		else {
			Q_EMIT fileDeleteFailed();
		}
		reply->deleteLater();
	}
}


void CRemoteFileController::OnFileDownloaded()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if(reply){
		QByteArray representationData = reply->readAll();
		if(m_downloadedFileLocation.isEmpty()){
			m_downloadedFileLocation = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
		}
		if(!QDir().exists(m_downloadedFileLocation)){
			QDir().mkpath(m_downloadedFileLocation);
		}
		QFile downloadedFile(m_downloadedFileLocation +'/' + m_preferredFileNameForSave);
		QFileInfo fileInfo(downloadedFile);
		if(!QDir().exists(fileInfo.absoluteDir().absolutePath())){
			QDir().mkpath(fileInfo.absoluteDir().absolutePath());
		}
		downloadedFile.open(QFile::WriteOnly);
		downloadedFile.write(representationData);
		downloadedFile.close();
		SetState("Ready");
		if(!reply->error()){
			if (m_isAutoOpen){
				OpenFile(downloadedFile.fileName());
			}
			else{
				Q_EMIT fileDownloaded(downloadedFile.fileName());
			}
		}
		else {
			Q_EMIT fileDownloadFailed();
		}

		reply->deleteLater();
	}
}


void CRemoteFileController::OnFileUploaded()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if(reply){
		QByteArray representationData = reply->readAll();
		setJson(representationData);
		SetState("Ready");
		if (!reply->error()){
			Q_EMIT fileUploaded();
		}
		else {
			Q_EMIT fileUploadFailed();
		}
		reply->deleteLater();
	}
}


void CRemoteFileController::OnProgressChanged(qint64 bytesLoaded, qint64 bytesTotal)
{
	if(bytesLoaded > 0 && bytesTotal > 0){
		qDebug() << __FILE__ << __LINE__ << "File load progress" << bytesLoaded/bytesTotal;
		Q_EMIT progress(bytesLoaded, bytesTotal);
	}
}

const QString& CRemoteFileController::state() const
{
	return m_state;
}


void CRemoteFileController::SetState(const QString& newState)
{
	if (m_state == newState)
		return;
	m_state = newState;
	emit stateChanged();
}


const QString& CRemoteFileController::downloadedFilePath() const
{
	return m_downloadedFilePath;
}


void CRemoteFileController::setDownloadedFilePath(const QString& newDownloadedFilePath)
{
	if (m_downloadedFilePath == newDownloadedFilePath)
		return;
	m_downloadedFilePath = newDownloadedFilePath;
	emit downloadedFilePathChanged();
}


const QString& CRemoteFileController::downloadedFileLocation() const
{
	return m_downloadedFileLocation;
}


void CRemoteFileController::setDownloadedFileLocation(const QString& newDownloadedFileLocation)
{
	if (m_downloadedFileLocation == newDownloadedFileLocation)
		return;
	m_downloadedFileLocation = newDownloadedFileLocation;
	emit downloadedFileLocationChanged();
}


const QByteArray& CRemoteFileController::json() const
{
	return m_json;
}


void CRemoteFileController::setJson(const QByteArray& newJson)
{
	if (m_json == newJson)
		return;
	m_json = newJson;
	emit jsonChanged();
}


const QString& CRemoteFileController::prefix() const
{
	return m_prefix;
}


void CRemoteFileController::setPrefix(const QString& newPrefix)
{
	if (m_prefix == newPrefix)
		return;
	m_prefix = newPrefix;
	emit prefixChanged();
}


} // namespace imtqml


