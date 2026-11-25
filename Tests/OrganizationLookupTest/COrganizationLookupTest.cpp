// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "COrganizationLookupTest.h"


// STL includes
#include <atomic>
#include <thread>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TSimComponentWrap.h>
#include <icomp/TSimComponentsFactory.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CDocumentShareInfo.h>
#include <imtauth/CDocumentShareInfoComp.h>
#include <imtauth/CDocumentShareManagerComp.h>
#include <imtauth/CPersonalAccessToken.h>
#include <imtauth/CPersonalAccessTokenManagerComp.h>
#include <imtauth/CTenantConnection.h>
#include <imtauth/CTenantConnectionComp.h>
#include <imtauth/CTenantConnectionRequestManagerComp.h>
#include <imtauth/CTenantInfo.h>
#include <imtauth/CTenantInfoComp.h>
#include <imtauth/CTenantInvitation.h>
#include <imtauth/CTenantInvitationComp.h>
#include <imtauth/CTenantInvitationManagerComp.h>
#include <imtauth/CTenantManagerComp.h>
#include <imtauth/CTenantMembership.h>
#include <imtauth/CTenantMembershipComp.h>
#include <imtauth/CTenantMembershipManagerComp.h>
#include <imtauth/TenantEvents.h>
#include <imtauth/CUserVerification.h>
#include <imtauth/CUserVerificationControllerComp.h>
#include <imtdoc/imtdoc.h>

// Test includes
#include "CTestCollectionComp.h"


namespace
{


QByteArray NewId()
{
	return QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
}


QByteArrayList Sorted(QByteArrayList ids)
{
	std::sort(ids.begin(), ids.end());

	return ids;
}


class CMembershipFixture
{
public:
	CMembershipFixture()
	{
		collection->SetUuidFieldIds({"UserId", "TenantId"});
		collection->InitComponent();

		manager->SetRef("MembershipCollection", collection);
		manager->SetFactory("MembershipFactory", &membershipFactory);
		manager->InitComponent();
	}

	QByteArray Add(const QByteArray& userId, const QByteArray& tenantId, bool isActive = true)
	{
		const QByteArray membershipId = NewId();

		imtauth::CIdentifiableTenantMembership membership;
		membership.SetMembershipId(membershipId);
		membership.SetUserId(userId);
		membership.SetTenantId(tenantId);
		membership.SetActive(isActive);

		return collection->AddObject("TenantMembership", membership, membershipId);
	}

	CTestCollectionComp& Collection()
	{
		return collection.GetImpl();
	}

	imtauth::ITenantMembershipManager& Manager()
	{
		return manager.GetImpl();
	}

	icomp::TSimSharedComponentPtr<CTestCollectionComp> collection;
	icomp::TSimComponentsFactory<imtauth::CTenantMembershipComp> membershipFactory;
	icomp::TSimSharedComponentPtr<imtauth::CTenantMembershipManagerComp> manager;
};


class CTenantFixture
{
public:
	CTenantFixture()
	{
		collection->InitComponent();

		manager->SetRef("TenantCollection", collection);
		manager->SetFactory("TenantFactory", &tenantFactory);
		manager->InitComponent();
	}

	QByteArray Add(const QByteArray& ownerId, const QByteArray& creatorId, const QByteArray& parentTenantId = QByteArray())
	{
		const QByteArray tenantId = NewId();

		imtauth::CIdentifiableTenantInfo tenant;
		tenant.SetTenantId(tenantId);
		tenant.SetTenantName(QStringLiteral("Organization %1").arg(QString::fromUtf8(tenantId.left(8))));
		tenant.SetOwnerId(ownerId);
		tenant.SetCreatorId(creatorId);
		tenant.SetParentTenantId(parentTenantId);
		tenant.SetActive(true);

		return collection->AddObject("Tenant", tenant, tenantId);
	}

	void Rename(const QByteArray& tenantId, const QString& name)
	{
		imtbase::IObjectCollection::DataPtr dataPtr;
		QVERIFY(collection->GetObjectData(tenantId, dataPtr));

		imtauth::CIdentifiableTenantInfo tenant;
		QVERIFY(tenant.CopyFrom(*dataPtr));
		tenant.SetTenantName(name);
		QVERIFY(collection->SetObjectData(tenantId, tenant));
	}

	CTestCollectionComp& Collection()
	{
		return collection.GetImpl();
	}

	imtauth::ITenantManager& Manager()
	{
		return manager.GetImpl();
	}

	icomp::TSimSharedComponentPtr<CTestCollectionComp> collection;
	icomp::TSimComponentsFactory<imtauth::CTenantInfoComp> tenantFactory;
	icomp::TSimSharedComponentPtr<imtauth::CTenantManagerComp> manager;
};


class CInvitationFixture
{
public:
	CInvitationFixture()
	{
		collection->SetUuidFieldIds({"UserId", "TenantId"});
		collection->InitComponent();

		manager->SetRef("InvitationCollection", collection);
		manager->SetFactory("InvitationFactory", &invitationFactory);
		manager->SetRef("MembershipManager", memberships.manager);
		manager->InitComponent();
	}

	QByteArray Add(
				const QByteArray& tenantId,
				const QByteArray& invitedByUserId,
				imtauth::ITenantInvitation::TenantInvitationStatus status = imtauth::ITenantInvitation::TIS_PENDING,
				int expiresInDays = 7)
	{
		const QByteArray invitationId = NewId();
		const QDateTime now = QDateTime::currentDateTimeUtc();

		imtauth::CIdentifiableTenantInvitation invitation;
		invitation.SetInvitationId(invitationId);
		invitation.SetUserId(NewId());
		invitation.SetTenantId(tenantId);
		invitation.SetInvitedByUserId(invitedByUserId);
		invitation.SetStatus(status);
		invitation.SetCreatedAt(now.toString(Qt::ISODateWithMs));
		invitation.SetExpiresAt(now.addDays(expiresInDays).toString(Qt::ISODateWithMs));

		return collection->AddObject("TenantInvitation", invitation, invitationId);
	}

	imtauth::ITenantInvitationUniquePtr Get(const QByteArray& invitationId)
	{
		return manager->GetInvitation(invitationId);
	}

	CTestCollectionComp& Collection()
	{
		return collection.GetImpl();
	}

	imtauth::ITenantInvitationManager& Manager()
	{
		return manager.GetImpl();
	}

	CMembershipFixture memberships;
	icomp::TSimSharedComponentPtr<CTestCollectionComp> collection;
	icomp::TSimComponentsFactory<imtauth::CTenantInvitationComp> invitationFactory;
	icomp::TSimSharedComponentPtr<imtauth::CTenantInvitationManagerComp> manager;
};


class CShareFixture
{
public:
	CShareFixture()
	{
		collection->InitComponent();
		relationships->InitComponent();

		manager->SetRef("ShareCollection", collection);
		manager->SetFactory("ShareFactory", &shareFactory);
		manager->SetRef("RelationshipProvider", relationships);
		manager->SetBoolAttr("AutoAccept", true);
		manager->SetIntAttr("RelationshipCacheTimeout", 10);
		manager->InitComponent();
	}

	QByteArray Add(
				const QByteArray& ownerTenantId,
				const QByteArray& targetTenantId,
				const QDateTime& createdAt = QDateTime(),
				const QByteArray& entityId = QByteArray())
	{
		const QByteArray shareId = NewId();

		imtauth::CIdentifiableDocumentShareInfo share;
		share.SetShareId(shareId);
		share.SetCollectionId("Customers");
		share.SetEntityId(entityId.isEmpty() ? NewId() : entityId);
		share.SetCreatedAt(createdAt);
		share.SetOwnerTenantId(ownerTenantId);
		share.SetTargetTenantId(targetTenantId);
		share.SetAccessLevel(imtauth::IDocumentShareInfo::DAL_VIEW);
		share.SetStatus(imtauth::IDocumentShareInfo::DSS_ACTIVE);

		return collection->AddObject("DocumentShare", share, shareId);
	}

	QByteArrayList StoredShareIds()
	{
		return Sorted(collection->GetElementIds());
	}

	CTestCollectionComp& Collection()
	{
		return collection.GetImpl();
	}

	CTestRelationshipProviderComp& Relationships()
	{
		return relationships.GetImpl();
	}

	imtauth::IDocumentShareManager& Manager()
	{
		return manager.GetImpl();
	}

	icomp::TSimSharedComponentPtr<CTestCollectionComp> collection;
	icomp::TSimSharedComponentPtr<CTestRelationshipProviderComp> relationships;
	icomp::TSimComponentsFactory<imtauth::CDocumentShareInfoComp> shareFactory;
	icomp::TSimSharedComponentPtr<imtauth::CDocumentShareManagerComp> manager;
};


QByteArray WrongCode(const QByteArray& code)
{
	return code == "000000" ? QByteArray("111111") : QByteArray("000000");
}


/**
	The connection manager keeping the events it publishes.
*/
class CTestConnectionManagerComp: public imtauth::CTenantConnectionRequestManagerComp
{
public:
	typedef imtauth::CTenantConnectionRequestManagerComp BaseClass;

	I_BEGIN_COMPONENT(CTestConnectionManagerComp);
	I_END_COMPONENT;

	QStringList GetEventTypes() const
	{
		QStringList retVal;
		for (const QVariantMap& event : m_events){
			retVal << event.value(imtauth::TenantEvent::s_type).toString();
		}

		return retVal;
	}

	QList<QVariantMap> m_events;

	// reimplemented (istd::IChangeable)
	virtual void EndChanges(const ChangeSet& changeSet) override
	{
		m_events += imtauth::TenantEvent::GetEvents(changeSet);

		BaseClass::EndChanges(changeSet);
	}
};


class CConnectionFixture
{
public:
	CConnectionFixture()
	{
		for (CTestCollectionComp* collectionPtr : {&requests.GetImpl(), &codes.GetImpl(), &connections.GetImpl()}){
			collectionPtr->SetUuidFieldIds({"TenantAId", "TenantBId", "SourceTenantId", "TargetTenantId", "ConnectionId", "Id"});
		}
		requests->InitComponent();
		codes->InitComponent();
		connections->InitComponent();

		manager->SetRef("RequestCollection", requests);
		manager->SetRef("ConnectionCodeCollection", codes);
		manager->SetRef("ConnectionCollection", connections);
		manager->SetFactory("ConnectionFactory", &connectionFactory);
		manager->InitComponent();
	}

	QByteArray AddConnection(
				const QByteArray& tenantAId,
				const QByteArray& tenantBId,
				imtauth::ITenantConnectionInfo::ConnectionStatus status = imtauth::ITenantConnectionInfo::CS_ACTIVE)
	{
		const QByteArray connectionId = NewId();

		imtauth::CIdentifiableTenantConnection connection;
		connection.SetConnectionId(connectionId);
		connection.SetTenantAId(tenantAId);
		connection.SetTenantBId(tenantBId);
		connection.SetStatus(status);

		return connections->AddObject("TenantConnection", connection, connectionId);
	}

	CTestCollectionComp& Connections()
	{
		return connections.GetImpl();
	}

	imtauth::ITenantConnectionRequestManager& Manager()
	{
		return manager.GetImpl();
	}

	CTestConnectionManagerComp& EventManager()
	{
		return manager.GetImpl();
	}

	icomp::TSimSharedComponentPtr<CTestCollectionComp> requests;
	icomp::TSimSharedComponentPtr<CTestCollectionComp> codes;
	icomp::TSimSharedComponentPtr<CTestCollectionComp> connections;
	icomp::TSimComponentsFactory<imtauth::CTenantConnectionComp> connectionFactory;
	icomp::TSimSharedComponentPtr<CTestConnectionManagerComp> manager;
};


typedef icomp::TMakeComponentWrap<imtauth::CPersonalAccessToken, imtauth::IPersonalAccessToken, istd::IChangeable> CPersonalAccessTokenComp;


class CTokenFixture
{
public:
	CTokenFixture()
	{
		collection->SetUuidFieldIds({"UserId"});
		collection->InitComponent();

		manager->SetRef("TokenCollection", collection);
		manager->SetFactory("TokenFactory", &tokenFactory);
		// A private in-memory collection: the manager serializes the updates itself
		manager->SetBoolAttr("InMemoryUpdates", true);
		manager->InitComponent();
	}

	imtauth::IPersonalAccessTokenManager::TokenCreationResult Create(const QByteArray& userId)
	{
		return manager->CreateToken(userId, "Product", "Token", QString(), {"read"}, QDateTime());
	}

	CTestCollectionComp& Collection()
	{
		return collection.GetImpl();
	}

	imtauth::IPersonalAccessTokenManager& Manager()
	{
		return manager.GetImpl();
	}

	icomp::TSimSharedComponentPtr<CTestCollectionComp> collection;
	icomp::TSimComponentsFactory<CPersonalAccessTokenComp> tokenFactory;
	icomp::TSimSharedComponentPtr<imtauth::CPersonalAccessTokenManagerComp> manager;
};


class CVerificationFixture
{
public:
	CVerificationFixture()
	{
		collection->InitComponent();

		controller->SetRef("UserVerificationCollection", collection);
		controller->SetIntAttr("CodeValidityPeriod", 180);
		controller->InitComponent();
	}

	QByteArray Generate(const QByteArray& userId)
	{
		imtauth::CUserVerification verification;
		if (!controller->GenerateVerificationData(userId, &verification)){
			return QByteArray();
		}

		return verification.GetVerificationCode();
	}

	imtauth::IUserVerificationController& Controller()
	{
		return controller.GetImpl();
	}

	CTestCollectionComp& Collection()
	{
		return collection.GetImpl();
	}

	icomp::TSimSharedComponentPtr<CTestCollectionComp> collection;
	icomp::TSimSharedComponentPtr<imtauth::CUserVerificationControllerComp> controller;
};


} // anonymous namespace


// Memberships

void COrganizationLookupTest::testFindMembership_LoadsOnlyTheCandidate()
{
	CMembershipFixture fixture;

	const QByteArray userId = NewId();
	QByteArrayList tenantIds;
	for (int i = 0; i < 40; ++i){
		const QByteArray tenantId = NewId();
		tenantIds.append(tenantId);
		fixture.Add(NewId(), tenantId);
		fixture.Add(NewId(), tenantId);
	}
	const QByteArray membershipId = fixture.Add(userId, tenantIds[17]);

	fixture.Collection().ResetLoadCount();
	imtauth::ITenantMembershipUniquePtr membershipPtr = fixture.Manager().FindMembership(userId, tenantIds[17]);

	QVERIFY(membershipPtr.IsValid());
	QCOMPARE(membershipPtr->GetMembershipId(), membershipId);
	QCOMPARE(fixture.Collection().GetLoadCount(), 1);
	QVERIFY(fixture.Collection().GetUnknownFieldIds().isEmpty());

	fixture.Collection().ResetLoadCount();
	QVERIFY(!fixture.Manager().FindMembership(userId, tenantIds[3]).IsValid());
	QCOMPARE(fixture.Collection().GetLoadCount(), 0);
}


void COrganizationLookupTest::testFindMembership_CollectionWithoutFilters_FindsTheRightOne()
{
	CMembershipFixture fixture;
	fixture.Collection().SetFilterSupported(false);

	const QByteArray userId = NewId();
	const QByteArray otherUserId = NewId();
	const QByteArray tenantId = NewId();
	const QByteArray otherTenantId = NewId();
	fixture.Add(otherUserId, tenantId);
	fixture.Add(userId, otherTenantId);
	const QByteArray membershipId = fixture.Add(userId, tenantId);

	imtauth::ITenantMembershipUniquePtr membershipPtr = fixture.Manager().FindMembership(userId, tenantId);
	QVERIFY(membershipPtr.IsValid());
	QCOMPARE(membershipPtr->GetMembershipId(), membershipId);

	QVERIFY(!fixture.Manager().FindMembership(otherUserId, otherTenantId).IsValid());
}


void COrganizationLookupTest::testFindMembership_NonUuidIds_MatchNothing()
{
	CMembershipFixture fixture;

	const QByteArray tenantId = NewId();
	fixture.Add(NewId(), tenantId);
	fixture.Add("not-a-user", tenantId);

	fixture.Collection().ResetLoadCount();
	QVERIFY(!fixture.Manager().FindMembership("not-a-user", tenantId).IsValid());
	QVERIFY(fixture.Manager().GetMembershipsByUser("not-a-user").isEmpty());
	QVERIFY(fixture.Manager().GetMembershipsByTenant("not-a-tenant").isEmpty());
	QCOMPARE(fixture.Collection().GetLoadCount(), 0);
	QCOMPARE(fixture.Collection().GetUnfilteredSelectionCount(), 0);
}


void COrganizationLookupTest::testGetMembershipsByUser_ReturnsOnlyTheUsers()
{
	CMembershipFixture fixture;

	const QByteArray userId = NewId();
	QByteArrayList expectedIds;
	for (int i = 0; i < 30; ++i){
		const QByteArray tenantId = NewId();
		fixture.Add(NewId(), tenantId);
		if (i % 10 == 0){
			expectedIds.append(fixture.Add(userId, tenantId));
		}
	}

	fixture.Collection().ResetLoadCount();
	QCOMPARE(Sorted(fixture.Manager().GetMembershipsByUser(userId)), Sorted(expectedIds));
	QCOMPARE(fixture.Collection().GetLoadCount(), expectedIds.size());
}


void COrganizationLookupTest::testGetMembershipsByTenant_ReturnsOnlyTheTenants()
{
	CMembershipFixture fixture;

	const QByteArray tenantId = NewId();
	QByteArrayList expectedIds;
	for (int i = 0; i < 30; ++i){
		fixture.Add(NewId(), NewId());
		if (i % 7 == 0){
			expectedIds.append(fixture.Add(NewId(), tenantId));
		}
	}

	fixture.Collection().ResetLoadCount();
	QCOMPARE(Sorted(fixture.Manager().GetMembershipsByTenant(tenantId)), Sorted(expectedIds));
	QCOMPARE(fixture.Collection().GetLoadCount(), expectedIds.size());
}


void COrganizationLookupTest::testIsMember_InactiveMembershipIsNoMember()
{
	CMembershipFixture fixture;

	const QByteArray activeUserId = NewId();
	const QByteArray inactiveUserId = NewId();
	const QByteArray tenantId = NewId();
	fixture.Add(activeUserId, tenantId);
	fixture.Add(inactiveUserId, tenantId, false);

	QVERIFY(fixture.Manager().IsMember(activeUserId, tenantId));
	QVERIFY(!fixture.Manager().IsMember(inactiveUserId, tenantId));
	QVERIFY(!fixture.Manager().IsMember(NewId(), tenantId));
}


// Organizations

void COrganizationLookupTest::testOwnedOrCreated_ReturnsOwnedAndCreatedOnly()
{
	CTenantFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray otherUserId = NewId();
	QByteArrayList expectedIds;
	expectedIds.append(fixture.Add(userId, userId));
	expectedIds.append(fixture.Add(otherUserId, userId));
	expectedIds.append(fixture.Add(userId, otherUserId));
	for (int i = 0; i < 25; ++i){
		fixture.Add(otherUserId, otherUserId);
	}

	fixture.Collection().ResetLoadCount();
	QCOMPARE(Sorted(fixture.Manager().GetOwnedOrCreatedTenantIds(userId)), Sorted(expectedIds));
	QCOMPARE(fixture.Collection().GetLoadCount(), expectedIds.size());
	QVERIFY(fixture.Collection().GetUnknownFieldIds().isEmpty());
}


void COrganizationLookupTest::testOwnedOrCreated_CollectionWithoutFilters_ReturnsOwnedAndCreatedOnly()
{
	CTenantFixture fixture;
	fixture.Collection().SetFilterSupported(false);

	const QByteArray userId = NewId();
	const QByteArray otherUserId = NewId();
	QByteArrayList expectedIds;
	expectedIds.append(fixture.Add(otherUserId, userId));
	expectedIds.append(fixture.Add(userId, otherUserId));
	fixture.Add(otherUserId, otherUserId);

	QCOMPARE(Sorted(fixture.Manager().GetOwnedOrCreatedTenantIds(userId)), Sorted(expectedIds));
}


void COrganizationLookupTest::testOwnedOrCreated_InvalidUserId_ReturnsNothing()
{
	CTenantFixture fixture;
	fixture.Add(QByteArray(), QByteArray());

	QVERIFY(fixture.Manager().GetOwnedOrCreatedTenantIds(QByteArray()).isEmpty());
	QVERIFY(fixture.Manager().GetOwnedOrCreatedTenantIds("not-a-user").isEmpty());
}


void COrganizationLookupTest::testChildTenantIds_ReturnsDirectChildrenOnly()
{
	CTenantFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray parentId = fixture.Add(userId, userId);
	const QByteArray firstChildId = fixture.Add(userId, userId, parentId);
	const QByteArray secondChildId = fixture.Add(userId, userId, parentId);
	const QByteArray grandchildId = fixture.Add(userId, userId, firstChildId);
	const QByteArray otherRootId = fixture.Add(userId, userId);

	QCOMPARE(Sorted(fixture.Manager().GetChildTenantIds(parentId)), Sorted({firstChildId, secondChildId}));
	QCOMPARE(fixture.Manager().GetChildTenantIds(firstChildId), QByteArrayList({grandchildId}));
	QVERIFY(fixture.Manager().GetChildTenantIds(otherRootId).isEmpty());
	QVERIFY(fixture.Manager().GetChildTenantIds(QByteArray()).isEmpty());

	fixture.Collection().SetFilterSupported(false);
	QCOMPARE(Sorted(fixture.Manager().GetChildTenantIds(parentId)), Sorted({firstChildId, secondChildId}));
}


void COrganizationLookupTest::testFindTenantIdsByName_IgnoresCaseAndSpacesNotParts()
{
	CTenantFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray acmeId = fixture.Add(userId, userId);
	fixture.Rename(acmeId, "  Acme Corp ");
	const QByteArray otherId = fixture.Add(userId, userId);
	fixture.Rename(otherId, "Acme Corporation");
	for (int i = 0; i < 20; ++i){
		fixture.Add(userId, userId);
	}

	fixture.Collection().ResetLoadCount();
	QCOMPARE(fixture.Manager().FindTenantIdsByName("acme corp"), QByteArrayList({acmeId}));
	QCOMPARE(fixture.Collection().GetLoadCount(), 2);
	QCOMPARE(fixture.Collection().GetUnfilteredSelectionCount(), 0);

	QCOMPARE(fixture.Manager().FindTenantIdsByName(" ACME CORPORATION "), QByteArrayList({otherId}));
	QVERIFY(fixture.Manager().FindTenantIdsByName("Acme").isEmpty());
	QVERIFY(fixture.Manager().FindTenantIdsByName("  ").isEmpty());

	fixture.Collection().SetFilterSupported(false);
	QCOMPARE(fixture.Manager().FindTenantIdsByName("ACME corp"), QByteArrayList({acmeId}));
}


// Invitations

void COrganizationLookupTest::testRevokeInvitationsSentBy_RevokesOnlyPendingInvitationsOfTheInviter()
{
	CInvitationFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray otherTenantId = NewId();
	const QByteArray inviterId = NewId();
	const QByteArray otherInviterId = NewId();
	const QByteArray adminId = NewId();

	const QByteArray firstPendingId = fixture.Add(tenantId, inviterId);
	const QByteArray secondPendingId = fixture.Add(tenantId, inviterId);
	const QByteArray otherInviterPendingId = fixture.Add(tenantId, otherInviterId);
	const QByteArray otherTenantPendingId = fixture.Add(otherTenantId, inviterId);
	const QByteArray acceptedId = fixture.Add(tenantId, inviterId, imtauth::ITenantInvitation::TIS_ACCEPTED);
	const QByteArray expiredId = fixture.Add(tenantId, inviterId, imtauth::ITenantInvitation::TIS_PENDING, -1);

	QVERIFY(fixture.Manager().RevokeInvitationsSentBy(tenantId, inviterId, adminId));

	for (const QByteArray& invitationId : {firstPendingId, secondPendingId}){
		imtauth::ITenantInvitationUniquePtr invitationPtr = fixture.Get(invitationId);
		QVERIFY(invitationPtr.IsValid());
		QCOMPARE(invitationPtr->GetStatus(), imtauth::ITenantInvitation::TIS_REVOKED);
		QCOMPARE(invitationPtr->GetRevokedByUserId(), adminId);
	}

	QCOMPARE(fixture.Get(otherInviterPendingId)->GetStatus(), imtauth::ITenantInvitation::TIS_PENDING);
	QCOMPARE(fixture.Get(otherTenantPendingId)->GetStatus(), imtauth::ITenantInvitation::TIS_PENDING);
	QCOMPARE(fixture.Get(acceptedId)->GetStatus(), imtauth::ITenantInvitation::TIS_ACCEPTED);
	QCOMPARE(fixture.Manager().GetEffectiveStatus(*fixture.Get(expiredId)), imtauth::ITenantInvitation::TIS_EXPIRED);

	QVERIFY(fixture.Manager().RevokeInvitationsSentBy(tenantId, inviterId, adminId));
	QVERIFY(fixture.Manager().RevokeInvitationsSentBy(tenantId, QByteArray(), adminId));
	QCOMPARE(fixture.Get(otherInviterPendingId)->GetStatus(), imtauth::ITenantInvitation::TIS_PENDING);
}


void COrganizationLookupTest::testGetInvitationsByUser_LoadsOnlyTheUsers()
{
	CInvitationFixture fixture;

	for (int i = 0; i < 20; ++i){
		fixture.Add(NewId(), NewId());
	}
	const QByteArray invitationId = fixture.Add(NewId(), NewId());
	const QByteArray userId = fixture.Get(invitationId)->GetUserId();

	fixture.Collection().ResetLoadCount();
	QCOMPARE(fixture.Manager().GetInvitationsByUser(userId), QByteArrayList({invitationId}));
	QCOMPARE(fixture.Collection().GetLoadCount(), 1);
	QVERIFY(fixture.Collection().GetUnknownFieldIds().isEmpty());
}


// Document shares

void COrganizationLookupTest::testRemoveUnrelatedShares_RemovesTheSharesWithoutRelationship()
{
	CShareFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	const QByteArray formerPartnerId = NewId();
	const QByteArray otherTenantId = NewId();
	fixture.Relationships().AddRelationship(tenantId, partnerId);

	const QByteArray givenToPartnerId = fixture.Add(tenantId, partnerId);
	const QByteArray receivedFromPartnerId = fixture.Add(partnerId, tenantId);
	fixture.Add(tenantId, formerPartnerId);
	fixture.Add(formerPartnerId, tenantId);
	const QByteArray foreignShareId = fixture.Add(otherTenantId, formerPartnerId);

	QCOMPARE(fixture.Manager().RemoveUnrelatedShares(tenantId), 2);
	QCOMPARE(fixture.StoredShareIds(), Sorted({givenToPartnerId, receivedFromPartnerId, foreignShareId}));
	QVERIFY(fixture.Collection().GetUnknownFieldIds().isEmpty());

	QCOMPARE(fixture.Manager().RemoveUnrelatedShares(tenantId), 0);
	QCOMPARE(fixture.Manager().RemoveUnrelatedShares(QByteArray()), 0);
}


void COrganizationLookupTest::testRemoveUnrelatedShares_UnreadableRelationshipsRemoveNothing()
{
	CShareFixture fixture;
	fixture.Relationships().SetReadable(false);

	const QByteArray tenantId = NewId();
	fixture.Add(tenantId, NewId());
	fixture.Add(NewId(), tenantId);
	const QByteArrayList storedIds = fixture.StoredShareIds();

	QCOMPARE(fixture.Manager().RemoveUnrelatedShares(tenantId), 0);
	QCOMPARE(fixture.StoredShareIds(), storedIds);
}


void COrganizationLookupTest::testRemoveUnrelatedShares_CollectionWithoutFilters_KeepsSharesOfOtherTenants()
{
	CShareFixture fixture;
	fixture.Collection().SetFilterSupported(false);

	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	fixture.Relationships().AddRelationship(tenantId, partnerId);

	const QByteArray givenToPartnerId = fixture.Add(tenantId, partnerId);
	fixture.Add(tenantId, NewId());
	const QByteArray foreignShareId = fixture.Add(NewId(), NewId());

	QCOMPARE(fixture.Manager().RemoveUnrelatedShares(tenantId), 1);
	QCOMPARE(fixture.StoredShareIds(), Sorted({givenToPartnerId, foreignShareId}));
}


void COrganizationLookupTest::testRemoveUnrelatedShares_RemovesTheSharesOfAnEarlierRelationship()
{
	CShareFixture fixture;

	const QDateTime now = QDateTime::currentDateTimeUtc();
	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	const QByteArray unknownTimePartnerId = NewId();
	fixture.Relationships().AddRelationship(tenantId, partnerId, now);
	fixture.Relationships().AddRelationship(tenantId, unknownTimePartnerId);

	fixture.Add(tenantId, partnerId, now.addSecs(-3600));
	fixture.Add(partnerId, tenantId, now.addSecs(-3600));
	const QByteArray withinToleranceId = fixture.Add(tenantId, partnerId, now.addSecs(-60));
	const QByteArray currentId = fixture.Add(partnerId, tenantId, now.addSecs(5));
	const QByteArray unknownTimeId = fixture.Add(tenantId, unknownTimePartnerId, now.addSecs(-3600));

	QCOMPARE(fixture.Manager().RemoveUnrelatedShares(tenantId), 2);
	QCOMPARE(fixture.StoredShareIds(), Sorted({withinToleranceId, currentId, unknownTimeId}));
}


void COrganizationLookupTest::testGetShareOwnerTenantIds_DropsTheSharesOfAnEarlierRelationship()
{
	CShareFixture fixture;

	const QDateTime now = QDateTime::currentDateTimeUtc();
	const QByteArray tenantId = NewId();
	const QByteArray formerPartnerId = NewId();
	const QByteArray partnerId = NewId();
	fixture.Relationships().AddRelationship(tenantId, formerPartnerId, now);
	fixture.Relationships().AddRelationship(tenantId, partnerId, now.addSecs(-7200));

	fixture.Add(formerPartnerId, tenantId, now.addSecs(-3600));
	const QByteArray partnerShareId = fixture.Add(partnerId, tenantId, now.addSecs(-3600));

	// Both relationships are active: the share of the earlier one goes, the owners stay
	QCOMPARE(Sorted(fixture.Manager().GetShareOwnerTenantIds(tenantId)), Sorted({formerPartnerId, partnerId}));
	QCOMPARE(fixture.StoredShareIds(), QByteArrayList({partnerShareId}));
}


void COrganizationLookupTest::testShareDocument_ReplacesTheShareOfAnEarlierRelationship()
{
	CShareFixture fixture;

	const QDateTime now = QDateTime::currentDateTimeUtc();
	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	fixture.Relationships().AddRelationship(tenantId, partnerId, now);

	const QByteArray entityId = NewId();
	const QByteArray staleShareId = fixture.Add(tenantId, partnerId, now.addSecs(-3600), entityId);

	imtauth::CIdentifiableDocumentShareInfo share;
	share.SetCollectionId("Customers");
	share.SetEntityId(entityId);
	share.SetOwnerTenantId(tenantId);
	share.SetTargetTenantId(partnerId);
	share.SetAccessLevel(imtauth::IDocumentShareInfo::DAL_VIEW);

	QString errorMessage;
	const QByteArray shareId = fixture.Manager().ShareDocument(share, &errorMessage);
	QVERIFY2(!shareId.isEmpty(), qPrintable(errorMessage));
	QVERIFY(shareId != staleShareId);
	QCOMPARE(fixture.StoredShareIds(), QByteArrayList({shareId}));

	// Sharing again within the same relationship updates the share
	QCOMPARE(fixture.Manager().ShareDocument(share, &errorMessage), shareId);
	QCOMPARE(fixture.StoredShareIds(), QByteArrayList({shareId}));
}


// Connections and relationships

void COrganizationLookupTest::testGetConnectionIds_LoadsOnlyTheTenants()
{
	CConnectionFixture fixture;

	const QByteArray tenantId = NewId();
	for (int i = 0; i < 20; ++i){
		fixture.AddConnection(NewId(), NewId());
	}
	const QByteArray asFirstId = fixture.AddConnection(tenantId, NewId());
	const QByteArray asSecondId = fixture.AddConnection(NewId(), tenantId);
	fixture.AddConnection(tenantId, NewId(), imtauth::ITenantConnectionInfo::CS_REMOVED);

	fixture.Connections().ResetLoadCount();
	QCOMPARE(Sorted(fixture.Manager().GetConnectionIds(tenantId)), Sorted({asFirstId, asSecondId}));
	QCOMPARE(fixture.Connections().GetLoadCount(), 3);
	QCOMPARE(fixture.Connections().GetUnfilteredSelectionCount(), 0);
	QVERIFY(fixture.Connections().GetUnknownFieldIds().isEmpty());
}


void COrganizationLookupTest::testConnectionLookups_NonUuidIds_MatchNothing()
{
	CConnectionFixture fixture;
	fixture.AddConnection("not-a-tenant", NewId());

	fixture.Connections().ResetLoadCount();
	QVERIFY(fixture.Manager().GetConnectionIds("not-a-tenant").isEmpty());
	QVERIFY(fixture.Manager().GetConnectionIds(QByteArray()).isEmpty());
	QCOMPARE(fixture.Connections().GetLoadCount(), 0);
}


// Personal access tokens

void COrganizationLookupTest::testValidateToken_LoadsOnlyTheToken()
{
	CTokenFixture fixture;

	for (int i = 0; i < 20; ++i){
		QVERIFY(fixture.Create(NewId()).success);
	}
	const QByteArray userId = NewId();
	const imtauth::IPersonalAccessTokenManager::TokenCreationResult created = fixture.Create(userId);
	QVERIFY(created.success);

	fixture.Collection().ResetLoadCount();
	QByteArray foundUserId;
	QByteArray foundTokenId;
	QByteArrayList scopes;
	QVERIFY(fixture.Manager().ValidateToken(created.rawToken, foundUserId, foundTokenId, scopes));
	QCOMPARE(foundUserId, userId);
	QCOMPARE(foundTokenId, created.tokenId);
	QCOMPARE(fixture.Collection().GetLoadCount(), 1);
	QCOMPARE(fixture.Collection().GetUnfilteredSelectionCount(), 0);

	fixture.Collection().ResetLoadCount();
	QVERIFY(!fixture.Manager().ValidateToken(created.rawToken + "x", foundUserId, foundTokenId, scopes));
	QCOMPARE(fixture.Collection().GetLoadCount(), 0);
	QVERIFY(fixture.Collection().GetUnknownFieldIds().isEmpty());
}


void COrganizationLookupTest::testGetTokenIds_LoadsOnlyTheUsers()
{
	CTokenFixture fixture;

	for (int i = 0; i < 20; ++i){
		QVERIFY(fixture.Create(NewId()).success);
	}
	const QByteArray userId = NewId();
	const QByteArray firstId = fixture.Create(userId).tokenId;
	const QByteArray secondId = fixture.Create(userId).tokenId;

	fixture.Collection().ResetLoadCount();
	QCOMPARE(Sorted(fixture.Manager().GetTokenIds(userId)), Sorted({firstId, secondId}));
	QCOMPARE(fixture.Collection().GetLoadCount(), 2);
	QCOMPARE(fixture.Collection().GetUnfilteredSelectionCount(), 0);

	QVERIFY(fixture.Manager().GetTokenIds("not-a-user").isEmpty());
}


// Verification codes

void COrganizationLookupTest::testVerifyUser_RightCodeAfterSomeWrongOnes()
{
	CVerificationFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray code = fixture.Generate(userId);
	for (int i = 0; i < 4; ++i){
		QVERIFY(!fixture.Controller().VerifyUser(userId, WrongCode(code)));
	}
	QVERIFY(!fixture.Controller().VerifyUser(NewId(), code));
	QVERIFY(fixture.Controller().VerifyUser(userId, code));

	// A right code clears the failures
	for (int i = 0; i < 4; ++i){
		QVERIFY(!fixture.Controller().VerifyUser(userId, WrongCode(code)));
	}
	QVERIFY(fixture.Controller().VerifyUser(userId, code));
}


void COrganizationLookupTest::testVerifyUser_TooManyWrongCodesEndTheVerification()
{
	CVerificationFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray otherUserId = NewId();
	const QByteArray code = fixture.Generate(userId);
	const QByteArray otherCode = fixture.Generate(otherUserId);
	for (int i = 0; i < 4; ++i){
		QVERIFY(!fixture.Controller().VerifyUser(userId, WrongCode(code)));
	}
	QVERIFY(!fixture.Controller().VerifyUser(userId, QByteArray()));

	QVERIFY(!fixture.Controller().VerifyUser(userId, code));
	QVERIFY(fixture.Controller().GetVerificationData(userId) == nullptr);
	QVERIFY(fixture.Controller().VerifyUser(otherUserId, otherCode));
}


void COrganizationLookupTest::testVerifyUser_NewCodeResetsTheAttempts()
{
	CVerificationFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray code = fixture.Generate(userId);
	for (int i = 0; i < 4; ++i){
		QVERIFY(!fixture.Controller().VerifyUser(userId, WrongCode(code)));
	}

	const QByteArray newCode = fixture.Generate(userId);
	for (int i = 0; i < 4; ++i){
		QVERIFY(!fixture.Controller().VerifyUser(userId, WrongCode(newCode)));
	}
	QVERIFY(fixture.Controller().VerifyUser(userId, newCode));
}


void COrganizationLookupTest::testRemoveVerificationData_EndsTheVerification()
{
	CVerificationFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray code = fixture.Generate(userId);
	QVERIFY(fixture.Controller().VerifyUser(userId, code));

	QVERIFY(fixture.Controller().RemoveVerificationData(userId));
	QVERIFY(!fixture.Controller().VerifyUser(userId, code));
	QVERIFY(fixture.Controller().GetVerificationData(userId) == nullptr);
}


void COrganizationLookupTest::testRemoveUnrelatedShares_KeepsTheSharesOfARelationshipOutsideItsValidity()
{
	CShareFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	fixture.Relationships().AddRelationship(tenantId, partnerId, QDateTime::currentDateTimeUtc().addDays(-30), false);

	fixture.Add(tenantId, partnerId, QDateTime::currentDateTimeUtc().addDays(-10));
	fixture.Add(partnerId, tenantId, QDateTime::currentDateTimeUtc().addDays(-10));
	const QByteArrayList storedIds = fixture.StoredShareIds();

	// The relationship exists but is not valid now: its shares grant nothing and stay until it is valid again
	QCOMPARE(fixture.Manager().RemoveUnrelatedShares(tenantId), 0);
	QVERIFY(fixture.Manager().GetShareOwnerTenantIds(tenantId).isEmpty());
	QCOMPARE(fixture.StoredShareIds(), storedIds);
}


void COrganizationLookupTest::testShareDocument_RefusesAPartnerWithoutAContractInForce()
{
	CShareFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	fixture.Relationships().AddRelationship(tenantId, partnerId, QDateTime(), false);

	imtauth::CIdentifiableDocumentShareInfo share;
	share.SetCollectionId("Customers");
	share.SetEntityId(NewId());
	share.SetOwnerTenantId(tenantId);
	share.SetTargetTenantId(partnerId);
	share.SetAccessLevel(imtauth::IDocumentShareInfo::DAL_VIEW);

	QString errorMessage;
	QVERIFY(fixture.Manager().ShareDocument(share, &errorMessage).isEmpty());
	QCOMPARE(errorMessage, QStringLiteral("No contract in force allows the organizations to share documents"));
	QVERIFY(fixture.StoredShareIds().isEmpty());
}


void COrganizationLookupTest::testRemoveUnrelatedShares_ReadsTheRelationshipsOncePerCachePeriod()
{
	CShareFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	fixture.Relationships().AddRelationship(tenantId, partnerId);
	fixture.Add(tenantId, NewId());

	QCOMPARE(fixture.Manager().RemoveUnrelatedShares(tenantId), 1);
	QCOMPARE(fixture.Manager().RemoveUnrelatedShares(tenantId), 0);
	QCOMPARE(fixture.Manager().GetShareOwnerTenantIds(tenantId), QByteArrayList({partnerId}));
	QCOMPARE(fixture.Relationships().GetReadCount(), 1);
}


// Removal events

void COrganizationLookupTest::testRemoveConnection_PublishesTheConnectionRemoval()
{
	CConnectionFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	const QByteArray connectionId = fixture.AddConnection(tenantId, partnerId);

	// The owners are told once about the removed entry of the address book
	QVERIFY(fixture.Manager().RemoveConnection(connectionId, tenantId));
	QCOMPARE(fixture.EventManager().GetEventTypes(), QStringList({imtauth::TenantEvent::s_connectionRemoved}));
	QVERIFY(!fixture.Manager().AreConnected(tenantId, partnerId));
}


// Review findings

void COrganizationLookupTest::testFindTenantIdsByName_SemicolonAndBackslashStayOutOfTheSelection()
{
	CTenantFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray semicolonId = fixture.Add(userId, userId);
	fixture.Rename(semicolonId, "Acme;North");
	const QByteArray backslashId = fixture.Add(userId, userId);
	fixture.Rename(backslashId, "A\\B Corp");
	const QByteArray separatorsOnlyId = fixture.Add(userId, userId);
	fixture.Rename(separatorsOnlyId, ";\\;");

	// A selection query gets a semicolon changed and a backslash escapes in LIKE: neither may reach it
	QCOMPARE(fixture.Manager().FindTenantIdsByName("acme;north"), QByteArrayList({semicolonId}));
	QCOMPARE(fixture.Manager().FindTenantIdsByName("a\\b corp"), QByteArrayList({backslashId}));
	QCOMPARE(fixture.Manager().FindTenantIdsByName(";\\;"), QByteArrayList({separatorsOnlyId}));
	for (const QString& filterValue : fixture.Collection().GetRequestedFilterValues()){
		QVERIFY2(!filterValue.contains(';') && !filterValue.contains('\\'), qPrintable(filterValue));
	}
}


void COrganizationLookupTest::testGetShareOwnerTenantIds_FailedRemovalTrustsNoPartner()
{
	CShareFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	fixture.Relationships().AddRelationship(tenantId, partnerId);
	fixture.Add(tenantId, partnerId);
	fixture.Add(NewId(), tenantId);

	// The share of an ended relationship is still stored, so its owner must not be trusted again
	fixture.Collection().SetRemovalFailing(true);
	QVERIFY(fixture.Manager().GetShareOwnerTenantIds(tenantId).isEmpty());
	QVERIFY(fixture.Manager().GetShareOwnerTenantIds(tenantId).isEmpty());
	QCOMPARE(fixture.Relationships().GetReadCount(), 2);

	fixture.Collection().SetRemovalFailing(false);
	QCOMPARE(fixture.Manager().GetShareOwnerTenantIds(tenantId), QByteArrayList({partnerId}));
	QCOMPARE(fixture.Relationships().GetReadCount(), 3);
}


void COrganizationLookupTest::testRemoveMembership_RefusedWhenItCannotBeRead()
{
	CTenantFixture tenants;
	CMembershipFixture memberships;

	icomp::TSimSharedComponentPtr<imtauth::CTenantMembershipManagerComp> manager;
	manager->SetRef("MembershipCollection", memberships.collection);
	manager->SetRef("TenantManager", tenants.manager);
	manager->SetFactory("MembershipFactory", &memberships.membershipFactory);
	manager->InitComponent();

	const QByteArray ownerId = NewId();
	const QByteArray memberId = NewId();
	const QByteArray tenantId = tenants.Add(ownerId, ownerId);
	const QByteArray ownerMembershipId = memberships.Add(ownerId, tenantId);
	const QByteArray membershipId = memberships.Add(memberId, tenantId);

	// Without the member and the organization the removal would notify nobody and could not be repeated
	memberships.Collection().SetLoadFailingIds({membershipId});
	QVERIFY(!manager->RemoveMembership(membershipId));
	memberships.Collection().SetLoadFailingIds({});

	// An organization that cannot be read may be owned by the member
	tenants.Collection().SetLoadFailingIds({tenantId});
	QVERIFY(!manager->RemoveMembership(membershipId));
	QVERIFY(!manager->RemoveMembership(ownerMembershipId));
	tenants.Collection().SetLoadFailingIds({});
	QCOMPARE(Sorted(memberships.Collection().GetElementIds()), Sorted({ownerMembershipId, membershipId}));

	QVERIFY(!manager->RemoveMembership(ownerMembershipId));
	QVERIFY(manager->RemoveMembership(membershipId));
	QCOMPARE(memberships.Collection().GetElementIds(), QByteArrayList({ownerMembershipId}));
}


void COrganizationLookupTest::testVerifyUser_CodeThatCannotBeRemovedStaysBlocked()
{
	CVerificationFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray code = fixture.Generate(userId);
	fixture.Collection().SetRemovalFailing(true);
	for (int i = 0; i < 5; ++i){
		QVERIFY(!fixture.Controller().VerifyUser(userId, WrongCode(code)));
	}

	// The exhausted code is still stored: it must not come back with a fresh budget of attempts
	QVERIFY(fixture.Controller().GetVerificationData(userId) != nullptr);
	QVERIFY(!fixture.Controller().VerifyUser(userId, code));

	// A used code that cannot be removed cannot be used again either
	const QByteArray otherUserId = NewId();
	fixture.Collection().SetRemovalFailing(false);
	const QByteArray otherCode = fixture.Generate(otherUserId);
	fixture.Collection().SetRemovalFailing(true);
	QVERIFY(fixture.Controller().VerifyUser(otherUserId, otherCode));
	QVERIFY(!fixture.Controller().RemoveVerificationData(otherUserId));
	QVERIFY(!fixture.Controller().VerifyUser(otherUserId, otherCode));
}


void COrganizationLookupTest::testGenerateVerificationData_FailsWhileTheOldCodeStays()
{
	CVerificationFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray code = fixture.Generate(userId);
	fixture.Collection().SetRemovalFailing(true);

	imtauth::CUserVerification verification;
	QVERIFY(!fixture.Controller().GenerateVerificationData(userId, &verification));
	QVERIFY(!fixture.Controller().VerifyUser(userId, code));

	fixture.Collection().SetRemovalFailing(false);
	const QByteArray newCode = fixture.Generate(userId);
	QVERIFY(fixture.Controller().VerifyUser(userId, newCode));
}


// Second review round

void COrganizationLookupTest::testChildTenantIds_ReportsAFailedRead()
{
	CTenantFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray parentId = fixture.Add(userId, userId);
	const QByteArray childId = fixture.Add(userId, userId, parentId);

	bool isRead = false;
	QCOMPARE(fixture.Manager().GetChildTenantIds(parentId, &isRead), QByteArrayList({childId}));
	QVERIFY(isRead);

	// A sub-organization that cannot be read must not let the parent go
	fixture.Collection().SetLoadFailingIds({childId});
	QVERIFY(fixture.Manager().GetChildTenantIds(parentId, &isRead).isEmpty());
	QVERIFY(!isRead);

	fixture.Collection().SetLoadFailingIds({});
	fixture.Collection().SetSelectionFailing(true);
	QVERIFY(fixture.Manager().GetChildTenantIds(parentId, &isRead).isEmpty());
	QVERIFY(!isRead);
}


void COrganizationLookupTest::testOwnedOrCreated_ReportsAFailedRead()
{
	CTenantFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray ownedId = fixture.Add(userId, userId);

	bool isRead = false;
	QCOMPARE(fixture.Manager().GetOwnedOrCreatedTenantIds(userId, &isRead), QByteArrayList({ownedId}));
	QVERIFY(isRead);

	// An organization that cannot be read may be owned by the user, who must not be removed then
	fixture.Collection().SetLoadFailingIds({ownedId});
	QVERIFY(fixture.Manager().GetOwnedOrCreatedTenantIds(userId, &isRead).isEmpty());
	QVERIFY(!isRead);

	fixture.Collection().SetLoadFailingIds({});
	fixture.Collection().SetSelectionFailing(true);
	QVERIFY(fixture.Manager().GetOwnedOrCreatedTenantIds(userId, &isRead).isEmpty());
	QVERIFY(!isRead);
}


void COrganizationLookupTest::testRemoveVerificationData_UnreadableCodeIsRemovedAnyway()
{
	CVerificationFixture fixture;

	const QByteArray userId = NewId();
	const QByteArray code = fixture.Generate(userId);
	QVERIFY(fixture.Controller().VerifyUser(userId, code));

	// A failed read is no proof that the used code is gone
	fixture.Collection().SetLoadFailingIds({userId});
	QVERIFY(fixture.Controller().RemoveVerificationData(userId));

	fixture.Collection().SetLoadFailingIds({});
	QVERIFY(fixture.Controller().GetVerificationData(userId) == nullptr);
	QVERIFY(!fixture.Controller().VerifyUser(userId, code));
}


void COrganizationLookupTest::testConsumeVerification_OnlyOneConcurrentCallerSucceeds()
{
	CVerificationFixture fixture;

	for (int round = 0; round < 20; ++round){
		const QByteArray userId = NewId();
		const QByteArray code = fixture.Generate(userId);

		std::atomic<int> successCount{0};
		auto consume = [&fixture, &userId, &code, &successCount](){
			if (fixture.Controller().ConsumeVerification(userId, code)){
				++successCount;
			}
		};
		std::thread first(consume);
		std::thread second(consume);
		first.join();
		second.join();

		QCOMPARE(successCount.load(), 1);
		QVERIFY(!fixture.Controller().VerifyUser(userId, code));
	}
}


// Third review round

void COrganizationLookupTest::testRevokeInvitationsSentBy_ReportsAnUnreadInvitation()
{
	CInvitationFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray inviterId = NewId();
	const QByteArray unreadId = fixture.Add(tenantId, inviterId);
	const QByteArray revokedId = fixture.Add(tenantId, inviterId);

	// The invitation that cannot be read may be a pending one of the member: the membership must stay
	fixture.Collection().SetLoadFailingIds({unreadId});
	QVERIFY(!fixture.Manager().RevokeInvitationsSentBy(tenantId, inviterId, NewId()));

	fixture.Collection().SetLoadFailingIds({});
	QCOMPARE(fixture.Get(revokedId)->GetStatus(), imtauth::ITenantInvitation::TIS_REVOKED);
	QCOMPARE(fixture.Get(unreadId)->GetStatus(), imtauth::ITenantInvitation::TIS_PENDING);

	fixture.Collection().SetSelectionFailing(true);
	QVERIFY(!fixture.Manager().RevokeInvitationsSentBy(tenantId, inviterId, NewId()));

	fixture.Collection().SetSelectionFailing(false);
	QVERIFY(fixture.Manager().RevokeInvitationsSentBy(tenantId, inviterId, NewId()));
	QCOMPARE(fixture.Get(unreadId)->GetStatus(), imtauth::ITenantInvitation::TIS_REVOKED);
}


namespace
{


imtauth::CIdentifiableDocumentShareInfo ShareOf(const QByteArray& entityId, const QByteArray& ownerTenantId, const QByteArray& targetTenantId)
{
	imtauth::CIdentifiableDocumentShareInfo share;
	share.SetCollectionId("Customers");
	share.SetEntityId(entityId);
	share.SetOwnerTenantId(ownerTenantId);
	share.SetTargetTenantId(targetTenantId);
	share.SetAccessLevel(imtauth::IDocumentShareInfo::DAL_VIEW);

	return share;
}


} // anonymous namespace


void COrganizationLookupTest::testShareDocument_RefusesUnreadRelationships()
{
	CShareFixture fixture;

	// The relationship started long before the share, but the part of its history that says so cannot be read:
	// the answered start is too late and would take the live share for one of an earlier relationship
	const QDateTime now = QDateTime::currentDateTimeUtc();
	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	fixture.Relationships().AddRelationship(tenantId, partnerId, now);
	const QByteArray entityId = NewId();
	const QByteArray shareId = fixture.Add(tenantId, partnerId, now.addSecs(-3600), entityId);

	fixture.Relationships().SetPartialRead(true);
	QString errorMessage;
	QVERIFY(fixture.Manager().ShareDocument(ShareOf(entityId, tenantId, partnerId), &errorMessage).isEmpty());
	QVERIFY(!errorMessage.isEmpty());
	QCOMPARE(fixture.StoredShareIds(), QByteArrayList({shareId}));
}


void COrganizationLookupTest::testShareDocument_RefusesAnUnreadShareOfTheDocument()
{
	CShareFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	fixture.Relationships().AddRelationship(tenantId, partnerId, QDateTime::currentDateTimeUtc());
	const QByteArray entityId = NewId();
	const QByteArray shareId = fixture.Add(tenantId, partnerId, QDateTime::currentDateTimeUtc(), entityId);

	// The share that cannot be read may be the existing one: no second share is created beside it
	fixture.Collection().SetLoadFailingIds({shareId});
	QString errorMessage;
	QVERIFY(fixture.Manager().ShareDocument(ShareOf(entityId, tenantId, partnerId), &errorMessage).isEmpty());
	QVERIFY(!errorMessage.isEmpty());

	fixture.Collection().SetLoadFailingIds({});
	QCOMPARE(fixture.StoredShareIds(), QByteArrayList({shareId}));
}


void COrganizationLookupTest::testShareDocument_RefusesWhenTheEarlierShareStays()
{
	CShareFixture fixture;

	const QDateTime now = QDateTime::currentDateTimeUtc();
	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	fixture.Relationships().AddRelationship(tenantId, partnerId, now);
	const QByteArray entityId = NewId();
	const QByteArray staleShareId = fixture.Add(tenantId, partnerId, now.addSecs(-3600), entityId);

	fixture.Collection().SetRemovalFailing(true);
	QString errorMessage;
	QVERIFY(fixture.Manager().ShareDocument(ShareOf(entityId, tenantId, partnerId), &errorMessage).isEmpty());
	QVERIFY(!errorMessage.isEmpty());
	QCOMPARE(fixture.StoredShareIds(), QByteArrayList({staleShareId}));
}


void COrganizationLookupTest::testGetShareOwnerTenantIds_UnreadShareTrustsNoPartner()
{
	CShareFixture fixture;

	const QDateTime now = QDateTime::currentDateTimeUtc();
	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	fixture.Relationships().AddRelationship(tenantId, partnerId, now);
	fixture.Add(partnerId, tenantId, now.addSecs(5));
	const QByteArray staleShareId = fixture.Add(partnerId, tenantId, now.addSecs(-3600));

	// The share of the earlier relationship cannot be read, so the cleanup cannot know it is gone
	fixture.Collection().SetLoadFailingIds({staleShareId});
	QVERIFY(fixture.Manager().GetShareOwnerTenantIds(tenantId).isEmpty());

	fixture.Collection().SetLoadFailingIds({});
	QCOMPARE(fixture.Manager().GetShareOwnerTenantIds(tenantId), QByteArrayList({partnerId}));
	QVERIFY(!fixture.StoredShareIds().contains(staleShareId));
}


I_ADD_TEST(COrganizationLookupTest);


