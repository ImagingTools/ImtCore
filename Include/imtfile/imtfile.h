// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtfile.h
	\brief Main header file for the imtfile module - File I/O Operations and Management
	
	This file provides namespace-level documentation for the imtfile module.
	Include this file to access the complete imtfile module documentation in Doxygen.
*/

/**
	\namespace imtfile
	\brief File I/O Operations and Management Module
	
	The \b imtfile module provides file input/output utilities, format conversion,
	compression services, and file content provisioning for ImtCore applications.
	
	\section imtfile_overview Overview
	
	This module extends the base ACF file operations (ifile namespace) with
	specialized functionality:
	- Binary file content loading and provision
	- File format conversion between compatible formats
	- File and folder compression/decompression
	- Integration with ACF persistence infrastructure
	
	\section imtfile_architecture Architecture
	
	\subsection imtfile_patterns Design Patterns
	
	<b>Provider Pattern:</b>
	- IFileProvider abstracts file content loading
	- CFileProviderComp provides concrete implementation
	- Enables dependency injection for file access
	- Facilitates testing with mock providers
	
	<b>Strategy Pattern:</b>
	- IFileCompression defines compression algorithm interface
	- Multiple compression strategies (ZIP, GZIP, etc.) possible
	- Algorithm selection based on file type or user preference
	
	<b>Adapter Pattern:</b>
	- CFileFormatConversionComp adapts between document formats
	- Uses IFilePersistence for format-specific I/O
	- Converts through intermediate working object
	- Transparent format translation
	
	<b>Component Pattern (ACF):</b>
	- All major classes are ACF components
	- Factory-based instantiation
	- Reference-based dependency injection
	- Configuration through attributes
	
	\subsection imtfile_core_interfaces Core Interfaces
	
	The module provides three primary service interfaces:
	
	<b>File Content Provider:</b>
	\code
	IFileProvider (istd::IPolymorphic)
	├─ LoadData(data, filePath) - Load binary file contents
	└─ Use cases: Resource loading, data import
	    │
	    └─ CFileProviderComp (concrete implementation)
	        ├─ Handles binary file reading
	        ├─ Error handling and logging
	        └─ Cross-platform path handling
	\endcode
	
	<b>File Compression Service:</b>
	\code
	IFileCompression (istd::IPolymorphic)
	├─ CompressFile(source, target) - Compress single file
	├─ DecompressFile(source, target) - Decompress single file
	├─ CompressFolder(source, target, recursive) - Compress directory
	└─ DecompressFolder(source, target) - Extract archive
	    │
	    └─ Implementations available in imtzip module
	        ├─ ZIP format support
	        ├─ Recursive folder compression
	        └─ Preserve file attributes
	\endcode
	
	<b>Format Conversion Service:</b>
	\code
	CFileFormatConversionComp (ifileproc::IFileConversion)
	├─ ConvertFiles(inputPath, outputPath, params, progress)
	├─ Depends on:
	│   ├─ IFilePersistence - Format reading/writing
	│   └─ IChangeable - Working object for conversion
	└─ Pattern: Load → Convert → Save
	    │
	    Example: XML → JSON, CSV → SQL, etc.
	\endcode
	
	\section imtfile_services Core Services
	
	\subsection imtfile_provider File Content Provider
	
	**IFileProvider / CFileProviderComp** - Binary file loading service:
	
	\code{.cpp}
	// Create file provider
	auto fileProvider = CFileProviderComp::CreateInstance();
	
	// Load binary file contents
	QByteArray fileData;
	QString filePath = "/path/to/resource.dat";
	bool success = fileProvider->LoadData(fileData, filePath.toUtf8());
	
	if (success) {
	    // Process file data
	    qDebug() << "Loaded" << fileData.size() << "bytes";
	}
	\endcode
	
	**Use Cases:**
	- Loading application resources
	- Reading configuration files
	- Importing binary data
	- Reading embedded assets
	
	**Features:**
	- Cross-platform path handling
	- Automatic error logging
	- Memory-efficient streaming
	- Binary format agnostic
	
	\subsection imtfile_compression File Compression
	
	**IFileCompression** - File and folder compression/decompression interface.
	Concrete implementations are provided in the imtzip module.
	
	\code{.cpp}
	// Assuming implementation from imtzip
	auto compression = CZipCompressionComp::CreateInstance();
	
	// Compress single file
	compression->CompressFile(
	    "/path/to/input.txt",
	    "/path/to/output.zip");
	
	// Decompress single file
	compression->DecompressFile(
	    "/path/to/archive.zip",
	    "/path/to/extracted.txt");
	
	// Compress entire folder (recursive)
	compression->CompressFolder(
	    "/path/to/folder",
	    "/path/to/archive.zip",
	    true  // recursive
	);
	
	// Extract folder
	compression->DecompressFolder(
	    "/path/to/archive.zip",
	    "/path/to/output_folder");
	\endcode
	
	**Use Cases:**
	- Creating backup archives
	- Compressing log files
	- Packaging application data
	- Reducing storage footprint
	- Preparing data for transmission
	
	\subsection imtfile_conversion Format Conversion
	
	**CFileFormatConversionComp** - Convert between document formats:
	
	\code{.cpp}
	// Create conversion component
	auto converter = CFileFormatConversionComp::CreateInstance();
	
	// Setup persistence for source and target formats
	// Example: Convert XML to JSON
	auto xmlPersistence = CXmlPersistenceComp::CreateInstance();
	auto jsonPersistence = CJsonPersistenceComp::CreateInstance();
	
	// Setup working object (document model)
	auto document = CDataDocumentComp::CreateInstance();
	
	// Configure converter
	converter->SetAttribute("Persistence", xmlPersistence.get());
	converter->SetAttribute("WorkingObject", document.get());
	
	// Perform conversion
	QString inputPath = "data.xml";
	QString outputPath = "data.json";
	int result = converter->ConvertFiles(inputPath, outputPath);
	
	if (result == ifileproc::IFileConversion::CS_OK) {
	    qDebug() << "Conversion successful";
	}
	\endcode
	
	**Conversion Process:**
	1. Load source file using IFilePersistence (format A)
	2. Populate working object (IChangeable)
	3. Save working object using IFilePersistence (format B)
	4. Report progress if IProgressManager provided
	
	**Supported Format Combinations:**
	- Any formats with compatible IFilePersistence implementations
	- XML ↔ JSON (via imtbase persistence components)
	- Binary ↔ Text formats
	- Custom formats via persistence plugins
	
	\section imtfile_integration Integration Patterns
	
	\subsection imtfile_with_persistence Integration with ACF Persistence
	
	**Pattern: File Provider with Persistence:**
	\code{.cpp}
	class CDataLoaderComp : public ACF_COMPONENT(IDataLoader)
	{
	    I_REFERENCE(IFileProvider, m_fileProvider)
	    I_REFERENCE(IFilePersistence, m_persistence)
	    
	    bool LoadDocument(const QString& filePath) override
	    {
	        // Load raw file data
	        QByteArray rawData;
	        if (!m_fileProvider->LoadData(rawData, filePath.toUtf8())) {
	            return false;
	        }
	        
	        // Parse using persistence
	        QBuffer buffer(&rawData);
	        buffer.open(QIODevice::ReadOnly);
	        
	        return m_persistence->LoadFromDevice(m_document, &buffer) 
	            == IFilePersistence::OS_OK;
	    }
	};
	\endcode
	
	\subsection imtfile_with_compression Integration with Compression
	
	**Pattern: Automatic Archive Creation:**
	\code{.cpp}
	class CBackupManagerComp : public ACF_COMPONENT(IBackupManager)
	{
	    I_REFERENCE(IFileCompression, m_compression)
	    
	    bool CreateBackup(const QString& dataDir) override
	    {
	        // Generate timestamp-based archive name
	        QString timestamp = QDateTime::currentDateTime()
	            .toString("yyyyMMdd_hhmmss");
	        QString archivePath = QString("backup_%1.zip").arg(timestamp);
	        
	        // Compress entire data directory
	        return m_compression->CompressFolder(
	            dataDir, 
	            archivePath,
	            true  // recursive
	        );
	    }
	    
	    bool RestoreBackup(const QString& archivePath, 
	                      const QString& targetDir) override
	    {
	        return m_compression->DecompressFolder(archivePath, targetDir);
	    }
	};
	\endcode
	
	\subsection imtfile_with_conversion Integration with Format Conversion
	
	**Pattern: Multi-Format Export:**
	\code{.cpp}
	class CReportExporterComp : public ACF_COMPONENT(IReportExporter)
	{
	    I_REFERENCE(IFileFormatConversion, m_converter)
	    
	    void ExportReport(IReport* report, const QString& format)
	    {
	        // Save to intermediate format (e.g., XML)
	        QString tempFile = QDir::temp().filePath("report.xml");
	        SaveReportToXml(report, tempFile);
	        
	        // Convert to target format
	        QString outputFile = QString("report.%1").arg(format);
	        
	        if (format == "json") {
	            m_converter->SetAttribute("Persistence", m_jsonPersistence);
	            m_converter->ConvertFiles(tempFile, outputFile);
	        }
	        else if (format == "csv") {
	            m_converter->SetAttribute("Persistence", m_csvPersistence);
	            m_converter->ConvertFiles(tempFile, outputFile);
	        }
	    }
	};
	\endcode
	
	\section imtfile_examples Complete Examples
	
	\subsection imtfile_example_resource Resource Loading System
	
	\code{.cpp}
	// Resource manager using file provider
	class CResourceManagerComp : public ACF_COMPONENT(IResourceManager)
	{
	    I_REFERENCE(IFileProvider, m_fileProvider)
	    QHash<QString, QByteArray> m_cache;
	    
	public:
	    QByteArray LoadResource(const QString& resourceId)
	    {
	        // Check cache
	        if (m_cache.contains(resourceId)) {
	            return m_cache[resourceId];
	        }
	        
	        // Build resource path
	        QString resourcePath = QString(":/resources/%1").arg(resourceId);
	        
	        // Load from file
	        QByteArray resourceData;
	        if (m_fileProvider->LoadData(resourceData, resourcePath.toUtf8())) {
	            m_cache[resourceId] = resourceData;
	            return resourceData;
	        }
	        
	        return QByteArray();
	    }
	    
	    QPixmap LoadImage(const QString& imageId)
	    {
	        QByteArray imageData = LoadResource(imageId);
	        QPixmap pixmap;
	        pixmap.loadFromData(imageData);
	        return pixmap;
	    }
	};
	\endcode
	
	\subsection imtfile_example_backup Backup and Restore System
	
	\code{.cpp}
	// Comprehensive backup system
	class CApplicationBackupComp : public ACF_COMPONENT(IApplicationBackup)
	{
	    I_REFERENCE(IFileCompression, m_compression)
	    I_REFERENCE(IFileProvider, m_fileProvider)
	    
	public:
	    struct BackupInfo {
	        QString archivePath;
	        QDateTime timestamp;
	        qint64 sizeBytes;
	    };
	    
	    bool CreateFullBackup(IProgressManager* progress = nullptr)
	    {
	        // Get application data directory
	        QString dataDir = QStandardPaths::writableLocation(
	            QStandardPaths::AppDataLocation);
	        
	        // Create backup directory if needed
	        QDir backupDir(dataDir + "/backups");
	        if (!backupDir.exists()) {
	            backupDir.mkpath(".");
	        }
	        
	        // Generate archive name
	        QString timestamp = QDateTime::currentDateTime()
	            .toString("yyyyMMdd_hhmmss");
	        QString archivePath = backupDir.filePath(
	            QString("backup_%1.zip").arg(timestamp));
	        
	        // Compress application data
	        bool success = m_compression->CompressFolder(
	            dataDir + "/data",
	            archivePath,
	            true  // recursive
	        );
	        
	        if (success && progress) {
	            progress->SetProgressValue(100);
	        }
	        
	        return success;
	    }
	    
	    QList<BackupInfo> ListAvailableBackups()
	    {
	        QList<BackupInfo> backups;
	        
	        QString dataDir = QStandardPaths::writableLocation(
	            QStandardPaths::AppDataLocation);
	        QDir backupDir(dataDir + "/backups");
	        
	        QStringList archives = backupDir.entryList(
	            QStringList() << "backup_*.zip",
	            QDir::Files,
	            QDir::Time | QDir::Reversed);
	        
	        for (const QString& archive : archives) {
	            BackupInfo info;
	            info.archivePath = backupDir.filePath(archive);
	            
	            QFileInfo fileInfo(info.archivePath);
	            info.timestamp = fileInfo.lastModified();
	            info.sizeBytes = fileInfo.size();
	            
	            backups.append(info);
	        }
	        
	        return backups;
	    }
	    
	    bool RestoreBackup(const QString& archivePath)
	    {
	        QString dataDir = QStandardPaths::writableLocation(
	            QStandardPaths::AppDataLocation);
	        
	        // Create temporary restore directory
	        QString restoreDir = dataDir + "/restore_temp";
	        QDir().mkpath(restoreDir);
	        
	        // Extract archive
	        bool success = m_compression->DecompressFolder(
	            archivePath,
	            restoreDir);
	        
	        if (success) {
	            // Move restored data to actual location
	            // (implement atomic move with backup of current data)
	            MoveRestoredData(restoreDir, dataDir + "/data");
	        }
	        
	        // Clean up temporary directory
	        QDir(restoreDir).removeRecursively();
	        
	        return success;
	    }
	};
	\endcode
	
	\subsection imtfile_example_conversion Document Conversion Pipeline
	
	\code{.cpp}
	// Multi-stage document conversion pipeline
	class CDocumentConverterComp : public ACF_COMPONENT(IDocumentConverter)
	{
	    I_REFERENCE(IFileFormatConversion, m_converter)
	    
	public:
	    enum Format {
	        FMT_XML,
	        FMT_JSON,
	        FMT_CSV,
	        FMT_BINARY
	    };
	    
	    bool ConvertDocument(const QString& inputPath,
	                        Format inputFormat,
	                        const QString& outputPath,
	                        Format outputFormat,
	                        IProgressManager* progress = nullptr)
	    {
	        // Setup source persistence
	        auto sourcePersistence = CreatePersistence(inputFormat);
	        if (!sourcePersistence) {
	            return false;
	        }
	        
	        // Setup target persistence
	        auto targetPersistence = CreatePersistence(outputFormat);
	        if (!targetPersistence) {
	            return false;
	        }
	        
	        // Create working document
	        auto document = CDataDocument::CreateInstance();
	        
	        // Configure converter for loading
	        m_converter->SetAttribute("Persistence", sourcePersistence.get());
	        m_converter->SetAttribute("WorkingObject", document.get());
	        
	        // Load document
	        QString tempPath = outputPath;
	        int result = m_converter->ConvertFiles(inputPath, tempPath, nullptr, progress);
	        
	        if (result != ifileproc::IFileConversion::CS_OK) {
	            return false;
	        }
	        
	        // Configure converter for saving
	        m_converter->SetAttribute("Persistence", targetPersistence.get());
	        
	        // Save in target format
	        result = m_converter->ConvertFiles(tempPath, outputPath, nullptr, progress);
	        
	        return result == ifileproc::IFileConversion::CS_OK;
	    }
	    
	private:
	    IFilePersistenceUniquePtr CreatePersistence(Format format)
	    {
	        switch (format) {
	            case FMT_XML:
	                return CXmlPersistenceComp::CreateInstance();
	            case FMT_JSON:
	                return CJsonPersistenceComp::CreateInstance();
	            case FMT_CSV:
	                return CCsvPersistenceComp::CreateInstance();
	            case FMT_BINARY:
	                return CBinaryPersistenceComp::CreateInstance();
	            default:
	                return nullptr;
	        }
	    }
	};
	\endcode
	
	\section imtfile_best_practices Best Practices
	
	\subsection imtfile_error_handling Error Handling
	
	- Always check return values from LoadData(), CompressFile(), etc.
	- Use try-catch blocks when working with file I/O
	- Provide meaningful error messages to users
	- Log errors using ilog::ILogger for debugging
	
	\subsection imtfile_performance Performance Considerations
	
	- Cache frequently accessed file contents in memory
	- Use streaming for large files instead of loading entirely
	- Compress files asynchronously to avoid blocking UI
	- Consider file size before loading into memory
	
	\subsection imtfile_security Security Best Practices
	
	- Validate file paths to prevent directory traversal
	- Check file permissions before accessing
	- Sanitize user-provided file paths
	- Use appropriate file system APIs for platform security
	
	\section imtfile_related_modules Integration with Other Modules
	
	<b>With imtzip (Compression):</b>
	- Concrete IFileCompression implementations
	- ZIP format support
	- Archive creation and extraction
	
	<b>With imtcrypt (Encryption):</b>
	- Encrypted file storage via CEncryptionBasedPersistenceComp
	- Secure file loading with IFileProvider
	- Protected backup archives
	
	<b>With imtdb (Database):</b>
	- Import/export database content to files
	- Backup database to compressed archives
	- Load database initialization scripts
	
	<b>With imtbase (Collections):</b>
	- File-based collections with IFileProvider
	- Bulk file operations on collections
	- Resource loading for collection items
	
	\section imtfile_references References
	
	<b>Related Modules:</b>
	- ifile (ACF) - Base file persistence interfaces
	- ifileproc (ACF) - File processing interfaces
	- imtzip - ZIP compression implementation
	- imtcrypt - Encrypted file operations
	
	<b>ACF Interfaces:</b>
	- ifile::IFilePersistence - File format persistence
	- ifile::IFileNameParam - File path parameter
	- ifileproc::IFileConversion - Format conversion
	
	<b>External Documentation:</b>
	- Qt File I/O: https://doc.qt.io/qt-6/qfile.html
	- Qt Resource System: https://doc.qt.io/qt-6/resources.html
*/

namespace imtfile
{
	// Namespace for file I/O operations and management
}
