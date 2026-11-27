// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtauthgui 1.0

/**
 * ContractView
 *
 * The card of a contract seen by one of its parties. The steps the organization can take are the commands
 * of the type "Contract" in the commands panel, shown by the state of the contract; under them the state and
 * what to do next. The parts of the contract are the pages of a MultiPageView: the terms, the period,
 * the parties, the links, the permissions, the references, the documents, the internal data and the journal.
 *
 * The view keeps the working copy of the terms (draft and the lists). A form page shows it in updateGui()
 * and writes it back in updateModel(); a list page shows its list as a table and changes it through the view.
 * The initiator edits the terms until the contract is concluded; every change asks the parties again.
 */
ViewBase {
	id: contractView

	contentColor: Style.baseColor
	commandsControllerComp: contractView.contractsApiClient ? contractView.contractsApiClient.contractCommandsControllerComp : null

	property string tenantId: ""
	property string tenantName: ""
	// Empty for a new contract
	property string contractId: ""
	// ContractsApiClient
	property var contractsApiClient: null
	// TenantManagementApiClient: the permission scope and the groups of the organization
	property var tenantApiClient: null
	property var stateManager: null

	property string title: ""

	property var contract: null
	property var privateInfo: null
	property var events: []

	// The working copy of the terms: the fields of ContractDraftInput but the lists
	property var draft: ({})
	// The working copy of the internal data of the organization
	property var parties: []
	property var links: []
	property var references: []

	property string __currentId: contractView.contractId
	property bool __busy: false

	// The working copy differs from the contract it was filled from
	property bool isDirty: false
	// The revision the working copy was filled from: a save sends it, so a change made meanwhile is refused
	property int __baseRevision: 0
	property string __baseline: ""
	property bool __rebaseline: false
	// The next contract read replaces the working copy, edited or not: after the own save
	property bool __replacesEdits: false
	// The user whose change was read last
	property string lastChangedBy: ""
	property bool __changeNoticePending: false

	// A command runs: the steps wait for its answer
	readonly property bool isBusy: contractView.__busy
	readonly property bool isNew: contractView.__currentId === ""
	readonly property string contractState: contractView.contract ? (contractView.contract.state || "Draft") : "Draft"
	readonly property bool isInitiator: contractView.isNew || (contractView.contract !== null && contractView.contract.initiatorTenantId === contractView.tenantId)
	readonly property bool isSubmitted: contractView.contract !== null && !!contractView.contract.submittedAt
	readonly property bool isNegotiated: contractView.contractState === "Negotiation" || contractView.contractState === "Signing"
	readonly property bool isOfferStuck: contractView.contractState === "Rejected"
	readonly property bool isConcluded: contractView.contractState === "Signed" || contractView.contractState === "Active"
	readonly property bool isClosed: contractView.contract !== null && (!!contractView.contract.signedAt || !!contractView.contract.cancelledAt)
	readonly property var myParty: contractView.__findParty(contractView.contract ? contractView.contract.parties : [], contractView.tenantId)
	readonly property string myPartyState: contractView.myParty ? (contractView.myParty.state || "") : ""
	readonly property bool myPartyBound: contractView.myParty ? contractView.myParty.isBound === true : false

	readonly property bool canEdit: contractView.__can("canEditContracts")
	readonly property bool canApprove: contractView.__can("canApproveContracts")
	readonly property bool canSign: contractView.__can("canSignContracts")
	readonly property bool canGive: contractView.__can("canGiveContractPermissions")
	readonly property bool canDistribute: contractView.__can("canDistributeContractPermissions")
	readonly property string responsibleUserId: contractView.privateInfo ? (contractView.privateInfo.responsibleUserId || "") : ""

	readonly property bool isEditable: contractView.canEdit && contractView.isInitiator && !contractView.isClosed
	// Another user changed the terms while this one edits them: the edits cannot be saved
	readonly property bool hasConflict: contractView.isDirty && contractView.contract !== null && (contractView.contract.revision || 0) > contractView.__baseRevision
	// A new contract is saved to see what it lacks; a stored one when it was changed
	readonly property bool canSave: contractView.isEditable && !contractView.__busy && !contractView.hasConflict
		&& (contractView.isNew || contractView.isDirty)

	signal closeRequested()

	function __can(flag) {
		return contractView.contract && contractView.contract.isReadOnly ? false : (contractView.stateManager ? contractView.stateManager[flag] === true : false)
	}

	function setResponsibleUser(userId) {
		if (!contractView.privateInfo || !contractView.canEdit || contractView.__busy || userId === contractView.responsibleUserId)
			return
		contractView.__busy = true
		contractView.contractsApiClient.setPrivateInfo(contractView.tenantId, contractView.__currentId, {"responsibleUserId": userId})
	}

	// The party of the stored contract
	function contractParty(tenantId) {
		return contractView.__findParty(contractView.contract ? contractView.contract.parties : [], tenantId)
	}

	function __findParty(parties, tenantId) {
		var list = parties || []
		for (var i = 0; i < list.length; i++) {
			if (list[i].tenantId === tenantId)
				return list[i]
		}
		return null
	}

	function partyName(tenantId) {
		var party = contractView.__findParty(contractView.parties, tenantId)
		return party ? (party.tenantName || tenantId) : tenantId
	}

	function __copy(object) {
		var result = {}
		for (var key in object)
			result[key] = object[key]
		return result
	}

	// Called by the dialog once its parameters are set
	function start() {
		if (!contractView.contractsApiClient)
			return
		// The API client came after the base view was completed
		if (!contractView.commandsController && contractView.commandsControllerComp) {
			contractView.commandsController = contractView.commandsControllerComp.createObject(contractView)
			contractView.commandsController.view = contractView
			contractView.requestCommands()
		}
		contractView.contractsApiClient.fetchPartyRoles(contractView.tenantId, false)
		contractView.contractsApiClient.fetchContractTypes(contractView.tenantId, false)
		contractView.contractsApiClient.fetchPartyCandidates(contractView.tenantId)
		if (contractView.isNew)
			contractView.__fillNew()
		else
			contractView.__load()
	}

	onIsNewChanged: {
		multiPageView.updatePages()
		commandsTimer.restart()
	}

	onContractChanged: {
		commandsTimer.restart()
	}

	onIsBusyChanged: {
		commandsTimer.restart()
	}

	onCommandsControllerChanged: {
		contractView.updateCommands()
	}

	onDraftChanged: {
		contractView.markChanged()
	}

	onPartiesChanged: {
		contractView.markChanged()
	}

	onLinksChanged: {
		contractView.markChanged()
	}

	onReferencesChanged: {
		contractView.markChanged()
	}

	// A page or a list of the working copy changed: compared once the bindings are updated
	function markChanged() {
		dirtyTimer.restart()
	}

	Timer {
		id: dirtyTimer

		interval: 0
		repeat: false

		onTriggered: {
			contractView.__checkDirty()
		}
	}

	// The working copy as the pages show it, against the copy it was filled from
	function __checkDirty() {
		if (!contractView.isEditable) {
			contractView.isDirty = false
			return
		}
		contractView.__updateFromPages()
		var state = JSON.stringify(contractView.__buildDraft())
		if (contractView.__rebaseline) {
			contractView.__baseline = state
			contractView.__rebaseline = false
		}
		contractView.isDirty = state !== contractView.__baseline
	}

	// The edits are dropped and the stored contract is shown
	function discardChanges() {
		contractView.isDirty = false
		contractView.__fill()
	}

	// The commands are updated after the bindings of the contract have been updated
	Timer {
		id: commandsTimer

		interval: 0
		repeat: false

		onTriggered: {
			contractView.updateCommands()
		}
	}

	// --- The commands ---

	// The commands shown by the last update: the panel hides when there are none
	property int __shownCommands: 0

	function __setCommand(commandId, isVisible) {
		if (isVisible)
			contractView.__shownCommands++
		contractView.commandsController.setCommandVisible(commandId, isVisible)
		contractView.commandsController.setCommandIsEnabled(commandId, !contractView.__busy)
	}

	function updateCommands() {
		if (!contractView.commandsController)
			return
		contractView.__shownCommands = 0
		var view = contractView
		var party = view.myParty
		var isSigned = view.contract !== null && !!view.contract.signedAt
		// A party answers until it has signed or declined
		var isAnswering = view.myPartyState === "Reviewing" || view.myPartyState === "Approved"
		// Save is a button of the dialog; Ctrl+S still saves
		view.__setCommand("Save", false)
		view.__setCommand("Submit", view.isEditable && !view.isNew && !view.isSubmitted)
		view.__setCommand("RemoveDraft", view.isEditable && !view.isNew && !view.isSubmitted)
		// A refused offer stands until the initiator changes it or withdraws it
		view.__setCommand("Withdraw", view.isEditable && view.isSubmitted && (view.isNegotiated || view.isOfferStuck))
		view.__setCommand("Approve", view.canApprove && view.isNegotiated && view.myPartyState === "Reviewing")
		view.__setCommand("Sign", view.canSign && view.isNegotiated && isAnswering)
		view.__setCommand("RequestChanges", view.canEdit && !view.isInitiator && view.isNegotiated && isAnswering)
		view.__setCommand("Decline", view.canSign && !view.isInitiator && view.isNegotiated && isAnswering)
		view.__setCommand("GiveNotice", view.canSign && view.isConcluded && view.myPartyState === "Signed")
		// A notice is revoked until the day the organization leaves
		view.__setCommand("RevokeNotice", view.canSign && view.myPartyState === "NoticeGiven" && party !== null && !!party.withdrawAt
						  && new Date(party.withdrawAt) > new Date())
		// One refusal ends the renewal for all
		view.__setCommand("StopRenewal", view.canSign && view.isConcluded && view.contract !== null && view.contract.autoRenew === true
						  && view.myPartyBound && !view.__isRenewalStopped())
		view.commandsPanelVisible = view.__shownCommands > 0
	}

	onCommandActivated: {
		if (commandId === "Save") {
			contractView.save()
			return
		}
		// Ctrl+S reaches a hidden command too
		if (contractView.__busy || !contractView.commandsController.commandIsEnabled(commandId)
				|| !contractView.commandsController.getCommandProperty(commandId, "m_visible"))
			return
		if (commandId === "Submit")
			contractView.submit()
		else if (commandId === "RemoveDraft")
			ModalDialogManager.showConfirmationDialog(qsTr("Remove draft"), qsTr("Remove the draft? It was never sent, so the parties do not see it."), function(result) {
				if (result === Enums.yes)
					contractView.run("RemoveContract", "")
			})
		else if (commandId === "Withdraw")
			contractView.askReason("CancelContract", qsTr("The parties are told the offer is withdrawn."), qsTr("Withdraw"), false)
		else if (commandId === "Approve")
			contractView.run("ApproveContract", "")
		else if (commandId === "Sign")
			ModalDialogManager.openDialog(signDialogComp, {"revision": contractView.contract.revision || 0})
		else if (commandId === "RequestChanges")
			contractView.askReason("RequestContractChanges", qsTr("The initiator sees what you ask to change."), qsTr("Ask"), true)
		else if (commandId === "Decline")
			contractView.askReason("DeclineContract", qsTr("The organization does not conclude the contract."), qsTr("Decline"), true)
		else if (commandId === "GiveNotice")
			ModalDialogManager.openDialog(noticeDialogComp, {"message": contractView.__noticeText(false), "immediateMessage": contractView.__noticeText(true)})
		else if (commandId === "RevokeNotice")
			contractView.run("RevokeContractNotice", "")
		else if (commandId === "StopRenewal")
			contractView.run("GiveContractNonRenewalNotice", "")
	}

	// A contract is sent once it has a party besides the organization
	function submit() {
		if (contractView.isDirty) {
			PopupManager.addWarningMessage(qsTr("Save the changes first: the parties get the saved terms."), true)
			return
		}
		if (contractView.parties.length < 2) {
			PopupManager.addWarningMessage(qsTr("Add at least one more party before sending the contract."), true)
			multiPageView.activatePage(multiPageView.getIndexById("Parties"))
			return
		}
		contractView.run("SubmitContract", "")
	}

	// What the organization does next, by the state of the contract and of its party
	function nextStepText() {
		var view = contractView
		if (view.isNew)
			return qsTr("Name the contract, choose its type and add the parties on the page Parties. Save the draft: the parties see the contract only after it is sent.")
		if (!view.contract)
			return ""
		var state = view.contractState
		if (state === "Draft") {
			return view.isEditable
				? qsTr("The draft is saved. Check the parties, the links and the permissions, then send the contract to the parties.")
				: qsTr("The draft is prepared by the initiator.")
		}
		if (view.isNegotiated) {
			if (view.isInitiator && view.contract.isChangesRequested)
				return qsTr("Changes asked by %1. Change the terms and save: every party is asked again.").arg(view.__partiesInStateText("ChangesRequested"))
			if (view.myPartyState === "Reviewing")
				return qsTr("Read the terms and answer: approve them, sign the contract, or ask for changes.")
			if (view.myPartyState === "Approved")
				return qsTr("Your organization approved the terms. Sign the contract to conclude it.")
			if (view.myPartyState === "Signed")
				return qsTr("Your organization signed. The contract is concluded once every party has signed.")
			if (view.myPartyState === "ChangesRequested")
				return qsTr("Your organization asked for changes. The initiator changes the terms and asks again.")
			if (view.myPartyState === "Declined")
				return qsTr("Your organization declined the contract.")
			return qsTr("The parties are reading the terms.")
		}
		if (view.isConcluded) {
			if (view.myPartyState === "Withdrawn")
				return qsTr("Your organization left the contract on %1. The other parties go on with it; for your organization it stays in the history.")
					.arg(ContractTexts.dateText(view.myParty ? view.myParty.withdrawAt : ""))
			if (view.myPartyState === "NoticeGiven" && view.myParty && view.myParty.withdrawAt)
				return qsTr("Your organization gave notice and leaves the contract on %1. Until then the notice can be revoked.")
					.arg(ContractTexts.dateText(view.myParty.withdrawAt))
			if (view.contract.isTerminating && view.contract.endsAt)
				return qsTr("A party gave notice: the contract ends on %1. It is in force until then.").arg(ContractTexts.dateText(view.contract.endsAt))
			return qsTr("The contract is in force: the links and the permissions given in it work until it ends.")
		}
		if (view.contractState === "Rejected") {
			if (view.myPartyState === "Declined")
				return qsTr("Your organization declined the contract. The initiator may change the parties and ask again.")
			var refusals = view.__partiesInStateText("Declined")
			return view.isInitiator
				? qsTr("Declined by %1. Remove or replace the party on the page Parties and save to ask the parties again, or withdraw the offer.").arg(refusals)
				: qsTr("Declined by %1. The initiator decides how to go on.").arg(refusals)
		}
		if (view.contractState === "Cancelled")
			return qsTr("The initiator withdrew the offer: the contract was not concluded. It stays in the history of the parties.")
		if (!view.contract.signedAt)
			return qsTr("The contract was not concluded. It stays in the history of the parties.")
		return qsTr("The contract is closed. It stays in the history of the parties.")
	}

	function __load() {
		if (!contractView.contractsApiClient || contractView.isNew)
			return
		reloadTimer.restart()
	}

	Timer {
		id: reloadTimer
		interval: 50
		repeat: false

		onTriggered: {
			if (!contractView.contractsApiClient || contractView.isNew || contractView.isBusy)
				return
			if (contractView.__changeNoticePending) {
				contractView.__changeNoticePending = false
				PopupManager.addInfoMessage(qsTr("%1 changed the contract").arg(contractView.lastChangedBy), true)
			}
			contractView.contractsApiClient.fetchContract(contractView.tenantId, contractView.__currentId)
			contractView.contractsApiClient.fetchEvents(contractView.tenantId, contractView.__currentId)
		}
	}

	// --- The working copy ---

	function __fillNew() {
		contractView.title = qsTr("New contract")
		// A year and a month of notice: a type or the initiator changes them
		contractView.draft = {"name": "", "description": "", "terms": "", "typeId": "", "effectiveFrom": "", "termDays": 365, "validUntil": "",
							  "autoRenew": false, "renewalPeriodDays": 365, "renewalNoticeDays": 30, "terminationNoticeDays": 30}
		contractView.__rebaseline = true
		contractView.__baseRevision = 0
		contractView.parties = [{"tenantId": contractView.tenantId, "tenantName": contractView.tenantName, "roleId": "", "roleName": "",
								 "state": "Invited", "channel": "Initiator"}]
		contractView.links = []
		contractView.references = []
		contractView.__updatePagesGui()
	}

	function __fill() {
		var contract = contractView.contract
		if (!contract)
			return
		contractView.title = contract.number ? contract.number + " " + (contract.name || "") : (contract.name || qsTr("Draft"))
		contractView.__rebaseline = true
		contractView.__baseRevision = contract.revision || 0
		contractView.draft = {"name": contract.name || "", "description": contract.description || "", "terms": contract.terms || "",
							  "typeId": contract.typeId || "", "effectiveFrom": contract.effectiveFrom || "", "termDays": contract.termDays || 0,
							  "validUntil": contract.validUntil || "", "autoRenew": contract.autoRenew === true,
							  "renewalPeriodDays": contract.renewalPeriodDays || 0, "renewalNoticeDays": contract.renewalNoticeDays || 0,
							  "terminationNoticeDays": contract.terminationNoticeDays || 0}
		contractView.parties = (contract.parties || []).slice()
		contractView.links = (contract.links || []).slice()
		contractView.references = (contract.references || []).slice()
		contractView.__updatePagesGui()
	}

	function __updatePagesGui() {
		for (var i = 0; i < multiPageView.pagesModel.count; i++) {
			var page = multiPageView.getPageByIndex(i)
			if (page && typeof page.updateGui === "function")
				page.updateGui()
		}
	}

	function __updateFromPages() {
		for (var i = 0; i < multiPageView.pagesModel.count; i++) {
			var page = multiPageView.getPageByIndex(i)
			if (page && typeof page.updateModel === "function")
				page.updateModel()
		}
	}

	function __buildDraft() {
		var parties = []
		for (var i = 0; i < contractView.parties.length; i++) {
			var party = contractView.parties[i]
			parties.push({"tenantId": party.tenantId, "roleId": party.roleId || ""})
		}
		var links = []
		for (var j = 0; j < contractView.links.length; j++) {
			var link = contractView.links[j]
			links.push({"linkId": link.linkId || "", "tenantAId": link.tenantAId, "tenantBId": link.tenantBId,
						"roleAId": link.roleAId || "", "roleBId": link.roleBId || "", "validFrom": link.validFrom || "",
						"validUntil": link.validUntil || "", "isMessagingAllowed": link.isMessagingAllowed !== false,
						"isSharingAllowed": link.isSharingAllowed !== false})
		}
		var references = []
		for (var r = 0; r < contractView.references.length; r++)
			references.push({"contractId": contractView.references[r].contractId, "referenceType": contractView.references[r].referenceType})

		var result = contractView.__copy(contractView.draft)
		result.name = (result.name || "").trim()
		result.parties = parties
		result.links = links
		result.references = references
		return result
	}

	function save() {
		if (!contractView.contractsApiClient || contractView.__busy || !contractView.isEditable)
			return
		contractView.__checkDirty()
		if (!contractView.canSave)
			return
		contractView.__updateFromPages()
		var termsPage = multiPageView.getPageById("Terms")
		var isNamed = !!contractView.draft.name && contractView.draft.name.trim() !== ""
		if (!isNamed || (termsPage && !termsPage.isValid())) {
			if (termsPage)
				termsPage.revealErrors()
			PopupManager.addWarningMessage(isNamed ? qsTr("Correct the fields marked on the page Terms.") : qsTr("A contract needs a name."), true)
			multiPageView.activatePage(multiPageView.getIndexById("Terms"))
			return
		}
		contractView.__busy = true
		if (contractView.isNew)
			contractView.contractsApiClient.createContract(contractView.tenantId, contractView.__buildDraft())
		else
			contractView.contractsApiClient.updateContract(contractView.tenantId, contractView.__currentId, contractView.__baseRevision, contractView.__buildDraft())
	}

	// --- The catalogs ---

	function typeById(typeId) {
		var types = contractView.contractsApiClient ? contractView.contractsApiClient.contractTypes : []
		for (var i = 0; i < types.length; i++) {
			if (types[i].typeId === typeId)
				return types[i]
		}
		return null
	}

	// The types offered: the ones in use, an archived one only for the contract that has it
	function availableTypes() {
		var result = []
		var types = contractView.contractsApiClient ? contractView.contractsApiClient.contractTypes : []
		for (var i = 0; i < types.length; i++) {
			if (!types[i].isArchived || types[i].typeId === contractView.draft.typeId)
				result.push(types[i])
		}
		return result
	}

	// The terms of the type are offered with it: the periods
	function applyType(typeId) {
		var draft = contractView.__copy(contractView.draft)
		draft.typeId = typeId
		var type = contractView.typeById(typeId)
		if (type) {
			draft.termDays = type.termDays || 0
			draft.autoRenew = type.autoRenew === true
			draft.renewalPeriodDays = type.renewalPeriodDays || 0
			draft.renewalNoticeDays = type.renewalNoticeDays || 0
			draft.terminationNoticeDays = type.terminationNoticeDays || 0
		}
		contractView.draft = draft
		contractView.__rebuildLinks()
		var periodPage = multiPageView.getPageById("Period")
		if (periodPage)
			periodPage.updateGui()
	}

	function __roleById(roleId) {
		var roles = contractView.contractsApiClient ? contractView.contractsApiClient.partyRoles : []
		for (var i = 0; i < roles.length; i++) {
			if (roles[i].roleId === roleId)
				return roles[i]
		}
		return null
	}

	function roleName(roleId) {
		var role = roleId !== "" ? contractView.__roleById(roleId) : null
		return role ? role.name : ""
	}

	// The role of a party on its links: its own role when it is a role of a link
	function __linkRoleOf(tenantId) {
		var party = contractView.__findParty(contractView.parties, tenantId)
		var role = party && party.roleId ? contractView.__roleById(party.roleId) : null
		return role && role.isLinkRole ? role.roleId : ""
	}

	// The roles offered; a type with slots offers the roles of its slots only
	function roles(isPartyRole) {
		var result = []
		var roles = contractView.contractsApiClient ? contractView.contractsApiClient.partyRoles : []
		var type = isPartyRole && contractView.draft && contractView.draft.typeId ? contractView.typeById(contractView.draft.typeId) : null
		var slotRoleIds = []
		var slots = type && type.roleSlots ? type.roleSlots : []
		for (var s = 0; s < slots.length; s++)
			slotRoleIds.push(slots[s].roleId)
		for (var i = 0; i < roles.length; i++) {
			if (roles[i].isArchived)
				continue
			if (slotRoleIds.length > 0 && slotRoleIds.indexOf(roles[i].roleId) < 0)
				continue
			if (isPartyRole ? roles[i].isPartyRole : roles[i].isLinkRole)
				result.push({"roleId": roles[i].roleId, "name": roles[i].name})
		}
		return result
	}

	function candidates() {
		var result = []
		var candidates = contractView.contractsApiClient ? contractView.contractsApiClient.partyCandidates : []
		for (var i = 0; i < candidates.length; i++) {
			if (candidates[i].tenantId !== contractView.tenantId && !contractView.__findParty(contractView.parties, candidates[i].tenantId))
				result.push(candidates[i])
		}
		return result
	}

	// --- The parties and the links ---

	function addParty(tenantId, tenantName, roleId, roleName) {
		var parties = []
		var role = contractView.__roleById(roleId)
		var pairedRole = role && role.pairedRoleId ? contractView.__roleById(role.pairedRoleId) : null
		for (var i = 0; i < contractView.parties.length; i++) {
			var party = contractView.__copy(contractView.parties[i])
			if (party.tenantId === contractView.tenantId && !party.roleId && pairedRole && pairedRole.isPartyRole) {
				party.roleId = pairedRole.roleId
				party.roleName = pairedRole.name
			}
			parties.push(party)
		}
		parties.push({"tenantId": tenantId, "tenantName": tenantName, "roleId": roleId, "roleName": roleName, "state": "Invited"})
		contractView.parties = parties
		contractView.__rebuildLinks()
	}

	function setPartyRole(tenantId, roleId, roleName) {
		var parties = []
		for (var i = 0; i < contractView.parties.length; i++) {
			var party = contractView.__copy(contractView.parties[i])
			if (party.tenantId === tenantId) {
				party.roleId = roleId
				party.roleName = roleName
			}
			parties.push(party)
		}
		contractView.parties = parties
		contractView.__rebuildLinks()
	}

	function removeParty(tenantId) {
		var parties = []
		for (var i = 0; i < contractView.parties.length; i++) {
			if (contractView.parties[i].tenantId !== tenantId)
				parties.push(contractView.parties[i])
		}
		contractView.parties = parties
		contractView.__rebuildLinks()
	}

	function __findLink(tenantAId, tenantBId) {
		for (var i = 0; i < contractView.links.length; i++) {
			var link = contractView.links[i]
			if ((link.tenantAId === tenantAId && link.tenantBId === tenantBId) || (link.tenantAId === tenantBId && link.tenantBId === tenantAId))
				return link
		}
		return null
	}

	// The links follow the scheme of the type: every pair (mesh), or the party of the star role with every other one.
	// A pair keeps its link with its id, so the permissions given within it stay.
	function __rebuildLinks() {
		var type = contractView.typeById(contractView.draft.typeId)
		var scheme = type ? type.linkScheme : "Mesh"
		var centerId = ""
		if (scheme === "Star") {
			for (var c = 0; c < contractView.parties.length; c++) {
				if (contractView.parties[c].roleId === type.starRoleId) {
					centerId = contractView.parties[c].tenantId
					break
				}
			}
			if (centerId === "")
				centerId = contractView.parties.length > 0 ? contractView.parties[0].tenantId : ""
		}
		var links = []
		for (var i = 0; i < contractView.parties.length; i++) {
			for (var j = i + 1; j < contractView.parties.length; j++) {
				var a = contractView.parties[i].tenantId
				var b = contractView.parties[j].tenantId
				if (scheme === "Star" && a !== centerId && b !== centerId)
					continue
				var link = contractView.__findLink(a, b)
				link = link ? contractView.__copy(link) : {"linkId": "", "tenantAId": a, "tenantBId": b, "roleAId": "", "roleBId": "",
														   "isMessagingAllowed": true, "isSharingAllowed": true}
				if (!link.roleAId)
					link.roleAId = contractView.__linkRoleOf(link.tenantAId)
				if (!link.roleBId)
					link.roleBId = contractView.__linkRoleOf(link.tenantBId)
				links.push(link)
			}
		}
		contractView.links = links
	}

	function setLinkFlag(linkIndex, flag, value) {
		if (linkIndex < 0 || linkIndex >= contractView.links.length)
			return
		var links = contractView.links.slice()
		var link = contractView.__copy(links[linkIndex])
		link[flag] = value
		links[linkIndex] = link
		contractView.links = links
	}

	// --- The permissions ---

	// The parties the organization is linked with by the saved contract: it gives its permissions to them
	function linkedPartners() {
		var result = []
		var links = contractView.contract ? (contractView.contract.links || []) : []
		for (var i = 0; i < links.length; i++) {
			var otherId = links[i].tenantAId === contractView.tenantId ? links[i].tenantBId
				: (links[i].tenantBId === contractView.tenantId ? links[i].tenantAId : "")
			var other = otherId !== "" ? contractView.contractParty(otherId) : null
			if (other && other.state !== "Declined" && other.state !== "Withdrawn")
				result.push({"tenantId": otherId, "tenantName": other.tenantName || otherId})
		}
		return result
	}

	// The organization gives and takes back its own permissions until the contract ends, unless it declined or left it
	readonly property bool canChangeOwnPermissions: contractView.canGive && !contractView.isNew && contractView.contract !== null
		&& contractView.myParty !== null && contractView.myPartyState !== "Declined" && contractView.myPartyState !== "Withdrawn"
		&& ["Cancelled", "Expired", "Terminated", "Superseded"].indexOf(contractView.contractState) < 0

	function givePermissions(granteeTenantId, permissionIds, validUntil, comment) {
		if (!contractView.contractsApiClient || contractView.__busy)
			return
		contractView.__busy = true
		contractView.contractsApiClient.givePermissions(contractView.tenantId, contractView.__currentId, granteeTenantId,
														permissionIds, validUntil, comment)
	}

	function withdrawPermissions(allocationId) {
		if (!contractView.contractsApiClient || contractView.__busy)
			return
		contractView.__busy = true
		contractView.contractsApiClient.withdrawPermissions(contractView.tenantId, contractView.__currentId, allocationId)
	}

	function allocationGroupIds(allocationId) {
		var groups = contractView.privateInfo ? (contractView.privateInfo.allocationGroups || []) : []
		for (var i = 0; i < groups.length; i++) {
			if (groups[i].allocationId === allocationId)
				return groups[i].groupIds || []
		}
		return []
	}

	function giveAllocationToGroups(allocationId, groupIds) {
		if (!contractView.contractsApiClient || contractView.__busy)
			return
		contractView.__busy = true
		contractView.contractsApiClient.setAllocationGroups(contractView.tenantId, contractView.__currentId, allocationId, groupIds)
	}

	// --- The references ---

	function addReference(contractId, number, name, referenceType) {
		var references = []
		for (var i = 0; i < contractView.references.length; i++) {
			if (contractView.references[i].contractId !== contractId)
				references.push(contractView.references[i])
		}
		references.push({"contractId": contractId, "contractNumber": number, "contractName": name, "referenceType": referenceType})
		contractView.references = references
	}

	function removeReference(index) {
		var references = contractView.references.slice()
		references.splice(index, 1)
		contractView.references = references
	}

	function referenceCandidates() {
		var result = []
		var contracts = contractView.contractsApiClient ? contractView.contractsApiClient.contracts : []
		for (var i = 0; i < contracts.length; i++) {
			if (contracts[i].id !== contractView.__currentId && contracts[i].number && contracts[i].number !== "")
				result.push(contracts[i])
		}
		return result
	}

	// --- The documents ---

	function attachments() {
		var result = []
		var shared = contractView.contract ? (contractView.contract.attachments || []) : []
		for (var i = 0; i < shared.length; i++)
			result.push(shared[i])
		var internal = contractView.privateInfo ? (contractView.privateInfo.attachments || []) : []
		for (var j = 0; j < internal.length; j++)
			result.push(internal[j])
		return result
	}

	function attachDocument(fileName, mimeType, data, isInternal, replacesId) {
		if (!contractView.contractsApiClient || contractView.__busy)
			return
		contractView.__busy = true
		contractView.contractsApiClient.addAttachment(contractView.tenantId, contractView.__currentId, fileName, mimeType, data, replacesId || "", isInternal)
	}

	function downloadDocument(attachmentId) {
		if (contractView.contractsApiClient)
			contractView.contractsApiClient.fetchAttachment(contractView.tenantId, contractView.__currentId, attachmentId)
	}

	// --- The steps ---

	function run(command, comment) {
		if (!contractView.contractsApiClient || contractView.__busy || !contractView.contract)
			return
		contractView.__busy = true
		contractView.contractsApiClient.runCommand(command, contractView.tenantId, contractView.__currentId, contractView.contract.revision || 0, comment || "")
	}

	// When the organization leaves after a notice, and whether the contract ends with it
	function __noticeText(isImmediate) {
		var days = contractView.contract ? (contractView.contract.terminationNoticeDays || 0) : 0
		var parties = contractView.contract ? (contractView.contract.parties || []) : []
		var boundOthers = []
		for (var i = 0; i < parties.length; i++) {
			if (parties[i].tenantId !== contractView.tenantId && (parties[i].state === "Signed" || parties[i].state === "NoticeGiven"))
				boundOthers.push(parties[i].tenantId)
		}
		// The others hold the contract while two of them stay linked with each other
		var links = contractView.contract ? (contractView.contract.links || []) : []
		var isLinkLeft = false
		for (var l = 0; l < links.length; l++)
			isLinkLeft = isLinkLeft || (boundOthers.indexOf(links[l].tenantAId) >= 0 && boundOthers.indexOf(links[l].tenantBId) >= 0)
		var leaveDate = new Date()
		leaveDate.setDate(leaveDate.getDate() + days)
		var text = isImmediate
			? qsTr("Your organization leaves the contract at once, without the notice period. The breach is the reason.")
			: days > 0
				? qsTr("Your organization leaves the contract on %1, after the notice period of %2 days. The other parties are told now.")
					.arg(ContractTexts.dateText(leaveDate.toISOString())).arg(days)
				: qsTr("The contract has no notice period: your organization leaves it at once.")
		if (!isLinkLeft)
			text = text + " " + qsTr("No two other parties stay linked with each other, so the contract ends then.")
		return text
	}

	// The parties in a state with their comments: "QUISS (reason)"
	function __partiesInStateText(state) {
		var result = []
		var parties = contractView.contract ? (contractView.contract.parties || []) : []
		for (var i = 0; i < parties.length; i++) {
			if (parties[i].state === state)
				result.push((parties[i].tenantName || parties[i].tenantId) + (parties[i].stateComment ? " (" + parties[i].stateComment + ")" : ""))
		}
		return result.join(", ")
	}

	function __isRenewalStopped() {
		var parties = contractView.contract ? (contractView.contract.parties || []) : []
		for (var i = 0; i < parties.length; i++) {
			if (parties[i].nonRenewalNoticeAt)
				return true
		}
		return false
	}

	// The period and the marks of the contract, joined by dots
	function __periodLine(contract) {
		if (!contract)
			return ""
		var parts = []
		var period = ContractTexts.periodText(contract.effectiveStart, contract.effectiveStart ? (contract.endedAt || contract.periodEnd) : contract.periodEnd)
		if (period !== "")
			parts.push(period)
		if (contract.isTerminating)
			parts.push(qsTr("terminating on %1").arg(ContractTexts.dateText(contract.endsAt)))
		else if (contract.autoRenew && !contract.endedAt && period !== "") {
			var isRenewalStopped = false
			var parties = contract.parties || []
			for (var i = 0; i < parties.length; i++)
				isRenewalStopped = isRenewalStopped || !!parties[i].nonRenewalNoticeAt
			parts.push(isRenewalStopped ? qsTr("not renewed: a party stopped the renewal") : qsTr("renewed automatically"))
		}
		// A party that leaves while the others keep the contract
		var leaving = contract.parties || []
		for (var p = 0; p < leaving.length; p++) {
			if (!contract.isTerminating && leaving[p].state === "NoticeGiven" && leaving[p].withdrawAt && new Date(leaving[p].withdrawAt) > new Date())
				parts.push(qsTr("%1 leaves on %2").arg(leaving[p].tenantName || leaving[p].tenantId).arg(ContractTexts.dateText(leaving[p].withdrawAt)))
		}
		if (contract.isChangesRequested && contractView.isNegotiated)
			parts.push(qsTr("a party asked for changes"))
		if (contract.supersededBy)
			parts.push(qsTr("replaced by another contract on %1").arg(ContractTexts.dateText(contract.supersededAt)))
		return parts.join(" · ")
	}

	function askReason(command, message, okText, isRequired) {
		ModalDialogManager.openDialog(reasonDialogComp, {"command": command, "message": message, "okText": okText, "isReasonRequired": isRequired})
	}

	function __successText(command) {
		if (command === "CreateContract")
			return qsTr("The draft was created")
		if (command === "UpdateContract")
			return qsTr("The terms were saved")
		if (command === "SubmitContract")
			return qsTr("The contract was sent to the parties")
		if (command === "SignContract")
			return qsTr("The contract was signed")
		if (command === "ApproveContract")
			return qsTr("The terms were approved")
		if (command === "AddContractAttachment")
			return qsTr("The document was attached")
		if (command === "CancelContract")
			return qsTr("The offer was withdrawn")
		if (command === "RequestContractChanges")
			return qsTr("The initiator was asked for changes")
		if (command === "DeclineContract")
			return qsTr("The contract was declined")
		if (command === "GiveContractNotice")
			return qsTr("The notice was given")
		if (command === "RevokeContractNotice")
			return qsTr("The notice was revoked")
		if (command === "GiveContractNonRenewalNotice")
			return qsTr("The contract will not be renewed")
		if (command === "AddContractPartyByCode")
			return qsTr("The party was added")
		if (command === "SetContractAllocationGroups")
			return qsTr("The groups were saved")
		if (command === "SetContractPrivateInfo")
			return qsTr("The responsible person was saved")
		if (command === "GiveContractPermissions")
			return qsTr("The permissions were given")
		if (command === "WithdrawContractPermissions")
			return qsTr("The permissions were taken back")
		return qsTr("Done")
	}

	Connections {
		target: contractView.contractsApiClient

		function onContractReceived(contractId) {
			if (contractId !== contractView.__currentId)
				return
			var received = contractView.contractsApiClient.contract
			// The edits stay while they can still be saved; the state, the parties and the documents follow at once
			var keepsEdits = contractView.isDirty && !contractView.__replacesEdits && received !== null && !received.signedAt && !received.cancelledAt
			contractView.__replacesEdits = false
			contractView.privateInfo = contractView.contractsApiClient.privateInfo
			contractView.contract = received
			if (!keepsEdits)
				contractView.__fill()
		}

		function onEventsReceived(contractId) {
			if (contractId === contractView.__currentId)
				contractView.events = contractView.contractsApiClient.events
		}

		function onContractTypesReceived() {
			var termsPage = multiPageView.getPageById("Terms")
			if (termsPage)
				termsPage.updateGui()
		}

		function onCommandSucceeded(command, contractId, revision) {
			if (!contractView.__busy)
				return
			contractView.__busy = false
			if (command === "RemoveContract") {
				PopupManager.addSuccessMessage(qsTr("The draft was removed"), true)
				contractView.closeRequested()
				return
			}
			if (command === "CreateContract")
				contractView.__currentId = contractId
			if (command === "CreateContract" || command === "UpdateContract")
				contractView.__replacesEdits = true
			PopupManager.addSuccessMessage(contractView.__successText(command), true)
			contractView.__load()
		}

		function onCommandFailed(command, errorMessage) {
			if (!contractView.__busy)
				return
			contractView.__busy = false
			PopupManager.addErrorMessage(errorMessage, true)
			contractView.__load()
		}

		function onContractNotified(contractId, tenantId, sourceUserId, sourceUserName) {
			if (contractView.__busy || contractView.isNew || (contractId !== "" && contractId !== contractView.__currentId)
					|| (tenantId !== "" && tenantId !== contractView.tenantId))
				return
			// Another user changed it: the user is told who
			if (sourceUserId !== "" && sourceUserId !== AuthorizationController.getUserId()) {
				contractView.lastChangedBy = sourceUserName !== "" ? sourceUserName : qsTr("Another user")
				contractView.__changeNoticePending = true
			}
			contractView.__load()
		}

		function onAttachmentReceived(attachmentId, fileName, mimeType, data) {
			var extension = fileName.lastIndexOf(".") >= 0 ? fileName.substring(fileName.lastIndexOf(".") + 1) : "bin"
			var dialog = exportDialogComp.createObject(contractView, {"dataStr": data, "fileExt": extension,
															 "currentFile": fileName, "nameFilters": [qsTr("Document") + " (*." + extension + ")"],
															 "mimeTypes": [mimeType]})
			dialog.open()
		}
	}

	// --- The state and what to do next ---
	Column {
		id: stepsColumn

		anchors.top: parent.top
		anchors.topMargin: Style.marginL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL

		spacing: Style.marginS

		// Where the contract is: its stages and the state of every party
		ContractStateBar {
			width: parent.width
			contractState: contractView.isNew || !contractView.contract ? "" : contractView.contractState
			parties: contractView.contract ? (contractView.contract.parties || []) : []
			ownTenantId: contractView.tenantId
			wasConcluded: contractView.contract !== null && !!contractView.contract.signedAt
		}

		BaseText {
			width: parent.width
			wrapMode: Text.WordWrap
			text: (contractView.isNew
				? qsTr("New contract")
				: qsTr("State: %1").arg(ContractTexts.contractStateText(contractView.contractState))
					+ (contractView.contract && contractView.contract.revision ? " · " + qsTr("revision %1").arg(contractView.contract.revision) : ""))
				+ (contractView.isDirty ? " · " + qsTr("unsaved changes") : "")
			font.pixelSize: Style.fontSizeM
			font.bold: true
			color: Style.textColor
		}

		BaseText {
			width: parent.width
			visible: contractView.contract !== null && !contractView.isNew && text !== ""
			wrapMode: Text.WordWrap
			text: contractView.__periodLine(contractView.contract)
			font.pixelSize: Style.fontSizeS
			color: Style.inactiveTextColor
		}

		BaseText {
			width: parent.width
			visible: text !== ""
			wrapMode: Text.WordWrap
			text: contractView.nextStepText()
			font.pixelSize: Style.fontSizeM
			color: Style.textColor
		}

		// The terms changed under the edits of the user
		Rectangle {
			width: parent.width
			height: Math.max(conflictText.height, reloadButton.height) + 2 * Style.marginM
			visible: contractView.hasConflict
			radius: Style.radiusM
			color: Style.alternateBaseColor
			border.width: 1
			border.color: Style.errorColor

			BaseText {
				id: conflictText
				anchors.left: parent.left
				anchors.right: reloadButton.left
				anchors.verticalCenter: parent.verticalCenter
				anchors.margins: Style.marginM
				wrapMode: Text.WordWrap
				text: qsTr("%1 changed the terms meanwhile (revision %2). Your changes are based on revision %3 and cannot be saved: reload the contract and make them again.")
					.arg(contractView.lastChangedBy !== "" ? contractView.lastChangedBy : qsTr("Another user"))
					.arg(contractView.contract ? (contractView.contract.revision || 0) : 0).arg(contractView.__baseRevision)
				font.pixelSize: Style.fontSizeS
				color: Style.textColor
			}

			Button {
				id: reloadButton
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Reload")
				onClicked: {
					contractView.discardChanges()
				}
			}
		}
	}

	// --- The parts of the contract ---
	MultiPageView {
		id: multiPageView

		anchors.top: stepsColumn.bottom
		anchors.topMargin: Style.marginL
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		panelWidth: Style.sizeHintXXS

		function addPageOnce(pageId, name, pageComp, icon) {
			if (multiPageView.getIndexById(pageId) < 0)
				multiPageView.addPage(pageId, name, pageComp, icon)
		}

		// A new contract has its terms only; the stored one has its permissions, documents and journal too
		function updatePages() {
			multiPageView.addPageOnce("Terms", qsTr("Terms"), termsPageComp, "Icons/Edit")
			multiPageView.addPageOnce("Period", qsTr("Period"), periodPageComp, "Icons/Calendar")
			multiPageView.addPageOnce("Parties", qsTr("Parties"), partiesPageComp, "Icons/Organizations")
			multiPageView.addPageOnce("Links", qsTr("Links"), linksPageComp, "Icons/Link")
			multiPageView.addPageOnce("References", qsTr("Other contracts"), referencesPageComp, "Icons/Assignment")
			if (contractView.isNew)
				return
			multiPageView.addPageOnce("Permissions", qsTr("Permissions"), permissionsPageComp, "Icons/Key")
			multiPageView.addPageOnce("Documents", qsTr("Documents"), documentsPageComp, "Icons/Attachment")
			multiPageView.addPageOnce("Responsible", qsTr("Responsible"), responsiblePageComp, "Icons/Assignment")
			multiPageView.addPageOnce("Journal", qsTr("Journal"), journalPageComp, "Icons/History")
		}

		onPageLoaded: {
			if (pageItem && typeof pageItem.updateGui === "function")
				pageItem.updateGui()
			// A page shown first puts its fields into the copy: that is no change of the user
			if (!contractView.isDirty)
				contractView.__rebaseline = true
			contractView.markChanged()
		}

		Component.onCompleted: {
			multiPageView.updatePages()
			multiPageView.activatePage(0)
		}
	}

	Component {
		id: termsPageComp

		ContractTermsPage {
			editor: contractView
		}
	}

	Component {
		id: periodPageComp

		ContractPeriodPage {
			editor: contractView
		}
	}

	Component {
		id: partiesPageComp

		ContractPartiesPage {
			editor: contractView

			onAddPartyRequested: {
				var candidates = contractView.candidates()
				if (candidates.length === 0) {
					PopupManager.addInfoMessage(contractView.isNew
						? qsTr("There is no partner to add. Add one on the page Partners of the organization, or save the draft and add an organization by its connection code.")
						: qsTr("There is no partner to add. Add an organization by its connection code, or add a partner on the page Partners of the organization."), true)
					return
				}
				ModalDialogManager.openDialog(partyDialogComp, {"candidates": candidates, "roles": contractView.roles(true)})
			}

			onAddPartyByCodeRequested: {
				// The server adds the party to the saved terms: the edits are saved first
				if (contractView.isDirty) {
					PopupManager.addWarningMessage(qsTr("Save the changes first: a party found by its code is added to the saved terms."), true)
					return
				}
				ModalDialogManager.openDialog(partyDialogComp, {"byCode": true, "roles": contractView.roles(true)})
			}

			onPartyRoleRequested: {
				var party = contractView.__findParty(contractView.parties, tenantId)
				if (party)
					ModalDialogManager.openDialog(partyDialogComp, {"fixedTenantId": party.tenantId, "fixedTenantName": party.tenantName,
																	"initialRoleId": party.roleId || "", "roles": contractView.roles(true)})
			}
		}
	}

	Component {
		id: linksPageComp

		ContractLinksPage {
			editor: contractView
		}
	}

	Component {
		id: permissionsPageComp

		ContractPermissionsPage {
			editor: contractView

			onAddAllocationRequested: {
				var partners = contractView.linkedPartners()
				if (partners.length === 0) {
					PopupManager.addWarningMessage(qsTr("Your organization is linked with no party yet: the permissions are given within a saved link (page Links)."), true)
					return
				}
				ModalDialogManager.openDialog(allocationDialogComp, {"partners": partners,
																	 "tenantApiClient": contractView.tenantApiClient,
																	 "scopeIds": contractView.stateManager && contractView.stateManager.tenantData
																		 ? (contractView.stateManager.tenantData.m_tenantPermissions || []) : []})
			}
		}
	}

	Component {
		id: referencesPageComp

		ContractReferencesPage {
			editor: contractView

			onAddReferenceRequested: {
				ModalDialogManager.openDialog(referenceDialogComp, {"contracts": contractView.referenceCandidates()})
			}
		}
	}

	Component {
		id: documentsPageComp

		ContractDocumentsPage {
			editor: contractView
		}
	}

	Component {
		id: responsiblePageComp

		Column {
			id: responsiblePage

			anchors.left: parent.left
			anchors.right: parent.right
			anchors.margins: Style.marginXL
			anchors.top: parent.top
			spacing: Style.marginL

			readonly property string ownerChoice: "owner"
			readonly property var members: contractView.stateManager ? contractView.stateManager.pendingMembers : []
			readonly property string selectedUserId: membersModel.idAt(responsibleInput.currentIndex)
			readonly property bool canAssign: contractView.canEdit && contractView.privateInfo !== null && !contractView.isBusy

			Component.onCompleted: responsiblePage.fill()
			onMembersChanged: responsiblePage.fill()

			Connections {
				target: contractView

				function onResponsibleUserIdChanged() {
					responsiblePage.fill()
				}
			}

			function fill() {
				var choices = [{"id": responsiblePage.ownerChoice, "name": qsTr("Organization owner (default)")}].concat(responsiblePage.members)
				var assignedId = contractView.responsibleUserId
				var selectedId = assignedId || responsiblePage.ownerChoice
				responsibleInput.currentIndex = membersModel.fill(choices, "id", "name", selectedId)
			}

			ChoiceModel {
				id: membersModel
			}

			BaseText {
				width: parent.width
				wrapMode: Text.WordWrap
				text: qsTr("Contract notifications for this organization go to the responsible person. If nobody is assigned, the organization owner receives them.")
			}

			ComboBoxElementView {
				id: responsibleInput
				width: Math.min(responsiblePage.width, Style.sizeHintL)
				name: qsTr("Responsible person")
				enabled: responsiblePage.canAssign
				model: membersModel
			}

			Button {
				text: qsTr("Save responsible person")
				visible: contractView.canEdit
				enabled: responsiblePage.canAssign && responsiblePage.selectedUserId !== ""
					&& responsiblePage.selectedUserId !== (contractView.responsibleUserId || responsiblePage.ownerChoice)
				onClicked: contractView.setResponsibleUser(responsiblePage.selectedUserId === responsiblePage.ownerChoice ? "" : responsiblePage.selectedUserId)
			}
		}
	}

	Component {
		id: journalPageComp

		ContractJournalPage {
			editor: contractView
		}
	}

	// --- Dialogs ---

	Component {
		id: partyDialogComp

		ContractPartyDialog {
			id: partyDialog
			title: partyDialog.fixedTenantId !== "" ? qsTr("Role of the party") : qsTr("Add party")
			onFinished: {
				var content = partyDialog.contentItem
				if (buttonId != Enums.ok || !content)
					return
				if (partyDialog.byCode) {
					contractView.__busy = true
					contractView.contractsApiClient.addPartyByCode(contractView.tenantId, contractView.__currentId, contractView.contract.revision || 0,
																	content.code, content.roleId)
				}
				else if (partyDialog.fixedTenantId !== "") {
					contractView.setPartyRole(content.tenantId, content.roleId, content.roleName)
				}
				else {
					contractView.addParty(content.tenantId, content.tenantName, content.roleId, content.roleName)
				}
			}
		}
	}

	Component {
		id: reasonDialogComp

		ContractReasonDialog {
			id: reasonDialog
			property string command: ""
			title: reasonDialog.command === "CancelContract" ? qsTr("Withdraw the offer")
				: reasonDialog.command === "RequestContractChanges" ? qsTr("Ask for changes")
				: reasonDialog.command === "DeclineContract" ? qsTr("Decline the contract") : qsTr("Contract")
			onFinished: {
				if (buttonId == Enums.ok && reasonDialog.contentItem)
					contractView.run(reasonDialog.command, reasonDialog.contentItem.reason)
			}
		}
	}

	Component {
		id: noticeDialogComp

		ContractReasonDialog {
			id: noticeDialog
			title: qsTr("Give notice")
			message: qsTr("The organization leaves the contract when the notice period ends. The other parties are told now.")
			okText: qsTr("Give notice")
			isReasonRequired: false
			showImmediate: true
			onFinished: {
				var content = noticeDialog.contentItem
				if (buttonId != Enums.ok || !content)
					return
				contractView.__busy = true
				contractView.contractsApiClient.giveNotice(contractView.tenantId, contractView.__currentId, content.reason, content.isImmediate)
			}
		}
	}

	Component {
		id: signDialogComp

		ContractSignDialog {
			id: signDialog
			title: qsTr("Sign the contract")
			onFinished: {
				var content = signDialog.contentItem
				if (buttonId != Enums.ok || !content)
					return
				contractView.__busy = true
				contractView.contractsApiClient.signContract(contractView.tenantId, contractView.__currentId, contractView.contract.revision || 0, content.position)
			}
		}
	}

	Component {
		id: allocationDialogComp

		ContractAllocationDialog {
			id: allocationDialog
			title: qsTr("Give permissions")
			onFinished: {
				var content = allocationDialog.contentItem
				if (buttonId != Enums.ok || !content)
					return
				contractView.givePermissions(content.granteeTenantId, content.permissionIds, content.validUntil(), content.comment)
			}
		}
	}

	Component {
		id: referenceDialogComp

		ContractReferenceDialog {
			id: referenceDialog
			title: qsTr("Refer to a contract")
			onFinished: {
				var content = referenceDialog.contentItem
				if (buttonId != Enums.ok || !content || content.contractId === "")
					return
				contractView.addReference(content.contractId, content.contractNumber, content.contractName, content.referenceType)
			}
		}
	}

	Component {
		id: exportDialogComp

		FileExportDialog {
			id: exportDialog
			title: qsTr("Save document")

			onAccepted: {
				exportDialog.destroy()
			}

			onRejected: {
				exportDialog.destroy()
			}
		}
	}
}
