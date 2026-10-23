// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CPermissionCheckTest.h"


// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtauth/CCheckPermissionCompBase.h>
#include <imtauth/CCheckPermissionOrComp.h>
#include <imtauth/CUserInfo.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlParamObject.h>
#include <imtgql/CGqlRequest.h>
#include <imtlic/CFeatureInfo.h>
#include <imtlic/CProductInfo.h>
#include <imtserverapp/imtserverapp.h>
#include <imtservergql/imtservergql.h>


namespace
{


typedef QHash<QByteArray, QByteArrayList> Requirements;


const QByteArray s_productId = "ProLife";
const QByteArray s_tenantId = "TenantA";

const QByteArray s_viewAdministration = "/Administration/ViewAdministration";
const QByteArray s_viewUsers = "/Administration/ViewAdministration/ViewUsers";
const QByteArray s_viewRoles = "/Administration/ViewAdministration/ViewRoles";
const QByteArray s_editAdministration = "/Administration/EditAdministration";
const QByteArray s_editUser = "/Administration/EditAdministration/EditUser";
const QByteArray s_changeUser = "/Administration/EditAdministration/EditUser/ChangeUser";
const QByteArray s_removeUser = "/Administration/EditAdministration/EditUser/RemoveUser";
const QByteArray s_changeRole = "/Administration/EditAdministration/EditRole/ChangeRole";
const QByteArray s_viewDeviceTickets = "/Devices/ViewDeviceTickets";
const QByteArray s_editDevice = "/Devices/EditDevice";
const QByteArray s_changeDevice = "/Devices/EditDevice/ChangeDevice";
const QByteArray s_viewTickets = "/Support/ViewTickets";
const QByteArray s_deviceRevisions = "/Devices/Revisions";
const QByteArray s_viewDeviceRevisions = "/Devices/Revisions/View";
const QByteArray s_restoreDeviceRevision = "/Devices/Revisions/Restore";
const QByteArray s_deleteDeviceRevision = "/Devices/Revisions/Delete";
const QByteArray s_ticketRevisions = "/Support/Revisions";
const QByteArray s_viewTicketRevisions = "/Support/Revisions/View";

const QByteArray s_usersList = "UsersList";
const QByteArray s_rolesList = "RolesList";
const QByteArray s_userUpdate = "UserUpdate";
const QByteArray s_userReplace = "UserReplace";
const QByteArray s_roleUpdate = "RoleUpdate";
const QByteArray s_ticketsList = "TicketsList";
const QByteArray s_deviceTicketsList = "DeviceTicketsList";
const QByteArray s_supportTicketsList = "SupportTicketsList";
const QByteArray s_serverVersion = "GetServerVersion";
const QByteArray s_legacyCommand = "LegacyCommand";

const QByteArray s_revisionList = "GetRevisionInfoList";
const QByteArray s_restoreRevision = "RestoreRevision";
const QByteArray s_deleteRevision = "DeleteRevision";
const QByteArray s_devicesCollection = "Devices";
const QByteArray s_ticketsCollection = "Tickets";


/**
	Commands and elements of the user interface with the permissions they are declared with.
*/
class CDeclaredPermissions: virtual public imtserverapp::ICommandPermissionsProvider
{
public:
	CDeclaredPermissions()
	{
		m_permissions.insert(s_usersList, {s_viewUsers});
		m_permissions.insert(s_rolesList, {s_viewRoles});
		m_permissions.insert(s_userUpdate, {s_changeUser});
		m_permissions.insert(s_userReplace, {s_changeUser, s_removeUser});
		m_permissions.insert(s_roleUpdate, {s_changeRole});
		m_permissions.insert(s_ticketsList, {s_viewDeviceTickets, s_viewTickets});
		m_permissions.insert(s_deviceTicketsList, {s_viewDeviceTickets});
		m_permissions.insert(s_supportTicketsList, {s_viewTickets});
		m_permissions.insert(s_serverVersion, QByteArrayList());
		m_permissions.insert(s_legacyCommand, {QByteArrayLiteral("ChangeUser")});
		m_permissions.insert("UsersPage", {s_viewUsers, s_editUser});
		m_permissions.insert("DeviceTicketsPanel", {s_viewDeviceTickets});
	}

	// reimplemented (imtserverapp::ICommandPermissionsProvider)
	virtual LogicalCheckMode GetCommandPermissionCheckMode() const override
	{
		return LCM_AND;
	}

	virtual QByteArrayList GetCommandIds() const override
	{
		return m_permissions.keys();
	}

	virtual QByteArrayList GetCommandPermissions(const QByteArray& commandId) const override
	{
		return m_permissions.value(commandId);
	}

private:
	QHash<QByteArray, QByteArrayList> m_permissions;
};


class CSuperuserInfo: public imtauth::CUserInfo
{
public:
	virtual bool IsAdmin() const override
	{
		return true;
	}

	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override
	{
		istd::IChangeableUniquePtr clonePtr(new CSuperuserInfo);
		if (clonePtr->CopyFrom(*this, mode)){
			return clonePtr;
		}

		return nullptr;
	}
};


void FillFeature(imtlic::CFeatureInfo& featureInfo, const QByteArray& path, const Requirements& requirements)
{
	featureInfo.SetFeatureId(path.mid(path.lastIndexOf('/') + 1));
	featureInfo.SetIsPermission(true);
	featureInfo.SetRequirements(requirements.value(path));
}


imtlic::IFeatureInfo::FeatureInfoPtr CreateFeature(const QByteArray& path, const Requirements& requirements, const QByteArrayList& subPaths = QByteArrayList())
{
	imtlic::CFeatureInfo* featureInfoPtr = new imtlic::CFeatureInfo();
	FillFeature(*featureInfoPtr, path, requirements);

	for (const QByteArray& subPath : subPaths){
		featureInfoPtr->InsertSubFeature(CreateFeature(subPath, requirements));
	}

	return imtlic::IFeatureInfo::FeatureInfoPtr(featureInfoPtr);
}


void AddRootFeature(imtlic::CProductInfo& productInfo, const QByteArray& path, const Requirements& requirements, const QList<imtlic::IFeatureInfo::FeatureInfoPtr>& subFeatures)
{
	imtlic::CIdentifiableFeatureInfo featureInfo;
	FillFeature(featureInfo, path, requirements);
	for (const imtlic::IFeatureInfo::FeatureInfoPtr& subFeaturePtr : subFeatures){
		featureInfo.InsertSubFeature(subFeaturePtr);
	}

	QVERIFY(productInfo.AddFeature(featureInfo.GetFeatureId(), featureInfo));
}


/**
	/Administration
	  ViewAdministration: ViewUsers, ViewRoles
	  EditAdministration
	    EditUser: ChangeUser, RemoveUser
	    EditRole: ChangeRole
	/Devices
	  ViewDeviceTickets
	  EditDevice: ChangeDevice
	  Revisions: View, Restore, Delete
	/Support
	  ViewTickets
	  Revisions: View, Restore, Delete
*/
void SetupProduct(imtlic::CProductInfo& productInfo, const Requirements& requirements)
{
	productInfo.SetProductId(s_productId);

	imtlic::IFeatureInfo::FeatureInfoPtr editAdministrationPtr = CreateFeature(s_editAdministration, requirements);
	editAdministrationPtr->InsertSubFeature(CreateFeature(s_editUser, requirements, {s_changeUser, s_removeUser}));
	editAdministrationPtr->InsertSubFeature(CreateFeature("/Administration/EditAdministration/EditRole", requirements, {s_changeRole}));

	AddRootFeature(productInfo, "/Administration", requirements, {
				CreateFeature(s_viewAdministration, requirements, {s_viewUsers, s_viewRoles}),
				editAdministrationPtr});
	AddRootFeature(productInfo, "/Devices", requirements, {
				CreateFeature(s_viewDeviceTickets, requirements),
				CreateFeature(s_editDevice, requirements, {s_changeDevice}),
				CreateFeature(s_deviceRevisions, requirements, {s_viewDeviceRevisions, s_restoreDeviceRevision, s_deleteDeviceRevision})});
	AddRootFeature(productInfo, "/Support", requirements, {
				CreateFeature(s_viewTickets, requirements),
				CreateFeature(s_ticketRevisions, requirements, {s_viewTicketRevisions, s_ticketRevisions + "/Restore", s_ticketRevisions + "/Delete"})});
}


void SetPermissions(imtauth::CUserInfo& userInfo, const QByteArrayList& permissions, const QByteArray& productId = s_productId)
{
	userInfo.SetLocalPermissions(productId, permissions);
}


/**
	The server side of one request: the product it runs against and the declared permissions.
*/
class CServer
{
public:
	explicit CServer(const Requirements& requirements = Requirements())
	{
		SetupProduct(m_productInfo, requirements);
	}


	void SetupRequest(
				imtgql::CGqlRequest& request,
				const QByteArray& commandId,
				const imtauth::IUserInfo* userInfoPtr,
				const QByteArray& permissionPath = QByteArray(),
				const QByteArrayList& tenantPermissions = QByteArrayList(),
				bool isTenantOwner = false,
				bool inTenant = false)
	{
		imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
		contextPtr->SetProductId(s_productId);
		contextPtr->SetProductInfo(&m_productInfo);
		contextPtr->SetUserInfo(userInfoPtr);
		// A permission scope exists only inside an organization
		if (inTenant || isTenantOwner || !tenantPermissions.isEmpty()){
			contextPtr->SetTenantId(s_tenantId);
		}
		contextPtr->SetTenantPermissions(tenantPermissions);
		contextPtr->SetIsTenantOwner(isTenantOwner);
		if (!permissionPath.isEmpty()){
			imtgql::IGqlContext::Headers headers;
			headers.insert(imtbase::s_permissionPathHeaderId.toLower(), permissionPath);
			contextPtr->SetHeaders(headers);
		}

		request.SetCommandId(commandId);
		request.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));
	}

	bool IsAllowed(
				const QByteArray& commandId,
				const imtauth::IUserInfo* userInfoPtr,
				const QByteArray& permissionPath = QByteArray(),
				const QByteArrayList& tenantPermissions = QByteArrayList(),
				bool isTenantOwner = false,
				bool inTenant = false)
	{
		imtgql::CGqlRequest request;
		SetupRequest(request, commandId, userInfoPtr, permissionPath, tenantPermissions, isTenantOwner, inTenant);

		m_refusalReason.clear();

		return imtservergql::CheckRequestPermissions(request, &m_declaredPermissions, &m_allChecker, true, m_refusalReason);
	}

	bool IsRevisionCommandAllowed(
				const QByteArray& commandId,
				const QByteArray& collectionId,
				const imtauth::IUserInfo* userInfoPtr,
				const QByteArrayList& tenantPermissions = QByteArrayList())
	{
		imtgql::CGqlRequest request;
		SetupRequest(request, commandId, userInfoPtr, QByteArray(), tenantPermissions);
		imtgql::CGqlParamObject input;
		input.InsertParam("collectionId", QVariant(collectionId));
		request.AddParam("input", input);

		m_refusalReason.clear();

		return imtservergql::CheckRevisionPermissions(
					request,
					{s_devicesCollection, s_ticketsCollection},
					{s_deviceRevisions, s_ticketRevisions},
					m_refusalReason);
	}

	QString GetRefusalReason() const
	{
		return m_refusalReason;
	}

	bool IsElementShown(const QByteArray& elementId, const QByteArrayList& userPermissions, const QByteArray& permissionPath = QByteArray())
	{
		return imtserverapp::IsElementAccessible(&m_declaredPermissions, &m_anyChecker, elementId, userPermissions, false, permissionPath);
	}

private:
	imtlic::CProductInfo m_productInfo;
	CDeclaredPermissions m_declaredPermissions;
	imtauth::CCheckPermissionCompBase m_allChecker;
	imtauth::CCheckPermissionOrComp m_anyChecker;
	QString m_refusalReason;
};


} // anonymous namespace


void CPermissionCheckTest::testRequestWithoutUserIsRefused()
{
	CServer server;

	QVERIFY(!server.IsAllowed(s_serverVersion, nullptr));
}


void CPermissionCheckTest::testSuperuserIsNotChecked()
{
	CServer server;
	CSuperuserInfo superuser;

	QVERIFY(server.IsAllowed(s_userUpdate, &superuser));
	QVERIFY(server.IsAllowed("NotDeclaredCommand", &superuser));
}


void CPermissionCheckTest::testTenantOwnerIsChecked()
{
	CServer server;
	imtauth::CUserInfo owner;
	SetPermissions(owner, QByteArrayList());

	// The owner holds the scope as permissions of its context, nothing lets it through
	QVERIFY(!server.IsAllowed(s_userUpdate, &owner, QByteArray(), {s_changeUser}, true));

	SetPermissions(owner, {s_changeUser});
	QVERIFY(server.IsAllowed(s_userUpdate, &owner, QByteArray(), {s_changeUser}, true));
}


void CPermissionCheckTest::testUndeclaredCommandIsRefused()
{
	CServer server;
	imtauth::CUserInfo user;
	SetPermissions(user, {s_viewUsers, s_changeUser});

	QVERIFY(!server.IsAllowed("NotDeclaredCommand", &user));
}


void CPermissionCheckTest::testCommandWithoutPermissionsIsOpen()
{
	CServer server;
	imtauth::CUserInfo user;
	SetPermissions(user, QByteArrayList());

	QVERIFY(server.IsAllowed(s_serverVersion, &user));
}


void CPermissionCheckTest::testUserWithoutPermissionIsRefused()
{
	CServer server;
	imtauth::CUserInfo user;
	SetPermissions(user, {s_viewRoles});

	QVERIFY(server.IsAllowed(s_rolesList, &user));
	QVERIFY(server.GetRefusalReason().isEmpty());

	QVERIFY(!server.IsAllowed(s_usersList, &user));
	QVERIFY(server.GetRefusalReason().contains(QString::fromUtf8(s_viewUsers)));

	QVERIFY(!server.IsAllowed(s_roleUpdate, &user));
}


void CPermissionCheckTest::testAllPermissionsOfCommandAreRequired()
{
	CServer server;
	imtauth::CUserInfo editor;
	SetPermissions(editor, {s_changeUser});
	imtauth::CUserInfo administrator;
	SetPermissions(administrator, {s_changeUser, s_removeUser});

	QVERIFY(!server.IsAllowed(s_userReplace, &editor));
	QVERIFY(server.IsAllowed(s_userReplace, &administrator));
}


void CPermissionCheckTest::testLeafRequiresLeaf()
{
	CServer server({{s_changeUser, {s_viewUsers}}});
	imtauth::CUserInfo editor;
	SetPermissions(editor, {s_changeUser});
	imtauth::CUserInfo viewer;
	SetPermissions(viewer, {s_viewUsers});

	QVERIFY(server.IsAllowed(s_userUpdate, &editor));
	QVERIFY(server.IsAllowed(s_usersList, &editor));
	QVERIFY(!server.IsAllowed(s_rolesList, &editor));

	QVERIFY(server.IsAllowed(s_usersList, &viewer));
	QVERIFY(!server.IsAllowed(s_userUpdate, &viewer));
}


void CPermissionCheckTest::testGroupRequiresLeaf()
{
	CServer server({{s_editUser, {s_viewUsers}}});
	imtauth::CUserInfo remover;
	SetPermissions(remover, {s_removeUser});
	imtauth::CUserInfo roleEditor;
	SetPermissions(roleEditor, {s_changeRole});

	QVERIFY(server.IsAllowed(s_usersList, &remover));
	QVERIFY(!server.IsAllowed(s_userUpdate, &remover));
	QVERIFY(!server.IsAllowed(s_usersList, &roleEditor));
}


void CPermissionCheckTest::testLeafRequiresGroup()
{
	CServer server({{s_changeUser, {s_viewAdministration}}});
	imtauth::CUserInfo editor;
	SetPermissions(editor, {s_changeUser});
	imtauth::CUserInfo viewer;
	SetPermissions(viewer, {s_viewRoles});

	QVERIFY(server.IsAllowed(s_usersList, &editor));
	QVERIFY(server.IsAllowed(s_rolesList, &editor));
	QVERIFY(!server.IsAllowed(s_roleUpdate, &editor));
	QVERIFY(!server.IsAllowed(s_userUpdate, &viewer));
}


void CPermissionCheckTest::testGroupRequiresGroup()
{
	CServer server({{s_editAdministration, {s_viewAdministration}}});
	imtauth::CUserInfo roleEditor;
	SetPermissions(roleEditor, {s_changeRole});
	imtauth::CUserInfo viewer;
	SetPermissions(viewer, {s_viewUsers});

	QVERIFY(server.IsAllowed(s_usersList, &roleEditor));
	QVERIFY(server.IsAllowed(s_rolesList, &roleEditor));
	QVERIFY(!server.IsAllowed(s_userUpdate, &roleEditor));
	QVERIFY(!server.IsAllowed(s_roleUpdate, &viewer));
}


void CPermissionCheckTest::testRequirementsAreTransitive()
{
	CServer server({
				{s_changeUser, {s_viewUsers}},
				{s_viewUsers, {s_viewDeviceTickets}}});
	imtauth::CUserInfo editor;
	SetPermissions(editor, {s_changeUser});

	QVERIFY(server.IsAllowed(s_deviceTicketsList, &editor));
	QVERIFY(!server.IsAllowed(s_rolesList, &editor));
}


void CPermissionCheckTest::testRequirementsChainThroughGroup()
{
	// ChangeUser includes ViewUsers, which brings what its group ViewAdministration requires.
	CServer server({
				{s_editAdministration, {s_viewAdministration}},
				{s_viewAdministration, {s_viewDeviceTickets}}});
	imtauth::CUserInfo editor;
	SetPermissions(editor, {s_changeUser});

	QVERIFY(server.IsAllowed(s_usersList, &editor));
	QVERIFY(server.IsAllowed(s_deviceTicketsList, &editor));
	QVERIFY(!server.IsAllowed(s_supportTicketsList, &editor));
}


void CPermissionCheckTest::testRequirementCycleIncludesBothWays()
{
	CServer server({
				{s_changeUser, {s_viewUsers}},
				{s_viewUsers, {s_changeUser}}});
	imtauth::CUserInfo viewer;
	SetPermissions(viewer, {s_viewUsers});

	QVERIFY(server.IsAllowed(s_userUpdate, &viewer));
	QVERIFY(!server.IsAllowed(s_userReplace, &viewer));
}


void CPermissionCheckTest::testGroupAndOwnRequirementsCombine()
{
	CServer server({
				{s_editUser, {s_viewUsers}},
				{s_changeUser, {s_viewRoles}}});
	imtauth::CUserInfo editor;
	SetPermissions(editor, {s_changeUser});
	imtauth::CUserInfo remover;
	SetPermissions(remover, {s_removeUser});

	QVERIFY(server.IsAllowed(s_usersList, &editor));
	QVERIFY(server.IsAllowed(s_rolesList, &editor));
	QVERIFY(server.IsAllowed(s_usersList, &remover));
	QVERIFY(!server.IsAllowed(s_rolesList, &remover));
}


void CPermissionCheckTest::testRequirementOnOwnBranchIsIgnored()
{
	// Self, the own group, an ancestor and descendants: each would hand out siblings.
	CServer server({
				{s_changeUser, {s_changeUser, s_editUser, "/Administration"}},
				{s_editUser, {s_changeUser}},
				{s_editAdministration, {s_editUser}}});
	imtauth::CUserInfo editor;
	SetPermissions(editor, {s_changeUser});
	imtauth::CUserInfo roleEditor;
	SetPermissions(roleEditor, {s_changeRole});

	QVERIFY(server.IsAllowed(s_userUpdate, &editor));
	QVERIFY(!server.IsAllowed(s_userReplace, &editor));
	QVERIFY(!server.IsAllowed(s_usersList, &editor));
	QVERIFY(!server.IsAllowed(s_userUpdate, &roleEditor));
}


void CPermissionCheckTest::testUnknownRequirementTargetIsIgnored()
{
	CServer server({{s_changeUser, {"/Administration/NoSuchPermission", "ViewUsers"}}});
	imtauth::CUserInfo editor;
	SetPermissions(editor, {s_changeUser});

	QVERIFY(server.IsAllowed(s_userUpdate, &editor));
	QVERIFY(!server.IsAllowed(s_usersList, &editor));
}


void CPermissionCheckTest::testBarePermissionOfUserOpensNothing()
{
	CServer server({{s_editUser, {s_viewUsers}}});
	imtauth::CUserInfo editor;
	SetPermissions(editor, {QByteArrayLiteral("ChangeUser")});

	QVERIFY(!server.IsAllowed(s_userUpdate, &editor));
	QVERIFY(!server.IsAllowed(s_usersList, &editor));
	QVERIFY(!server.IsAllowed(s_legacyCommand, &editor));
}


void CPermissionCheckTest::testBareDeclarationOfCommandIsHeldByNobody()
{
	CServer server;
	imtauth::CUserInfo editor;
	SetPermissions(editor, {s_changeUser});
	CSuperuserInfo superuser;

	QVERIFY(server.IsAllowed(s_userUpdate, &editor));
	QVERIFY(!server.IsAllowed(s_legacyCommand, &editor));
	QVERIFY(server.IsAllowed(s_legacyCommand, &superuser));
}


void CPermissionCheckTest::testPermissionsOfAnotherProductDoNotOpenCommands()
{
	CServer server({{s_editUser, {s_viewUsers}}});
	imtauth::CUserInfo user;
	SetPermissions(user, {s_changeUser}, "Lisa");

	QVERIFY(!server.IsAllowed(s_userUpdate, &user));
	QVERIFY(!server.IsAllowed(s_usersList, &user));
}


void CPermissionCheckTest::testTenantLimitsIncludedPermissions()
{
	CServer server({{s_editUser, {s_viewUsers}}});
	imtauth::CUserInfo user;
	SetPermissions(user, {s_changeUser});

	QVERIFY(server.IsAllowed(s_usersList, &user, QByteArray(), {s_changeUser, s_viewUsers}));
	QVERIFY(!server.IsAllowed(s_usersList, &user, QByteArray(), {s_changeUser}));
	QVERIFY(server.IsAllowed(s_userUpdate, &user, QByteArray(), {s_changeUser}));
}


void CPermissionCheckTest::testEmptyTenantScopeIncludesNothing()
{
	CServer server({{s_editUser, {s_viewUsers}}});
	imtauth::CUserInfo user;
	SetPermissions(user, {s_changeUser});

	QVERIFY(server.IsAllowed(s_usersList, &user));
	QVERIFY(!server.IsAllowed(s_usersList, &user, QByteArray(), QByteArrayList(), false, true));
}


void CPermissionCheckTest::testTenantLimitsLeavesOfRequiredGroup()
{
	CServer server({{s_changeUser, {s_viewAdministration}}});
	imtauth::CUserInfo editor;
	SetPermissions(editor, {s_changeUser});

	QVERIFY(server.IsAllowed(s_rolesList, &editor, QByteArray(), {s_changeUser, s_viewRoles}));
	QVERIFY(!server.IsAllowed(s_usersList, &editor, QByteArray(), {s_changeUser, s_viewRoles}));
}


void CPermissionCheckTest::testPermissionPathEqualToDeclaredIsAccepted()
{
	CServer server;
	imtauth::CUserInfo user;
	SetPermissions(user, {s_viewDeviceTickets});

	QVERIFY(server.IsAllowed(s_ticketsList, &user, s_viewDeviceTickets));
}


void CPermissionCheckTest::testPermissionPathInsideDeclaredIsAccepted()
{
	CServer server;
	imtauth::CUserInfo user;
	SetPermissions(user, {s_viewDeviceTickets + "/Assigned"});

	QVERIFY(server.IsAllowed(s_ticketsList, &user, s_viewDeviceTickets + "/Assigned"));
}


void CPermissionCheckTest::testPermissionPathWiderThanDeclaredIsRefused()
{
	CServer server;
	imtauth::CUserInfo user;
	SetPermissions(user, {s_changeDevice});

	// Any leaf of "/Devices" would do otherwise, not the declared one.
	QVERIFY(!server.IsAllowed(s_ticketsList, &user, QByteArrayLiteral("/Devices")));
	QVERIFY(server.GetRefusalReason().contains(QStringLiteral("permission path '/Devices'")));
}


void CPermissionCheckTest::testPermissionPathNotDeclaredIsRefused()
{
	CServer server;
	imtauth::CUserInfo user;
	SetPermissions(user, {s_viewUsers, s_viewDeviceTickets});

	QVERIFY(!server.IsAllowed(s_ticketsList, &user, s_viewUsers));
}


void CPermissionCheckTest::testPermissionPathMustBeHeldByUser()
{
	CServer server;
	imtauth::CUserInfo supporter;
	SetPermissions(supporter, {s_viewTickets});

	// Without a path the command needs all it is declared with; the path picks the context.
	QVERIFY(!server.IsAllowed(s_ticketsList, &supporter));
	QVERIFY(server.IsAllowed(s_ticketsList, &supporter, s_viewTickets));
	QVERIFY(!server.IsAllowed(s_ticketsList, &supporter, s_viewDeviceTickets));
}


void CPermissionCheckTest::testPermissionPathIsIncludedByHeldPermission()
{
	CServer server({{s_editDevice, {s_viewDeviceTickets}}});
	imtauth::CUserInfo user;
	SetPermissions(user, {s_changeDevice});

	QVERIFY(server.IsAllowed(s_ticketsList, &user, s_viewDeviceTickets));
}


void CPermissionCheckTest::testRevisionPermissionOpensOnlyItsCollection()
{
	CServer server;
	imtauth::CUserInfo user;
	SetPermissions(user, {s_viewDeviceRevisions, s_viewTickets});

	QVERIFY(server.IsRevisionCommandAllowed(s_revisionList, s_devicesCollection, &user));
	QVERIFY(!server.IsRevisionCommandAllowed(s_revisionList, s_ticketsCollection, &user));
	QVERIFY(server.GetRefusalReason().contains(QString::fromUtf8(s_viewTicketRevisions)));
}


void CPermissionCheckTest::testRevisionActionNeedsItsOwnLeaf()
{
	CServer server;
	imtauth::CUserInfo viewer;
	SetPermissions(viewer, {s_viewDeviceRevisions});
	imtauth::CUserInfo restorer;
	SetPermissions(restorer, {s_viewDeviceRevisions, s_restoreDeviceRevision});
	imtauth::CUserInfo groupHolder;
	SetPermissions(groupHolder, {s_deviceRevisions});

	QVERIFY(!server.IsRevisionCommandAllowed(s_restoreRevision, s_devicesCollection, &viewer));
	QVERIFY(!server.IsRevisionCommandAllowed(s_deleteRevision, s_devicesCollection, &viewer));
	QVERIFY(server.IsRevisionCommandAllowed(s_restoreRevision, s_devicesCollection, &restorer));
	QVERIFY(!server.IsRevisionCommandAllowed(s_deleteRevision, s_devicesCollection, &restorer));

	// Users hold leaves only; a stored group path is not inside its own leaf.
	QVERIFY(!server.IsRevisionCommandAllowed(s_deleteRevision, s_devicesCollection, &groupHolder));
}


void CPermissionCheckTest::testRevisionPermissionIncludesItsRequirements()
{
	CServer server({
				{s_restoreDeviceRevision, {s_viewDeviceRevisions}},
				{s_viewDeviceRevisions, {s_viewDeviceTickets}}});
	imtauth::CUserInfo restorer;
	SetPermissions(restorer, {s_restoreDeviceRevision});

	QVERIFY(server.IsRevisionCommandAllowed(s_revisionList, s_devicesCollection, &restorer));
	QVERIFY(server.IsRevisionCommandAllowed(s_restoreRevision, s_devicesCollection, &restorer));
	QVERIFY(!server.IsRevisionCommandAllowed(s_deleteRevision, s_devicesCollection, &restorer));
	QVERIFY(server.IsAllowed(s_deviceTicketsList, &restorer));
}


void CPermissionCheckTest::testRevisionPermissionIsLimitedByTenant()
{
	CServer server({{s_restoreDeviceRevision, {s_viewDeviceRevisions}}});
	imtauth::CUserInfo restorer;
	SetPermissions(restorer, {s_restoreDeviceRevision});

	QVERIFY(server.IsRevisionCommandAllowed(s_revisionList, s_devicesCollection, &restorer, {s_restoreDeviceRevision, s_viewDeviceRevisions}));
	QVERIFY(!server.IsRevisionCommandAllowed(s_revisionList, s_devicesCollection, &restorer, {s_restoreDeviceRevision}));
}


void CPermissionCheckTest::testRevisionsOfUndeclaredCollectionAreRefused()
{
	CServer server;
	imtauth::CUserInfo user;
	SetPermissions(user, {s_viewDeviceRevisions, s_viewTicketRevisions});
	CSuperuserInfo superuser;

	QVERIFY(!server.IsRevisionCommandAllowed(s_revisionList, "Accounts", &user));
	QVERIFY(server.GetRefusalReason().contains(QStringLiteral("Accounts")));
	QVERIFY(server.IsRevisionCommandAllowed(s_revisionList, "Accounts", &superuser));
	QVERIFY(!server.IsRevisionCommandAllowed(s_revisionList, s_devicesCollection, nullptr));
}


void CPermissionCheckTest::testUnknownRevisionCommandIsRefused()
{
	CServer server;
	imtauth::CUserInfo user;
	SetPermissions(user, {s_viewDeviceRevisions, s_restoreDeviceRevision, s_deleteDeviceRevision});

	QVERIFY(!server.IsRevisionCommandAllowed("BackupRevision", s_devicesCollection, &user));
	QVERIFY(!server.IsRevisionCommandAllowed("ExportRevision", s_devicesCollection, &user));
}


void CPermissionCheckTest::testGuiElementOpensForAnyDeclaredPermission()
{
	CServer server;

	QVERIFY(server.IsElementShown("UsersPage", {s_viewUsers}));
	QVERIFY(server.IsElementShown("UsersPage", {s_removeUser}));
	QVERIFY(!server.IsElementShown("UsersPage", {s_viewRoles, s_changeRole}));
}


void CPermissionCheckTest::testGuiElementPermissionPathWiderThanDeclaredIsHidden()
{
	CServer server;

	QVERIFY(server.IsElementShown("DeviceTicketsPanel", {s_viewDeviceTickets}, s_viewDeviceTickets));
	QVERIFY(!server.IsElementShown("DeviceTicketsPanel", {s_changeDevice}, QByteArrayLiteral("/Devices")));
}


I_ADD_TEST(CPermissionCheckTest);


