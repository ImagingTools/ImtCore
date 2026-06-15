// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpDataUploader.h>


// Qt includes
#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

// ImtCCore includes
#include<imtcom/CRequestSender.h>


namespace imtrest
{


CHttpDataUploader::CHttpDataUploader(QObject* parentPtr)
	:QObject(parentPtr),
	m_progressManager(nullptr),
	m_partMaxSize(1000000)
{
}


void CHttpDataUploader::SetDevice(QIODevice* devicePtr)
{
	if (!m_devicePtr.isNull() && m_devicePtr->parent() == this){
		m_devicePtr->deleteLater();
	}

	Q_ASSERT_X(devicePtr != nullptr, __func__, "NULL device provided!");

	m_devicePtr = devicePtr;
}


bool CHttpDataUploader::InitializeDeviceFromFile(const QString& filePath)
{
	if (!m_devicePtr.isNull() && m_devicePtr->parent() == this){
		m_devicePtr->deleteLater();
	}

	QFile* filePtr = new QFile(filePath, this);
	if (!filePtr->open(QIODevice::ReadOnly)){
		qWarning() << "Unable to open file" << filePtr->fileName() << filePtr->error() << filePtr->errorString();

		return false;
	}

	m_devicePtr = filePtr;

	return true;
}


bool CHttpDataUploader::UploadFile(const QUrl& url)
{
	QUrl uploadUrl = url.isValid() ? url : m_uploadUrl;
	if(!uploadUrl.isValid()){
		qCritical() << __func__ << "Unable to upload data. URL is invalid" << url << m_uploadUrl;
		I_CRITICAL();

		return false;
	}

	if (m_devicePtr.isNull()){
		qWarning() << __func__ << "Unable to upload data. Device is not set.";
		I_CRITICAL();

		return false;
	}

	if (!m_devicePtr->seek(0)){
		qWarning() << __func__ << "Unable to upload data. Unexpected device";
		I_CRITICAL();

		return false;
	}

	qsizetype totalSize = m_devicePtr->size();
	while(!m_devicePtr->atEnd()){
		QNetworkRequest uploadRequest(uploadUrl);
		for(auto headersIter = m_additionalHeaders.cbegin(); headersIter != m_additionalHeaders.cend(); ++headersIter){
			QVariant headerValue = headersIter.value();
			if (!headerValue.canConvert<QByteArray>()){
				qCritical() << "Unexpected header" << headersIter.key() << headersIter.value();
				I_CRITICAL();

				continue;
			}
			uploadRequest.setRawHeader(headersIter.key().toUtf8(), headerValue.toByteArray());
		}
		QByteArray contentRangeHeaderValue = QByteArrayLiteral("bytes ");
		contentRangeHeaderValue += QByteArray::number(m_devicePtr->pos());
		contentRangeHeaderValue += '-';

		QByteArray dataPart = m_devicePtr->read(m_partMaxSize);
		contentRangeHeaderValue += QByteArray::number(dataPart.length());
		contentRangeHeaderValue += '/';
		contentRangeHeaderValue += QByteArray::number(totalSize);

		uploadRequest.setRawHeader(QByteArrayLiteral("Content-Range"), contentRangeHeaderValue);

		QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncPut(uploadRequest, dataPart, 30000);
		Q_ASSERT(replyPtr != nullptr);

		if(replyPtr->error()){
			qWarning() << "Unable to send data. error" << replyPtr->error() << replyPtr->errorString();

			return false;
		}
	}

	return true;
}


void CHttpDataUploader::SetProgressManager(ibase::IProgressManager* progressManager)
{
	m_progressManager = progressManager;
}


qsizetype CHttpDataUploader::GetPartMaxSize() const
{
	return m_partMaxSize;
}


void CHttpDataUploader::SetPartMaxSize(qsizetype partMaxSize)
{
	if (m_partMaxSize == partMaxSize){
		return;
	}

	m_partMaxSize = partMaxSize;
	emit PartMaxSizeChanged();
}


QVariantMap CHttpDataUploader::GetAdditionalHeaders() const
{
	return m_additionalHeaders;
}


void CHttpDataUploader::SetAdditionalHeaders(const QVariantMap& additionalHeaders)
{
	if (m_additionalHeaders == additionalHeaders){
		return;
	}

	m_additionalHeaders = additionalHeaders;
	emit AdditionalHeadersChanged();
}


QUrl CHttpDataUploader::GetUploadUrl() const
{
	return m_uploadUrl;
}


void CHttpDataUploader::SetUploadUrl(const QUrl& uploadUrl)
{
	if (m_uploadUrl == uploadUrl){
		return;
	}

	m_uploadUrl = uploadUrl;
	emit uploadUrlChanged();
}



} // namespace imtrest


