// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpDataUploader.h>


// Qt includes
#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

// ImtCCore includes
#include<imtcom/CRequestSender.h>
#include <imtrest/CHttpRangeTools.h>


namespace imtrest
{


namespace
{


bool SendPart(const QNetworkRequest& request, const QByteArray& data)
{
	QNetworkReply* replyPtr = imtcom::CRequestSender::DoSyncPut(request, data, 30000);
	if (replyPtr == nullptr){
		return false;
	}

	// The reply belongs to the caller of DoSyncPut.
	replyPtr->deleteLater();

	if (replyPtr->error() != QNetworkReply::NoError){
		qWarning() << "Unable to send data. error" << replyPtr->error() << replyPtr->errorString();

		return false;
	}

	return true;
}


} // namespace


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

	// The part size is a runtime property (QML), so it is a normal error and not a programming one.
	if (m_partMaxSize <= 0){
		qWarning() << __func__ << "Unable to upload data. Invalid part size" << m_partMaxSize;

		return false;
	}

	qint64 totalSize = m_devicePtr->size();
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

		qint64 partStart = m_devicePtr->pos();
		QByteArray dataPart = m_devicePtr->read(m_partMaxSize);
		if (dataPart.isEmpty()){
			qWarning() << __func__ << "Unable to read data from the device at" << partStart;

			return false;
		}

		const QByteArray contentRange = CHttpRangeTools::CreateContentRange(partStart, dataPart.size(), totalSize);
		if (contentRange.isEmpty()){
			// The device has grown since the upload was started.
			qWarning() << __func__ << "Unable to upload data. The device is changed during the upload";

			return false;
		}

		uploadRequest.setRawHeader(QByteArrayLiteral("Content-Range"), contentRange);

		if (!SendPart(uploadRequest, dataPart)){
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


