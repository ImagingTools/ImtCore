// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantFilterParam.h>


// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>


namespace imtauth
{


CTenantFilterParam::CTenantFilterParam()
	: m_filterMode(TFM_INCLUDE),
	m_sharingScope(SS_OWN),
	m_minimumAccessLevel(IDocumentShareInfo::DAL_VIEW),
	m_isGroupRestrictionEnabled(false),
	m_isShareOwnerRestrictionEnabled(false),
	m_sharesRevision(0)
{
}


// reimplemented (imtauth::ITenantFilterParam)

QByteArray CTenantFilterParam::GetTenantId() const
{
	return m_tenantId;
}


void CTenantFilterParam::SetTenantId(const QByteArray& tenantId)
{
	if (m_tenantId != tenantId){
		istd::CChangeNotifier notifier(this);

		m_tenantId = tenantId;
	}
}


QByteArray CTenantFilterParam::GetOwnerId() const
{
	return m_ownerId;
}


void CTenantFilterParam::SetOwnerId(const QByteArray& ownerId)
{
	if (m_ownerId != ownerId){
		istd::CChangeNotifier notifier(this);

		m_ownerId = ownerId;
	}
}


TenantFilterMode CTenantFilterParam::GetFilterMode() const
{
	return m_filterMode;
}


void CTenantFilterParam::SetFilterMode(TenantFilterMode mode)
{
	if (m_filterMode != mode){
		istd::CChangeNotifier notifier(this);

		m_filterMode = mode;
	}
}


SharingScope CTenantFilterParam::GetSharingScope() const
{
	return m_sharingScope;
}


void CTenantFilterParam::SetSharingScope(SharingScope scope)
{
	if (m_sharingScope != scope){
		istd::CChangeNotifier notifier(this);

		m_sharingScope = scope;
	}
}


IDocumentShareInfo::AccessLevel CTenantFilterParam::GetMinimumAccessLevel() const
{
	return m_minimumAccessLevel;
}


void CTenantFilterParam::SetMinimumAccessLevel(IDocumentShareInfo::AccessLevel accessLevel)
{
	if (m_minimumAccessLevel != accessLevel){
		istd::CChangeNotifier notifier(this);

		m_minimumAccessLevel = accessLevel;
	}
}


QByteArrayList CTenantFilterParam::GetUserGroupIds() const
{
	return m_userGroupIds;
}


bool CTenantFilterParam::IsGroupRestrictionEnabled() const
{
	return m_isGroupRestrictionEnabled;
}


void CTenantFilterParam::SetUserGroupIds(const QByteArrayList& groupIds, bool restrictionEnabled)
{
	if (m_userGroupIds != groupIds || m_isGroupRestrictionEnabled != restrictionEnabled){
		istd::CChangeNotifier notifier(this);

		m_userGroupIds = groupIds;
		m_isGroupRestrictionEnabled = restrictionEnabled;
	}
}


QByteArrayList CTenantFilterParam::GetShareOwnerTenantIds() const
{
	return m_shareOwnerTenantIds;
}


bool CTenantFilterParam::IsShareOwnerRestrictionEnabled() const
{
	return m_isShareOwnerRestrictionEnabled;
}


void CTenantFilterParam::SetShareOwnerTenantIds(const QByteArrayList& tenantIds, bool restrictionEnabled)
{
	if (m_shareOwnerTenantIds != tenantIds || m_isShareOwnerRestrictionEnabled != restrictionEnabled){
		istd::CChangeNotifier notifier(this);

		m_shareOwnerTenantIds = tenantIds;
		m_isShareOwnerRestrictionEnabled = restrictionEnabled;
	}
}


int CTenantFilterParam::GetSharesRevision() const
{
	return m_sharesRevision;
}


void CTenantFilterParam::SetSharesRevision(int revision)
{
	if (m_sharesRevision != revision){
		istd::CChangeNotifier notifier(this);

		m_sharesRevision = revision;
	}
}


// reimplemented (iser::ISerializable)

bool CTenantFilterParam::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag tenantIdTag("TenantId", "Tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantIdTag);
	retVal = retVal && archive.Process(m_tenantId);
	retVal = retVal && archive.EndTag(tenantIdTag);

	iser::CArchiveTag ownerIdTag("OwnerId", "Owner ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(ownerIdTag);
	retVal = retVal && archive.Process(m_ownerId);
	retVal = retVal && archive.EndTag(ownerIdTag);

	iser::CArchiveTag filterModeTag("FilterMode", "Filter Mode", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(filterModeTag);
	int filterModeInt = static_cast<int>(m_filterMode);
	retVal = retVal && archive.Process(filterModeInt);
	m_filterMode = static_cast<TenantFilterMode>(filterModeInt);
	retVal = retVal && archive.EndTag(filterModeTag);

	iser::CArchiveTag sharingScopeTag("SharingScope", "Sharing scope", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sharingScopeTag);
	int sharingScopeInt = static_cast<int>(m_sharingScope);
	retVal = retVal && archive.Process(sharingScopeInt);
	m_sharingScope = static_cast<SharingScope>(sharingScopeInt);
	retVal = retVal && archive.EndTag(sharingScopeTag);

	iser::CArchiveTag accessLevelTag("MinimumAccessLevel", "Minimum access level", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(accessLevelTag);
	int accessLevelInt = static_cast<int>(m_minimumAccessLevel);
	retVal = retVal && archive.Process(accessLevelInt);
	m_minimumAccessLevel = static_cast<IDocumentShareInfo::AccessLevel>(accessLevelInt);
	retVal = retVal && archive.EndTag(accessLevelTag);

	iser::CArchiveTag groupIdsTag("UserGroupIds", "User group IDs (semicolon-separated)", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(groupIdsTag);
	QByteArray groupIds = m_userGroupIds.join(';');
	retVal = retVal && archive.Process(groupIds);
	if (!archive.IsStoring()){
		m_userGroupIds = groupIds.split(';');
		m_userGroupIds.removeAll(QByteArray());
	}
	retVal = retVal && archive.EndTag(groupIdsTag);

	iser::CArchiveTag groupRestrictionTag("GroupRestrictionEnabled", "Group restriction enabled", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(groupRestrictionTag);
	retVal = retVal && archive.Process(m_isGroupRestrictionEnabled);
	retVal = retVal && archive.EndTag(groupRestrictionTag);

	iser::CArchiveTag shareOwnersTag("ShareOwnerTenantIds", "Organizations whose shares are effective (semicolon-separated)", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(shareOwnersTag);
	QByteArray shareOwnerTenantIds = m_shareOwnerTenantIds.join(';');
	retVal = retVal && archive.Process(shareOwnerTenantIds);
	if (!archive.IsStoring()){
		m_shareOwnerTenantIds = shareOwnerTenantIds.split(';');
		m_shareOwnerTenantIds.removeAll(QByteArray());
	}
	retVal = retVal && archive.EndTag(shareOwnersTag);

	iser::CArchiveTag shareOwnerRestrictionTag("ShareOwnerRestrictionEnabled", "Share owner restriction enabled", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(shareOwnerRestrictionTag);
	retVal = retVal && archive.Process(m_isShareOwnerRestrictionEnabled);
	retVal = retVal && archive.EndTag(shareOwnerRestrictionTag);

	iser::CArchiveTag sharesRevisionTag("SharesRevision", "Revision of the document shares", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sharesRevisionTag);
	retVal = retVal && archive.Process(m_sharesRevision);
	retVal = retVal && archive.EndTag(sharesRevisionTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CTenantFilterParam::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_RESET;
}


bool CTenantFilterParam::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const ITenantFilterParam* sourcePtr = dynamic_cast<const ITenantFilterParam*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier notifier(this);

		m_tenantId = sourcePtr->GetTenantId();
		m_ownerId = sourcePtr->GetOwnerId();
		m_filterMode = sourcePtr->GetFilterMode();
		m_sharingScope = sourcePtr->GetSharingScope();
		m_minimumAccessLevel = sourcePtr->GetMinimumAccessLevel();
		m_userGroupIds = sourcePtr->GetUserGroupIds();
		m_isGroupRestrictionEnabled = sourcePtr->IsGroupRestrictionEnabled();
		m_shareOwnerTenantIds = sourcePtr->GetShareOwnerTenantIds();
		m_isShareOwnerRestrictionEnabled = sourcePtr->IsShareOwnerRestrictionEnabled();
		m_sharesRevision = sourcePtr->GetSharesRevision();

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CTenantFilterParam::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTenantFilterParam());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTenantFilterParam::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_tenantId.clear();
	m_ownerId.clear();
	m_filterMode = TFM_INCLUDE;
	m_sharingScope = SS_OWN;
	m_minimumAccessLevel = IDocumentShareInfo::DAL_VIEW;
	m_userGroupIds.clear();
	m_isGroupRestrictionEnabled = false;
	m_shareOwnerTenantIds.clear();
	m_isShareOwnerRestrictionEnabled = false;
	m_sharesRevision = 0;

	return true;
}


} // namespace imtauth
