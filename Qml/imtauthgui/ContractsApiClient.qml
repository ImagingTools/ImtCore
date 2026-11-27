// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12

/**
 * ContractsApiClient
 *
 * Abstract contract (interface) for the contracts of the organizations, the catalogs of party roles
 * and contract types, and the directory of the organizations.
 *
 * The data reach the views as plain JS objects with the field names of the API
 * (e.g. contract.parties[i].tenantName), so the views depend on no transport and no SDL module.
 *
 * A concrete implementation (e.g. GqlBasedContractsApiClient) provides the transport.
 *
 * This file MUST NOT import imtguigql or any SDL module.
 */
QtObject {
	id: root

	// --- Contracts ---
	property var contracts: []
	// The contract last read, its private part of the organization and its journal
	property var contract: null
	property var privateInfo: null
	property var events: []
	// The organizations that can be added to a contract: the address book and the co-parties
	property var partyCandidates: []

	signal contractsReceived()
	signal contractReceived(string contractId)
	signal eventsReceived(string contractId)
	signal partyCandidatesReceived()
	// A command on a contract succeeded: its name is the one of the API, e.g. "SubmitContract"
	signal commandSucceeded(string command, string contractId, int revision)
	signal commandFailed(string command, string errorMessage)
	signal attachmentReceived(string attachmentId, string fileName, string mimeType, string data)
	// A contract the organization takes part in changed, whoever changed it
	// A contract changed; the user who changed it, empty when unknown
	signal contractNotified(string contractId, string tenantId, string sourceUserId, string sourceUserName)

	// --- Catalogs ---
	property var partyRoles: []
	property var contractTypes: []

	signal partyRolesReceived()
	signal contractTypesReceived()
	signal catalogItemSaved(string catalog, string itemId)

	// --- Directory ---
	property var directoryProfile: null

	signal directoryProfileReceived()
	// The profile could not be read; directoryProfile keeps the one before
	signal directoryProfileReadFailed(string message)
	signal directoryProfileSaved()

	// --- Commands of the card of a contract: a CommandsController component of the type "Contract" ---
	property Component contractCommandsControllerComp: null
	// The search of the directory, a data provider of a SimpleCollectionPage (FilterableSelectDataProvider):
	// an item has id, title, industry, country, offeredRoles, description, website, acceptsInvitationsFromUnknown
	property QtObject directoryDataProvider: null

	// --- Generic error ---
	signal requestFailed(string message)

	// --- Contracts ---
	function fetchContracts(tenantId) {}
	function fetchContract(tenantId, contractId) {}
	function fetchEvents(tenantId, contractId) {}
	function fetchPartyCandidates(tenantId) {}

	// The draft has the fields of ContractDraftInput: name, description, terms, typeId, effectiveFrom, termDays,
	// validUntil, autoRenew, renewalPeriodDays, renewalNoticeDays, terminationNoticeDays, parties, links, allocations, references
	function createContract(tenantId, draft) {}
	function updateContract(tenantId, contractId, revision, draft) {}
	function addPartyByCode(tenantId, contractId, revision, connectionCode, roleId) {}

	// The commands taking the read revision and a comment: RemoveContract, SubmitContract, CancelContract,
	// RequestContractChanges, ApproveContract, DeclineContract, RevokeContractNotice, GiveContractNonRenewalNotice
	function runCommand(command, tenantId, contractId, revision, comment) {}
	function signContract(tenantId, contractId, revision, position) {}
	function giveNotice(tenantId, contractId, reason, isImmediate) {}
	// The organization gives its own permissions to a party it is linked with, and takes them back
	function givePermissions(tenantId, contractId, granteeTenantId, productId, permissionIds, validUntil, comment) {}
	function withdrawPermissions(tenantId, contractId, allocationId) {}

	// The data is the Base64 of the file
	function addAttachment(tenantId, contractId, fileName, mimeType, data, replacesAttachmentId, isInternal) {}
	function fetchAttachment(tenantId, contractId, attachmentId) {}

	// Supply only the fields to change: internalNumber, responsibleUserId, notes, reminderDays.
	function setPrivateInfo(tenantId, contractId, info) {}
	function setAllocationGroups(tenantId, contractId, allocationId, groupIds) {}

	// --- Catalogs ---
	function fetchPartyRoles(tenantId, includeArchived) {}
	// The role has the fields of PartyRoleInput, an empty roleId for a new one
	function savePartyRole(tenantId, role) {}
	function setPartyRoleArchived(tenantId, roleId, isArchived) {}

	function fetchContractTypes(tenantId, includeArchived) {}
	// The type has the fields of ContractTypeInput, an empty typeId for a new one
	function saveContractType(tenantId, contractType) {}
	function setContractTypeArchived(tenantId, typeId, isArchived) {}

	// --- Directory ---
	function fetchDirectoryProfile(tenantId) {}
	// The profile has the fields of DirectoryProfileInput
	function setDirectoryProfile(tenantId, profile) {}
}
