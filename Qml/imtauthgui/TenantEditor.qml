// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtauthgui 1.0
import imtauthTenantsSdl 1.0

/**
 * TenantEditor
 *
 * Thin orchestrator that composes:
 *   - TenantEditorStateManager  — local UI state + pure logic
 *   - an injected `apiClient` (abstract TenantManagementApiClient) — transport
 *   - the page components (General / Members / Roles / Groups / Permissions)
 *
 * The editor itself does NOT depend on any concrete transport (no GQL/SDL transport
 * imports). The concrete client (e.g. GqlBasedTenantManagementApiClient from
 * imtauthgui) is supplied by the embedding view (e.g. TenantCollectionView).
 *
 * SDL imports here are limited to imtauthTenantsSdl (TenantData type of the model).
 */
DocumentViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	// GitHub-like typography applied locally within TenantEditor scope
	// font.family: "Segoe UI, Helvetica, Arial, sans-serif"

	property TenantData tenantData: model

	/**
	 * Injected transport implementing the TenantManagementApiClient contract.
	 * Must be set by the embedding view before the editor becomes active.
	 */
	property var apiClient: null

	// Injected transport implementing the ContractsApiClient contract: the contracts, their catalogs and the directory
	property var contractsApiClient: null
	property string contractToOpenId: ""
	signal contractOpenHandled()

	onContractToOpenIdChanged: container.showRequestedContract()

	function showRequestedContract() {
		if (container.contractToOpenId !== "" && multiPageView && multiPageView.getIndexById("Contracts") >= 0)
			container.__openPage("Contracts")
	}

	/**
	 * Exposed so that TenantCollectionView can bind commandsPanelVisible: isNewTenant.
	 * After save, m_id is populated → isNewTenant becomes false → panel hides.
	 */
	readonly property bool isNewTenant: stateManager_.isNewTenant

	// The view hosting the editor when it continues its header (SingleDocumentCollectionView): the section shown and
	// the editor open in it are added after the organization, and the pages show no header of their own
	property var __headerHost: null
	// A close of an editor of a page is in progress: the header does not start another one
	property bool __navigating: false

	onDocumentManagerViewChanged: {
		if (container.documentManagerView && typeof container.documentManagerView.addDocumentSubHeader === "function"){
			container.__headerHost = container.documentManagerView
			container.__headerHost.subHeaderNavigator = container
			container.updateSubHeaders()
		}
	}

	Component.onDestruction: {
		if (container.__headerHost && container.__headerHost.subHeaderNavigator === container)
			container.__headerHost.subHeaderNavigator = null
	}

	// The headers past the organization: the section shown, then the editor open in it
	function updateSubHeaders(){
		if (!container.__headerHost)
			return
		container.__headerHost.removeDocumentSubHeader("section")
		var index = multiPageView.currentIndex
		if (index < 0 || index >= multiPageView.pagesModel.count)
			return
		container.__headerHost.addDocumentSubHeader("section", multiPageView.pagesModel.get(index).name)
		var page = multiPageView.getPageByIndex(index)
		if (page && page.editorOpen === true)
			container.__headerHost.addDocumentSubHeader("editor", page.editorTitle)
	}

	function __currentPage(){
		return multiPageView.getPageByIndex(multiPageView.currentIndex)
	}

	function __hasOpenEditor(){
		for (var i = 0; i < multiPageView.pagesModel.count; i++){
			var page = multiPageView.getPageByIndex(i)
			if (page && page.editorOpen === true)
				return true
		}
		return false
	}

	// Closes the editor open in the section shown, asking about its unsaved changes
	function __closeCurrentEditor(callback){
		var page = container.__currentPage()
		if (!page || page.editorOpen !== true){
			callback(true)
			return
		}
		container.__navigating = true
		page.requestCloseEditor(function(closed){
			container.__navigating = false
			callback(closed)
		})
	}

	// Closes the editors open in the sections from `index` on, each shown while it is asked about
	function __closeEditorsFrom(index, callback){
		for (var i = index; i < multiPageView.pagesModel.count; i++){
			var page = multiPageView.getPageByIndex(i)
			if (page && page.editorOpen === true){
				var next = i + 1
				multiPageView.activatePage(i)
				container.__closeCurrentEditor(function(closed){
					if (closed)
						container.__closeEditorsFrom(next, callback)
					else
						callback(false)
				})
				return
			}
		}
		callback(true)
	}

	// The navigator of the host header (see SingleDocumentCollectionView.subHeaderNavigator)
	function canNavigateBack(){
		var page = container.__currentPage()
		return !!page && page.editorOpen === true
	}

	function navigateBack(){
		if (container.__navigating)
			return
		container.__closeCurrentEditor(function(closed){})
	}

	// The section header closes the editor open in it, the organization header also goes to the first section
	function navigateToHeader(headerId){
		if (container.__navigating)
			return
		container.__closeCurrentEditor(function(closed){
			if (closed && headerId !== "section")
				multiPageView.activatePage(0)
		})
	}

	// The editors of all sections are closed before the organization: the shown one first, then the hidden ones
	function requestLeave(callback){
		if (container.__navigating){
			callback(false)
			return
		}
		container.__closeCurrentEditor(function(closed){
			if (closed)
				container.__closeEditorsFrom(0, callback)
			else
				callback(false)
		})
	}

	// --- Composition root ---
	TenantEditorStateManager {
		id: stateManager_
		tenantData: container.tenantData
		apiClient: container.apiClient
	}

	function updateGui() {
		// The role, group and user editors of the organization offer only its permission scope
		if (container.apiClient && container.tenantData && container.tenantData.m_id !== "")
			container.apiClient.rolePermissionsTenantId = container.tenantData.m_id
		stateManager_.loadMembersFromModel()
		stateManager_.loadInvitationsFromModel()
		var generalPage = multiPageView.getPageByIndex(0)
		if (generalPage)
			generalPage.doUpdateGui()
		var permissionsPage = multiPageView.getPageById("Permissions")
		if (permissionsPage)
			permissionsPage.doUpdateGui()
	}

	// Runs inside this editor's change transaction on the shared model, so the pages must not open their own.
	function updateModel() {
		var generalPage = multiPageView.getPageByIndex(0)
		if (generalPage)
			generalPage.updateModel()
		// The permissions page writes its selection itself when it changes; rewriting it here made every commit,
		// the one of Save included, a change of the model
		if (container.tenantData)
			stateManager_.syncMembersToModel()
	}

	// --- Multi-page navigation ---
	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		onCurrentIndexChanged: {
			container.updateSubHeaders()
		}

		// Three groups: the organization itself, its people, its links to other organizations
		function updatePages() {
			multiPageView.clear()

			// The creator picks the permission scope while creating the organization
			if (stateManager_.isNewTenant) {
				multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
				multiPageView.addPage("Permissions", qsTr("Available permissions"), permissionsPageComp, "Icons/Key")
				multiPageView.currentIndex = 0
				return
			}

			multiPageView.addPage("OrganizationGroup", qsTr("Organization"), null, "Icons/Settings")
			multiPageView.addSubPage("OrganizationGroup", "General", qsTr("General"), generalPageComp)
			if (stateManager_.canViewOrganizationPermissions)
				multiPageView.addSubPage("OrganizationGroup", "Permissions", qsTr("Available permissions"), permissionsPageComp)
			multiPageView.toggleExpanded("OrganizationGroup")

			if (stateManager_.canViewOrganizationMembers || stateManager_.canViewOrganizationRoles || stateManager_.canViewOrganizationGroups) {
				multiPageView.addPage("PeopleGroup", qsTr("People"), null, "Icons/MultipleUser")
				if (stateManager_.canViewOrganizationMembers)
					multiPageView.addSubPage("PeopleGroup", "Members", qsTr("Members and invitations"), membersPageComp)
				if (stateManager_.canViewOrganizationRoles)
					multiPageView.addSubPage("PeopleGroup", "Roles", qsTr("Roles"), rolesPageComp)
				if (stateManager_.canViewOrganizationGroups)
					multiPageView.addSubPage("PeopleGroup", "Groups", qsTr("Groups"), groupsPageComp)
				multiPageView.toggleExpanded("PeopleGroup")
			}

			// Partners: the address book, finding a partner, how the others find this organization, the shared documents
			var hasContracts = !!container.contractsApiClient
			var canSeeAddressBook = stateManager_.canConnectOrganization || stateManager_.canViewOrganizationConnections
			var canSearch = stateManager_.canConnectOrganization && hasContracts && !!container.contractsApiClient.directoryDataProvider
			var canSeeDiscovery = stateManager_.canViewOrganizationConnectionCode || (stateManager_.canEditDirectoryProfile && hasContracts)
			var canSeeShares = container.__canSeeShares()
			if (canSeeAddressBook || canSearch || canSeeDiscovery || canSeeShares) {
				multiPageView.addPage("PartnersGroup", qsTr("Partners"), null, "Icons/Participant")
				if (canSeeAddressBook)
					multiPageView.addSubPage("PartnersGroup", "AddressBook", qsTr("Address book"), addressBookPageComp)
				if (canSearch)
					multiPageView.addSubPage("PartnersGroup", "DirectorySearch", qsTr("Find an organization"), directorySearchPageComp)
				if (canSeeDiscovery)
					multiPageView.addSubPage("PartnersGroup", "Discovery", qsTr("How others find you"), discoveryPageComp)
				if (canSeeShares)
					multiPageView.addSubPage("PartnersGroup", "DocumentShares", qsTr("Shared documents"), documentSharesPageComp)
				multiPageView.toggleExpanded("PartnersGroup")
				container.__fetchConnectionRequests()
			}

			// Contracts: the contracts and what a new one is made of
			var canSeeContracts = stateManager_.canViewContracts && hasContracts
			var canSeeCatalogs = (stateManager_.canViewContracts || stateManager_.canEditPartyRoles || stateManager_.canEditContractTypes) && hasContracts
			if (canSeeContracts || canSeeCatalogs) {
				multiPageView.addPage("ContractsGroup", qsTr("Contracts"), null, "Icons/Assignment")
				if (canSeeContracts)
					multiPageView.addSubPage("ContractsGroup", "Contracts", qsTr("Contracts"), contractsPageComp)
				if (canSeeCatalogs) {
					multiPageView.addSubPage("ContractsGroup", "ContractTypes", qsTr("Types of contracts"), contractTypesPageComp)
					multiPageView.addSubPage("ContractsGroup", "PartyRoles", qsTr("Roles of the parties"), partyRolesPageComp)
				}
				multiPageView.toggleExpanded("ContractsGroup")
			}

			multiPageView.currentIndex = 0
			container.showRequestedContract()
		}

		Component.onCompleted: {
			container.__builtPagesKey = container.__pagesKey()
			multiPageView.updatePages()
		}
	}

	function __openPage(pageId) {
		multiPageView.activatePage(multiPageView.getIndexById(pageId))
	}

	function __fetchConnectionRequests() {
		if (container.apiClient && container.tenantData && container.tenantData.m_id)
			container.apiClient.fetchConnectionRequests(String(container.tenantData.m_id))
	}

	// The requests waiting for an answer of the organization are counted on the address book
	function __updateAddressBookBadge() {
		var count = 0
		var tenantId = container.tenantData && container.tenantData.m_id ? String(container.tenantData.m_id) : ""
		var requests = container.apiClient ? container.apiClient.connectionRequestsModel : null
		if (requests) {
			for (var i = 0; i < requests.count; ++i) {
				var request = requests.get(i)
				if (request.status === "Pending" && request.targetTenantId === tenantId)
					count++
			}
		}
		multiPageView.setPageBadge("AddressBook", count > 0 ? String(count) : "")
	}

	// The shares belong to the organization of the session, so only its own editor shows them
	function __canSeeShares() {
		return ApplicationInfoProvider.documentSharingEnabled
			&& (stateManager_.isAdmin || stateManager_.canViewOrganizationConnections)
			&& !!container.tenantData && String(container.tenantData.m_id) === AuthorizationController.getTenantId()
	}

	// The page list follows what the pages depend on, including the organization of the session for the shares
	function __pagesKey() {
		var sessionTenant = container.tenantData && String(container.tenantData.m_id) === AuthorizationController.getTenantId()
		return stateManager_.pagesConfigKey + (sessionTenant ? "S" : "")
	}

	property string __builtPagesKey: ""

	// A representation fills the model field by field and every field flips some of the flags: the pages are rebuilt
	// once, after the model is filled, and only when the list really changes (a rebuild recreates the shown page).
	Timer {
		id: pagesUpdateTimer
		interval: 0
		onTriggered: {
			var key = container.__pagesKey()
			if (key === container.__builtPagesKey)
				return
			// A rebuild destroys the pages with their editors: it waits until the user closes them
			if (container.__hasOpenEditor()){
				pagesUpdateRetryTimer.restart()
				return
			}
			container.__builtPagesKey = key
			multiPageView.updatePages()
			// The rebuilt pages are new: the representation they missed is shown again
			container.doUpdateGui()
		}
	}

	Timer {
		id: pagesUpdateRetryTimer
		interval: 500
		onTriggered: {
			pagesUpdateTimer.restart()
		}
	}

	Connections {
		target: stateManager_
		function onIsNewTenantChanged() {
			pagesUpdateTimer.restart()
			if (!stateManager_.isNewTenant) {
				stateManager_.loadMembersFromModel()
				stateManager_.loadInvitationsFromModel()
			}
		}
		function onPagesConfigKeyChanged() {
			pagesUpdateTimer.restart()
		}
	}

	Connections {
		target: container.tenantData
		function onM_idChanged() {
			pagesUpdateTimer.restart()
		}
		// The editors of roles, groups and users offer the permission scope of the organization
		function onM_tenantPermissionsChanged() {
			if (container.apiClient)
				container.apiClient.invalidateTenantPermissions()
		}
	}

	// The reasons to read the organization again come in bursts: an own change and its notification,
	// a notification relayed by the API client and by AuthorizationController. One reading serves them all.
	function __refreshRepresentation() {
		if (!stateManager_.isNewTenant && container.representationController)
			representationRefreshTimer.restart()
	}

	Timer {
		id: representationRefreshTimer
		interval: 100
		onTriggered: {
			if (!stateManager_.isNewTenant && container.representationController)
				container.representationController.updateRepresentationFromDocument()
		}
	}

	// --- Refresh the document when the server confirms membership changes ---
	Connections {
		target: container.apiClient ? container.apiClient.userDocumentManager : null
		function onDocumentSaved(documentId) {
			container.__refreshRepresentation()
		}
	}

	Connections {
		target: container.apiClient
		function onConnectionRequestsReceived() {
			container.__updateAddressBookBadge()
		}
		function onInvitationCreated() {
			PopupManager.addSuccessMessage(qsTr("Invitation created successfully"), true)
		}
		function onOwnershipTransferred() {
			PopupManager.addSuccessMessage(qsTr("Ownership transferred successfully"), true)
		}
		function onRoleCreated() {
			PopupManager.addSuccessMessage(qsTr("Role created successfully"), true)
		}
		function onGroupCreated() {
			PopupManager.addSuccessMessage(qsTr("Group created successfully"), true)
		}
		function onUserCreated() {
			PopupManager.addSuccessMessage(qsTr("User created successfully"), true)
		}
		function onMemberRemoved(userId) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			// If the current user left/was removed, force-close the editor
			var currentUserId = container.tenantData.m_currentUserId || ""
			if (currentUserId && userId === currentUserId) {
				PopupManager.addInfoMessage(qsTr("You have left this organization. Closing editor."), true)
				if (AuthorizationController.currentTenantId === container.tenantData.m_id)
					AuthorizationController.selectTenant("")
				if (container.documentManager && container.documentId)
					container.documentManager.closeDocument(container.documentId)
			}
		}
		function onSubscriptionMembershipRemoved(tenantId, userId) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (tenantId === container.tenantData.m_id) {
				// If the removed member is the current user, force-close the document
				var currentUserId = container.tenantData.m_currentUserId || ""
				if (currentUserId && userId === currentUserId) {
					PopupManager.addInfoMessage(qsTr("You have been removed from this organization. Closing editor."), true)
					if (AuthorizationController.currentTenantId === container.tenantData.m_id)
						AuthorizationController.selectTenant("")
					if (container.documentManager && container.documentId)
						container.documentManager.closeDocument(container.documentId)
					return
				}
			}
		}
	}

	// Members and invitations are read again only when the server reports a change of them, whoever made it
	Connections {
		target: AuthorizationController

		function onTenantMembersChanged(tenantId) {
			if (!container.tenantData || stateManager_.isNewTenant)
				return
			if (tenantId === container.tenantData.m_id) {
				container.__refreshRepresentation()
			}
		}
	}

	// =========================================================================
	// Page factories — each delegates to a dedicated component file.
	// =========================================================================
	Component {
		id: generalPageComp

		TenantGeneralPage {
			model: container.tenantData
			stateManager: stateManager_
		}
	}

	Component {
		id: membersPageComp

		TenantMembersPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
			headerHost: container.__headerHost ? container : null
		}
	}

	Component {
		id: rolesPageComp

		TenantRolesPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
			headerHost: container.__headerHost ? container : null
		}
	}

	Component {
		id: groupsPageComp

		TenantGroupsPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
			headerHost: container.__headerHost ? container : null
		}
	}

	Component {
		id: permissionsPageComp

		TenantPermissionsPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
		}
	}


	Component {
		id: addressBookPageComp

		TenantPartnersPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
			canFindInDirectory: multiPageView.getIndexById("DirectorySearch") >= 0

			onFindInDirectoryRequested: {
				container.__openPage("DirectorySearch")
			}
		}
	}

	Component {
		id: directorySearchPageComp

		TenantDirectorySearchPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
			contractsApiClient: container.contractsApiClient
			canEditProfile: stateManager_.canEditDirectoryProfile
			headerHost: container.__headerHost ? container : null

			onProfileRequested: {
				container.__openPage("Discovery")
			}

			onAddressBookRequested: {
				container.__openPage("AddressBook")
			}
		}
	}

	Component {
		id: discoveryPageComp

		TenantConnectionCodePage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
			contractsApiClient: container.contractsApiClient
		}
	}

	Component {
		id: documentSharesPageComp

		TenantDocumentSharesPage {
		}
	}

	Component {
		id: contractsPageComp

		TenantContractsPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
			contractsApiClient: container.contractsApiClient
			headerHost: container.__headerHost ? container : null
			contractToOpenId: container.contractToOpenId
			onContractOpenHandled: container.contractOpenHandled()
		}
	}

	Component {
		id: partyRolesPageComp

		TenantPartyRolesPage {
			model: container.tenantData
			stateManager: stateManager_
			contractsApiClient: container.contractsApiClient
			headerHost: container.__headerHost ? container : null
		}
	}

	Component {
		id: contractTypesPageComp

		TenantContractTypesPage {
			model: container.tenantData
			stateManager: stateManager_
			contractsApiClient: container.contractsApiClient
			headerHost: container.__headerHost ? container : null
		}
	}

	Component {
		id: messagesPageComp

		TenantMessagesPage {
			model: container.tenantData
			stateManager: stateManager_
			apiClient: container.apiClient
			headerHost: container.__headerHost ? container : null
		}
	}
}
