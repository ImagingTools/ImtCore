// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/**
 * SimpleCollectionItemDelegateBase
 *
 * Reusable row for SimpleCollectionTable: selection highlight, checkbox,
 * hover state, double-click to edit and a "more" menu with Edit/Delete.
 *
 * Declared children are placed in the row's content column, so a subclass
 * only describes what is specific to it:
 *
 *     SimpleCollectionItemDelegateBase {
 *         selectionManager: page.selectionManager
 *         actionHandler: page
 *         BaseText { text: parent.itemTitle }
 *     }
 *
 * `actionHandler` is a duck-typed interface - anything providing these:
 *     bool canManage, canEdit, canDelete
 *     openEdit(itemId, itemTitle, itemDescription)
 *     confirmRemoveItems(ids, itemTitle)
 * `SimpleCollectionPage` implements it; a consumer may pass its own object.
 */
Rectangle {
	id: root

	// Array models expose the row as `modelData`; role-based models (ListModel,
	// QAbstractItemModel) expose it as `model` and leave `modelData` undefined.
	// Read whichever exists so the delegate works with either model shape.
	readonly property var modelItem: (typeof modelData !== "undefined" && modelData !== null)
		? modelData
		: (typeof model !== "undefined" ? model : null)

	property string itemId: root.modelItem && root.modelItem.id ? root.modelItem.id : ""
	property string itemTitle: root.modelItem ? (root.modelItem.title || root.modelItem.id || "") : ""
	property string itemDescription: root.modelItem && root.modelItem.description ? root.modelItem.description : ""
	property var itemParameters: root.modelItem ? (root.modelItem.parameters || null) : null

	property var selectionManager: null
	property var actionHandler: null
	property bool canManage: actionHandler ? actionHandler.canManage : false
	property bool canEdit: actionHandler ? actionHandler.canEdit : canManage
	property bool canDelete: actionHandler ? actionHandler.canDelete : canManage
	property bool showCheckBox: true
	property bool enableDefaultDoubleClickEdit: true
	property bool showDefaultActionsMenu: true
	property Component customActionsComponent: null

	// Single click. Rows that are not checkbox-selectable usually act on this
	// (open the ticket, show the revision); rows that are usually do not.
	signal itemClicked()
	signal itemDoubleClicked()

	readonly property bool isSelected: selectionManager ? selectionManager.isSelected(itemId) : false
	readonly property bool isHovered: itemMouseArea.containsMouse
	readonly property int checkBoxSize: Style.itemSizeS + Style.marginXS

	readonly property bool isFirstItem: index === 0

	default property alias content: contentColumn.data

	width: parent ? parent.width : 0
	height: contentRow.implicitHeight + Style.marginXL
	color: root.isSelected ? Style.selectedColor
		: root.isHovered ? Style.buttonHoverColor
		: "transparent"

	MouseArea {
		id: itemMouseArea
		anchors.fill: parent
		hoverEnabled: true
		cursorShape: Qt.PointingHandCursor
		onClicked: root.itemClicked()
		onDoubleClicked: {
			root.itemDoubleClicked()
			if (root.enableDefaultDoubleClickEdit && root.actionHandler && root.canEdit)
				root.actionHandler.openEdit(root.itemId, root.itemTitle, root.itemDescription)
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
			visible: root.showCheckBox
			anchors.verticalCenter: parent.verticalCenter
			height: root.checkBoxSize
			width: visible ? root.checkBoxSize : 0
			checkState: root.isSelected ? Qt.Checked : Qt.Unchecked

			MouseArea {
				anchors.fill: parent
				visible: root.showCheckBox
				cursorShape: Qt.PointingHandCursor
				onClicked: {
					if (root.selectionManager)
						root.selectionManager.toggleSelect(root.itemId)
				}
			}
		}

		Column {
			id: contentColumn
			anchors.verticalCenter: parent.verticalCenter
			spacing: Style.marginXS
			width: parent.width - (root.showCheckBox ? root.checkBoxSize + parent.spacing : 0)
		}
	}

	Item {
		id: trailingActions
		anchors.right: parent.right
		anchors.rightMargin: Style.marginL
		anchors.verticalCenter: parent.verticalCenter
		width: customActionsLoader.visible
			? customActionsLoader.implicitWidth
			: (root.showDefaultActionsMenu ? Style.controlHeightM : 0)
		height: customActionsLoader.visible
			? customActionsLoader.implicitHeight
			: (root.showDefaultActionsMenu ? Style.controlHeightM : 0)

		Loader {
			id: customActionsLoader
			anchors.centerIn: parent
			sourceComponent: root.customActionsComponent
			visible: root.customActionsComponent !== null
		}
	}

	ToolButton {
		id: moreButton
		anchors.centerIn: trailingActions
		width: Style.buttonWidthL
		height: Style.buttonWidthL
		iconSource: "qrc:/" + Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal)
		tooltipText: qsTr("Actions")
		visible: root.showDefaultActionsMenu
			&& (root.canEdit || root.canDelete)
			&& !customActionsLoader.visible
			&& (root.isHovered || root.isSelected)

		onClicked: {
			contextMenuModel.fillModel()
			var point = moreButton.mapToItem(null, 0, moreButton.height)
			ModalDialogManager.openDialog(itemPopupMenuComp, {
				"x": point.x,
				"y": point.y,
				"model": contextMenuModel
			})
		}
	}

	ListModel {
		id: contextMenuModel

		function fillModel() {
			contextMenuModel.clear()
			if (root.canEdit)
				contextMenuModel.append({"id": "Edit", "name": qsTr("Edit"), "icon": "", "isEnabled": true})
			if (root.canDelete)
				contextMenuModel.append({"id": "Delete", "name": qsTr("Delete"), "icon": "", "isEnabled": true})
		}
	}

	Component {
		id: itemPopupMenuComp

		PopupMenuDialog {
			onFinished: {
				if (commandId === "Edit") {
					if (root.actionHandler && root.canEdit)
						root.actionHandler.openEdit(root.itemId, root.itemTitle, root.itemDescription)
				}
				else if (commandId === "Delete") {
					if (root.actionHandler && root.canDelete)
						root.actionHandler.confirmRemoveItems([root.itemId], root.itemTitle)
				}
			}
		}
	}

	Rectangle {
		id: topDivider
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: 1
		color: Style.borderColor
		opacity: 0.5
		visible: !root.isFirstItem
	}
}
