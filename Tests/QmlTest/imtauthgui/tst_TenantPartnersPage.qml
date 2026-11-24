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
			property int relationshipReads: 0
			property int proposalReads: 0
			property var connectionRequestsModel: null
			property var connectionsModel: null
			property var tenantRelationshipsModel: null
			property var relationshipProposalsModel: null

			signal connectionRequestCreated(string requestId)
			signal connectionRequestError(string errorMessage)
			signal requestFailed(string message)
			signal connectionRequestsReceived()
			signal connectionRequestApproved(string connectionId)
			signal connectionRequestRejected()
			signal connectionRequestCanceled()
			signal connectionsReceived()
			signal tenantRelationshipsReceived()
			signal relationshipProposalCreated()
			signal relationshipProposalApproved(string relationshipId)
			signal relationshipProposalRejected()
			signal relationshipProposalCanceled()
			signal relationshipProposalsReceived()
			signal subscriptionConnectionCodesChanged(var notification)
			signal subscriptionsResumed()
			signal connectionRemoved(string connectionId)

			function fetchConnectionRequests(tenantId){ requestReads++ }
			function fetchConnections(tenantId){ connectionReads++ }
			function fetchTenantRelationships(tenantId){ relationshipReads++ }
			function fetchRelationshipProposals(tenantId){ proposalReads++ }
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
		apiClient.relationshipReads = 0
		apiClient.proposalReads = 0

		return page
	}

	function createFilledClient(){
		let apiClient = createTemporaryObject(apiClientComp, testCase)
		apiClient.connectionRequestsModel = listOf([
			{"id": "in", "status": "Pending", "sourceTenantId": "x", "sourceTenantName": "X", "targetTenantId": "tenant-id", "targetTenantName": "Me",
				"message": "", "proposedSourceRole": "Customer", "proposedTargetRole": "Supplier", "proposedScope": "Devices"},
			{"id": "out", "status": "Pending", "sourceTenantId": "tenant-id", "sourceTenantName": "Me", "targetTenantId": "y", "targetTenantName": "Y",
				"message": "", "proposedSourceRole": "", "proposedTargetRole": "", "proposedScope": ""},
			{"id": "old", "status": "Rejected", "sourceTenantId": "z", "sourceTenantName": "Z", "targetTenantId": "tenant-id", "targetTenantName": "Me",
				"message": "", "proposedSourceRole": "", "proposedTargetRole": "", "proposedScope": ""}
		])
		apiClient.connectionsModel = listOf([
			{"id": "c1", "status": "Active", "partnerId": "p1", "partnerName": "P1"},
			{"id": "c2", "status": "Active", "partnerId": "p2", "partnerName": "P2"},
			{"id": "c3", "status": "Removed", "partnerId": "p3", "partnerName": "P3"}
		])
		apiClient.tenantRelationshipsModel = listOf([
			{"id": "r1", "connectionId": "c1", "status": "Active", "sourceTenantId": "p1", "targetTenantId": "tenant-id",
				"sourceRole": "Supplier", "targetRole": "Customer", "scope": ""}
		])
		apiClient.relationshipProposalsModel = listOf([
			{"id": "pr2", "connectionId": "c2", "status": "ApprovedByInitiator", "proposalType": "Create",
				"initiatorTenantId": "p2", "counterpartyTenantId": "tenant-id",
				"proposedSourceRole": "Parent", "proposedTargetRole": "Child", "proposedScope": ""}
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

	// The answers wanted from this organization come first: the incoming request and the new terms of a partner
	function test_theAnswersWantedComeFirst(){
		let page = createPage(createFilledClient())
		let rows = page.buildRows()

		compare(rows.length, 4, "Only the pending requests and the active connections are rows")
		compare(rows[0].kind, "incomingRequest")
		compare(rows[1].kind, "incomingProposal")
		compare(rows[1].proposalId, "pr2")
		verify(rowById(rows, "request:old") === null, "An answered request is not a row")
		verify(rowById(rows, "connection:c3") === null, "A removed connection is not a row")
	}

	// The terms of a request are read from the side of this organization: the requester's role is the partner's
	function test_theTermsAreSeenFromThisOrganization(){
		let page = createPage(createFilledClient())
		let rows = page.buildRows()

		let incoming = rowById(rows, "request:in")
		compare(incoming.partner, "X")
		compare(incoming.terms, page.termsText("Supplier", "Customer", "Devices"))

		let outgoing = rowById(rows, "request:out")
		compare(outgoing.kind, "outgoingRequest")
		compare(outgoing.partner, "Y")

		// This organization is the target of r1: its role is the target role
		let partner = rowById(rows, "connection:c1")
		compare(partner.kind, "partner")
		compare(partner.relationshipId, "r1")
		compare(partner.myRole, "Customer")
		compare(partner.partnerRole, "Supplier")
	}

	// New terms proposed by this organization wait for the partner and can be withdrawn
	function test_ownProposalWaitsForThePartner(){
		let apiClient = createFilledClient()
		apiClient.relationshipProposalsModel = listOf([
			{"id": "pr1", "connectionId": "c1", "status": "ApprovedByInitiator", "proposalType": "Update",
				"initiatorTenantId": "tenant-id", "counterpartyTenantId": "p1",
				"proposedSourceRole": "Partner", "proposedTargetRole": "Affiliate", "proposedScope": ""}
		])
		let page = createPage(apiClient)
		let row = rowById(page.buildRows(), "connection:c1")

		compare(row.kind, "outgoingProposal")
		compare(row.proposalId, "pr1")
		// p1 is the source of r1: the proposed target role is the one of this organization
		compare(row.terms, page.termsText("Affiliate", "Partner", ""))
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
		compare(apiClient.relationshipReads, 1)
		compare(apiClient.proposalReads, 1)
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
