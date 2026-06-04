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
 * ViewBase-inherited editor for creating or editing tenant relationships.
 * Follows the RoleView / UserGroupView pattern.
 *
 * When `editRelationshipId` is non-empty the view operates in edit mode:
 * it pre-populates fields from `apiClient.tenantRelationshipsModel` and on
 * submit removes the old relationship then adds a new one with the updated
 * values.  In create mode (editRelationshipId is empty) only AddTenantRelationship
 * is called.
 */
ViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	property var apiClient: null
	property var tenantData: null

	// Edit-mode: set to the ID of the relationship to edit (empty = create mode)
	property string editRelationshipId: ""

	readonly property bool __isEditMode: container.editRelationshipId !== ""

	property string __selectedTargetTenantId: ""
	property string __selectedTargetTenantName: ""

	signal relationshipCreated()

	function updateGui() {
	}

	function updateModel() {
	}

	// Pre-populate fields when entering edit mode
	Component.onCompleted: {
		if (container.__isEditMode && container.apiClient) {
			var model = container.apiClient.tenantRelationshipsModel
			for (var i = 0; i < model.count; i++) {
				var item = model.get(i)
				if (item.id === container.editRelationshipId) {
					container.__selectedTargetTenantId = item.targetTenantId || ""
					container.__selectedTargetTenantName = item.targetTenantId || ""
					var roles = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
					var srcIdx = roles.indexOf(item.sourceRole)
					var tgtIdx = roles.indexOf(item.targetRole)
					sourceRoleCB.currentIndex = srcIdx >= 0 ? srcIdx : 2
					targetRoleCB.currentIndex = tgtIdx >= 0 ? tgtIdx : 2
					relScopeInput.text = item.scope || ""
					relDescriptionInput.text = item.description || ""
					break
				}
			}
		}
	}

	function submitRelationship() {
		if (!container.__selectedTargetTenantId) {
			ModalDialogManager.showInfoDialog(qsTr("Target tenant is required."))
			return
		}
		var roleTokens = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
		var srcIdx = sourceRoleCB.currentIndex >= 0 ? sourceRoleCB.currentIndex : 2
		var tgtIdx = targetRoleCB.currentIndex >= 0 ? targetRoleCB.currentIndex : 2
		if (container.__isEditMode && container.apiClient && container.tenantData) {
			container.apiClient.removeTenantRelationship(
				container.tenantData.m_id, container.editRelationshipId)
		}
		if (container.apiClient) {
			container.apiClient.addTenantRelationship(
				container.tenantData ? container.tenantData.m_id : "",
				container.__selectedTargetTenantId,
				roleTokens[srcIdx],
				roleTokens[tgtIdx],
				relScopeInput.text.trim(),
				"",
				"",
				relDescriptionInput.text.trim())
		}
		container.relationshipCreated()
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
				title: container.__isEditMode ? qsTr("Edit Relationship") : qsTr("Tenant Relationship")
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

				TextInputElementView {
					id: relDescriptionInput
					name: qsTr("Description")
					placeHolderText: qsTr("Optional description")
				}
			}

			Row {
				spacing: Style.marginM

				Button {
					text: container.__isEditMode ? qsTr("Save Changes") : qsTr("Create Relationship")
					onClicked: { container.submitRelationship() }
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
