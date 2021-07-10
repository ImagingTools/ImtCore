#include <imtcrypt/CSrp.h>


// Qt includes
#include <QtCore/QCryptographicHash>

// OpenSSL includes
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/kdf.h>

// std includes
#include <iostream>
#include <vector>


namespace imtcrypt {


static const QByteArray __awsAuthSrpPrimeN
	= "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1"
	"29024E088A67CC74020BBEA63B139B22514A08798E3404DD"
	"EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245"
	"E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"
	"EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3D"
	"C2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F"
	"83655D23DCA3AD961C62F356208552BB9ED529077096966D"
	"670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"
	"E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9"
	"DE2BCBF6955817183995497CEA956AE515D2261898FA0510"
	"15728E5A8AAAC42DAD33170D04507A33A85521ABDF1CBA64"
	"ECFB850458DBEF0A8AEA71575D060C7DB3970F85A6E1E4C7"
	"ABF5AE8CDB0933D71E8C94E04A25619DCEE3D2261AD2EE6B"
	"F12FFA06D98A0864D87602733EC86A64521F2B18177B200C"
	"BBE117577A615D6C770988C0BAD946E208E24FA074E5AB31"
	"43DB5BFCE0FD108E4B82D120A93AD2CAFFFFFFFFFFFFFFFF";

// SHA256 digest for AWS N prime
static const std::vector<uint8_t> s_nPrimeDigest{ 0x53,
	0x82,
	0x82,
	0xc4,
	0x35,
	0x47,
	0x42,
	0xd7,
	0xcb,
	0xbd,
	0xe2,
	0x35,
	0x9f,
	0xcf,
	0x67,
	0xf9,
	0xf5,
	0xb3,
	0xa6,
	0xb0,
	0x87,
	0x91,
	0xe5,
	0x01,
	0x1b,
	0x43,
	0xb8,
	0xa5,
	0xb6,
	0x6d,
	0x9e,
	0xe6 };


CSrp::CSrp()
{
	m_k = BN_new();
	m_random = BN_new();
	m_N = BN_new();
	m_g = BN_new();

	GenerateSrpA();
}


CSrp::~CSrp()
{
	BN_free(m_k);
	BN_free(m_random);
	BN_free(m_N);
	BN_free(m_g);
}


QByteArray CSrp::GeneratePasswordClaim(const QByteArray& userPoolId,
	const QByteArray& username,
	const QByteArray& password,
	const QByteArray& salt,
	const QByteArray& sB,
	const QByteArray& sSecretBlock,
	const QByteArray& timestamp)
{
	QByteArray secretBlock = QByteArray::fromBase64(sSecretBlock);

	QByteArray key;

	GenerateKey(key, userPoolId + username + ":" + password, salt, sB);

	QByteArray content = userPoolId + username + secretBlock + timestamp;

	QByteArray hmac;

	//Hmac::ComputeSha256(hmac, key, content);
	hmac.resize(32);
	HMAC(EVP_sha256(),
		(uchar*)key.data(),
		static_cast<int>(key.size()),
		(uchar*)content.data(),
		static_cast<int>(content.size()),
		(uchar*)hmac.data(),
		NULL);

	return hmac.toBase64();
}


const char* CSrp::A() const
{
	return m_A.data();
}


void CSrp::GenerateSrpA()
{
	BN_rand(m_random, 256, 1, 1);
	BN_hex2bn(&m_N, __awsAuthSrpPrimeN.data());
	BN_hex2bn(&m_g, "2");
	BN_bin2bn(s_nPrimeDigest.data(), s_nPrimeDigest.size(), m_k);

	BN_CTX* context = BN_CTX_new();
	BIGNUM* a = BN_new();
	BIGNUM* A = BN_new();

	BN_mod(a, m_random, m_N, context);
	BN_mod_exp(A, m_g, a, m_N, context);

	m_A = QByteArray(BN_bn2hex(A));

	BN_free(A);
	BN_free(a);
	BN_CTX_free(context);
}


void CSrp::GenerateKey(QByteArray& out,
	const QByteArray& id,
	const QByteArray& sSaltIn,
	const QByteArray& sB)
{
	//Digest d;

	QByteArray ab = QByteArray::fromHex(PadLeftZeroString(m_A) + PadLeftZeroString(sB));

	QByteArray ab_digest;
	//d.Sha256(ab_digest, ab);
	Sha256(ab_digest, ab);
	ab_digest = PadLeftZero(ab_digest);

	QByteArray idDigest;
	//d.Sha256(idDigest, id);
	Sha256(idDigest, id);

	QByteArray saltIn = QByteArray::fromHex(PadLeftZeroString(sSaltIn));

	QByteArray x_array = saltIn + idDigest;
	QByteArray x_digest;
	//d.Sha256(x_digest, x_array);
	Sha256(x_digest, x_array);
	x_digest = PadLeftZero(x_digest);

	BIGNUM* x = BN_new();
	BIGNUM* u = BN_new();
	BIGNUM* B = BN_new();

	BN_bin2bn((uchar*)x_digest.data(), x_digest.size(), x);
	BN_bin2bn((uchar*)ab_digest.data(), ab_digest.size(), u);
	BN_hex2bn(&B, sB);

	BIGNUM* g_mod_xn = BN_new();
	BIGNUM* k_mult = BN_new();
	BIGNUM* b_sub = BN_new();
	BIGNUM* u_x = BN_new();
	BIGNUM* a_add = BN_new();
	BIGNUM* b_sub_modpow = BN_new();
	BIGNUM* S = BN_new();
	BIGNUM* a = BN_new();

	BN_CTX* context = BN_CTX_new();

	BN_mod(a, m_random, m_N, context);
	BN_mod_exp(g_mod_xn, m_g, x, m_N, context);
	BN_mul(k_mult, m_k, g_mod_xn, context);
	BN_sub(b_sub, B, k_mult);
	BN_mul(u_x, u, x, context);
	BN_add(a_add, a, u_x);
	BN_mod_exp(b_sub_modpow, b_sub, a_add, m_N, context);
	BN_mod(S, b_sub_modpow, m_N, context);

	QByteArray salt = QByteArray(BN_bn2hex(u));
	salt = QByteArray::fromHex(salt);
	salt = PadLeftZero(salt);

	QByteArray secret = QByteArray(BN_bn2hex(S));
	secret = QByteArray::fromHex(secret);
	secret = PadLeftZero(secret);

	const QByteArray labelS = "Caldera Derived Key";
	HkdfSha256(out, salt, secret, labelS);

	BN_CTX_free(context);
	BN_free(a);
	BN_free(S);
	BN_free(b_sub_modpow);
	BN_free(a_add);
	BN_free(u_x);
	BN_free(b_sub);
	BN_free(k_mult);
	BN_free(g_mod_xn);
	BN_free(B);
	BN_free(u);
	BN_free(x);
}


QByteArray CSrp::PadLeftZeroString(const QByteArray& hex)
{
	QByteArray result;

	if ((hex.size() & 1) == 1) {
		result = "0" + hex;
	}
	else if (hex[0] > '7') {
		result = "00" + hex;
	}
	else {
		result = hex;
	}

	return result;
}


QByteArray CSrp::PadLeftZero(const QByteArray& v)
{
	QByteArray result;

	if ((uchar)v.at(0) > 0x7f) {
		QByteArray result(v);
		result.insert(0, char(0));

		return result;
	}

	return v;
}


void  CSrp::Sha256(QByteArray& out, const QByteArray& in)
{
	EVP_MD_CTX* context = EVP_MD_CTX_create();
	EVP_DigestInit_ex(context, EVP_sha256(), nullptr);
	EVP_DigestUpdate(context, in.data(), in.size());

	out.resize(EVP_MD_size(EVP_sha256()));
	EVP_DigestFinal_ex(context, (unsigned char*)out.data(), NULL);
	EVP_MD_CTX_free(context);
}


void  CSrp::HkdfSha256(QByteArray& output,
	const QByteArray& salt,
	const QByteArray& secret,
	const QByteArray& label)
{
	EVP_PKEY_CTX* context = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);
	output.resize(16);
	size_t outlen = output.size();
	EVP_PKEY_derive_init(context);
	EVP_PKEY_CTX_set_hkdf_md(context, EVP_sha256());
	EVP_PKEY_CTX_set1_hkdf_salt(
		context, salt.data(), static_cast<int>(salt.size()));
	EVP_PKEY_CTX_set1_hkdf_key(
		context, secret.data(), static_cast<int>(secret.size()));
	EVP_PKEY_CTX_add1_hkdf_info(
		context, label.data(), static_cast<int>(label.size()));
	EVP_PKEY_derive(context, (uchar*)output.data(), &outlen);
	EVP_PKEY_CTX_free(context);
}


}

