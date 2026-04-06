// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtdeskgui 1.0
import imtchatgui 1.0

Item {
	id: deskPageRoot
	objectName: "DeskPage"

	anchors.fill: parent

	property alias multiPageView: multiPageView
	signal multiPageUpdated()

	property var documentManager: MainDocumentManager.getDocumentManager("Desk")

	Component.onCompleted: {
		Events.subscribeEvent("OnLocalizationChanged", deskPageRoot.onLocalizationChanged)
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("OnLocalizationChanged", deskPageRoot.onLocalizationChanged)
	}

	onDocumentManagerChanged: {
		if (documentManager){
			if (documentManager.activeView){
				documentManager.activeView.visualStatusProvider = visualStatusProvider
			}
		}
	}

	GqlBasedObjectVisualStatusProvider {
		id: visualStatusProvider
	}

	function onLocalizationChanged(language){
		let ticketsIndex = multiPageView.getIndexById("Tickets")
		if (ticketsIndex >= 0){
			multiPageView.pagesModel.setProperty(ticketsIndex, "name", qsTr("Tickets"))
		}

		let conversationsIndex = multiPageView.getIndexById("Conversations")
		if (conversationsIndex >= 0){
			multiPageView.pagesModel.setProperty(conversationsIndex, "name", qsTr("Conversations"))
		}
	}

	Rectangle {
		anchors.fill: parent
		color: Style.backgroundColor2
	}

	Component {
		id: ticketCollectionComp

		TicketCollectionView {
			documentManager: deskPageRoot.documentManager
		}
	}

	Component {
		id: conversationCollectionComp

		ConversationCollectionView {
			documentManager: deskPageRoot.documentManager
		}
	}

	MultiPageView {
		id: multiPageView

		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		NavigableItem {
			parentSegment: "Desk"
			paths: ["Tickets", "Conversations"]
			onActivated: {
				let index = paths.indexOf(matchedPath)
				multiPageView.block = true
				multiPageView.currentIndex = index
				if (deskPageRoot.documentManager){
					deskPageRoot.documentManager.closeAllDocuments()
				}

				if (restPath.length >= 2){
					let documentTypeId = restPath[0]
					let documentId = restPath[1]
					deskPageRoot.documentManager.openDocument(documentId, documentTypeId)
				}

				multiPageView.block = false
			}
		}

		Component.onCompleted: {
			MainDocumentManager.registerDocumentManager("Desk/Tickets", deskPageRoot.documentManager)
			MainDocumentManager.registerDocumentManager("Desk/Conversations", deskPageRoot.documentManager)

			multiPageView.addPage("Tickets", qsTr("Tickets"), ticketCollectionComp, "Icons/Ticket")
			multiPageView.addPage("Conversations", qsTr("Conversations"), conversationCollectionComp, "Icons/Chat")
			multiPageView.currentIndex = 0
			deskPageRoot.multiPageUpdated()
		}

		property bool block: false
		onCurrentIndexChanged: {
			if (currentIndex >= 0 && multiPageView.pagesModel.count > currentIndex){
				let pageId = multiPageView.pagesModel.get(currentIndex).id
				visualStatusProvider.collectionId = pageId
				if (!block){
					NavigationController.push("Desk/" + pageId)
				}
			}
		}
	}
}
