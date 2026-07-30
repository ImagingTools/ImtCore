// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtauth/IDocumentShare.h>


namespace imtauth
{


/**
	Self-contained mock replicating the semantics of CDocumentShareManagerComp
	(validation, access mode update, revocation and access resolution) without
	bringing up the component framework.
*/
class CMockDocumentShareManager
{
public:
	void RegisterRelationship(const QByteArray& sourceTenantId, const QByteArray& relationshipId)
	{
		m_relationships.append(QByteArray(sourceTenantId + '|' + relationshipId));
	}

	bool ValidateRelationship(const QByteArray& sourceTenantId, const QByteArray& relationshipId) const
	{
		if (relationshipId.isEmpty()){
			return true;
		}

		return m_relationships.contains(QByteArray(sourceTenantId + '|' + relationshipId));
	}

	QByteArray CreateShare(const DocumentShareInfo& shareInfo)
	{
		if (shareInfo.sourceTenantId.isEmpty() || shareInfo.targetTenantId.isEmpty() || shareInfo.documentId.isEmpty()){
			return QByteArray();
		}
		if (shareInfo.sourceTenantId == shareInfo.targetTenantId){
			return QByteArray();
		}
		if (!ValidateRelationship(shareInfo.sourceTenantId, shareInfo.relationshipId)){
			return QByteArray();
		}

		QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

		DocumentShareInfo info = shareInfo;
		info.shareId = QByteArray("share-") + QByteArray::number(++m_counter);
		info.status = DSS_ACTIVE;
		info.createdAt = now;
		info.updatedAt = now;
		info.revokedAt.clear();

		m_shares.append(info);

		return info.shareId;
	}

	DocumentShareInfo GetShare(const QByteArray& shareId) const
	{
		for (const DocumentShareInfo& info : m_shares){
			if (info.shareId == shareId){
				return info;
			}
		}

		return DocumentShareInfo();
	}

	DocumentShares GetIncomingShares(const QByteArray& targetTenantId) const
	{
		DocumentShares result;
		for (const DocumentShareInfo& info : m_shares){
			if (info.targetTenantId == targetTenantId){
				result.append(info);
			}
		}

		return result;
	}

	DocumentShares GetOutgoingShares(const QByteArray& sourceTenantId) const
	{
		DocumentShares result;
		for (const DocumentShareInfo& info : m_shares){
			if (info.sourceTenantId == sourceTenantId){
				result.append(info);
			}
		}

		return result;
	}

	DocumentShares GetSharesOfDocument(const QByteArray& documentId) const
	{
		DocumentShares result;
		for (const DocumentShareInfo& info : m_shares){
			if (info.documentId == documentId){
				result.append(info);
			}
		}

		return result;
	}

	bool SetAccessMode(const QByteArray& shareId, DocumentShareAccessMode accessMode)
	{
		for (DocumentShareInfo& info : m_shares){
			if (info.shareId == shareId){
				if (info.status != DSS_ACTIVE){
					return false;
				}

				info.accessMode = accessMode;
				info.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

				return true;
			}
		}

		return false;
	}

	bool RevokeShare(const QByteArray& shareId)
	{
		for (DocumentShareInfo& info : m_shares){
			if (info.shareId == shareId){
				if (info.status == DSS_REVOKED){
					return true;
				}

				QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

				info.status = DSS_REVOKED;
				info.revokedAt = now;
				info.updatedAt = now;

				return true;
			}
		}

		return false;
	}

	bool GetDocumentAccess(
				const QByteArray& tenantId,
				const QByteArray& documentId,
				DocumentShareAccessMode* accessModePtr = nullptr) const
	{
		if (tenantId.isEmpty() || documentId.isEmpty()){
			return false;
		}

		bool hasAccess = false;
		DocumentShareAccessMode accessMode = DSAM_READ_ONLY;

		for (const DocumentShareInfo& info : m_shares){
			if (info.targetTenantId != tenantId || info.documentId != documentId){
				continue;
			}
			if (info.status != DSS_ACTIVE || IsExpired(info.expiresAt)){
				continue;
			}

			hasAccess = true;
			if (info.accessMode == DSAM_FULL_ACCESS){
				accessMode = DSAM_FULL_ACCESS;
			}
		}

		if (hasAccess && accessModePtr != nullptr){
			*accessModePtr = accessMode;
		}

		return hasAccess;
	}

	static bool IsExpired(const QString& expiresAt)
	{
		if (expiresAt.isEmpty()){
			return false;
		}

		QDateTime expires = QDateTime::fromString(expiresAt, Qt::ISODateWithMs);
		if (!expires.isValid()){
			return false;
		}

		return expires < QDateTime::currentDateTimeUtc();
	}

	DocumentShares m_shares;
	QList<QByteArray> m_relationships;
	int m_counter = 0;
};


} // namespace imtauth


class CDocumentShareTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Data object tests
	void testShareInfo_RoundTrip();
	void testShareInfo_CopyFromAndClone();
	void testShareInfo_ResetDataDefaults();
	void testShareInfo_FullAccessSerializationRoundTrip();

	// Manager semantics
	void testCreateShare_MissingFields_Fails();
	void testCreateShare_SameTenant_Fails();
	void testCreateShare_RequiresRelationship();
	void testCreateShare_KeepsFullAccess();
	void testCreateShare_DefaultsToReadOnly();
	void testSetAccessMode_UpdatesShare();
	void testSetAccessMode_RevokedShare_Fails();
	void testRevokeShare_RemovesAccess();
	void testGetDocumentAccess_MostPermissiveWins();
	void testGetDocumentAccess_ExpiredShareIgnored();
	void testShareLists_IncomingOutgoingAndDocument();

private:
	imtauth::CMockDocumentShareManager* m_managerPtr = nullptr;
};
