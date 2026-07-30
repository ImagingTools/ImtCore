// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CDocumentShareTest.h"

// ACF includes
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <imtauth/CDocumentShare.h>


namespace
{


imtauth::DocumentShareInfo MakeSampleInfo()
{
	imtauth::DocumentShareInfo info;
	info.shareId = "share-1";
	info.sourceTenantId = "tenantB";
	info.targetTenantId = "tenantA";
	info.relationshipId = "rel-1";
	info.documentId = "doc-42";
	info.documentType = "Drawing";
	info.documentName = "Assembly.step";
	info.accessMode = imtauth::DSAM_FULL_ACCESS;
	info.status = imtauth::DSS_ACTIVE;
	info.sharedByUserId = "user-7";
	info.createdAt = "2026-01-01T00:00:00.000Z";
	info.updatedAt = "2026-01-01T00:01:00.000Z";
	info.expiresAt = "2030-01-01T00:00:00.000Z";
	info.revokedAt = QString();
	return info;
}


imtauth::DocumentShareInfo MakeShareRequest(imtauth::DocumentShareAccessMode accessMode)
{
	imtauth::DocumentShareInfo info;
	info.sourceTenantId = "tenantB";
	info.targetTenantId = "tenantA";
	info.relationshipId = "rel-1";
	info.documentId = "doc-42";
	info.documentType = "Drawing";
	info.documentName = "Assembly.step";
	info.accessMode = accessMode;
	return info;
}


} // anonymous namespace


void CDocumentShareTest::init()
{
	m_managerPtr = new imtauth::CMockDocumentShareManager();
	m_managerPtr->RegisterRelationship("tenantB", "rel-1");
}


void CDocumentShareTest::cleanup()
{
	delete m_managerPtr;
	m_managerPtr = nullptr;
}


void CDocumentShareTest::testShareInfo_RoundTrip()
{
	imtauth::CDocumentShare share;
	imtauth::DocumentShareInfo info = MakeSampleInfo();
	share.SetShareInfo(info);

	QCOMPARE(share.GetShareId(), QByteArray("share-1"));
	QVERIFY(share.GetShareInfo() == info);
}


void CDocumentShareTest::testShareInfo_CopyFromAndClone()
{
	imtauth::CDocumentShare source;
	source.SetShareInfo(MakeSampleInfo());

	imtauth::CDocumentShare target;
	QVERIFY(target.CopyFrom(source));
	QVERIFY(target.GetShareInfo() == source.GetShareInfo());

	istd::IChangeableUniquePtr clonePtr = source.CloneMe();
	QVERIFY(clonePtr.IsValid());
	const imtauth::IDocumentShareData* clonedPtr = dynamic_cast<const imtauth::IDocumentShareData*>(clonePtr.GetPtr());
	QVERIFY(clonedPtr != nullptr);
	QVERIFY(clonedPtr->GetShareInfo() == source.GetShareInfo());
	QCOMPARE(clonedPtr->GetShareInfo().accessMode, imtauth::DSAM_FULL_ACCESS);
}


void CDocumentShareTest::testShareInfo_ResetDataDefaults()
{
	imtauth::CDocumentShare share;
	share.SetShareInfo(MakeSampleInfo());

	QVERIFY(share.ResetData());

	imtauth::DocumentShareInfo info = share.GetShareInfo();
	QVERIFY(info.shareId.isEmpty());
	QVERIFY(info.sourceTenantId.isEmpty());
	QVERIFY(info.targetTenantId.isEmpty());
	QVERIFY(info.documentId.isEmpty());
	QCOMPARE(info.accessMode, imtauth::DSAM_READ_ONLY);
	QCOMPARE(info.status, imtauth::DSS_ACTIVE);
}


void CDocumentShareTest::testShareInfo_FullAccessSerializationRoundTrip()
{
	imtauth::CDocumentShare source;
	source.SetShareInfo(MakeSampleInfo());

	iser::CMemoryWriteArchive writeArchive;
	QVERIFY(source.Serialize(writeArchive));

	iser::CMemoryReadArchive readArchive(writeArchive.GetBuffer(), writeArchive.GetBufferSize());
	imtauth::CDocumentShare target;
	QVERIFY(target.Serialize(readArchive));

	// The non-default access mode has to survive the persistence round trip.
	QCOMPARE(target.GetShareInfo().accessMode, imtauth::DSAM_FULL_ACCESS);
	QVERIFY(target.GetShareInfo() == source.GetShareInfo());
}


void CDocumentShareTest::testCreateShare_MissingFields_Fails()
{
	imtauth::DocumentShareInfo info = MakeShareRequest(imtauth::DSAM_READ_ONLY);

	imtauth::DocumentShareInfo noSource = info;
	noSource.sourceTenantId.clear();
	QVERIFY(m_managerPtr->CreateShare(noSource).isEmpty());

	imtauth::DocumentShareInfo noTarget = info;
	noTarget.targetTenantId.clear();
	QVERIFY(m_managerPtr->CreateShare(noTarget).isEmpty());

	imtauth::DocumentShareInfo noDocument = info;
	noDocument.documentId.clear();
	QVERIFY(m_managerPtr->CreateShare(noDocument).isEmpty());
}


void CDocumentShareTest::testCreateShare_SameTenant_Fails()
{
	imtauth::DocumentShareInfo info = MakeShareRequest(imtauth::DSAM_READ_ONLY);
	info.targetTenantId = info.sourceTenantId;

	QVERIFY(m_managerPtr->CreateShare(info).isEmpty());
}


void CDocumentShareTest::testCreateShare_RequiresRelationship()
{
	imtauth::DocumentShareInfo info = MakeShareRequest(imtauth::DSAM_READ_ONLY);
	info.relationshipId = "rel-unknown";
	QVERIFY(m_managerPtr->CreateShare(info).isEmpty());

	info.relationshipId = "rel-1";
	QVERIFY(!m_managerPtr->CreateShare(info).isEmpty());
}


void CDocumentShareTest::testCreateShare_KeepsFullAccess()
{
	QByteArray shareId = m_managerPtr->CreateShare(MakeShareRequest(imtauth::DSAM_FULL_ACCESS));
	QVERIFY(!shareId.isEmpty());

	imtauth::DocumentShareInfo stored = m_managerPtr->GetShare(shareId);
	QCOMPARE(stored.accessMode, imtauth::DSAM_FULL_ACCESS);
	QCOMPARE(stored.status, imtauth::DSS_ACTIVE);
	QVERIFY(!stored.createdAt.isEmpty());

	imtauth::DocumentShareAccessMode accessMode = imtauth::DSAM_READ_ONLY;
	QVERIFY(m_managerPtr->GetDocumentAccess("tenantA", "doc-42", &accessMode));
	QCOMPARE(accessMode, imtauth::DSAM_FULL_ACCESS);
}


void CDocumentShareTest::testCreateShare_DefaultsToReadOnly()
{
	imtauth::DocumentShareInfo info;
	info.sourceTenantId = "tenantB";
	info.targetTenantId = "tenantA";
	info.documentId = "doc-1";

	QByteArray shareId = m_managerPtr->CreateShare(info);
	QVERIFY(!shareId.isEmpty());
	QCOMPARE(m_managerPtr->GetShare(shareId).accessMode, imtauth::DSAM_READ_ONLY);

	imtauth::DocumentShareAccessMode accessMode = imtauth::DSAM_FULL_ACCESS;
	QVERIFY(m_managerPtr->GetDocumentAccess("tenantA", "doc-1", &accessMode));
	QCOMPARE(accessMode, imtauth::DSAM_READ_ONLY);
}


void CDocumentShareTest::testSetAccessMode_UpdatesShare()
{
	QByteArray shareId = m_managerPtr->CreateShare(MakeShareRequest(imtauth::DSAM_READ_ONLY));
	QVERIFY(!shareId.isEmpty());

	QVERIFY(m_managerPtr->SetAccessMode(shareId, imtauth::DSAM_FULL_ACCESS));
	QCOMPARE(m_managerPtr->GetShare(shareId).accessMode, imtauth::DSAM_FULL_ACCESS);

	QVERIFY(m_managerPtr->SetAccessMode(shareId, imtauth::DSAM_READ_ONLY));
	QCOMPARE(m_managerPtr->GetShare(shareId).accessMode, imtauth::DSAM_READ_ONLY);

	QVERIFY(!m_managerPtr->SetAccessMode("share-unknown", imtauth::DSAM_FULL_ACCESS));
}


void CDocumentShareTest::testSetAccessMode_RevokedShare_Fails()
{
	QByteArray shareId = m_managerPtr->CreateShare(MakeShareRequest(imtauth::DSAM_READ_ONLY));
	QVERIFY(m_managerPtr->RevokeShare(shareId));

	QVERIFY(!m_managerPtr->SetAccessMode(shareId, imtauth::DSAM_FULL_ACCESS));
	QCOMPARE(m_managerPtr->GetShare(shareId).accessMode, imtauth::DSAM_READ_ONLY);
}


void CDocumentShareTest::testRevokeShare_RemovesAccess()
{
	QByteArray shareId = m_managerPtr->CreateShare(MakeShareRequest(imtauth::DSAM_FULL_ACCESS));
	QVERIFY(m_managerPtr->GetDocumentAccess("tenantA", "doc-42"));

	QVERIFY(m_managerPtr->RevokeShare(shareId));

	imtauth::DocumentShareInfo stored = m_managerPtr->GetShare(shareId);
	QCOMPARE(stored.status, imtauth::DSS_REVOKED);
	QVERIFY(!stored.revokedAt.isEmpty());
	QVERIFY(!m_managerPtr->GetDocumentAccess("tenantA", "doc-42"));

	// Revoking twice stays successful and idempotent.
	QVERIFY(m_managerPtr->RevokeShare(shareId));
	QVERIFY(!m_managerPtr->RevokeShare("share-unknown"));
}


void CDocumentShareTest::testGetDocumentAccess_MostPermissiveWins()
{
	QVERIFY(!m_managerPtr->CreateShare(MakeShareRequest(imtauth::DSAM_READ_ONLY)).isEmpty());
	QVERIFY(!m_managerPtr->CreateShare(MakeShareRequest(imtauth::DSAM_FULL_ACCESS)).isEmpty());

	imtauth::DocumentShareAccessMode accessMode = imtauth::DSAM_READ_ONLY;
	QVERIFY(m_managerPtr->GetDocumentAccess("tenantA", "doc-42", &accessMode));
	QCOMPARE(accessMode, imtauth::DSAM_FULL_ACCESS);

	// Tenants without a share have no access at all.
	QVERIFY(!m_managerPtr->GetDocumentAccess("tenantC", "doc-42"));
	QVERIFY(!m_managerPtr->GetDocumentAccess("tenantA", "doc-unknown"));
	QVERIFY(!m_managerPtr->GetDocumentAccess("", "doc-42"));
}


void CDocumentShareTest::testGetDocumentAccess_ExpiredShareIgnored()
{
	imtauth::DocumentShareInfo info = MakeShareRequest(imtauth::DSAM_FULL_ACCESS);
	info.expiresAt = QDateTime::currentDateTimeUtc().addDays(-1).toString(Qt::ISODateWithMs);

	QVERIFY(!m_managerPtr->CreateShare(info).isEmpty());
	QVERIFY(!m_managerPtr->GetDocumentAccess("tenantA", "doc-42"));

	info.expiresAt = QDateTime::currentDateTimeUtc().addDays(1).toString(Qt::ISODateWithMs);
	QVERIFY(!m_managerPtr->CreateShare(info).isEmpty());

	imtauth::DocumentShareAccessMode accessMode = imtauth::DSAM_READ_ONLY;
	QVERIFY(m_managerPtr->GetDocumentAccess("tenantA", "doc-42", &accessMode));
	QCOMPARE(accessMode, imtauth::DSAM_FULL_ACCESS);
}


void CDocumentShareTest::testShareLists_IncomingOutgoingAndDocument()
{
	QVERIFY(!m_managerPtr->CreateShare(MakeShareRequest(imtauth::DSAM_READ_ONLY)).isEmpty());

	imtauth::DocumentShareInfo otherDocument = MakeShareRequest(imtauth::DSAM_FULL_ACCESS);
	otherDocument.documentId = "doc-43";
	QVERIFY(!m_managerPtr->CreateShare(otherDocument).isEmpty());

	QCOMPARE(m_managerPtr->GetOutgoingShares("tenantB").size(), 2);
	QCOMPARE(m_managerPtr->GetIncomingShares("tenantA").size(), 2);
	QCOMPARE(m_managerPtr->GetIncomingShares("tenantB").size(), 0);
	QCOMPARE(m_managerPtr->GetSharesOfDocument("doc-42").size(), 1);
	QCOMPARE(m_managerPtr->GetSharesOfDocument("doc-43").size(), 1);
}
