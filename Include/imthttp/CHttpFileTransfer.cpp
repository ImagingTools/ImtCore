// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthttp/CHttpFileTransfer.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

// ImtCore includes
#include <imtcom/CRequestSender.h>


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
				replyPtr->deleteLater();

				return replyPtr->error() == QNetworkReply::NoError;
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
			if (replyPtr->error() == QNetworkReply::NoError){
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


