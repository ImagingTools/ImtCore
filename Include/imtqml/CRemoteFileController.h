#pragma once


// Qt includes
#include <QtCore/QObject>

#undef DeleteFile


namespace imtqml
{


/**
	Universal data controller for UI representations
	\ingroup LicenseManagement
*/
class CRemoteFileController: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString state READ state WRITE SetState NOTIFY stateChanged)
	Q_PROPERTY(QString downloadedFileLocation READ downloadedFileLocation WRITE setDownloadedFileLocation NOTIFY downloadedFileLocationChanged)
	Q_PROPERTY(QString downloadedFilePath READ downloadedFilePath WRITE setDownloadedFilePath NOTIFY downloadedFilePathChanged)
	Q_PROPERTY(QByteArray json READ json WRITE setJson NOTIFY jsonChanged)
	Q_PROPERTY(QString prefix READ prefix WRITE setPrefix NOTIFY prefixChanged)

	QString m_state;
	QString m_downloadedFilePath;

public:
	explicit CRemoteFileController(QObject* parent = nullptr);
	~CRemoteFileController();

	const QString& state() const;
	void SetState(const QString& newState);
	const QString& downloadedFilePath() const;
	void setDownloadedFilePath(const QString& newDownloadedFilePath);
	const QString& downloadedFileLocation() const;
	void setDownloadedFileLocation(const QString& newDownloadedFileLocation);
	const QByteArray& json() const;
	void setJson(const QByteArray& newJson);
	const QString& prefix() const;
	void setPrefix(const QString& newPrefix);

public Q_SLOTS:
	virtual bool DeleteFile(const QString& fileId);
	virtual bool GetFile(const QString& fileId, const QString& fileName);
	virtual bool SendFile(const QString& fileUrl);
	virtual bool OpenFile(const QString& filePath = QString()) const;

private Q_SLOTS:
	void OnFileDeleted();
	void OnFileDownloaded();
	void OnFileUploaded();
	void OnProgressChanged(qint64 bytesLoaded, qint64 bytesTotal);

signals:
	void stateChanged();
	void downloadedFilePathChanged();
	void downloadedFileLocationChanged();
	void jsonChanged();
	void fileDownloaded(const QString& filePath);
	void progress(qint64 bytesLoaded, qint64 bytesTotal);
	void fileUploaded();
	void fileDeleted();
	void fileUploadFailed();
	void fileDeleteFailed();
	void fileDownloadFailed();

	void prefixChanged();

private:
	/**
		\brief Calculates openable url for the specific OS
		\param filePath - path to the file for calculating the Url
		\return Openable url, that can be opened with \c QDesktopServices::openUrl()
		\warning The ANDROID required to update gradle and mainfest and do not forget to add permissons for read and write storage
				gradle
					dependencies {
						implementation 'androidx.core:core:1.3.2'
					}

				manifest
					<provider android:name="androidx.core.content.FileProvider" android:authorities="--%%INSERT_PACKAGE_NAME%%--.provider" android:exported="false" android:grantUriPermissions="true">
							<meta-data android:name="android.support.FILE_PROVIDER_PATHS" android:resource="@xml/provider_paths"/>
					</provider>
	 */

private:
	QString m_preferredFileNameForSave;
	QString m_downloadedFileLocation;
	QByteArray m_json;
	qint64 m_bytesLoaded;
	qint64 m_bytesTotal;
	bool m_isAutoOpen;
	QString m_prefix;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CRemoteFileController*)


