#pragma once


// ACF includes
#include <ifile/ITempFileManager.h>
#include <ilog/TLoggerCompWrap.h>
#include <istd/TRange.h>

// ImtCore includes
#include <imtrest/CHttpServletCompBase.h>
#include <imtrest/CHttpResponse.h>
#include <imtbase/IProgressLoggerProvider.h>


namespace imtrest
{


class CHttpTempFileStorageServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CHttpTempFileStorageServletComp)
		I_ASSIGN(m_fileManagerCompPtr, "TempFileManager", "Temp file manager for ", true, "TempFileManager");
		I_ASSIGN(m_progressLoggerProviderCompPtr, "ProgressLoggerProvider", "Progress logger provider", false, "ProgressLoggerProvider");
	I_END_COMPONENT

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual ConstResponsePtr OnPut(
		const QByteArray& commandId,
		const imtrest::IRequest::CommandParams& commandParams,
		const HeadersMap& headers,
		const imtrest::CHttpRequest& request) const override;

	// reimplemented (icomp::CComponentCreated)
	virtual void OnComponentCreated() override;

private:
	enum ProcessingStatus
	{
		PS_OK,
		PS_FAILED_TO_INIT_SESSION,
		PS_FAILED_TO_INIT_FILE,
		PS_EMPTY_FILE_PATH,
		PS_FAILED_TO_OPEN,
		PS_FAILED_TO_SEEK,
		PS_FAILED_TO_WRITE,
		PS_CHUNK_OUT_OF_RANGE
	};

	struct FileInfo
	{
		int size = 0;
		QVector<istd::CIntRange> uploadedChunks;
	};

	struct RangeData
	{
		QString units;
		qsizetype startIndex = -1;
		qsizetype endIndex = -1;
		qsizetype totalSize = -1;
	};

private:
	RangeData GetRangeDataFromString(const QString& dataString) const;
	ProcessingStatus PutFileChunk(
		const QByteArray& sessionId,
		const QByteArray& fileId,
		const QString fileName,
		int fileSize,
		const istd::CIntRange& range,
		const QByteArray& data) const;
	ProcessingStatus AddFileChunk(const QByteArray& fileId, const istd::CIntRange& range) const;
	double GetFileProgress(const QByteArray& fileId) const;
	void UpdateUploadProgress(const QByteArray& fileId) const;
	CHttpResponse::Headers GetRequestHeaders(const CHttpRequest& request) const;
	ConstResponsePtr CreateResponse(
		const IRequest& request,
		int statusCode,
		const imtrest::CHttpResponse::Headers& headers,
		const QByteArray& data,
		const QByteArray& dataTypeId) const;

private:
	I_REF(ifile::ITempFileManager, m_fileManagerCompPtr);
	I_REF(imtbase::IProgressLoggerProvider, m_progressLoggerProviderCompPtr);

	mutable QMap<QByteArray, FileInfo> m_uploadInfos;

	static QByteArray s_plainTextReponseTypeId;
	static QByteArray s_octetStreamTypeId;
};


} // namespace imtrest


