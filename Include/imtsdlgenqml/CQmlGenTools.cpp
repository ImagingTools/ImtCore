#include <imtsdlgenqml/CQmlGenTools.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QLockFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>


namespace imtsdlgenqml
{


bool CQmlGenTools::SaveGenerationResultToFile(const CSdlQmlGenerationResult& result, const QString& filePath)
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
	
	// Convert result to JSON
	QJsonObject jsonObject = ResultToJson(result);
	QJsonDocument jsonDoc(jsonObject);
	
	// Write to file
	QFile file(filePath);
	if (!file.open(QFile::WriteOnly | QFile::Text)){
		lockFile.unlock();
		return false;
	}
	
	qint64 bytesWritten = file.write(jsonDoc.toJson(QJsonDocument::Indented));
	file.close();
	
	// Release the lock
	lockFile.unlock();
	
	return bytesWritten > 0;
}


bool CQmlGenTools::LoadGenerationResultFromFile(CSdlQmlGenerationResult& result, const QString& filePath)
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
	
	// Parse JSON
	QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
	if (jsonDoc.isNull() || !jsonDoc.isObject()){
		return false;
	}
	
	// Populate result from JSON
	return JsonToResult(result, jsonDoc.object());
}


QJsonObject CQmlGenTools::ResultToJson(const CSdlQmlGenerationResult& result)
{
	QJsonObject jsonObject;
	
	// Serialize creation timestamp
	QDateTime createdAt = result.GetCreatedAt();
	if (createdAt.isValid()){
		jsonObject["createdAt"] = createdAt.toString(Qt::ISODate);
	}
	
	// Serialize generator version
	QString generatorVersion = result.GetGeneratorVersion();
	if (!generatorVersion.isEmpty()){
		jsonObject["generatorVersion"] = generatorVersion;
	}
	
	// Serialize created folders
	QStringList createdFolders = result.GetCreatedFolders();
	if (!createdFolders.isEmpty()){
		QJsonArray foldersArray;
		for (const QString& folder : createdFolders){
			foldersArray.append(folder);
		}
		jsonObject["createdFolders"] = foldersArray;
	}
	
	return jsonObject;
}


bool CQmlGenTools::JsonToResult(CSdlQmlGenerationResult& result, const QJsonObject& jsonObject)
{
	// Deserialize creation timestamp
	if (jsonObject.contains("createdAt")){
		QString createdAtStr = jsonObject["createdAt"].toString();
		QDateTime createdAt = QDateTime::fromString(createdAtStr, Qt::ISODate);
		if (createdAt.isValid()){
			result.SetCreatedAt(createdAt);
		}
	}
	
	// Deserialize generator version
	if (jsonObject.contains("generatorVersion")){
		result.SetGeneratorVersion(jsonObject["generatorVersion"].toString());
	}
	
	// Deserialize created folders
	if (jsonObject.contains("createdFolders") && jsonObject["createdFolders"].isArray()){
		QJsonArray foldersArray = jsonObject["createdFolders"].toArray();
		QStringList folders;
		for (const QJsonValue& folderValue : foldersArray){
			if (folderValue.isString()){
				folders.append(folderValue.toString());
			}
		}
		result.SetCreatedFolders(folders);
	}
	
	return true;
}


} // namespace imtsdlgenqml
