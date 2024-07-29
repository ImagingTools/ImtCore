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
		I_REGISTER_INTERFACE(imtbase::IObjectCollection)
		I_ASSIGN(m_userCollectionCompPtr, "InternalUserCollection", "Internal user collection", true, "InternalUserCollection");
		I_ASSIGN(m_ldapUserCollectionCompPtr, "LdapUserCollection", "LDAP user collection", true, "LdapUserCollection");
		I_ASSIGN_TO(m_ldapUserCollectionModelCompPtr, m_ldapUserCollectionCompPtr, true);
	I_END_COMPONENT;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_ldapUserCollectionCompPtr);
	I_REF(imod::IModel, m_ldapUserCollectionModelCompPtr);
};


} // namespace imtauth
