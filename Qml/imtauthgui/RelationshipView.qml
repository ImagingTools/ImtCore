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
 * RelationshipView
 *
 * ViewBase-inherited editor for creating tenant relationships.
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

	signal relationshipCreated()

	function updateGui() {
	}

	function updateModel() {
	}

	function submitRelationship() {
		if (!container.__selectedTargetTenantId) {
			ModalDialogManager.showInfoDialog(qsTr("Target tenant is required."))
			return
		}
		var roleTokens = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
		var srcIdx = sourceRoleCB.currentIndex >= 0 ? sourceRoleCB.currentIndex : 2
		var tgtIdx = targetRoleCB.currentIndex >= 0 ? targetRoleCB.currentIndex : 2
		if (container.apiClient) {
			container.apiClient.addTenantRelationship(
				container.tenantData ? container.tenantData.m_id : "",
				container.__selectedTargetTenantId,
				roleTokens[srcIdx],
				roleTokens[tgtIdx],
				relScopeInput.text.trim(),
				relValidFromPicker.getDateAsString(),
				relValidUntilPicker.getDateAsString(),
				relDescriptionInput.text.trim())
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
				title: qsTr("Tenant Relationship")
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

				ComboBoxElementView {
					id: sourceRoleCB
					name: qsTr("Source Role")
					model: roleModel
					currentIndex: 2
				}

				ComboBoxElementView {
					id: targetRoleCB
					name: qsTr("Target Role")
					model: roleModel
					currentIndex: 2
				}

				TextInputElementView {
					id: relScopeInput
					name: qsTr("Scope")
					placeHolderText: qsTr("Optional — empty applies to all resources")
				}

				DateTimePickerElementView {
					id: relValidFromPicker
					name: qsTr("Valid From")
				}

				DateTimePickerElementView {
					id: relValidUntilPicker
					name: qsTr("Valid Until")
				}

				TextInputElementView {
					id: relDescriptionInput
					name: qsTr("Description")
					placeHolderText: qsTr("Optional description")
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

	TreeItemModel {
		id: roleModel
		Component.onCompleted: {
			var roles = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
			for (var i = 0; i < roles.length; i++) {
				var idx = roleModel.insertNewItem()
				roleModel.setData("id", roles[i], idx)
				roleModel.setData("name", roles[i], idx)
			}
		}
	}
}
