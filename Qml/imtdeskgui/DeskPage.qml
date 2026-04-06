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

	property var documentManager: MainDocumentManager.getDocumentManager("Desk")

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

	Rectangle {
		anchors.fill: parent
		color: Style.backgroundColor2
	}

	MultiPageView {
		id: multiPageView

		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		Component.onCompleted: {
			multiPageView.addPage("Tickets", qsTr("Tickets"), ticketCollectionComp, "Icons/Ticket")
			multiPageView.addPage("Conversations", qsTr("Conversations"), conversationCollectionComp, "Icons/Chat")
			multiPageView.currentIndex = 0
		}
	}
}
