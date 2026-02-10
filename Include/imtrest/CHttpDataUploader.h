// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QPointer>
#include <QtCore/QUrl>
#include <QtCore/QIODevice>
#include <QtCore/QVariantMap>

// ACF includes
#include <ibase/IProgressManager.h>


namespace imtrest
{


class CHttpDataUploader: public QObject
{
	Q_OBJECT

	Q_PROPERTY(qsizetype partMaxSize READ GetPartMaxSize WRITE SetPartMaxSize NOTIFY PartMaxSizeChanged)
	Q_PROPERTY(QVariantMap additionalHeaders READ GetAdditionalHeaders WRITE SetAdditionalHeaders NOTIFY AdditionalHeadersChanged)
	Q_PROPERTY(QUrl uploadUrl READ GetUploadUrl WRITE SetUploadUrl NOTIFY uploadUrlChanged FINAL)
public:
	explicit CHttpDataUploader(QObject* parentPtr = nullptr);

	void SetDevice(QIODevice* devicePtr);
	bool InitializeDeviceFromFile(const QString& filePath);
	bool UploadFile(const QUrl& url = QUrl());

	void SetProgressManager(ibase::IProgressManager* progressManager);


	[[nodiscard]] qsizetype GetPartMaxSize() const;
	void SetPartMaxSize(qsizetype partMaxSize);
	[[nodiscard]] QVariantMap GetAdditionalHeaders() const;
	void SetAdditionalHeaders(const QVariantMap& additionalHeaders);
	[[nodiscard]] QUrl GetUploadUrl() const;
	void SetUploadUrl(const QUrl& uploadUrl);

Q_SIGNALS:
	void PartMaxSizeChanged();
	void AdditionalHeadersChanged();
	void uploadUrlChanged();

private:
	QPointer<QIODevice> m_devicePtr;
	qsizetype m_partMaxSize;
	QVariantMap m_additionalHeaders;
	QUrl m_uploadUrl;
	ibase::IProgressManager* m_progressManager;
};


} // namespace imtrest


