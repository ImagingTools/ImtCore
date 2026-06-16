// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantCollectionItemDelegateBase
 *
 * Reusable base delegate for TenantSimpleCollectionPage list items.
 * Provides: selection highlight, CheckBox, hover state, double-click to edit,
 * "more" button with Edit/Delete menu.
 *
 * Place custom content inside `contentItem` (default property).
 */
Rectangle {
	id: delegateRoot

	property string itemId: modelData.id || ""
	property string itemTitle: modelData.title || modelData.id || ""
	property string itemDescription: modelData.description || ""
	property var itemParameters: modelData.parameters || null

	property var selectionManager: null
	property var collectionPage: null
	property bool canManage: collectionPage ? collectionPage.__canManage : false
	property bool showCheckBox: true
	property bool enableDefaultDoubleClickEdit: true
	property bool showDefaultActionsMenu: true
	property Component customActionsComponent: null

	signal itemDoubleClicked()

	readonly property bool isSelected: selectionManager ? selectionManager.isSelected(itemId) : false
	readonly property bool isHovered: itemMouseArea.containsMouse
	readonly property int checkBoxSize: Style.itemSizeS + Style.marginXS

	readonly property int index: model.index
	readonly property int totalCount: modelData.totalCount
	readonly property bool isLastItem: index === totalCount - 1

	default property alias contentChildren: contentColumn.data

	width: parent ? parent.width : 0
	height: contentRow.implicitHeight + Style.marginXL

	color: delegateRoot.isSelected ? Style.selectedColor
					  : delegateRoot.isHovered ? Style.buttonHoverColor
								  : "transparent"

	MouseArea {
		id: itemMouseArea
		anchors.fill: parent
		hoverEnabled: true
		cursorShape: Qt.PointingHandCursor
		onDoubleClicked: {
			delegateRoot.itemDoubleClicked()
			if (delegateRoot.enableDefaultDoubleClickEdit && delegateRoot.collectionPage)
				delegateRoot.collectionPage.__openEdit(delegateRoot.itemId, delegateRoot.itemTitle, delegateRoot.itemDescription)
		}
	}

	Row {
		id: contentRow
		anchors.left: parent.left
		anchors.right: trailingActions.left
		anchors.verticalCenter: parent.verticalCenter
		anchors.leftMargin: Style.marginL
		anchors.rightMargin: Style.marginL
		spacing: Style.marginL

		CheckBox {
			id: delegateCheckBox
			visible: delegateRoot.showCheckBox
			anchors.verticalCenter: parent.verticalCenter
			height: delegateRoot.checkBoxSize
			width: visible ? delegateRoot.checkBoxSize : 0
			checkState: delegateRoot.isSelected ? Qt.Checked : Qt.Unchecked

			MouseArea {
				visible: delegateRoot.showCheckBox
				anchors.fill: delegateCheckBox
				cursorShape: Qt.PointingHandCursor
				onClicked: {
					if (delegateRoot.selectionManager)
						delegateRoot.selectionManager.toggleSelect(delegateRoot.itemId)
				}
			}
		}

		Column {
			id: contentColumn
			anchors.verticalCenter: parent.verticalCenter
			spacing: Style.marginXS
			width: parent.width
				   - (delegateRoot.showCheckBox ? delegateRoot.checkBoxSize + parent.spacing : 0)
		}
	}

	Item {
		id: trailingActions
		anchors.right: parent.right
		anchors.rightMargin: Style.marginL
		anchors.verticalCenter: parent.verticalCenter
		width: customActionsLoader.visible
			? customActionsLoader.implicitWidth
			: (delegateRoot.showDefaultActionsMenu ? Style.controlHeightM : 0)
		height: customActionsLoader.visible
			? customActionsLoader.implicitHeight
			: (delegateRoot.showDefaultActionsMenu ? Style.controlHeightM : 0)

		Loader {
			id: customActionsLoader
			anchors.centerIn: parent
			sourceComponent: delegateRoot.customActionsComponent
			visible: !!sourceComponent
		}
	}

	Rectangle {
		id: moreButton
		anchors.centerIn: trailingActions
		width: Style.controlHeightM
		height: Style.controlHeightM
		radius: Style.controlHeightM / 2
		color: moreButtonMA.containsMouse ? Style.buttonHoverColor : "transparent"
		visible: delegateRoot.showDefaultActionsMenu && delegateRoot.canManage
				 && !customActionsLoader.visible
				 && (delegateRoot.isHovered || delegateRoot.isSelected || moreButtonMA.containsMouse)

		Text {
			anchors.centerIn: parent
			text: "\u2026"
			font.pixelSize: Style.fontSizeL
			color: Style.textColor
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
		}

		MouseArea {
			id: moreButtonMA
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: Qt.PointingHandCursor
			onClicked: itemMenu.popup(moreButton.x, moreButton.y + moreButton.height)
		}
	}

	Menu {
		id: itemMenu
		MenuItem {
			text: qsTr("Edit")
			enabled: delegateRoot.canManage
			onTriggered: {
				if (delegateRoot.collectionPage)
					delegateRoot.collectionPage.__openEdit(delegateRoot.itemId, delegateRoot.itemTitle, delegateRoot.itemDescription)
			}
		}
		MenuItem {
			text: qsTr("Delete")
			enabled: delegateRoot.canManage
			onTriggered: {
				if (delegateRoot.collectionPage) {
					ModalDialogManager.showConfirmationDialog(
								delegateRoot.collectionPage.__deleteSingleTitle,
								qsTr("Are you sure you want to delete \"%1\"? This action cannot be undone.").arg(delegateRoot.itemTitle),
								function(result) {
									if (result === Enums.yes)
										delegateRoot.collectionPage.removeItems([delegateRoot.itemId])
								}
								)
				}
			}
		}
	}

	Rectangle {
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		height: 1
		color: Style.borderColor
		opacity: 0.5
		visible: !delegateRoot.isLastItem
	}
}
