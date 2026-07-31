// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ProfileAccessPage
 *
 * Roles & Permissions tab of the ProfileView — read-only tables of the roles,
 * groups and permissions granted to the current user for the active product.
 * Each table sits in the same bordered baseColor card + eyebrow-label idiom used
 * across the whole Profile dialog (General / Organizations / Access Tokens).
 *
 * Superusers implicitly have every permission and are never assigned explicit
 * roles/groups/permissions, so the tables would always be empty for them — this
 * page shows a short explanation instead of a confusing blank page.
 *
 * Transport-free: the page owns its document as ViewBase.model and subscribes
 * directly to `apiClient.profileReceived` — it does not rely on the value being
 * forwarded through the orchestrator, so it also picks up the cached
 * `apiClient.lastProfile` if it mounts after the fetch already returned.
 */
ViewBase {
	id: accessPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	property var apiClient: null
	readonly property var profileData: accessPage.model
	readonly property bool isSuperuser: AuthorizationController.loggedUserIsSuperuser()

	function applyProfile(profile) {
		if (!profile)
			return
		accessPage.model = profile
		accessPage.doUpdateGui()
	}

	Component.onCompleted: {
		if (accessPage.apiClient && accessPage.apiClient.lastProfile)
			accessPage.applyProfile(accessPage.apiClient.lastProfile)
	}

	Connections {
		target: accessPage.apiClient ? accessPage.apiClient : null
		function onProfileReceived(profile) {
			accessPage.applyProfile(profile)
		}
	}

	function updateGui() {
		if (!accessPage.profileData)
			return
		rolesTable.elements = accessPage.profileData.m_roles
		groupsTable.elements = accessPage.profileData.m_groups
		permissionsTable.elements = accessPage.profileData.m_permissions
	}

	CustomScrollbar {
		id: scrollbar
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
		anchors.margins: Style.marginXL
		contentWidth: width
		contentHeight: bodyColumn.height + Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn
			width: parent.width
			spacing: Style.marginXL

			// --- Superuser explanation ---
			Rectangle {
				width: parent.width
				height: superuserRow.height + 2 * Style.marginM
				visible: accessPage.isSuperuser
				radius: Style.radiusL
				color: Style.baseColor
				border.width: 1
				border.color: Style.borderColor

				Row {
					id: superuserRow
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					anchors.margins: Style.marginM
					spacing: Style.marginM

					Rectangle {
						width: 32
						height: 32
						radius: 16
						color: Style.imaginToolsAccentColor
						anchors.verticalCenter: parent.verticalCenter

						Text {
							anchors.centerIn: parent
							text: "★"
							color: Style.baseColor
							font.pixelSize: Style.fontSizeM
						}
					}

					Column {
						width: parent.width - 32 - parent.spacing
						spacing: Style.marginXS
						anchors.verticalCenter: parent.verticalCenter

						Text {
							text: qsTr("You have full access")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							font.family: Style.fontFamily
							color: Style.textColor
						}

						Text {
							text: qsTr("As a superuser, every permission is already granted to you, so no roles, groups or individual permissions are listed here — there is nothing more to add.")
							font.pixelSize: Style.fontSizeS
							font.family: Style.fontFamily
							color: Style.inactiveTextColor
							wrapMode: Text.WordWrap
							width: parent.width
						}
					}
				}
			}

			// --- Roles ---
			Column {
				width: parent.width
				spacing: Style.marginXS
				visible: !accessPage.isSuperuser && rolesTable.elementsCount > 0

				Text {
					text: qsTr("Roles")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				Rectangle {
					width: parent.width
					height: rolesTable.height + 2 * Style.marginL
					border.width: 1
					border.color: Style.borderColor
					radius: Style.radiusL
					color: Style.baseColor

					Table {
						id: rolesTable
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.margins: Style.marginL
						height: contentHeight + headerHeight
					}
				}
			}

			// --- Groups ---
			Column {
				width: parent.width
				spacing: Style.marginXS
				visible: !accessPage.isSuperuser && groupsTable.elementsCount > 0

				Text {
					text: qsTr("Groups")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				Rectangle {
					width: parent.width
					height: groupsTable.height + 2 * Style.marginL
					border.width: 1
					border.color: Style.borderColor
					radius: Style.radiusL
					color: Style.baseColor

					Table {
						id: groupsTable
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.margins: Style.marginL
						height: contentHeight + headerHeight
					}
				}
			}

			// --- Permissions ---
			Column {
				width: parent.width
				spacing: Style.marginXS
				visible: !accessPage.isSuperuser && permissionsTable.elementsCount > 0

				Text {
					text: qsTr("Permissions")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				Rectangle {
					width: parent.width
					height: permissionsTable.height + 2 * Style.marginL
					border.width: 1
					border.color: Style.borderColor
					radius: Style.radiusL
					color: Style.baseColor

					Table {
						id: permissionsTable
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.margins: Style.marginL
						height: contentHeight + headerHeight
					}
				}
			}

			Text {
				width: parent.width
				visible: !accessPage.isSuperuser && rolesTable.elementsCount === 0 && groupsTable.elementsCount === 0 && permissionsTable.elementsCount === 0
				text: qsTr("No roles, groups or permissions are assigned.")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				wrapMode: Text.WordWrap
			}
		}
	}

	// Populate first, THEN assign to each table — Table.headers is read once at
	// assignment time, so assigning it before addHeader() has run leaves the
	// table with no columns (and nothing renders even once elements arrive).
	TableHeaders {
		id: sharedHeadersModel

		Component.onCompleted: {
			addHeader("name", qsTr("Name"))
			addHeader("description", qsTr("Description"))
			rolesTable.headers = sharedHeadersModel
			groupsTable.headers = sharedHeadersModel
			permissionsTable.headers = sharedHeadersModel
		}
	}
}
