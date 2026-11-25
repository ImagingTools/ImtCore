// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantConnectionCodePage
 *
 * How other organizations find this one: its connection code with the switch of connecting by it,
 * and its profile in the directory of the organizations (TenantDirectoryProfileSection).
 * The switch applies at once; the profile is saved by its own command.
 */
ViewBase {
	id: connectionCodePage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: connectionCodePage.model
	property var stateManager: null
	property var apiClient: null
	// ContractsApiClient: the directory profile; without it the section is not shown
	property var contractsApiClient: null
	readonly property bool __canViewConnectionCode: connectionCodePage.stateManager ? connectionCodePage.stateManager.canViewOrganizationConnectionCode : false
	readonly property bool __canEditDirectoryProfile: !!connectionCodePage.contractsApiClient
		&& (connectionCodePage.stateManager ? connectionCodePage.stateManager.canEditDirectoryProfile : false)
	readonly property bool __canManageConnections: connectionCodePage.stateManager ? connectionCodePage.stateManager.canManageOrganizationConnections : false

	property string connectionCode: ""
	property bool allowConnectionsByCode: true
	property bool loading: false
	// The switch shows the received state before it sends changes
	property bool __ready: false

	function updateGui() {
		if (connectionCodePage.apiClient && connectionCodePage.tenantData
				&& connectionCodePage.tenantData.m_id) {
			connectionCodePage.loading = true
			connectionCodePage.apiClient.getConnectionCode(connectionCodePage.tenantData.m_id)
		}
	}

	Component.onCompleted: {
		connectionCodePage.updateGui()
	}

	function copyCode() {
		clipboardHelper.selectAll()
		clipboardHelper.copy()
		PopupManager.addSuccessMessage(qsTr("The code was copied"), true)
	}

	function regenerateCode() {
		ModalDialogManager.showConfirmationDialog(
			qsTr("Change the connection code"),
			qsTr("The organizations that have the old code cannot use it any more. Change the code?"),
			function(result) {
				if (result === Enums.yes && connectionCodePage.apiClient && connectionCodePage.tenantData)
					connectionCodePage.apiClient.regenerateConnectionCode(connectionCodePage.tenantData.m_id)
			})
	}

	Connections {
		target: connectionCodePage.apiClient

		function onConnectionCodeReceived(code, allowByCode) {
			connectionCodePage.__ready = false
			connectionCodePage.connectionCode = code
			connectionCodePage.allowConnectionsByCode = allowByCode
			allowSwitch.checked = allowByCode
			connectionCodePage.loading = false
			connectionCodePage.__ready = true
		}

		function onConnectionCodeRegenerated(newCode) {
			connectionCodePage.connectionCode = newCode
			PopupManager.addSuccessMessage(qsTr("The code was changed. Give the new one to the organizations that should connect."), true)
		}

		function onAllowConnectionsByCodeChanged(allow) {
			connectionCodePage.allowConnectionsByCode = allow
		}
	}

	TextEdit {
		id: clipboardHelper
		visible: false
		text: connectionCodePage.connectionCode
	}

	CustomScrollbar {
		z: pageFlickable.z + 1
		anchors.right: parent.right
		anchors.top: pageFlickable.top
		anchors.bottom: pageFlickable.bottom
		secondSize: Style.marginM
		targetItem: pageFlickable
	}

	Flickable {
		id: pageFlickable

		anchors.fill: parent
		anchors.topMargin: Style.marginXL
		contentHeight: pageColumn.height + Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: pageColumn

			anchors.horizontalCenter: parent.horizontalCenter
			width: Math.min(pageFlickable.width - Style.marginXL * 2, Style.contentWidthMax)
			spacing: Style.marginL

			// --- Connection code ---
			Column {
				width: parent.width
				visible: connectionCodePage.__canViewConnectionCode
				spacing: Style.marginM

				GroupHeaderView {
					width: parent.width
					title: qsTr("Connection code")
				}

				BaseText {
					width: parent.width
					wrapMode: Text.WordWrap
					text: qsTr("Give the code to an organization you know: with it, it sends your organization a request to join the address book.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				GroupElementView {
					width: parent.width

					ElementView {
						name: qsTr("Your code")
						controlComp: Component {
							Row {
								spacing: Style.marginM

								BaseText {
									anchors.verticalCenter: parent.verticalCenter
									text: connectionCodePage.connectionCode !== "" ? connectionCodePage.connectionCode : qsTr("Loading...")
									font.pixelSize: Style.fontSizeL
									font.bold: true
									color: Style.textColor
								}

								ToolButton {
									anchors.verticalCenter: parent.verticalCenter
									width: Style.buttonWidthL
									height: Style.buttonWidthL
									enabled: connectionCodePage.connectionCode !== ""
									tooltipText: qsTr("Copy the code")
									iconSource: "../../../" + Style.getIconPath("Icons/Copy", Icon.State.On, Icon.Mode.Normal)
									onClicked: {
										connectionCodePage.copyCode()
									}
								}

								Button {
									anchors.verticalCenter: parent.verticalCenter
									visible: connectionCodePage.__canManageConnections
									text: qsTr("Change code")
									onClicked: {
										connectionCodePage.regenerateCode()
									}
								}
							}
						}
					}

					SwitchElementView {
						id: allowSwitch
						name: qsTr("Connecting by code")
						description: allowSwitch.checked
									 ? qsTr("Organizations can send a request by this code")
									 : qsTr("Nobody can send a request by the code now")
						readOnly: !connectionCodePage.__canManageConnections || !connectionCodePage.__ready
						onCheckedChanged: {
							if (!connectionCodePage.__ready || allowSwitch.checked === connectionCodePage.allowConnectionsByCode)
								return
							if (connectionCodePage.apiClient && connectionCodePage.tenantData)
								connectionCodePage.apiClient.setAllowConnectionsByCode(connectionCodePage.tenantData.m_id, allowSwitch.checked)
						}
					}
				}
			}

			// --- Directory of the organizations ---
			TenantDirectoryProfileSection {
				id: directorySection

				width: parent.width
				visible: connectionCodePage.__canEditDirectoryProfile
				tenantId: connectionCodePage.tenantData && connectionCodePage.tenantData.m_id ? String(connectionCodePage.tenantData.m_id) : ""
				contractsApiClient: connectionCodePage.__canEditDirectoryProfile ? connectionCodePage.contractsApiClient : null
				canEdit: connectionCodePage.__canEditDirectoryProfile
			}
		}
	}
}
