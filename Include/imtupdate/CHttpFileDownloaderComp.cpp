#include<imtupdate/CHttpFileDownloaderComp.h>

//Qt includes
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>


namespace imtupdate
{

// public methods

// reimplemented (IDownloader)

CHttpFileDownloaderComp::DownloadStatus CHttpFileDownloaderComp::DownloadFile(
			const QString& remoteLink,
			const QString& localFilePath,
			bool contunieDownload,
			ibase::IProgressManager* progressManagerPtr) const
{
	IDownloader::DownloadStatus downloadingStatus = IDownloader::DS_FAILED;

	QStringList dirParts = localFilePath.split('/');
	dirParts.removeLast();
	QString dirPath = dirParts.join('/');
	QDir().mkpath(dirPath);

	QFile downloadingFile(localFilePath);

	if (!contunieDownload && downloadingFile.exists()){
		downloadingFile.remove();
	}

	QUrl downloadUrl(remoteLink);
	QNetworkRequest request(downloadUrl);

	QNetworkAccessManager networkAccessManager;
	QEventLoop downloadLoop;
	QObject::connect(&networkAccessManager, &QNetworkAccessManager::finished, &downloadLoop, &QEventLoop::quit);
	QNetworkReply* replyPtr = networkAccessManager.get(request);
	downloadLoop.exec();

	Q_ASSERT_X (replyPtr != nullptr, __FILE__, "Invlid reply");
	if (replyPtr != nullptr){
		const QByteArray body = replyPtr->readAll();
		if (replyPtr->error()){
			SendErrorMessage(0, replyPtr->errorString());
			qDebug() << body;
		}
		else {
			if (downloadingFile.open(QIODevice::WriteOnly)){
				const qint64 written = downloadingFile.write(body);

				Q_ASSERT_X (written == body.length(), __FILE__, QString("Write error. Length: %1, written: %2").arg(QString::number(written), QString::number(body.length())).toLocal8Bit());
				if (written == body.length()){
					downloadingStatus = IDownloader::DS_OK;
				}
				else {
					SendErrorMessage(0, QString("Write error. Length: %1, written: %2. Error: '%3'")
								.arg(QString::number(written), QString::number(body.length()), downloadingFile.errorString()));
				}
			}
		}
	}


	return downloadingStatus;
}


// protected methods


} // namespace imtupdate

