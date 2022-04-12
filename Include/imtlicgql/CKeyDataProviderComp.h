#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtbase/IBinaryDataProvider.h>
#include <imtbase/IObjectCollection.h>
#include <imtcrypt/IEncryptionKeysProvider.h>


namespace imtlicgql
{


/**
	This component accepts data and returns it in encrypted form
*/
class CKeyDataProviderComp:
		public imtbase::IBinaryDataProvider,
		public ilog::CLoggerComponentBase,
		virtual public imtcrypt::IEncryptionKeysProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CKeyDataProviderComp);
		I_REGISTER_INTERFACE(imtbase::IBinaryDataProvider);
		I_REGISTER_INTERFACE(imtcrypt::IEncryptionKeysProvider);
		I_ASSIGN(m_licensePersistenceCompPtr, "LicensePersistence", "Persistence used for license export", false, "LicensePersistence");
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN(m_vectorKeyCompPtr, "VectorKey", "Additional key for AES encryption", false, "VectorKey");
	I_END_COMPONENT;

	enum CommandGroup
	{
		CG_LICENSE = 2150
	};

	// reimplemented (imtbase::IBinaryDataProvider)
	virtual bool GetData(QByteArray& data, const QByteArray& dataId) const override;

	// reimplemented (imtcrypt::IEncryptionKeysProvider)
	virtual QByteArray GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const override;

private:
	mutable QByteArray m_productInstanceId;

	I_REF(ifile::IFilePersistence, m_licensePersistenceCompPtr);
	I_REF(iprm::IIdParam, m_vectorKeyCompPtr);
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace imtqml


