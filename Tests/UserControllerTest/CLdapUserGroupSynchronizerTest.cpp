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
#include <iprm/CComposedParamsSetComp.h>
#include <iprm/CEnableableParamComp.h>
#include <iprm/CTextParamComp.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>
#include <imtauth/CLdapUserGroupSynchronizerComp.h>
#include <imtauth/CUserGroupInfo.h>
#include <imtauth/CUserInfo.h>
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


class CTestVersionInfoComp:
			public icomp::CComponentBase,
			public CFixedVersionInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestVersionInfoComp);
		I_REGISTER_INTERFACE(iser::IVersionInfo);
		I_ASSIGN(m_imtCoreVersionAttrPtr, "ImtCoreVersion", "ImtCore version number", true, 0);
	I_END_COMPONENT;

	CTestVersionInfoComp(): CFixedVersionInfo(0)
	{
	}

	// reimplemented (iser::IVersionInfo)
	virtual bool GetVersionNumber(int versionId, quint32& result) const override
	{
		if (versionId == imtcore::VI_IMTCORE){
			result = quint32(*m_imtCoreVersionAttrPtr);

			return true;
		}

		return false;
	}

private:
	I_ATTR(int, m_imtCoreVersionAttrPtr);
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
	virtual istd::IChangeableUniquePtr CreateObjectInstance(const QByteArray& typeId) const override
	{
		if (typeId == "User"){
			return new imtauth::CIdentifiableUserInfo;
		}

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


imtauth::CLdapUserGroupSynchronizerComp::LdapUser MakeLdapUser(
			const QByteArray& sid,
			const QByteArray& accountName,
			const QByteArrayList& groupSids)
{
	imtauth::CLdapUserGroupSynchronizerComp::LdapUser retVal;
	retVal.sid = sid;
	retVal.accountName = accountName;
	retVal.domainAccountName = "EXAMPLE\\" + accountName;
	retVal.login = accountName + "@example.com";
	retVal.name = QString::fromUtf8(accountName).toUpper();
	retVal.description = QString::fromUtf8(accountName) + QStringLiteral(" description");
	retVal.mail = QString::fromUtf8(retVal.login);
	retVal.groupSids = groupSids;

	return retVal;
}


class CFixture
{
public:
	explicit CFixture(const QByteArray& ldapSystemId = "Ldap")
	{
		groups->InitComponent();
		users->InitComponent();

		synchronizer->SetRef("UserGroupCollection", groups);
		synchronizer->SetRef("UserCollection", users);
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

	const imtauth::IUserInfo* GetUser(const QByteArray& userId)
	{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!users->GetObjectData(userId, dataPtr)){
			return nullptr;
		}

		loadedGroups << dataPtr;

		return dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	QByteArray AddInternalUser(const QByteArray& login, const QByteArray& sid = QByteArray())
	{
		const QByteArray userId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

		imtauth::CIdentifiableUserInfo user;
		user.SetObjectUuid(userId);
		user.SetId(login);
		user.SetName(QString::fromUtf8(login));
		user.SetSid(sid);
		users->InsertNewObject("User", user.GetName(), QString(), &user, userId);

		return userId;
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
	icomp::TSimSharedComponentPtr<CTestGroupCollectionComp> users;
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


void CLdapUserGroupSynchronizerTest::testSynchronize_RemovesMembershipsOfVanishedGroups()
{
	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Child");
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	const QByteArray rootId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1");
	const QByteArray childId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-2");

	// An internal user added to both LDAP groups by an administrator.
	const QByteArray userId = fixture.AddInternalUser("alice");
	{
		const imtauth::IUserInfo* storedUserPtr = fixture.GetUser(userId);
		QVERIFY(storedUserPtr != nullptr);

		istd::IChangeableUniquePtr userCopyPtr = storedUserPtr->CloneMe();
		imtauth::IUserInfo* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(userCopyPtr.GetPtr());
		QVERIFY(userInfoPtr != nullptr);
		userInfoPtr->AddToGroup(rootId);
		userInfoPtr->AddToGroup(childId);
		QVERIFY(fixture.users->SetObjectData(userId, *userInfoPtr));
	}

	// The root group was removed from the directory, the run without a member snapshot removes the membership too.
	ldapGroups.clear();
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Child");
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	QVERIFY(fixture.GetGroup(rootId) == nullptr);

	const imtauth::IUserInfo* userPtr = fixture.GetUser(userId);
	QVERIFY(userPtr != nullptr);
	QCOMPARE(userPtr->GetGroups(), QByteArrayList({childId}));

	// The user cannot be written: the group is kept, the next run completes the removal.
	fixture.users->m_writeFailingIds.insert(userId);
	QVERIFY(!fixture.synchronizer->SynchronizeGroups(imtauth::CLdapUserGroupSynchronizerComp::LdapGroups()));
	fixture.users->m_writeFailingIds.clear();

	QVERIFY(fixture.GetGroup(childId) != nullptr);

	QVERIFY(fixture.synchronizer->SynchronizeGroups(imtauth::CLdapUserGroupSynchronizerComp::LdapGroups()));
	QVERIFY(fixture.GetGroup(childId) == nullptr);

	userPtr = fixture.GetUser(userId);
	QVERIFY(userPtr != nullptr);
	QVERIFY(userPtr->GetGroups().isEmpty());
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


void CLdapUserGroupSynchronizerTest::testGetMirroredLdapGroups()
{
	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "App Root");
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));

	// The existing mirror is kept also outside of the filter, groups outside of the filter without mirror are not mirrored.
	ldapGroups.clear();
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root", {}, false);
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "App Child", {"S-1-5-21-1"});
	ldapGroups << MakeLdapGroup("S-1-5-21-3", "Other", {}, false);

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups mirroredGroups;
	QVERIFY(fixture.synchronizer->GetMirroredLdapGroups(ldapGroups, mirroredGroups));
	QCOMPARE(mirroredGroups.count(), 2);
	QCOMPARE(mirroredGroups[0].sid, QByteArray("S-1-5-21-1"));
	QCOMPARE(mirroredGroups[1].sid, QByteArray("S-1-5-21-2"));
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

void CLdapUserGroupSynchronizerTest::testSynchronize_AddsExistingUsersToGroups()
{
	CFixture fixture;

	// Known in the system by the SID, by the user principal name and by DOMAIN\account name.
	const QByteArray sidUserId = fixture.AddInternalUser("alice.local", "S-1-5-21-100");
	const QByteArray principalUserId = fixture.AddInternalUser("BOB@example.com");
	const QByteArray accountUserId = fixture.AddInternalUser("EXAMPLE\\carol");

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Child", {"S-1-5-21-1"});

	imtauth::CLdapUserGroupSynchronizerComp::LdapUsers ldapUsers;
	ldapUsers << MakeLdapUser("S-1-5-21-100", "alice", {"S-1-5-21-1"});
	ldapUsers << MakeLdapUser("S-1-5-21-101", "bob", {"S-1-5-21-1", "S-1-5-21-2"});
	ldapUsers << MakeLdapUser("S-1-5-21-102", "carol", {"S-1-5-21-2", "S-1-5-21-99"});
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups, &ldapUsers));

	const QByteArray rootId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1");
	const QByteArray childId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-2");

	// No user is created, the existing users are only added to the groups.
	QCOMPARE(fixture.users->GetElementIds().size(), 3);

	const imtauth::IUserInfo* alicePtr = fixture.GetUser(sidUserId);
	QVERIFY(alicePtr != nullptr);
	QCOMPARE(alicePtr->GetId(), QByteArray("alice.local"));
	QCOMPARE(alicePtr->GetName(), QString("alice.local"));
	QCOMPARE(alicePtr->GetGroups(), QByteArrayList({rootId}));

	const imtauth::IUserInfo* bobPtr = fixture.GetUser(principalUserId);
	QVERIFY(bobPtr != nullptr);
	QCOMPARE(bobPtr->GetId(), QByteArray("BOB@example.com"));
	QCOMPARE(bobPtr->GetSid(), QByteArray("S-1-5-21-101"));
	QCOMPARE(bobPtr->GetGroups(), QByteArrayList({rootId, childId}));

	const imtauth::IUserInfo* carolPtr = fixture.GetUser(accountUserId);
	QVERIFY(carolPtr != nullptr);
	QCOMPARE(carolPtr->GetGroups(), QByteArrayList({childId}));

	const imtauth::IUserGroupInfo* rootPtr = fixture.GetGroup(rootId);
	const imtauth::CUserGroupInfo* childPtr = dynamic_cast<const imtauth::CUserGroupInfo*>(fixture.GetGroup(childId));
	QVERIFY(rootPtr != nullptr && childPtr != nullptr);
	QCOMPARE(rootPtr->GetUsers(), QByteArrayList({sidUserId, principalUserId}));
	QCOMPARE(childPtr->GetOwnUsers(), QByteArrayList({principalUserId, accountUserId}));

	// The users found by the login are linked by the SID, they leave the groups also when they are no longer in the snapshot.
	ldapUsers.clear();
	ldapUsers << MakeLdapUser("S-1-5-21-100", "alice", {"S-1-5-21-1"});
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups, &ldapUsers));

	QCOMPARE(fixture.users->GetElementIds().size(), 3);
	QVERIFY(fixture.GetUser(principalUserId)->GetGroups().isEmpty());
	QVERIFY(fixture.GetUser(accountUserId)->GetGroups().isEmpty());
	QCOMPARE(fixture.GetGroup(rootId)->GetUsers(), QByteArrayList({sidUserId}));
	QVERIFY(dynamic_cast<const imtauth::CUserGroupInfo*>(fixture.GetGroup(childId))->GetOwnUsers().isEmpty());

	// DOMAIN\account name of another domain or without resolved domain is another account, it is not linked.
	CFixture domainFixture;
	const QByteArray otherDomainUserId = domainFixture.AddInternalUser("OTHER\\erin");
	const QByteArray unresolvedDomainUserId = domainFixture.AddInternalUser("EXAMPLE\\frank");

	imtauth::CLdapUserGroupSynchronizerComp::LdapUser unresolvedDomainUser = MakeLdapUser("S-1-5-21-104", "frank", {"S-1-5-21-1"});
	unresolvedDomainUser.domainAccountName.clear();

	ldapUsers.clear();
	ldapUsers << MakeLdapUser("S-1-5-21-103", "erin", {"S-1-5-21-1"});
	ldapUsers << unresolvedDomainUser;
	QVERIFY(domainFixture.synchronizer->SynchronizeGroups(ldapGroups, &ldapUsers));

	const QByteArray erinUserId = imtauth::CLdapUserGroupSynchronizerComp::GetUserObjectId("S-1-5-21-103");
	const QByteArray frankUserId = imtauth::CLdapUserGroupSynchronizerComp::GetUserObjectId("S-1-5-21-104");
	QCOMPARE(domainFixture.users->GetElementIds().size(), 4);
	QVERIFY(domainFixture.GetUser(otherDomainUserId)->GetSid().isEmpty());
	QVERIFY(domainFixture.GetUser(otherDomainUserId)->GetGroups().isEmpty());
	QVERIFY(domainFixture.GetUser(unresolvedDomainUserId)->GetSid().isEmpty());
	QVERIFY(domainFixture.GetUser(unresolvedDomainUserId)->GetGroups().isEmpty());
	QCOMPARE(domainFixture.GetUser(erinUserId)->GetSid(), QByteArray("S-1-5-21-103"));
	QCOMPARE(domainFixture.GetUser(frankUserId)->GetSid(), QByteArray("S-1-5-21-104"));
	const QByteArrayList rootUserIds = domainFixture.GetGroup(rootId)->GetUsers();
	QCOMPARE(rootUserIds.size(), 2);
	QVERIFY(rootUserIds.contains(erinUserId) && rootUserIds.contains(frankUserId));
}


void CLdapUserGroupSynchronizerTest::testSynchronize_CreatesUnknownUsers()
{
	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");

	imtauth::CLdapUserGroupSynchronizerComp::LdapUsers ldapUsers;
	ldapUsers << MakeLdapUser("S-1-5-21-100", "dave", {"S-1-5-21-1"});
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups, &ldapUsers));

	const QByteArray rootId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1");
	const QByteArray userId = imtauth::CLdapUserGroupSynchronizerComp::GetUserObjectId("S-1-5-21-100");
	QVERIFY(userId != imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-100"));

	QCOMPARE(fixture.users->GetElementIds(), imtbase::ICollectionInfo::Ids({userId}));

	const imtauth::IUserInfo* userPtr = fixture.GetUser(userId);
	QVERIFY(userPtr != nullptr);
	QCOMPARE(userPtr->GetId(), QByteArray("dave@example.com"));
	QCOMPARE(userPtr->GetName(), QString("DAVE"));
	QCOMPARE(userPtr->GetDescription(), QString("dave description"));
	QCOMPARE(userPtr->GetMail(), QString("dave@example.com"));
	QCOMPARE(userPtr->GetSid(), QByteArray("S-1-5-21-100"));
	QVERIFY(userPtr->GetPasswordHash().isEmpty());
	QCOMPARE(userPtr->GetSystemInfos().size(), 1);
	QCOMPARE(userPtr->GetSystemInfos().first().systemId, QByteArray("Ldap"));
	QCOMPARE(userPtr->GetGroups(), QByteArrayList({rootId}));

	const imtauth::IUserGroupInfo* rootPtr = fixture.GetGroup(rootId);
	QVERIFY(rootPtr != nullptr);
	QCOMPARE(rootPtr->GetUsers(), QByteArrayList({userId}));

	// A second run without directory changes changes nothing.
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups, &ldapUsers));
	QCOMPARE(fixture.users->GetElementIds(), imtbase::ICollectionInfo::Ids({userId}));

	// A login used by another account is not taken over.
	CFixture usedLoginFixture;
	const QByteArray otherUserId = usedLoginFixture.AddInternalUser("dave@example.com", "S-1-5-21-999");
	QVERIFY(usedLoginFixture.synchronizer->SynchronizeGroups(ldapGroups, &ldapUsers));
	QCOMPARE(usedLoginFixture.users->GetElementIds(), imtbase::ICollectionInfo::Ids({otherUserId}));
	const imtauth::IUserGroupInfo* usedLoginRootPtr = usedLoginFixture.GetGroup(rootId);
	QVERIFY(usedLoginRootPtr != nullptr);
	QVERIFY(usedLoginRootPtr->GetUsers().isEmpty());
}


void CLdapUserGroupSynchronizerTest::testSynchronize_UpdatesMembers()
{
	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");
	ldapGroups << MakeLdapGroup("S-1-5-21-2", "Other");

	imtauth::CLdapUserGroupSynchronizerComp::LdapUsers ldapUsers;
	ldapUsers << MakeLdapUser("S-1-5-21-100", "erin", {"S-1-5-21-1"});
	ldapUsers << MakeLdapUser("S-1-5-21-101", "frank", {"S-1-5-21-1", "S-1-5-21-2"});
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups, &ldapUsers));

	const QByteArray rootId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-1");
	const QByteArray otherId = imtauth::CLdapUserGroupSynchronizerComp::GetGroupObjectId("S-1-5-21-2");
	const QByteArray erinId = imtauth::CLdapUserGroupSynchronizerComp::GetUserObjectId("S-1-5-21-100");
	const QByteArray frankId = imtauth::CLdapUserGroupSynchronizerComp::GetUserObjectId("S-1-5-21-101");

	// An administrator adds an internal user to the mirror and the LDAP user to an internal group.
	const QByteArray internalUserId = fixture.AddInternalUser("internal");
	const QByteArray internalGroupId = fixture.AddInternalGroup("Internal");
	{
		const imtauth::IUserGroupInfo* storedRootPtr = fixture.GetGroup(rootId);
		QVERIFY(storedRootPtr != nullptr);
		istd::IChangeableUniquePtr editedRootPtr = storedRootPtr->CloneMe();
		imtauth::IUserGroupInfo* editedRootInfoPtr = dynamic_cast<imtauth::IUserGroupInfo*>(editedRootPtr.GetPtr());
		QVERIFY(editedRootInfoPtr != nullptr);
		editedRootInfoPtr->AddUser(internalUserId);
		QVERIFY(fixture.groups->SetObjectData(rootId, *editedRootInfoPtr));

		const imtauth::IUserInfo* storedFrankPtr = fixture.GetUser(frankId);
		QVERIFY(storedFrankPtr != nullptr);
		istd::IChangeableUniquePtr editedFrankPtr = storedFrankPtr->CloneMe();
		imtauth::IUserInfo* editedFrankInfoPtr = dynamic_cast<imtauth::IUserInfo*>(editedFrankPtr.GetPtr());
		QVERIFY(editedFrankInfoPtr != nullptr);
		editedFrankInfoPtr->AddToGroup(internalGroupId);
		QVERIFY(fixture.users->SetObjectData(frankId, *editedFrankInfoPtr));
	}

	// In the directory erin left all groups, frank left the root group and the other group was removed.
	ldapGroups.clear();
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");
	ldapUsers.clear();
	ldapUsers << MakeLdapUser("S-1-5-21-101", "frank", {"S-1-5-21-3"});
	ldapUsers << MakeLdapUser("S-1-5-21-102", "gina", {"S-1-5-21-1"});
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups, &ldapUsers));

	const QByteArray ginaId = imtauth::CLdapUserGroupSynchronizerComp::GetUserObjectId("S-1-5-21-102");

	QVERIFY(fixture.GetGroup(otherId) == nullptr);

	const imtauth::IUserGroupInfo* rootPtr = fixture.GetGroup(rootId);
	QVERIFY(rootPtr != nullptr);
	QCOMPARE(rootPtr->GetUsers(), QByteArrayList({internalUserId, ginaId}));

	// The users are kept, only the memberships of the mirrors are changed.
	const imtauth::IUserInfo* erinPtr = fixture.GetUser(erinId);
	QVERIFY(erinPtr != nullptr);
	QVERIFY(erinPtr->GetGroups().isEmpty());

	const imtauth::IUserInfo* frankPtr = fixture.GetUser(frankId);
	QVERIFY(frankPtr != nullptr);
	QCOMPARE(frankPtr->GetGroups(), QByteArrayList({internalGroupId}));

	const imtauth::IUserInfo* ginaPtr = fixture.GetUser(ginaId);
	QVERIFY(ginaPtr != nullptr);
	QCOMPARE(ginaPtr->GetGroups(), QByteArrayList({rootId}));

	// Without user snapshot the members are kept.
	QVERIFY(fixture.synchronizer->SynchronizeGroups(ldapGroups));
	rootPtr = fixture.GetGroup(rootId);
	QVERIFY(rootPtr != nullptr);
	QCOMPARE(rootPtr->GetUsers(), QByteArrayList({internalUserId, ginaId}));
}


void CLdapUserGroupSynchronizerTest::testSynchronize_RejectsInvalidUserSnapshot()
{
	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups ldapGroups;
	ldapGroups << MakeLdapGroup("S-1-5-21-1", "Root");

	CFixture fixture;

	imtauth::CLdapUserGroupSynchronizerComp::LdapUsers ldapUsers;
	ldapUsers << MakeLdapUser("S-1-5-21-100", "henry", {"S-1-5-21-1"});
	ldapUsers << MakeLdapUser("S-1-5-21-100", "henry2", {"S-1-5-21-1"});
	QVERIFY(!fixture.synchronizer->SynchronizeGroups(ldapGroups, &ldapUsers));

	ldapUsers.clear();
	ldapUsers << MakeLdapUser(QByteArray(), "henry", {"S-1-5-21-1"});
	QVERIFY(!fixture.synchronizer->SynchronizeGroups(ldapGroups, &ldapUsers));

	QVERIFY(fixture.groups->GetElementIds().isEmpty());
	QVERIFY(fixture.users->GetElementIds().isEmpty());

	// Unreadable users change nothing.
	fixture.users->m_isListReadFailing = true;
	ldapUsers.clear();
	ldapUsers << MakeLdapUser("S-1-5-21-100", "henry", {"S-1-5-21-1"});
	QVERIFY(!fixture.synchronizer->SynchronizeGroups(ldapGroups, &ldapUsers));
	QVERIFY(fixture.groups->GetElementIds().isEmpty());

	// Members cannot be synchronized without user collection.
	icomp::TSimSharedComponentPtr<CTestGroupCollectionComp> groups;
	groups->InitComponent();
	icomp::TSimSharedComponentPtr<imtauth::CLdapUserGroupSynchronizerComp> synchronizer;
	synchronizer->SetRef("UserGroupCollection", groups);
	synchronizer->SetIntAttr("CheckInterval", 0);
	synchronizer->InitComponent();
	QVERIFY(!synchronizer->SynchronizeGroups(ldapGroups, &ldapUsers));
	QVERIFY(synchronizer->SynchronizeGroups(ldapGroups));
}


void CLdapUserGroupSynchronizerTest::testIsMatchingName()
{
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("*", "Any Group"));
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName(QString(), "Any Group"));
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("App*", "application users"));
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("MyGrou*", "myGroup users"));
	QVERIFY(!imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("MyGrou*", "other myGroup"));
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("*Admins", "Domain Admins"));
	QVERIFY(!imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("App*", "My App"));
	QVERIFY(!imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("a.b", "axb"));
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::IsMatchingName("a.b(c)", "A.B(C)"));
}


void CLdapUserGroupSynchronizerTest::testGetSearchPath()
{
	// Without base DN the default naming context is used.
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::GetSearchPath(QString()).isEmpty());
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::GetSearchPath("  ").isEmpty());

	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchPath(" DC=atlascopco,DC=group "), QString("LDAP://DC=atlascopco,DC=group"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchPath("DC=atlascopco, DC=group"), QString("LDAP://DC=atlascopco,DC=group"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchPath("OU=Groups,DC=example,DC=com"), QString("LDAP://OU=Groups,DC=example,DC=com"));

	// An LDAP path is used unchanged (except of the provider name), slashes inside of a DN are escaped once.
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchPath("LDAP://server/DC=example,DC=com"), QString("LDAP://server/DC=example,DC=com"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchPath("ldap://server/DC=example,DC=com"), QString("LDAP://server/DC=example,DC=com"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchPath("OU=A/B,DC=example,DC=com"), QString("LDAP://OU=A\\/B,DC=example,DC=com"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchPath("OU=A\\/B,DC=example,DC=com"), QString("LDAP://OU=A\\/B,DC=example,DC=com"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchPath("OU=A\\,B,DC=example,DC=com"), QString("LDAP://OU=A\\,B,DC=example,DC=com"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchPath("CN=Example\\, Group, DC=example, DC=com"), QString("LDAP://CN=Example\\, Group,DC=example,DC=com"));
}


void CLdapUserGroupSynchronizerTest::testGetDomainSearchPath()
{
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::GetDomainSearchPath(QString()).isEmpty());

	// The members are searched in the whole domain of the base DN.
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetDomainSearchPath("OU=Groups,DC=example,DC=com"), QString("LDAP://DC=example,DC=com"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetDomainSearchPath("ou=A\\,B, dc=example, dc=com"), QString("LDAP://dc=example,dc=com"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetDomainSearchPath("DC=example,DC=com"), QString("LDAP://DC=example,DC=com"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetDomainSearchPath("LDAP://server/OU=A\\/B,DC=example,DC=com"), QString("LDAP://server/DC=example,DC=com"));

	// Without domain components the path is used unchanged.
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetDomainSearchPath("OU=Groups,O=Example"), QString("LDAP://OU=Groups,O=Example"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetDomainSearchPath("CN=DC=x,OU=Groups"), QString("LDAP://CN=DC=x,OU=Groups"));
}


void CLdapUserGroupSynchronizerTest::testGetSearchFilter()
{
	// Without name filter all groups are queried.
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchFilter(QString()), QString("(objectCategory=group)"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchFilter("*"), QString("(objectCategory=group)"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchFilter("**"), QString("(objectCategory=group)"));

	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchFilter("MyGrou*"), QString("(&(objectCategory=group)(name=MyGrou*))"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchFilter("*Admins"), QString("(&(objectCategory=group)(name=*Admins))"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchFilter("App**Users"), QString("(&(objectCategory=group)(name=App*Users))"));

	// Special characters of the filter syntax are literal characters of the name.
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchFilter("a.b(c)"), QString("(&(objectCategory=group)(name=a.b\\28c\\29))"));
	QCOMPARE(imtauth::CLdapUserGroupSynchronizerComp::GetSearchFilter("A\\B"), QString("(&(objectCategory=group)(name=A\\5cB))"));
}


void CLdapUserGroupSynchronizerTest::testGetUserSearchFilters()
{
	// Without groups no users are queried.
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::GetUserSearchFilters(QStringList()).isEmpty());
	QVERIFY(imtauth::CLdapUserGroupSynchronizerComp::GetUserSearchFilters(QStringList() << QString()).isEmpty());

	// Only the members of the given groups are queried.
	QCOMPARE(
				imtauth::CLdapUserGroupSynchronizerComp::GetUserSearchFilters(QStringList() << "CN=A,DC=example,DC=com"),
				QStringList() << "(&(objectCategory=person)(objectClass=user)(memberOf=CN=A,DC=example,DC=com))");

	// Duplicates are ignored case-insensitively.
	QCOMPARE(
				imtauth::CLdapUserGroupSynchronizerComp::GetUserSearchFilters(QStringList() << "CN=A,DC=x" << "CN=B,DC=x" << "cn=a,dc=x"),
				QStringList() << "(&(objectCategory=person)(objectClass=user)(|(memberOf=CN=A,DC=x)(memberOf=CN=B,DC=x)))");

	// The groups are split into several filters.
	QCOMPARE(
				imtauth::CLdapUserGroupSynchronizerComp::GetUserSearchFilters(QStringList() << "CN=A" << "CN=B" << "CN=C", 2),
				QStringList()
							<< "(&(objectCategory=person)(objectClass=user)(|(memberOf=CN=A)(memberOf=CN=B)))"
							<< "(&(objectCategory=person)(objectClass=user)(memberOf=CN=C))");

	// Special characters of the filter syntax are escaped, also the escape characters of the DN.
	QCOMPARE(
				imtauth::CLdapUserGroupSynchronizerComp::GetUserSearchFilters(QStringList() << "CN=Doe\\, John (Admins*),DC=x"),
				QStringList() << "(&(objectCategory=person)(objectClass=user)(memberOf=CN=Doe\\5c, John \\28Admins\\2a\\29,DC=x))");
}


void CLdapUserGroupSynchronizerTest::testGetBaseDn()
{
	// Without base DN parameter the default naming context is used.
	CFixture fixture;
	QVERIFY(fixture.synchronizer->GetBaseDn().isEmpty());

	icomp::TSimSharedComponentPtr<iprm::CTextParamComp> baseDnParam;
	baseDnParam->SetBoolAttr("IsReadOnly", false);
	baseDnParam->SetBoolAttr("SerializeParams", true);
	baseDnParam->InitComponent();

	icomp::TSimSharedComponentPtr<CTestGroupCollectionComp> groups;
	groups->InitComponent();

	icomp::TSimSharedComponentPtr<imtauth::CLdapUserGroupSynchronizerComp> synchronizer;
	synchronizer->SetRef("UserGroupCollection", groups);
	synchronizer->SetRef("BaseDnParam", baseDnParam);
	synchronizer->SetIntAttr("CheckInterval", 0);
	synchronizer->InitComponent();

	QVERIFY(synchronizer->GetBaseDn().isEmpty());

	// The current value is used, a changed setting needs no restart.
	baseDnParam->SetText(" DC=atlascopco,DC=group ");
	QCOMPARE(synchronizer->GetBaseDn(), QString("DC=atlascopco,DC=group"));
}


void CLdapUserGroupSynchronizerTest::testGetGroupNameFilter()
{
	CFixture fixture;
	QCOMPARE(fixture.synchronizer->GetGroupNameFilter(), QStringLiteral("*"));

	icomp::TSimSharedComponentPtr<imtauth::CLdapUserGroupSynchronizerComp> legacySynchronizer;
	legacySynchronizer->SetRef("UserGroupCollection", fixture.groups);
	legacySynchronizer->SetStringAttr("GroupNameFilter", QStringLiteral("Legacy*"));
	legacySynchronizer->SetIntAttr("CheckInterval", 0);
	legacySynchronizer->InitComponent();
	QCOMPARE(legacySynchronizer->GetGroupNameFilter(), QStringLiteral("Legacy*"));

	icomp::TSimSharedComponentPtr<iprm::CTextParamComp> nameFilterParam;
	nameFilterParam->SetBoolAttr("IsReadOnly", false);
	nameFilterParam->InitComponent();

	icomp::TSimSharedComponentPtr<imtauth::CLdapUserGroupSynchronizerComp> synchronizer;
	synchronizer->SetRef("UserGroupCollection", fixture.groups);
	synchronizer->SetStringAttr("GroupNameFilter", QStringLiteral("Legacy*"));
	synchronizer->SetRef("GroupNameFilterParam", nameFilterParam);
	synchronizer->SetIntAttr("CheckInterval", 0);
	synchronizer->InitComponent();

	QVERIFY(synchronizer->GetGroupNameFilter().isEmpty());
	nameFilterParam->SetText("MyGrou*");
	QCOMPARE(synchronizer->GetGroupNameFilter(), QStringLiteral("MyGrou*"));
	nameFilterParam->SetText("*Admins");
	QCOMPARE(synchronizer->GetGroupNameFilter(), QStringLiteral("*Admins"));
}


void CLdapUserGroupSynchronizerTest::testLdapSettingsLabels()
{
	icomp::TSimSharedComponentPtr<iprm::CEnableableParamComp> enabledParam;
	enabledParam->InitComponent();
	icomp::TSimSharedComponentPtr<iprm::CTextParamComp> filterParam;
	filterParam->InitComponent();

	icomp::TSimSharedComponentPtr<iprm::CComposedParamsSetComp> settings;
	QVERIFY(settings->InsertMultiRef("Parameters", enabledParam));
	QVERIFY(settings->InsertMultiRef("Parameters", filterParam));

	auto* idsPtr = new iattr::CIdListAttribute;
	idsPtr->InsertValue("LdapEnabled");
	idsPtr->InsertValue("LdapGroupFilter");
	QVERIFY(settings->SetAttr("ParametersId", idsPtr));
	QVERIFY(settings->InsertMultiAttr("ParametersName", QStringLiteral("Enable LDAP")));
	QVERIFY(settings->InsertMultiAttr("ParametersName", QStringLiteral("LDAP-Gruppenfilter (Basis-DN f\u00fcr Gruppen)")));
	QVERIFY(settings->InsertMultiAttr("ParametersDescription", QString()));
	QVERIFY(settings->InsertMultiAttr("ParametersDescription", QStringLiteral("Verzeichnis f\u00fcr die Suche")));
	settings->InitComponent();

	const auto* infoProviderPtr = settings->GetParamsInfoProvider();
	QVERIFY(infoProviderPtr != nullptr);
	const auto enabledInfo = infoProviderPtr->GetParamInfo("LdapEnabled");
	QVERIFY(enabledInfo != nullptr);
	QCOMPARE(enabledInfo->name, QStringLiteral("Enable LDAP"));
	QVERIFY(enabledInfo->description.isEmpty());
	const auto filterInfo = infoProviderPtr->GetParamInfo("LdapGroupFilter");
	QVERIFY(filterInfo != nullptr);
	QCOMPARE(filterInfo->name, QStringLiteral("LDAP-Gruppenfilter (Basis-DN f\u00fcr Gruppen)"));
	QCOMPARE(filterInfo->description, QStringLiteral("Verzeichnis f\u00fcr die Suche"));
	QVERIFY(infoProviderPtr->GetParamInfo("Unknown") == nullptr);
}


void CLdapUserGroupSynchronizerTest::testCreateGroupTree()
{
	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups groups;
	groups << MakeLdapGroup("S-1", "Sales")
				<< MakeLdapGroup("S-2", "engineering")
				<< MakeLdapGroup("S-3", "Design", {"S-2"})
				<< MakeLdapGroup("S-4", "Backend", {"S-2", "S-1"})
				<< MakeLdapGroup("S-5", "Hidden", QByteArrayList(), false)
				<< MakeLdapGroup("S-6", "Tools", {"S-5", "S-404"})
				<< MakeLdapGroup("S-7", "Api", {"S-4"});

	const imtauth::CLdapUserGroupSynchronizerComp::LdapGroupTree tree = imtauth::CLdapUserGroupSynchronizerComp::CreateGroupTree(groups);

	// Sorted by name per level, a group with two parents is shown below both, not matching or unknown parents are ignored.
	QStringList items;
	for (const imtauth::CLdapUserGroupSynchronizerComp::LdapGroupTreeItem& item : tree){
		items << QString::number(item.level) + QLatin1Char(':') + groups[item.groupIndex].name;
	}

	QCOMPARE(items, QStringList({
				"0:engineering",
				"1:Backend",
				"2:Api",
				"1:Design",
				"0:Sales",
				"1:Backend",
				"2:Api",
				"0:Tools"}));
}


void CLdapUserGroupSynchronizerTest::testCreateGroupTreeCycle()
{
	imtauth::CLdapUserGroupSynchronizerComp::LdapGroups groups;
	groups << MakeLdapGroup("S-1", "A", {"S-2"})
				<< MakeLdapGroup("S-2", "B", {"S-1"})
				<< MakeLdapGroup("S-3", "C", {"S-3"});

	const imtauth::CLdapUserGroupSynchronizerComp::LdapGroupTree tree = imtauth::CLdapUserGroupSynchronizerComp::CreateGroupTree(groups);

	// Groups only reachable through a cycle are shown on the top level, each cycle once per branch.
	QStringList items;
	for (const imtauth::CLdapUserGroupSynchronizerComp::LdapGroupTreeItem& item : tree){
		items << QString::number(item.level) + QLatin1Char(':') + groups[item.groupIndex].name;
	}

	QCOMPARE(items, QStringList({"0:C", "0:A", "1:B"}));
}


void CLdapUserGroupSynchronizerTest::testIsSynchronizationEnabled()
{
	// Without enableable parameters the synchronization is always enabled.
	CFixture fixture;
	QVERIFY(fixture.synchronizer->IsSynchronizationEnabled());

	icomp::TSimSharedComponentPtr<iprm::CEnableableParamComp> enabledParam;
	enabledParam->SetBoolAttr("DefaultEnabled", true);
	enabledParam->InitComponent();

	icomp::TSimSharedComponentPtr<iprm::CEnableableParamComp> disabledParam;
	disabledParam->SetBoolAttr("DefaultEnabled", false);
	disabledParam->InitComponent();

	icomp::TSimSharedComponentPtr<CTestGroupCollectionComp> groups;
	groups->InitComponent();

	icomp::TSimSharedComponentPtr<imtauth::CLdapUserGroupSynchronizerComp> synchronizer;
	synchronizer->SetRef("UserGroupCollection", groups);
	synchronizer->InsertMultiRef("EnableableParams", enabledParam);
	synchronizer->InsertMultiRef("EnableableParams", disabledParam);
	synchronizer->SetIntAttr("CheckInterval", 0);
	synchronizer->InitComponent();

	// All parameters must be enabled.
	QVERIFY(!synchronizer->IsSynchronizationEnabled());

	disabledParam->SetEnabled(true);
	QVERIFY(synchronizer->IsSynchronizationEnabled());

	enabledParam->SetEnabled(false);
	QVERIFY(!synchronizer->IsSynchronizationEnabled());
}


void CLdapUserGroupSynchronizerTest::testIsSystemFlagStored()
{
	// Without a version info the version is not checked.
	CFixture fixture;
	QVERIFY(fixture.synchronizer->IsSystemFlagStored());

	for (const quint32 version : {s_groupSystemInfosVersion - 1, s_groupSystemInfosVersion}){
		icomp::TSimSharedComponentPtr<CTestVersionInfoComp> versionInfo;
		versionInfo->SetIntAttr("ImtCoreVersion", int(version));
		versionInfo->InitComponent();

		icomp::TSimSharedComponentPtr<CTestGroupCollectionComp> groups;
		groups->InitComponent();

		icomp::TSimSharedComponentPtr<imtauth::CLdapUserGroupSynchronizerComp> synchronizer;
		synchronizer->SetRef("UserGroupCollection", groups);
		synchronizer->SetRef("VersionInfo", versionInfo);
		synchronizer->SetIntAttr("CheckInterval", 0);
		synchronizer->InitComponent();

		QCOMPARE(synchronizer->IsSystemFlagStored(), version >= s_groupSystemInfosVersion);
	}
}


I_ADD_TEST(CLdapUserGroupSynchronizerTest);
