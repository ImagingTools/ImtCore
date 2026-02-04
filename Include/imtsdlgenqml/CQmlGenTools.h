#pragma once


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtsdlgenqml/CSdlQmlGenerationResult.h>


namespace imtsdlgenqml
{


/**
	\brief Utility class providing convenient methods for managing SDL QML code generation metadata.
 */
class CQmlGenTools
{
public:
	/**
		\brief Updates generation result metadata file with file locking protection.
		
		\details This is the primary method for working with generation result files. It creates a new file
		if it doesn't exist or updates an existing one. The entire operation (open/create, write, close)
		is protected by a file lock to prevent concurrent access issues.
		
		Uses iser::CJsonMemWriteArchive for serialization to ensure proper data formatting.
		
		\param result The generation result object containing metadata to save
		\param filePath Full path to the JSON file where the result should be saved
		\returns true if the operation succeeded, false otherwise
		\note Non-const reference required because Serialize() method is not const
	*/
	static bool UpdateGenerationResultFile(CSdlQmlGenerationResult& result, const QString& filePath);
	
	/**
		\brief Reads generation result metadata from a JSON file.
		
		\details This method reads and deserializes the CSdlQmlGenerationResult object from a JSON file
		using iser::CJsonMemReadArchive.
		
		\param result The generation result object to populate with loaded data
		\param filePath Full path to the JSON file to read from
		\returns true if the operation succeeded, false otherwise
	*/
	static bool ReadGenerationResultFile(CSdlQmlGenerationResult& result, const QString& filePath);
	
	/**
		\brief Writes generation result metadata to a JSON file.
		
		\details This is a helper method that writes the result to file using iser::CJsonMemWriteArchive.
		For complete file safety, use UpdateGenerationResultFile instead.
		
		\param result The generation result object containing metadata to save
		\param filePath Full path to the JSON file where the result should be saved
		\returns true if the operation succeeded, false otherwise
		\note Non-const reference required because Serialize() method is not const
	*/
	static bool WriteGenerationResultFile(CSdlQmlGenerationResult& result, const QString& filePath);
	
	/**
		\brief Appends additional directories to existing generation result file with file locking protection.
		
		\details This method reads the existing generation result file, adds new directories to the existing set,
		updates the creation timestamp, and writes the updated result back to the file. The entire operation
		(read, modify, write) is protected by a file lock to prevent concurrent access issues.
		
		If the file doesn't exist, it creates a new one with the provided directories.
		
		\param filePath Full path to the JSON file where the result should be updated
		\param additionalFolders Set of folder paths to append to the existing folders
		\returns true if the operation succeeded, false otherwise
	*/
	static bool AppendFoldersToGenerationResultFile(const QString& filePath, const QSet<QString>& additionalFolders);
};


} // namespace imtsdlgenqml
