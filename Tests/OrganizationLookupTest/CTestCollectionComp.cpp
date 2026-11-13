// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTestCollectionComp.h"


// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>
#include <ilog/CMessage.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtauth/CCrossOrgGrant.h>
#include <imtauth/CDocumentShareInfo.h>
#include <imtauth/CPersonalAccessToken.h>
#include <imtauth/CTenantConnection.h>
#include <imtauth/CTenantInfo.h>
#include <imtauth/CTenantInvitation.h>
#include <imtauth/CTenantMembership.h>
#include <imtauth/CTenantRelationshipInfo.h>
#include <imtauth/CTenantRelationshipProposal.h>
#include <imtauth/CUserVerification.h>


// public methods of CTestCollectionComp

QByteArray CTestCollectionComp::AddObject(const QByteArray& typeId, const istd::IChangeable& object, const QByteArray& objectId)
{
	return InsertNewObject(typeId, QString(), QString(), &object, objectId);
}


void CTestCollectionComp::SetFilterSupported(bool isSupported)
{
	m_isFilterSupported = isSupported;
}


void CTestCollectionComp::SetUuidFieldIds(const QSet<QByteArray>& fieldIds)
{
	m_uuidFieldIds = fieldIds;
}


int CTestCollectionComp::GetLoadCount() const
{
	return m_loadCount;
}


void CTestCollectionComp::ResetLoadCount()
{
	m_loadCount = 0;
	m_unfilteredSelectionCount = 0;
}


int CTestCollectionComp::GetUnfilteredSelectionCount() const
{
	return m_unfilteredSelectionCount;
}


QSet<QByteArray> CTestCollectionComp::GetUnknownFieldIds() const
{
	QMutexLocker locker(&m_recordMutex);

	return m_unknownFieldIds;
}


QStringList CTestCollectionComp::GetRequestedFilterValues() const
{
	QMutexLocker locker(&m_recordMutex);

	return m_requestedFilterValues;
}


void CTestCollectionComp::SetSelectionFailing(bool isFailing)
{
	m_isSelectionFailing = isFailing;
}


void CTestCollectionComp::SetLoadFailingIds(const QSet<QByteArray>& objectIds)
{
	m_loadFailingIds = objectIds;
}


void CTestCollectionComp::SetRemovalFailing(bool isFailing)
{
	m_isRemovalFailing = isFailing;
}


void CTestCollectionComp::SetStoreFailing(bool isFailing)
{
	m_isStoreFailing = isFailing;
}


void CTestCollectionComp::SetAfterLoadHook(const std::function<void()>& hook)
{
	m_afterLoadHook = hook;
}


// reimplemented (imtbase::IObjectCollection)

bool CTestCollectionComp::GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr) const
{
	++m_loadCount;
	if (m_loadFailingIds.contains(objectId)){
		return false;
	}

	const bool retVal = BaseClass2::GetObjectData(objectId, dataPtr, dataConfigurationPtr);
	if (m_afterLoadHook){
		const std::function<void()> hook = m_afterLoadHook;
		m_afterLoadHook = nullptr;
		hook();
	}

	return retVal;
}


bool CTestCollectionComp::RemoveElements(const Ids& elementIds, const imtbase::IOperationContext* operationContextPtr)
{
	if (m_isRemovalFailing){
		return false;
	}

	return BaseClass2::RemoveElements(elementIds, operationContextPtr);
}


bool CTestCollectionComp::SetObjectData(const Id& objectId, const istd::IChangeable& object, CompatibilityMode mode, const imtbase::IOperationContext* operationContextPtr)
{
	if (m_isStoreFailing){
		return false;
	}

	return BaseClass2::SetObjectData(objectId, object, mode, operationContextPtr);
}


// reimplemented (imtbase::ICollectionInfo)

int CTestCollectionComp::GetElementsCount(const iprm::IParamsSet* selectionParamPtr, ilog::IMessageConsumer* logPtr) const
{
	return GetElementIds(0, -1, selectionParamPtr, logPtr).count();
}


imtbase::ICollectionInfo::Ids CTestCollectionComp::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	const Ids allIds = imtbase::CObjectCollectionBase::GetElementIds(0, -1, nullptr, logPtr);

	iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterPtr(selectionParamsPtr, "ComplexFilter", false);
	iprm::TParamsPtr<iprm::IIdParam> parentTenantIdParamPtr(selectionParamsPtr, "ParentTenantId", false);
	if (!complexFilterPtr.IsValid() && !parentTenantIdParamPtr.IsValid()){
		++m_unfilteredSelectionCount;
	}

	if (complexFilterPtr.IsValid()){
		QList<imtbase::IComplexCollectionFilter::FilterExpression> expressions{complexFilterPtr->GetFilterExpression()};
		while (!expressions.isEmpty()){
			const imtbase::IComplexCollectionFilter::FilterExpression expression = expressions.takeFirst();
			for (const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : expression.fieldFilters){
				QMutexLocker locker(&m_recordMutex);
				m_requestedFilterValues << fieldFilter.filterValue.toString();
			}
			expressions << expression.filterExpressions;
		}
	}

	if (m_isSelectionFailing){
		if (logPtr != nullptr){
			logPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, "Selection failed", "CTestCollectionComp")));
		}

		return Ids();
	}

	if (selectionParamsPtr == nullptr || !m_isFilterSupported){
		return allIds.mid(offset, count);
	}

	if (complexFilterPtr.IsValid() && HasInvalidUuid(complexFilterPtr->GetFilterExpression())){
		return Ids();
	}

	Ids retVal;
	for (const QByteArray& objectId : allIds){
		if (IsSelected(objectId, *selectionParamsPtr)){
			retVal.append(objectId);
		}
	}

	return retVal.mid(offset, count);
}


// protected methods

// reimplemented (imtbase::CObjectCollectionBase)

istd::IChangeableUniquePtr CTestCollectionComp::CreateObjectInstance(const QByteArray& typeId) const
{
	if (typeId == "TenantMembership"){
		return new imtauth::CIdentifiableTenantMembership;
	}
	if (typeId == "Tenant"){
		return new imtauth::CIdentifiableTenantInfo;
	}
	if (typeId == "TenantInvitation"){
		return new imtauth::CIdentifiableTenantInvitation;
	}
	if (typeId == "DocumentShare"){
		return new imtauth::CIdentifiableDocumentShareInfo;
	}
	if (typeId == "TenantConnection"){
		return new imtauth::CIdentifiableTenantConnection;
	}
	if (typeId == "TenantRelationship"){
		return new imtauth::CIdentifiableTenantRelationshipInfo;
	}
	if (typeId == "TenantRelationshipProposal"){
		return new imtauth::CIdentifiableTenantRelationshipProposal;
	}
	if (typeId == "CrossOrgGrant"){
		return new imtauth::CIdentifiableCrossOrgGrant;
	}
	if (typeId == "PersonalAccessToken"){
		return new imtauth::CPersonalAccessToken;
	}
	if (typeId == "UserVerification"){
		return new imtauth::CIdentifiableUserVerificationInfo;
	}

	return nullptr;
}


// private methods

bool CTestCollectionComp::IsSelected(const QByteArray& objectId, const iprm::IParamsSet& selectionParams) const
{
	DataPtr dataPtr;
	if (!BaseClass2::GetObjectData(objectId, dataPtr) || !dataPtr.IsValid()){
		return false;
	}

	iprm::TParamsPtr<iprm::IIdParam> parentTenantIdParamPtr(&selectionParams, "ParentTenantId", false);
	if (parentTenantIdParamPtr.IsValid()){
		QString value;
		if (!GetFieldValue(*dataPtr, "ParentTenantId", value) || value != QString::fromUtf8(parentTenantIdParamPtr->GetId())){
			return false;
		}
	}

	iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterPtr(&selectionParams, "ComplexFilter", false);

	return !complexFilterPtr.IsValid() || Matches(*dataPtr, complexFilterPtr->GetFilterExpression());
}


bool CTestCollectionComp::Matches(const istd::IChangeable& object, const imtbase::IComplexCollectionFilter::FilterExpression& expression) const
{
	if (expression.fieldFilters.isEmpty() && expression.filterExpressions.isEmpty()){
		return true;
	}

	QList<bool> results;
	for (const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : expression.fieldFilters){
		QString value;
		const bool isKnown = GetFieldValue(object, fieldFilter.fieldId, value);
		if (!isKnown){
			QMutexLocker locker(&m_recordMutex);
			m_unknownFieldIds.insert(fieldFilter.fieldId);
		}

		const QString filterValue = fieldFilter.filterValue.toString();
		bool isMatch = false;
		if (isKnown && fieldFilter.filterOperation == imtbase::IComplexCollectionFilter::FO_EQUAL){
			isMatch = value == filterValue;
		}
		else if (isKnown && fieldFilter.filterOperation == imtbase::IComplexCollectionFilter::FO_CONTAINS){
			isMatch = value.contains(filterValue, Qt::CaseInsensitive);
		}

		results.append(isMatch);
	}

	for (const imtbase::IComplexCollectionFilter::FilterExpression& nested : expression.filterExpressions){
		results.append(Matches(object, nested));
	}

	if (expression.logicalOperation == imtbase::IComplexCollectionFilter::LO_OR){
		return results.contains(true);
	}

	return !results.contains(false);
}


bool CTestCollectionComp::HasInvalidUuid(const imtbase::IComplexCollectionFilter::FilterExpression& expression) const
{
	for (const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : expression.fieldFilters){
		if (m_uuidFieldIds.contains(fieldFilter.fieldId) && !imtbase::isValidUuid(fieldFilter.filterValue.toString().toUtf8())){
			return true;
		}
	}

	for (const imtbase::IComplexCollectionFilter::FilterExpression& nested : expression.filterExpressions){
		if (HasInvalidUuid(nested)){
			return true;
		}
	}

	return false;
}


bool CTestCollectionComp::GetFieldValue(const istd::IChangeable& object, const QByteArray& fieldId, QString& value) const
{
	QByteArray rawValue;
	bool isKnown = false;

	if (const imtauth::ITenantMembership* membershipPtr = dynamic_cast<const imtauth::ITenantMembership*>(&object)){
		isKnown = fieldId == "UserId" || fieldId == "TenantId";
		rawValue = fieldId == "UserId" ? membershipPtr->GetUserId() : membershipPtr->GetTenantId();
	}
	else if (const imtauth::ITenantInfo* tenantPtr = dynamic_cast<const imtauth::ITenantInfo*>(&object)){
		if (fieldId == "Name"){
			value = tenantPtr->GetTenantName();
			return true;
		}
		isKnown = fieldId == "OwnerId" || fieldId == "CreatorId" || fieldId == "ParentTenantId" || fieldId == "Id";
		rawValue = fieldId == "OwnerId" ? tenantPtr->GetOwnerId()
					: fieldId == "CreatorId" ? tenantPtr->GetCreatorId()
					: fieldId == "ParentTenantId" ? tenantPtr->GetParentTenantId()
					: tenantPtr->GetTenantId();
	}
	else if (const imtauth::ITenantInvitation* invitationPtr = dynamic_cast<const imtauth::ITenantInvitation*>(&object)){
		isKnown = fieldId == "UserId" || fieldId == "TenantId" || fieldId == "InvitedByUserId" || fieldId == "RevokedByUserId";
		rawValue = fieldId == "UserId" ? invitationPtr->GetUserId()
					: fieldId == "TenantId" ? invitationPtr->GetTenantId()
					: fieldId == "InvitedByUserId" ? invitationPtr->GetInvitedByUserId()
					: invitationPtr->GetRevokedByUserId();
	}
	else if (const imtauth::IDocumentShareInfo* sharePtr = dynamic_cast<const imtauth::IDocumentShareInfo*>(&object)){
		isKnown = fieldId == "EntityId" || fieldId == "OwnerTenantId" || fieldId == "TargetTenantId";
		rawValue = fieldId == "EntityId" ? sharePtr->GetEntityId()
					: fieldId == "OwnerTenantId" ? sharePtr->GetOwnerTenantId()
					: sharePtr->GetTargetTenantId();
	}
	else if (const imtauth::ITenantConnectionInfo* connectionPtr = dynamic_cast<const imtauth::ITenantConnectionInfo*>(&object)){
		isKnown = fieldId == "TenantAId" || fieldId == "TenantBId" || fieldId == "Id";
		rawValue = fieldId == "TenantAId" ? connectionPtr->GetTenantAId()
					: fieldId == "TenantBId" ? connectionPtr->GetTenantBId()
					: connectionPtr->GetConnectionId();
	}
	else if (const imtauth::ITenantRelationshipInfo* relationshipPtr = dynamic_cast<const imtauth::ITenantRelationshipInfo*>(&object)){
		isKnown = fieldId == "SourceTenantId" || fieldId == "TargetTenantId" || fieldId == "ConnectionId" || fieldId == "Id";
		rawValue = fieldId == "SourceTenantId" ? relationshipPtr->GetSourceTenantId()
					: fieldId == "TargetTenantId" ? relationshipPtr->GetTargetTenantId()
					: fieldId == "ConnectionId" ? relationshipPtr->GetConnectionId()
					: relationshipPtr->GetRelationshipId();
	}
	else if (const imtauth::ITenantRelationshipProposalInfo* proposalPtr = dynamic_cast<const imtauth::ITenantRelationshipProposalInfo*>(&object)){
		isKnown = fieldId == "InitiatorTenantId" || fieldId == "CounterpartyTenantId" || fieldId == "ConnectionId";
		rawValue = fieldId == "InitiatorTenantId" ? proposalPtr->GetInitiatorTenantId()
					: fieldId == "CounterpartyTenantId" ? proposalPtr->GetCounterpartyTenantId()
					: proposalPtr->GetConnectionId();
	}
	else if (const imtauth::ICrossOrgGrantData* grantPtr = dynamic_cast<const imtauth::ICrossOrgGrantData*>(&object)){
		const imtauth::CrossOrgGrantInfo grantInfo = grantPtr->GetGrantInfo();
		isKnown = fieldId == "SourceTenantId" || fieldId == "TargetTenantId";
		rawValue = fieldId == "SourceTenantId" ? grantInfo.sourceTenantId : grantInfo.targetTenantId;
	}
	else if (const imtauth::IPersonalAccessToken* tokenPtr = dynamic_cast<const imtauth::IPersonalAccessToken*>(&object)){
		isKnown = fieldId == "UserId" || fieldId == "TokenHash";
		rawValue = fieldId == "UserId" ? tokenPtr->GetUserId() : tokenPtr->GetTokenHash();
	}

	if (isKnown){
		value = QString::fromUtf8(rawValue);
	}

	return isKnown;
}


// public methods of CTestRelationshipProviderComp

void CTestRelationshipProviderComp::AddRelationship(const QByteArray& tenantId, const QByteArray& otherTenantId, const QDateTime& relatedSince, bool isEffective)
{
	m_relationships.append({tenantId, otherTenantId, relatedSince, isEffective});
}


void CTestRelationshipProviderComp::SetReadable(bool isReadable)
{
	m_isReadable = isReadable;
}


void CTestRelationshipProviderComp::SetPartialRead(bool isPartial)
{
	m_isPartialRead = isPartial;
}


int CTestRelationshipProviderComp::GetReadCount() const
{
	return m_readCount;
}


// reimplemented (imtauth::ITenantRelationshipProvider)

imtauth::RelatedTenantInfos CTestRelationshipProviderComp::GetRelatedTenants(const QByteArray& tenantId, QString* /*tenantNamePtr*/, bool* isReadPtr) const
{
	++m_readCount;
	if (isReadPtr != nullptr){
		*isReadPtr = m_isReadable && !m_isPartialRead;
	}

	imtauth::RelatedTenantInfos retVal;
	if (!m_isReadable){
		return retVal;
	}

	for (const Relationship& relationship : m_relationships){
		imtauth::RelatedTenantInfo relatedTenant;
		if (relationship.tenantId == tenantId){
			relatedTenant.tenantId = relationship.otherTenantId;
		}
		else if (relationship.otherTenantId == tenantId){
			relatedTenant.tenantId = relationship.tenantId;
		}
		else{
			continue;
		}

		relatedTenant.relatedSince = relationship.relatedSince;
		relatedTenant.isEffective = relationship.isEffective;
		retVal.append(relatedTenant);
	}

	return retVal;
}


