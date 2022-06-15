// ImtCore includes
#include <imtlicgql/CKeyDataProviderComp.h>
#include <imtlic/IProductInstanceInfo.h>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>
#include <QtCore/QFileInfo>


namespace imtlicgql
{


// public methods

// reimplemented (imtbase::IBinaryDataProvider)

bool CKeyDataProviderComp::GetData(QByteArray& data, const QByteArray& dataId) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	m_productInstanceId = dataId;

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(dataId, dataPtr)){
		imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());

		if (productInstancePtr == nullptr){
			return false;
		}

		if (m_licensePersistenceCompPtr.IsValid()){
			QTemporaryDir tempDir;
			QString filePathTmp = tempDir.path() + "/"  + QUuid::createUuid().toString() + ".xml";
			
			int state = m_licensePersistenceCompPtr->SaveToFile(*productInstancePtr, filePathTmp);
			if (state != ifile::IFilePersistence::OS_OK){
				SendErrorMessage(0, "License file could not be saved", "Server data provider");

				return false;
			}

			QFile file(filePathTmp);

			if (!file.open(QIODevice::ReadOnly )){
				SendErrorMessage(0, "License file could not be opened", "Server data provider");

				return false;
			}

			data = file.readAll();

			file.close();
		}
	}

	return true;
}


// reimplemented (imtcrypt::IEncryptionKeysProvider)

QByteArray CKeyDataProviderComp::GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const
{
	QByteArray retVal;

	if (type == KT_PASSWORD){
		QByteArray id = m_productInstanceId;
		imtbase::IObjectCollection::DataPtr dataPtr;
		m_objectCollectionCompPtr->GetObjectData(id, dataPtr);
		if (dataPtr.IsValid()){
			imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
			Q_ASSERT(productInstanceInfoPtr != nullptr);

			if (productInstanceInfoPtr != nullptr){
				retVal = productInstanceInfoPtr->GetProductInstanceId();
			}
		}
	}
	else if (type == KT_INIT_VECTOR){
		if (m_vectorKeyCompPtr.IsValid()){
			retVal = m_vectorKeyCompPtr->GetId();
		}
	}

	return retVal;
}


} // namespace imtqml


