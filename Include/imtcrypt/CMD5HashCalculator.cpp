#include <imtcrypt/CMD5HashCalculator.h>


// Qt includes
#include <QtCore/QCryptographicHash>


namespace imtcrypt
{


// reimplemented (IEncryption)

QByteArray imtcrypt::CMD5HashCalculator::Calculate(const QByteArray &input) const
{
	QByteArray cryptoHash = QCryptographicHash::hash(input, QCryptographicHash::Md5).toHex();

	return cryptoHash;
}


} //namespace imtcrypt


