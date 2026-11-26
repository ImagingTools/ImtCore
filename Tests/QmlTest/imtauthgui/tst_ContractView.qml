import QtQuick 2.12
import QtTest 1.12
import imtauthgui 1.0

TestCase {
	id: testCase

	name: "ContractView"
	when: windowShown

	// Records what the card sends; the answers are given by the test
	Component {
		id: contractsClientComp

		QtObject {
			property var contracts: []
			property var contract: null
			property var privateInfo: null
			property var events: []
			property var partyCandidates: []
			property var partyRoles: [{"roleId": "supplier", "name": "Supplier", "isPartyRole": true, "isLinkRole": true},
				{"roleId": "customer", "name": "Customer", "isPartyRole": true, "isLinkRole": true}]
			property var contractTypes: [{"typeId": "star", "name": "Star", "linkScheme": "Star", "starRoleId": "supplier"}]
			property Component contractCommandsControllerComp: null
			property var lastDraft: null
			property string lastCommand: ""
			property int lastRevision: -1
			property int contractReads: 0

			signal contractsReceived()
			signal contractReceived(string contractId)
			signal eventsReceived(string contractId)
			signal partyCandidatesReceived()
			signal commandSucceeded(string command, string contractId, int revision)
			signal commandFailed(string command, string errorMessage)
			signal attachmentReceived(string attachmentId, string fileName, string mimeType, string data)
			signal contractNotified(string contractId, string tenantId, string sourceUserId, string sourceUserName)
			signal partyRolesReceived()
			signal contractTypesReceived()

			function fetchPartyRoles(tenantId, includeArchived){}
			function fetchContractTypes(tenantId, includeArchived){}
			function fetchPartyCandidates(tenantId){}
			function fetchContract(tenantId, contractId){ contractReads++ }
			function fetchEvents(tenantId, contractId){}
			function createContract(tenantId, draft){ lastCommand = "CreateContract"; lastDraft = draft }
			function updateContract(tenantId, contractId, revision, draft){ lastCommand = "UpdateContract"; lastDraft = draft; lastRevision = revision }
			function runCommand(command, tenantId, contractId, revision, comment){ lastCommand = command }
		}
	}

	Component {
		id: stateManagerComp

		QtObject {
			property bool canEditContracts: true
			property bool canApproveContracts: true
			property bool canSignContracts: true
			property bool canSuspendContractPermissions: true
			property bool canDistributeContractPermissions: true
		}
	}

	Component {
		id: viewComp

		ContractView {
			width: 900
			height: 700
		}
	}

	function init(){
		failOnWarning(/ReferenceError|TypeError/)
	}

	function createView(client, contractId){
		let view = createTemporaryObject(viewComp, testCase.parent, {"tenantId": "me", "tenantName": "Me", "contractId": contractId,
			"contractsApiClient": client, "stateManager": createTemporaryObject(stateManagerComp, testCase)})
		verify(view !== null, "The card is created")
		// The dialog starts the card once its parameters are set
		view.start()
		return view
	}

	// A new draft has the organization as its party, and every added party is linked with every other one
	function test_aNewDraftLinksEveryPair(){
		let client = createTemporaryObject(contractsClientComp, testCase)
		let view = createView(client, "")

		verify(view.isNew)
		verify(view.isEditable)
		compare(view.parties.length, 1)
		compare(view.parties[0].tenantId, "me")

		view.addParty("a", "A", "customer", "Customer")
		view.addParty("b", "B", "", "")
		compare(view.links.length, 3, "Three parties have three pairs")

		view.removeParty("b")
		compare(view.links.length, 1)
		compare(view.links[0].tenantBId, "a")
	}

	// A type with a star links its center with every other party only
	function test_aStarTypeLinksTheCenter(){
		let client = createTemporaryObject(contractsClientComp, testCase)
		let view = createView(client, "")

		view.addParty("a", "A", "supplier", "Supplier")
		view.addParty("b", "B", "customer", "Customer")
		view.applyType("star")

		compare(view.links.length, 2)
		for (let i = 0; i < view.links.length; i++)
			verify(view.links[i].tenantAId === "a" || view.links[i].tenantBId === "a", "Every link has the center")
	}

	// A pair keeps its saved link, so the permissions given within it stay
	function test_aSavedLinkIsKept(){
		let client = createTemporaryObject(contractsClientComp, testCase)
		let view = createView(client, "")

		view.addParty("a", "A", "", "")
		view.links = [{"linkId": "link-1", "tenantAId": "me", "tenantBId": "a", "isMessagingAllowed": false, "isSharingAllowed": true}]
		view.addParty("b", "B", "", "")

		let kept = view.__findLink("me", "a")
		compare(kept.linkId, "link-1")
		compare(kept.isMessagingAllowed, false)
	}

	// The draft sent has the parties with their roles and nothing the API does not take
	function test_saveSendsTheDraft(){
		let client = createTemporaryObject(contractsClientComp, testCase)
		let view = createView(client, "")

		view.addParty("a", "A", "customer", "Customer")
		view.save()
		compare(client.lastCommand, "", "A contract without a name is not sent")

		let draft = view.__buildDraft()
		compare(draft.parties.length, 2)
		compare(draft.parties[1].roleId, "customer")
		compare(Object.keys(draft.parties[1]).sort(), ["roleId", "tenantId"])
		compare(draft.links.length, 1)
		verify(!("signOrder" in draft) && !("signDeadline" in draft), "The draft has no signing order and no deadline")
	}

	// A party other than the initiator reads the terms; its steps follow its state
	function test_anInvitedPartyDoesNotEditTheTerms(){
		let client = createTemporaryObject(contractsClientComp, testCase)
		let view = createView(client, "c1")
		compare(client.contractReads, 1)

		client.contract = {"id": "c1", "name": "Supply", "initiatorTenantId": "other", "state": "Negotiation", "revision": 3,
			"submittedAt": "2027-01-01T00:00:00Z",
			"parties": [{"tenantId": "other", "tenantName": "Other", "state": "Reviewing"},
				{"tenantId": "me", "tenantName": "Me", "state": "Reviewing"}],
			"links": [{"linkId": "l1", "tenantAId": "other", "tenantBId": "me", "isInForce": false}]}
		client.contractReceived("c1")

		verify(!view.isNew)
		verify(!view.isInitiator)
		verify(!view.isEditable)
		verify(view.isNegotiated)
		compare(view.myPartyState, "Reviewing")
		compare(view.links.length, 1)

		view.run("ApproveContract", "")
		compare(client.lastCommand, "ApproveContract")
	}

	// A contract of the organization, sent to one partner
	function editableContract(revision){
		return {"id": "c1", "name": "Supply", "initiatorTenantId": "me", "state": "Negotiation", "revision": revision,
			"submittedAt": "2027-01-01T00:00:00Z", "termDays": 365, "terminationNoticeDays": 30,
			"parties": [{"tenantId": "me", "tenantName": "Me", "state": "Reviewing"},
				{"tenantId": "a", "tenantName": "A", "state": "Reviewing"}],
			"links": [{"linkId": "l1", "tenantAId": "me", "tenantBId": "a"}]}
	}

	function loadEditable(client, revision){
		let view = createView(client, "c1")
		client.contract = testCase.editableContract(revision)
		client.contractReceived("c1")
		wait(50)
		verify(view.isEditable)
		verify(!view.isDirty, "A loaded contract has no changes")
		return view
	}

	// The card knows its unsaved changes; a save sends the revision they are based on
	function test_anEditMakesTheCardDirty(){
		let client = createTemporaryObject(contractsClientComp, testCase)
		let view = testCase.loadEditable(client, 3)
		verify(!view.canSave, "Nothing to save")

		view.addParty("b", "B", "customer", "Customer")
		tryCompare(view, "isDirty", true)
		verify(view.canSave)

		view.save()
		compare(client.lastCommand, "UpdateContract")
		compare(client.lastRevision, 3)
	}

	// The others go on while the user edits: their steps show at once, the edits stay
	function test_aChangeOfTheOthersKeepsTheEdits(){
		let client = createTemporaryObject(contractsClientComp, testCase)
		let view = testCase.loadEditable(client, 3)
		view.addParty("b", "B", "customer", "Customer")
		tryCompare(view, "isDirty", true)

		client.contractNotified("c1", "me", "u2", "Ann")
		compare(client.contractReads, 2, "A change of the contract is read again")
		let approved = testCase.editableContract(3)
		approved.parties[1].state = "Approved"
		client.contract = approved
		client.contractReceived("c1")

		compare(view.contractParty("a").state, "Approved")
		compare(view.parties.length, 3, "The edits stay")
		verify(!view.hasConflict)
		verify(view.canSave)
	}

	// The terms changed under the edits: they cannot be saved until the contract is reloaded
	function test_aNewRevisionUnderTheEditsIsAConflict(){
		let client = createTemporaryObject(contractsClientComp, testCase)
		let view = testCase.loadEditable(client, 3)
		view.addParty("b", "B", "customer", "Customer")
		tryCompare(view, "isDirty", true)

		client.contract = testCase.editableContract(4)
		client.contractReceived("c1")
		verify(view.hasConflict)
		verify(!view.canSave)
		compare(view.parties.length, 3, "The edits are not dropped silently")

		view.discardChanges()
		compare(view.parties.length, 2)
		wait(50)
		verify(!view.isDirty)
		verify(!view.hasConflict)
	}

	// Without edits a change of the others replaces the copy at once
	function test_aChangeWithoutEditsIsShown(){
		let client = createTemporaryObject(contractsClientComp, testCase)
		let view = testCase.loadEditable(client, 3)

		let renamed = testCase.editableContract(4)
		renamed.name = "Supply 2"
		client.contract = renamed
		client.contractReceived("c1")
		compare(view.draft.name, "Supply 2")
		wait(50)
		verify(!view.isDirty)
	}

	// The answer of another card does not touch this one
	function test_anotherContractIsIgnored(){
		let client = createTemporaryObject(contractsClientComp, testCase)
		let view = createView(client, "c1")

		client.contract = {"id": "c2", "name": "Other", "initiatorTenantId": "me", "state": "Draft", "parties": []}
		client.contractReceived("c2")

		verify(view.contract === null)
	}

	function test_theComponentsCompile_data(){
		return [
			{"tag": "ContractPartyDialog"}, {"tag": "ContractReasonDialog"}, {"tag": "ContractSignDialog"},
			{"tag": "ContractAllocationDialog"}, {"tag": "ContractReferenceDialog"}, {"tag": "ContractRoleSlotDialog"},
			{"tag": "PartnerInviteDialog"}, {"tag": "TenantContractsPage"}, {"tag": "ChoiceModel"},
			{"tag": "ContractTermsPage"}, {"tag": "ContractPeriodPage"}, {"tag": "ContractPartiesPage"},
			{"tag": "ContractLinksPage"}, {"tag": "ContractPermissionsPage"}, {"tag": "ContractReferencesPage"},
			{"tag": "ContractDocumentsPage"}, {"tag": "ContractJournalPage"}, {"tag": "ContractStateBar"},
			{"tag": "TenantPartyRolesPage"}, {"tag": "PartyRoleView"}, {"tag": "TenantContractTypesPage"},
			{"tag": "ContractTypeView"}, {"tag": "TenantDirectoryProfileSection"}, {"tag": "TenantConnectionCodePage"}, {"tag": "TenantPartnersPage"}, {"tag": "DirectoryProfileDialog"}, {"tag": "DirectoryProfileEditDialog"}, {"tag": "PartyRoleDialog"}, {"tag": "ContractTypeDialog"}, {"tag": "ContractDialog"}, {"tag": "TenantDirectorySearchPage"},
			{"tag": "ContractsApiClient"}
		]
	}

	function test_theComponentsCompile(data){
		let component = Qt.createComponent("qrc:/qml/imtauthgui/" + data.tag + ".qml")
		compare(component.status, Component.Ready, component.errorString())
	}

	function test_theGqlClientCompiles(){
		let component = Qt.createComponent("qrc:/qml/imtguigql/GqlBasedContractsApiClient.qml")
		compare(component.status, Component.Ready, component.errorString())
	}
}
