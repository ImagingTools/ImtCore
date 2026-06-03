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
 * TenantConnectCodesView
 *
 * Page for generating and managing one-time connect codes.
 * Card-based layout with generated code display and history.
 */
ViewBase {
	id: connectCodesView

	contentColor: Style.baseColor

	readonly property var tenantData: connectCodesView.model
	property var stateManager: null
	property var apiClient: null
	
	readonly property bool __canManage: connectCodesView.stateManager
										&& (connectCodesView.stateManager.isCreator || connectCodesView.stateManager.isOwner)
	
	function updateGui() {
		// No specific GUI updates needed for this view
	}
	
	function updateModel() {
		// No model updates needed for this view
	}
	
	// --- Real-time subscription-based updates ---
	Connections {
		target: connectCodesView.apiClient
		
		function onConnectCodeCreated(requestId, connectCode) {
			generatedCodeText.text = connectCode
			generatedCodeCard.visible = true
			generatedCodesHistory.append({
				"code": connectCode,
				"requestId": requestId,
				"createdAt": new Date().toLocaleString()
			})
			PopupManager.addSuccessMessage(qsTr("Connect code generated successfully"), true)
		}
	}

	ListModel {
		id: generatedCodesHistory
	}

	CustomScrollbar {
		id: scrollbar
		z: connectCodesView.z + 1
		anchors.right: parent.right
		anchors.top: mainFlickable.top
		anchors.bottom: mainFlickable.bottom
		secondSize: Style.marginM
		targetItem: mainFlickable
	}

	Flickable {
		id: mainFlickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, Style.sizeHintXXL)
		contentWidth: mainColumn.width
		contentHeight: mainColumn.height + 2 * Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true
		
		Column {
			id: mainColumn
			width: mainFlickable.width
			spacing: Style.marginXL
			
			// =============================================================
			// SECTION: Generate Connect Code (card)
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Generate Connect Code")
			}
			
			BaseText {
				width: mainColumn.width
				wrapMode: Text.WordWrap
				text: qsTr("Generate a one-time code that another tenant can use to establish a connection with you.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}
			
			Rectangle {
				id: generateCard
				width: mainColumn.width
				height: generateContent.height + 2 * Style.marginL
				radius: Style.radiusS
				color: Style.alternateBaseColor
				border.color: Style.borderColor
				border.width: 1
				
				Column {
					id: generateContent
					anchors.left: generateCard.left
					anchors.right: generateCard.right
					anchors.top: generateCard.top
					anchors.margins: Style.marginL
					spacing: Style.marginM
					
					GroupElementView {
						id: generateCodeGroup
						width: generateContent.width
						
						ComboBoxElementView {
							id: codeSourceRoleCB
							name: qsTr("Source Role")
							model: connRoleModel
							currentIndex: 2
						}
						
						ComboBoxElementView {
							id: codeTargetRoleCB
							name: qsTr("Target Role")
							model: connRoleModel
							currentIndex: 2
						}
						
						TextInputElementView {
							id: codeMessageInput
							name: qsTr("Message")
							placeHolderText: qsTr("Optional message to include")
						}
						
						DateTimePickerElementView {
							id: codeExpiresAtPicker
							name: qsTr("Expires At")
						}
					}
					
					Button {
						enabled: connectCodesView.__canManage
						text: qsTr("Generate Code")
						onClicked: {
							var roleTokens = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
							var srcIdx = codeSourceRoleCB.currentIndex >= 0 ? codeSourceRoleCB.currentIndex : 2
							var tgtIdx = codeTargetRoleCB.currentIndex >= 0 ? codeTargetRoleCB.currentIndex : 2
							connectCodesView.apiClient.createConnectCode(
										connectCodesView.tenantData ? connectCodesView.tenantData.m_id : "",
										roleTokens[srcIdx],
										roleTokens[tgtIdx],
										codeMessageInput.text.trim(),
										codeExpiresAtPicker.getDateAsString())
						}
					}
				}
			}
			
			// =============================================================
			// SECTION: Generated Code Display (card, shown after generation)
			// =============================================================
			Rectangle {
				id: generatedCodeCard
				visible: false
				width: mainColumn.width
				height: generatedCodeContent.height + 2 * Style.marginL
				radius: Style.radiusS
				color: Style.alternateBaseColor
				border.color: Style.linkColor
				border.width: 2
				
				Column {
					id: generatedCodeContent
					anchors.left: generatedCodeCard.left
					anchors.right: generatedCodeCard.right
					anchors.top: generatedCodeCard.top
					anchors.margins: Style.marginL
					spacing: Style.marginM
					
					BaseText {
						width: generatedCodeContent.width
						text: qsTr("Your Connect Code:")
						font.pixelSize: Style.fontSizeM
						font.bold: true
						color: Style.textColor
					}
					
					BaseText {
						id: generatedCodeText
						width: generatedCodeContent.width
						font.pixelSize: Style.fontSizeL
						font.bold: true
						color: Style.linkColor
						wrapMode: Text.WrapAnywhere
						text: ""
					}
					
					BaseText {
						width: generatedCodeContent.width
						wrapMode: Text.WordWrap
						text: qsTr("Share this code with the tenant you want to connect with. The code can only be used once.")
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}
				}
			}
			
			// =============================================================
			// SECTION: Generated Codes History
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				visible: generatedCodesHistory.count > 0
				title: qsTr("Generated Codes (this session)")
			}
			
			Column {
				width: mainColumn.width
				spacing: Style.marginS
				visible: generatedCodesHistory.count > 0
				
				Repeater {
					id: historyRepeater
					model: generatedCodesHistory
					
					delegate: Rectangle {
						id: historyDelegate
						width: mainColumn.width
						height: historyContent.height + 2 * Style.marginS
						radius: Style.radiusS
						color: "transparent"
						border.color: Style.borderColor
						border.width: 1
						
						readonly property var __item: model
						
						Row {
							id: historyContent
							anchors.left: historyDelegate.left
							anchors.right: historyDelegate.right
							anchors.verticalCenter: historyDelegate.verticalCenter
							anchors.margins: Style.marginM
							spacing: Style.marginM
							
							BaseText {
								width: historyContent.width - historyTimestamp.width - Style.marginM
								elide: Text.ElideMiddle
								text: historyDelegate.__item.code || ""
								font.pixelSize: Style.fontSizeM
								color: Style.linkColor
							}
							
							BaseText {
								id: historyTimestamp
								text: historyDelegate.__item.createdAt || ""
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}
						}
					}
				}
			}
		}
	}
	
	TreeItemModel {
		id: connRoleModel
		Component.onCompleted: {
			var roles = ["Parent", "Child", "Partner", "Supplier", "Customer", "Affiliate"]
			for (var i = 0; i < roles.length; i++) {
				var idx = connRoleModel.insertNewItem()
				connRoleModel.setData("id", roles[i], idx)
				connRoleModel.setData("name", roles[i], idx)
			}
		}
	}
}
