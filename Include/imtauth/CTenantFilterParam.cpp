// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantFilterParam.h>


// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>


namespace imtauth
{


CTenantFilterParam::CTenantFilterParam()
	: m_filterMode(TFM_INCLUDE)
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

	return true;
}


} // namespace imtauth
