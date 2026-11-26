// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CFileTools.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif


namespace imtfile
{


// public static methods

bool CFileTools::OpenRegularFile(const QString& filePath, QFile& file)
{
#ifdef Q_OS_WIN
	// A folder, a junction or a link is not opened. Pipes and devices are not in the folders: what is opened is a file of the disk,
	// a device would be sequential
	const std::wstring nativePath = QDir::toNativeSeparators(filePath).toStdWString();
	const DWORD attributes = ::GetFileAttributesW(nativePath.c_str());
	if ((attributes == INVALID_FILE_ATTRIBUTES) || ((attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) != 0)){
		return false;
	}

	file.setFileName(filePath);
	if (!file.open(QIODevice::ReadOnly)){
		return false;
	}

	if (file.isSequential()){
		file.close();

		return false;
	}

	return true;
#else
	// Opened without waiting (a FIFO without a writer would wait), the opened file itself is checked
	const int fileHandle = ::open(QFile::encodeName(filePath).constData(), O_RDONLY | O_NONBLOCK | O_NOFOLLOW | O_CLOEXEC);
	if (fileHandle < 0){
		return false;
	}

	struct stat info;
	if ((::fstat(fileHandle, &info) != 0) || !S_ISREG(info.st_mode) || !file.open(fileHandle, QIODevice::ReadOnly, QFileDevice::AutoCloseHandle)){
		::close(fileHandle);

		return false;
	}

	return true;
#endif
}


QByteArray CFileTools::GetFileDigest(const QString& filePath)
{
	QFile file;
	if (!OpenRegularFile(filePath, file)){
		return {};
	}

	QCryptographicHash hash(QCryptographicHash::Sha256);
	if (!hash.addData(&file)){
		return {};
	}

	return hash.result();
}


} // namespace imtfile


