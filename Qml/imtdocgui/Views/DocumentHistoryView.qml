// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0

/**
 * DocumentHistoryView
 *
 * A document's revision history, rendered as a SimpleCollectionTable: search,
 * server-side paging and empty/loading/error states come from the table
 * itself. The columns (revision/user/date/description) are a custom
 * `columnHeaderComponent` + `delegateComponent` pair defined here -
 * DocumentRevisionsDataProvider just supplies the raw per-revision fields.
 *
 * Revisions are read-only, so the table's select-all bar and row checkboxes
 * are switched off. Give this item a bounded height - the table pages as the
 * list is scrolled, which needs a viewport smaller than its content.
 */
Rectangle {
	id: container

	color: Style.baseColor
	clip: true

	property string documentId: ""
	property string collectionId: ""

	// Opt-in row selection (used by DocumentRevisionDialog to pick a revision
	// to restore/delete); plain history tabs leave this off.
	property bool selectable: false
	property int selectedRevision: -1
	signal revisionSelected(int revision)

	//! Revisions matching the current filter, across all pages.
	readonly property int revisionsCount: revisionsProvider.totalCount
	//! Revision currently in effect, even when it is not on the loaded page.
	readonly property int activeRevision: revisionsProvider.activeRevision
	readonly property alias contentHeight: historyTable.contentHeight

	readonly property bool __canViewRevisions: PermissionsController.checkPermission("ViewRevisions")

	property QtObject __selectionManager: QtObject {
		function isSelected(id){
			return container.selectable && id === String(container.selectedRevision)
		}
	}

	function getHeaders(){
		return {}
	}

	function sendRequest(){
		if (container.__canViewRevisions){
			revisionsProvider.refresh()
		}
	}

	DocumentRevisionsDataProvider {
		id: revisionsProvider
		documentId: container.documentId
		collectionId: container.collectionId

		function getHeaders(){
			return container.getHeaders()
		}

		onDataChanged: historyTable.model = revisionsProvider.items
	}

	Component.onCompleted: {
		if (container.collectionId !== ""){
			subscriptionClient.gqlCommandId = "On" + container.collectionId + "CollectionChanged"
		}
		container.sendRequest()
	}

	onDocumentIdChanged: container.sendRequest()
	onCollectionIdChanged: {
		if (container.collectionId !== ""){
			subscriptionClient.gqlCommandId = "On" + container.collectionId + "CollectionChanged"
		}
		container.sendRequest()
	}

	SubscriptionClient {
		id: subscriptionClient
		onMessageReceived: container.sendRequest()
	}

	Component {
		id: revisionColumnHeaderComp

		Item {
			height: Style.controlHeightL + Style.marginM

			Rectangle {
				anchors.fill: parent
				color: Style.backgroundColor2
			}

			Row {
				id: headerRow
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.leftMargin: Style.marginL
				anchors.rightMargin: Style.marginL
				anchors.verticalCenter: parent.verticalCenter
				spacing: Style.marginL

				readonly property int columnCount: 4
				readonly property real columnsWidth: width - spacing * (columnCount - 1)

				BaseText {
					width: headerRow.columnsWidth * 0.14
					text: qsTr("Revision")
					font.bold: true
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
				BaseText {
					width: headerRow.columnsWidth * 0.20
					text: qsTr("User")
					font.bold: true
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
				BaseText {
					width: headerRow.columnsWidth * 0.22
					text: qsTr("Date")
					font.bold: true
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
				BaseText {
					width: headerRow.columnsWidth * 0.40
					text: qsTr("Description")
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
		id: revisionRowDelegateComp

		SimpleCollectionItemDelegateBase {
			id: revisionDelegate
			showCheckBox: false
			showDefaultActionsMenu: false
			enableDefaultDoubleClickEdit: false
			selectionManager: container.selectable ? container.__selectionManager : null

			readonly property var revisionItem: revisionDelegate.modelItem

			onItemClicked: {
				if (container.selectable && revisionDelegate.revisionItem)
					container.revisionSelected(revisionDelegate.revisionItem.revision)
			}

			Row {
				id: dataRow
				width: parent.width
				spacing: Style.marginL

				readonly property int columnCount: 4
				readonly property real columnsWidth: width - spacing * (columnCount - 1)

				Row {
					anchors.verticalCenter: parent.verticalCenter
					width: dataRow.columnsWidth * 0.14
					spacing: Style.spacingS
					clip: true

					Text {
						anchors.verticalCenter: parent.verticalCenter
						text: revisionDelegate.revisionItem ? "#" + revisionDelegate.revisionItem.revision : ""
						font.pixelSize: Style.fontSizeL
						font.bold: true
						color: Style.textColor
					}

					Rectangle {
						anchors.verticalCenter: parent.verticalCenter
						visible: revisionDelegate.revisionItem ? revisionDelegate.revisionItem.isActive : false
						width: currentLabel.width + Style.marginL
						height: Style.buttonHeightS
						radius: height / 2
						color: Style.imaginToolsAccentColor

						Text {
							id: currentLabel
							anchors.centerIn: parent
							text: qsTr("Current")
							font.pixelSize: Style.fontSizeM
							font.bold: true
							color: "white"
						}
					}
				}

				Row {
					anchors.verticalCenter: parent.verticalCenter
					width: dataRow.columnsWidth * 0.20
					spacing: Style.spacingS

					Rectangle {
						anchors.verticalCenter: parent.verticalCenter
						visible: revisionDelegate.revisionItem && revisionDelegate.revisionItem.user !== ""
						width: Style.iconSizeS
						height: width
						radius: width / 2
						color: Style.imaginToolsAccentColor

						BaseText {
							anchors.centerIn: parent
							text: revisionDelegate.revisionItem && revisionDelegate.revisionItem.user
								? revisionDelegate.revisionItem.user.charAt(0).toUpperCase()
								: ""
							font.pixelSize: Style.fontSizeS
							font.bold: true
							color: "white"
						}
					}

					Text {
						anchors.verticalCenter: parent.verticalCenter
						width: parent.width - (parent.spacing + Style.iconSizeS)
						text: revisionDelegate.revisionItem ? revisionDelegate.revisionItem.user : ""
						font.pixelSize: Style.fontSizeM
						color: Style.inactiveTextColor
						elide: Text.ElideRight
					}
				}

				Text {
					anchors.verticalCenter: parent.verticalCenter
					width: dataRow.columnsWidth * 0.22
					text: revisionDelegate.revisionItem ? revisionDelegate.revisionItem.timestamp : ""
					font.pixelSize: Style.fontSizeM
					color: Style.inactiveTextColor
					elide: Text.ElideRight
				}

				Text {
					anchors.verticalCenter: parent.verticalCenter
					width: dataRow.columnsWidth * 0.40
					text: revisionDelegate.revisionItem ? revisionDelegate.revisionItem.description : ""
					font.pixelSize: Style.fontSizeM
					color: Style.textColor
					elide: Text.ElideRight
				}
			}
		}
	}

	SimpleCollectionTable {
		id: historyTable
		anchors.fill: parent
		selectionEnabled: false
		// The panel around this view already applies its own margins.
		maximumWidth: container.width
		horizontalMargin: 0
		emptyText: qsTr("No document history yet")
		filterPlaceholder: qsTr("Filter revisions...")
		columnHeaderComponent: revisionColumnHeaderComp
		delegateComponent: revisionRowDelegateComp

		initialLoading: revisionsProvider.isInitialLoading
		loadingMore: revisionsProvider.isPageLoading
		errorMessage: revisionsProvider.error
			? (revisionsProvider.error.message || qsTr("Error loading document history"))
			: ""

		onFilterRequested: revisionsProvider.fetch(text)
		onLoadMoreRequested: revisionsProvider.fetchMore()
		onRetryRequested: revisionsProvider.retry()
	}
}
