# imtservergql Module

## Overview

The `imtservergql` module provides server-side GraphQL and REST API components for the ImtCore framework. It contains controllers, publishers, and handlers that enable client-server communication through GraphQL subscriptions, queries, mutations, and REST endpoints.

## Key Interfaces

### IFileUploadHandler

**Purpose**: Handles chunked file uploads, enabling efficient transfer of large files.

**Location**: `Include/imtservergql/IFileUploadHandler.h`

**Description**: IFileUploadHandler provides a mechanism for receiving file data in chunks through the `ReceiveFileChunk()` method. This interface supports resumable uploads by accepting file data in arbitrary byte ranges, making it suitable for large file transfers over unreliable networks.

#### Key Features

- **Chunked Upload**: Files can be uploaded in multiple chunks of arbitrary size
- **Out-of-Order Upload**: Chunks can be received in any order
- **Overlap Handling**: Overlapping or duplicate chunks are automatically merged
- **Progress Tracking**: Upload progress can be monitored through external mechanisms
- **Error Reporting**: Detailed status codes indicate success or specific failure reasons

#### Status Codes

- `FUS_OK_PARTIAL`: Chunk received successfully, more chunks expected
- `FUS_OK_COMPLETE`: Chunk received successfully, upload is complete
- `FUS_INVALID_FILE_ID`: The provided file ID is not recognized
- `FUS_FAILED_TO_OPEN`: Failed to open the target file for writing
- `FUS_FAILED_TO_SEEK`: Failed to seek to the chunk's position in the file
- `FUS_FAILED_TO_WRITE`: Failed to write the chunk data to the file
- `FUS_CHUNK_OUT_OF_RANGE`: The chunk range exceeds the expected file size

#### Implementation

**CCollectionImportControllerComp** implements IFileUploadHandler to handle file uploads for collection imports:

- Manages temporary files for uploaded data
- Tracks which byte ranges have been received
- Reports upload progress through the progress manager system
- Triggers import jobs when all files are fully uploaded

#### Protocol Integration

**CHttpFileUploadServletComp** (in the `imtrest` module) provides HTTP integration:

- Handles HTTP PUT requests with `Content-Range` headers
- Extracts file metadata from custom HTTP headers:
  - `X-Session-Id`: Import session identifier
  - `X-File-Id`: Unique file identifier
  - `X-File-Name`: Original file name
- Maps HTTP status codes to upload status:
  - 206 Partial Content: More chunks expected (FUS_OK_PARTIAL)
  - 201 Created: Upload complete (FUS_OK_COMPLETE)
  - 500 Internal Server Error: Upload failed

#### Usage Example

```cpp
// 1. Begin an import session
ICollectionImportController* controller = ...;
ICollectionImportController::SessionInfo sessionInfo;
sessionInfo.sessionId = "session-123";
sessionInfo.collectionId = "collection-456";
sessionInfo.userId = "user-789";

ICollectionImportController::FileInfo fileInfo;
fileInfo.id = "file-001";
fileInfo.name = "document.pdf";
fileInfo.size = 5242880; // 5MB
sessionInfo.files.push_back(fileInfo);

QString errorMessage;
if (!controller->BeginCollectionImportSession(sessionInfo, errorMessage)) {
    qWarning() << "Failed to begin session:" << errorMessage;
    return;
}

// 2. Upload file chunks
IFileUploadHandler* uploadHandler = dynamic_cast<IFileUploadHandler*>(controller);
QFile file("document.pdf");
file.open(QFile::ReadOnly);

const int chunkSize = 1024 * 1024; // 1MB chunks
qint64 offset = 0;

while (!file.atEnd()) {
    QByteArray chunk = file.read(chunkSize);
    istd::CIntRange range(offset, offset + chunk.size() - 1);
    
    IFileUploadHandler::FilelUploadStatus status = 
        uploadHandler->ReceiveFileChunk(fileInfo.id, range, chunk);
    
    if (status >= IFileUploadHandler::FUS_INVALID_FILE_ID) {
        qWarning() << "Upload failed with status:" << status;
        break;
    }
    
    if (status == IFileUploadHandler::FUS_OK_COMPLETE) {
        qDebug() << "Upload complete!";
        break;
    }
    
    offset += chunk.size();
}
```

#### HTTP Upload Example

Using curl to upload a file in chunks:

```bash
# Upload first chunk (bytes 0-1048575)
curl -X PUT \
  -H "Content-Range: bytes 0-1048575/5242880" \
  -H "X-Session-Id: session-123" \
  -H "X-File-Id: file-001" \
  -H "X-File-Name: document.pdf" \
  --data-binary "@chunk1.bin" \
  http://server/api/upload

# Upload second chunk (bytes 1048576-2097151)
curl -X PUT \
  -H "Content-Range: bytes 1048576-2097151/5242880" \
  -H "X-Session-Id: session-123" \
  -H "X-File-Id: file-001" \
  -H "X-File-Name: document.pdf" \
  --data-binary "@chunk2.bin" \
  http://server/api/upload

# ... continue until all chunks uploaded
```

#### Architecture

```
Client Application
       |
       | HTTP PUT with Content-Range
       v
CHttpFileUploadServletComp (imtrest)
       |
       | ReceiveFileChunk()
       v
IFileUploadHandler Interface
       |
       | (implemented by)
       v
CCollectionImportControllerComp (imtservergql)
       |
       |-- ITempFileManager: Manages temporary files
       |-- IProgressSessionsManager: Tracks upload progress
       |-- IJobQueueManager: Triggers import jobs
       v
Import Job Processing
```

### ICollectionImportController

**Purpose**: Manages collection import sessions and coordinates file uploads.

**Location**: `Include/imtservergql/ICollectionImportController.h`

**Description**: ICollectionImportController provides session management for importing files into collections. It works in conjunction with IFileUploadHandler to:

1. Initialize import sessions with file metadata
2. Create temporary storage for uploaded files
3. Monitor upload progress
4. Trigger import jobs when uploads complete
5. Handle session cancellation

## Related Modules

- **imtrest**: HTTP servlet implementations for REST API endpoints
- **imtbase**: Progress management and base utilities
- **imthype**: Job queue management for background processing
- **imtgql**: GraphQL query processing infrastructure

## See Also

- `CCollectionImportControllerComp.h`: Complete implementation of file upload handling
- `CHttpFileUploadServletComp.h`: HTTP protocol adapter for file uploads
- `ICollectionImportJobParamsFiller.h`: Job parameter configuration for imports
