// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imthttp.h
	\brief Main header file for the imthttp module - HTTP Client and File Transfer
	
	This file provides namespace-level documentation for the imthttp module.
	Include this file to access the complete imthttp module documentation in Doxygen.
*/

/**
	\namespace imthttp
	\brief HTTP Client and File Transfer Module
	
	The \b imthttp module provides HTTP-based file transfer capabilities,
	enabling file uploads and downloads via HTTP/HTTPS protocols with
	simple, straightforward API.
	
	\section imthttp_overview Overview
	
	This lightweight module provides HTTP file operations:
	- HTTP file uploads to remote servers
	- HTTP file downloads from remote URLs
	- HTTPS secure transfers
	- Integration with imtcom file transfer interface
	- Qt Network-based implementation
	
	\section imthttp_architecture Architecture
	
	\subsection imthttp_patterns Design Patterns
	
	<b>Adapter Pattern:</b>
	- Adapts Qt Network classes to IFileTransfer interface
	- Provides common API for file transfers
	- Enables interchangeable transfer mechanisms
	
	<b>Facade Pattern:</b>
	- Simplifies Qt Network API complexity
	- Single interface for upload/download operations
	- Hides HTTP protocol details
	
	\subsection imthttp_core_classes Core Classes
	
	**HTTP File Transfer:**
	\code
	CHttpFileTransfer (imtcom::IFileTransfer)
	├─ UploadFile(filePath, url) - Upload file via HTTP POST
	├─ DownloadFile(filePath, url) - Download file via HTTP GET
	└─ Qt Network-based implementation
	    │
	    Uses:
	    ├─ QNetworkAccessManager - HTTP request management
	    ├─ QNetworkRequest - HTTP request configuration
	    ├─ QNetworkReply - HTTP response handling
	    └─ QFile - Local file I/O
	\endcode
	
	\section imthttp_usage Usage Examples
	
	\subsection imthttp_upload File Upload
	
	**Upload File to Server:**
	\code{.cpp}
	#include <imthttp/CHttpFileTransfer.h>
	
	// Create HTTP file transfer
	imthttp::CHttpFileTransfer httpTransfer;
	
	// Upload file
	QString localFile = "/path/to/local/document.pdf";
	QUrl uploadUrl = QUrl("https://api.example.com/upload");
	
	bool success = httpTransfer.UploadFile(localFile, uploadUrl);
	
	if (success) {
	    qDebug() << "File uploaded successfully";
	} else {
	    qCritical() << "File upload failed";
	}
	\endcode
	
	\subsection imthttp_download File Download
	
	**Download File from Server:**
	\code{.cpp}
	// Download file
	QString saveToFile = "/path/to/save/downloaded.pdf";
	QUrl downloadUrl = QUrl("https://example.com/files/document.pdf");
	
	bool success = httpTransfer.DownloadFile(saveToFile, downloadUrl);
	
	if (success) {
	    qDebug() << "File downloaded successfully to:" << saveToFile;
	} else {
	    qCritical() << "File download failed";
	}
	\endcode
	
	\subsection imthttp_secure_transfer Secure HTTPS Transfer
	
	**HTTPS Upload with Authentication:**
	\code{.cpp}
	// For advanced scenarios, extend CHttpFileTransfer
	class CSecureHttpTransferComp : public CHttpFileTransfer
	{
	public:
	    bool UploadFileWithAuth(const QString& filePath,
	                           const QUrl& url,
	                           const QString& authToken)
	    {
	        // Create network manager
	        QNetworkAccessManager manager;
	        
	        // Open file
	        QFile file(filePath);
	        if (!file.open(QIODevice::ReadOnly)) {
	            return false;
	        }
	        
	        // Create request with authentication
	        QNetworkRequest request(url);
	        request.setRawHeader("Authorization", 
	            QString("Bearer %1").arg(authToken).toUtf8());
	        request.setHeader(QNetworkRequest::ContentTypeHeader,
	            "application/octet-stream");
	        
	        // Upload file
	        QNetworkReply* reply = manager.post(request, &file);
	        
	        // Wait for completion
	        QEventLoop loop;
	        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	        loop.exec();
	        
	        // Check result
	        bool success = (reply->error() == QNetworkReply::NoError);
	        
	        reply->deleteLater();
	        return success;
	    }
	};
	\endcode
	
	\subsection imthttp_progress Progress Tracking
	
	**Track Upload/Download Progress:**
	\code{.cpp}
	class CProgressTrackingTransfer : public imthttp::CHttpFileTransfer
	{
	    Q_OBJECT
	    
	public:
	    bool UploadFileWithProgress(const QString& filePath,
	                                const QUrl& url,
	                                IProgressManager* progress = nullptr)
	    {
	        QNetworkAccessManager manager;
	        QFile file(filePath);
	        
	        if (!file.open(QIODevice::ReadOnly)) {
	            return false;
	        }
	        
	        QNetworkRequest request(url);
	        QNetworkReply* reply = manager.post(request, &file);
	        
	        // Connect progress signal
	        if (progress) {
	            connect(reply, &QNetworkReply::uploadProgress,
	                   [progress](qint64 bytesSent, qint64 bytesTotal) {
	                if (bytesTotal > 0) {
	                    int percent = (bytesSent * 100) / bytesTotal;
	                    progress->SetProgressValue(percent);
	                }
	            });
	        }
	        
	        QEventLoop loop;
	        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	        loop.exec();
	        
	        bool success = (reply->error() == QNetworkReply::NoError);
	        reply->deleteLater();
	        
	        return success;
	    }
	};
	\endcode
	
	\section imthttp_integration Integration Patterns
	
	\subsection imthttp_with_imtcom Integration with imtcom
	
	**Pattern: Pluggable File Transfer:**
	\code{.cpp}
	class CFileManagerComp : public ACF_COMPONENT(IFileManager)
	{
	    I_REFERENCE(imtcom::IFileTransfer, m_fileTransfer)
	    
	public:
	    void InitializeComponent() override
	    {
	        // Use HTTP file transfer
	        auto httpTransfer = std::make_unique<imthttp::CHttpFileTransfer>();
	        m_fileTransfer = httpTransfer.get();
	    }
	    
	    bool UploadToServer(const QString& localFile, const QString& remoteUrl)
	    {
	        // Use IFileTransfer interface (could be HTTP, FTP, etc.)
	        return m_fileTransfer->UploadFile(localFile, QUrl(remoteUrl));
	    }
	    
	    bool DownloadFromServer(const QString& remoteUrl, 
	                           const QString& localFile)
	    {
	        return m_fileTransfer->DownloadFile(localFile, QUrl(remoteUrl));
	    }
	};
	\endcode
	
	\subsection imthttp_backup_sync Backup and Sync
	
	**Pattern: Cloud Backup via HTTP:**
	\code{.cpp}
	class CCloudBackupComp : public ACF_COMPONENT(ICloudBackup)
	{
	    imthttp::CHttpFileTransfer m_httpTransfer;
	    QString m_backupServerUrl;
	    QString m_authToken;
	    
	public:
	    bool BackupFile(const QString& localFile)
	    {
	        // Generate remote file name
	        QFileInfo fileInfo(localFile);
	        QString fileName = fileInfo.fileName();
	        QString timestamp = QDateTime::currentDateTime()
	            .toString("yyyyMMdd_hhmmss");
	        QString remoteFileName = QString("%1_%2").arg(timestamp).arg(fileName);
	        
	        // Build upload URL
	        QUrl uploadUrl = QUrl(QString("%1/backup/%2")
	            .arg(m_backupServerUrl)
	            .arg(remoteFileName));
	        
	        // Upload to cloud
	        bool success = m_httpTransfer.UploadFile(localFile, uploadUrl);
	        
	        if (success) {
	            qDebug() << "Backed up to cloud:" << remoteFileName;
	        }
	        
	        return success;
	    }
	    
	    bool RestoreFile(const QString& remoteFileName, 
	                    const QString& localFile)
	    {
	        QUrl downloadUrl = QUrl(QString("%1/backup/%2")
	            .arg(m_backupServerUrl)
	            .arg(remoteFileName));
	        
	        return m_httpTransfer.DownloadFile(localFile, downloadUrl);
	    }
	};
	\endcode
	
	\section imthttp_best_practices Best Practices
	
	\subsection imthttp_error_handling Error Handling
	
	- Check return values from UploadFile/DownloadFile
	- Implement retry logic for network failures
	- Handle HTTP error codes appropriately
	- Provide user feedback for long transfers
	- Log transfer errors for debugging
	
	\subsection imthttp_security Security Considerations
	
	- Always use HTTPS for sensitive data transfers
	- Validate SSL certificates in production
	- Implement authentication (OAuth, API tokens)
	- Never embed credentials in URLs
	- Sanitize file paths to prevent path traversal
	- Implement file size limits
	
	\subsection imthttp_performance Performance Optimization
	
	- Use asynchronous transfers for large files
	- Implement progress tracking for user feedback
	- Consider chunked uploads for very large files
	- Reuse QNetworkAccessManager instances
	- Implement transfer timeout handling
	- Use compression when appropriate
	
	\section imthttp_related_modules Integration with Other Modules
	
	<b>With imtcom (Server Communication):</b>
	- Implements IFileTransfer interface
	- Integrates with server connection management
	- Common file transfer abstraction
	
	<b>With imtfile (File Operations):</b>
	- Local file I/O for transfers
	- File compression before upload
	- File validation after download
	
	<b>With imtcrypt (Encryption):</b>
	- Encrypt files before upload
	- Decrypt files after download
	- Secure sensitive data transfers
	
	\section imthttp_references References
	
	<b>Related Modules:</b>
	- imtcom - Server communication and file transfer interface
	- imtfile - File I/O operations
	- imtcrypt - File encryption
	
	<b>Qt Network Module:</b>
	- QNetworkAccessManager: https://doc.qt.io/qt-6/qnetworkaccessmanager.html
	- QNetworkRequest: https://doc.qt.io/qt-6/qnetworkrequest.html
	- QNetworkReply: https://doc.qt.io/qt-6/qnetworkreply.html
	
	<b>HTTP Specifications:</b>
	- HTTP/1.1: RFC 2616
	- HTTP/2: RFC 7540
	- HTTPS: RFC 2818
*/

namespace imthttp
{
	// Namespace for HTTP client and file transfer
}
