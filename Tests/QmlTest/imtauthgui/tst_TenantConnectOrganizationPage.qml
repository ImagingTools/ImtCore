import QtQuick 2.12
import QtTest 1.12
import imtauthgui 1.0

TestCase {
	id: testCase

	name: "TenantConnectOrganizationPage"
	when: windowShown

	// Counts what the page reads
	Component {
		id: apiClientComp

		QtObject {
			property int requestReads: 0
			property int connectionReads: 0
			property var connectionRequests: []
			property var connections: []

			signal connectionRequestCreated(string requestId)
			signal connectionRequestError(string errorMessage)
			signal requestFailed(string message)
			signal connectionRequestsReceived()
			signal connectionRequestApproved(string connectionId)
			signal connectionRequestRejected()
			signal connectionRequestCanceled()
			signal connectionsReceived()
			signal subscriptionConnectionCodesChanged(var notification)
			signal subscriptionsResumed()
			signal connectionRemoved(string connectionId)

			function fetchConnectionRequests(tenantId){ requestReads++ }
			function fetchConnections(tenantId){ connectionReads++ }
		}
	}

	Component {
		id: pageComp

		TenantConnectOrganizationPage {
			width: 800
			height: 600
		}
	}

	function createPage(apiClient){
		let page = createTemporaryObject(pageComp, testCase.parent, {"apiClient": apiClient, "model": {"m_id": "tenant-id"}})
		verify(page.visible, "The page is shown")
		wait(300)
		apiClient.requestReads = 0
		apiClient.connectionReads = 0

		return page
	}

	// Notifications of the partner, e.g. an incoming request, show without a reload; a burst reads once
	function test_aBurstOfNotificationsReadsOnce(){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		let page = createPage(apiClient)

		apiClient.subscriptionConnectionCodesChanged({"notificationType": "ConnectionRequestReceived"})
		apiClient.subscriptionConnectionCodesChanged({"notificationType": "ConnectionRequestReceived"})
		apiClient.subscriptionsResumed()

		tryCompare(apiClient, "requestReads", 1, 3000)
		wait(300)
		compare(apiClient.requestReads, 1)
		compare(apiClient.connectionReads, 1)
	}

	// An own action and the notification it causes read once
	function test_anOwnActionAndItsNotificationReadOnce(){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		let page = createPage(apiClient)

		apiClient.connectionRemoved("connection-id")
		apiClient.subscriptionConnectionCodesChanged({"notificationType": "ConnectionRemoved"})

		tryCompare(apiClient, "connectionReads", 1, 3000)
		wait(300)
		compare(apiClient.connectionReads, 1)
	}

	// A hidden page reads when it is shown, once for every notification it missed
	function test_aHiddenPageReadsWhenShown(){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		let page = createPage(apiClient)

		page.visible = false
		apiClient.subscriptionConnectionCodesChanged({"notificationType": "ConnectionRequestApproved"})
		apiClient.subscriptionConnectionCodesChanged({"notificationType": "ConnectionRemoved"})
		wait(300)
		compare(apiClient.requestReads, 0)

		page.visible = true
		tryCompare(apiClient, "requestReads", 1, 3000)
		wait(300)
		compare(apiClient.requestReads, 1)
	}
}
