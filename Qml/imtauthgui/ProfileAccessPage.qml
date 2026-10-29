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
 * groups and permissions the current user holds for the active product, each
 * with the paths it comes through: assigned directly, through a group, a parent
 * role or a delegation, or required by another held permission.
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

	property var roleRows: []
	property var groupRows: []
	property var permissionRows: []
	property int selectedPermissionIndex: -1

	function updateGui() {
		if (!accessPage.profileData)
			return
		accessPage.roleRows = accessPage.createRows(accessPage.profileData.m_roles)
		accessPage.groupRows = accessPage.createRows(accessPage.profileData.m_groups)
		let permissionRows = accessPage.createRows(accessPage.profileData.m_permissions)
		permissionRows.sort(function(a, b) { return a.fullPath.localeCompare(b.fullPath) })
		accessPage.permissionRows = permissionRows
		accessPage.selectedPermissionIndex = -1
		rolesTable.elements = accessPage.roleRows
		groupsTable.elements = accessPage.groupRows
		permissionsTable.elements = accessPage.permissionRows
		accessPage.updateWarnings()
	}

	function kindText(kind) {
		let kindId = String(kind)
		if (kindId === "Group")
			return qsTr("Group")
		if (kindId === "ParentGroup")
			return qsTr("Parent group")
		if (kindId === "Role")
			return qsTr("Role")
		if (kindId === "ParentRole")
			return qsTr("Parent role")
		if (kindId === "DelegatedRole")
			return qsTr("Delegated role")
		if (kindId === "Requirement")
			return qsTr("Required by")
		return kindId
	}

	// Rows of a table from an SDL list of Assignment: name, id, the paths it comes through and a short mark.
	function createRows(assignments) {
		let rows = []
		for (let i = 0; assignments && i < assignments.count; i++) {
			let item = assignments.get(i).item
			let itemId = String(item.m_id)
			let direct = item.m_direct === true
			let paths = direct ? [qsTr("Assigned to you")] : []
			let onlyRequired = !direct
			let sources = item.m_sources
			for (let k = 0; sources && k < sources.count; k++) {
				let steps = sources.get(k).item.m_steps
				let parts = []
				let lastKind = ""
				for (let s = 0; steps && s < steps.count; s++) {
					let step = steps.get(s).item
					lastKind = String(step.m_kind)
					// A required permission is named by its path; roles and groups by their names.
					let stepName = lastKind === "Requirement" || !step.m_name ? String(step.m_id) : String(step.m_name)
					parts.push(accessPage.kindText(step.m_kind) + " " + stepName)
				}
				if (parts.length > 0)
					paths.push(parts.join(" " + String.fromCharCode(0x2192) + " "))
				if (lastKind !== "Requirement")
					onlyRequired = false
			}
			rows.push({
				id: itemId,
				name: item.m_name ? String(item.m_name) : itemId,
				fullPath: itemId,
				source: paths.join("; "),
				paths: paths,
				badge: onlyRequired && paths.length > 0 ? qsTr("Required") : ""
			})
		}
		rows.sort(function(a, b) { return a.name.localeCompare(b.name) })
		return rows
	}

	function updateWarnings() {
		let lines = []
		let warnings = accessPage.profileData ? accessPage.profileData.m_accessWarnings : null
		for (let i = 0; warnings && i < warnings.count; i++)
			lines.push(String(warnings.get(i).item.m_message))
		warningsText.text = lines.join("\n")
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
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: permissionDetails.visible ? permissionDetails.top : parent.bottom
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
				visible: !accessPage.isSuperuser && accessPage.roleRows.length > 0

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
						objectName: "ProfileRolesTable"
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.margins: Style.marginL
						height: contentHeight + headerHeight
						canMoveColumns: false
						selectable: false
					}
				}
			}

			// --- Groups ---
			Column {
				width: parent.width
				spacing: Style.marginXS
				visible: !accessPage.isSuperuser && accessPage.groupRows.length > 0

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
						objectName: "ProfileGroupsTable"
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.margins: Style.marginL
						height: contentHeight + headerHeight
						canMoveColumns: false
						selectable: false
					}
				}
			}

			// --- Permissions ---
			Column {
				width: parent.width
				spacing: Style.marginXS
				visible: !accessPage.isSuperuser && accessPage.permissionRows.length > 0

				Text {
					text: qsTr("Permissions")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				Text {
					id: warningsText
					objectName: "ProfileAccessWarnings"
					width: parent.width
					visible: text !== ""
					color: Style.errorTextColor
					font.family: Style.fontFamily
					font.pixelSize: Style.fontSizeS
					wrapMode: Text.WordWrap
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
						objectName: "ProfilePermissionsTable"
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.margins: Style.marginL
						height: contentHeight + headerHeight
						canMoveColumns: false
						isMultiSelect: false

						onSelectionChanged: {
							accessPage.selectedPermissionIndex = selection.length > 0 ? selection[0] : -1
						}
					}
				}
			}

			Text {
				width: parent.width
				visible: !accessPage.isSuperuser && accessPage.roleRows.length === 0 && accessPage.groupRows.length === 0 && accessPage.permissionRows.length === 0
				text: qsTr("No roles, groups or permissions are assigned.")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				wrapMode: Text.WordWrap
			}
		}
	}

	// Stays under the scrolled content, so it is in view whichever row is selected.
	PermissionDetailsPanel {
		id: permissionDetails
		objectName: "ProfilePermissionDetails"

		readonly property var row: accessPage.selectedPermissionIndex >= 0 && accessPage.selectedPermissionIndex < accessPage.permissionRows.length
			? accessPage.permissionRows[accessPage.selectedPermissionIndex] : null

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.leftMargin: Style.marginXL
		anchors.rightMargin: Style.marginXL
		anchors.bottomMargin: Style.marginXL
		visible: !accessPage.isSuperuser && accessPage.permissionRows.length > 0
		path: permissionDetails.row ? permissionDetails.row.fullPath : ""
		badge: permissionDetails.row ? permissionDetails.row.badge : ""
		source: permissionDetails.row ? permissionDetails.row.source : ""
		showDescription: false
		showRequires: false
	}

	// Populate first, THEN assign to each table — Table.headers is read once at
	// assignment time, so assigning it before addHeader() has run leaves the
	// table with no columns (and nothing renders even once elements arrive).
	TableHeaders {
		id: assignmentHeaders

		Component.onCompleted: {
			addHeader("name", qsTr("Name"), false, false)
			addHeader("source", qsTr("Source"), false, false)
			rolesTable.headers = assignmentHeaders
			groupsTable.headers = assignmentHeaders
			rolesTable.setColumnContentById("source", sourceCellComp)
			groupsTable.setColumnContentById("source", sourceCellComp)
		}
	}

	// A long source is cut in the cell, the tooltip shows it whole, one path per line.
	Component {
		id: sourceCellComp

		TableCellDelegateBase {
			id: sourceCell

			readonly property var rows: sourceCell.table ? sourceCell.table.elements : null
			readonly property var row: sourceCell.rows && sourceCell.rowIndex >= 0 && sourceCell.rowIndex < sourceCell.rows.length
				? sourceCell.rows[sourceCell.rowIndex] : null

			BaseText {
				objectName: "SourceText"
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				text: sourceCell.row ? sourceCell.row.source : ""
				font.pixelSize: Style.fontSizeM
				color: Style.textColor
				elide: Text.ElideRight
			}

			MouseArea {
				id: sourceMouseArea
				anchors.fill: parent
				hoverEnabled: true
				acceptedButtons: Qt.NoButton
			}

			TooltipArea {
				text: sourceCell.row ? sourceCell.row.paths.join("\n") : ""
				mouseArea: sourceMouseArea
			}
		}
	}

	TableHeaders {
		id: permissionHeaders

		Component.onCompleted: {
			addHeader("fullPath", qsTr("Full path"), false, false)
			permissionsTable.headers = permissionHeaders
			permissionsTable.setColumnContentById("fullPath", permissionPathCellComp)
		}
	}

	// The path with "Required" on the right for what other held permissions require.
	Component {
		id: permissionPathCellComp

		TableCellDelegateBase {
			id: permissionPathCell

			readonly property var row: permissionPathCell.rowIndex >= 0 && permissionPathCell.rowIndex < accessPage.permissionRows.length
				? accessPage.permissionRows[permissionPathCell.rowIndex] : null

			Item {
				anchors.fill: parent

				BaseText {
					objectName: "PermissionPath"
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: parent.left
					anchors.leftMargin: Style.marginM
					anchors.right: badgeItem.visible ? badgeItem.left : parent.right
					anchors.rightMargin: Style.marginM
					text: permissionPathCell.row ? permissionPathCell.row.fullPath : ""
					font.pixelSize: Style.fontSizeM
					color: Style.textColor
					elide: Text.ElideLeft
				}

				StatusBadge {
					id: badgeItem
					objectName: "PermissionBadge"
					anchors.verticalCenter: parent.verticalCenter
					anchors.right: parent.right
					anchors.rightMargin: Style.marginM
					visible: permissionPathCell.row !== null && permissionPathCell.row.badge !== ""
					text: permissionPathCell.row ? permissionPathCell.row.badge : ""
					textColor: Style.inactiveTextColor
				}
			}
		}
	}
}
