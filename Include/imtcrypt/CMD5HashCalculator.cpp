#include <imtcrypt/CMD5HashCalculator.h>


// Qt includes
#include <QtCore/QCryptographicHash>


namespace imtcrypt
{


// public methods

// reimplemented (IEncryption)

QByteArray CMD5HashCalculator::GenerateHash(const QByteArray &input) const
{
	QByteArray cryptoHash = QCryptographicHash::hash(input, QCryptographicHash::Md5).toHex();

	return cryptoHash;
}


bool CMD5HashCalculator::ValidateHash(const QByteArray& inputData, const QByteArray& hashValue) const
{
	bool retVal = false;

	// use GenerateHash because MD5 generates same hash for the same string
	const QByteArray generatedHash = GenerateHash(inputData);
	retVal = (generatedHash == hashValue);

	return retVal;
}


} //namespace imtcrypt




