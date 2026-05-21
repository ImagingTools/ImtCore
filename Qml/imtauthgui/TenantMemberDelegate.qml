// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantMemberDelegate
 *
 * Single row delegate for either an active tenant member or a pending invitation.
 *
 * Set `kind` to "member" or "invitation". `memberData` is the modelData object
 * (shape from TenantEditorStateManager.pendingMembers / pendingInvitations).
 *
 * Action requests are surfaced as signals so the parent (TenantMembersPage)
 * can pop the appropriate menu and call the api client.
 */
Rectangle {
	id: row

	// --- Inputs ---
	property string kind: "member"                  // "member" | "invitation"
	property var memberData: ({})
	property var tenantData: null
	property var stateManager: null
	property bool canManageMembers: false
	property bool isOwner: false
	property var selectionManager: null
	property bool showCheckBox: false

	// --- Outputs (action requests) ---
	signal memberActionsRequested(var menuItems, string userId, string userName,
	                              bool isOwnerTarget, bool isCurrentUserTarget)
	signal inviteActionsRequested(var menuItems, string invitationId, string userName)
	signal memberEditRequested(string userId, string userName)

	readonly property bool isMember: row.kind === "member"

	readonly property bool isMemberOwner: row.isMember && row.tenantData
		&& row.memberData.id === row.tenantData.m_ownerId
	readonly property bool isMemberCreator: row.isMember && row.tenantData
		&& row.tenantData.m_creatorId
		&& row.memberData.id === row.tenantData.m_creatorId
	readonly property bool isCurrentUser: row.isMember && row.tenantData
		&& row.tenantData.m_currentUserId
		&& row.memberData.id === row.tenantData.m_currentUserId
	readonly property string memberRole: row.isMemberCreator
		? "Creator"
		: row.isMemberOwner
			? "Owner"
			: (row.stateManager ? row.stateManager.getUserRole(row.memberData.id) : "Member")

	readonly property bool isExpired: !row.isMember && row.stateManager
		? row.stateManager.isInvitationExpired(row.memberData.expiresAt)
		: false
	readonly property bool isRevoked: !row.isMember
		&& (row.memberData.status === "revoked" || row.memberData.status === "Revoked")
	readonly property string effectiveStatus: row.isExpired
		? "Expired"
		: row.isRevoked ? "Revoked" : "Pending"

	readonly property string selectionId: row.isMember ? (row.memberData.id || "") : ("inv_" + (row.memberData.id || ""))
	readonly property bool isSelected: row.selectionManager ? row.selectionManager.isSelected(row.selectionId) : false

	height: contentRow.implicitHeight + Style.marginL * 2
	radius: 0
	color: row.isSelected ? Style.selectedColor
		: rowMouseArea.containsMouse ? Style.buttonHoverColor : "transparent"

	MouseArea {
		id: rowMouseArea
		anchors.fill: parent
		hoverEnabled: true
		acceptedButtons: Qt.LeftButton
		onDoubleClicked: {
			if (row.isMember && row.canManageMembers) {
				row.memberEditRequested(row.memberData.id, row.memberData.name || row.memberData.id || "")
			}
		}
	}

	Row {
		id: contentRow
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		anchors.margins: Style.marginM
		spacing: Style.marginM

		// ----- CheckBox -----
		CheckBox {
			visible: row.showCheckBox
			anchors.verticalCenter: parent.verticalCenter
			height: Style.itemSizeS
			width: visible ? Style.itemSizeS : 0
			checkState: row.isSelected ? Qt.Checked : Qt.Unchecked
			onCheckStateChanged: {
				if (!row.selectionManager) return
				var shouldBeSelected = (checkState === Qt.Checked)
				var currentlySelected = row.selectionManager.isSelected(row.selectionId)
				if (shouldBeSelected !== currentlySelected)
					row.selectionManager.toggleSelect(row.selectionId)
			}
		}

		// ----- Avatar -----
		Rectangle {
			id: avatar
			width: Style.iconSizeL
			height: Style.iconSizeL
			radius: width / 2
			anchors.verticalCenter: parent.verticalCenter
			color: row.isMember
				? ((row.isMemberOwner || row.isMemberCreator) ? Style.selectedColor : Style.borderColor)
				: "transparent"
			border.color: row.isMember ? "transparent" : Style.inactiveTextColor
			border.width: row.isMember ? 0 : 1

			BaseText {
				anchors.centerIn: parent
				visible: row.isMember
				text: (row.memberData.name || row.memberData.id)
					? (row.memberData.name || row.memberData.id).charAt(0).toUpperCase()
					: "?"
				font.pixelSize: Style.fontSizeM
				font.bold: true
				color: Style.textColor
			}

			BaseText {
				anchors.centerIn: parent
				visible: !row.isMember
				text: "✉"
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
			}

			Image {
				visible: row.isMember && (row.isMemberOwner || row.isMemberCreator)
				anchors.top: parent.top
				anchors.right: parent.right
				anchors.topMargin: -Style.marginXS
				anchors.rightMargin: -Style.marginXS
				width: Style.fontSizeS
				height: Style.fontSizeS
				source: "qrc:/" + Style.getIconPath("Icons/Crown", Icon.State.On, Icon.Mode.Normal)
			}
		}

		// ----- Info column -----
		Column {
			anchors.verticalCenter: parent.verticalCenter
			spacing: 2
			width: parent.width
				- (row.showCheckBox ? Style.itemSizeS + parent.spacing : 0)
				- avatar.width
				- badgesItem.width
				- actionsItem.width
				- parent.spacing * 3

			Row {
				spacing: Style.marginS
				width: parent.width

				BaseText {
					text: row.isMember
						? (row.memberData.name || row.memberData.id || "")
						: (row.memberData.userName || row.memberData.userId || "")
					font.bold: row.isMember && (row.isMemberOwner || row.isMemberCreator)
					font.pixelSize: Style.fontSizeL
					color: Style.textColor
					elide: Text.ElideRight
				}

				Rectangle {
					visible: row.isMember && row.isCurrentUser
					anchors.verticalCenter: parent.verticalCenter
					width: youText.implicitWidth + Style.marginS
					height: youText.implicitHeight + 2
					radius: Style.radiusS
					color: Style.selectedColor

					BaseText {
						id: youText
						anchors.centerIn: parent
						text: qsTr("You")
						font.pixelSize: Style.fontSizeXS
						color: Style.linkColor
					}
				}
			}

			BaseText {
				visible: !row.isMember
				text: row.stateManager
					? row.stateManager.formatInvitationInfo(
						row.memberData.invitedByName || row.memberData.invitedByUserId || "",
						row.memberData.expiresAt || "")
					: ""
				font.pixelSize: Style.fontSizeS
				color: row.isExpired ? Style.errorTextColor : Style.inactiveTextColor
				elide: Text.ElideRight
				width: parent.width
			}
		}

		// ----- Status / Role badges -----
		Item {
			id: badgesItem
			anchors.verticalCenter: parent.verticalCenter
			width: badgesRow.implicitWidth
			height: badgesRow.implicitHeight

			Row {
				id: badgesRow
				spacing: Style.marginXS

				StatusBadge {
					visible: row.isMember
					text: row.memberRole
					badgeColor: (row.isMemberOwner || row.isMemberCreator) ? Style.selectedColor : Style.baseColor
					badgeBorderColor: (row.isMemberOwner || row.isMemberCreator) ? Style.secondColor : Style.borderColor
					textColor: (row.isMemberOwner || row.isMemberCreator) ? Style.secondColor : Style.textColor
				}

				StatusBadge {
					visible: row.isMember
					text: qsTr("Active")
					badgeColor: Style.selectedColor
					badgeBorderWidth: 0
					textColor: Style.textColor
				}

				StatusBadge {
					visible: !row.isMember
					text: row.effectiveStatus === "Expired"
						? qsTr("Expired")
						: row.effectiveStatus === "Revoked"
							? qsTr("Revoked")
							: qsTr("Pending")
					badgeColor: Style.backgroundColor2
					badgeBorderWidth: 0
					textColor: row.effectiveStatus === "Expired" ? Style.errorTextColor : Style.textColor
					horizontalPadding: Style.marginM
					verticalPadding: Style.marginXS
					fontPixelSize: Style.fontSizeXS
				}
			}
		}

		// ----- Actions column -----
		Item {
			id: actionsItem
			width: Style.iconSizeL
			height: parent.height
			anchors.verticalCenter: parent.verticalCenter

			ToolButton {
				visible: row.isMember
					? (row.canManageMembers || row.isCurrentUser)
					: (row.canManageMembers && row.effectiveStatus === "Pending")
				anchors.centerIn: parent
				tooltipText: qsTr("Actions")
				iconSource: "qrc:/" + Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal)
				decorator: Component {
					ToolButtonDecorator {
						color: "transparent"
						icon.width: Style.iconSizeXS
					}
				}

				onClicked: {
					if (row.isMember) {
						var menuItems = []
						if (row.canManageMembers && !row.isCurrentUser) {
							if (!row.isMemberOwner && !row.isMemberCreator) {
								menuItems.push({ text: qsTr("Change Environment Role"), action: "changeRole" })
								menuItems.push({ text: qsTr("Remove Member"), action: "remove" })
							}
							if (row.isOwner && !row.isMemberOwner && !row.isMemberCreator) {
								menuItems.push({ text: qsTr("Transfer Ownership"), action: "transfer" })
							}
						}
						if (row.isCurrentUser && !row.isMemberOwner && !row.isMemberCreator) {
							menuItems.push({ text: qsTr("Leave Workspace"), action: "leave" })
						}
						if (menuItems.length === 0) return
						row.memberActionsRequested(menuItems,
							row.memberData.id,
							row.memberData.name || row.memberData.id,
							row.isMemberOwner,
							row.isCurrentUser)
					} else {
						var inviteMenuItems = [
							{ text: qsTr("Resend Invitation"), action: "resend" },
							{ text: qsTr("Revoke Invitation"), action: "revoke" }
						]
						row.inviteActionsRequested(inviteMenuItems,
							row.memberData.id,
							row.memberData.userName || row.memberData.userId || "")
					}
				}
			}
		}
	}

	// Bottom separator line
	Rectangle {
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		height: 1
		color: Style.borderColor
		opacity: 0.5
	}
}
