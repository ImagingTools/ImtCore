// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ProfileOrganizationsPage
 *
 * Organizations tab of the ProfileView — a "you are here" callout for the active
 * organization, pending tenant invitations (accept / reject), and the list of
 * other organizations the user belongs to (switch / leave).
 *
 * Transport-free: invitation accept/reject and leaving an organization go through
 * the injected `apiClient` (ProfileApiClient contract); switching is a client-side
 * action on AuthorizationController. The page owns its organizations list locally
 * and subscribes directly to `apiClient.organizationsReceived` — it does not rely
 * on the value being forwarded through the orchestrator, so it also picks up the
 * cached `apiClient.lastOrganizations` if it mounts after the fetch already
 * returned.
 */
ViewBase {
	id: organizationsPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	property var apiClient: null
	// Plain JS array of { id, name, description, isDelegated }.
	property var organizationsList: []

	function __currentOrg() {
		var list = organizationsPage.organizationsList
		for (var i = 0; i < list.length; i++) {
			if (list[i].id === AuthorizationController.currentTenantId)
				return list[i]
		}
		return null
	}

	function __otherOrgs() {
		var list = organizationsPage.organizationsList
		var others = []
		for (var i = 0; i < list.length; i++) {
			if (list[i].id !== AuthorizationController.currentTenantId)
				others.push(list[i])
		}
		return others
	}

	function applyOrganizations(list) {
		organizationsPage.organizationsList = list || []
	}

	Component.onCompleted: {
		if (organizationsPage.apiClient)
			organizationsPage.applyOrganizations(organizationsPage.apiClient.lastOrganizations)
	}

	Connections {
		target: organizationsPage.apiClient ? organizationsPage.apiClient : null
		function onOrganizationsReceived(list) {
			organizationsPage.applyOrganizations(list)
		}
		function onInvitationAccepted() {
			banner.show(qsTr("Invitation accepted"), false)
		}
		function onInvitationRejected() {
			banner.show(qsTr("Invitation declined"), false)
		}
		function onLeftTenant(tenantId) {
			banner.show(qsTr("You have left the organization"), false)
		}
		function onOrganizationOperationFailed(message) {
			banner.show(message || qsTr("Unable to complete the request."), true)
		}
	}

	function __confirmLeaveTenant(tenantId, tenantName) {
		ModalDialogManager.openDialog(__leaveConfirmDialogComp, {"tenantId": tenantId, "tenantName": tenantName || tenantId})
	}

	Component {
		id: __leaveConfirmDialogComp
		MessageDialog {
			property string tenantId: ""
			property string tenantName: ""
			width: Style.sizeHintM
			title: qsTr("Leave organization")
			message: qsTr("Are you sure you want to leave \"%1\"?").arg(tenantName)
			onFinished: {
				if (buttonId == Enums.yes && organizationsPage.apiClient) {
					organizationsPage.apiClient.leaveTenant(tenantId)
				}
			}
		}
	}

	ProfileNoticeBanner {
		id: banner
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Style.marginXL
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
		anchors.top: banner.visible ? banner.bottom : parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: Style.marginXL
		anchors.topMargin: banner.visible ? Style.marginM : Style.marginXL
		contentWidth: width
		contentHeight: bodyColumn.height + Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn
			width: parent.width
			spacing: Style.marginXL

			// --- You are here ---
			Column {
				width: parent.width
				spacing: Style.marginXS

				Text {
					text: qsTr("Current organization")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				Rectangle {
					width: parent.width
					height: currentOrgRow.height + 2 * Style.marginM
					radius: Style.radiusL
					color: Style.baseColor
					border.width: 1
					border.color: Style.imaginToolsAccentColor

					Row {
						id: currentOrgRow
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.verticalCenter: parent.verticalCenter
						anchors.margins: Style.marginM
						spacing: Style.marginM

						// Evaluated once per organizationsList/currentTenantId change; children read this.
						property var currentOrgData: organizationsPage.__currentOrg()

						Connections {
							target: AuthorizationController
							function onTenantSelected(tenantId) {
								currentOrgRow.currentOrgData = organizationsPage.__currentOrg()
							}
						}

						Rectangle {
							width: 8
							height: 8
							radius: 4
							color: Style.imaginToolsAccentColor
							anchors.verticalCenter: parent.verticalCenter
						}

						Column {
							width: parent.width - 8 - parent.spacing
							spacing: Style.marginXS

							Text {
								text: currentOrgRow.currentOrgData
									? (currentOrgRow.currentOrgData.name || currentOrgRow.currentOrgData.id)
									: qsTr("No organization")
								font.pixelSize: Style.fontSizeL
								font.bold: true
								font.family: Style.fontFamily
								color: Style.textColor
								elide: Text.ElideRight
								width: parent.width
							}

							Text {
								text: currentOrgRow.currentOrgData && currentOrgRow.currentOrgData.description !== ""
									? currentOrgRow.currentOrgData.description
									: qsTr("You are currently working in this organization. Everything you do applies here.")
								font.pixelSize: Style.fontSizeS
								font.family: Style.fontFamily
								color: Style.inactiveTextColor
								wrapMode: Text.WordWrap
								width: parent.width
							}
						}
					}
				}
			}

			// --- My Invitations ---
			Column {
				width: parent.width
				spacing: Style.marginXS
				visible: AuthorizationController.pendingInvitationsCount > 0

				Text {
					text: qsTr("Pending invitations")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				Rectangle {
					width: parent.width
					height: invitationsColumn.height + 2 * Style.marginM
					border.width: 1
					border.color: Style.borderColor
					radius: Style.radiusL
					color: Style.baseColor

					Column {
						id: invitationsColumn
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.margins: Style.marginM
						spacing: Style.spacingM

						Repeater {
							model: AuthorizationController.pendingInvitations

							delegate: Rectangle {
								width: invitationsColumn.width
								height: Style.controlHeightM
								color: "transparent"

								Text {
									anchors.verticalCenter: parent.verticalCenter
									anchors.left: parent.left
									anchors.right: btnRow.left
									anchors.rightMargin: Style.marginM
									text: modelData.tenantName ? modelData.tenantName : modelData.tenantId
									font.pixelSize: Style.fontSizeM
									font.family: Style.fontFamily
									color: Style.textColor
									elide: Text.ElideRight
								}

								Row {
									id: btnRow
									anchors.verticalCenter: parent.verticalCenter
									anchors.right: parent.right
									spacing: Style.spacingM

									Rectangle {
										width: acceptLabel.contentWidth + 2 * Style.marginL
										height: Style.controlHeightS
										radius: Style.radiusM
										color: Style.imaginToolsAccentColor
										opacity: acceptMA.containsMouse ? 0.8 : 1

										Text {
											id: acceptLabel
											anchors.centerIn: parent
											text: qsTr("Accept")
											font.pixelSize: Style.fontSizeS
											color: Style.baseColor
										}

										MouseArea {
											id: acceptMA
											anchors.fill: parent
											hoverEnabled: true
											cursorShape: Qt.PointingHandCursor
											onClicked: {
												if (organizationsPage.apiClient)
													organizationsPage.apiClient.acceptInvitation(modelData.id)
											}
										}
									}

									Rectangle {
										width: rejectLabel.contentWidth + 2 * Style.marginL
										height: Style.controlHeightS
										radius: Style.radiusM
										color: Style.negativeAccentColor
										opacity: rejectMA.containsMouse ? 0.8 : 1

										Text {
											id: rejectLabel
											anchors.centerIn: parent
											text: qsTr("Reject")
											font.pixelSize: Style.fontSizeS
											color: Style.baseColor
										}

										MouseArea {
											id: rejectMA
											anchors.fill: parent
											hoverEnabled: true
											cursorShape: Qt.PointingHandCursor
											onClicked: {
												if (organizationsPage.apiClient)
													organizationsPage.apiClient.rejectInvitation(modelData.id)
											}
										}
									}
								}
							}
						}
					}
				}
			}

			// --- Other organizations ---
			Column {
				id: otherOrgsSection
				width: parent.width
				spacing: Style.marginXS
				visible: otherOrgsSection.otherOrgsData.length > 0

				// Evaluated once per organizationsList/currentTenantId change; children read this.
				property var otherOrgsData: organizationsPage.__otherOrgs()

				Text {
					text: qsTr("Other organizations")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				Rectangle {
					width: parent.width
					height: organizationsColumn.height + 2 * Style.marginM
					border.width: 1
					border.color: Style.borderColor
					radius: Style.radiusL
					color: Style.baseColor

					Column {
						id: organizationsColumn
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.margins: Style.marginM
						spacing: Style.spacingM

						Repeater {
							id: organizationsRepeater
							model: otherOrgsSection.otherOrgsData

							delegate: Rectangle {
								width: organizationsColumn.width
								height: Style.controlHeightL
								color: "transparent"

								Column {
									id: orgCol
									anchors.verticalCenter: parent.verticalCenter
									anchors.left: parent.left
									anchors.right: leaveBtn.visible ? leaveBtn.left : switchBtn.left
									anchors.rightMargin: Style.marginM
									spacing: Style.marginXS

									Text {
										text: modelData.name ? modelData.name : modelData.id
										font.pixelSize: Style.fontSizeM
										font.bold: true
										font.family: Style.fontFamily
										color: Style.textColor
										elide: Text.ElideRight
										width: parent.width
									}

									Text {
										visible: modelData.description && modelData.description !== ""
										text: modelData.description
										font.pixelSize: Style.fontSizeS
										font.family: Style.fontFamily
										color: Style.inactiveTextColor
										elide: Text.ElideRight
										width: parent.width
									}
								}

								Rectangle {
									id: switchBtn
									anchors.right: parent.right
									anchors.verticalCenter: orgCol.verticalCenter
									width: switchLabel.contentWidth + 2 * Style.marginL
									height: Style.controlHeightS
									radius: Style.radiusM
									color: Style.imaginToolsAccentColor
									opacity: switchMA.containsMouse ? 0.8 : 1

									Text {
										id: switchLabel
										anchors.centerIn: parent
										text: qsTr("Switch")
										font.pixelSize: Style.fontSizeS
										color: Style.baseColor
									}

									MouseArea {
										id: switchMA
										anchors.fill: parent
										hoverEnabled: true
										cursorShape: Qt.PointingHandCursor
										onClicked: {
											AuthorizationController.selectTenant(modelData.id)
										}
									}
								}

								Rectangle {
									id: leaveBtn
									visible: !modelData.isDelegated && modelData.id !== ""
									anchors.right: switchBtn.left
									anchors.rightMargin: Style.marginS
									anchors.verticalCenter: orgCol.verticalCenter
									width: leaveLabel.contentWidth + 2 * Style.marginL
									height: Style.controlHeightS
									radius: Style.radiusM
									color: Style.negativeAccentColor
									opacity: leaveMA.containsMouse ? 0.8 : 1

									Text {
										id: leaveLabel
										anchors.centerIn: parent
										text: qsTr("Leave")
										font.pixelSize: Style.fontSizeS
										color: Style.baseColor
									}

									MouseArea {
										id: leaveMA
										anchors.fill: parent
										hoverEnabled: true
										cursorShape: Qt.PointingHandCursor
										onClicked: {
											organizationsPage.__confirmLeaveTenant(modelData.id, modelData.name)
										}
									}
								}
							}
						}
					}
				}
			}

			// --- Empty state ---
			Text {
				width: parent.width
				visible: AuthorizationController.pendingInvitationsCount === 0 && otherOrgsSection.otherOrgsData.length === 0
				text: qsTr("You are not a member of any other organization yet.")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				wrapMode: Text.WordWrap
			}
		}
	}
}
