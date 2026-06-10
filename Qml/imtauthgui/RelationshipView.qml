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
 *
 * The editor displays fields relative to the current tenant:
 *   - "Partner Organization" (the other side)
 *   - "My Role" (role of the current tenant in this relationship)
 *   - "Partner Role" (role of the partner tenant)
 * This avoids confusion when both sides can edit the same relationship.
 */
ViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor
	commandsSeparatorVisible: false

	property var relationshipData: model
	property var apiClient: null
	property var tenantData: null

	property string __selectedPartnerTenantId: ""
	property string __selectedPartnerTenantName: ""
	// Whether the current tenant is the "source" in the underlying model
	property bool __isSource: true
	readonly property bool __isExistingRelationship: (relationshipData
		&& relationshipData.m_sourceTenantId
		&& relationshipData.m_targetTenantId)

	// Resolve the current tenant ID from tenantData or AuthorizationController
	readonly property string __currentTenantId: container.tenantData
		? (container.tenantData.m_id || "")
		: (typeof AuthorizationController !== "undefined" ? AuthorizationController.currentTenantId : "")

	function updateGui() {
		if (!container.relationshipData) {
			return
		}
		// Determine which side we are: if current tenant is the source,
		// the partner is target; otherwise, we swap the perspective.
		var currentTenantId = container.__currentTenantId
		var sourceTenantId = container.relationshipData.m_sourceTenantId || ""
		var targetTenantId = container.relationshipData.m_targetTenantId || ""

		if (targetTenantId && currentTenantId && targetTenantId === currentTenantId && sourceTenantId !== "") {
			// Current tenant is the target → swap perspective
			container.__isSource = false
			container.__selectedPartnerTenantId = sourceTenantId
			container.__selectedPartnerTenantName = container.relationshipData.m_sourceTenantName || ""
		} else {
			// Current tenant is the source (or new relationship)
			container.__isSource = true
			container.__selectedPartnerTenantId = targetTenantId
			container.__selectedPartnerTenantName = container.relationshipData.m_targetTenantName || ""
		}

		var roles = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
		var myRoleValue = container.__isSource
			? (container.relationshipData.m_sourceRole || "")
			: (container.relationshipData.m_targetRole || "")
		var partnerRoleValue = container.__isSource
			? (container.relationshipData.m_targetRole || "")
			: (container.relationshipData.m_sourceRole || "")

		var myIdx = roles.indexOf(myRoleValue)
		var partnerIdx = roles.indexOf(partnerRoleValue)
		myRoleCB.currentIndex = myIdx >= 0 ? myIdx : 2
		partnerRoleCB.currentIndex = partnerIdx >= 0 ? partnerIdx : 2
		relScopeInput.text = container.relationshipData.m_scope || ""
		relDescriptionInput.text = container.relationshipData.m_description || ""
	}

	function updateModel() {
		if (!container.relationshipData) {
			return
		}

		if (!container.__isExistingRelationship) {
			if (container.__isSource) {
				if (container.__currentTenantId) {
					container.relationshipData.m_sourceTenantId = container.__currentTenantId
				}
				container.relationshipData.m_targetTenantId = container.__selectedPartnerTenantId || ""
			} else {
				container.relationshipData.m_sourceTenantId = container.__selectedPartnerTenantId || ""
				if (container.__currentTenantId) {
					container.relationshipData.m_targetTenantId = container.__currentTenantId
				}
			}
		}

		var roleTokens = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
		var myIdx = myRoleCB.currentIndex >= 0 ? myRoleCB.currentIndex : 2
		var partnerIdx = partnerRoleCB.currentIndex >= 0 ? partnerRoleCB.currentIndex : 2

		if (container.__isSource) {
			container.relationshipData.m_sourceRole = roleTokens[myIdx]
			container.relationshipData.m_targetRole = roleTokens[partnerIdx]
		} else {
			container.relationshipData.m_targetRole = roleTokens[myIdx]
			container.relationshipData.m_sourceRole = roleTokens[partnerIdx]
		}
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
		width: container.width
		contentWidth: bodyColumn.width
		contentHeight: bodyColumn.height + 2 * Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn
			width: container.width
			spacing: Style.marginXL

			GroupElementView {
				id: generalGroup
				width: parent.width

				ElementView {
					name: qsTr("Partner Organization")

					controlComp: Component {
						Row {
							id: partnerTenantRow
							spacing: Style.marginM

							BaseText {
								anchors.verticalCenter: parent.verticalCenter
								text: container.__selectedPartnerTenantName
								  || container.__selectedPartnerTenantId
								  || qsTr("Select organization...")
								color: container.__selectedPartnerTenantId
								   ? Style.textColor : Style.inactiveTextColor
								font.pixelSize: Style.fontSizeM
							}

							Button {
								text: qsTr("Select")
								enabled: !container.__isExistingRelationship
								visible: !container.__isExistingRelationship
								onClicked: {
									var point = partnerTenantRow.mapToItem(null, 0, partnerTenantRow.height)
									ModalDialogManager.openDialog(tenantSelectComp, {
																	  "x": point.x,
																	  "y": point.y
																  })
								}
							}

							Button {
								text: qsTr("Remove")
								visible: !container.__isExistingRelationship && container.__selectedPartnerTenantId !== ""
								onClicked: {
									container.__selectedPartnerTenantId = ""
									container.__selectedPartnerTenantName = ""
									container.doUpdateModel()
								}
							}
						}
					}
				}

				ComboBoxElementView {
					id: myRoleCB
					name: qsTr("My Role")
					model: roleModel
					currentIndex: 2
					onCurrentIndexChanged: {
						container.doUpdateModel()
					}
				}

				ComboBoxElementView {
					id: partnerRoleCB
					name: qsTr("Partner Role")
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
			filterPlaceholder: qsTr("Select organization...")
			preselectedIds: container.__selectedPartnerTenantId
				? [container.__selectedPartnerTenantId] : []

			onItemSelected: {
				if (container.__isExistingRelationship) {
					ModalDialogManager.showInfoDialog(qsTr("Partner organization cannot be changed after relationship creation."))
					return
				}
				if (itemId === container.__currentTenantId) {
					ModalDialogManager.showInfoDialog(qsTr("Choose another organization. Current and partner organizations must be different."))
					return
				}
				container.__selectedPartnerTenantId = itemId
				container.__selectedPartnerTenantName = dataProvider
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
