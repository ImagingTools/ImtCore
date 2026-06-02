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
 * ContractView
 *
 * ViewBase-inherited editor for creating cooperation contracts.
 * Follows the RoleView / UserGroupView pattern with GqlBasedCommandsController.
 * The commands controller provides the Save button at the top.
 */
ViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	property var apiClient: null
	property var tenantData: null

	property string __selectedRelationshipId: ""
	property string __selectedRelationshipName: ""
	property string __selectedTargetTenantId: ""
	property string __selectedTargetTenantName: ""

	signal contractCreated()

	function updateGui() {
	}

	function updateModel() {
	}

	function submitContract() {
		if (!container.__selectedRelationshipId || !container.__selectedTargetTenantId) {
			ModalDialogManager.showInfoDialog(qsTr("Relationship and counterparty tenant are required."))
			return
		}
		if (container.apiClient) {
			container.apiClient.createContract(
				container.__selectedRelationshipId,
				container.tenantData ? container.tenantData.m_id : "",
				container.__selectedTargetTenantId,
				contractScopeInput.text.trim(),
				validFromPicker.getDateAsString(),
				validUntilPicker.getDateAsString(),
				contractDescriptionInput.text.trim(),
				contractTermsInput.text.trim())
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
				title: qsTr("Cooperation Contract")
			}

			GroupElementView {
				id: generalGroup
				width: parent.width

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

				ElementView {
					name: qsTr("Counterparty Tenant")

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

				TextInputElementView {
					id: contractScopeInput
					name: qsTr("Scope")
					placeHolderText: qsTr("Optional — agreed scope of cooperation")
				}

				DateTimePickerElementView {
					id: validFromPicker
					name: qsTr("Valid From")
				}

				DateTimePickerElementView {
					id: validUntilPicker
					name: qsTr("Valid Until")
				}

				TextInputElementView {
					id: contractDescriptionInput
					name: qsTr("Description")
					placeHolderText: qsTr("Optional description")
				}

				TextInputElementView {
					id: contractTermsInput
					name: qsTr("Terms")
					placeHolderText: qsTr("Optional free-form terms metadata")
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

			onItemSelected: function(itemId, index) {
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

			onItemSelected: function(itemId, index) {
				container.__selectedRelationshipId = itemId
				container.__selectedRelationshipName = dataProvider
					? dataProvider.getSelectedItemText(itemId) : ""
			}
		}
	}
}
