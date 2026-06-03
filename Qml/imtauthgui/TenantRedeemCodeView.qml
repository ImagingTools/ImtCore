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
 * TenantRedeemCodeView
 *
 * Page for activating received connect codes.
 * Card-based layout with clear instructions and feedback.
 */
ViewBase {
	id: redeemCodeView

	contentColor: Style.baseColor

	readonly property var tenantData: redeemCodeView.model
	property var stateManager: null
	property var apiClient: null
	
	readonly property bool __canManage: redeemCodeView.stateManager
										&& (redeemCodeView.stateManager.isCreator || redeemCodeView.stateManager.isOwner)
	
	function updateGui() {
		// No specific GUI updates needed for this view
	}
	
	function updateModel() {
		// No model updates needed for this view
	}
	
	// --- Real-time subscription-based updates ---
	Connections {
		target: redeemCodeView.apiClient
		
		function onConnectionRequestAccepted(requestId) {
			PopupManager.addSuccessMessage(qsTr("Connect code successfully redeemed! Connection established."), true)
			redeemCodeInput.text = ""
			successCard.visible = true
		}
		
		function onConnectionRequestRejected(requestId) {
			PopupManager.addErrorMessage(qsTr("Failed to redeem connect code. The code may be expired or invalid."), true)
		}
	}
	
	CustomScrollbar {
		id: scrollbar
		z: redeemCodeView.z + 1
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
			// SECTION: Redeem a Connect Code
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Redeem a Connect Code")
			}
			
			BaseText {
				width: mainColumn.width
				wrapMode: Text.WordWrap
				text: qsTr("Enter a connect code you received from another tenant to establish a connection. The code will be consumed after successful redemption.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}
			
			Rectangle {
				id: redeemCard
				width: mainColumn.width
				height: redeemContent.height + 2 * Style.marginL
				radius: Style.radiusS
				color: Style.alternateBaseColor
				border.color: Style.borderColor
				border.width: 1
				
				Column {
					id: redeemContent
					anchors.left: redeemCard.left
					anchors.right: redeemCard.right
					anchors.top: redeemCard.top
					anchors.margins: Style.marginL
					spacing: Style.marginM
					
					GroupElementView {
						id: redeemGroup
						width: redeemContent.width
						
						TextInputElementView {
							id: redeemCodeInput
							name: qsTr("Connect Code")
							placeHolderText: qsTr("Paste the connect code here")
						}
					}
					
					Button {
						enabled: redeemCodeView.__canManage && redeemCodeInput.text.trim() !== ""
						text: qsTr("Redeem Code")
						onClicked: {
							var code = redeemCodeInput.text.trim()
							if (code === "") {
								ModalDialogManager.showInfoDialog(qsTr("Connect code is required."))
								return
							}
							redeemCodeView.apiClient.acceptConnectCode(
										code,
										redeemCodeView.tenantData ? redeemCodeView.tenantData.m_id : "")
						}
					}
				}
			}
			
			// --- Success feedback card ---
			Rectangle {
				id: successCard
				visible: false
				width: mainColumn.width
				height: successContent.height + 2 * Style.marginL
				radius: Style.radiusS
				color: Style.alternateBaseColor
				border.color: Style.linkColor
				border.width: 2
				
				Column {
					id: successContent
					anchors.left: successCard.left
					anchors.right: successCard.right
					anchors.top: successCard.top
					anchors.margins: Style.marginL
					spacing: Style.marginS
					
					BaseText {
						width: successContent.width
						text: qsTr("Connection Established!")
						font.pixelSize: Style.fontSizeM
						font.bold: true
						color: Style.linkColor
					}
					
					BaseText {
						width: successContent.width
						wrapMode: Text.WordWrap
						text: qsTr("The connect code was redeemed successfully. You can view the new connection in the Active Connections page.")
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}
				}
			}
			
			// --- Instructions ---
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("How it works")
			}
			
			Column {
				width: mainColumn.width
				spacing: Style.marginS
				
				BaseText {
					width: mainColumn.width
					wrapMode: Text.WordWrap
					text: qsTr("1. Receive a connect code from another tenant")
					font.pixelSize: Style.fontSizeS
					color: Style.textColor
				}
				
				BaseText {
					width: mainColumn.width
					wrapMode: Text.WordWrap
					text: qsTr("2. Paste the code in the field above")
					font.pixelSize: Style.fontSizeS
					color: Style.textColor
				}
				
				BaseText {
					width: mainColumn.width
					wrapMode: Text.WordWrap
					text: qsTr("3. Click \"Redeem Code\" to establish the connection")
					font.pixelSize: Style.fontSizeS
					color: Style.textColor
				}
				
				BaseText {
					width: mainColumn.width
					wrapMode: Text.WordWrap
					text: qsTr("4. The connection will appear in Active Connections")
					font.pixelSize: Style.fontSizeS
					color: Style.textColor
				}
			}
		}
	}
}
