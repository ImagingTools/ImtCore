// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtauth/ICrossOrgGrant.h>


namespace imtauth
{


/**
	Self-contained mock replicating the access-resolution semantics of
	CCrossOrgGrantManagerComp, used to verify the expected behaviour of
	cross-org grants without bringing up the component framework.
*/
class CMockCrossOrgGrantManager
{
public:
	QByteArray CreateGrant(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QByteArrayList& roleIds,
				const QString& description = QString(),
				const QString& expiresAt = QString())
	{
		if (sourceTenantId.isEmpty() || targetTenantId.isEmpty()){
			return QByteArray();
		}
		if (sourceTenantId == targetTenantId){
			return QByteArray();
		}

		CrossOrgGrantInfo info;
		info.grantId = QByteArray::number(++m_counter);
		info.sourceTenantId = sourceTenantId;
		info.targetTenantId = targetTenantId;
		info.roleIds = roleIds;
		info.description = description;
		info.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
		info.expiresAt = expiresAt;
		info.isActive = true;

		m_grants.append(info);
		return info.grantId;
	}

	bool RevokeGrant(const QByteArray& grantId)
	{
		for (CrossOrgGrantInfo& info : m_grants){
			if (info.grantId == grantId && info.isActive){
				info.isActive = false;
				return true;
			}
		}
		return false;
	}

	CrossOrgGrants GetGrantsByTargetTenant(const QByteArray& targetTenantId) const
	{
		CrossOrgGrants result;
		for (const CrossOrgGrantInfo& info : m_grants){
			if (info.targetTenantId == targetTenantId){
				result.append(info);
			}
		}
		return result;
	}

	static bool IsGrantEffective(const CrossOrgGrantInfo& info)
	{
		if (!info.isActive){
			return false;
		}
		if (!info.expiresAt.isEmpty()){
			QDateTime expiresAt = QDateTime::fromString(info.expiresAt, Qt::ISODateWithMs);
			if (expiresAt.isValid() && expiresAt < QDateTime::currentDateTimeUtc()){
				return false;
			}
		}
		return true;
	}

	bool HasAccess(
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				const QByteArray& roleId) const
	{
		if (roleId.isEmpty()){
			return true;
		}

		for (const CrossOrgGrantInfo& info : GetGrantsByTargetTenant(targetTenantId)){
			if (info.sourceTenantId != sourceTenantId){
				continue;
			}
			if (!IsGrantEffective(info)){
				continue;
			}
			if (info.roleIds.contains(roleId)){
				return true;
			}
		}

		return false;
	}

	QList<CrossOrgGrantInfo> m_grants;
	int m_counter = 0;
};


} // namespace imtauth


class CCrossOrgGrantTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Data object tests
	void testGrantInfo_RoundTrip();
	void testGrantInfo_CopyFromAndClone();
	void testGrantInfo_ResetDataDefaults();

	// Manager semantics
	void testCreateGrant_Success();
	void testCreateGrant_MissingFields_Fails();
	void testCreateGrant_SameTenant_Fails();
	void testRevokeGrant_DeactivatesGrant();
	void testRevokeGrant_Unknown_ReturnsFalse();

	// Access resolution
	void testHasAccess_NoneAlwaysAllowed();
	void testHasAccess_GrantMatrix();
	void testHasAccess_RevokedDenied();
	void testHasAccess_ExpiredDenied();
	void testHasAccess_ScopedGrant();
	void testHasAccess_WrongSourceDenied();

private:
	imtauth::CMockCrossOrgGrantManager* m_managerPtr = nullptr;
};
