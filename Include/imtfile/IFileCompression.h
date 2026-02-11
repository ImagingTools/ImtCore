// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtfile
{


class IFileCompression: virtual public istd::IPolymorphic
{
public:
	/**
		Compress a single file.
	*/
	virtual bool CompressFile(const QString& sourceFilePath, const QString& targetFilePath) const = 0;

	/**
		Decompress a single file.
	*/
	virtual bool DecompressFile(const QString& sourceFilePath, const QString& targetFilePath) const = 0;

	/**
		Compress a folder will all files inside.
	*/
	virtual bool CompressFolder(const QString& sourceFolderPath, const QString& targetFilePath, bool isRecursive) const = 0;

	/**
		Decompress a folder.
	*/
	virtual bool DecompressFolder(const QString& sourceFilePath, const QString& targetFolderPath) const = 0;
};


} // namespace imtfile


