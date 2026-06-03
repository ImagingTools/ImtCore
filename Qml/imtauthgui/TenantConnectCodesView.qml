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
			generatedCodeText.visible = true
			copyButton.visible = true
			PopupManager.addSuccessMessage(qsTr("Connect code generated: %1").arg(connectCode), true)
		}
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
			// SECTION: Generate Connect Code
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Generate Connect Code")
			}
			
			GroupElementView {
				id: generateCodeGroup
				width: mainColumn.width
				
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
			
			// --- Generated code display ---
			BaseText {
				id: generatedCodeText
				visible: false
				width: mainColumn.width
				font.pixelSize: Style.fontSizeL
				font.bold: true
				color: Style.linkColor
				wrapMode: Text.WrapAnywhere
				text: ""
			}
			
			Button {
				id: copyButton
				visible: false
				text: qsTr("Copy")
				onClicked: {
					if (generatedCodeText.text && generatedCodeText.text !== "") {
						// TODO: Implement clipboard copy functionality
						// For now, show a message that the user should manually copy
						PopupManager.addInfoMessage(qsTr("Please manually copy the code above"), true)
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
