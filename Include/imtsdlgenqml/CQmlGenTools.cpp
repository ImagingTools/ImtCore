// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgenqml/CQmlGenTools.h>


// Standard includes
#include <chrono>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QLockFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>

// ACF includes
#include <iser/CJsonMemWriteArchive.h>
#include <iser/CJsonMemReadArchive.h>


namespace imtsdlgenqml
{


bool CQmlGenTools::ReadGenerationResultFile(CSdlQmlGenerationResult& result, const QString& filePath)
{
	// Check if file exists
	if (!QFile::exists(filePath)){
		qWarning() << "File does not exist:" << filePath;
		
		return false;
	}
	
	// Read file
	QFile file(filePath);
	if (!file.open(QFile::ReadOnly | QFile::Text)){
		qWarning() << "Failed to open file for reading:" << filePath << "Error:" << file.errorString();
		
		return false;
	}
	
	QByteArray jsonData = file.readAll();
	file.close();
	
	// Deserialize using CJsonMemReadArchive
	iser::CJsonMemReadArchive archive(jsonData, false);
	if (!result.Serialize(archive)){
		qWarning() << "Failed to deserialize generation result from file:" << filePath;
		
		return false;
	}
	
	return true;
}


bool CQmlGenTools::WriteGenerationResultFile(CSdlQmlGenerationResult& result, const QString& filePath)
{
	// Ensure the directory exists
	QFileInfo fileInfo(filePath);
	QDir dir = fileInfo.absoluteDir();
	if (!dir.exists()){
		if (!dir.mkpath(".")){
			qWarning() << "Failed to create directory:" << dir.absolutePath();
			
			return false;
		}
	}
	
	// Open file for writing
	QFile file(filePath);
	if (!file.open(QFile::WriteOnly | QFile::Text)){
		qWarning() << "Failed to open file for writing:" << filePath << "Error:" << file.errorString();
		
		return false;
	}
	
	// Serialize using CJsonMemWriteArchive
	iser::CJsonMemWriteArchive archive(nullptr, false);
	if (!result.Serialize(archive)){
		qWarning() << "Failed to serialize generation result for file:" << filePath;
		file.close();
		
		return false;
	}
	
	// Write serialized data to file
	QByteArray jsonData = archive.GetData();
	qint64 bytesWritten = file.write(jsonData);
	file.close();
	
	if (bytesWritten <= 0){
		qWarning() << "Failed to write data to file:" << filePath;
		
		return false;
	}
	
	return true;
}


bool CQmlGenTools::UpdateGenerationResult(const QString& filePath, const CSdlQmlGenerationResult& updateData)
{
	// Create lock file to prevent concurrent access
	QString lockFilePath = filePath + ".lock";
	QLockFile lockFile(lockFilePath);
	
	// Try to acquire the lock with a timeout of 10 minutes
	if (!lockFile.tryLock(std::chrono::minutes(10))){
		qWarning() << "Failed to acquire lock for file:" << filePath;
		
		return false;
	}
	
	// Read existing file if it exists
	CSdlQmlGenerationResult result;
	bool fileExists = QFile::exists(filePath);
	
	if (fileExists){
		if (!ReadGenerationResultFile(result, filePath)){
			return false;
		}
	}
	
	// Append additional folders from updateData to existing set using operator|=
	QSet<QString> existingFolders = result.GetCreatedFolders();
	existingFolders |= updateData.GetCreatedFolders();
	result.SetCreatedFolders(existingFolders);
	
	// Update creation date: use the one from updateData if valid, otherwise use current UTC time
	QDateTime creationTime = updateData.GetCreatedAt();
	if (!creationTime.isValid()){
		creationTime = QDateTime::currentDateTimeUtc();
	}
	result.SetCreatedAt(creationTime);
	
	// Write updated result back to file
	bool success = WriteGenerationResultFile(result, filePath);
	
	return success;
}


} // namespace imtsdlgenqml
