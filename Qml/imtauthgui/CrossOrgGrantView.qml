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
 * Simplified model: TargetTenant, Roles (multi-select), Description, Expires.
 */
ViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	property var apiClient: null
	property var tenantData: null

	property string __selectedTargetTenantId: ""
	property string __selectedTargetTenantName: ""
	property var __selectedRoleIds: []
	property string __selectedRoleNames: ""

	signal grantCreated()

	function updateGui() {
	}

	function updateModel() {
	}

	function submitGrant() {
		if (!container.__selectedTargetTenantId) {
			ModalDialogManager.showInfoDialog(qsTr("Target tenant is required."))
			return
		}
		if (container.__selectedRoleIds.length === 0) {
			ModalDialogManager.showInfoDialog(qsTr("At least one role must be selected."))
			return
		}
		var roleIdsStr = container.__selectedRoleIds.join(";")
		if (container.apiClient) {
			container.apiClient.createCrossOrgGrant(
				container.tenantData ? container.tenantData.m_id : "",
				container.__selectedTargetTenantId,
				roleIdsStr,
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
					name: qsTr("Roles")

					controlComp: Component {
						Row {
							spacing: Style.marginM

							BaseText {
								anchors.verticalCenter: parent.verticalCenter
								text: container.__selectedRoleNames
								  || qsTr("Select roles...")
								color: container.__selectedRoleIds.length > 0
								   ? Style.textColor : Style.inactiveTextColor
								font.pixelSize: Style.fontSizeM
							}

							Button {
								text: qsTr("Select")
								onClicked: {
									ModalDialogManager.openDialog(roleSelectComp, {})
								}
							}
						}
					}
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
		id: roleSelectComp

		FilterableSelectPopup {
			dataProvider: FilterableSelectGqlDataProvider {
				collectionId: "Roles"
				multiSelect: true
			}
			filterPlaceholder: qsTr("Select roles...")
			preselectedIds: container.__selectedRoleIds

			onItemSelected: {
				container.__selectedRoleIds = selectedIds || []
				var names = []
				if (dataProvider) {
					for (var i = 0; i < container.__selectedRoleIds.length; ++i) {
						var n = dataProvider.getSelectedItemText(container.__selectedRoleIds[i])
						if (n) names.push(n)
					}
				}
				container.__selectedRoleNames = names.join(", ")
			}
		}
	}
}
