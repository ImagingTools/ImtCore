// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IChangeable.h>


namespace imtcrypt
{


/**
	Interface for reading encrypted file.
	\ingroup Cryptography
*/
class IEncryptedFilePersistence : virtual public ifile::IFilePersistence
{
public:

	/**
		Get the key data according to specified type.
		\sa KeyType
	*/
	virtual ifile::IFilePersistence::OperationState LoadFromEncryptedFile(const QByteArray& key, const QString& filePath, istd::IChangeable& data) const = 0;
};


} //namespace imtcrypt


