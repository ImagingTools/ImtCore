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
SimpleCollectionItemDelegateBase {
	id: row

	// --- Inputs ---
	property string kind: "member"                  // "member" | "invitation"
	property var memberData: ({})
	property var tenantData: null
	property var stateManager: null
	property bool isOwner: false

	// --- Outputs (action requests) ---
	signal memberActionsRequested(var menuItems, string userId, string userName,
	                              bool isOwnerTarget, bool isCurrentUserTarget,
	                              real menuX, real menuY)
	signal inviteActionsRequested(var menuItems, string invitationId, string userName,
	                              real menuX, real menuY)
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
	readonly property bool isExpired: !row.isMember && row.stateManager
		? row.stateManager.isInvitationExpired(row.memberData.expiresAt)
		: false
	readonly property bool isRevoked: !row.isMember
		&& (row.memberData.status === "revoked" || row.memberData.status === "Revoked")
	readonly property string effectiveStatus: row.isExpired
		? "Expired"
		: row.isRevoked ? "Revoked" : "Pending"
	readonly property bool canEditMember: row.stateManager ? row.stateManager.canChangeOrganizationMember : false
	readonly property bool canChangeMemberRole: row.stateManager ? row.stateManager.canChangeOrganizationMemberRole : false
	readonly property bool canManageOrganizationMembers: row.stateManager ? row.stateManager.canManageOrganizationMembers : false
	readonly property bool canRemoveMember: row.stateManager ? row.stateManager.canExcludeOrganizationMember : false
	readonly property bool canInviteMember: row.stateManager ? row.stateManager.canInviteOrganizationMember : false

	readonly property string selectionId: row.isMember ? (row.memberData.id || "") : ("inv_" + (row.memberData.id || ""))
	itemId: row.selectionId
	enableDefaultDoubleClickEdit: false
	showDefaultActionsMenu: false

	onItemDoubleClicked: {
		if (row.isMember && row.canEditMember) {
			row.memberEditRequested(row.memberData.id, row.memberData.name || row.memberData.id || "")
		}
	}

	Row {
		id: contentRow
		width: parent.width
		spacing: Style.marginM

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
				sourceSize.width: width
				sourceSize.height: height
			}
		}

		// ----- Info column -----
		Column {
			anchors.verticalCenter: parent.verticalCenter
			spacing: 2
			width: parent.width
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
					visible: row.isMember && (row.isMemberOwner || row.isMemberCreator)
					text: row.isMemberOwner ? qsTr("Owner") : qsTr("Creator")
					badgeColor: Style.selectedColor
					badgeBorderColor: Style.secondColor
					textColor: Style.secondColor
				}

				StatusBadge {
					visible: row.isMember
					text: qsTr("Active")
					badgeColor: Style.selectedColor
					badgeBorderWidth: 0
					textColor: Style.textColor
				}

				StatusBadge {
					readonly property int __permCount: row.isMember
						&& row.memberData
						&& Array.isArray(row.memberData.organizationPermissions)
						? row.memberData.organizationPermissions.length : 0
					visible: row.isMember && __permCount > 0
					text: qsTr("%1 perm%2").arg(__permCount).arg(__permCount === 1 ? "" : "s")
					badgeColor: Style.baseColor
					badgeBorderColor: Style.borderColor
					textColor: Style.inactiveTextColor
					horizontalPadding: Style.marginS
					verticalPadding: Style.marginXS
					fontPixelSize: Style.fontSizeXS
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

			readonly property bool __hasActions: row.isMember
				? (((row.canChangeMemberRole || row.canRemoveMember || row.isOwner) && !row.isCurrentUser && !row.isMemberOwner && !row.isMemberCreator)
				   || (row.isCurrentUser && !row.isMemberOwner && !row.isMemberCreator))
				: ((row.canInviteMember || row.canRemoveMember) && row.effectiveStatus === "Pending")

			ToolButton {
				id: actionsButton
				visible: actionsItem.__hasActions
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
					var btnPos = actionsButton.mapToItem(null, 0, actionsButton.height)
					if (row.isMember) {
						var menuItems = []
						if (!row.isCurrentUser) {
							if (!row.isMemberOwner && !row.isMemberCreator) {
								if (row.canChangeMemberRole || row.canManageOrganizationMembers)
									menuItems.push({ text: qsTr("Manage Organization Permissions"), action: "managePermissions" })
								if (row.canRemoveMember)
									menuItems.push({ text: qsTr("Exclude from Tenant"), action: "remove" })
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
							row.isCurrentUser,
							btnPos.x, btnPos.y)
					} else {
						var inviteMenuItems = []
						if (row.canInviteMember)
							inviteMenuItems.push({ text: qsTr("Resend Invitation"), action: "resend" })
						if (row.canRemoveMember)
							inviteMenuItems.push({ text: qsTr("Revoke Invitation"), action: "revoke" })
						if (inviteMenuItems.length === 0)
							return
						row.inviteActionsRequested(inviteMenuItems,
							row.memberData.id,
							row.memberData.userName || row.memberData.userId || "",
							btnPos.x, btnPos.y)
					}
				}
			}
		}
	}
}
