#pragma once


// Qt includes
#include <QtCore/QObject>



namespace imtrest
{


/**
    Universal data controller for UI representations
    \ingroup LicenseManagement
*/
class CFileSender: public QObject
{
    Q_OBJECT
	Q_PROPERTY(QString state READ state WRITE SetState NOTIFY stateChanged)
	Q_PROPERTY(QString downloadedFileLocation READ downloadedFileLocation WRITE setDownloadedFileLocation NOTIFY downloadedFileLocationChanged)
	Q_PROPERTY(QString downloadedFilePath READ downloadedFilePath WRITE setDownloadedFilePath NOTIFY downloadedFilePathChanged)

	QString m_state;

	QString m_downloadedFilePath;

public:
    explicit CFileSender(QObject* parent = nullptr);
    ~CFileSender();

	const QString& state() const;
	void SetState(const QString& newState);

	const QString& downloadedFilePath() const;
	void setDownloadedFilePath(const QString& newDownloadedFilePath);

	const QString& downloadedFileLocation() const;
	void setDownloadedFileLocation(const QString& newDownloadedFileLocation);

public Q_SLOTS:
	bool DeleteFile(const QString& fileId);
	bool GetFile(const QString& fileId, const QString& fileName);
	bool SendFile(const QString& fileUrl);


private Q_SLOTS:
	void OnFileDeleted();
	void OnFileDownloaded();
	void OnFileUploaded();

signals:
	void stateChanged();
	void downloadedFilePathChanged();

	void downloadedFileLocationChanged();

private:
	QString m_preferredFileNameForSave;
	QString m_downloadedFileLocation;
};


} // namespace imtrest

Q_DECLARE_METATYPE(imtrest::CFileSender*)







