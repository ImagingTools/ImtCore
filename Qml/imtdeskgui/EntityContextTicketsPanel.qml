// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Qt5Compat.GraphicalEffects 6.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdeskImtDeskSdl 1.0

/**
 * EntityContextTicketsPanel
 *
 * Support tickets attached to one entity (a device, an order, ...), rendered
 * as a SimpleCollectionTable: search and server-side paging come from the
 * table itself. The columns (ticket/status/priority/assignee/created) are a
 * custom `columnHeaderComponent` + `delegateComponent` pair defined here -
 * EntityContextTicketsDataProvider just supplies the raw per-ticket fields.
 *
 * Tickets are opened, not selected in bulk, so the table's select-all bar and
 * row checkboxes are switched off and a single click navigates to the ticket.
 * Filtering and paging happen in CEntityContextTicketsControllerComp, so the
 * panel never holds the whole ticket list.
 */
Item {
	id: root

	property string entityType: ""
	property string entityId: ""
	property string entityDisplayName: ""
	readonly property bool hasValidEntityContext: entityType !== "" && entityId !== ""
	readonly property string resolvedEntityDisplayName: entityDisplayName !== "" ? entityDisplayName : entityId

	property bool creatingTicket: false

	readonly property bool loading: ticketsProvider.isInitialLoading
	readonly property int ticketsCount: ticketsProvider.totalCount

	readonly property string pageBgColor: Style.baseColor
	readonly property string accentColor: Style.imaginToolsAccentColor

	function reloadTickets() {
		ticketsProvider.refresh()
	}

	function openTicket(ticketId) {
		if (!ticketId) {
			return
		}

		NavigationController.navigate("Tickets/Ticket/" + ticketId)
	}

	function createTicket(title, description) {
		var normalizedTitle = title ? title.trim() : ""
		if (!normalizedTitle || !root.hasValidEntityContext || root.creatingTicket) {
			return
		}

		root.creatingTicket = true

		createEntityContextTicketInput.m_entityId = root.entityId
		createEntityContextTicketInput.m_entityType = root.entityType
		createEntityContextTicketInput.m_entityDisplayName = root.resolvedEntityDisplayName
		createEntityContextTicketInput.m_title = normalizedTitle
		createEntityContextTicketInput.m_description = description

		createTicketRequest.send(createEntityContextTicketInput)
	}

	function openCreateTicketPopup() {
		var point = newTicketButton.mapToItem(root, 0, newTicketButton.height)
		createTicketPopup.x = Math.max(0, Math.min(root.width - createTicketPopup.width,
												   Math.max(0, point.x + newTicketButton.width - createTicketPopup.width)))
		createTicketPopup.y = Math.max(0, Math.min(root.height - createTicketPopup.height,
												   point.y + Style.spacingS))
		createTicketPopup.visible = true
		popupTitleInput.forceActiveFocus()
	}

	function closeCreateTicketPopup() {
		if (root.creatingTicket) {
			return
		}

		createTicketPopup.visible = false
	}

	EntityContextTicketsDataProvider {
		id: ticketsProvider
		entityType: root.entityType
		entityId: root.entityId

		onDataChanged: ticketsTable.model = ticketsProvider.items
	}

	onHasValidEntityContextChanged: {
		if (root.hasValidEntityContext) {
			root.reloadTickets()
		}
	}

	Rectangle {
		anchors.fill: parent
		color: root.pageBgColor
	}

	Item {
		id: pageHeader
		anchors.top: parent.top
		anchors.topMargin: Style.marginL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginL
		height: headerActions.height

		Row {
			id: ticketsHeaderLabel
			anchors.left: parent.left
			anchors.right: headerActions.left
			anchors.rightMargin: Style.marginL
			anchors.verticalCenter: parent.verticalCenter
			spacing: Style.spacingS
			clip: true

			Text {
				text: qsTr("Tickets")
				font.pixelSize: Style.fontSizeXL
				font.family: Style.fontFamilyBold
				color: Style.textColor
				anchors.verticalCenter: parent.verticalCenter
			}

			Text {
				visible: root.ticketsCount > 0
				text: "(" + root.ticketsCount + ")"
				font.pixelSize: Style.fontSizeXL
				font.family: Style.fontFamilyBold
				color: root.accentColor
				anchors.verticalCenter: parent.verticalCenter
			}
		}

		Row {
			id: headerActions
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			spacing: Style.spacingS

			ToolButton {
				id: reloadTicketsButton
				anchors.verticalCenter: parent.verticalCenter
				height: Style.buttonHeightS
				width: height
				enabled: !root.loading
				tooltipText: qsTr("Reload tickets")
				iconSource: "qrc:/" + Style.getIconPath("Icons/AutoUpdate", Icon.State.On,
														enabled ? Icon.Mode.Normal : Icon.Mode.Disabled)
				onClicked: root.reloadTickets()
			}

			Button {
				id: newTicketButton
				text: qsTr("New Ticket")
				enabled: !root.creatingTicket
				decorator: Component {
					ButtonDecorator {
						color: Style.imaginToolsAccentColor
						textColor: "white"
						opacity: newTicketButton.hovered ? 0.85 : 1
					}
				}
				onClicked: root.openCreateTicketPopup()
			}
		}
	}

	Component {
		id: ticketColumnHeaderComp

		Item {
			height: Style.controlHeightL + Style.marginM

			Rectangle {
				anchors.fill: parent
				color: Style.backgroundColor2
			}

			Row {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.leftMargin: Style.marginL
				anchors.rightMargin: Style.marginL
				anchors.verticalCenter: parent.verticalCenter
				spacing: Style.marginL

				BaseText {
					width: parent.width * 0.34
					text: qsTr("Ticket")
					font.bold: true
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
				BaseText {
					width: parent.width * 0.14
					text: qsTr("Status")
					font.bold: true
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
				BaseText {
					width: parent.width * 0.14
					text: qsTr("Priority")
					font.bold: true
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
				BaseText {
					width: parent.width * 0.18
					text: qsTr("Assignee")
					font.bold: true
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
				BaseText {
					width: parent.width * 0.14
					text: qsTr("Created")
					font.bold: true
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
			}

			Rectangle {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				height: 1
				color: Style.borderColor
			}
		}
	}

	Component {
		id: ticketRowDelegateComp

		SimpleCollectionItemDelegateBase {
			id: ticketDelegate
			showCheckBox: false
			showDefaultActionsMenu: false
			enableDefaultDoubleClickEdit: false

			readonly property var ticket: ticketDelegate.modelItem
			readonly property var statusColors: ({"Open": "#1a7f37", "Closed": "#8957e5"})
			readonly property var priorityColors: ({"Low": "#3FB950", "Medium": "#D29922", "High": "#DB6D28", "Critical": "#F85149"})

			function formatTimestamp(isoStr) {
				if (!isoStr)
					return ""
				var d = new Date(isoStr)
				if (isNaN(d.getTime()))
					return isoStr
				return d.toLocaleDateString(Qt.locale(), "d MMM yyyy") + " " + d.toLocaleTimeString(Qt.locale(), "HH:mm")
			}

			onItemClicked: root.openTicket(ticketDelegate.itemId)

			Row {
				width: parent.width
				spacing: Style.marginL

				Text {
					anchors.verticalCenter: parent.verticalCenter
					width: parent.width * 0.34
					text: (ticketDelegate.ticket && ticketDelegate.ticket.number
						? "#" + ticketDelegate.ticket.number + "  " : "")
						+ (ticketDelegate.ticket ? ticketDelegate.ticket.title : "")
					font.pixelSize: Style.fontSizeL
					font.bold: true
					color: Style.textColor
					elide: Text.ElideRight
				}

				Item {
					anchors.verticalCenter: parent.verticalCenter
					width: parent.width * 0.14
					height: Style.controlHeightM

					Rectangle {
						anchors.left: parent.left
						anchors.verticalCenter: parent.verticalCenter
						width: statusLabel.implicitWidth + Style.marginM
						height: Style.controlHeightM
						radius: height / 2
						color: ticketDelegate.ticket
							? (ticketDelegate.statusColors[ticketDelegate.ticket.status] || Style.buttonInactiveTextColor)
							: Style.buttonInactiveTextColor
						visible: ticketDelegate.ticket && ticketDelegate.ticket.status !== ""

						Text {
							id: statusLabel
							anchors.centerIn: parent
							text: ticketDelegate.ticket ? qsTr(ticketDelegate.ticket.status) : ""
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: "white"
						}
					}
				}

				Item {
					anchors.verticalCenter: parent.verticalCenter
					width: parent.width * 0.14
					height: Style.controlHeightM

					Rectangle {
						anchors.left: parent.left
						anchors.verticalCenter: parent.verticalCenter
						width: priorityLabel.implicitWidth + Style.marginM
						height: Style.controlHeightM
						radius: height / 2
						color: ticketDelegate.ticket
							? (ticketDelegate.priorityColors[ticketDelegate.ticket.priority] || Style.buttonInactiveTextColor)
							: Style.buttonInactiveTextColor
						visible: ticketDelegate.ticket && ticketDelegate.ticket.priority !== ""

						Text {
							id: priorityLabel
							anchors.centerIn: parent
							text: ticketDelegate.ticket ? qsTr(ticketDelegate.ticket.priority) : ""
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: "white"
						}
					}
				}

				Text {
					anchors.verticalCenter: parent.verticalCenter
					width: parent.width * 0.18
					text: ticketDelegate.ticket ? ticketDelegate.ticket.assignee : ""
					font.pixelSize: Style.fontSizeM
					color: Style.inactiveTextColor
					elide: Text.ElideRight
				}

				Text {
					anchors.verticalCenter: parent.verticalCenter
					width: parent.width * 0.14
					text: ticketDelegate.ticket ? ticketDelegate.formatTimestamp(ticketDelegate.ticket.createdAt) : ""
					font.pixelSize: Style.fontSizeM
					color: Style.inactiveTextColor
					elide: Text.ElideRight
				}
			}
		}
	}

	SimpleCollectionTable {
		id: ticketsTable
		anchors.top: pageHeader.bottom
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		selectionEnabled: false
		maximumWidth: root.width
		horizontalMargin: Style.marginL
		emptyText: qsTr("No tickets yet")
		filterPlaceholder: qsTr("Filter tickets...")
		columnHeaderComponent: ticketColumnHeaderComp
		delegateComponent: ticketRowDelegateComp

		initialLoading: ticketsProvider.isInitialLoading
		loadingMore: ticketsProvider.isPageLoading
		errorMessage: ticketsProvider.error
			? (ticketsProvider.error.message || qsTr("Error loading tickets"))
			: ""

		onFilterRequested: ticketsProvider.fetch(text)
		onLoadMoreRequested: ticketsProvider.fetchMore()
		onRetryRequested: ticketsProvider.retry()
	}

	PopupView {
		id: createTicketPopup
		z: 10
		visible: false
		width: Math.max(0, Math.min(Style.sizeHintM, root.width - 2 * Style.marginL))
		height: popupContent.height + 2 * Style.marginXL

		Rectangle {
			id: popupBackground
			anchors.fill: parent
			radius: Style.radiusL
			border.width: Style.buttonBorderWidth
			border.color: Style.borderColor
			color: Style.baseColor
		}

		DropShadow {
			anchors.fill: popupBackground
			z: popupBackground.z - 1
			horizontalOffset: 3
			verticalOffset: 3
			radius: Style.radiusL
			color: Style.shadowColor
			source: popupBackground
		}

		Column {
			id: popupContent
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.top: parent.top
			anchors.margins: Style.marginXL
			spacing: Style.marginL

			Text {
				width: parent.width
				text: qsTr("Create support ticket")
				font.pixelSize: Style.fontSizeXL
				font.family: Style.fontFamilyBold
				color: Style.textColor
				elide: Text.ElideRight
			}

			Text {
				width: parent.width
				text: qsTr("Describe the issue for %1. Support will receive the ticket with a link to this item.").arg(root.resolvedEntityDisplayName)
				font.pixelSize: Style.fontSizeM
				font.family: Style.fontFamily
				color: Style.inactiveTextColor
				wrapMode: Text.WrapAnywhere
			}

			Column {
				width: parent.width
				spacing: Style.spacingS

				Text {
					text: qsTr("Title")
					font.pixelSize: Style.fontSizeS
					font.family: Style.fontFamilyBold
					color: Style.textColor
				}

				CustomTextField {
					id: popupTitleInput
					width: parent.width
					height: Style.controlHeightM
					placeHolderText: qsTr("Short summary of the issue")
					KeyNavigation.tab: popupDescriptionInput
					onAccepted: root.createTicket(popupTitleInput.text, popupDescriptionInput.text)
					onCancelled: root.closeCreateTicketPopup()
				}
			}

			Column {
				width: parent.width
				spacing: Style.spacingS

				Text {
					text: qsTr("Description")
					font.pixelSize: Style.fontSizeS
					font.family: Style.fontFamilyBold
					color: Style.textColor
				}

				Rectangle {
					width: parent.width
					height: 150
					radius: Style.radiusM
					border.width: popupDescriptionInput.activeFocus ? 2 : Style.buttonBorderWidth
					border.color: popupDescriptionInput.activeFocus ? root.accentColor : Style.borderColor
					color: Style.baseColor

					Flickable {
						id: popupDescriptionFlick
						anchors.fill: parent
						anchors.margins: Style.paddingM
						anchors.rightMargin: Style.paddingM + Style.marginM
						contentWidth: width
						contentHeight: popupDescriptionInput.height
						clip: true
						boundsBehavior: Flickable.StopAtBounds

						TextEdit {
							id: popupDescriptionInput
							width: popupDescriptionFlick.width
							height: Math.max(popupDescriptionFlick.height, contentHeight)
							font.pixelSize: Style.fontSizeM
							font.family: Style.fontFamily
							color: Style.textColor
							wrapMode: TextEdit.Wrap
							textFormat: TextEdit.PlainText
							KeyNavigation.backtab: popupTitleInput

							Keys.onPressed: {
								if (event.key === Qt.Key_Escape) {
									root.closeCreateTicketPopup()
									event.accepted = true
								}
							}

							Text {
								anchors.fill: parent
								text: qsTr("Steps, expected result, and any useful context")
								font.pixelSize: Style.fontSizeM
								font.family: Style.fontFamily
								color: Style.inactiveTextColor
								wrapMode: Text.WrapAnywhere
								visible: popupDescriptionInput.text.length === 0
							}
						}
					}

					CustomScrollbar {
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.bottom: parent.bottom
						anchors.margins: Style.spacingXXS
						secondSize: Style.marginM
						targetItem: popupDescriptionFlick
						visible: popupDescriptionFlick.contentHeight > popupDescriptionFlick.height
					}
				}
			}

			Row {
				anchors.right: parent.right
				spacing: Style.spacingS

				Button {
					text: qsTr("Cancel")
					onClicked: root.closeCreateTicketPopup()
				}

				Button {
					id: popupCreateButton
					text: root.creatingTicket ? qsTr("Creating...") : qsTr("Create Ticket")
					enabled: popupTitleInput.text.trim().length > 0 && !root.creatingTicket
					widthFromDecorator: true
					decorator: Component {
						ButtonDecorator {
							color: popupCreateButton.enabled ? Style.imaginToolsAccentColor : Style.baseColor
							textColor: popupCreateButton.enabled ? "white" : Style.inactiveTextColor
							opacity: popupCreateButton.hovered ? 0.85 : 1
						}
					}
					onClicked: root.createTicket(popupTitleInput.text, popupDescriptionInput.text)
				}
			}
		}
	}

	CreateEntityContextTicketInput {
		id: createEntityContextTicketInput
	}

	GqlSdlRequestSender {
		id: createTicketRequest
		gqlCommandId: ImtdeskImtDeskSdlCommandIds.s_createEntityContextTicket
		requestType: 1

		sdlObjectComp: Component {
			CreateEntityContextTicketPayload {
				onFinished: {
					root.creatingTicket = false
					root.closeCreateTicketPopup()
					PopupManager.addSuccessMessage(qsTr("Ticket successfully created"), true)
					root.reloadTickets()
				}
			}
		}

		onFinished: {
			if (status < 0) {
				root.creatingTicket = false
				PopupManager.addErrorMessage(qsTr("Failed to create the ticket. Please try again."), true)
			}
		}
	}
}
