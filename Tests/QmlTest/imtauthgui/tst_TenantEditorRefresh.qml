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
			property QtObject userDocumentManager: QtObject {
				signal documentSaved(string documentId)
			}
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

	// The server tells it to every session of the organization, also when it asks them again after a reconnect
	function test_aBurstOfNotificationsReadsOnce(){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		let controller = createTemporaryObject(controllerComp, testCase)
		createEditor(apiClient, controller)

		AuthorizationController.tenantMembersChanged("tenant-id")
		AuthorizationController.tenantMembersChanged("tenant-id")

		tryCompare(controller, "readCount", 1, 3000)
		wait(300)
		compare(controller.readCount, 1)
	}

	// An own change of a member and its notification read once
	function test_anOwnChangeAndItsNotificationReadOnce(){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		let controller = createTemporaryObject(controllerComp, testCase)
		createEditor(apiClient, controller)

		apiClient.userDocumentManager.documentSaved("document-id")
		AuthorizationController.tenantMembersChanged("tenant-id")

		tryCompare(controller, "readCount", 1, 3000)
		wait(300)
		compare(controller.readCount, 1)
	}

	function test_anotherOrganizationIsNotRead(){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		let controller = createTemporaryObject(controllerComp, testCase)
		createEditor(apiClient, controller)

		AuthorizationController.tenantMembersChanged("other-id")
		wait(400)
		compare(controller.readCount, 0)
	}
}
