// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CDocumentShareTest.h"


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CDocumentShareInfo.h>
#include <imtauth/CTenantFilterParam.h>


namespace
{


void FillSampleShare(imtauth::IDocumentShareInfo& share)
{
	share.SetShareId("share-1");
	share.SetCollectionId("Accounts");
	share.SetEntityId("entity-1");
	share.SetEntityName("Account; with separator");
	share.SetOwnerTenantId("tenant-owner");
	share.SetOwnerTenantName("Owner Ltd");
	share.SetTargetTenantId("tenant-target");
	share.SetTargetTenantName("Target GmbH");
	share.SetTargetGroupIds(QByteArrayList{"group-1", "group-2"});
	share.SetAccessLevel(imtauth::IDocumentShareInfo::DAL_EDIT);
	share.SetStatus(imtauth::IDocumentShareInfo::DSS_ACTIVE);
	share.SetValidUntil(QDateTime::fromString("2030-01-01T00:00:00.000Z", Qt::ISODateWithMs));
	share.SetMessage("Please review");
	share.SetContractId("contract-1");
	share.SetCreatedByUserId("user-1");
	share.SetCreatedAt(QDateTime::fromString("2026-10-21T10:00:00.000Z", Qt::ISODateWithMs));
	share.SetUpdatedAt(QDateTime::fromString("2026-10-21T11:00:00.000Z", Qt::ISODateWithMs));
}


QByteArray SerializeToBytes(iser::ISerializable& object)
{
	iser::CMemoryWriteArchive archive;
	if (!object.Serialize(archive)){
		return QByteArray();
	}

	return QByteArray(static_cast<const char*>(archive.GetBuffer()), archive.GetBufferSize());
}


bool DeserializeFromBytes(iser::ISerializable& object, const QByteArray& data)
{
	iser::CMemoryReadArchive archive(data.constData(), data.size());

	return object.Serialize(archive);
}


} // anonymous namespace


void CDocumentShareTest::testShare_SerializeRoundTrip()
{
	imtauth::CDocumentShareInfo source;
	FillSampleShare(source);

	const QByteArray data = SerializeToBytes(source);
	QVERIFY(!data.isEmpty());

	imtauth::CDocumentShareInfo target;
	QVERIFY(DeserializeFromBytes(target, data));
	QVERIFY(target.IsEqual(source));
	QCOMPARE(target.GetTargetGroupIds(), QByteArrayList({"group-1", "group-2"}));
	QCOMPARE(target.GetEntityName(), QString("Account; with separator"));
	QCOMPARE(target.GetContractId(), QByteArray("contract-1"));
}


void CDocumentShareTest::testShare_CopyFromAndClone()
{
	imtauth::CDocumentShareInfo source;
	FillSampleShare(source);

	imtauth::CDocumentShareInfo target;
	QVERIFY(target.CopyFrom(source));
	QVERIFY(target.IsEqual(source));

	target.SetMessage("Changed");
	QVERIFY(!target.IsEqual(source));

	istd::IChangeableUniquePtr clonePtr = source.CloneMe();
	QVERIFY(clonePtr.IsValid());

	const imtauth::IDocumentShareInfo* clonedPtr = dynamic_cast<const imtauth::IDocumentShareInfo*>(clonePtr.GetPtr());
	QVERIFY(clonedPtr != nullptr);
	QVERIFY(clonedPtr->IsEqual(source));
}


void CDocumentShareTest::testShare_ResetData()
{
	imtauth::CDocumentShareInfo share;
	FillSampleShare(share);

	QVERIFY(share.ResetData());
	QVERIFY(share.IsEqual(imtauth::CDocumentShareInfo()));
	QCOMPARE(share.GetStatus(), imtauth::IDocumentShareInfo::DSS_PENDING);
	QCOMPARE(share.GetAccessLevel(), imtauth::IDocumentShareInfo::DAL_VIEW);
}


void CDocumentShareTest::testTenantFilter_DefaultsSelectOwnDocuments()
{
	imtauth::CTenantFilterParam filter;

	QCOMPARE(filter.GetSharingScope(), imtauth::SS_OWN);
	QCOMPARE(filter.GetTenantType(), imtbase::TT_ORGANIZATION);
	QCOMPARE(filter.GetMinimumAccessLevel(), imtauth::IDocumentShareInfo::DAL_VIEW);
	QVERIFY(!filter.IsGroupRestrictionEnabled());
	QVERIFY(filter.GetUserGroupIds().isEmpty());
}


void CDocumentShareTest::testTenantFilter_SerializeRoundTrip()
{
	imtauth::CTenantFilterParam source;
	source.SetTenantId("tenant-1");
	source.SetTenantType(imtbase::TT_USER);
	source.SetSharingScope(imtauth::SS_INCOMING);
	source.SetMinimumAccessLevel(imtauth::IDocumentShareInfo::DAL_EDIT);
	source.SetUserGroupIds(QByteArrayList{"group-1", "group-2"}, true);

	imtauth::CTenantFilterParam target;
	QVERIFY(DeserializeFromBytes(target, SerializeToBytes(source)));

	QCOMPARE(target.GetTenantId(), QByteArray("tenant-1"));
	QCOMPARE(target.GetTenantType(), imtbase::TT_USER);
	QCOMPARE(target.GetSharingScope(), imtauth::SS_INCOMING);
	QCOMPARE(target.GetMinimumAccessLevel(), imtauth::IDocumentShareInfo::DAL_EDIT);
	QCOMPARE(target.GetUserGroupIds(), QByteArrayList({"group-1", "group-2"}));
	QVERIFY(target.IsGroupRestrictionEnabled());
}


void CDocumentShareTest::testTenantFilter_SerializationDiffersBySharing()
{
	imtauth::CTenantFilterParam baseFilter;
	baseFilter.SetTenantId("tenant-1");
	const QByteArray baseData = SerializeToBytes(baseFilter);

	imtauth::CTenantFilterParam scopeFilter;
	QVERIFY(scopeFilter.CopyFrom(baseFilter));
	scopeFilter.SetSharingScope(imtauth::SS_ALL);
	QVERIFY(SerializeToBytes(scopeFilter) != baseData);

	imtauth::CTenantFilterParam groupFilter;
	QVERIFY(groupFilter.CopyFrom(scopeFilter));
	groupFilter.SetUserGroupIds(QByteArrayList{"group-1"}, true);

	imtauth::CTenantFilterParam otherGroupFilter;
	QVERIFY(otherGroupFilter.CopyFrom(scopeFilter));
	otherGroupFilter.SetUserGroupIds(QByteArrayList{"group-2"}, true);

	// Users of different groups must not share a cached selection
	QVERIFY(SerializeToBytes(groupFilter) != SerializeToBytes(otherGroupFilter));
	QVERIFY(SerializeToBytes(groupFilter) != SerializeToBytes(scopeFilter));

	// A personal space must not share a cached selection with an organization of the same ID
	imtauth::CTenantFilterParam userFilter;
	QVERIFY(userFilter.CopyFrom(baseFilter));
	userFilter.SetTenantType(imtbase::TT_USER);
	QVERIFY(SerializeToBytes(userFilter) != baseData);
}


void CDocumentShareTest::testTenantFilter_CopyFrom()
{
	imtauth::CTenantFilterParam source;
	source.SetTenantId("tenant-1");
	source.SetTenantType(imtbase::TT_USER);
	source.SetFilterMode(imtauth::TFM_EXCLUDE);
	source.SetSharingScope(imtauth::SS_OUTGOING);
	source.SetMinimumAccessLevel(imtauth::IDocumentShareInfo::DAL_OWNER);
	source.SetUserGroupIds(QByteArrayList{"group-1"}, true);

	imtauth::CTenantFilterParam target;
	QVERIFY(target.CopyFrom(source));
	QCOMPARE(target.GetTenantType(), imtbase::TT_USER);
	QCOMPARE(SerializeToBytes(target), SerializeToBytes(source));

	QVERIFY(target.ResetData());
	QCOMPARE(target.GetTenantType(), imtbase::TT_ORGANIZATION);
	QCOMPARE(target.GetSharingScope(), imtauth::SS_OWN);
	QVERIFY(!target.IsGroupRestrictionEnabled());
}


I_ADD_TEST(CDocumentShareTest);


