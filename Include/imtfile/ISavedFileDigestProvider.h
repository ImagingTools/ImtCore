// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtfile
{


/**
	Digest of the contents that the last successful save wrote to a file.
	The file can be changed by another program right after the save: the digest tells the own contents from the contents of the other
	program, reading the file back cannot.
*/
class ISavedFileDigestProvider: virtual public istd::IPolymorphic
{
public:
	/**
		SHA-256 of the whole contents that the last successful save of this object wrote to the file.
		\return empty if this object has not saved the file.
	*/
	virtual QByteArray GetSavedFileDigest(const QString& filePath) const = 0;
};


} // namespace imtfile


