#include <imtrest/CFileSender.h>


// Qt includes
#include <QtCore/QUrlQuery>
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


CFileSender::CFileSender(QObject *parent) : QObject(parent)
{

}

CFileSender::~CFileSender()
{

}

bool CFileSender::DeleteFile(const QString& fileId)
{
	QQmlEngine* engine = qmlEngine(this);
	if (engine != nullptr){
		SetState("Loading");
		QNetworkAccessManager* accessManager = engine->networkAccessManager();
		qDebug() << "baseUrl" << engine->baseUrl();
		QUrl requestUrl = engine->baseUrl();

		QString path = requestUrl.path();
		path.replace("${fileId}",fileId);
		requestUrl.setPath(path);
		QNetworkReply* reply = accessManager->deleteResource(QNetworkRequest(requestUrl));
		connect(reply, &QNetworkReply::finished, this, &CFileSender::OnFileDeleted);
		return true;
	}

	return false;
}

bool CFileSender::GetFile(const QString& fileId, const QString& fileName)
{
	m_preferredFileNameForSave = fileName;
	QQmlEngine* engine = qmlEngine(this);
	if (engine != nullptr){
		SetState("Loading");
		QNetworkAccessManager* accessManager = engine->networkAccessManager();
		qDebug() << "baseUrl" << engine->baseUrl();
		QUrl requestUrl = engine->baseUrl();

		QString path = requestUrl.path();
		path.append("/").append(fileName);
		requestUrl.setPath(path);

		QUrlQuery urlQuery;
		urlQuery.addQueryItem("FileId", fileId);
		requestUrl.setQuery(urlQuery);

		QNetworkReply* reply = accessManager->get(QNetworkRequest(requestUrl));
		connect(reply, &QNetworkReply::finished, this, &CFileSender::OnFileDownloaded);
		return true;
	}
	return false;
}

bool CFileSender::SendFile(const QString& fileUrl)
{
	QQmlEngine* engine = qmlEngine(this);
	if (engine != nullptr){
		SetState("Loading");
		QNetworkAccessManager* accessManager = engine->networkAccessManager();
		qDebug() << "baseUrl" << engine->baseUrl();
		QUrl requestUrl = engine->baseUrl();

		QFileInfo uploadingFileInfo(fileUrl);
		QString fileName = uploadingFileInfo.completeBaseName();

		QFile uploadingFile(uploadingFileInfo.absoluteFilePath());
		uploadingFile.open(QFile::ReadOnly);
		QByteArray payload = uploadingFile.readAll();
		uploadingFile.close();

		QString path = requestUrl.path();
		path.append("/").append(fileName);
		requestUrl.setPath(path);

		QNetworkRequest request(requestUrl);
		request.setHeader(QNetworkRequest::ContentLengthHeader, payload.size());

		QNetworkReply* reply = accessManager->post(request, payload);
		connect(reply, &QNetworkReply::finished, this, &CFileSender::OnFileUploaded);
		return true;
	}
	return false;
}



void CFileSender::OnFileDeleted()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if(reply){
		QByteArray representationData = reply->readAll();
		qDebug() << representationData;
		SetState("Ready");
		reply->deleteLater();
	}
}

void CFileSender::OnFileDownloaded()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if(reply){
		QByteArray representationData = reply->readAll();
		QFile downloadedFile(m_downloadedFileLocation +'/' + m_preferredFileNameForSave);
		downloadedFile.open(QFile::WriteOnly);
		downloadedFile.write(representationData);
		downloadedFile.close();
		SetState("Ready");
		reply->deleteLater();
	}
}

void CFileSender::OnFileUploaded()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	if(reply){
		QByteArray representationData = reply->readAll();
		SetState("Ready");
		reply->deleteLater();
		qDebug() << "File id is: "<< representationData;
	}
}



/*
function SendFile(fileUrl){
	//        console.log("SendFile", fileUrl)
	this.state = "Loading"
			var xhr = new XMLHttpRequest;

	let reader = new FileReader()
			reader.readAsArrayBuffer(fileUrl)

			reader.onload = ()=>{
			//            xhr.open("POST", `../../files?name=${fileUrl.name}`);
			xhr.open("POST", `../../files/${fileUrl.name}`);
			xhr.send(reader.result)
}

			xhr.onreadystatechange = () => {
			if (xhr.readyState === XMLHttpRequest.DONE){
			this.json = xhr.responseText;
			this.state = "Ready"
		}
	}
	xhr.onprogress = (event)=>{
		this.progress(event.loaded, event.total)
	}

}




function GetFile(fileId, fileUrl)
{
	open(`../../get_files/${fileUrl.name}?FileId=${fileId}`)
}

function DeleteFile(fileId)
{
	this.state = "Loading"
	var xhr = new XMLHttpRequest;
	xhr.open("DELETE", `../../files/${fileId}`);
	xhr.send(fileId)

	xhr.onreadystatechange = () => {
		if (xhr.readyState === XMLHttpRequest.DONE){
			this.json = xhr.responseText;
			this.state = "Done"
		}
	}
}
*/

const QString& CFileSender::state() const
{
	return m_state;
}


void CFileSender::SetState(const QString& newState)
{
	if (m_state == newState)
		return;
	m_state = newState;
	emit stateChanged();
}

const QString& CFileSender::downloadedFilePath() const
{
	return m_downloadedFilePath;
}

void CFileSender::setDownloadedFilePath(const QString& newDownloadedFilePath)
{
	if (m_downloadedFilePath == newDownloadedFilePath)
		return;
	m_downloadedFilePath = newDownloadedFilePath;
	emit downloadedFilePathChanged();
}

const QString& CFileSender::downloadedFileLocation() const
{
	return m_downloadedFileLocation;
}

void CFileSender::setDownloadedFileLocation(const QString& newDownloadedFileLocation)
{
	if (m_downloadedFileLocation == newDownloadedFileLocation)
		return;
	m_downloadedFileLocation = newDownloadedFileLocation;
	emit downloadedFileLocationChanged();
}


} // namespace imtrest


