// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>


namespace imtfile
{


/**
	Names of files and folders that every common file system takes: NTFS (Windows), ext4 (Linux) and APFS (macOS).
	The same rules are in FileNameTools.js of the QML module imtcontrols.
*/
class CFileNameTools
{
public:
	/**
		Checks a name of a file or a folder (one segment of a path) against the rules all of the file systems share:
		- not empty, not "." and not "..";
		- none of the characters < > : " / \ | ? *, no control characters and no unpaired surrogates;
		- no dot and no space at the end;
		- not a name Windows reserves for devices (CON, PRN, AUX, NUL, COM0-COM9, LPT0-LPT9, also with an extension);
		- at most 255 bytes in UTF-8.
		\param error [OUTPUT] - why the name cannot be used; empty for a valid name
		\return \c true if the name can be used
	*/
	static bool CheckFileName(const QString& name, QString& error);
};


} // namespace imtfile
