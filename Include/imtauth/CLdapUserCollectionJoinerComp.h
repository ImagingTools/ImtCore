#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/IModel.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


class CLdapUserCollectionJoinerComp:
			public icomp::CComponentBase,
			public imod::TSingleModelObserverBase<istd::IChangeable>
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLdapUserCollectionJoinerComp);
		I_ASSIGN(m_userCollectionCompPtr, "InternalUserCollection", "Internal user collection", true, "InternalUserCollection");
		I_ASSIGN(m_ldapUserCollectionCompPtr, "LdapUserCollection", "LDAP user collection", true, "LdapUserCollection");
		I_ASSIGN_TO(m_ldapUserCollectionModelCompPtr, m_ldapUserCollectionCompPtr, true);
		I_ASSIGN(m_ldapSystemIdAttrPtr, "LdapSystemId", "LDAP System-ID", false, "Ldap");
		I_ASSIGN(m_ldapSystemNameAttrPtr, "LdapSystemName", "LDAP System Name", false, "LDAP");
	I_END_COMPONENT;

	QByteArray GetUserUuidByLogin(const QByteArray& login) const;
	QByteArray GetLoginByUserUuid(const QByteArray& userUuid) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_ldapUserCollectionCompPtr);
	I_REF(imod::IModel, m_ldapUserCollectionModelCompPtr);
	I_ATTR(QByteArray, m_ldapSystemIdAttrPtr);
	I_TEXTATTR(m_ldapSystemNameAttrPtr);
};


} // namespace imtauth
