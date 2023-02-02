#pragma once


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtcom/CRequestSender.h>
#include <imthttp/CHttpFileTransfer.h>


namespace imthttp
{


// reimplemented (IFileTransferDelegate)

bool CHttpFileTransfer::UploadFile(const QString& filePath, const QUrl& url) const
{
	if (!url.isEmpty()){
		QFile file(filePath);
		if (file.open(QIODevice::ReadOnly)){
			QByteArray data = file.readAll();

			QNetworkRequest request;
			request.setUrl(url);
			QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncPut(request, data, 30000);
			if (replyPtr != nullptr){
				if (!replyPtr->error()){
					replyPtr->deleteLater();

					return true;
				}

				replyPtr->deleteLater();
			}
		}
	}

	return false;
}


bool CHttpFileTransfer::DownloadFile(const QString& filePath, const QUrl& url) const
{
	if (!url.isEmpty()){
		QNetworkRequest request;
		request.setUrl(url);
		QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncGet(request, 30000);
		if (replyPtr != nullptr){
			if (!replyPtr->error()){
				QFile file(filePath);
				if (file.open(QIODevice::WriteOnly)){
					QByteArray data = replyPtr->readAll();
					qint64 count = file.write(data);
					file.close();

					replyPtr->deleteLater();

					return data.size() == count;
				}
			}

			replyPtr->deleteLater();
		}
	}

	return false;
}


} // namespace imthttp


