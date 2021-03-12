#include <imtzip/CFileCompressor.h>


// Quazip includes
#include <JlCompress.h>

// Qt includes
#include <QtCore/QString>


namespace imtzip
{


// pulic methods

bool CFileCompressor::CompressFile(const QString& sourceFilePath, const QString& targetFilePath) const
{
	return JlCompress::compressFile(targetFilePath, sourceFilePath);
}


bool CFileCompressor::DecompressFile(const QString& sourceFilePath, const QString& targetFilePath) const
{
	QString filePath = JlCompress::extractFile(sourceFilePath, targetFilePath);
	
	return !filePath.isEmpty();
}


bool CFileCompressor::CompressFolder(const QString& sourceDirPath, const QString& targetFilePath, bool isRecursive) const
{
	return JlCompress::compressDir(targetFilePath, sourceDirPath, isRecursive);
}


bool CFileCompressor::DecompressFolder(const QString& sourceFilePath, const QString& targetFolderPath) const
{
	return (JlCompress::extractDir(sourceFilePath, targetFolderPath).size() != 0);
}


} // namespace imtzip


