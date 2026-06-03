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
 * Two-step page for activating received connect codes:
 *   Step 1 — Enter the code and click "Check Code" to see a preview.
 *   Step 2 — Review the preview (source tenant, relationship, message) and
 *             click "Confirm Connection" to complete, or "Cancel" to go back.
 */
ViewBase {
	id: redeemCodeView

	contentColor: Style.baseColor

	readonly property var tenantData: redeemCodeView.model
	property var stateManager: null
	property var apiClient: null

	readonly property bool __canManage: redeemCodeView.stateManager
										&& (redeemCodeView.stateManager.isCreator || redeemCodeView.stateManager.isOwner)

	// Preview state
	property var __preview: null
	property bool __showPreview: redeemCodeView.__preview !== null
	property bool __redeemed: false

	function updateGui() {
		// No specific GUI updates needed for this view
	}

	function updateModel() {
		// No model updates needed for this view
	}

	function __reset() {
		redeemCodeView.__preview = null
		redeemCodeView.__redeemed = false
		redeemCodeInput.text = ""
	}

	// --- Subscription-based updates ---
	Connections {
		target: redeemCodeView.apiClient

		function onConnectCodeDetailsReceived(preview) {
			if (!preview) {
				ModalDialogManager.showInfoDialog(qsTr("Connect code not found or already used."))
				return
			}
			redeemCodeView.__preview = {
				"sourceTenantId":    preview.m_sourceTenantId    || "",
				"sourceTenantName":  preview.m_sourceTenantName  || "",
				"proposedSourceRole": preview.m_proposedSourceRole || "",
				"proposedTargetRole": preview.m_proposedTargetRole || "",
				"message":           preview.m_message            || "",
				"expiresAt":         preview.m_expiresAt          || ""
			}
		}

		function onConnectionRequestAccepted(requestId) {
			redeemCodeView.__redeemed = true
			redeemCodeView.__preview = null
			redeemCodeInput.text = ""
			PopupManager.addSuccessMessage(qsTr("Connect code successfully redeemed! Connection established."), true)
			// Refresh connection requests so Active Connections picks up the new connection
			if (redeemCodeView.apiClient && redeemCodeView.tenantData && redeemCodeView.tenantData.m_id) {
				redeemCodeView.apiClient.fetchConnectionRequests(redeemCodeView.tenantData.m_id)
			}
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

			// ================================================================
			// SECTION: Redeem a Connect Code (step 1 — enter code)
			// ================================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Redeem a Connect Code")
				visible: !redeemCodeView.__showPreview && !redeemCodeView.__redeemed
			}

			BaseText {
				width: mainColumn.width
				visible: !redeemCodeView.__showPreview && !redeemCodeView.__redeemed
				wrapMode: Text.WordWrap
				text: qsTr("Paste a connect code received from another tenant. Click \"Check Code\" to preview the connection details before confirming.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Rectangle {
				id: redeemCard
				visible: !redeemCodeView.__showPreview && !redeemCodeView.__redeemed
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
						text: qsTr("Check Code")
						onClicked: {
							var code = redeemCodeInput.text.trim()
							if (code === "") {
								ModalDialogManager.showInfoDialog(qsTr("Connect code is required."))
								return
							}
							if (redeemCodeView.apiClient) {
								redeemCodeView.apiClient.getConnectCodeDetails(code)
							}
						}
					}
				}
			}

			// ================================================================
			// SECTION: Connection Preview (step 2 — confirm or cancel)
			// ================================================================
			GroupHeaderView {
				width: mainColumn.width
				visible: redeemCodeView.__showPreview
				title: qsTr("Connection Preview")
			}

			BaseText {
				width: mainColumn.width
				visible: redeemCodeView.__showPreview
				wrapMode: Text.WordWrap
				text: qsTr("Review the connection details below. Click \"Confirm Connection\" to establish the connection, or \"Cancel\" to go back.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Rectangle {
				id: previewCard
				visible: redeemCodeView.__showPreview
				width: mainColumn.width
				height: previewContent.height + 2 * Style.marginL
				radius: Style.radiusS
				color: Style.alternateBaseColor
				border.color: Style.linkColor
				border.width: 2

				Column {
					id: previewContent
					anchors.left: previewCard.left
					anchors.right: previewCard.right
					anchors.top: previewCard.top
					anchors.margins: Style.marginL
					spacing: Style.marginS

					GroupElementView {
						width: previewContent.width

						TextInputElementView {
							name: qsTr("Source Tenant")
							readOnly: true
							text: {
								var p = redeemCodeView.__preview
								if (!p)
									return ""
								var tName = p.sourceTenantName || ""
								if (tName !== "")
									return tName
								return p.sourceTenantId || ""
							}
						}

						TextInputElementView {
							name: qsTr("Relationship")
							readOnly: true
							text: {
								var p = redeemCodeView.__preview
								if (!p)
									return ""
								return (p.proposedSourceRole || qsTr("Partner"))
									+ " → "
									+ (p.proposedTargetRole || qsTr("Partner"))
							}
						}

						TextInputElementView {
							name: qsTr("Message")
							readOnly: true
							visible: redeemCodeView.__preview && redeemCodeView.__preview.message !== ""
							text: redeemCodeView.__preview ? (redeemCodeView.__preview.message || "") : ""
						}

						TextInputElementView {
							name: qsTr("Expires")
							readOnly: true
							visible: redeemCodeView.__preview && redeemCodeView.__preview.expiresAt !== ""
							text: redeemCodeView.__preview ? (redeemCodeView.__preview.expiresAt || "") : ""
						}
					}

					Row {
						spacing: Style.marginM

						Button {
							enabled: redeemCodeView.__canManage
							text: qsTr("Confirm Connection")
							onClicked: {
								var code = redeemCodeInput.text.trim()
								if (code === "") {
									ModalDialogManager.showInfoDialog(qsTr("Connect code is required."))
									return
								}
								if (redeemCodeView.apiClient) {
									redeemCodeView.apiClient.acceptConnectCode(
										code,
										redeemCodeView.tenantData ? redeemCodeView.tenantData.m_id : "")
								}
							}
						}

						Button {
							text: qsTr("Cancel")
							onClicked: {
								redeemCodeView.__preview = null
							}
						}
					}
				}
			}

			// ================================================================
			// SECTION: Success feedback
			// ================================================================
			Rectangle {
				id: successCard
				visible: redeemCodeView.__redeemed
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

					Button {
						text: qsTr("Redeem Another Code")
						onClicked: {
							redeemCodeView.__reset()
						}
					}
				}
			}

			// ================================================================
			// SECTION: How it works (always visible)
			// ================================================================
			GroupHeaderView {
				width: mainColumn.width
				visible: !redeemCodeView.__redeemed
				title: qsTr("How it works")
			}

			Column {
				width: mainColumn.width
				visible: !redeemCodeView.__redeemed
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
					text: qsTr("2. Paste the code and click \"Check Code\" to preview connection details")
					font.pixelSize: Style.fontSizeS
					color: Style.textColor
				}

				BaseText {
					width: mainColumn.width
					wrapMode: Text.WordWrap
					text: qsTr("3. Review the preview and click \"Confirm Connection\" to establish the connection")
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
