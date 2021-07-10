#pragma once


// Qt includes
#include <QtCore/QByteArray>

// openssl includes
#include <openssl/bn.h>


namespace imtcrypt
{


class CSrp
{

public:
	CSrp();

	~CSrp();

	QByteArray GeneratePasswordClaim(const QByteArray& userPoolId,
		const QByteArray& username,
		const QByteArray& password,
		const QByteArray& salt,
		const QByteArray& sB,
		const QByteArray& secretBlock,
		const QByteArray& timestamp);

	const char* A() const;

protected:

	BIGNUM* m_k;
	BIGNUM* m_random;
	BIGNUM* m_N;
	BIGNUM* m_g;

	QByteArray m_A;

	void GenerateSrpA();

	void GenerateKey(QByteArray& out,
		const QByteArray& id,
		const QByteArray& salt,
		const QByteArray& sB);

	QByteArray PadLeftZeroString(const QByteArray& hex);

	QByteArray PadLeftZero(const QByteArray& v);

	void Sha256(QByteArray& out, const QByteArray& in);

	void HkdfSha256(QByteArray& output,
		const QByteArray& salt,
		const QByteArray& secret,
		const QByteArray& label);

};


} // namespace awsx



