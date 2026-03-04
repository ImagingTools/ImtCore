// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtzip/CFileCompressor.h>


// Quazip includes
#include <quazip/JlCompress.h>

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


