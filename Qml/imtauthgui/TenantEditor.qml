// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtcolgui 1.0
import imtauthTenantsSdl 1.0
import imtauthTenantMembershipsSdl 1.0

DocumentViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	property TenantData tenantData: model
	property var pendingMembers: []
	property var __originalMemberUserIds: []
	property bool _membersChanged: false
	property var __userNameCache: ({})

	function updateGui(){
		generalGroup.updateGui();
		__loadMembers();
	}

	function updateModel(){
		generalGroup.updateModel();
	}

	function __loadMembers() {
		if (!container.tenantData || !container.tenantData.m_id)
			return
		getMembershipsByTenantInput.m_tenantId = container.tenantData.m_id
		getMembershipsByTenantRequest.send(getMembershipsByTenantInput)
	}

	property GetMembershipsByTenantInput getMembershipsByTenantInput: GetMembershipsByTenantInput {}
	property GqlSdlRequestSender getMembershipsByTenantRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_getMembershipsByTenant
		sdlObjectComp: Component {
			GetMembershipsByTenantPayload {
				onFinished: {
					if (m_errorMessage) return
					var members = []
					if (m_membershipIds) {
						for (var i = 0; i < m_membershipIds.length; i++) {
							var membershipId = m_membershipIds[i]
							// Each entry stores membershipId for removal and uses it as display key
							members.push({ membershipId: membershipId, id: membershipId, name: container.__userNameCache[membershipId] || membershipId })
						}
					}
					container.pendingMembers = members
					// Track original member IDs so sync only adds new ones
					container.__originalMemberUserIds = members.map(function(m) { return m.membershipId })
				}
			}
		}
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
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: scrollbar.left
		anchors.rightMargin: Style.marginXL
		contentWidth: bodyColumn.width
		contentHeight: bodyColumn.height + 2 * Style.marginXL

		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn
			width: Style.sizeHintXXL
			spacing: Style.marginXL

			GroupHeaderView {
				width: parent.width
				title: qsTr("General")
				groupView: generalGroup
			}

			GroupElementView {
				id: generalGroup
				width: parent.width

				TextInputElementView {
					id: nameInput

					name: qsTr("Tenant Name")
					placeHolderText: qsTr("Enter the tenant name")

					onEditingFinished: {
						let oldText = container.tenantData ? container.tenantData.m_name : ""
						if (oldText !== nameInput.text){
							container.doUpdateModel();
						}
					}

					KeyNavigation.tab: descriptionInput
					KeyNavigation.backtab: isActiveInput
				}

				TextInputElementView {
					id: descriptionInput

					name: qsTr("Description")
					placeHolderText: qsTr("Enter the description")

					onEditingFinished: {
						let oldText = container.tenantData ? container.tenantData.m_description : ""
						if (oldText !== descriptionInput.text){
							container.doUpdateModel();
						}
					}

					KeyNavigation.tab: isActiveInput
					KeyNavigation.backtab: nameInput
				}

				SwitchElementView {
					id: isActiveInput

					name: qsTr("Active")

					onCheckedChanged: {
						container.doUpdateModel();
					}
				}

				function updateGui(){
					if (!container.tenantData) return;
					nameInput.text = container.tenantData.m_name || ""
					descriptionInput.text = container.tenantData.m_description || ""
					isActiveInput.checked = container.tenantData.m_isActive !== undefined ? container.tenantData.m_isActive : true
				}

				function updateModel(){
					if (!container.tenantData) return;
					container.tenantData.m_name = nameInput.text
					container.tenantData.m_description = descriptionInput.text
					container.tenantData.m_isActive = isActiveInput.checked
				}
			}

			GroupHeaderView {
				width: parent.width
				title: qsTr("Members")
			}

			ElementView {
				width: parent.width
				name: qsTr("Members")

				controlComp: Component {
					Text {
						id: addMemberBtn
						text: "+ " + qsTr("Add member")
						font.pixelSize: Style.fontSizeM
						font.bold: true
						color: Style.accentColor

						MouseArea {
							anchors.fill: parent
							hoverEnabled: true
							cursorShape: Qt.PointingHandCursor
							onClicked: {
								var known = []
								for (var j = 0; j < container.pendingMembers.length; j++) {
									var member = container.pendingMembers[j]
									known.push({ id: member.id, title: member.name || member.id })
								}
								var ids = []
								for (var i = 0; i < container.pendingMembers.length; i++)
									ids.push(container.pendingMembers[i].id)
								var point = addMemberBtn.mapToItem(null, 0, addMemberBtn.height)
								ModalDialogManager.openDialog(memberSelectComp, {
									"x": point.x,
									"y": point.y,
									"knownItems": known,
									"preselectedIds": ids
								})
							}
						}
					}
				}

				bottomComp: Component {
					Column {
						width: parent.width
						spacing: Style.spacingXS

						Flow {
							width: parent.width
							clip: true
							spacing: Style.spacingXS
							visible: container.pendingMembers.length > 0

							Repeater {
								model: container.pendingMembers
								delegate: Rectangle {
									width: Math.min(memberChipText.contentWidth + memberChipRemove.width + Style.paddingS * 3, 200)
									height: 28
									radius: 14
									color: Qt.rgba(Style.accentColor.r, Style.accentColor.g, Style.accentColor.b, 0.1)
									border.color: Qt.rgba(Style.accentColor.r, Style.accentColor.g, Style.accentColor.b, 0.3)
									border.width: 1

									Text {
										id: memberChipText
										anchors.left: parent.left
										anchors.leftMargin: Style.paddingS + 2
										anchors.right: memberChipRemove.left
										anchors.rightMargin: 2
										anchors.verticalCenter: parent.verticalCenter
										text: modelData.name || modelData.id
										font.pixelSize: Style.fontSizeM
										color: Style.accentColor
										elide: Text.ElideRight
										maximumLineCount: 1
									}

									ToolButton {
										id: memberChipRemove
										anchors.right: parent.right
										anchors.verticalCenter: parent.verticalCenter
										iconSource: Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
										decorator: Component {
											ToolButtonDecorator {
												color: "transparent"
												icon.width: Style.iconSizeXS
											}
										}
										onClicked: {
											// Remove membership via API using the membershipId
											removeMembershipInput.m_membershipId = modelData.membershipId
											removeMembershipRequest.send(removeMembershipInput)
										}
									}
								}
							}
						}

						Text {
							visible: container.pendingMembers.length === 0
							width: parent.width
							text: qsTr("No members")
							font.pixelSize: Style.fontSizeM
							color: Style.inactiveTextColor
							wrapMode: Text.WordWrap
						}
					}
				}
			}

			// Member picker Component — opened via ModalDialogManager
			Component {
				id: memberSelectComp

				FilterableSelectPopup {
					dataProvider: FilterableSelectGqlDataProvider {
						collectionId: "Users"
						multiSelect: true
					}

					itemWidth: 280
					showCheckBox: true
					showSelectedGroup: true
					filterPlaceholder: qsTr("Type or choose a user")

					onSelectionChanged: {
						var arr = []
						for (var i = 0; i < selectedIds.length; i++) {
							var selId = selectedIds[i]
							var selName = dataProvider ? dataProvider.getSelectedItemText(selId) : ""
							if (!selName)
								selName = selId
							if (selName !== selId) container.__userNameCache[selId] = selName
							arr.push({id: selId, name: selName})
						}
						container.pendingMembers = arr
						container._membersChanged = true
					}

					Component.onDestruction: {
						if (container._membersChanged) {
							container.__syncMemberships()
						}
					}
				}
			}
		}
	}

	// Sync memberships: only add genuinely new members (not already in original list)
	function __syncMemberships() {
		if (!container.tenantData || !container.tenantData.m_id) return
		for (var i = 0; i < pendingMembers.length; i++) {
			var userId = pendingMembers[i].id
			// Skip members that were already loaded from backend
			if (container.__originalMemberUserIds.indexOf(userId) >= 0) continue
			addMembershipInput.m_userId = userId
			addMembershipInput.m_tenantId = container.tenantData.m_id
			addMembershipInput.m_role = "Member"
			addMembershipRequest.send(addMembershipInput)
		}
		container._membersChanged = false
	}

	property AddMembershipInput addMembershipInput: AddMembershipInput {}
	property GqlSdlRequestSender addMembershipRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_addMembership
		sdlObjectComp: Component {
			AddMembershipPayload {
				onFinished: {
					// Refresh members list after adding
					container.__loadMembers()
				}
			}
		}
	}

	property RemoveMembershipInput removeMembershipInput: RemoveMembershipInput {}
	property GqlSdlRequestSender removeMembershipRequest: GqlSdlRequestSender {
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_removeMembership
		sdlObjectComp: Component {
			RemoveMembershipPayload {
				onFinished: {
					// Refresh members list after removal
					container.__loadMembers()
				}
			}
		}
	}
}
