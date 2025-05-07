#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TRange.h>


namespace imtservergql
{


class IFileUploadHandler : virtual public istd::IPolymorphic
{
public:
	enum FilelUploadStatus
	{
		FUS_OK_PARTIAL = 0,
		FUS_OK_COMPLETE,

		FUS_INVALID_FILE_ID,
		FUS_FAILED_TO_OPEN,
		FUS_FAILED_TO_SEEK,
		FUS_FAILED_TO_WRITE,
		FUS_CHUNK_OUT_OF_RANGE
	};

	virtual FilelUploadStatus ReceiveFileChunk(
		const QByteArray& fileId,
		const istd::CIntRange& range,
		const QByteArray& data) = 0;
};


} // namespace imtservergql


