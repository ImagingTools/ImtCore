// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "COrganizationLookupTest.h"


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>

// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TSimComponentWrap.h>
#include <icomp/TSimComponentsFactory.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CCrossOrgGrantComp.h>
#include <imtauth/CCrossOrgGrantManagerComp.h>
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
#include <imtauth/CTenantRelationshipInfo.h>
#include <imtauth/CTenantRelationshipInfoComp.h>
#include <imtauth/CTenantRelationshipProposal.h>
#include <imtauth/CTenantRelationshipProposalComp.h>
#include <imtauth/TenantEvents.h>
#include <imtauth/CUserVerification.h>
#include <imtauth/CUserVerificationControllerComp.h>

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
		for (CTestCollectionComp* collectionPtr : {&requests.GetImpl(), &codes.GetImpl(), &connections.GetImpl(), &proposals.GetImpl(), &relationships.GetImpl()}){
			collectionPtr->SetUuidFieldIds({"TenantAId", "TenantBId", "SourceTenantId", "TargetTenantId", "ConnectionId", "Id"});
		}
		requests->InitComponent();
		codes->InitComponent();
		connections->InitComponent();
		proposals->InitComponent();
		relationships->InitComponent();

		manager->SetRef("RequestCollection", requests);
		manager->SetRef("ConnectionCodeCollection", codes);
		manager->SetRef("ConnectionCollection", connections);
		manager->SetRef("ProposalCollection", proposals);
		manager->SetRef("RelationshipCollection", relationships);
		manager->SetFactory("ConnectionFactory", &connectionFactory);
		manager->SetFactory("RelationshipFactory", &relationshipFactory);
		manager->SetFactory("ProposalFactory", &proposalFactory);
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

	QByteArray AddRelationship(
				const QByteArray& connectionId,
				const QByteArray& sourceTenantId,
				const QByteArray& targetTenantId,
				imtauth::ITenantRelationshipInfo::TenantRelationshipStatus status = imtauth::ITenantRelationshipInfo::TRS_ACTIVE,
				const QDateTime& createdAt = QDateTime(),
				const QDateTime& updatedAt = QDateTime())
	{
		const QByteArray relationshipId = NewId();

		imtauth::CIdentifiableTenantRelationshipInfo relationship;
		relationship.SetRelationshipId(relationshipId);
		relationship.SetConnectionId(connectionId);
		relationship.SetSourceTenantId(sourceTenantId);
		relationship.SetTargetTenantId(targetTenantId);
		relationship.SetStatus(status);
		relationship.SetCreatedAt(createdAt.toString(Qt::ISODateWithMs));
		relationship.SetUpdatedAt(updatedAt.toString(Qt::ISODateWithMs));

		return relationships->AddObject("TenantRelationship", relationship, relationshipId);
	}

	CTestCollectionComp& Connections()
	{
		return connections.GetImpl();
	}

	CTestCollectionComp& Relationships()
	{
		return relationships.GetImpl();
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
	icomp::TSimSharedComponentPtr<CTestCollectionComp> proposals;
	icomp::TSimSharedComponentPtr<CTestCollectionComp> relationships;
	icomp::TSimComponentsFactory<imtauth::CTenantConnectionComp> connectionFactory;
	icomp::TSimComponentsFactory<imtauth::CTenantRelationshipInfoComp> relationshipFactory;
	icomp::TSimComponentsFactory<imtauth::CTenantRelationshipProposalComp> proposalFactory;
	icomp::TSimSharedComponentPtr<CTestConnectionManagerComp> manager;
};


class CGrantFixture
{
public:
	CGrantFixture()
	{
		collection->SetUuidFieldIds({"SourceTenantId", "TargetTenantId"});
		collection->InitComponent();

		manager->SetRef("GrantCollection", collection);
		manager->SetFactory("GrantFactory", &grantFactory);
		manager->InitComponent();
	}

	CTestCollectionComp& Collection()
	{
		return collection.GetImpl();
	}

	imtauth::ICrossOrgGrant& Manager()
	{
		return manager.GetImpl();
	}

	icomp::TSimSharedComponentPtr<CTestCollectionComp> collection;
	icomp::TSimComponentsFactory<imtauth::CCrossOrgGrantComp> grantFactory;
	icomp::TSimSharedComponentPtr<imtauth::CCrossOrgGrantManagerComp> manager;
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

	QCOMPARE(fixture.Manager().RevokeInvitationsSentBy(tenantId, inviterId, adminId), 2);

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

	QCOMPARE(fixture.Manager().RevokeInvitationsSentBy(tenantId, inviterId, adminId), 0);
	QCOMPARE(fixture.Manager().RevokeInvitationsSentBy(tenantId, QByteArray(), adminId), 0);
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


void COrganizationLookupTest::testGetTenantRelationshipIds_LoadsOnlyTheTenants()
{
	CConnectionFixture fixture;

	const QByteArray tenantId = NewId();
	for (int i = 0; i < 20; ++i){
		fixture.AddRelationship(NewId(), NewId(), NewId());
	}
	const QByteArray asSourceId = fixture.AddRelationship(NewId(), tenantId, NewId());
	const QByteArray asTargetId = fixture.AddRelationship(NewId(), NewId(), tenantId);
	fixture.AddRelationship(NewId(), tenantId, NewId(), imtauth::ITenantRelationshipInfo::TRS_ARCHIVED);

	fixture.Relationships().ResetLoadCount();
	QCOMPARE(Sorted(fixture.Manager().GetTenantRelationshipIds(tenantId)), Sorted({asSourceId, asTargetId}));
	QCOMPARE(fixture.Relationships().GetLoadCount(), 3);
	QCOMPARE(fixture.Relationships().GetUnfilteredSelectionCount(), 0);
	QVERIFY(fixture.Relationships().GetUnknownFieldIds().isEmpty());
}


void COrganizationLookupTest::testRemoveConnection_ArchivesOnlyItsRelationships()
{
	CConnectionFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	const QByteArray otherPartnerId = NewId();
	const QByteArray connectionId = fixture.AddConnection(tenantId, partnerId);
	const QByteArray otherConnectionId = fixture.AddConnection(tenantId, otherPartnerId);
	const QByteArray firstId = fixture.AddRelationship(connectionId, tenantId, partnerId);
	const QByteArray secondId = fixture.AddRelationship(connectionId, partnerId, tenantId);
	const QByteArray otherId = fixture.AddRelationship(otherConnectionId, tenantId, otherPartnerId);

	QVERIFY(!fixture.Manager().RemoveConnection(connectionId, NewId()));

	fixture.Relationships().ResetLoadCount();
	QVERIFY(fixture.Manager().RemoveConnection(connectionId, tenantId));
	QCOMPARE(fixture.Relationships().GetUnfilteredSelectionCount(), 0);

	QCOMPARE(fixture.Manager().GetConnectionIds(tenantId), QByteArrayList({otherConnectionId}));
	QCOMPARE(fixture.Manager().GetTenantRelationshipIds(tenantId), QByteArrayList({otherId}));
	QCOMPARE(fixture.Manager().GetTenantRelationship(firstId)->GetStatus(), imtauth::ITenantRelationshipInfo::TRS_ARCHIVED);
	QCOMPARE(fixture.Manager().GetTenantRelationship(secondId)->GetStatus(), imtauth::ITenantRelationshipInfo::TRS_ARCHIVED);
	QVERIFY(!fixture.Manager().RemoveConnection(connectionId, tenantId));
}


void COrganizationLookupTest::testConnectionLookups_NonUuidIds_MatchNothing()
{
	CConnectionFixture fixture;
	fixture.AddConnection("not-a-tenant", NewId());
	fixture.AddRelationship(NewId(), "not-a-tenant", NewId());

	fixture.Connections().ResetLoadCount();
	fixture.Relationships().ResetLoadCount();
	QVERIFY(fixture.Manager().GetConnectionIds("not-a-tenant").isEmpty());
	QVERIFY(fixture.Manager().GetTenantRelationshipIds("not-a-tenant").isEmpty());
	QVERIFY(fixture.Manager().GetConnectionIds(QByteArray()).isEmpty());
	QCOMPARE(fixture.Connections().GetLoadCount(), 0);
	QCOMPARE(fixture.Relationships().GetLoadCount(), 0);
}


// Cross-organization grants

void COrganizationLookupTest::testGetGrantsBySourceTenant_LoadsOnlyTheTenants()
{
	CGrantFixture fixture;

	const QByteArray tenantId = NewId();
	for (int i = 0; i < 20; ++i){
		QVERIFY(!fixture.Manager().CreateGrant(NewId(), NewId(), {"Viewer"}).isEmpty());
	}
	const QByteArray grantId = fixture.Manager().CreateGrant(tenantId, NewId(), {"Viewer"});
	QVERIFY(!fixture.Manager().CreateGrant(NewId(), tenantId, {"Viewer"}).isEmpty());

	fixture.Collection().ResetLoadCount();
	const imtauth::CrossOrgGrants grants = fixture.Manager().GetGrantsBySourceTenant(tenantId);
	QCOMPARE(grants.size(), 1);
	QCOMPARE(grants[0].grantId, grantId);
	QCOMPARE(fixture.Collection().GetLoadCount(), 1);
	QCOMPARE(fixture.Collection().GetUnfilteredSelectionCount(), 0);
	QVERIFY(fixture.Collection().GetUnknownFieldIds().isEmpty());

	fixture.Collection().ResetLoadCount();
	QVERIFY(fixture.Manager().GetGrantsBySourceTenant("not-a-tenant").isEmpty());
	QCOMPARE(fixture.Collection().GetLoadCount(), 0);
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


void COrganizationLookupTest::testShareDocument_RefusesARelationshipOutsideItsValidity()
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
	QCOMPARE(errorMessage, QStringLiteral("The relationship of the organizations is not valid at this time"));
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


void COrganizationLookupTest::testGetTenantRelationship_UnknownIdIsNoRelationship()
{
	CConnectionFixture fixture;
	fixture.AddRelationship(NewId(), NewId(), NewId());

	QVERIFY(!fixture.Manager().GetTenantRelationship(NewId()).IsValid());
	QVERIFY(!fixture.Manager().RemoveTenantRelationship(NewId(), NewId()));
	QVERIFY(fixture.EventManager().m_events.isEmpty());
}


// The period of relationships of two organizations

void COrganizationLookupTest::testGetUninterruptedStart_FollowsTheRelationshipsThatOverlap()
{
	const QDateTime t0 = QDateTime::currentDateTimeUtc().addDays(-100);

	// R1 from day 0 to day 20, R2 from day 10 to day 30, R3 from day 30 on
	const QList<imtauth::RelationshipPeriod> periods = {
		{t0, t0.addDays(20)},
		{t0.addDays(10), t0.addDays(30)},
		{t0.addDays(30), QDateTime()}};
	QCOMPARE(imtauth::GetUninterruptedStart(periods), t0);

	// The order of the periods does not matter
	QCOMPARE(imtauth::GetUninterruptedStart({periods[2], periods[1], periods[0]}), t0);

	// Of two active relationships the earlier one starts the period
	QCOMPARE(imtauth::GetUninterruptedStart({{t0.addDays(5), QDateTime()}, {t0.addDays(1), QDateTime()}}), t0.addDays(1));
}


void COrganizationLookupTest::testGetUninterruptedStart_StopsAtAGap()
{
	const QDateTime t0 = QDateTime::currentDateTimeUtc().addDays(-100);

	// R1 ended on day 20, R2 started on day 21
	QCOMPARE(imtauth::GetUninterruptedStart({{t0, t0.addDays(20)}, {t0.addDays(21), QDateTime()}}), t0.addDays(21));

	// An earlier relationship without a known end does not bridge the gap
	QCOMPARE(imtauth::GetUninterruptedStart({{t0, t0.addDays(20)}, {QDateTime(), t0.addDays(40)}, {t0.addDays(30), QDateTime()}}), t0.addDays(30));
}


void COrganizationLookupTest::testGetUninterruptedStart_UnknownOrNoActiveRelationship()
{
	const QDateTime t0 = QDateTime::currentDateTimeUtc().addDays(-100);

	QVERIFY(!imtauth::GetUninterruptedStart({}).isValid());
	QVERIFY(!imtauth::GetUninterruptedStart({{t0, t0.addDays(20)}}).isValid());
	QVERIFY(!imtauth::GetUninterruptedStart({{QDateTime(), QDateTime()}, {t0, QDateTime()}}).isValid());
}


void COrganizationLookupTest::testGetRelatedSince_KeepsTheStartWhenARelationshipIsReplaced()
{
	CConnectionFixture fixture;

	const QDateTime t0 = QDateTime::currentDateTimeUtc().addDays(-100).toUTC();
	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	const QByteArray connectionId = fixture.AddConnection(tenantId, partnerId);

	// R1 is archived only after R2 started: the organizations were related the whole time
	fixture.AddRelationship(connectionId, tenantId, partnerId, imtauth::ITenantRelationshipInfo::TRS_ARCHIVED, t0, t0.addDays(20));
	fixture.AddRelationship(connectionId, partnerId, tenantId, imtauth::ITenantRelationshipInfo::TRS_ACTIVE, t0.addDays(10));
	fixture.AddRelationship(connectionId, tenantId, NewId(), imtauth::ITenantRelationshipInfo::TRS_ARCHIVED, t0.addDays(-50), t0.addDays(30));

	fixture.Relationships().ResetLoadCount();
	QCOMPARE(fixture.Manager().GetRelatedSince(tenantId, partnerId), t0);
	QCOMPARE(fixture.Manager().GetRelatedSince(partnerId, tenantId), t0);
	QCOMPARE(fixture.Relationships().GetLoadCount(), 4);
	QCOMPARE(fixture.Relationships().GetUnfilteredSelectionCount(), 0);
}


void COrganizationLookupTest::testGetRelatedSince_StartsAnewAfterAGap()
{
	CConnectionFixture fixture;

	const QDateTime t0 = QDateTime::currentDateTimeUtc().addDays(-100).toUTC();
	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	const QByteArray connectionId = fixture.AddConnection(tenantId, partnerId);

	fixture.AddRelationship(connectionId, tenantId, partnerId, imtauth::ITenantRelationshipInfo::TRS_ARCHIVED, t0, t0.addDays(20));
	QVERIFY(!fixture.Manager().GetRelatedSince(tenantId, partnerId).isValid());

	fixture.AddRelationship(connectionId, tenantId, partnerId, imtauth::ITenantRelationshipInfo::TRS_ACTIVE, t0.addDays(21));
	QCOMPARE(fixture.Manager().GetRelatedSince(tenantId, partnerId), t0.addDays(21));
	QVERIFY(!fixture.Manager().GetRelatedSince(tenantId, "not-a-tenant").isValid());
}


// Removal events

void COrganizationLookupTest::testRemoveTenantRelationship_PublishesTheRemoval()
{
	CConnectionFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	const QByteArray relationshipId = fixture.AddRelationship(fixture.AddConnection(tenantId, partnerId), tenantId, partnerId);

	QVERIFY(!fixture.Manager().RemoveTenantRelationship(NewId(), relationshipId));
	QVERIFY(fixture.EventManager().m_events.isEmpty());

	QVERIFY(fixture.Manager().RemoveTenantRelationship(partnerId, relationshipId));
	QCOMPARE(fixture.EventManager().GetEventTypes(), QStringList({imtauth::TenantEvent::s_relationshipRemoved}));
	QCOMPARE(fixture.EventManager().m_events.first().value(imtauth::TenantEvent::s_relationshipId).toByteArray(), relationshipId);
	QCOMPARE(fixture.Manager().GetTenantRelationship(relationshipId)->GetStatus(), imtauth::ITenantRelationshipInfo::TRS_ARCHIVED);
}


void COrganizationLookupTest::testDeleteProposal_PublishesTheRemoval()
{
	CConnectionFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	const QByteArray connectionId = fixture.AddConnection(tenantId, partnerId);
	const QByteArray relationshipId = fixture.AddRelationship(connectionId, tenantId, partnerId);

	imtauth::CTenantRelationshipProposal proposal;
	proposal.SetConnectionId(connectionId);
	proposal.SetInitiatorTenantId(tenantId);
	proposal.SetCounterpartyTenantId(partnerId);
	proposal.SetProposalType(imtauth::ITenantRelationshipProposalInfo::RPT_DELETE);
	proposal.SetExistingRelationshipId(relationshipId);

	const QByteArray proposalId = fixture.Manager().CreateRelationshipProposal(proposal);
	QVERIFY(!proposalId.isEmpty());
	fixture.EventManager().m_events.clear();

	QCOMPARE(fixture.Manager().ApproveRelationshipProposal(proposalId, partnerId), relationshipId);
	QVERIFY(fixture.EventManager().GetEventTypes().contains(imtauth::TenantEvent::s_relationshipRemoved));
	QCOMPARE(fixture.Manager().GetTenantRelationship(relationshipId)->GetStatus(), imtauth::ITenantRelationshipInfo::TRS_ARCHIVED);
}


void COrganizationLookupTest::testRemoveConnection_PublishesOnlyTheConnectionRemoval()
{
	CConnectionFixture fixture;

	const QByteArray tenantId = NewId();
	const QByteArray partnerId = NewId();
	const QByteArray connectionId = fixture.AddConnection(tenantId, partnerId);
	fixture.AddRelationship(connectionId, tenantId, partnerId);
	fixture.AddRelationship(connectionId, partnerId, tenantId);

	// The owners are told once about the connection, not again about each of its relationships
	QVERIFY(fixture.Manager().RemoveConnection(connectionId, tenantId));
	QCOMPARE(fixture.EventManager().GetEventTypes(), QStringList({imtauth::TenantEvent::s_connectionRemoved}));
	QVERIFY(fixture.Manager().GetTenantRelationshipIds(tenantId).isEmpty());
}


I_ADD_TEST(COrganizationLookupTest);


