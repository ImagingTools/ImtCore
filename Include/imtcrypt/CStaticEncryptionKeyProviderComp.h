#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CModelUpdateBridge.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtcrypt/IEncryptionKeysProvider.h>


namespace imtcrypt
{


/**
	\ingroup Cryptography
*/
class CStaticEncryptionKeyProviderComp: public icomp::CComponentBase, virtual public imtcrypt::IEncryptionKeysProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStaticEncryptionKeyProviderComp);
		I_REGISTER_INTERFACE(imtcrypt::IEncryptionKeysProvider);
		I_ASSIGN(m_publicKeyCompPtr, "PublicKey", "Public key for RSA algorithm", false, "PublicKey");
		I_ASSIGN(m_privateKeyCompPtr, "PrivateKey", "Private key for RSA algorithm", false, "PrivateKey");
		I_ASSIGN(m_passwordCompPtr, "Password", "Password key for AES algorithm", false, "Password");
		I_ASSIGN(m_initializationVectorCompPtr, "InitializationVector", "Initialization vector key für AES algorithm", false, "InitializationVector");
	I_END_COMPONENT;

	CStaticEncryptionKeyProviderComp();

protected:
	// reimplemented (IEncryptionKeysProvider)
	virtual QByteArray GetEncryptionKey(KeyType type) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(iprm::IIdParam, m_publicKeyCompPtr);
	I_REF(iprm::IIdParam, m_privateKeyCompPtr);
	I_REF(iprm::IIdParam, m_passwordCompPtr);
	I_REF(iprm::IIdParam, m_initializationVectorCompPtr);

	imod::CModelUpdateBridge m_updateBridge;
};


} //namespace imtcrypt


