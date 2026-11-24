// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <QtCore/QString>


namespace imtfile
{


/**
	Reading of files that never waits: a special file in place of a file (a FIFO, a device) is not opened. The reader of a FIFO would
	wait for a writer for ever, for example in the handler of a file watcher.
*/
class CFileTools
{
public:
	/**
		Open a regular file for reading. A special file is not opened: nothing is waited for.
		The file itself must not be a link: a symbolic link (on Windows also a junction or another reparse point) in place of the file is
		not opened. The links in the folders of the path are followed as usual: to check them is up to the caller. A hard link is a
		regular file and is opened.
		On Unix the opened file itself is checked, it cannot be replaced between the check and the opening. On Windows the path is
		checked before the opening.
		\return \c false if the file cannot be opened or is not a regular file.
	*/
	static bool OpenRegularFile(const QString& filePath, QFile& file);

	/**
		SHA-256 of the whole contents of a regular file (OpenRegularFile).
		\return empty if the file cannot be read or is not a regular file.
	*/
	static QByteArray GetFileDigest(const QString& filePath);
};


} // namespace imtfile


