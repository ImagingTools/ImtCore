// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/ContractState.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMap>

// STL includes
#include <algorithm>


namespace imtauth
{


namespace
{


static const int s_expiringWindowDays = 30;


QString TimeText(const QDateTime& time)
{
	return time.isValid() ? time.toUTC().toString(Qt::ISODateWithMs) : QString();
}


QDateTime EarlierOf(const QDateTime& first, const QDateTime& second)
{
	if (!first.isValid()){
		return second;
	}

	if (!second.isValid()){
		return first;
	}

	return (first < second) ? first : second;
}


bool HasPartyLeftBy(const IContractInfo::Party& party, const QDateTime& time)
{
	return (party.state == IContractInfo::PS_WITHDRAWN || party.state == IContractInfo::PS_NOTICE_GIVEN)
				&& party.withdrawAt.isValid() && party.withdrawAt <= time;
}


/**
	A contract signed by all parties still holds at \c time: two bound parties, one of their links, and the slots of its type.
*/
bool IsViableAt(const IContractInfo& contract, const QDateTime& time)
{
	const IContractInfo::Parties parties = contract.GetParties();
	QByteArrayList boundTenantIds;
	for (const IContractInfo::Party& party : parties){
		if (!HasPartyLeftBy(party, time)){
			boundTenantIds.append(party.tenantId);
		}
	}

	if (boundTenantIds.size() < 2){
		return false;
	}

	bool hasLink = false;
	for (const IContractInfo::Link& link : contract.GetLinks()){
		if (boundTenantIds.contains(link.tenantAId) && boundTenantIds.contains(link.tenantBId)){
			hasLink = true;

			break;
		}
	}

	if (!hasLink){
		return false;
	}

	for (const IContractInfo::RoleSlot& slot : contract.GetRoleSlots()){
		int count = 0;
		for (const IContractInfo::Party& party : parties){
			if (party.roleId == slot.roleId && boundTenantIds.contains(party.tenantId)){
				++count;
			}
		}

		if (count < slot.minCount){
			return false;
		}
	}

	return true;
}


/**
	The first time a party left and the contract no longer held, invalid when it holds for good.
*/
QDateTime GetBreakupTime(const IContractInfo& contract)
{
	QList<QDateTime> leaveTimes;
	for (const IContractInfo::Party& party : contract.GetParties()){
		if ((party.state == IContractInfo::PS_WITHDRAWN || party.state == IContractInfo::PS_NOTICE_GIVEN) && party.withdrawAt.isValid()){
			leaveTimes.append(party.withdrawAt);
		}
	}

	std::sort(leaveTimes.begin(), leaveTimes.end());

	for (const QDateTime& leaveTime : leaveTimes){
		if (!IsViableAt(contract, leaveTime)){
			return leaveTime;
		}
	}

	return QDateTime();
}


struct ContractEnd
{
	QDateTime time;
	ContractState state = CS_ACTIVE;
};


/**
	The ends of a contract signed by all parties, each with the state it leads to, earliest first.
*/
QList<ContractEnd> GetContractEnds(const IContractInfo& contract, const QDateTime& now)
{
	QList<ContractEnd> retVal;
	if (!contract.GetSupersededBy().isEmpty() && contract.GetSupersededAt().isValid()){
		retVal.append({contract.GetSupersededAt(), CS_SUPERSEDED});
	}

	if (contract.GetTerminatedAt().isValid()){
		retVal.append({contract.GetTerminatedAt(), CS_TERMINATED});
	}

	const QDateTime breakupTime = GetBreakupTime(contract);
	if (breakupTime.isValid()){
		retVal.append({breakupTime, CS_TERMINATED});
	}

	const QDateTime periodEnd = GetContractPeriodEnd(contract, now);
	if (periodEnd.isValid()){
		retVal.append({periodEnd, CS_EXPIRED});
	}

	// A stable sort keeps the replacement before a termination at the same time
	std::stable_sort(retVal.begin(), retVal.end(), [](const ContractEnd& first, const ContractEnd& second){
		return first.time < second.time;
	});

	return retVal;
}


QJsonArray ToJsonArray(QByteArrayList values)
{
	std::sort(values.begin(), values.end());

	QJsonArray retVal;
	for (const QByteArray& value : values){
		retVal.append(QString::fromUtf8(value));
	}

	return retVal;
}


QJsonArray ToJsonArray(const QMap<QByteArray, QJsonObject>& items)
{
	QJsonArray retVal;
	for (const QJsonObject& item : items){
		retVal.append(item);
	}

	return retVal;
}


} // anonymous namespace


int FindContractParty(const IContractInfo::Parties& parties, const QByteArray& tenantId)
{
	for (int i = 0; i < parties.size(); ++i){
		if (parties[i].tenantId == tenantId){
			return i;
		}
	}

	return -1;
}


int FindContractLink(const IContractInfo::Links& links, const QByteArray& linkId)
{
	for (int i = 0; i < links.size(); ++i){
		if (links[i].linkId == linkId){
			return i;
		}
	}

	return -1;
}


int FindContractLinkOfPair(const IContractInfo::Links& links, const QByteArray& tenantId, const QByteArray& otherTenantId)
{
	for (int i = 0; i < links.size(); ++i){
		if (links[i].Connects(tenantId, otherTenantId)){
			return i;
		}
	}

	return -1;
}


int FindContractAllocation(const IContractInfo::Allocations& allocations, const QByteArray& allocationId)
{
	for (int i = 0; i < allocations.size(); ++i){
		if (allocations[i].allocationId == allocationId){
			return i;
		}
	}

	return -1;
}


int FindContractAttachment(const IContractInfo::Attachments& attachments, const QByteArray& attachmentId)
{
	for (int i = 0; i < attachments.size(); ++i){
		if (attachments[i].attachmentId == attachmentId){
			return i;
		}
	}

	return -1;
}


QByteArrayList GetContractPartyTenantIds(const IContractInfo& contract)
{
	QByteArrayList retVal;
	for (const IContractInfo::Party& party : contract.GetParties()){
		retVal.append(party.tenantId);
	}

	return retVal;
}


QByteArrayList GetAllocationGroupIds(const IContractPartyPrivateInfo& privateInfo, const QByteArray& allocationId)
{
	for (const IContractPartyPrivateInfo::AllocationGroups& entry : privateInfo.GetAllocationGroups()){
		if (entry.allocationId == allocationId){
			return entry.groupIds;
		}
	}

	return QByteArrayList();
}


void SetAllocationGroupIds(IContractPartyPrivateInfo& privateInfo, const QByteArray& allocationId, const QByteArrayList& groupIds)
{
	IContractPartyPrivateInfo::AllocationGroupsList entries = privateInfo.GetAllocationGroups();
	bool isFound = false;
	for (int i = 0; i < entries.size(); ++i){
		if (entries[i].allocationId == allocationId){
			isFound = true;
			if (groupIds.isEmpty()){
				entries.removeAt(i);
			}
			else{
				entries[i].groupIds = groupIds;
			}

			break;
		}
	}

	if (!isFound && !groupIds.isEmpty()){
		entries.append({allocationId, groupIds});
	}

	privateInfo.SetAllocationGroups(entries);
}


QDateTime GetContractEffectiveStart(const IContractInfo& contract)
{
	if (!contract.GetSignedAt().isValid()){
		return QDateTime();
	}

	return contract.GetEffectiveFrom().isValid() ? contract.GetEffectiveFrom() : contract.GetSignedAt();
}


QDateTime GetContractPeriodEnd(const IContractInfo& contract, const QDateTime& now)
{
	const QDateTime start = GetContractEffectiveStart(contract);
	if (!start.isValid()){
		return QDateTime();
	}

	QDateTime firstEnd = contract.GetValidUntil();
	if (!firstEnd.isValid() && contract.GetTermDays() > 0){
		firstEnd = start.addDays(contract.GetTermDays());
	}

	if (!firstEnd.isValid() || !contract.IsAutoRenew() || contract.GetRenewalPeriodDays() <= 0){
		return firstEnd;
	}

	QDateTime noticeTime;
	for (const IContractInfo::Party& party : contract.GetParties()){
		noticeTime = EarlierOf(noticeTime, party.nonRenewalNoticeAt);
	}

	// With a notice the contract ends at the first end the notice period allows, without one the period renews
	const bool hasNotice = noticeTime.isValid();
	const QDateTime limit = hasNotice ? noticeTime.addDays(contract.GetRenewalNoticeDays()) : now;
	if (hasNotice ? (limit <= firstEnd) : (limit < firstEnd)){
		return firstEnd;
	}

	const qint64 periodSeconds = qint64(contract.GetRenewalPeriodDays()) * 86400;
	const qint64 elapsedSeconds = firstEnd.secsTo(limit);
	qint64 periods = elapsedSeconds / periodSeconds;
	if (elapsedSeconds % periodSeconds != 0 || !hasNotice){
		++periods;
	}

	return firstEnd.addDays(periods * contract.GetRenewalPeriodDays());
}


bool IsPartyBoundAt(const IContractInfo& contract, const IContractInfo::Party& party, const QDateTime& time)
{
	if (!contract.GetSignedAt().isValid()){
		return false;
	}

	if (party.state != IContractInfo::PS_SIGNED && party.state != IContractInfo::PS_NOTICE_GIVEN && party.state != IContractInfo::PS_WITHDRAWN){
		return false;
	}

	return !HasPartyLeftBy(party, time);
}


ContractStateInfo ComputeContractState(const IContractInfo& contract, const QDateTime& now)
{
	ContractStateInfo retVal;

	const IContractInfo::Parties parties = contract.GetParties();
	if (!contract.GetSignedAt().isValid()){
		bool isDeclined = false;
		bool isSigned = false;
		bool areChangesRequested = false;
		for (const IContractInfo::Party& party : parties){
			isDeclined = isDeclined || party.state == IContractInfo::PS_DECLINED;
			isSigned = isSigned || party.state == IContractInfo::PS_SIGNED || !party.signatures.isEmpty();
			areChangesRequested = areChangesRequested || party.state == IContractInfo::PS_CHANGES_REQUESTED;
		}

		if (contract.GetCancelledAt().isValid()){
			retVal.state = CS_CANCELLED;
			retVal.endedAt = contract.GetCancelledAt();
		}
		else if (!contract.GetSubmittedAt().isValid()){
			retVal.state = CS_DRAFT;
		}
		else if (isDeclined){
			retVal.state = CS_REJECTED;
		}
		else if (contract.GetSignDeadline().isValid() && now > contract.GetSignDeadline()){
			retVal.state = CS_OFFER_EXPIRED;
			retVal.endedAt = contract.GetSignDeadline();
		}
		else if (isSigned){
			retVal.state = CS_SIGNING;
		}
		else{
			retVal.state = CS_NEGOTIATION;
		}

		if (areChangesRequested){
			retVal.flags |= CSF_CHANGES_REQUESTED;
		}

		return retVal;
	}

	retVal.effectiveStart = GetContractEffectiveStart(contract);
	retVal.periodEnd = GetContractPeriodEnd(contract, now);

	const QList<ContractEnd> ends = GetContractEnds(contract, now);
	if (!ends.isEmpty() && ends.first().time <= now){
		retVal.state = ends.first().state;
		retVal.endedAt = ends.first().time;

		return retVal;
	}

	retVal.state = (now < retVal.effectiveStart) ? CS_SIGNED : CS_ACTIVE;

	// What ends later: the termination, a party leaving, or the contract breaking up with it
	QDateTime endsAt = contract.GetTerminatedAt();
	for (const IContractInfo::Party& party : parties){
		if (party.state == IContractInfo::PS_NOTICE_GIVEN && party.withdrawAt.isValid() && party.withdrawAt > now){
			endsAt = EarlierOf(endsAt, party.withdrawAt);
		}

		if (IsPartyBoundAt(contract, party, now) && party.allocationsSuspended){
			retVal.flags |= CSF_PARTIALLY_SUSPENDED;
		}
	}

	if (endsAt.isValid()){
		retVal.flags |= CSF_TERMINATING;
		retVal.endsAt = endsAt;
	}

	if (retVal.periodEnd.isValid()){
		const int windowDays = qMax(contract.GetRenewalNoticeDays(), s_expiringWindowDays);
		if (now.addDays(windowDays) >= retVal.periodEnd){
			retVal.flags |= CSF_EXPIRING;
		}
	}

	return retVal;
}


bool IsContractInForce(const IContractInfo& contract, const QDateTime& now)
{
	return ComputeContractState(contract, now).state == CS_ACTIVE;
}


bool IsLinkInForce(const IContractInfo& contract, const IContractInfo::Link& link, const QDateTime& now)
{
	if (!IsContractInForce(contract, now)){
		return false;
	}

	if ((link.validFrom.isValid() && now < link.validFrom) || (link.validUntil.isValid() && now >= link.validUntil)){
		return false;
	}

	const IContractInfo::Parties parties = contract.GetParties();
	const int partyAIndex = FindContractParty(parties, link.tenantAId);
	const int partyBIndex = FindContractParty(parties, link.tenantBId);

	return partyAIndex >= 0 && partyBIndex >= 0
				&& IsPartyBoundAt(contract, parties[partyAIndex], now)
				&& IsPartyBoundAt(contract, parties[partyBIndex], now);
}


bool IsAllocationInForce(const IContractInfo& contract, const IContractInfo::Allocation& allocation, const QDateTime& now)
{
	const IContractInfo::Links links = contract.GetLinks();
	const int linkIndex = FindContractLink(links, allocation.linkId);
	if (linkIndex < 0 || !links[linkIndex].Connects(allocation.grantorTenantId, allocation.granteeTenantId)){
		return false;
	}

	if (!IsLinkInForce(contract, links[linkIndex], now)){
		return false;
	}

	if ((allocation.validFrom.isValid() && now < allocation.validFrom) || (allocation.validUntil.isValid() && now >= allocation.validUntil)){
		return false;
	}

	const IContractInfo::Parties parties = contract.GetParties();
	const int grantorIndex = FindContractParty(parties, allocation.grantorTenantId);

	return grantorIndex >= 0 && !parties[grantorIndex].allocationsSuspended;
}


bool GetLinkPeriod(const IContractInfo& contract, const IContractInfo::Link& link, const QDateTime& now, RelationshipPeriod& period)
{
	if (!contract.GetSignedAt().isValid()){
		return false;
	}

	const ContractStateInfo stateInfo = ComputeContractState(contract, now);

	period.start = stateInfo.effectiveStart;
	if (link.validFrom.isValid() && link.validFrom > period.start){
		period.start = link.validFrom;
	}

	QDateTime end = stateInfo.endedAt;
	end = EarlierOf(end, (link.validUntil.isValid() && link.validUntil <= now) ? link.validUntil : QDateTime());

	for (const IContractInfo::Party& party : contract.GetParties()){
		if ((party.tenantId == link.tenantAId || party.tenantId == link.tenantBId) && HasPartyLeftBy(party, now)){
			end = EarlierOf(end, party.withdrawAt);
		}
	}

	period.end = end;

	return true;
}


QByteArray ComputeContractContentHash(const IContractInfo& contract)
{
	QJsonObject root;
	root["name"] = contract.GetName();
	root["description"] = contract.GetDescription();
	root["terms"] = contract.GetTerms();
	root["typeId"] = QString::fromUtf8(contract.GetTypeId());
	root["effectiveFrom"] = TimeText(contract.GetEffectiveFrom());
	root["termDays"] = contract.GetTermDays();
	root["validUntil"] = TimeText(contract.GetValidUntil());
	root["autoRenew"] = contract.IsAutoRenew();
	root["renewalPeriodDays"] = contract.GetRenewalPeriodDays();
	root["renewalNoticeDays"] = contract.GetRenewalNoticeDays();
	root["terminationNoticeDays"] = contract.GetTerminationNoticeDays();
	root["signDeadline"] = TimeText(contract.GetSignDeadline());
	root["signOrder"] = int(contract.GetSignOrder());

	QMap<QByteArray, QJsonObject> parties;
	for (const IContractInfo::Party& party : contract.GetParties()){
		QJsonObject item;
		item["tenantId"] = QString::fromUtf8(party.tenantId);
		item["roleId"] = QString::fromUtf8(party.roleId);
		item["signOrder"] = party.signOrder;
		item["requiredSignatures"] = party.requiredSignatures;
		parties.insert(party.tenantId, item);
	}

	QMap<QByteArray, QJsonObject> links;
	for (const IContractInfo::Link& link : contract.GetLinks()){
		QJsonObject item;
		item["linkId"] = QString::fromUtf8(link.linkId);
		item["tenantAId"] = QString::fromUtf8(link.tenantAId);
		item["tenantBId"] = QString::fromUtf8(link.tenantBId);
		item["roleAId"] = QString::fromUtf8(link.roleAId);
		item["roleBId"] = QString::fromUtf8(link.roleBId);
		item["validFrom"] = TimeText(link.validFrom);
		item["validUntil"] = TimeText(link.validUntil);
		item["isMessagingAllowed"] = link.isMessagingAllowed;
		item["isSharingAllowed"] = link.isSharingAllowed;
		links.insert(link.linkId, item);
	}

	QMap<QByteArray, QJsonObject> allocations;
	for (const IContractInfo::Allocation& allocation : contract.GetAllocations()){
		QJsonObject item;
		item["allocationId"] = QString::fromUtf8(allocation.allocationId);
		item["linkId"] = QString::fromUtf8(allocation.linkId);
		item["grantorTenantId"] = QString::fromUtf8(allocation.grantorTenantId);
		item["granteeTenantId"] = QString::fromUtf8(allocation.granteeTenantId);
		item["productId"] = QString::fromUtf8(allocation.productId);
		item["permissionIds"] = ToJsonArray(allocation.permissionIds);
		item["validFrom"] = TimeText(allocation.validFrom);
		item["validUntil"] = TimeText(allocation.validUntil);
		item["comment"] = allocation.comment;
		allocations.insert(allocation.allocationId, item);
	}

	QMap<QByteArray, QJsonObject> references;
	for (const IContractInfo::Reference& reference : contract.GetReferences()){
		QJsonObject item;
		item["contractId"] = QString::fromUtf8(reference.contractId);
		item["type"] = int(reference.type);
		references.insert(reference.contractId, item);
	}

	QByteArrayList attachmentHashes;
	for (const IContractInfo::Attachment& attachment : contract.GetAttachments()){
		if (!attachment.isReplaced && (attachment.kind == IContractInfo::AK_MAIN_TEXT || attachment.kind == IContractInfo::AK_APPENDIX)){
			attachmentHashes.append(QByteArray::number(int(attachment.kind)) + ':' + attachment.contentHash);
		}
	}

	QMap<QByteArray, QJsonObject> roleSlots;
	for (const IContractInfo::RoleSlot& slot : contract.GetRoleSlots()){
		QJsonObject item;
		item["roleId"] = QString::fromUtf8(slot.roleId);
		item["minCount"] = slot.minCount;
		item["maxCount"] = slot.maxCount;
		roleSlots.insert(slot.roleId, item);
	}

	QByteArrayList requiredKinds;
	for (int kind : contract.GetRequiredAttachmentKinds()){
		requiredKinds.append(QByteArray::number(kind));
	}

	root["parties"] = ToJsonArray(parties);
	root["links"] = ToJsonArray(links);
	root["allocations"] = ToJsonArray(allocations);
	root["references"] = ToJsonArray(references);
	root["roleSlots"] = ToJsonArray(roleSlots);
	root["attachments"] = ToJsonArray(attachmentHashes);
	root["requiredAttachmentKinds"] = ToJsonArray(requiredKinds);

	// The keys of a JSON object are sorted, so equal terms give equal bytes
	const QByteArray canonical = QJsonDocument(root).toJson(QJsonDocument::Compact);

	return QCryptographicHash::hash(canonical, QCryptographicHash::Sha256).toHex();
}


} // namespace imtauth


