// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtauthgui 1.0
import imtauthUsersSdl 1.0

/**
 * TenantGeneralPage
 *
 * General tab of the TenantEditor — basic workspace settings.
 *
 * Inherits ViewBase so that updates flow through the protected
 * doUpdateGui / doUpdateModel wrappers (re-entrance guard).
 *
 * Uses the same layout as the document editors: a section title over a card of label | field rows.
 */
ViewBase {
	id: generalPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	// Backward-compat accessor: pages reference tenantData; ViewBase exposes it as `model`.
	readonly property var tenantData: generalPage.model
	property var stateManager: null

	readonly property bool __canEditName: generalPage.stateManager
		? (generalPage.stateManager.isNewTenant || generalPage.stateManager.canChangeOrganizationName)
		: false
	readonly property bool __canEditDescription: generalPage.stateManager
		? (generalPage.stateManager.isNewTenant || generalPage.stateManager.canChangeOrganizationDescription)
		: false
	readonly property bool __canEditIsActive: generalPage.stateManager
		? (generalPage.stateManager.isNewTenant || generalPage.stateManager.canEditOrganization)
		: false
	readonly property bool __isNewTenant: generalPage.stateManager ? generalPage.stateManager.isNewTenant : false

	function updateGui() {
		if (!generalPage.tenantData) return
		nameInput.text = generalPage.tenantData.m_name || ""
		descriptionInput.text = generalPage.tenantData.m_description || ""
		isActiveSwitch.checked = generalPage.tenantData.m_isActive !== undefined
			? generalPage.tenantData.m_isActive
			: true
		ownerText.text = generalPage.tenantData.m_ownerName
		ownerSelect.selectedId = generalPage.tenantData.m_ownerId
		ownerSelect.selectedText = generalPage.tenantData.m_ownerName
	}

	// The creator picks the owner of a new organization; the owner holds every available permission
	function __selectOwner(userId, userName) {
		if (!generalPage.tenantData || userId === "")
			return
		generalPage.tenantData.m_ownerId = userId
		generalPage.tenantData.m_ownerName = userName
		generalPage.doUpdateModel()
	}

	function updateModel() {
		if (!generalPage.tenantData) return
		generalPage.tenantData.m_name = nameInput.text
		generalPage.tenantData.m_description = descriptionInput.text
		generalPage.tenantData.m_isActive = isActiveSwitch.checked
	}

	CustomScrollbar {
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: flickable.top
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
		targetItem: flickable
	}

	Flickable {
		id: flickable

		anchors.fill: parent
		anchors.topMargin: Style.marginXL
		contentHeight: mainColumn.height + Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: mainColumn

			anchors.horizontalCenter: parent.horizontalCenter
			width: Math.min(parent.width - Style.marginXL * 2, Style.contentWidthMax)
			spacing: Style.marginM

			GroupHeaderView {
				width: parent.width
				title: qsTr("Organization")
			}

			GroupElementView {
				width: parent.width

				TextInputElementView {
					id: nameInput

					name: qsTr("Name")
					placeHolderText: qsTr("Enter the organization name")
					readOnly: !generalPage.__canEditName

					onEditingFinished: {
						if (generalPage.tenantData && generalPage.tenantData.m_name !== nameInput.text)
							generalPage.doUpdateModel()
					}
				}

				TextInputElementView {
					id: descriptionInput

					name: qsTr("Description")
					placeHolderText: qsTr("Enter the description")
					readOnly: !generalPage.__canEditDescription

					onEditingFinished: {
						if (generalPage.tenantData && generalPage.tenantData.m_description !== descriptionInput.text)
							generalPage.doUpdateModel()
					}
				}

				TextInputElementView {
					id: parentInput
					name: qsTr("Parent organization")
					readOnly: true
					visible: generalPage.stateManager ? (generalPage.stateManager.isNewTenant && generalPage.stateManager.isSubOrganization) : false
					text: AuthorizationController.currentTenantName
				}

				// Until the organization is saved the creator picks its owner; afterwards it is transferred on the Members page
				CollectionSelectElementView {
					id: ownerSelect
					visible: generalPage.__isNewTenant
					name: qsTr("Owner")
					description: qsTr("The owner holds every available permission of the organization")
					commandId: ImtauthUsersSdlCommandIds.s_usersList
					fields: [UserItemDataTypeMetaInfo.s_id, UserItemDataTypeMetaInfo.s_name]
					titleField: UserItemDataTypeMetaInfo.s_name
					textFilterFieldIds: [UserItemDataTypeMetaInfo.s_name]
					sortByField: UserItemDataTypeMetaInfo.s_name
					placeHolderText: qsTr("You (the creator)")
					filterPlaceholder: qsTr("Search for a user")

					onItemSelected: {
						generalPage.__selectOwner(itemId, item ? item.title : "")
					}
				}

				TextElementView {
					id: ownerText
					visible: !generalPage.__isNewTenant
					name: qsTr("Owner")
				}

				SwitchElementView {
					id: isActiveSwitch

					name: qsTr("Active")
					readOnly: !generalPage.__canEditIsActive

					onCheckedChanged: {
						generalPage.doUpdateModel()
					}
				}
			}
		}
	}
}
