#include <imtcrypt/CStaticEncryptionKeyProviderComp.h>


namespace imtcrypt
{


// public methods

CStaticEncryptionKeyProviderComp::CStaticEncryptionKeyProviderComp()
	:m_updateBridge(this, imod::CModelUpdateBridge::UF_SOURCE)
{
}


// reimplemented (IEncryptionKeysProvider)

QByteArray CStaticEncryptionKeyProviderComp::GetEncryptionKey(KeyType type) const
{
	switch (type){
		case KT_PUBLIC:
			if (m_publicKeyCompPtr.IsValid()){
				return m_publicKeyCompPtr->GetId();
			}
			break;

		case KT_PRIVATE:
			if (m_privateKeyCompPtr.IsValid()){
				return m_privateKeyCompPtr->GetId();
			}
			break;

		case KT_PASSWORD:
			if (m_passwordCompPtr.IsValid()){
				return m_passwordCompPtr->GetId();
			}
			break;

		case KT_INIT_VECTOR:
			if (m_initializationVectorCompPtr.IsValid()){
				return m_initializationVectorCompPtr->GetId();
			}
			break;

		default:
			Q_ASSERT(false);
			break;
	}

	return QByteArray();
}


// reimplemented (icomp::CComponentBase)

void CStaticEncryptionKeyProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	imod::IModel* publicKeyModelPtr = dynamic_cast<imod::IModel*>(m_publicKeyCompPtr.GetPtr());
	if (publicKeyModelPtr != nullptr){
		publicKeyModelPtr->AttachObserver(&m_updateBridge);
	}

	imod::IModel* privateKeyModelPtr = dynamic_cast<imod::IModel*>(m_privateKeyCompPtr.GetPtr());
	if (privateKeyModelPtr != nullptr){
		privateKeyModelPtr->AttachObserver(&m_updateBridge);
	}

	imod::IModel* passwordModelPtr = dynamic_cast<imod::IModel*>(m_passwordCompPtr.GetPtr());
	if (passwordModelPtr != nullptr){
		passwordModelPtr->AttachObserver(&m_updateBridge);
	}

	imod::IModel* initializationVectorModelPtr = dynamic_cast<imod::IModel*>(m_initializationVectorCompPtr.GetPtr());
	if (initializationVectorModelPtr != nullptr){
		initializationVectorModelPtr->AttachObserver(&m_updateBridge);
	}
}


void CStaticEncryptionKeyProviderComp::OnComponentDestroyed()
{
	m_updateBridge.EnsureModelsDetached();
	
	BaseClass::OnComponentDestroyed();
}


} //namespace imtcrypt


