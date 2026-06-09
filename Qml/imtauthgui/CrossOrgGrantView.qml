// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0

/**
 * CrossOrgGrantView
 *
 * ViewBase-inherited editor for creating/editing cross-org grants.
 * Follows the document service pattern (updateGui/updateModel).
 * Simplified model: TargetTenant, Roles (multi-select), Description, Expires.
 */
ViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	property var grantData: model
	property var apiClient: null

		property string __selectedTargetTenantId: ""
	property string __selectedTargetTenantName: ""
	property string __selectedExpiresAt: ""

		function updateGui() {
		if (!container.grantData) {
			return
		}
		container.__selectedTargetTenantId = container.grantData.m_targetTenantId || ""
		container.__selectedTargetTenantName = container.grantData.m_targetTenantName || container.grantData.m_targetTenantId || ""

		var roleIds = container.grantData.m_roleIds || []
		var arr = []
		for (var i = 0; i < roleIds.length; i++)
			arr.push({id: roleIds[i], name: roleIds[i]})
		rolesSelectEditor.items = arr

		grantDescriptionInput.text = container.grantData.m_description || ""

		var exp = container.grantData.m_expiresAt || ""
		container.__selectedExpiresAt = exp
	}

		function updateModel() {
		if (!container.grantData) {
			return
		}
		container.grantData.m_targetTenantId = container.__selectedTargetTenantId || ""

		var ids = []
		for (var i = 0; i < rolesSelectEditor.items.length; i++)
			ids.push(rolesSelectEditor.items[i].id)
		container.grantData.m_roleIds = ids

		container.grantData.m_description = grantDescriptionInput.text.trim()
		container.grantData.m_expiresAt = container.__selectedExpiresAt
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
							id: targetTenantRow
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
									var point = targetTenantRow.mapToItem(null, 0, targetTenantRow.height)
									ModalDialogManager.openDialog(tenantSelectComp, {
																	  "x": point.x,
																	  "y": point.y
																  })
								}
							}

							Button {
								text: qsTr("Remove")
								visible: container.__selectedTargetTenantId !== ""
								onClicked: {
									container.__selectedTargetTenantId = ""
									container.__selectedTargetTenantName = ""
									container.doUpdateModel()
								}
							}
						}
					}
				}

				GqlBasedItemSelectElementView {
					id: rolesSelectEditor
					collectionId: "Roles"
					label: qsTr("Roles")
					addButtonText: qsTr("Add Role")
					showCount: true
					onSelectionChanged: {
						container.doUpdateModel()
					}
				}

				TextInputElementView {
					id: grantDescriptionInput
					name: qsTr("Description")
					placeHolderText: qsTr("Optional description")
					onEditingFinished: {
						container.doUpdateModel()
					}
				}

								ComboBoxElementView {
					id: expirationCb
					name: qsTr("Expires At")
					description: qsTr("The grant will expire on the selected date")
					model: expirationModel
					currentIndex: {
						// Select index if editing
						if (container.__selectedExpiresAt === "") return 4
						return 1 // Default to 30 days if not easily parsable, but let's see
					}
					TreeItemModel {
						id: expirationModel
						Component.onCompleted: {
							let index = expirationModel.insertNewItem()
							expirationModel.setData("id", "7", index)
							expirationModel.setData("name", qsTr("7 Days"), index)

							index = expirationModel.insertNewItem()
							expirationModel.setData("id", "30", index)
							expirationModel.setData("name", qsTr("30 Days"), index)
							
							index = expirationModel.insertNewItem()
							expirationModel.setData("id", "60", index)
							expirationModel.setData("name", qsTr("60 Days"), index)
							
							index = expirationModel.insertNewItem()
							expirationModel.setData("id", "90", index)
							expirationModel.setData("name", qsTr("90 Days"), index)

							index = expirationModel.insertNewItem()
							expirationModel.setData("id", "unlimited", index)
							expirationModel.setData("name", qsTr("No Expiration"), index)
						}
					}

					function computeExpiresAtIso() {
						var id = expirationModel.getData("id", currentIndex)
					
						if (id === "unlimited")
							return ""
					
						if (id === "" || id === undefined || id === null)
							return ""
					
						let days = Number(id)
						if (days <= 0)
							return ""
					
						let d = new Date()
						d.setDate(d.getDate() + days)
						return d.toISOString()
					}
					
					onCurrentIndexChanged: {
						// When user explicitly selects something
						container.__selectedExpiresAt = computeExpiresAtIso()
						container.doUpdateModel()
					}

					Binding {
						target: expirationCb
						property: "currentIndex"
						value: {
							if (container.__selectedExpiresAt === "") return 4
							// Approximation. Usually expiresAt will not exactly match 30 days from now
							// if it was set in the past. But let's show "30 days" as fallback.
							return 1
						}
						restoreMode: Binding.RestoreBinding
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
}
