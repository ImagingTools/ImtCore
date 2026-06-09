// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CCrossOrgGrant.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


CCrossOrgGrant::CCrossOrgGrant()
{
	m_info.isActive = true;
}


CrossOrgGrantInfo CCrossOrgGrant::GetGrantInfo() const
{
	return m_info;
}


void CCrossOrgGrant::SetGrantInfo(const CrossOrgGrantInfo& grantInfo)
{
	if (m_info != grantInfo){
		istd::CChangeNotifier notifier(this);
		m_info = grantInfo;
	}
}


QByteArray CCrossOrgGrant::GetGrantId() const
{
	return m_info.grantId;
}


bool CCrossOrgGrant::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_info.grantId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag sourceTag("SourceTenantId", "Source tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceTag);
	retVal = retVal && archive.Process(m_info.sourceTenantId);
	retVal = retVal && archive.EndTag(sourceTag);

	iser::CArchiveTag targetTag("TargetTenantId", "Target tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetTag);
	retVal = retVal && archive.Process(m_info.targetTenantId);
	retVal = retVal && archive.EndTag(targetTag);

	iser::CArchiveTag roleIdsTag("RoleIds", "Role IDs (semicolon-separated)", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleIdsTag);
	QString roleIdsStr;
	if (archive.IsStoring()){
		roleIdsStr = QString::fromUtf8(m_info.roleIds.join(';'));
	}
	retVal = retVal && archive.Process(roleIdsStr);
	if (!archive.IsStoring()){
		m_info.roleIds.clear();
		if (!roleIdsStr.isEmpty()){
			QStringList parts = roleIdsStr.split(';', Qt::SkipEmptyParts);
			for (const QString& part : parts){
				m_info.roleIds.append(part.trimmed().toUtf8());
			}
		}
	}
	retVal = retVal && archive.EndTag(roleIdsTag);

	iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_info.description);
	retVal = retVal && archive.EndTag(descriptionTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_info.createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag expiresAtTag("ExpiresAt", "Expires at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(expiresAtTag);
	retVal = retVal && archive.Process(m_info.expiresAt);
	retVal = retVal && archive.EndTag(expiresAtTag);

	iser::CArchiveTag isActiveTag("IsActive", "Is active", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isActiveTag);
	retVal = retVal && archive.Process(m_info.isActive);
	retVal = retVal && archive.EndTag(isActiveTag);

	return retVal;
}


bool CCrossOrgGrant::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CCrossOrgGrant* sourcePtr = dynamic_cast<const CCrossOrgGrant*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_info = sourcePtr->m_info;

	return true;
}


istd::IChangeableUniquePtr CCrossOrgGrant::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CCrossOrgGrant);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CCrossOrgGrant::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_info = CrossOrgGrantInfo();
	m_info.isActive = true;

	return true;
}


} // namespace imtauth
