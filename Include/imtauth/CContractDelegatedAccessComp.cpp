// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractDelegatedAccessComp.h>


// Qt includes
#include <QtCore/QHash>

// ImtCore includes
#include <imtauth/ContractState.h>
#include <imtauth/IUserGroupInfo.h>
#include <imtauth/IUserInfo.h>


namespace imtauth
{


// reimplemented (imtauth::IDelegatedAccess)

QByteArrayList CContractDelegatedAccessComp::GetDelegatedPermissions(
			const QByteArray& userId,
			const QByteArray& targetTenantId,
			const QByteArray& productId,
			bool* isReadPtr) const
{
	QByteArrayList retVal;
	if (targetTenantId.isEmpty()){
		if (isReadPtr != nullptr){
			*isReadPtr = true;
		}

		return retVal;
	}

	const bool isRead = VisitUserAllocations(userId, targetTenantId, productId, [&](const IContractInfo::Allocation& allocation){
		for (const QByteArray& permissionId : allocation.permissionIds){
			if (!retVal.contains(permissionId)){
				retVal.append(permissionId);
			}
		}
	});

	if (isReadPtr != nullptr){
		*isReadPtr = isRead;
	}

	return retVal;
}


QByteArrayList CContractDelegatedAccessComp::GetDelegatingTenantIds(const QByteArray& userId, const QByteArray& productId, bool* isReadPtr) const
{
	QByteArrayList retVal;
	const bool isRead = VisitUserAllocations(userId, QByteArray(), productId, [&](const IContractInfo::Allocation& allocation){
		if (!retVal.contains(allocation.grantorTenantId)){
			retVal.append(allocation.grantorTenantId);
		}
	});

	if (isReadPtr != nullptr){
		*isReadPtr = isRead;
	}

	return retVal;
}


bool CContractDelegatedAccessComp::HasDelegatedAccess(const QByteArray& userId, const QByteArray& targetTenantId, const QByteArray& productId, bool* isReadPtr) const
{
	bool hasAccess = false;
	const bool isRead = !targetTenantId.isEmpty() && VisitUserAllocations(userId, targetTenantId, productId, [&hasAccess](const IContractInfo::Allocation& /*allocation*/){
		hasAccess = true;
	});

	if (isReadPtr != nullptr){
		*isReadPtr = isRead;
	}

	return hasAccess;
}


// private methods

bool CContractDelegatedAccessComp::ReadUserScope(const QByteArray& userId, QByteArrayList& homeTenantIds, QSet<QByteArray>& groupIds) const
{
	if (userId.isEmpty()){
		return false;
	}

	bool isRead = true;
	for (const QByteArray& membershipId : m_membershipManagerCompPtr->GetMembershipsByUser(userId, &isRead)){
		ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
		if (!membershipPtr.IsValid()){
			isRead = false;

			continue;
		}

		if (membershipPtr->IsActive() && !membershipPtr->GetTenantId().isEmpty()){
			homeTenantIds.append(membershipPtr->GetTenantId());
		}
	}

	imtbase::IObjectCollection::DataPtr userDataPtr;
	const IUserInfo* userInfoPtr = nullptr;
	if (m_userCollectionCompPtr->GetObjectData(userId, userDataPtr)){
		userInfoPtr = dynamic_cast<const IUserInfo*>(userDataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		return false;
	}

	// A member of a group is a member of its parent groups too
	QByteArrayList pendingGroupIds = userInfoPtr->GetGroups();
	while (!pendingGroupIds.isEmpty()){
		const QByteArray groupId = pendingGroupIds.takeFirst();
		if (groupId.isEmpty() || groupIds.contains(groupId)){
			continue;
		}

		groupIds.insert(groupId);

		imtbase::IObjectCollection::DataPtr groupDataPtr;
		const IUserGroupInfo* groupInfoPtr = nullptr;
		if (m_groupCollectionCompPtr->GetObjectData(groupId, groupDataPtr)){
			groupInfoPtr = dynamic_cast<const IUserGroupInfo*>(groupDataPtr.GetPtr());
		}

		if (groupInfoPtr == nullptr){
			isRead = false;

			continue;
		}

		pendingGroupIds.append(groupInfoPtr->GetParentGroups());
	}

	return isRead;
}


bool CContractDelegatedAccessComp::VisitUserAllocations(const QByteArray& userId, const QByteArray& grantorTenantId, const QByteArray& productId, const AllocationVisitor& visitor) const
{
	if (productId.isEmpty()){
		return false;
	}

	QByteArrayList homeTenantIds;
	QSet<QByteArray> groupIds;
	bool isRead = ReadUserScope(userId, homeTenantIds, groupIds);
	if (groupIds.isEmpty()){
		return isRead;
	}

	const QDateTime now = QDateTime::currentDateTimeUtc();
	QHash<QByteArray, QSet<QByteArray>> grantorScopes;
	for (const QByteArray& homeTenantId : std::as_const(homeTenantIds)){
		if (homeTenantId == grantorTenantId){
			continue;
		}

		bool areContractsRead = false;
		const ContractInfoList contracts = grantorTenantId.isEmpty()
					? m_contractProviderCompPtr->GetContractsOfParty(homeTenantId, productId, &areContractsRead)
					: m_contractProviderCompPtr->GetContractsOfPair(homeTenantId, grantorTenantId, productId, &areContractsRead);
		isRead = isRead && areContractsRead;

		for (const IContractInfoSharedPtr& contractPtr : contracts){
			if (!contractPtr.IsValid() || contractPtr->GetProductId() != productId){
				continue;
			}

			IContractPartyPrivateInfoUniquePtr privateInfoPtr;
			for (const IContractInfo::Allocation& allocation : contractPtr->GetAllocations()){
				if (allocation.granteeTenantId != homeTenantId || (!grantorTenantId.isEmpty() && allocation.grantorTenantId != grantorTenantId)){
					continue;
				}

				if (!IsAllocationInForce(*contractPtr, allocation, now)){
					continue;
				}

				// The party decides which of its groups get the permissions; no group is nobody
				if (!privateInfoPtr.IsValid()){
					privateInfoPtr = m_contractProviderCompPtr->GetPrivateInfo(contractPtr->GetContractId(), homeTenantId);
					if (!privateInfoPtr.IsValid()){
						isRead = false;

						break;
					}
				}

				bool isGroupOfUser = false;
				for (const QByteArray& groupId : GetAllocationGroupIds(*privateInfoPtr, allocation.allocationId)){
					isGroupOfUser = isGroupOfUser || groupIds.contains(groupId);
				}

				if (!isGroupOfUser){
					continue;
				}

				// A grantor gives no more than it holds now, whatever it held when the contract was concluded
				auto scopeIter = grantorScopes.constFind(allocation.grantorTenantId);
				if (scopeIter == grantorScopes.constEnd()){
					const QByteArrayList scope = m_tenantManagerCompPtr->GetTenantPermissions(allocation.grantorTenantId);
					scopeIter = grantorScopes.insert(allocation.grantorTenantId, QSet<QByteArray>(scope.begin(), scope.end()));
				}

				IContractInfo::Allocation boundedAllocation = allocation;
				boundedAllocation.permissionIds.clear();
				for (const QByteArray& permissionId : allocation.permissionIds){
					if (scopeIter->contains(permissionId)){
						boundedAllocation.permissionIds.append(permissionId);
					}
				}

				if (!boundedAllocation.permissionIds.isEmpty()){
					visitor(boundedAllocation);
				}
			}
		}
	}

	return isRead;
}


} // namespace imtauth


