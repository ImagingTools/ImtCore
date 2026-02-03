#pragma once


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtsdlgenqml/CSdlQmlGenerationResult.h>


namespace imtsdlgenqml
{


/**
	Utility class providing convenient methods for managing SDL QML code generation metadata.
 */
class CQmlGenTools
{
public:
	/**
		Saves or updates generation result metadata to a JSON file with file locking protection.
		
		This method serializes the CSdlQmlGenerationResult object to JSON format and writes it
		to the specified file. The operation is protected by a file lock to prevent concurrent
		access issues.
		
		\param result The generation result object containing metadata to save
		\param filePath Full path to the JSON file where the result should be saved
		\return true if the operation succeeded, false otherwise
	*/
	static bool SaveGenerationResultToFile(const CSdlQmlGenerationResult& result, const QString& filePath);
	
	/**
		Loads generation result metadata from a JSON file.
		
		This method reads and deserializes the CSdlQmlGenerationResult object from a JSON file.
		
		\param result The generation result object to populate with loaded data
		\param filePath Full path to the JSON file to read from
		\return true if the operation succeeded, false otherwise
	*/
	static bool LoadGenerationResultFromFile(CSdlQmlGenerationResult& result, const QString& filePath);

private:
	/**
		Converts CSdlQmlGenerationResult to QJsonObject.
		
		\param result The generation result to convert
		\return QJsonObject representation of the result
	*/
	static QJsonObject ResultToJson(const CSdlQmlGenerationResult& result);
	
	/**
		Populates CSdlQmlGenerationResult from QJsonObject.
		
		\param result The generation result to populate
		\param jsonObject The JSON object to read from
		\return true if successful, false otherwise
	*/
	static bool JsonToResult(CSdlQmlGenerationResult& result, const QJsonObject& jsonObject);
};


} // namespace imtsdlgenqml
