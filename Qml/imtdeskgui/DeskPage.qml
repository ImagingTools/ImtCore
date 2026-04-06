import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtchatgui 1.0

ViewBase {
	id: root
	anchors.fill: parent
	commandsControllerComp: Component {
		GqlBasedCommandsController {
			typeId: "Desk"
		}
	}
	
	Component.onCompleted: {
		stackView.addPage(ticketCollectionViewComp)
		stackView.addPage(conversationCollectionViewComp)

		if (commandsController){
			commandsController.setIsToggleable("Ticket", true)
			commandsController.setToggled("Ticket", true)
			commandsController.setIsToggleable("Conversation", true)
			commandsController.setToggled("Conversation", false)
		}
	}

	Component {
		id: conversationCollectionViewComp
		ConversationCollectionView {
		}
	}

	Component {
		id: ticketCollectionViewComp
		TicketCollectionView {
		}
	}

	StackView {
		id: stackView
		anchors.fill: parent
		currentIndex: 0
		adaptSizeToCurrentPage: true
	}
}
