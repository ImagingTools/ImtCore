// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtbaseImtCollectionSdl 1.0
import imtauthTenantsSdl 1.0

/**
 * ShareDocumentDialog
 *
 * Shares the selected documents of a collection with organizations related to the current one.
 * The organizations come from the active relationships; for a single document each organization
 * shows the access it already has, which can be removed in place.
 * The share request itself is sent by the caller through the shareRequested signal.
 */
Dialog {
	id: shareDialog

	width: Style.sizeHintL
	backgroundColor: Style.baseColor
	title: shareDialog.objectIds.length === 1 && shareDialog.documentName !== ""
		? qsTr("Share \"%1\"").arg(shareDialog.documentName)
		: qsTr("Share %1 documents").arg(shareDialog.objectIds.length)

	property string collectionId: ""
	property var objectIds: []
	property string documentName: ""
	property var permissionPaths: ({})

	property var relatedTenants: []
	property var selectedTenantIds: []
	property var shares: []
	property bool relationshipsLoading: true
	property string errorText: ""
	property string accessLevel: "View"
	property int expirationDays: 0
	property string message: ""
	property int labelWidth: Style.sizeHintBXS

	signal shareRequested(var targetTenantIds, string accessLevel, string validUntil, string message)

	Component.onCompleted: {
		addButton(Enums.ok, qsTr("Share"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	onStarted: {
		shareDialog.setButtonEnabled(Enums.ok, false)
		shareDialog.loadRelatedTenants()
		shareDialog.loadShares()
	}

	onFinished: {
		if (buttonId == Enums.ok && shareDialog.selectedTenantIds.length > 0){
			shareDialog.shareRequested(shareDialog.selectedTenantIds, shareDialog.accessLevel, shareDialog.computeValidUntil(), shareDialog.message.trim())
		}
	}

	Keys.onPressed: {
		if (event.key === Qt.Key_Escape){
			shareDialog.finished(Enums.cancel)
		}
	}

	function loadRelatedTenants(){
		shareDialog.relationshipsLoading = true
		getRelationshipsInput.m_tenantId = AuthorizationController.getTenantId()
		getRelationshipsSender.send(getRelationshipsInput)
	}

	function loadShares(){
		if (shareDialog.objectIds.length !== 1){
			shareDialog.shares = []
			return
		}

		getSharesInput.m_collectionId = shareDialog.collectionId
		getSharesInput.m_objectId = shareDialog.objectIds[0]
		getSharesSender.send(getSharesInput)
	}

	function revokeShare(shareId){
		revokeSharesInput.m_collectionId = shareDialog.collectionId
		revokeSharesInput.m_shareIds = [shareId]
		revokeSharesSender.send(revokeSharesInput)
	}

	function toggleTenant(tenantId){
		let ids = shareDialog.selectedTenantIds.slice()
		let index = ids.indexOf(tenantId)
		if (index >= 0){
			ids.splice(index, 1)
		}
		else{
			ids.push(tenantId)
		}

		shareDialog.selectedTenantIds = ids
		shareDialog.setButtonEnabled(Enums.ok, ids.length > 0)
	}

	function toggleAllTenants(){
		let ids = []
		if (shareDialog.selectedTenantIds.length < shareDialog.relatedTenants.length){
			for (let i = 0; i < shareDialog.relatedTenants.length; i++){
				ids.push(shareDialog.relatedTenants[i].id)
			}
		}

		shareDialog.selectedTenantIds = ids
		shareDialog.setButtonEnabled(Enums.ok, ids.length > 0)
	}

	// shares is passed explicitly so that bindings re-evaluate when the shares are reloaded
	function existingShare(tenantId, shares){
		for (let i = 0; i < shares.length; i++){
			if (shares[i].tenantId === tenantId){
				return shares[i]
			}
		}

		return null
	}

	function accessLevelText(accessLevel){
		return accessLevel === "Edit" ? qsTr("Can edit") : qsTr("Can view")
	}

	function computeValidUntil(){
		if (shareDialog.expirationDays <= 0){
			return ""
		}

		let date = new Date()
		date.setDate(date.getDate() + shareDialog.expirationDays)

		return date.toISOString()
	}

	contentComp: Component {
		Item {
			width: shareDialog.width
			height: bodyColumn.height + 2 * Style.marginL

			Column {
				id: bodyColumn

				anchors.top: parent.top
				anchors.topMargin: Style.marginL
				anchors.left: parent.left
				anchors.leftMargin: Style.marginL
				anchors.right: parent.right
				anchors.rightMargin: Style.marginL

				spacing: Style.marginM

				BaseText {
					width: parent.width
					visible: shareDialog.errorText !== ""
					text: shareDialog.errorText
					color: Style.errorTextColor
					wrapMode: Text.WordWrap
				}

				Item {
					width: parent.width
					height: Style.controlHeightS

					BaseText {
						anchors.left: parent.left
						anchors.verticalCenter: parent.verticalCenter

						text: qsTr("Share with")
						font.pixelSize: Style.fontSizeM
						font.bold: true
					}

					BaseText {
						anchors.right: parent.right
						anchors.verticalCenter: parent.verticalCenter

						visible: shareDialog.relatedTenants.length > 1
						text: shareDialog.selectedTenantIds.length === shareDialog.relatedTenants.length ? qsTr("Clear selection") : qsTr("Select all")
						color: Style.textSelectedColor
						font.pixelSize: Style.fontSizeS

						MouseArea {
							anchors.fill: parent
							cursorShape: Qt.PointingHandCursor

							onClicked: {
								shareDialog.toggleAllTenants()
							}
						}
					}
				}

				Rectangle {
					width: parent.width
					height: shareDialog.relatedTenants.length === 0 ? Style.sizeHintBXS : Math.min(shareDialog.relatedTenants.length, 5) * Style.controlHeightL + 2

					radius: Style.radiusM
					color: Style.baseColor
					border.width: 1
					border.color: Style.borderColor
					clip: true

					BaseText {
						anchors.centerIn: parent
						width: parent.width - 2 * Style.marginL

						visible: shareDialog.relatedTenants.length === 0
						text: shareDialog.relationshipsLoading
							? qsTr("Loading organizations...")
							: qsTr("There are no related organizations. Documents can be shared only with organizations that have an active relationship with yours.")
						color: Style.inactiveTextColor
						horizontalAlignment: Text.AlignHCenter
						wrapMode: Text.WordWrap
					}

					ListView {
						id: tenantsList

						anchors.fill: parent
						anchors.margins: 1

						clip: true
						boundsBehavior: Flickable.StopAtBounds
						model: shareDialog.relatedTenants

						delegate: Rectangle {
							id: tenantRow

							property var share: shareDialog.existingShare(modelData.id, shareDialog.shares)
							property bool selected: shareDialog.selectedTenantIds.indexOf(modelData.id) >= 0

							width: tenantsList.width
							height: Style.controlHeightL

							color: tenantRowMouseArea.containsMouse ? Style.hover : "transparent"

							MouseArea {
								id: tenantRowMouseArea

								anchors.fill: parent
								hoverEnabled: true
								cursorShape: Qt.PointingHandCursor

								onClicked: {
									shareDialog.toggleTenant(modelData.id)
								}
							}

							CheckBox {
								id: tenantCheckBox

								anchors.left: parent.left
								anchors.leftMargin: Style.marginM
								anchors.verticalCenter: parent.verticalCenter

								isActive: false
								checkState: tenantRow.selected ? Qt.Checked : Qt.Unchecked
							}

							BaseText {
								anchors.left: tenantCheckBox.right
								anchors.leftMargin: Style.marginM
								anchors.right: shareStateText.left
								anchors.rightMargin: Style.marginM
								anchors.verticalCenter: parent.verticalCenter

								text: modelData.name
								elide: Text.ElideRight
							}

							BaseText {
								id: shareStateText

								anchors.right: revokeButton.visible ? revokeButton.left : parent.right
								anchors.rightMargin: Style.marginM
								anchors.verticalCenter: parent.verticalCenter

								text: tenantRow.share ? shareDialog.accessLevelText(tenantRow.share.accessLevel) : ""
								color: Style.inactiveTextColor
								font.pixelSize: Style.fontSizeS
							}

							Button {
								id: revokeButton

								anchors.right: parent.right
								anchors.rightMargin: Style.marginM
								anchors.verticalCenter: parent.verticalCenter

								visible: tenantRow.share !== null
								text: qsTr("Remove access")

								onClicked: {
									shareDialog.revokeShare(tenantRow.share.id)
								}
							}

							Rectangle {
								anchors.left: parent.left
								anchors.right: parent.right
								anchors.bottom: parent.bottom

								height: 1
								color: Style.borderColor
								visible: index < shareDialog.relatedTenants.length - 1
							}
						}
					}
				}

				Item {
					width: parent.width
					height: Style.marginXS
				}

				Item {
					width: parent.width
					height: accessColumn.height

					BaseText {
						anchors.left: parent.left
						anchors.top: parent.top
						anchors.topMargin: Style.marginS

						width: shareDialog.labelWidth
						text: qsTr("Access")
						color: Style.inactiveTextColor
					}

					Column {
						id: accessColumn

						anchors.left: parent.left
						anchors.leftMargin: shareDialog.labelWidth
						anchors.right: parent.right

						spacing: Style.marginXS

						SegmentedButton {
							height: Style.controlHeightM
							isExclusive: true

							Button {
								checkable: true
								checked: true
								text: qsTr("Can view")
								widthFromDecorator: true
								heightFromDecorator: true
							}

							Button {
								checkable: true
								text: qsTr("Can edit")
								widthFromDecorator: true
								heightFromDecorator: true
							}

							onClicked: {
								shareDialog.accessLevel = index === 1 ? "Edit" : "View"
							}
						}

						BaseText {
							width: parent.width
							text: shareDialog.accessLevel === "Edit"
								? qsTr("Members of the organizations can open and change the document. Only your organization can delete it.")
								: qsTr("Members of the organizations can open the document, but cannot change it.")
							color: Style.inactiveTextColor
							font.pixelSize: Style.fontSizeS
							wrapMode: Text.WordWrap
						}
					}
				}

				Item {
					width: parent.width
					height: Style.controlHeightM

					BaseText {
						anchors.left: parent.left
						anchors.verticalCenter: parent.verticalCenter

						width: shareDialog.labelWidth
						text: qsTr("Expires")
						color: Style.inactiveTextColor
					}

					ComboBox {
						id: expirationCb

						anchors.left: parent.left
						anchors.leftMargin: shareDialog.labelWidth
						anchors.verticalCenter: parent.verticalCenter

						width: Style.sizeHintXXS
						height: Style.controlHeightM
						model: expirationModel
						currentIndex: 0

						onCurrentIndexChanged: {
							shareDialog.expirationDays = Number(expirationModel.getData("id", expirationCb.currentIndex))
						}
					}
				}

				Item {
					width: parent.width
					height: Style.controlHeightM

					BaseText {
						anchors.left: parent.left
						anchors.verticalCenter: parent.verticalCenter

						width: shareDialog.labelWidth
						text: qsTr("Message")
						color: Style.inactiveTextColor
					}

					CustomTextField {
						id: messageInput

						anchors.left: parent.left
						anchors.leftMargin: shareDialog.labelWidth
						anchors.right: parent.right
						anchors.verticalCenter: parent.verticalCenter

						height: Style.controlHeightM
						placeHolderText: qsTr("Optional note for the organizations")

						onTextEdited: {
							shareDialog.message = messageInput.text
						}

						onEditingFinished: {
							shareDialog.message = messageInput.text
						}
					}
				}
			}
		}
	}

	TreeItemModel {
		id: expirationModel

		Component.onCompleted: {
			let index = expirationModel.insertNewItem()
			expirationModel.setData("id", "0", index)
			expirationModel.setData("name", qsTr("Never"), index)

			index = expirationModel.insertNewItem()
			expirationModel.setData("id", "7", index)
			expirationModel.setData("name", qsTr("In 7 days"), index)

			index = expirationModel.insertNewItem()
			expirationModel.setData("id", "30", index)
			expirationModel.setData("name", qsTr("In 30 days"), index)

			index = expirationModel.insertNewItem()
			expirationModel.setData("id", "90", index)
			expirationModel.setData("name", qsTr("In 90 days"), index)
		}
	}

	GetTenantRelationshipsInput {
		id: getRelationshipsInput
	}

	GqlSdlRequestSender {
		id: getRelationshipsSender

		gqlCommandId: ImtauthTenantsSdlCommandIds.s_getTenantRelationships

		sdlObjectComp: Component {
			GetTenantRelationshipsPayload {
				onFinished: {
					shareDialog.relationshipsLoading = false

					if (m_errorMessage && m_errorMessage !== ""){
						shareDialog.errorText = m_errorMessage
						return
					}

					let currentTenantId = AuthorizationController.getTenantId()
					let tenants = []
					let knownIds = []
					if (m_relationships){
						for (let i = 0; i < m_relationships.count; i++){
							let relationship = m_relationships.get(i).item
							if (!relationship || relationship.m_status !== RelationshipStatusEnum.s_active){
								continue
							}

							let isSource = String(relationship.m_sourceTenantId) === currentTenantId
							let tenantId = isSource ? String(relationship.m_targetTenantId) : String(relationship.m_sourceTenantId)
							let tenantName = isSource ? relationship.m_targetTenantName : relationship.m_sourceTenantName
							if (tenantId !== "" && tenantId !== currentTenantId && knownIds.indexOf(tenantId) < 0){
								knownIds.push(tenantId)
								tenants.push({"id": tenantId, "name": tenantName ? String(tenantName) : tenantId})
							}
						}
					}

					shareDialog.relatedTenants = tenants
				}
			}
		}

		function onError(message, type){
			shareDialog.relationshipsLoading = false
			shareDialog.errorText = message
		}
	}

	GetDocumentSharesInput {
		id: getSharesInput
	}

	GqlSdlRequestSender {
		id: getSharesSender

		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_getDocumentShares
		permissionPath: shareDialog.permissionPaths[gqlCommandId] || ""

		sdlObjectComp: Component {
			GetDocumentSharesPayload {
				onFinished: {
					let shares = []
					if (m_shares){
						for (let i = 0; i < m_shares.count; i++){
							let share = m_shares.get(i).item
							if (share){
								shares.push({
									"id": String(share.m_id),
									"tenantId": String(share.m_targetTenantId),
									"tenantName": share.m_targetTenantName ? String(share.m_targetTenantName) : String(share.m_targetTenantId),
									"accessLevel": String(share.m_accessLevel)
								})
							}
						}
					}

					shareDialog.shares = shares
				}
			}
		}
	}

	RevokeDocumentSharesInput {
		id: revokeSharesInput
	}

	GqlSdlRequestSender {
		id: revokeSharesSender

		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_revokeDocumentShares
		permissionPath: shareDialog.permissionPaths[gqlCommandId] || ""
		requestType: 1

		sdlObjectComp: Component {
			RevokeDocumentSharesPayload {
				onFinished: {
					shareDialog.loadShares()
				}
			}
		}
	}
}
