#include <imtsdlgenqml/CQmlGenTools.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QLockFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

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
			return false;
		}
	}
	
	// Create lock file to prevent concurrent access
	QString lockFilePath = filePath + ".lock";
	QLockFile lockFile(lockFilePath);
	
	// Try to acquire the lock with a timeout
	if (!lockFile.tryLock(5000)){
		// Failed to acquire lock within timeout
		return false;
	}
	
	// The entire update operation is now protected by the lock
	bool success = false;
	
	// Check if file exists - if yes, open for writing; if no, create it
	QFile file(filePath);
	if (!file.open(QFile::WriteOnly | QFile::Text)){
		lockFile.unlock();
		return false;
	}
	
	// Serialize using CJsonMemWriteArchive
	// NOTE: result is non-const because Serialize() method is not const
	iser::CJsonMemWriteArchive archive(nullptr);
	if (result.Serialize(archive)){
		// Write serialized data to file
		QByteArray jsonData = archive.GetData();
		qint64 bytesWritten = file.write(jsonData);
		success = (bytesWritten > 0);
	}
	
	// Close file
	file.close();
	
	// Release the lock
	lockFile.unlock();
	
	return success;
}


bool CQmlGenTools::ReadGenerationResultFile(CSdlQmlGenerationResult& result, const QString& filePath)
{
	// Check if file exists
	if (!QFile::exists(filePath)){
		return false;
	}
	
	// Read file
	QFile file(filePath);
	if (!file.open(QFile::ReadOnly | QFile::Text)){
		return false;
	}
	
	QByteArray jsonData = file.readAll();
	file.close();
	
	// Deserialize using CJsonMemReadArchive
	iser::CJsonMemReadArchive archive(jsonData);
	if (!result.Serialize(archive)){
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
			return false;
		}
	}
	
	// Open file for writing
	QFile file(filePath);
	if (!file.open(QFile::WriteOnly | QFile::Text)){
		return false;
	}
	
	// Serialize using CJsonMemWriteArchive
	// NOTE: result is non-const because Serialize() method is not const
	iser::CJsonMemWriteArchive archive(nullptr);
	if (!result.Serialize(archive)){
		file.close();
		return false;
	}
	
	// Write serialized data to file
	QByteArray jsonData = archive.GetData();
	qint64 bytesWritten = file.write(jsonData);
	file.close();
	
	return bytesWritten > 0;
}


} // namespace imtsdlgenqml
