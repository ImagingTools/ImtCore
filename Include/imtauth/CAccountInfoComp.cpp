#include <imtauth/CAccountInfoComp.h>


// ImtCore includes
#include <imtauth/IContactInfo.h>


namespace imtauth
{


// protected methods

CAccountInfoComp::CAccountInfoComp()
	:m_contactCollectionUpdateBinder(*this)
{
}


// reimplemented (icomp::CComponentBase)

void CAccountInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_contactCollectionCompPtr.IsValid() && m_contactCollectionModelCompPtr.IsValid()){
		m_contactCollectionPtr = m_contactCollectionCompPtr.GetPtr();
		m_contactCollectionUpdateBinder.RegisterObject(
					m_contactCollectionCompPtr.GetPtr(),
					&CAccountInfoComp::OnContactCollectionUpdate);
	}
}


void CAccountInfoComp::OnComponentDestroyed()
{
	m_contactCollectionUpdateBinder.UnregisterAllObjects();
}


// private methods

void CAccountInfoComp::OnContactCollectionUpdate(
			const istd::IChangeable::ChangeSet& changeSet,
			const imtbase::IObjectCollection* objectCollectionPtr)
{
	if (!m_contactEMail.isEmpty()){
		imtbase::ICollectionInfo::Ids ids = objectCollectionPtr->GetElementIds();
		for (const QByteArray& id : ids){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (objectCollectionPtr->GetObjectData(id, dataPtr)){
				IContactInfo* contactPtr = dynamic_cast<IContactInfo*>(dataPtr.GetPtr());
				if (contactPtr != nullptr){
					if (contactPtr->GetEMail() == m_contactEMail){
						return;
					}
				}
			}
		}
	}

	SetAccountOwner(QString());
}


} // namespace imtauth


