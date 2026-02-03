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
		Updates generation result metadata file with file locking protection.
		
		This is the primary method for working with generation result files. It creates a new file
		if it doesn't exist or updates an existing one. The entire operation (open/create, write, close)
		is protected by a file lock to prevent concurrent access issues.
		
		Uses iser::CJsonMemWriteArchive for serialization to ensure proper data formatting.
		
		\param result The generation result object containing metadata to save
		             NOTE: Non-const reference required because Serialize() method is not const
		\param filePath Full path to the JSON file where the result should be saved
		\return true if the operation succeeded, false otherwise
	*/
	static bool UpdateGenerationResultFile(CSdlQmlGenerationResult& result, const QString& filePath);
	
	/**
		Reads generation result metadata from a JSON file.
		
		This method reads and deserializes the CSdlQmlGenerationResult object from a JSON file
		using iser::CJsonMemReadArchive.
		
		\param result The generation result object to populate with loaded data
		\param filePath Full path to the JSON file to read from
		\return true if the operation succeeded, false otherwise
	*/
	static bool ReadGenerationResultFile(CSdlQmlGenerationResult& result, const QString& filePath);
	
	/**
		Writes generation result metadata to a JSON file.
		
		This is a helper method that writes the result to file using iser::CJsonMemWriteArchive.
		For complete file safety, use UpdateGenerationResultFile instead.
		
		\param result The generation result object containing metadata to save
		             NOTE: Non-const reference required because Serialize() method is not const
		\param filePath Full path to the JSON file where the result should be saved
		\return true if the operation succeeded, false otherwise
	*/
	static bool WriteGenerationResultFile(CSdlQmlGenerationResult& result, const QString& filePath);
};


} // namespace imtsdlgenqml
