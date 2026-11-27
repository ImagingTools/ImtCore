// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QSet>

// STL includes
#include <functional>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IContractProvider.h>
#include <imtauth/IDelegatedAccess.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	The delegated access of the users from the allocations in force of the contracts of their organizations,
	given to the groups the users are in.
*/
class CContractDelegatedAccessComp:
			public icomp::CComponentBase,
			virtual public IDelegatedAccess
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CContractDelegatedAccessComp);
		I_REGISTER_INTERFACE(IDelegatedAccess);
		I_ASSIGN(m_contractProviderCompPtr, "ContractProvider", "Contracts of the organizations", true, "ContractProvider");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "The organizations of the users", true, "TenantMembershipManager");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Users: the groups they are in", true, "UserCollection");
		I_ASSIGN(m_groupCollectionCompPtr, "GroupCollection", "Groups: their parent groups", true, "UserGroupCollection");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Organizations: the permissions a grantor holds now", true, "TenantManager");
	I_END_COMPONENT;

	// reimplemented (imtauth::IDelegatedAccess)
	virtual QByteArrayList GetDelegatedPermissions(
				const QByteArray& userId,
				const QByteArray& targetTenantId,
				const QByteArray& productId,
				bool* isReadPtr = nullptr) const override;
	virtual QByteArrayList GetDelegatingTenantIds(const QByteArray& userId, const QByteArray& productId, bool* isReadPtr = nullptr) const override;
	virtual bool HasDelegatedAccess(const QByteArray& userId, const QByteArray& targetTenantId, const QByteArray& productId, bool* isReadPtr = nullptr) const override;

private:
	/**
		The organizations the user is an active member of, and its groups with their parents.
	*/
	bool ReadUserScope(const QByteArray& userId, QByteArrayList& homeTenantIds, QSet<QByteArray>& groupIds) const;

	typedef std::function<void(const IContractInfo::Allocation& allocation)> AllocationVisitor;

	/**
		Visit the allocations in force given to one of the organizations of the user and to groups the user is in,
		each with the permissions its grantor holds now only; an allocation of none of them is not visited.
		\param grantorTenantId An empty one visits the allocations of every grantor.
	*/
	bool VisitUserAllocations(const QByteArray& userId, const QByteArray& grantorTenantId, const QByteArray& productId, const AllocationVisitor& visitor) const;

private:
	I_REF(IContractProvider, m_contractProviderCompPtr);
	I_REF(ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_groupCollectionCompPtr);
	I_REF(ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtauth


