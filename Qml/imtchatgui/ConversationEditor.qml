// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtchatImtChatSdl 1.0
import imtchatConversationCollectionDocumentServiceSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtbaseUndoManagerSdl 1.0

ViewBase {
	id: root

	anchors.fill: parent

	property ConversationData conversationData: model

	function updateGui(){
		nameInput.text = conversationData.m_name
		metadataInput.text = conversationData.m_metadata || ""

		conversationTypeCB.currentIndex = -1
		if (conversationTypeCB.model){
			for (let i = 0; i < conversationTypeCB.model.getItemsCount(); i++){
				let typeId = conversationTypeCB.model.getData("id", i)
				if (conversationData.m_conversationType === typeId){
					conversationTypeCB.currentIndex = i
					break
				}
			}
		}
		if (conversationTypeCB.currentIndex < 0){
			conversationTypeCB.currentIndex = 0
		}
	}

	function updateModel(){
		conversationData.m_name = nameInput.text
		conversationData.m_metadata = metadataInput.text

		if (conversationTypeCB.model && conversationTypeCB.currentIndex >= 0){
			conversationData.m_conversationType = conversationTypeCB.model.getData("id", conversationTypeCB.currentIndex)
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
		visible: root.visible
	}

	CustomScrollbar{
		id: scrollHoriz
		z: parent.z + 1
		anchors.left: flickable.left
		anchors.right: flickable.right
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
		vertical: false
		targetItem: flickable
	}

	Flickable {
		id: flickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: scrollbar.left
		anchors.rightMargin: Style.marginXL
		contentWidth: bodyColumn.width
		contentHeight: Math.max(bodyColumn.height + 2 * Style.marginXL + 100)
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn
			width: 700
			spacing: Style.marginXL

			TextInputElementView {
				id: nameInput

				name: qsTr("Name")
				placeHolderText: qsTr("Enter the conversation name")
				textInputValidator: notEmptyRegexp
				showErrorWhenInvalid: true

				onEditingFinished: {
					root.doUpdateModel()
				}
			}

			RegularExpressionValidator {
				id: notEmptyRegexp
				regularExpression: /^(?!\s*$).+/;
			}

			ComboBoxElementView {
				id: conversationTypeCB
				name: qsTr("Conversation Type")
				currentIndex: 0
				model: TreeItemModel {
					Component.onCompleted: {
						let index = insertNewItem()
						setData("id", "Direct", index)
						setData("name", "Direct (1:1)", index)

						index = insertNewItem()
						setData("id", "Group", index)
						setData("name", "Group", index)

						index = insertNewItem()
						setData("id", "Channel", index)
						setData("name", "Channel", index)

						index = insertNewItem()
						setData("id", "Support", index)
						setData("name", "Support", index)
					}
				}

				onCurrentIndexChanged: {
					root.doUpdateModel()
				}
			}

			TextInputElementView {
				id: metadataInput

				name: qsTr("Metadata")
				placeHolderText: qsTr("Additional metadata (JSON)")

				onEditingFinished: {
					root.doUpdateModel()
				}
			}
		}
	}
}
