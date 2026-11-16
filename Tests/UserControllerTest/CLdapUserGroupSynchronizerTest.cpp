// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CLdapUserGroupSynchronizerTest.h"


// Qt includes
#include <QtCore/QSet>
#include <QtCore/QUuid>

// ACF includes
#include <ilog/CMessage.h>
#include <icomp/TSimComponentWrap.h>
#include <icomp/TSimComponentsFactory.h>
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>
#include <imtauth/CLdapUserGroupSynchronizerComp.h>
#include <imtauth/CUserGroupInfo.h>
#include <imtcore/Version.h>


namespace
{


const quint32 s_groupSystemInfosVersion = 23950;


class CFixedVersionInfo: virtual public iser::IVersionInfo
{
public:
	explicit CFixedVersionInfo(quint32 imtCoreVersion): m_imtCoreVersion(imtCoreVersion)
	{
	}

	// reimplemented (iser::IVersionInfo)
	virtual VersionIds GetVersionIds() const override
	{
		VersionIds retVal;
		retVal.insert(imtcore::VI_IMTCORE);

		return retVal;
	}

	virtual bool GetVersionNumber(int versionId, quint32& result) const override
	{
		if (versionId == imtcore::VI_IMTCORE){
			result = m_imtCoreVersion;

			return true;
		}

		return false;
	}

	virtual QString GetVersionIdDescription(int /*versionId*/) const override
	{
		return "ImtCore";
	}

	virtual QString GetEncodedVersionName(int /*versionId*/, quint32 versionNumber) const override
	{
		return QString::number(versionNumber);
	}

private:
	quint32 m_imtCoreVersion;
};


class CTestGroupCollectionComp:
			public icomp::CComponentBase,
			public imtbase::CObjectCollectionBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestGroupCollectionComp);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionInfo);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
	I_END_COMPONENT;

	QSet<QByteArray> m_loadFailingIds;
	QSet<QByteArray> m_writeFailingIds;
	bool m_isListReadFailing = false;

	// reimplemented (imtbase::IObjectCollection)
	virtual bool SetObjectData(
				const Id& objectId,
				const istd::IChangeable& object,
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override
	{
		if (m_writeFailingIds.contains(objectId)){
			return false;
		}

		return imtbase::CObjectCollectionBase::SetObjectData(objectId, object, mode, operationContextPtr);
	}

	virtual bool GetObjectData(
				const Id& objectId,
				DataPtr& dataPtr,
				const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override
	{
		if (m_loadFailingIds.contains(objectId)){
			return false;
		}

		return imtbase::CObjectCollectionBase::GetObjectData(objectId, dataPtr, dataConfigurationPtr);
	}

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override
	{
		if (m_isListReadFailing && logPtr != nullptr){
			logPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, "Read failed", "Test")));
		}

		return imtbase::CObjectCollectionBase::GetElementIds(offset, count, selectionParamsPtr, logPtr);
	}

protected:
	// reimplemented (imtbase::CObjectCollectionBase)
	virtual istd::IChangeableUniquePtr CreateObjectInstance(const QByteArray& /*typeId*/) const override
	{
		return new imtauth::CIdentifiableUserGroupInfo;
	}
};


imtauth::IUserGroupInfo::SystemInfo LdapSystemInfo()
{
	imtauth::IUserGroupInfo::SystemInfo retVal;
	retVal.systemId = "Ldap";
	retVal.systemName = "LDAP";

	return retVal;
}


imtauth::CLdapUserGroupSynchronizerComp::LdapGroup MakeLdapGroup(
			const QByteArray& sid,
			const QString& name,
			const QByteArrayList& parentSids = QByteArrayList(),
			bool isMatching = true)
{
	imtauth::CLdapUserGroupSynchronizerComp::LdapGroup retVal;
	retVal.sid = sid;
	retVal.name = name;
	retVal.description = name + QStringLiteral(" description");
	retVal.parentSids = parentSids;
	retVal.isMatching = isMatching;

	return retVal;
}


class CFixture
{
public:
	explicit CFixture(const QByteArray& ldapSystemId = "Ldap")
	{
		groups->InitComponent();

		synchronizer->SetRef("UserGroupCollection", groups);
		synchronizer->SetIdAttr("LdapSystemId", ldapSystemId);
		// No background synchronization against a real directory.
		synchronizer->SetIntAttr("CheckInterval", 0);
		synchronizer->InitComponent();
	}

	const imtauth::IUserGroupInfo* GetGroup(const QByteArray& groupId)
	{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!groups->GetObjectData(groupId, dataPtr)){
			return nullptr;
		}

		// The loaded groups are kept alive until the end of the test.
		loadedGroups << dataPtr;

		return dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
	}

	QByteArray AddInternalGroup(const QString& name, const QByteArray& proposedGroupId = QByteArray())
	{
		const QByteArray groupId = proposedGroupId.isEmpty() ? QUuid::createUuid().toByteArray(QUuid::WithoutBraces) : proposedGroupId;

		imtauth::CIdentifiableUserGroupInfo group;
		group.SetObjectUuid(groupId);
		group.SetId(groupId);
		group.SetName(name);
		group.AddToSystem(imtauth::IUserGroupInfo::SystemInfo());
		groups->InsertNewObject("Group", name, QString(), &group, groupId);

		return groupId;
	}

	QList<imtbase::IObjectCollection::DataPtr> loadedGroups;
	icomp::TSimSharedComponentPtr<CTestGroupCollectionComp> groups;
	icomp::TSimSharedComponentPtr<imtauth::CLdapUserGroupSynchronizerComp> synchronizer;
};


} // namespace


// System flag of groups

void CLdapUserGroupSynchronizerTest::testSystemInfos_SerializationRoundTrip()
{
	CFixedVersionInfo versionInfo(s_groupSystemInfosVersion);

	imtauth::CUserGroupInfo source;
	source.SetName("Group");
	source.AddToSystem(LdapSystemInfo());

	iser::CMemoryWriteArchive writeArchive(&versionInfo);
	QVERIFY(source.Serialize(writeArchive));

	iser::CMemoryReadArchive readArchive(writeArchive);
	imtauth::CUserGroupInfo restored;
	QVERIFY(restored.Serialize(readArchive));

	QCOMPARE(restored.GetSystemInfos().size(), 1);
	QVERIFY(restored.GetSystemInfos().first() == LdapSystemInfo());
}


void CLdapUserGroupSynchronizerTest::testSystemInfos_LegacyArchiveIsInternal()
{
	// Groups stored before the system flag existed are internal groups.
	CFixedVersionInfo versionInfo(s_groupSystemInfosVersion - 1);

	imtauth::CUserGroupInfo source;
	source.SetName("Group");
	source.AddToSystem(LdapSystemInfo());

	iser::CMemoryWriteArchive writeArchive(&versionInfo);
	QVERIFY(source.Serialize(writeArchive));

	iser::CMemoryReadArchive readArchive(writeArchive);
	imtauth::CUserGroupInfo restored;
	restored.AddToSystem(LdapSystemInfo());
	QVERIFY(restored.Serialize(readArchive));

	QCOMPARE(restored.GetName(), QString("Group"));
	QCOMPARE(restored.GetSystemInfos().size(), 1);
	QVERIFY(restored.GetSystemInfos().first().systemId.isEmpty());
}


void CLdapUserGroupSynchronizerTest::testSystemInfos_CopyAndReset()
{
	imtauth::CUserGroupInfo source;
	QVERIFY(source.AddToSystem(LdapSystemInfo()));
	QVERIFY(!source.AddToSystem(LdapSystemInfo()));

	istd::IChangeableUniquePtr clonePtr = source.CloneMe();
	imtauth::IUserGroupInfo* clonedGroupPtr = dynamic_cast<imtauth::IUserGroupInfo*>(clonePtr.GetPtr());
	QVERIFY(clonedGroupPtr != nullptr);
	QCOMPARE(clonedGroupPtr->GetSystemInfos().size(), 1);

	QVERIFY(clonedGroupPtr->RemoveFromSystem("Ldap"));
	QVERIFY(!clonedGroupPtr->RemoveFromSystem("Ldap"));
	QVERIFY(clonedGroupPtr->GetSystemInfos().isEmpty());

	QVERIFY(source.ResetData());
	QVERIFY(source.GetSystemInfos().isEmpty());
}


// Synchronization

void CLdapUserGroupSynchronizerTest::testSynchronize_CreatesHierarchy()
{
	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Child", {"S-1-5-21-1"});
	// The parent outside of the synchronized groups is ignored.
	ldapGroups << MakeLdapGroup("S-1-5-21-3", "Grandchild", {"S-1-5-21-2", "S-1-5-21-99"});

	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	const QByteArray rootId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1");
	const QByteArray childId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-2");
	const QByteArray grandchildId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-3");

	QCOMPARE(fixture.groups->GetElementIds().size(), 3);

	const imtauth::IUserGroupInfo* rootPtr = fixture.GetGroup(rootId);
	QVERIFY(rootPtr != nullptr);
	QCOMPARE(rootPtr->GetName(), QString("Root"));
	QCOMPARE(rootPtr->GetDescription(), QString("Root description"));
	QVERIFY(rootPtr->GetParentGroups().isEmpty());
	QCOMPARE(rootPtr->GetSystemInfos().size(), 1);
	QVERIFY(rootPtr->GetSystemInfos().first() == LdapSystemInfo());

	const imtauth::IUserGroupInfo* childPtr = fixture.GetGroup(childId);
	QVERIFY(childPtr != nullptr);
	QCOMPARE(childPtr->GetParentGroups(), QByteArrayList({rootId}));

	const imtauth::IUserGroupInfo* grandchildPtr = fixture.GetGroup(grandchildId);
	QVERIFY(grandchildPtr != nullptr);
	QCOMPARE(grandchildPtr->GetParentGroups(), QByteArrayList({childId}));

	// A second run without directory changes changes nothing.
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));
	QCOMPARE(fixture.groups->GetElementIds().size(), 3);
}


void CLdapUserGroupSynchronizerTest::testSynchronize_UpdateKeepsInternalAssignments()
{
	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Child", {"S-1-5-21-1"});
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	const QByteArray rootId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1");
	const QByteArray childId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-2");

	// An administrator assigns a role and an internal parent group to the mirrored group.
	const QByteArray internalGroupId = fixture.AddInternalGroup("Internal");
	const imtauth::IUserGroupInfo* storedChildPtr = fixture.GetGroup(childId);
	QVERIFY(storedChildPtr != nullptr);
	istd::IChangeableUniquePtr editedChildPtr = storedChildPtr->CloneMe();
	imtauth::IUserGroupInfo* editedChildInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(editedChildPtr.GetPtr());
	QVERIFY(editedChildInfoPtr != nullptr);
	editedChildInfoPtr->AddRole("Product", "Role");
	QVERIFY(editedChildInfoPtr->AddParentGroup(internalGroupId));
	QVERIFY(fixture.groups->SetObjectData(childId, *editedChildInfoPtr));

	// In the directory the group is renamed and no longer nested.
	ldapGroups.clear();
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Renamed");
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	const imtauth::IUserGroupInfo* childPtr = fixture.GetGroup(childId);
	QVERIFY(childPtr != nullptr);
	QCOMPARE(childPtr->GetName(), QString("Renamed"));
	QCOMPARE(childPtr->GetParentGroups(), QByteArrayList({internalGroupId}));
	QCOMPARE(childPtr->GetRoles("Product"), QByteArrayList({"Role"}));
	QVERIFY(fixture.GetGroup(rootId) != nullptr);
}


void CLdapUserGroupSynchronizerTest::testSynchronize_RemovesOnlyVanishedLdapGroups()
{
	CFixture fixture;

	const QByteArray internalGroupId = fixture.AddInternalGroup("Internal");

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Child", {"S-1-5-21-1"});
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));
	QCOMPARE(fixture.groups->GetElementIds().size(), 3);

	// The parent group was removed from the directory.
	ldapGroups.clear();
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Child", {"S-1-5-21-1"});
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	const QByteArray rootId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1");
	const QByteArray childId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-2");

	QVERIFY(fixture.GetGroup(rootId) == nullptr);
	QVERIFY(fixture.GetGroup(internalGroupId) != nullptr);

	const imtauth::IUserGroupInfo* childPtr = fixture.GetGroup(childId);
	QVERIFY(childPtr != nullptr);
	QVERIFY(childPtr->GetParentGroups().isEmpty());

	// All groups gone from the directory: only the internal group is left.
	QVERIFY(fixture.synchronizer->SynchronizeGroups(imtauth::CLdapUserGroupSynchronizerComp::LdapGroups()));
	QCOMPARE(fixture.groups->GetElementIds(), imtbase::ICollectionInfo::Ids({internalGroupId}));
}


void CLdapUserGroupSynchronizerTest::testSynchronize_FailedReadChangesNothing()
{
	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Child", {"S-1-5-21-1"});
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	const QByteArray rootId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1");
	const QByteArray childId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-2");

	// A group that cannot be read must not be taken as missing.
	fixture.groups->m_loadFailingIds.insert(rootId);
	QVERIFY(!fixture.synchronizer->SynchronizeGroups(imtauth::CLdapUserGroupSynchronizerComp::LdapGroups()));
	fixture.groups->m_loadFailingIds.clear();

	QCOMPARE(fixture.groups->GetElementIds().size(), 2);

	const imtauth::IUserGroupInfo* childPtr = fixture.GetGroup(childId);
	QVERIFY(childPtr != nullptr);
	QCOMPARE(childPtr->GetParentGroups(), QByteArrayList({rootId}));
}


void CLdapUserGroupSynchronizerTest::testSynchronize_FailedListReadChangesNothing()
{
	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	// A list read reporting an error must not be taken as a complete list.
	fixture.groups->m_isListReadFailing = true;
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Child", {"S-1-5-21-1"});
	QVERIFY(!fixture.synchronizer->SynchronizeGroups(ldapGroups));
	fixture.groups->m_isListReadFailing = false;

	QCOMPARE(fixture.groups->GetElementIds().size(), 1);
}


void CLdapUserGroupSynchronizerTest::testSynchronize_FailedWriteKeepsVanishedGroups()
{
	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Child", {"S-1-5-21-1"});
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	const QByteArray rootId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1");
	const QByteArray childId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-2");

	// The child cannot be detached from the removed parent: the parent must not be removed.
	fixture.groups->m_writeFailingIds.insert(childId);
	ldapGroups.clear();
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Child");
	QVERIFY(!fixture.synchronizer->SynchronizeGroups(ldapGroups));
	fixture.groups->m_writeFailingIds.clear();

	QVERIFY(fixture.GetGroup(rootId) != nullptr);

	const imtauth::IUserGroupInfo* childPtr = fixture.GetGroup(childId);
	QVERIFY(childPtr != nullptr);
	QCOMPARE(childPtr->GetParentGroups(), QByteArrayList({rootId}));

	// The next run completes the synchronization.
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));
	QVERIFY(fixture.GetGroup(rootId) == nullptr);
}


void CLdapUserGroupSynchronizerTest::testSynchronize_RejectsInvalidSnapshotAndConfiguration()
{
	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");

	// An empty system ID would create internal groups.
	CFixture internalSystemFixture("");
	QVERIFY(!internalSystemFixture.synchronizer->SynchronizeGroups(ldapGroups));
	QVERIFY(internalSystemFixture.groups->GetElementIds().isEmpty());

	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups duplicatedGroups = ldapGroups;
	duplicatedGroups << MakeLdapGroup("S-1-5-21-1", "Duplicate");
	QVERIFY(!fixture.synchronizer->SynchronizeGroups(duplicatedGroups));

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups incompleteGroups = ldapGroups;
	incompleteGroups << MakeLdapGroup("S-1-5-21-2", QString());
	QVERIFY(!fixture.synchronizer->SynchronizeGroups(incompleteGroups));

	incompleteGroups = ldapGroups;
	incompleteGroups << MakeLdapGroup(QByteArray(), "Unknown");
	QVERIFY(!fixture.synchronizer->SynchronizeGroups(incompleteGroups));

	QVERIFY(fixture.groups->GetElementIds().isEmpty());
}


void CLdapUserGroupSynchronizerTest::testSynchronize_RejectsMirrorWithoutSystemFlag()
{
	CFixture fixture;

	// The mirror was stored without its system flag (e.g. by a repository without version information).
	const QByteArray rootId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1");
	fixture.AddInternalGroup("Root", rootId);

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Child", {"S-1-5-21-1"});
	QVERIFY(!fixture.synchronizer->SynchronizeGroups(ldapGroups));

	QCOMPARE(fixture.groups->GetElementIds(), imtbase::ICollectionInfo::Ids({rootId}));

	const imtauth::IUserGroupInfo* rootPtr = fixture.GetGroup(rootId);
	QVERIFY(rootPtr != nullptr);
	QCOMPARE(rootPtr->GetSystemInfos().size(), 1);
	QVERIFY(rootPtr->GetSystemInfos().first().systemId.isEmpty());
}


void CLdapUserGroupSynchronizerTest::testSynchronize_KeepsMirrorsOutsideOfFilter()
{
	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "App Root");
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "App Child", {"S-1-5-21-1"});
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	const QByteArray rootId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1");
	const QByteArray childId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-2");
	const QByteArray otherId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-3");

	// The parent group was renamed out of the filter, a new group outside of the filter appears.
	ldapGroups.clear();
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root", {}, false);
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "App Child", {"S-1-5-21-1", "S-1-5-21-3"});
	ldapGroups << MakeLdapGroup("S-1-5-21-3", "Other", {}, false);
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	QCOMPARE(fixture.groups->GetElementIds().size(), 2);
	QVERIFY(fixture.GetGroup(otherId) == nullptr);

	const imtauth::IUserGroupInfo* rootPtr = fixture.GetGroup(rootId);
	QVERIFY(rootPtr != nullptr);
	QCOMPARE(rootPtr->GetName(), QString("Root"));

	const imtauth::IUserGroupInfo* childPtr = fixture.GetGroup(childId);
	QVERIFY(childPtr != nullptr);
	QCOMPARE(childPtr->GetParentGroups(), QByteArrayList({rootId}));

	// Only the removal from the directory removes the mirror.
	ldapGroups.removeFirst();
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));
	QVERIFY(fixture.GetGroup(rootId) == nullptr);
	QCOMPARE(fixture.groups->GetElementIds(), imtbase::ICollectionInfo::Ids({childId}));
}


void CLdapUserGroupSynchronizerTest::testSynchronize_OmitsNestingCycles()
{
	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "First", {"S-1-5-21-3"});
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Second", {"S-1-5-21-1"});
	ldapGroups << MakeLdapGroup("S-1-5-21-3", "Third", {"S-1-5-21-2"});
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	const QByteArray firstId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1");
	const QByteArray secondId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-2");
	const QByteArray thirdId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-3");

	// The link closing the cycle is omitted.
	const imtauth::IUserGroupInfo* firstPtr = fixture.GetGroup(firstId);
	const imtauth::IUserGroupInfo* secondPtr = fixture.GetGroup(secondId);
	const imtauth::IUserGroupInfo* thirdPtr = fixture.GetGroup(thirdId);
	QVERIFY(firstPtr != nullptr && secondPtr != nullptr && thirdPtr != nullptr);
	QCOMPARE(firstPtr->GetParentGroups(), QByteArrayList({thirdId}));
	QCOMPARE(secondPtr->GetParentGroups(), QByteArrayList({firstId}));
	QVERIFY(thirdPtr->GetParentGroups().isEmpty());

	// The result is stable.
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));
	thirdPtr = fixture.GetGroup(thirdId);
	QVERIFY(thirdPtr != nullptr);
	QVERIFY(thirdPtr->GetParentGroups().isEmpty());
}


void CLdapUserGroupSynchronizerTest::testSynchronize_DisambiguatesGroupNames()
{
	CFixture fixture;

	fixture.AddInternalGroup("Admins");

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Admins");
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Users");
	ldapGroups << MakeLdapGroup("S-1-5-21-3", "Users");
	ldapGroups << MakeLdapGroup("S-1-5-21-4", "Guests");
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	const imtauth::IUserGroupInfo* adminsPtr = fixture.GetGroup(imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1"));
	const imtauth::IUserGroupInfo* firstUsersPtr = fixture.GetGroup(imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-2"));
	const imtauth::IUserGroupInfo* secondUsersPtr = fixture.GetGroup(imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-3"));
	const imtauth::IUserGroupInfo* guestsPtr = fixture.GetGroup(imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-4"));
	QVERIFY(adminsPtr != nullptr && firstUsersPtr != nullptr && secondUsersPtr != nullptr && guestsPtr != nullptr);
	QCOMPARE(adminsPtr->GetName(), QString("Admins (S-1-5-21-1)"));
	QCOMPARE(firstUsersPtr->GetName(), QString("Users (S-1-5-21-2)"));
	QCOMPARE(secondUsersPtr->GetName(), QString("Users (S-1-5-21-3)"));
	QCOMPARE(guestsPtr->GetName(), QString("Guests"));
}


void CLdapUserGroupSynchronizerTest::testIsMatchingName()
{
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("*", "Any Group"));
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName(QString(), "Any Group"));
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("App*", "application users"));
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("*Admins", "Domain Admins"));
	QVERIFY(!imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("App*", "My App"));
	QVERIFY(!imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("a.b", "axb"));
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("a.b(c)", "A.B(C)"));
}


I_ADD_TEST(CLdapUserGroupSynchronizerTest);


