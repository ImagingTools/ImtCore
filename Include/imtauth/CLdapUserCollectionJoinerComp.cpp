// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CLdapUserCollectionJoinerComp.h>


// ACF includes
#include <istd/CChangeGroup.h>
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauth
{


// public methods

CLdapUserCollectionJoinerComp::CLdapUserCollectionJoinerComp()
	:m_ldapUserCollectionObserver(*this),
	m_isBlocked(true)
{
}


QByteArray CLdapUserCollectionJoinerComp::GetUserUuidByLogin(const QByteArray& login) const
{
	iprm::CParamsSet filterParam;
	iprm::CParamsSet paramsSet;

	iprm::CTextParam userIdParam;
	userIdParam.SetText(login);

	paramsSet.SetEditableParameter("Id", &userIdParam);
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (userIds.isEmpty()){
		return QByteArray();
	}

	return userIds[0];
}


QByteArray CLdapUserCollectionJoinerComp::GetLoginByUserUuid(const QByteArray& userUuid) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userUuid, dataPtr)){
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			return userInfoPtr->GetId();
		}
	}

	return QByteArray();
}


// reimplemented (icomp::CComponentBase)

void CLdapUserCollectionJoinerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_ldapUserCollectionObserver.RegisterObject(m_ldapUserCollectionCompPtr.GetPtr(), &CLdapUserCollectionJoinerComp::OnUpdate);

	m_isBlocked = false;
}


void CLdapUserCollectionJoinerComp::OnComponentDestroyed()
{
	m_ldapUserCollectionObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


void CLdapUserCollectionJoinerComp::OnUpdate(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IObjectCollection* /*objectCollectionPtr*/)
{
	if (m_isBlocked){
		return;
	}

	istd::CChangeGroup changeGroup(m_userCollectionCompPtr.GetPtr());

	imtbase::ICollectionInfo::Ids actualLdapUserIds = m_ldapUserCollectionCompPtr->GetElementIds();

	iprm::CParamsSet filterParam;
	iprm::CParamsSet paramsSet;
	iprm::CTextParam systemId;
	systemId.SetText(*m_ldapSystemIdAttrPtr);
	paramsSet.SetEditableParameter("SystemId", &systemId);
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	imtbase::IObjectCollection::Ids storedLdapUserIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);

	// Remove missing LDAP users from internal collection
	for (const QByteArray& userUuid : storedLdapUserIds){
		QByteArray login = GetLoginByUserUuid(userUuid);
		if (!actualLdapUserIds.contains(login)){
			imtbase::ICollectionInfo::Ids elementIds;
			elementIds << userUuid;
			m_userCollectionCompPtr->RemoveElements(elementIds);
		}
	}
}


} // namespace imtauth


