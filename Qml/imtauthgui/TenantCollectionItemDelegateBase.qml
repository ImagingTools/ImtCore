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
	property bool canEdit: collectionPage ? collectionPage.__canEdit : canManage
	property bool canDelete: collectionPage ? collectionPage.__canDelete : canManage
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
	property bool __isRelocatingExternalChildren: false

	// TODO: remove this temporary compatibility shim after web system supports default property alias
	// default property alias contentChildren: contentColumn.data

	function __relocateExternalChildren() {
		if (__isRelocatingExternalChildren)
			return

		__isRelocatingExternalChildren = true
		for (var i = 0; i < delegateRoot.children.length; i++) {
			var child = delegateRoot.children[i]
			if (!child)
				continue
			if (child === itemMouseArea || child === contentRow || child === trailingActions || child === moreButton || child === bottomDivider)
				continue
			child.parent = contentColumn
		}
		__isRelocatingExternalChildren = false
	}

	onChildrenChanged: {
		__relocateExternalChildren()
	}

	Component.onCompleted: {
		__relocateExternalChildren()
	}

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
			if (delegateRoot.enableDefaultDoubleClickEdit && delegateRoot.collectionPage && delegateRoot.canEdit)
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

	ToolButton {
		id: moreButton
		anchors.centerIn: trailingActions
		width: Style.buttonWidthL
		height: Style.buttonWidthL
		iconSource: "../../../" + Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal)
		tooltipText: qsTr("Actions")
		visible: delegateRoot.showDefaultActionsMenu
				 && (delegateRoot.canEdit || delegateRoot.canDelete)
				 && !customActionsLoader.visible
				 && (delegateRoot.isHovered || delegateRoot.isSelected || mouseArea.containsMouse)

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

		function fillModel(){
			contextMenuModel.clear()
			if (delegateRoot.canEdit)
				contextMenuModel.append({"id": "Edit", "name": qsTr("Edit"), "icon": "", "isEnabled": true})
			if (delegateRoot.canDelete)
				contextMenuModel.append({"id": "Delete", "name": qsTr("Delete"), "icon": "", "isEnabled": true})
		}
	}

	Component {
		id: itemPopupMenuComp

		PopupMenuDialog {
			id: itemPopupMenuDialog
			onFinished: {
				if (commandId === "Edit") {
					if (delegateRoot.collectionPage && delegateRoot.canEdit)
						delegateRoot.collectionPage.__openEdit(delegateRoot.itemId, delegateRoot.itemTitle, delegateRoot.itemDescription)
				}
				else if (commandId === "Delete") {
					if (delegateRoot.collectionPage && delegateRoot.canDelete) {
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
	}

	Menu {
		id: itemMenu
		MenuItem {
			text: qsTr("Edit")
			enabled: delegateRoot.canEdit
			onTriggered: {
				if (delegateRoot.collectionPage)
					delegateRoot.collectionPage.__openEdit(delegateRoot.itemId, delegateRoot.itemTitle, delegateRoot.itemDescription)
			}
		}
		MenuItem {
			text: qsTr("Delete")
			enabled: delegateRoot.canDelete
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
		id: bottomDivider
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		height: 1
		color: Style.borderColor
		opacity: 0.5
		visible: !delegateRoot.isLastItem
	}
}
