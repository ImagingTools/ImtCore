#include <imtrest/CRemoteFileController.h>


// Qt includes
#include <QtCore/QUrlQuery>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlEngine>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QFileInfo>

// ACF includes
#include <iser/CJsonStringReadArchive.h>



#include <QtCore/QAbstractListModel>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QtDebug>

// ACF includes
#include <iser/ISerializable.h>
#include <imod/CModelUpdateBridge.h>
#include <imod/CModelBase.h>



#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CJsonStringWriteArchive.h>
#include <istd/CChangeNotifier.h>
#include <istd/TSmartPtr.h>

namespace imtrest
{


CRemoteFileController::CRemoteFileController(QObject *parent) : QObject(parent)
{

}

CRemoteFileController::~CRemoteFileController()
{

}

bool CRemoteFileController::DeleteFile(const QString& fileId)
{
	QQmlEngine* engine = qmlEngine(this);
	if (engine != nullptr){
		SetState("Loading");
		QNetworkAccessManager* accessManager = engine->networkAccessManager();
		QUrl requestUrl = engine->baseUrl();

		QString path = requestUrl.path();
		path.append("/files/").append(fileId);
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
		path.append("/files/").append(fileName);
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
		QString fileName = uploadingFileInfo.fileName();

		QFile uploadingFile(uploadingFileInfo.absoluteFilePath());
		uploadingFile.open(QFile::ReadOnly);
		QByteArray payload = uploadingFile.readAll();
		uploadingFile.close();

		QString path = requestUrl.path();
		path.append("/files/").append(fileName);
		requestUrl.setPath(path);

		QNetworkRequest request(requestUrl);
		request.setHeader(QNetworkRequest::ContentLengthHeader, payload.size());

        QNetworkReply* reply = accessManager->post(request, payload);
        connect(reply, &QNetworkReply::uploadProgress, this, &CRemoteFileController::OnProgressChanged);
        connect(reply, &QNetworkReply::finished, this, &CRemoteFileController::OnFileUploaded);
		return true;
	}
	return false;
}



void CRemoteFileController::OnFileDeleted()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if(reply){
		QByteArray representationData = reply->readAll();
		qDebug() << representationData;
		setJson(representationData);
		SetState("Ready");
		Q_EMIT fileDeleted();
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
		downloadedFile.open(QFile::WriteOnly);
		downloadedFile.write(representationData);
		downloadedFile.close();
		Q_EMIT fileDownloaded(downloadedFile.fileName());
		SetState("Ready");
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
		reply->deleteLater();
		Q_EMIT fileUploaded();
		qDebug() << "File id is: "<< representationData;
	}
}

void CRemoteFileController::OnProgressChanged(qint64 bytesLoaded, qint64 bytesTotal)
{
    qDebug() << __FILE__ << __LINE__ << "File load progress" << bytesLoaded/bytesTotal;
    Q_EMIT progress(bytesLoaded, bytesTotal);
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


} // namespace imtrest


