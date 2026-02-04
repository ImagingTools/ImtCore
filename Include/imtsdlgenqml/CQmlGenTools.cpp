#include <imtsdlgenqml/CQmlGenTools.h>


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


bool CQmlGenTools::UpdateGenerationResultFile(CSdlQmlGenerationResult& result, const QString& filePath)
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
	
	// Create lock file to prevent concurrent access
	QString lockFilePath = filePath + ".lock";
	QLockFile lockFile(lockFilePath);
	
	// Try to acquire the lock with a timeout
	if (!lockFile.tryLock(5000)){
		qWarning() << "Failed to acquire lock for file:" << filePath;
		return false;
	}
	
	// Check if file exists - if yes, open for writing; if no, create it
	QFile file(filePath);
	if (!file.open(QFile::WriteOnly | QFile::Text)){
		qWarning() << "Failed to open file for writing:" << filePath << "Error:" << file.errorString();
		lockFile.unlock();
		return false;
	}
	
	// Serialize using CJsonMemWriteArchive
	iser::CJsonMemWriteArchive archive(nullptr);
	if (!result.Serialize(archive)){
		qWarning() << "Failed to serialize generation result for file:" << filePath;
		file.close();
		lockFile.unlock();
		return false;
	}
	
	// Write serialized data to file
	QByteArray jsonData = archive.GetData();
	qint64 bytesWritten = file.write(jsonData);
	if (bytesWritten <= 0){
		qWarning() << "Failed to write data to file:" << filePath;
		file.close();
		lockFile.unlock();
		return false;
	}
	
	// Close file
	file.close();
	
	// Release the lock
	lockFile.unlock();
	
	return true;
}


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
	iser::CJsonMemReadArchive archive(jsonData);
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
	iser::CJsonMemWriteArchive archive(nullptr);
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


bool CQmlGenTools::AppendFoldersToGenerationResultFile(const QString& filePath, const QStringList& additionalFolders)
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
	
	// Create lock file to prevent concurrent access
	QString lockFilePath = filePath + ".lock";
	QLockFile lockFile(lockFilePath);
	
	// Try to acquire the lock with a timeout
	if (!lockFile.tryLock(5000)){
		qWarning() << "Failed to acquire lock for file:" << filePath;
		return false;
	}
	
	// Read existing file if it exists
	CSdlQmlGenerationResult result;
	bool fileExists = QFile::exists(filePath);
	
	if (fileExists){
		QFile file(filePath);
		if (!file.open(QFile::ReadOnly | QFile::Text)){
			qWarning() << "Failed to open file for reading:" << filePath << "Error:" << file.errorString();
			lockFile.unlock();
			return false;
		}
		
		QByteArray jsonData = file.readAll();
		file.close();
		
		// Deserialize using CJsonMemReadArchive
		iser::CJsonMemReadArchive archive(jsonData);
		if (!result.Serialize(archive)){
			qWarning() << "Failed to deserialize generation result from file:" << filePath;
			lockFile.unlock();
			return false;
		}
	}
	
	// Append additional folders to existing list
	QStringList existingFolders = result.GetCreatedFolders();
	for (const QString& folder : additionalFolders){
		if (!existingFolders.contains(folder)){
			existingFolders.append(folder);
		}
	}
	result.SetCreatedFolders(existingFolders);
	
	// Update creation date
	result.SetCreatedAt(QDateTime::currentDateTimeUtc());
	
	// Write updated result back to file
	QFile file(filePath);
	if (!file.open(QFile::WriteOnly | QFile::Text)){
		qWarning() << "Failed to open file for writing:" << filePath << "Error:" << file.errorString();
		lockFile.unlock();
		return false;
	}
	
	// Serialize using CJsonMemWriteArchive
	iser::CJsonMemWriteArchive writeArchive(nullptr);
	if (!result.Serialize(writeArchive)){
		qWarning() << "Failed to serialize generation result for file:" << filePath;
		file.close();
		lockFile.unlock();
		return false;
	}
	
	// Write serialized data to file
	QByteArray jsonData = writeArchive.GetData();
	qint64 bytesWritten = file.write(jsonData);
	if (bytesWritten <= 0){
		qWarning() << "Failed to write data to file:" << filePath;
		file.close();
		lockFile.unlock();
		return false;
	}
	
	// Close file
	file.close();
	
	// Release the lock
	lockFile.unlock();
	
	return true;
}


} // namespace imtsdlgenqml
