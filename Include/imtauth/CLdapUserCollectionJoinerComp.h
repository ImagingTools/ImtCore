// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/IModel.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/TModelUpdateBinder.h>


namespace imtauth
{


class CLdapUserCollectionJoinerComp: public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLdapUserCollectionJoinerComp);
		I_ASSIGN(m_userCollectionCompPtr, "InternalUserCollection", "Internal user collection", true, "InternalUserCollection");
		I_ASSIGN(m_ldapUserCollectionCompPtr, "LdapUserCollection", "LDAP user collection", true, "LdapUserCollection");
		I_ASSIGN(m_ldapSystemIdAttrPtr, "LdapSystemId", "LDAP System-ID", false, "Ldap");
		I_ASSIGN(m_ldapSystemNameAttrPtr, "LdapSystemName", "LDAP System Name", false, "LDAP");
	I_END_COMPONENT;

	CLdapUserCollectionJoinerComp();

	QByteArray GetUserUuidByLogin(const QByteArray& login) const;
	QByteArray GetLoginByUserUuid(const QByteArray& userUuid) const;
	void OnUpdate(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IObjectCollection* objectCollectionPtr);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_ldapUserCollectionCompPtr);
	I_ATTR(QByteArray, m_ldapSystemIdAttrPtr);
	I_TEXTATTR(m_ldapSystemNameAttrPtr);

	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CLdapUserCollectionJoinerComp> m_ldapUserCollectionObserver;
	bool m_isBlocked;
};


} // namespace imtauth
