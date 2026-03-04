// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


class QUrl;


namespace imtcom
{


/**
	\class IFileTransfer
	\brief Interface for file upload and download operations via URLs.
	
	IFileTransfer provides a simple contract for transferring files to and from
	remote servers using URL-based addressing. This interface abstracts the underlying
	transfer protocol (HTTP, FTP, etc.) and provides a uniform API for file operations.
	
	\section ifiletransfer_features Key Features
	
	<b>Bidirectional Transfer:</b>
	- UploadFile(): Send local file to remote server
	- DownloadFile(): Retrieve remote file to local filesystem
	- URL-based addressing for flexibility
	
	<b>Protocol Agnostic:</b>
	- Supports HTTP/HTTPS (typical implementation)
	- Potentially FTP, SFTP, or custom protocols
	- Protocol determined by URL scheme (http://, ftp://, etc.)
	
	<b>Synchronous Operations:</b>
	- Blocking calls that complete before returning
	- Return value indicates success/failure
	- Use in separate thread if non-blocking behavior required
	
	\section ifiletransfer_implementations Implementation
	
	<b>Primary Implementation:</b>
	- imthttp::CHttpFileTransfer: HTTP/HTTPS-based file transfer
	- Supports authentication via IServerConnectionInterface
	- Handles multipart/form-data encoding for uploads
	- Manages progress tracking and error handling
	
	\section ifiletransfer_usage Usage Examples
	
	\subsection usage_upload File Upload
	
	\code{.cpp}
	IFileTransfer* transfer = ...;  // e.g., imthttp::CHttpFileTransfer
	
	// Upload local file to server
	QString localFile = "/path/to/document.pdf";
	QUrl uploadUrl("https://example.com/api/upload");
	
	if (transfer->UploadFile(localFile, uploadUrl)) {
	    qDebug() << "File uploaded successfully";
	} else {
	    qCritical() << "Upload failed";
	    // Check logs for detailed error
	}
	\endcode
	
	\subsection usage_download File Download
	
	\code{.cpp}
	IFileTransfer* transfer = ...;
	
	// Download remote file to local path
	QString localPath = "/tmp/downloaded-report.pdf";
	QUrl downloadUrl("https://example.com/api/files/report-2024.pdf");
	
	if (transfer->DownloadFile(localPath, downloadUrl)) {
	    qDebug() << "File downloaded to:" << localPath;
	    QFile::exists(localPath);  // Verify file exists
	} else {
	    qCritical() << "Download failed";
	}
	\endcode
	
	\subsection usage_document_management Document Management System
	
	\code{.cpp}
	class DocumentManager : public QObject {
	public:
	    DocumentManager(IFileTransfer* transfer, const QUrl& baseUrl)
	        : m_transfer(transfer), m_baseUrl(baseUrl) {}
	    
	    bool UploadDocument(const QString& localPath, const QString& documentId) {
	        QUrl uploadUrl = m_baseUrl;
	        uploadUrl.setPath(QString("/documents/%1").arg(documentId));
	        
	        qInfo() << "Uploading document" << documentId << "from" << localPath;
	        
	        if (m_transfer->UploadFile(localPath, uploadUrl)) {
	            qInfo() << "Document uploaded successfully";
	            emit documentUploaded(documentId);
	            return true;
	        } else {
	            qCritical() << "Failed to upload document";
	            emit uploadFailed(documentId);
	            return false;
	        }
	    }
	    
	    bool DownloadDocument(const QString& documentId, const QString& localPath) {
	        QUrl downloadUrl = m_baseUrl;
	        downloadUrl.setPath(QString("/documents/%1").arg(documentId));
	        
	        qInfo() << "Downloading document" << documentId << "to" << localPath;
	        
	        if (m_transfer->DownloadFile(localPath, downloadUrl)) {
	            qInfo() << "Document downloaded successfully";
	            emit documentDownloaded(documentId, localPath);
	            return true;
	        } else {
	            qCritical() << "Failed to download document";
	            emit downloadFailed(documentId);
	            return false;
	        }
	    }
	    
	signals:
	    void documentUploaded(const QString& documentId);
	    void documentDownloaded(const QString& documentId, const QString& path);
	    void uploadFailed(const QString& documentId);
	    void downloadFailed(const QString& documentId);
	    
	private:
	    IFileTransfer* m_transfer;
	    QUrl m_baseUrl;
	};
	\endcode
	
	\section ifiletransfer_best_practices Best Practices
	
	1. **Error Handling**: Always check return value and log failures
	   - Network errors, permission issues, disk full, etc.
	   - Provide user feedback for failed operations
	
	2. **File Validation**: Verify file exists before upload
	   \code{.cpp}
	   if (!QFile::exists(localPath)) {
	       qCritical() << "File not found:" << localPath;
	       return false;
	   }
	   transfer->UploadFile(localPath, url);
	   \endcode
	
	3. **URL Construction**: Use QUrl for proper URL encoding
	   \code{.cpp}
	   QUrl url("https://example.com/api/upload");
	   url.setQuery("token=abc123");  // Proper encoding
	   \endcode
	
	4. **Path Validation**: Ensure local paths are writable for downloads
	   \code{.cpp}
	   QFileInfo info(localPath);
	   if (!info.dir().exists()) {
	       info.dir().mkpath(".");  // Create directory
	   }
	   \endcode
	
	5. **Large Files**: Consider using separate thread for blocking operations
	   \code{.cpp}
	   QtConcurrent::run([transfer, localPath, url]() {
	       return transfer->UploadFile(localPath, url);
	   });
	   \endcode
	
	6. **Authentication**: Ensure URL includes auth tokens or use authenticated client
	   - Add tokens to URL query string
	   - Configure IServerConnectionInterface with credentials
	   - Use HTTPS for secure token transmission
	
	\section ifiletransfer_limitations Limitations
	
	- Synchronous (blocking) operations - may hang UI if called on main thread
	- No built-in progress reporting - implement separately if needed
	- No resume capability for interrupted transfers
	- No multi-part upload support in interface (implementation-specific)
	
	\section ifiletransfer_integration Integration Points
	
	<b>Implementations:</b>
	- imthttp::CHttpFileTransfer: HTTP/HTTPS file transfer
	  - Uses IServerConnectionInterface for server configuration
	  - Supports authentication headers
	  - Handles chunked transfer encoding
	
	<b>Related Components:</b>
	- IServerConnectionInterface: Provides server URL configuration
	- imtservergql: GraphQL endpoints for file metadata
	- imtrest: File upload/download servlets
	
	\sa imthttp::CHttpFileTransfer, IServerConnectionInterface
*/
class IFileTransfer: virtual public istd::IPolymorphic
{
public:
	/**
		\brief Upload a local file to a remote URL.
		
		Uploads the specified local file to the remote server identified by the URL.
		The operation is synchronous and blocks until completion or failure.
		
		\param filePath Absolute path to the local file to upload. Must exist and be readable.
		\param url Remote URL where the file should be uploaded. Typically an HTTP(S) endpoint.
		\return \c true if upload completed successfully; \c false on any error.
		
		\par Example:
		\code{.cpp}
		QString file = "/home/user/document.pdf";
		QUrl endpoint("https://api.example.com/upload");
		
		if (transfer->UploadFile(file, endpoint)) {
		    qDebug() << "Upload successful";
		} else {
		    qCritical() << "Upload failed - check logs";
		}
		\endcode
		
		\note This is a blocking operation. For large files or UI applications,
		      consider executing in a separate thread.
		
		\sa DownloadFile()
	*/
	virtual bool UploadFile(const QString& filePath, const QUrl& url) const = 0;
	
	/**
		\brief Download a remote file to a local path.
		
		Downloads the file from the specified URL and saves it to the local filesystem.
		The operation is synchronous and blocks until completion or failure.
		
		\param filePath Absolute path where the downloaded file should be saved.
		                Parent directory must exist and be writable.
		\param url Remote URL of the file to download. Typically an HTTP(S) endpoint.
		\return \c true if download completed successfully; \c false on any error.
		
		\par Example:
		\code{.cpp}
		QString savePath = "/tmp/report.pdf";
		QUrl fileUrl("https://example.com/reports/2024-annual.pdf");
		
		if (transfer->DownloadFile(savePath, fileUrl)) {
		    qDebug() << "Downloaded to:" << savePath;
		    // File now available at savePath
		} else {
		    qCritical() << "Download failed";
		}
		\endcode
		
		\note This is a blocking operation. For large files or UI applications,
		      consider executing in a separate thread.
		
		\warning If the local file already exists, it will be overwritten without warning.
		
		\sa UploadFile()
	*/
	virtual bool DownloadFile(const QString& filePath, const QUrl& url) const = 0;
};


} // namespace imtcom


