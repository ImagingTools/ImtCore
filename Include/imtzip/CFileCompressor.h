#pragma once


// ImtCore includes
#include <imtfile/IFileCompression.h>


namespace imtzip
{


class CFileCompressor: virtual public imtfile::IFileCompression
{
public:
	// reimplemented (imtfile::IFileCompression)
	virtual bool CompressFile(const QString& sourceFilePath, const QString& targetFilePath) const override;
	virtual bool DecompressFile(const QString& sourceFilePath, const QString& targetFilePath) const override;
	virtual bool CompressFolder(const QString& sourceFolderPath, const QString& targetFilePath, bool isRecursive) const override;
	virtual bool DecompressFolder(const QString& sourceFilePath, const QString& targetFolderPath) const override;
};


} // namespace imtzip


