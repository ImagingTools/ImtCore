import QtQuick 2.12
import QtTest 1.12
import imtauthgui 1.0

TestCase {
	id: testCase

	name: "TenantPartnersPage"
	when: windowShown

	// Counts what the page reads; the models are plain lists with count and get
	Component {
		id: apiClientComp

		QtObject {
			property int requestReads: 0
			property int connectionReads: 0
			property var connectionRequestsModel: null
			property var connectionsModel: null

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

		TenantPartnersPage {
			width: 800
			height: 600
		}
	}

	function listOf(rows){
		return {"count": rows.length, "get": function(index){ return rows[index] }}
	}

	function createPage(apiClient){
		let page = createTemporaryObject(pageComp, testCase.parent, {"apiClient": apiClient, "model": {"m_id": "tenant-id"}})
		verify(page.visible, "The page is shown")
		wait(300)
		apiClient.requestReads = 0
		apiClient.connectionReads = 0

		return page
	}

	function createFilledClient(){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		apiClient.connectionRequestsModel = listOf([
			{"id": "out", "status": "Pending", "sourceTenantId": "tenant-id", "sourceTenantName": "Me", "targetTenantId": "y", "targetTenantName": "Y",
				"message": "", "createdAt": ""},
			{"id": "in", "status": "Pending", "sourceTenantId": "x", "sourceTenantName": "X", "targetTenantId": "tenant-id", "targetTenantName": "Me",
				"message": "Let us work together", "createdAt": ""},
			{"id": "old", "status": "Rejected", "sourceTenantId": "z", "sourceTenantName": "Z", "targetTenantId": "tenant-id", "targetTenantName": "Me",
				"message": "", "createdAt": ""}
		])
		apiClient.connectionsModel = listOf([
			{"id": "c1", "status": "Active", "partnerId": "p1", "partnerName": "P1"},
			{"id": "c3", "status": "Removed", "partnerId": "p3", "partnerName": "P3"}
		])
		return apiClient
	}

	function rowById(rows, rowId){
		for (let i = 0; i < rows.length; ++i){
			if (rows[i].id === rowId)
				return rows[i]
		}
		return null
	}

	// The answers wanted from this organization come first; answered requests and removed partners are not rows
	function test_theAnswersWantedComeFirst(){
		let page = createPage(createFilledClient())
		let rows = page.buildRows()

		compare(rows.length, 3, "Only the pending requests and the active connections are rows")
		compare(rows[0].kind, "incomingRequest")
		compare(rows[0].partner, "X")
		compare(rows[0].message, "Let us work together")
		verify(rowById(rows, "request:old") === null, "An answered request is not a row")
		verify(rowById(rows, "connection:c3") === null, "A removed connection is not a row")

		let outgoing = rowById(rows, "request:out")
		compare(outgoing.kind, "outgoingRequest")
		compare(outgoing.partner, "Y")

		let partner = rowById(rows, "connection:c1")
		compare(partner.kind, "partner")
		compare(partner.partnerId, "p1")
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
