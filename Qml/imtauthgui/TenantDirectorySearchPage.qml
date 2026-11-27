// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtauthgui 1.0

/**
 * TenantDirectorySearchPage
 *
 * Finds an organization in the directory, laid out like the roles and the groups of the organization.
 * The list is searched by the filter only: an empty filter shows nothing. The column "For you" tells how a found
 * organization stands to this one. A row opens its profile (DirectoryProfileDialog); "Add to address book"
 * asks the selected one to join. An organization not listed itself is offered to set up its profile.
 */
SimpleCollectionPage {
	id: searchPage

	entityName: qsTr("Organization")
	entityNamePlural: qsTr("Organizations")
	listHeaderTitle: qsTr("Find an organization")
	descriptionText: qsTr("The organizations listed in the directory. Type a part of the name, the description or the industry; open one to see its profile and ask it to join your address book.")
	filterPlaceholder: qsTr("Name, description or industry")
	emptyText: searchPage.filterText.trim() === ""
			   ? qsTr("Type a part of the name of an organization to find it.")
			   : qsTr("No organization found. Try another text, or invite a partner by its connection code in the address book.")
	showCreateButton: false

	// ContractsApiClient: the directory and the own profile
	property var contractsApiClient: null
	// The organization changes its own profile: a hidden one is offered to set it up
	property bool canEditProfile: false

	signal profileRequested()
	signal addressBookRequested()

	dataProvider: searchPage.contractsApiClient ? searchPage.contractsApiClient.directoryDataProvider : null

	editPermissionIds: ["/OrganizationManagement/Connections/Edit/Connect", "/OrganizationManagement/Contracts/Edit"]

	columnIds: ["title", "industry", "country", "relation"]
	columnNames: [qsTr("Organization"), qsTr("Industry"), qsTr("Country"), qsTr("For you")]
	columnDelegateIds: ["relation"]
	columnDelegateComponents: [relationCellComp]

	headerButtonsComponent: searchButtonsComp

	readonly property string __tenantId: searchPage.model && searchPage.model.m_id ? String(searchPage.model.m_id) : ""
	readonly property bool __canConnect: searchPage.stateManager ? searchPage.stateManager.canConnectOrganization : false
	readonly property bool isOwnProfileHidden: searchPage.canEditProfile && !!searchPage.contractsApiClient
		&& searchPage.contractsApiClient.directoryProfile !== null && searchPage.contractsApiClient.directoryProfile.isListed !== true
	// Grows when the address book changes, the column "For you" follows it
	property int __relationRevision: 0

	readonly property var __selectedItem: searchPage.selectionManager && searchPage.selectionManager.selectedIds.length === 1
		? searchPage.__findItemById(searchPage.selectionManager.selectedIds[0]) : null
	readonly property string __selectedRelation: searchPage.__relationRevision >= 0 && searchPage.__selectedItem
		? searchPage.relationOf(searchPage.__selectedItem.id) : ""

	function updateGui() {
		if (searchPage.apiClient && searchPage.__tenantId !== "") {
			searchPage.apiClient.fetchConnections(searchPage.__tenantId)
			searchPage.apiClient.fetchConnectionRequests(searchPage.__tenantId)
		}
		if (searchPage.canEditProfile && searchPage.contractsApiClient && searchPage.__tenantId !== "")
			searchPage.contractsApiClient.fetchDirectoryProfile(searchPage.__tenantId)
	}

	Component.onCompleted: {
		searchPage.updateGui()
	}

	// How an organization stands to this one: "partner", "sent", "received" or ""
	function relationOf(otherTenantId) {
		var connections = searchPage.apiClient ? searchPage.apiClient.connectionsModel : null
		if (connections) {
			for (var i = 0; i < connections.count; ++i) {
				var connection = connections.get(i)
				if (connection.status === "Active" && connection.partnerId === otherTenantId)
					return "partner"
			}
		}
		var requests = searchPage.apiClient ? searchPage.apiClient.connectionRequestsModel : null
		if (requests) {
			for (var j = 0; j < requests.count; ++j) {
				var request = requests.get(j)
				if (request.status !== "Pending")
					continue
				if (request.sourceTenantId === searchPage.__tenantId && request.targetTenantId === otherTenantId)
					return "sent"
				if (request.targetTenantId === searchPage.__tenantId && request.sourceTenantId === otherTenantId)
					return "received"
			}
		}
		return ""
	}

	function relationText(relation) {
		if (relation === "partner")
			return qsTr("In your address book")
		if (relation === "sent")
			return qsTr("Request sent")
		if (relation === "received")
			return qsTr("Asks to join yours")
		return ""
	}

	// A row opens the profile instead of an editor
	function openItem(itemId) {
		var item = searchPage.__findItemById(itemId)
		if (!item)
			return
		ModalDialogManager.openDialog(profileDialogComp, {"profile": searchPage.__profileOf(item),
														  "relation": searchPage.relationOf(item.id),
														  "canConnect": searchPage.__canConnect})
	}

	function invite(item) {
		if (item)
			ModalDialogManager.openDialog(inviteDialogComp, {"targetTenantId": item.id, "targetTenantName": item.title})
	}

	function __profileOf(item) {
		return {"id": item.id, "tenantName": item.title, "industry": item.industry, "country": item.country,
				"offeredRoles": item.offeredRoles, "description": item.description, "website": item.website,
				"acceptsInvitationsFromUnknown": item.acceptsInvitationsFromUnknown}
	}

	Connections {
		target: searchPage.apiClient

		function onConnectionsReceived() {
			searchPage.__relationRevision++
		}

		function onConnectionRequestsReceived() {
			searchPage.__relationRevision++
		}

		function onConnectionRequestCreated(requestId) {
			if (!searchPage.visible)
				return
			PopupManager.addSuccessMessage(qsTr("The request was sent. The organization joins your address book once it accepts."), true)
			searchPage.apiClient.fetchConnectionRequests(searchPage.__tenantId)
		}

		function onConnectionRequestError(errorMessage) {
			if (searchPage.visible)
				PopupManager.addErrorMessage(errorMessage, true)
		}
	}

	Component {
		id: searchButtonsComp

		Flow {
			id: searchButtons

			width: Math.min(searchPage.commandsLineWidth(searchButtons), searchPage.contentWidth)
			spacing: Style.marginL

			Text {
				text: qsTr("Set up your profile")
				visible: searchPage.isOwnProfileHidden
				width: Math.min(implicitWidth, searchPage.contentWidth)
				elide: Text.ElideRight
				font.pixelSize: Style.fontSizeM
				font.bold: true
				color: Style.linkColor

				MouseArea {
					anchors.fill: parent
					cursorShape: Qt.PointingHandCursor
					onClicked: {
						searchPage.profileRequested()
					}
				}
			}

			Text {
				text: qsTr("Profile")
				width: Math.min(implicitWidth, searchPage.contentWidth)
				elide: Text.ElideRight
				font.pixelSize: Style.fontSizeM
				font.bold: true
				color: searchPage.__selectedItem ? Style.linkColor : Style.inactiveTextColor
				opacity: searchPage.__selectedItem ? 1.0 : 0.5

				MouseArea {
					anchors.fill: parent
					enabled: searchPage.__selectedItem !== null
					cursorShape: searchPage.__selectedItem ? Qt.PointingHandCursor : Qt.ArrowCursor
					onClicked: {
						searchPage.openItem(searchPage.__selectedItem.id)
					}
				}
			}

			Text {
				text: qsTr("Add to address book")
				visible: searchPage.__canConnect
				width: Math.min(implicitWidth, searchPage.contentWidth)
				elide: Text.ElideRight
				font.pixelSize: Style.fontSizeM
				font.bold: true
				color: searchPage.__selectedItem && searchPage.__selectedRelation === "" ? Style.linkColor : Style.inactiveTextColor
				opacity: searchPage.__selectedItem && searchPage.__selectedRelation === "" ? 1.0 : 0.5

				MouseArea {
					anchors.fill: parent
					enabled: searchPage.__selectedItem !== null && searchPage.__selectedRelation === ""
					cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
					onClicked: {
						searchPage.invite(searchPage.__selectedItem)
					}
				}
			}
		}
	}

	Component {
		id: relationCellComp

		TableCellDelegateBase {
			id: relationCell

			readonly property var rowData: relationCell.getRowData()

			BaseText {
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				anchors.right: parent.right
				elide: Text.ElideRight
				text: searchPage.__relationRevision >= 0 && relationCell.rowData
					  ? searchPage.relationText(searchPage.relationOf(relationCell.rowData.id)) : ""
				font.pixelSize: Style.fontSizeM
				color: Style.textColor
			}
		}
	}

	Component {
		id: profileDialogComp

		DirectoryProfileDialog {
			id: profileDialog
			title: qsTr("Profile of the organization")
			onFinished: {
				if (buttonId == Enums.ok && profileDialog.profile)
					ModalDialogManager.openDialog(inviteDialogComp, {"targetTenantId": profileDialog.profile.id, "targetTenantName": profileDialog.profile.tenantName})
			}
		}
	}

	Component {
		id: inviteDialogComp

		PartnerInviteDialog {
			id: inviteDialog
			property string targetTenantId: ""
			title: qsTr("Add to address book")
			onFinished: {
				if (buttonId == Enums.ok && searchPage.apiClient && inviteDialog.contentItem)
					searchPage.apiClient.createConnectionRequest(searchPage.__tenantId, "", inviteDialog.contentItem.message, inviteDialog.targetTenantId)
			}
		}
	}
}
