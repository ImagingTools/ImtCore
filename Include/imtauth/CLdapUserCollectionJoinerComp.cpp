#include <imtauth/CLdapUserCollectionJoinerComp.h>


// ACF includes
#include <istd/CChangeGroup.h>
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauth
{


// public methods

// reimplemented (icomp::CComponentBase)

void CLdapUserCollectionJoinerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_ldapUserCollectionModelCompPtr.IsValid()){
		m_ldapUserCollectionModelCompPtr->AttachObserver(this);
	}
}


void CLdapUserCollectionJoinerComp::OnComponentDestroyed()
{
	if (m_ldapUserCollectionModelCompPtr.IsValid()){
		m_ldapUserCollectionModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CLdapUserCollectionJoinerComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	iprm::CParamsSet filterParam;
	iprm::CParamsSet paramsSet;

	iprm::CIdParam systemId;
	systemId.SetId("Ldap");

	paramsSet.SetEditableParameter("SystemId", &systemId);
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	imtbase::IObjectCollection::Ids actualLdapUserIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);

	istd::CChangeGroup changeGroup(m_userCollectionCompPtr.GetPtr());

	QByteArrayList expectedLdapUserIds = m_ldapUserCollectionCompPtr->GetElementIds();

	// Add new LDAP users
	for (const QByteArray& userId : expectedLdapUserIds){
		if (!actualLdapUserIds.contains(userId)){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_ldapUserCollectionCompPtr->GetObjectData(userId, dataPtr)){
				const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
				if (userInfoPtr != nullptr){
					m_userCollectionCompPtr->InsertNewObject("", "", "", userInfoPtr, userId);
				}
			}
		}
	}

	// Remove missing LDAP users
	for (const QByteArray& userId : actualLdapUserIds){
		if (!expectedLdapUserIds.contains(userId)){
			m_userCollectionCompPtr->RemoveElement(userId);
		}
	}
}


} // namespace imtauth


