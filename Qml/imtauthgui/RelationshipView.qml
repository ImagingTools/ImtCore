// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtauthgui 1.0

/**
 * RelationshipView
 *
 * ViewBase-inherited editor for creating or editing tenant relationships.
 * Follows the RoleView / UserGroupView pattern.
 *
 * Used inside the document service flow (TenantDocumentEditorShell).
 * The document framework calls updateGui() to populate the form from
 * the representationModel and updateModel() to write form values back.
 * Saving is handled by the shell's Save button which triggers the
 * bilateral proposal flow via CRelationshipCollectionDocumentServiceComp.
 */
ViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	property var relationshipData: model
	property var apiClient: null

	property string __selectedTargetTenantId: ""
	property string __selectedTargetTenantName: ""

	function updateGui() {
		if (!container.relationshipData) {
			return
		}
		container.__selectedTargetTenantId = container.relationshipData.m_targetTenantId || ""
		container.__selectedTargetTenantName = container.relationshipData.m_targetTenantId || ""
		var roles = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
		var srcIdx = roles.indexOf(container.relationshipData.m_sourceRole || "")
		var tgtIdx = roles.indexOf(container.relationshipData.m_targetRole || "")
		sourceRoleCB.currentIndex = srcIdx >= 0 ? srcIdx : 2
		targetRoleCB.currentIndex = tgtIdx >= 0 ? tgtIdx : 2
		relScopeInput.text = container.relationshipData.m_scope || ""
		relDescriptionInput.text = container.relationshipData.m_description || ""
	}

	function updateModel() {
		if (!container.relationshipData) {
			return
		}
		container.relationshipData.m_targetTenantId = container.__selectedTargetTenantId || ""
		var roleTokens = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
		var srcIdx = sourceRoleCB.currentIndex >= 0 ? sourceRoleCB.currentIndex : 2
		var tgtIdx = targetRoleCB.currentIndex >= 0 ? targetRoleCB.currentIndex : 2
		container.relationshipData.m_sourceRole = roleTokens[srcIdx]
		container.relationshipData.m_targetRole = roleTokens[tgtIdx]
		container.relationshipData.m_scope = relScopeInput.text.trim()
		container.relationshipData.m_description = relDescriptionInput.text.trim()
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
							id: targetTenantRole
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
									var point = targetTenantRole.mapToItem(null, 0, targetTenantRole.height)
									ModalDialogManager.openDialog(tenantSelectComp, {
																	  "x": point.x,
																	  "y": point.y
																  })
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
					onCurrentIndexChanged: {
						container.doUpdateModel()
					}
				}

				ComboBoxElementView {
					id: targetRoleCB
					name: qsTr("Target Role")
					model: roleModel
					currentIndex: 2
					onCurrentIndexChanged: {
						container.doUpdateModel()
					}
				}

				TextInputElementView {
					id: relScopeInput
					name: qsTr("Scope")
					placeHolderText: qsTr("Optional — empty applies to all resources")
					onEditingFinished: {
						container.doUpdateModel()
					}
				}

				TextInputElementView {
					id: relDescriptionInput
					name: qsTr("Description")
					placeHolderText: qsTr("Optional description")
					onEditingFinished: {
						container.doUpdateModel()
					}
				}
			}
		}
	}

	Component {
		id: tenantSelectComp

		FilterableSelectPopup {
			dataProvider: container.apiClient ? container.apiClient.connectionsDataProvider : null
			filterPlaceholder: qsTr("Select tenant...")
			preselectedIds: container.__selectedTargetTenantId
				? [container.__selectedTargetTenantId] : []

			onItemSelected: {
				container.__selectedTargetTenantId = itemId
				container.__selectedTargetTenantName = dataProvider
					? dataProvider.getSelectedItemText(itemId) : ""
				container.doUpdateModel()
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
