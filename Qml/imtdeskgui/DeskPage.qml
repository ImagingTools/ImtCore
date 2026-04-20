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

	// Subscribes to server-side ticket message notifications and surfaces
	// them via PopupManager. The server-side filter (CTicketMessageNotifierComp)
	// already restricts delivery to users related to the ticket
	// (reporter / assignees / admin), excluding the sender.
	SubscriptionClient {
		id: ticketMessageSubscription
		gqlCommandId: "OnTicketMessageReceived"
		onMessageReceived: {
			if (!data){
				return
			}
			var ticketNumber = data.containsKey("ticketNumber") ? data.getData("ticketNumber") : ""
			var ticketTitle = data.containsKey("ticketTitle") ? data.getData("ticketTitle") : ""
			var senderName = data.containsKey("senderUserName") ? data.getData("senderUserName") : ""
			var content = data.containsKey("content") ? data.getData("content") : ""
			var messageId = data.containsKey("messageId") ? data.getData("messageId") : ""

			var preview = content ? String(content) : ""
			if (preview.length > 80){
				preview = preview.substring(0, 80) + "…"
			}

			var ticketLabel = ticketNumber ? ("#" + ticketNumber) : ""
			if (ticketTitle){
				ticketLabel = ticketLabel ? (ticketLabel + " " + ticketTitle) : String(ticketTitle)
			}
			var who = senderName ? String(senderName) : qsTr("Someone")
			var header = ticketLabel
				? qsTr("New message in %1 from %2").arg(ticketLabel).arg(who)
				: qsTr("New ticket message from %1").arg(who)
			var text = preview ? (header + ":\n" + preview) : header

			PopupManager.addSuccessMessage(text, true, "TicketMessage_" + messageId)
		}
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
