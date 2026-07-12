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
 * Follows the declarative write-through pattern (bindings + writeModelValues).
 * Simplified model: TargetTenant, Roles (multi-select), Description, Expires.
 */
ViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor
	commandsSeparatorVisible: false
	commandsPanelVisible: !isReadOnly

	property var grantData: model
	property var apiClient: null

	property string __selectedTargetTenantId: ""
	property string __selectedTargetTenantName: ""
	property string __selectedExpiresAt: ""
	property bool __isActive: true
	property bool __loadingGui: false
	property bool isReadOnly: grantData ? (grantData.m_isReadOnly === true) : false

	function expiresAtToIndex(iso) {
		if (!iso || iso === "")
			return 4

		var target = new Date(iso)
		if (isNaN(target.getTime()))
			return 4

		var days = (target.getTime() - (new Date()).getTime()) / (24 * 3600 * 1000)
		if (days <= 15)
			return 0
		if (days <= 45)
			return 1
		if (days <= 75)
			return 2
		return 3
	}

	function updateGui() {
		if (!container.grantData) {
			return
		}
		container.__selectedTargetTenantId = container.grantData.m_targetTenantId || ""
		container.__selectedTargetTenantName = container.grantData.m_targetTenantName || container.grantData.m_targetTenantId || ""

		grantNameInput.text = container.grantData.m_name || ""

		var roleIds = container.grantData.m_roleIds || []
		var arr = []
		for (var i = 0; i < roleIds.length; i++)
			arr.push({id: roleIds[i], name: roleIds[i]})
		rolesSelectEditor.items = arr

		grantDescriptionInput.text = container.grantData.m_description || ""

		var exp = container.grantData.m_expiresAt || ""
		container.__selectedExpiresAt = exp
		container.__isActive = container.grantData.m_isActive !== undefined
			? container.grantData.m_isActive
			: true
		isActiveSwitch.checked = container.__isActive
		container.__loadingGui = true
		expirationCb.currentIndex = container.expiresAtToIndex(exp)
		container.__loadingGui = false
	}

	function writeModelValues() {
		if (!container.grantData) {
			return
		}
		container.grantData.m_targetTenantId = container.__selectedTargetTenantId || ""

		container.grantData.m_name = grantNameInput.text.trim()

		var ids = []
		for (var i = 0; i < rolesSelectEditor.items.length; i++)
			ids.push(rolesSelectEditor.items[i].id)
		container.grantData.m_roleIds = ids

		container.grantData.m_description = grantDescriptionInput.text.trim()
		container.grantData.m_expiresAt = container.__selectedExpiresAt
		container.grantData.m_isActive = isActiveSwitch.checked
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

			// Read-only badge shown when the grant was created for this organization
			BaseText {
				visible: container.isReadOnly
				width: parent.width
				text: qsTr("This grant was created for your organization (read-only)")
				color: Style.inactiveTextColor
				font.pixelSize: Style.fontSizeM
				font.italic: true
			}

			GroupElementView {
				id: generalGroup
				width: parent.width

				TextInputElementView {
					id: grantNameInput
					name: qsTr("Name")
					placeHolderText: qsTr("Auto-generated from tenant and roles if left empty")
					readOnly: container.isReadOnly
					onEditingFinished: {
						container.writeModelValues()
					}
				}

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
								visible: !container.isReadOnly
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
								visible: container.__selectedTargetTenantId !== "" && !container.isReadOnly
								onClicked: {
									container.__selectedTargetTenantId = ""
									container.__selectedTargetTenantName = ""
									container.writeModelValues()
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
					editable: !container.isReadOnly
					onSelectionChanged: {
						container.writeModelValues()
					}
				}

				TextInputElementView {
					id: grantDescriptionInput
					name: qsTr("Description")
					placeHolderText: qsTr("Optional description")
					readOnly: container.isReadOnly
					onEditingFinished: {
						container.writeModelValues()
					}
				}

				SwitchElementView {
					id: isActiveSwitch
					name: qsTr("Active")
					checked: container.__isActive
					readOnly: container.isReadOnly
					onCheckedChanged: {
						if (container.__loadingGui)
							return

						container.__isActive = checked
						container.writeModelValues()
					}
				}

				ComboBoxElementView {
					id: expirationCb
					name: qsTr("Expires At")
					description: qsTr("The grant will expire on the selected date")
					nameId: "name"
					model: expirationModel
					changeable: !container.isReadOnly

					TreeItemModel {
						id: expirationModel
						Component.onCompleted: {
							var index = expirationModel.insertNewItem()
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
						var id = expirationModel.getData("id", expirationCb.currentIndex)

						if (id === "unlimited" || id === "" || id === undefined || id === null)
							return ""

						var days = Number(id)
						if (days <= 0)
							return ""

						var d = new Date()
						d.setDate(d.getDate() + days)
						return d.toISOString()
					}

					onCurrentIndexChanged: {
						// Ignore index changes triggered while loading the model into the GUI.
						if (container.__loadingGui)
							return

						container.__selectedExpiresAt = expirationCb.computeExpiresAtIso()
						container.writeModelValues()
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
				container.writeModelValues()
			}
		}
	}
}
