// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TRange.h>


namespace imtservergql
{


/**
 * @brief Interface for handling chunked file uploads.
 * 
 * IFileUploadHandler provides a mechanism for receiving file data in chunks,
 * enabling efficient upload of large files through HTTP or other protocols.
 * This interface is designed to support resumable uploads by accepting
 * file data in arbitrary byte ranges.
 * 
 * @section usage Usage
 * 
 * The typical workflow for using IFileUploadHandler is:
 * 
 * 1. Client begins an upload session (managed externally, e.g., via ICollectionImportController)
 * 2. Client uploads file data in chunks by calling ReceiveFileChunk() multiple times
 * 3. Each chunk specifies its byte range within the complete file
 * 4. The handler merges overlapping/adjacent chunks and tracks upload progress
 * 5. When all chunks are received, the file upload is complete
 * 
 * @section implementation Implementations
 * 
 * - CCollectionImportControllerComp: Handles file uploads for collection imports,
 *   storing temporary files and tracking upload progress
 * 
 * @section protocol Protocol Integration
 * 
 * - CHttpFileUploadServletComp: HTTP servlet that uses this interface to handle
 *   PUT requests with Content-Range headers for chunked uploads
 * 
 * @note File IDs must be established through an external session management
 *       mechanism before calling ReceiveFileChunk().
 * 
 * @see ICollectionImportController
 * @see CCollectionImportControllerComp
 * @see CHttpFileUploadServletComp
 */
class IFileUploadHandler : virtual public istd::IPolymorphic
{
public:
	/**
	 * @brief Status codes returned by ReceiveFileChunk().
	 * 
	 * Status codes indicate whether a chunk was successfully received,
	 * whether more chunks are expected, or if an error occurred.
	 * 
	 * @note The enum name contains a typo (Filel instead of File) but is
	 *       preserved for backward compatibility with existing code.
	 */
	enum FilelUploadStatus
	{
		/**
		 * @brief Chunk received successfully, more chunks expected.
		 * 
		 * The file chunk was written successfully, but the file upload
		 * is not yet complete. Additional chunks are expected.
		 */
		FUS_OK_PARTIAL = 0,

		/**
		 * @brief Chunk received successfully, upload is complete.
		 * 
		 * The file chunk was written successfully and all expected
		 * data has been received. The file upload is now complete.
		 */
		FUS_OK_COMPLETE,

		/**
		 * @brief The provided file ID is not recognized.
		 * 
		 * The fileId parameter does not correspond to any active
		 * upload session. The file ID must be established through
		 * session management before uploading chunks.
		 */
		FUS_INVALID_FILE_ID,

		/**
		 * @brief Failed to open the target file for writing.
		 * 
		 * The system was unable to open the temporary file associated
		 * with the file ID. This may indicate a file system error or
		 * that the temporary file was deleted.
		 */
		FUS_FAILED_TO_OPEN,

		/**
		 * @brief Failed to seek to the chunk's position in the file.
		 * 
		 * The system was unable to move to the byte offset specified
		 * by the range parameter. This may indicate a corrupted file
		 * or file system error.
		 */
		FUS_FAILED_TO_SEEK,

		/**
		 * @brief Failed to write the chunk data to the file.
		 * 
		 * The system was unable to write the provided data to the file.
		 * This may indicate insufficient disk space or file system errors.
		 */
		FUS_FAILED_TO_WRITE,

		/**
		 * @brief The chunk range is outside the expected file size.
		 * 
		 * The byte range specified by the range parameter extends beyond
		 * the expected total file size that was declared when the upload
		 * session was initiated.
		 */
		FUS_CHUNK_OUT_OF_RANGE
	};

	/**
	 * @brief Receive and store a chunk of file data.
	 * 
	 * This method receives a chunk of file data and writes it to the appropriate
	 * position in the target file. Chunks may be received in any order and may
	 * overlap with previously received chunks. The implementation is responsible
	 * for merging overlapping ranges and tracking which portions of the file
	 * have been received.
	 * 
	 * @param fileId Unique identifier for the file being uploaded. This ID must
	 *               have been established through a prior session initiation
	 *               (e.g., ICollectionImportController::BeginCollectionImportSession).
	 * 
	 * @param range Byte range within the complete file where this chunk should
	 *              be written. The range is inclusive: [minValue, maxValue].
	 *              For example, range [0, 1023] represents the first 1024 bytes.
	 * 
	 * @param data The actual bytes to write. The size of this data should match
	 *             the length of the range parameter (range.GetLength() == data.size()).
	 * 
	 * @return Status code indicating success or failure:
	 *         - FUS_OK_PARTIAL: Chunk written successfully, more chunks expected
	 *         - FUS_OK_COMPLETE: Chunk written successfully, upload complete
	 *         - FUS_INVALID_FILE_ID: The fileId is not recognized
	 *         - FUS_FAILED_TO_OPEN: Unable to open the target file
	 *         - FUS_FAILED_TO_SEEK: Unable to seek to the specified range
	 *         - FUS_FAILED_TO_WRITE: Unable to write the data
	 *         - FUS_CHUNK_OUT_OF_RANGE: The range exceeds the expected file size
	 * 
	 * @note This method should be thread-safe as it may be called concurrently
	 *       for different files in a multi-threaded server environment.
	 * 
	 * @note Implementations should validate that:
	 *       - The range is within the declared file size
	 *       - The data size matches the range length
	 *       - The file ID is valid and associated with an active session
	 * 
	 * @par Example
	 * @code
	 * // Upload a file in 1MB chunks
	 * IFileUploadHandler* handler = ...;
	 * QByteArray fileId = "unique-file-id";
	 * QFile file("largefile.dat");
	 * file.open(QFile::ReadOnly);
	 * 
	 * const int chunkSize = 1024 * 1024; // 1MB
	 * qint64 offset = 0;
	 * 
	 * while (!file.atEnd()) {
	 *     QByteArray chunk = file.read(chunkSize);
	 *     istd::CIntRange range(offset, offset + chunk.size() - 1);
	 *     
	 *     FilelUploadStatus status = handler->ReceiveFileChunk(fileId, range, chunk);
	 *     
	 *     if (status >= FUS_INVALID_FILE_ID) {
	 *         // Error occurred
	 *         qWarning() << "Upload failed with status:" << status;
	 *         break;
	 *     }
	 *     
	 *     if (status == FUS_OK_COMPLETE) {
	 *         qDebug() << "Upload complete!";
	 *         break;
	 *     }
	 *     
	 *     offset += chunk.size();
	 * }
	 * @endcode
	 */
	virtual FilelUploadStatus ReceiveFileChunk(
		const QByteArray& fileId,
		const istd::CIntRange& range,
		const QByteArray& data) = 0;
};


} // namespace imtservergql


