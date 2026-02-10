// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		
		\param result The generation result object containing metadata to save
		\param filePath Full path to the JSON file where the result should be saved
		\returns true if the operation succeeded, false otherwise
		\note Non-const reference required because Serialize() method is not const
	*/
	static bool WriteGenerationResultFile(CSdlQmlGenerationResult& result, const QString& filePath);
	
	/**
		\brief Appends additional directories to existing generation result file with file locking protection.
		
		\details This method reads the existing generation result file, adds new directories from the provided
		update data to the existing set, updates the creation timestamp, and writes the updated result back to the
		file. The entire operation (read, modify, write) is protected by a file lock to prevent concurrent access issues.
		
		If the file doesn't exist, it creates a new one with the data from updateData.
		
		\param filePath Full path to the JSON file where the result should be updated
		\param updateData Generation result object containing folders to append and optionally a creation timestamp
		\returns true if the operation succeeded, false otherwise
		\note If updateData contains a valid creation timestamp, it will be used; otherwise, the current UTC time is set
	*/
	static bool UpdateGenerationResult(const QString& filePath, const CSdlQmlGenerationResult& updateData);
};


} // namespace imtsdlgenqml
