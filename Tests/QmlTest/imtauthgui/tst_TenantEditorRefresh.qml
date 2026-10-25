import QtQuick 2.12
import QtTest 1.12
import imtdocgui 1.0
import imtauthgui 1.0
import imtauthTenantsSdl 1.0

TestCase {
	id: testCase

	name: "TenantEditorRefresh"
	when: windowShown

	// Counts the readings of the organization
	Component {
		id: controllerComp

		DocumentRepresentationController {
			property int readCount: 0

			function updateRepresentationFromDocument(){
				readCount++
			}
		}
	}

	Component {
		id: apiClientComp

		QtObject {
			property string rolePermissionsTenantId: ""

			signal subscriptionInvitationAccepted(string tenantId, string membershipId)
			signal subscriptionOwnershipTransferred(string tenantId)
			signal ownershipTransferred()
		}
	}

	Component {
		id: tenantDataComp

		TenantData {
			m_id: "tenant-id"
		}
	}

	Component {
		id: editorComp

		TenantEditor {
			width: 800
			height: 600
		}
	}

	function createEditor(apiClient, controller){
		let editor = createTemporaryObject(editorComp, testCase.parent, {
			"apiClient": apiClient,
			"representationController": controller,
			"model": createTemporaryObject(tenantDataComp, testCase)})
		wait(300)
		controller.readCount = 0

		return editor
	}

	// The API client relays the notification AuthorizationController receives: one reading serves both
	function test_aRelayedNotificationReadsOnce(){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		let controller = createTemporaryObject(controllerComp, testCase)
		createEditor(apiClient, controller)

		apiClient.subscriptionInvitationAccepted("tenant-id", "membership-id")
		AuthorizationController.tenantInvitationAccepted("tenant-id", "membership-id")

		tryCompare(controller, "readCount", 1, 3000)
		wait(300)
		compare(controller.readCount, 1)
	}

	// An own transfer of ownership and its notification read once
	function test_anOwnChangeAndItsNotificationReadOnce(){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		let controller = createTemporaryObject(controllerComp, testCase)
		createEditor(apiClient, controller)

		apiClient.ownershipTransferred()
		apiClient.subscriptionOwnershipTransferred("tenant-id")
		AuthorizationController.tenantOwnershipTransferred("tenant-id")

		tryCompare(controller, "readCount", 1, 3000)
		wait(300)
		compare(controller.readCount, 1)
	}

	function test_anotherOrganizationIsNotRead(){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		let controller = createTemporaryObject(controllerComp, testCase)
		createEditor(apiClient, controller)

		apiClient.subscriptionInvitationAccepted("other-id", "membership-id")
		wait(400)
		compare(controller.readCount, 0)
	}
}
