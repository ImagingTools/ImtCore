import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtchatgui 1.0
import imtdeskgui 1.0

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
			commandsController.setIsToggleable("Tickets", true)
			commandsController.setToggled("Tickets", true)
			commandsController.setIsToggleable("Conversations", true)
			commandsController.setToggled("Conversations", false)
		}

		stackView.setCurrentIndex(0)
	}

	onCommandActivated: {
		checkCurrentPage(commandId)
	}

	function checkCurrentPage(commandId){
		if (commandId === "Tickets"){
			stackView.setCurrentIndex(0)
		}
		else if (commandId === "Conversations"){
			stackView.setCurrentIndex(1)
		}

		commandsController.setToggled("Tickets", commandId === "Tickets")
		commandsController.setToggled("Conversations", commandId === "Conversations")
	}

	Component {
		id: conversationCollectionViewComp
		ConversationCollectionView {
			anchors.fill: parent
		}
	}

	Component {
		id: ticketCollectionViewComp
		TicketCollectionView {
			anchors.fill: parent
		}
	}

	StackView {
		id: stackView
		anchors.fill: parent
	}
}
