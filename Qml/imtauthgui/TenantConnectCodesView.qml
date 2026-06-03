// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtauthgui 1.0

ViewBase {
	id: connectCodesView

	contentColor: Style.baseColor

	readonly property var tenantData: connectCodesView.model
	property var stateManager: null
	property var apiClient: null

	readonly property bool __canManage: connectCodesView.stateManager
		&& (connectCodesView.stateManager.isCreator || connectCodesView.stateManager.isOwner)

	// =========================================================
	// Clipboard helper
	// =========================================================
	TextEdit {
		id: clipboardHelper
		visible: false
	}

	function copyToClipboard(text) {
		if (!text)
			return

		clipboardHelper.text = text
		clipboardHelper.selectAll()
		clipboardHelper.copy()

		PopupManager.addSuccessMessage(
			qsTr("Copied to clipboard"),
			true
		)
	}

	function updateGui() {}
	function updateModel() {}

	// =========================================================
	// API events
	// =========================================================
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

			PopupManager.addSuccessMessage(
				qsTr("Connect code generated successfully"),
				true
			)
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
		anchors.bottom: parent.bottom
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

			// =====================================================
			GroupHeaderView {
				width: parent.width
				title: qsTr("Generate Connect Code")
			}

			BaseText {
				width: parent.width
				wrapMode: Text.WordWrap
				text: qsTr("Generate a one-time code that another tenant can use to establish a connection with you.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			// =====================================================
			// Generate card (FIXED)
			// =====================================================
			Rectangle {
				id: generateCard

				width: parent.width

				radius: Style.radiusS
				color: Style.alternateBaseColor
				border.color: Style.borderColor
				border.width: 1

				readonly property int _pad: Style.marginL

				height: generateContent.implicitHeight + _pad * 2

				Column {
					id: generateContent

					width: parent.width - generateCard._pad * 2

					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.margins: generateCard._pad

					spacing: Style.marginM

					GroupElementView {
						width: parent.width

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
								codeExpiresAtPicker.datePicker.selectedDate.toISOString()
							)
						}
					}
				}
			}

			// =====================================================
			// Generated code card (FIXED)
			// =====================================================
			Rectangle {
				id: generatedCodeCard

				visible: false
				width: parent.width

				radius: Style.radiusS
				color: Style.alternateBaseColor
				border.color: Style.linkColor
				border.width: 2

				readonly property int _pad: Style.marginL

				height: generatedCodeContent.implicitHeight + _pad * 2

				Column {
					id: generatedCodeContent

					width: parent.width - generatedCodeCard._pad * 2

					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.margins: generatedCodeCard._pad

					spacing: Style.marginM

					BaseText {
						text: qsTr("Your Connect Code:")
						font.pixelSize: Style.fontSizeM
						font.bold: true
						color: Style.textColor
					}

					BaseText {
						id: generatedCodeText
						font.pixelSize: Style.fontSizeL
						font.bold: true
						color: Style.linkColor
						wrapMode: Text.WrapAnywhere
						text: ""
					}

					BaseText {
						text: qsTr("Share this code with the tenant you want to connect with. The code can only be used once.")
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
						wrapMode: Text.WordWrap
					}
				}

				ToolButton {
					z: 10

					anchors.top: parent.top
					anchors.right: parent.right
					anchors.topMargin: Style.marginM
					anchors.rightMargin: Style.marginM

					width: Style.buttonWidthM
					height: width

					iconSource: "../../../" +
						Style.getIconPath("Icons/Copy", Icon.State.On, Icon.Mode.Normal)

					tooltipText: qsTr("Copy code")

					onClicked: {
						connectCodesView.copyToClipboard(generatedCodeText.text)
					}
				}
			}

			// =====================================================
			GroupHeaderView {
				width: parent.width
				visible: generatedCodesHistory.count > 0
				title: qsTr("Generated Codes (this session)")
			}

			Column {
				width: parent.width
				spacing: Style.marginS
				visible: generatedCodesHistory.count > 0

				Repeater {
					model: generatedCodesHistory

					delegate: Rectangle {
						id: historyDelegate

						width: parent.width

						radius: Style.radiusS
						color: "transparent"
						border.color: Style.borderColor
						border.width: 1

						readonly property var __item: model

						readonly property int _pad: Style.marginM

						height: historyContent.implicitHeight + _pad * 2

						Item {
							id: historyContent

							width: parent.width - historyDelegate._pad * 2

							anchors.top: parent.top
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.margins: historyDelegate._pad

							BaseText {
								id: historyText

								anchors.left: parent.left
								anchors.verticalCenter: parent.verticalCenter
								anchors.right: historyTimestamp.left
								anchors.rightMargin: Style.marginM

								elide: Text.ElideMiddle
								text: historyDelegate.__item.code || ""

								font.pixelSize: Style.fontSizeM
								color: Style.linkColor
							}

							BaseText {
								id: historyTimestamp

								anchors.right: copyButton.left
								anchors.rightMargin: Style.marginM
								anchors.verticalCenter: parent.verticalCenter

								text: historyDelegate.__item.createdAt || ""

								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}

							ToolButton {
								id: copyButton

								anchors.right: parent.right
								anchors.verticalCenter: parent.verticalCenter

								width: Style.buttonWidthM
								height: width

								iconSource: "../../../" +
									Style.getIconPath("Icons/Copy", Icon.State.On, Icon.Mode.Normal)

								tooltipText: qsTr("Copy code")

								onClicked: {
									connectCodesView.copyToClipboard(
										historyDelegate.__item.code || ""
									)
								}
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
