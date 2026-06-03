// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtauthgui 1.0

/**
 * CrossOrgGrantView
 *
 * ViewBase-inherited editor for creating cross-org grants.
 * Follows the RoleView / UserGroupView pattern with GqlBasedCommandsController.
 * The commands controller provides the Save button at the top.
 */
ViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	property var apiClient: null
	property var tenantData: null

	property string __selectedTargetTenantId: ""
	property string __selectedTargetTenantName: ""
	property string __selectedRelationshipId: ""
	property string __selectedRelationshipName: ""

	signal grantCreated()

	function updateGui() {
	}

	function updateModel() {
	}

	function submitGrant() {
		if (!container.__selectedTargetTenantId || !container.__selectedRelationshipId) {
			ModalDialogManager.showInfoDialog(qsTr("Target tenant and relationship are required."))
			return
		}
		var levelIndex = accessLevelCB.currentIndex >= 0 ? accessLevelCB.currentIndex : 1
		var levelTokens = ["None", "Read", "Write", "Admin"]
		var accessLevel = levelTokens[levelIndex]
		if (container.apiClient) {
			container.apiClient.createCrossOrgGrant(
				container.tenantData ? container.tenantData.m_id : "",
				container.__selectedTargetTenantId,
				container.__selectedRelationshipId,
				accessLevel,
				resourceScopeInput.text.trim(),
				"",
				grantDescriptionInput.text.trim(),
				expiresAtPicker.getDateAsString())
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
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, Style.sizeHintXXL)
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
				title: qsTr("Cross-Org Grant")
			}

			GroupElementView {
				id: generalGroup
				width: parent.width

				ElementView {
					name: qsTr("Target Tenant")

					controlComp: Component {
						Row {
							spacing: Style.marginM

							BaseText {
								anchors.verticalCenter: parent.verticalCenter
								text: container.__selectedTargetTenantName
								  || container.__selectedTargetTenantId
								  || qsTr("Select tenant...")
								color: container.__selectedTargetTenantId
								   ? Style.textColor : Style.inactiveTextColor
								font.pixelSize: Style.fontSizeM
							}

							Button {
								text: qsTr("Select")
								onClicked: {
									ModalDialogManager.openDialog(tenantSelectComp, {})
								}
							}
						}
					}
				}

				ElementView {
					name: qsTr("Relationship")

					controlComp: Component {
						Row {
							spacing: Style.marginM

							BaseText {
								anchors.verticalCenter: parent.verticalCenter
								text: container.__selectedRelationshipName
								  || container.__selectedRelationshipId
								  || qsTr("Select relationship...")
								color: container.__selectedRelationshipId
								   ? Style.textColor : Style.inactiveTextColor
								font.pixelSize: Style.fontSizeM
							}

							Button {
								text: qsTr("Select")
								onClicked: {
									ModalDialogManager.openDialog(relationshipSelectComp, {})
								}
							}
						}
					}
				}

				ComboBoxElementView {
					id: accessLevelCB
					name: qsTr("Access Level")
					model: accessLevelModel
					currentIndex: 1
				}

				TextInputElementView {
					id: resourceScopeInput
					name: qsTr("Resource Scope")
					placeHolderText: qsTr("Optional — empty grants all resources")
				}

				TextInputElementView {
					id: grantDescriptionInput
					name: qsTr("Description")
					placeHolderText: qsTr("Optional description")
				}

				DateTimePickerElementView {
					id: expiresAtPicker
					name: qsTr("Expires At")
				}
			}
		}
	}

	Component {
		id: tenantSelectComp

		FilterableSelectPopup {
			dataProvider: FilterableSelectGqlDataProvider {
				collectionId: "Tenants"
				multiSelect: false
			}
			filterPlaceholder: qsTr("Select tenant...")
			preselectedIds: container.__selectedTargetTenantId
				? [container.__selectedTargetTenantId] : []

			onItemSelected: {
				container.__selectedTargetTenantId = itemId
				container.__selectedTargetTenantName = dataProvider
					? dataProvider.getSelectedItemText(itemId) : ""
			}
		}
	}

	Component {
		id: relationshipSelectComp

		FilterableSelectPopup {
			dataProvider: FilterableSelectGqlDataProvider {
				collectionId: "TenantRelationships"
				multiSelect: false
			}
			filterPlaceholder: qsTr("Select relationship...")
			preselectedIds: container.__selectedRelationshipId
				? [container.__selectedRelationshipId] : []

			onItemSelected: {
				container.__selectedRelationshipId = itemId
				container.__selectedRelationshipName = dataProvider
					? dataProvider.getSelectedItemText(itemId) : ""
			}
		}
	}

	TreeItemModel {
		id: accessLevelModel
		Component.onCompleted: {
			var levels = ["None", "Read", "Write", "Admin"]
			for (var i = 0; i < levels.length; i++) {
				var idx = accessLevelModel.insertNewItem()
				accessLevelModel.setData("id", levels[i], idx)
				accessLevelModel.setData("name", levels[i], idx)
			}
		}
	}
}
