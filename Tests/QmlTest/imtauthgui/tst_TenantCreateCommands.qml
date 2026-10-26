import QtQuick 2.12
import QtTest 1.12
import imtdocgui 1.0
import imtauthgui 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "TenantCreateCommands"
	when: windowShown

	Component {
		id: stateManagerComp

		TenantEditorStateManager {
		}
	}

	// A user who may create items in the organization
	Component {
		id: creatorStateComp

		QtObject {
			property bool canManageMembers: true

			function hasAnyPermission(permissionIds){ return true }
		}
	}

	Component {
		id: pageComp

		SimpleCollectionPage {
			width: 800
			height: 600
			entityName: "Role"
			descriptionText: "Roles of the organization."
			createPermissionIds: ["/OrganizationManagement/Roles/Edit/Add"]
			createUnavailableText: "Needs Administration."
		}
	}

	// Within an organization the server keeps a product permission only if the scope holds that exact permission
	function test_scopeHoldsOnlyTheExactPermission(){
		let stateManager = createTemporaryObject(stateManagerComp, testCase)
		stateManager.tenantData = {"m_tenantPermissions": ["/Administration/Roles/Edit/Add", "/Administration"]}

		verify(stateManager.scopeHolds("/Administration/Roles/Edit/Add"))
		verify(!stateManager.scopeHolds("/Administration/Users/Edit/Add"), "An ancestor in the scope holds no descendant")
		verify(!stateManager.scopeHolds("/Administration/Roles"), "A descendant in the scope holds no ancestor")

		stateManager.tenantData = {"m_tenantPermissions": []}
		verify(!stateManager.scopeHolds("/Administration/Roles/Edit/Add"), "An empty scope holds nothing")

		stateManager.tenantData = null
		verify(!stateManager.scopeHolds("/Administration/Roles/Edit/Add"))
	}

	function test_createGivesWayToTheExplanation(){
		let page = createTemporaryObject(pageComp, testCase, {"stateManager": createTemporaryObject(creatorStateComp, testCase)})

		verify(page.canCreate)
		compare(page.__descriptionText, "Roles of the organization.")

		page.createAllowed = false
		verify(page.__createUnavailable)
		compare(page.__descriptionText, "Roles of the organization. Needs Administration.")
	}

	function test_noExplanationWithoutTheRightToCreate(){
		let page = createTemporaryObject(pageComp, testCase)

		page.createAllowed = false
		verify(!page.canCreate)
		verify(!page.__createUnavailable, "Who may not create is told nothing about it")
		compare(page.__descriptionText, "Roles of the organization.")
	}
}
