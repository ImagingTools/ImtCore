// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtfile/IFileCompression.h>


namespace imtzip
{


/**
	Basic realization of the imtfile::IFileCompression interface.
*/
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


