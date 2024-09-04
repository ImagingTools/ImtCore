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
	virtual int LoadFromEncryptedFile(const QByteArray& key, const QString& filePath, istd::IChangeable& data) const = 0;
};


} // namespace imtcrypt


