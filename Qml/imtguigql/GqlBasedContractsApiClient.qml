// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import imtauthContractsSdl 1.0
import imtauthContractCatalogsSdl 1.0
import imtauthTenantDirectorySdl 1.0
import imtauthgui 1.0
import imtguigql 1.0

/**
 * The contracts, their catalogs and the directory of the organizations over GraphQL.
 *
 * The answers are given to the views as the plain objects the server sent. The changes are sent
 * one after another: a command waits until the one before it is answered.
 */
ContractsApiClient {
	id: root

	property string permissionPath: ""

	contractCommandsControllerComp: Component {
		GqlBasedCommandsController {
			typeId: "Contract"
		}
	}

	directoryDataProvider: GqlBasedDirectoryDataProvider {
		permissionPath: root.permissionPath
	}

	// --- Contracts ---
	function fetchContracts(tenantId) {
		root.__send(root.__getContractsRequest, root.__contractTenantInputComp, {"tenantId": tenantId})
	}

	function fetchContract(tenantId, contractId) {
		root.__send(root.__getContractRequest, root.__contractIdInputComp, {"tenantId": tenantId, "contractId": contractId})
	}

	function fetchEvents(tenantId, contractId) {
		root.__pendingEventsContractId = contractId
		root.__send(root.__getEventsRequest, root.__contractIdInputComp, {"tenantId": tenantId, "contractId": contractId})
	}

	function fetchPartyCandidates(tenantId) {
		root.__send(root.__getCandidatesRequest, root.__contractTenantInputComp, {"tenantId": tenantId})
	}

	function createContract(tenantId, draft) {
		root.__enqueue(ImtauthContractsSdlCommandIds.s_createContract, "contract", root.__createContractInputComp,
					   {"tenantId": tenantId, "draft": root.__draftInput(draft)})
	}

	function updateContract(tenantId, contractId, revision, draft) {
		root.__enqueue(ImtauthContractsSdlCommandIds.s_updateContract, "contract", root.__updateContractInputComp,
					   {"tenantId": tenantId, "contractId": contractId, "revision": revision, "draft": root.__draftInput(draft)})
	}

	function addPartyByCode(tenantId, contractId, revision, connectionCode, roleId) {
		root.__enqueue(ImtauthContractsSdlCommandIds.s_addContractPartyByCode, "contract", root.__addPartyByCodeInputComp,
					   {"tenantId": tenantId, "contractId": contractId, "revision": revision, "connectionCode": connectionCode, "roleId": roleId || ""})
	}

	function runCommand(command, tenantId, contractId, revision, comment) {
		root.__enqueue(command, "contract", root.__commandInputComp,
					   {"tenantId": tenantId, "contractId": contractId, "revision": revision, "comment": comment || ""})
	}

	function signContract(tenantId, contractId, revision, position, method, externalSignatureId, attachmentId) {
		root.__enqueue(ImtauthContractsSdlCommandIds.s_signContract, "contract", root.__signInputComp,
					   {"tenantId": tenantId, "contractId": contractId, "revision": revision, "position": position || "",
						"method": method || ContractSignatureMethodEnum.s_inSystem, "externalSignatureId": externalSignatureId || "",
						"attachmentId": attachmentId || ""})
	}

	function giveNotice(tenantId, contractId, reason, isImmediate) {
		root.__enqueue(ImtauthContractsSdlCommandIds.s_giveContractNotice, "contract", root.__noticeInputComp,
					   {"tenantId": tenantId, "contractId": contractId, "reason": reason || "", "isImmediate": isImmediate === true})
	}

	function proposeTermination(tenantId, contractId, terminateAt, reason) {
		root.__enqueue(ImtauthContractsSdlCommandIds.s_proposeContractTermination, "contract", root.__terminationInputComp,
					   {"tenantId": tenantId, "contractId": contractId, "terminateAt": terminateAt || "", "reason": reason || ""})
	}

	function setAllocationsSuspended(tenantId, contractId, isSuspended) {
		root.__enqueue(ImtauthContractsSdlCommandIds.s_setContractAllocationsSuspended, "contract", root.__suspendInputComp,
					   {"tenantId": tenantId, "contractId": contractId, "isSuspended": isSuspended === true})
	}

	function addAttachment(tenantId, contractId, kind, fileName, mimeType, data, replacesAttachmentId, isInternal) {
		root.__enqueue(ImtauthContractsSdlCommandIds.s_addContractAttachment, "contract", root.__attachmentInputComp,
					   {"tenantId": tenantId, "contractId": contractId, "kind": kind || ContractAttachmentKindEnum.s_other,
						"fileName": fileName, "mimeType": mimeType || "", "data": data,
						"replacesAttachmentId": replacesAttachmentId || "", "isInternal": isInternal === true})
	}

	function fetchAttachment(tenantId, contractId, attachmentId) {
		root.__pendingAttachmentId = attachmentId
		root.__send(root.__getAttachmentRequest, root.__getAttachmentInputComp,
					{"tenantId": tenantId, "contractId": contractId, "attachmentId": attachmentId})
	}

	function setPrivateInfo(tenantId, contractId, info) {
		root.__enqueue(ImtauthContractsSdlCommandIds.s_setContractPrivateInfo, "contract", root.__privateInfoInputComp,
					   {"tenantId": tenantId, "contractId": contractId, "internalNumber": info.internalNumber || "",
						"responsibleUserId": info.responsibleUserId || "", "notes": info.notes || "", "reminderDays": info.reminderDays || 0})
	}

	function setAllocationGroups(tenantId, contractId, allocationId, groupIds) {
		root.__enqueue(ImtauthContractsSdlCommandIds.s_setContractAllocationGroups, "contract", root.__allocationGroupsInputComp,
					   {"tenantId": tenantId, "contractId": contractId, "allocationId": allocationId, "groupIds": groupIds || []})
	}

	// --- Catalogs ---
	function fetchPartyRoles(tenantId, includeArchived) {
		root.__send(root.__getRolesRequest, root.__catalogInputComp, {"tenantId": tenantId, "includeArchived": includeArchived === true})
	}

	function savePartyRole(tenantId, role) {
		root.__enqueue(ImtauthContractCatalogsSdlCommandIds.s_savePartyRole, "PartyRoles", root.__saveRoleInputComp,
					   {"tenantId": tenantId, "role": role})
	}

	function setPartyRoleArchived(tenantId, roleId, isArchived) {
		root.__enqueue(ImtauthContractCatalogsSdlCommandIds.s_setPartyRoleArchived, "PartyRoles", root.__archiveInputComp,
					   {"tenantId": tenantId, "itemId": roleId, "isArchived": isArchived === true})
	}

	function fetchContractTypes(tenantId, includeArchived) {
		root.__send(root.__getTypesRequest, root.__catalogInputComp, {"tenantId": tenantId, "includeArchived": includeArchived === true})
	}

	function saveContractType(tenantId, contractType) {
		root.__enqueue(ImtauthContractCatalogsSdlCommandIds.s_saveContractType, "ContractTypes", root.__saveTypeInputComp,
					   {"tenantId": tenantId, "contractType": contractType})
	}

	function setContractTypeArchived(tenantId, typeId, isArchived) {
		root.__enqueue(ImtauthContractCatalogsSdlCommandIds.s_setContractTypeArchived, "ContractTypes", root.__archiveInputComp,
					   {"tenantId": tenantId, "itemId": typeId, "isArchived": isArchived === true})
	}

	// --- Directory ---
	function fetchDirectoryProfile(tenantId) {
		root.__send(root.__getProfileRequest, root.__getProfileInputComp, {"tenantId": tenantId})
	}

	function setDirectoryProfile(tenantId, profile) {
		root.__enqueue(ImtauthTenantDirectorySdlCommandIds.s_setDirectoryProfile, "Directory", root.__setProfileInputComp,
					   {"tenantId": tenantId, "profile": profile})
	}

	// =========================================================================
	// Transport
	// =========================================================================

	// The enums of the draft are always sent: an empty value is not one of them
	function __draftInput(draft) {
		var input = {}
		for (var key in draft)
			input[key] = draft[key]
		if (!input.signOrder || input.signOrder === "")
			input.signOrder = ContractSignOrderEnum.s_parallel
		var references = input.references || []
		for (var i = 0; i < references.length; i++) {
			if (!references[i].referenceType || references[i].referenceType === "")
				references[i].referenceType = ContractReferenceTypeEnum.s_related
		}
		return input
	}

	// A new input object for each request: no field of an earlier one is sent again
	function __createInput(inputComp, values) {
		var input = inputComp.createObject(root)
		input.fromObject(values)
		return input
	}

	function __send(sender, inputComp, values) {
		var input = root.__createInput(inputComp, values)
		sender.send(input)
		input.destroy()
	}

	property var __queue: []
	property string __runningCommand: ""
	property string __runningKind: ""
	property string __lastCommandError: ""
	property string __pendingAttachmentId: ""
	property string __pendingEventsContractId: ""

	function __enqueue(command, kind, inputComp, values) {
		root.__queue.push({"command": command, "kind": kind, "input": root.__createInput(inputComp, values)})
		if (root.__runningCommand === "")
			root.__sendNextCommand()
	}

	function __sendNextCommand() {
		if (root.__queue.length === 0) {
			root.__runningCommand = ""
			root.__runningKind = ""
			return
		}
		var next = root.__queue.shift()
		root.__runningCommand = next.command
		root.__runningKind = next.kind
		root.__lastCommandError = ""
		root.__commandRequest.gqlCommandId = next.command
		// A request that cannot be sent fails through onError
		root.__commandRequest.send(next.input)
		next.input.destroy()
	}

	function __finishCommand(success, itemId, revision, errorMessage) {
		var command = root.__runningCommand
		var kind = root.__runningKind
		if (!success) {
			root.commandFailed(command, errorMessage !== "" ? errorMessage : qsTr("The request failed"))
		}
		else if (kind === "contract") {
			root.commandSucceeded(command, itemId, revision)
		}
		else if (kind === "Directory") {
			root.directoryProfileSaved()
		}
		else {
			root.catalogItemSaved(kind, itemId)
		}
		root.__sendNextCommand()
	}

	property GqlSdlRequestSender __commandRequest: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1

		function onResult(data) {
			if (!data) {
				root.__finishCommand(false, "", 0, qsTr("The server returned no data. Please try again."))
				return
			}
			var errorMessage = data.errorMessage || ""
			if (data.success === false || errorMessage !== "") {
				root.__finishCommand(false, "", 0, errorMessage)
				return
			}
			var itemId = data.contractId || data.itemId || ""
			root.__finishCommand(true, itemId, data.revision || 0, "")
		}

		// The failure ends with finished(-1), whatever reported it
		function onError(message, type) {
			root.__lastCommandError = message || ""
			finished(-1)
		}

		onFinished: {
			if (status < 0 && root.__runningCommand !== "")
				root.__finishCommand(false, "", 0, root.__lastCommandError)
		}
	}

	function __readFailed(message) {
		if (message && message !== "")
			root.requestFailed(message)
	}

	property GqlSdlRequestSender __getContractsRequest: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthContractsSdlCommandIds.s_getContracts

		function onResult(data) {
			if (!data || (data.errorMessage && data.errorMessage !== "")) {
				root.__readFailed(data ? data.errorMessage : qsTr("The server returned no data. Please try again."))
				return
			}
			root.contracts = data.contracts || []
			root.contractsReceived()
		}

		function onError(message, type) {
			root.__readFailed(message)
		}
	}

	property GqlSdlRequestSender __getContractRequest: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthContractsSdlCommandIds.s_getContract

		function onResult(data) {
			if (!data || !data.contract || (data.errorMessage && data.errorMessage !== "")) {
				root.__readFailed(data ? data.errorMessage : qsTr("The server returned no data. Please try again."))
				return
			}
			root.privateInfo = data.privateInfo || null
			root.contract = data.contract
			root.contractReceived(data.contract.id || "")
		}

		function onError(message, type) {
			root.__readFailed(message)
		}
	}

	property GqlSdlRequestSender __getEventsRequest: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthContractsSdlCommandIds.s_getContractEvents

		function onResult(data) {
			if (!data || (data.errorMessage && data.errorMessage !== "")) {
				root.__readFailed(data ? data.errorMessage : qsTr("The server returned no data. Please try again."))
				return
			}
			root.events = data.events || []
			root.eventsReceived(root.__pendingEventsContractId)
		}

		function onError(message, type) {
			root.__readFailed(message)
		}
	}

	property GqlSdlRequestSender __getCandidatesRequest: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthContractsSdlCommandIds.s_getContractPartyCandidates

		function onResult(data) {
			if (!data || (data.errorMessage && data.errorMessage !== "")) {
				root.__readFailed(data ? data.errorMessage : qsTr("The server returned no data. Please try again."))
				return
			}
			root.partyCandidates = data.candidates || []
			root.partyCandidatesReceived()
		}

		function onError(message, type) {
			root.__readFailed(message)
		}
	}

	property GqlSdlRequestSender __getAttachmentRequest: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthContractsSdlCommandIds.s_getContractAttachment

		function onResult(data) {
			if (!data || (data.errorMessage && data.errorMessage !== "")) {
				root.__readFailed(data ? data.errorMessage : qsTr("The server returned no data. Please try again."))
				return
			}
			root.attachmentReceived(root.__pendingAttachmentId, data.fileName || "", data.mimeType || "", data.data || "")
		}

		function onError(message, type) {
			root.__readFailed(message)
		}
	}

	property GqlSdlRequestSender __getRolesRequest: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthContractCatalogsSdlCommandIds.s_getPartyRoles

		function onResult(data) {
			if (!data || (data.errorMessage && data.errorMessage !== "")) {
				root.__readFailed(data ? data.errorMessage : qsTr("The server returned no data. Please try again."))
				return
			}
			root.partyRoles = data.roles || []
			root.partyRolesReceived()
		}

		function onError(message, type) {
			root.__readFailed(message)
		}
	}

	property GqlSdlRequestSender __getTypesRequest: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthContractCatalogsSdlCommandIds.s_getContractTypes

		function onResult(data) {
			if (!data || (data.errorMessage && data.errorMessage !== "")) {
				root.__readFailed(data ? data.errorMessage : qsTr("The server returned no data. Please try again."))
				return
			}
			root.contractTypes = data.types || []
			root.contractTypesReceived()
		}

		function onError(message, type) {
			root.__readFailed(message)
		}
	}

	property GqlSdlRequestSender __getProfileRequest: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantDirectorySdlCommandIds.s_getDirectoryProfile

		function onResult(data) {
			if (!data || (data.errorMessage && data.errorMessage !== "")) {
				root.directoryProfileReadFailed(data ? data.errorMessage : qsTr("The server returned no data. Please try again."))
				return
			}
			root.directoryProfile = data.profile || null
			root.directoryProfileReceived()
		}

		function onError(message, type) {
			root.directoryProfileReadFailed(message)
		}
	}

	property SubscriptionClient __contractSubscription: SubscriptionClient {
		gqlCommandId: ImtauthContractsSdlCommandIds.s_onContractNotification

		function getHeaders() { return {} }

		onMessageReceived: {
			if (!data)
				return
			root.contractNotified(data.containsKey("contractId") ? data.getData("contractId") : "",
								 data.containsKey("tenantId") ? data.getData("tenantId") : "")
		}

		onRefreshRequired: {
			root.contractNotified("", "")
		}
	}

	// --- Inputs ---
	property Component __contractTenantInputComp: Component { ContractTenantInput {} }
	property Component __contractIdInputComp: Component { ContractIdInput {} }
	property Component __createContractInputComp: Component { CreateContractInput {} }
	property Component __updateContractInputComp: Component { UpdateContractInput {} }
	property Component __addPartyByCodeInputComp: Component { AddContractPartyByCodeInput {} }
	property Component __commandInputComp: Component { ContractCommandInput {} }
	property Component __signInputComp: Component { SignContractInput {} }
	property Component __noticeInputComp: Component { GiveContractNoticeInput {} }
	property Component __terminationInputComp: Component { ProposeContractTerminationInput {} }
	property Component __suspendInputComp: Component { SetContractAllocationsSuspendedInput {} }
	property Component __attachmentInputComp: Component { AddContractAttachmentInput {} }
	property Component __getAttachmentInputComp: Component { GetContractAttachmentInput {} }
	property Component __privateInfoInputComp: Component { SetContractPrivateInfoInput {} }
	property Component __allocationGroupsInputComp: Component { SetContractAllocationGroupsInput {} }
	property Component __catalogInputComp: Component { ContractCatalogInput {} }
	property Component __saveRoleInputComp: Component { SavePartyRoleInput {} }
	property Component __saveTypeInputComp: Component { SaveContractTypeInput {} }
	property Component __archiveInputComp: Component { SetCatalogItemArchivedInput {} }
	property Component __getProfileInputComp: Component { GetDirectoryProfileInput {} }
	property Component __setProfileInputComp: Component { SetDirectoryProfileInput {} }
}
