import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Dialog {
	id: dialog;

	width: Style.sizeHintS;

	title: qsTr("Table configuration");
	property TableViewParams tableViewParamsCopied: TableViewParams {};
	property Item tableItem: null;
	contentComp: Component {
		Column {
			id: item;

			width: dialog.width;
			topPadding: Style.marginL;
			bottomPadding: Style.marginL;

			property var checkedIndexes: [];
			property bool block: true;

			spacing: Style.marginM;

			function updateGui(){
				item.block = true;
				leftTable.uncheckAll();
				if (leftTable.elements){
					for (let i = 0; i < leftTable.elements.getItemsCount(); i++){
						let id = leftTable.elements.getData("m_id", i)

						if (dialog.tableViewParamsCopied.headerIsVisible(id)){
							leftTable.checkItem(i);
						}
					}
				}
				item.block = false;

				item.checkedIndexes = leftTable.getCheckedItems();
				errorText.visible = item.checkedIndexes.length < 1;
			}

			function updateModel(){
				if(dialog.tableItem){
					dialog.tableViewParamsCopied.copyFrom(dialog.tableItem.tableViewParams)
					leftTable.headers = availableHeadersModel;
					leftTable.elements = dialog.tableItem.headers.copyMe();
				}
			}

			function updateHeadersOrder(){
				for (let i = 0; i < leftTable.elements.getItemsCount(); i++){
					let id = leftTable.elements.getData("m_id", i)
					dialog.tableViewParamsCopied.setHeaderOrder(id, i);
				}
			}

			BaseText {
				id: introText;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginL;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginL;
				color: Style.subtitleColor;
				text: qsTr("Choose which columns to show, and drag their order with the arrows below.");
				wrapMode: Text.WordWrap;
			}

			Table {
				id: leftTable;

				anchors.left: parent.left;
				anchors.leftMargin: Style.marginL;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginL;

				height: Math.max(0, Math.min(ModalDialogManager.activeView.height - 200,
										 itemHeight * elementsCount + headerHeight));

				itemHeight: Style.tableRowHeight;
				checkable: true;
				onCheckedItemsChanged: {
					item.checkedIndexes = getCheckedItems();
					if (item.block){
						return;
					}

					dialog.setButtonEnabled(Enums.apply, item.checkedIndexes.length > 0)
					errorText.visible = item.checkedIndexes.length < 1;

					for (let i = 0; i < leftTable.elements.getItemsCount(); i++){
						let id = leftTable.elements.getData("m_id", i)
						let visible = item.checkedIndexes.includes(i);
						let headerSize = dialog.tableViewParamsCopied.getHeaderSize(id)

						dialog.tableViewParamsCopied.setHeaderVisible(id, visible);
						if(!dialog.tableItem.isFlickable || Number(headerSize) === 0){
							dialog.tableViewParamsCopied.setHeaderSize(id, visible ? -1 : 0);
						}
					}
				}

				onElementsChanged: {
					item.updateGui();
				}

				onSelectionChanged: {
					if (selection.length !== 1){
						upButton.enabled = false;
						downButton.enabled = false;
					}
					else{
						let index = selection[0]
						upButton.enabled = index > 0;
						downButton.enabled = index < leftTable.elementsCount - 1;
					}
				}
			}

			BaseText {
				id: errorText;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginL;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginL;
				color: Style.errorTextColor;
				text: qsTr("Select at least one column");
				visible: false;
			}

			Rectangle {
				id: toolbar;

				anchors.left: parent.left;
				anchors.leftMargin: Style.marginL;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginL;

				height: Style.buttonHeightM + 2 * Style.spacingXS;
				radius: Style.radiusM;
				color: Style.baseColor;
				border.width: Style.buttonBorderWidth;
				border.color: Style.borderColor;
				visible: buttonRow.visible || fitToWidthButton.visible;

				Row {
					id: buttonRow;

					anchors.left: parent.left;
					anchors.leftMargin: Style.spacingS;
					anchors.verticalCenter: parent.verticalCenter;

					height: Style.buttonHeightM;
					spacing: Style.spacingXS

					visible: !dialog.tableItem ? false : dialog.tableItem.canSwapColumns;

					ToolButton {
						id: upButton;
						// These 4 ToolButtons are icon-only (text: ""), so Button.qml's default
						// objectName ("" + "Button") would make them indistinguishable from each
						// other - name them explicitly for GUI test/automation addressability.
						objectName: "MoveColumnUpButton";
						enabled: false;
						width: buttonRow.height;
						height: width;
						tooltipText: qsTr("Move column up");
						iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Up", Icon.State.On, Icon.Mode.Normal):
											  "../../../" + Style.getIconPath("Icons/Up", Icon.State.Off, Icon.Mode.Disabled)

						onClicked: {
							let indexes = leftTable.getSelectedIndexes();
							if (indexes.length == 1 && indexes[0] > 0){
								let index = indexes[0];

								let elements = leftTable.elements;
								leftTable.elementsList.model = 0;
								elements.swapItems(index, index - 1);
								dialog.tableViewParamsCopied.swapHeaders(index, index - 1);
								leftTable.elements = elements;
								item.updateHeadersOrder();
								item.updateGui();

								dialog.setButtonEnabled(Enums.apply, true)
								leftTable.select(index - 1)
							}
						}
					}

					ToolButton {
						id: downButton;
						objectName: "MoveColumnDownButton";
						enabled: false;
						width: buttonRow.height;
						height: width;
						tooltipText: qsTr("Move column down");
						iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal):
											  "../../../" + Style.getIconPath("Icons/Down", Icon.State.Off, Icon.Mode.Disabled)
						onClicked: {
							let indexes = leftTable.getSelectedIndexes();
							if (indexes.length == 1 && indexes[indexes.length - 1] < leftTable.elementsCount - 1){
								let index = indexes[0];

								let elements = leftTable.elements;
								leftTable.elementsList.model = 0;
								elements.swapItems(index, index + 1);
								dialog.tableViewParamsCopied.swapHeaders(index, index + 1);
								leftTable.elements = elements;
								item.updateHeadersOrder();
								item.updateGui();

								dialog.setButtonEnabled(Enums.apply, true)

								leftTable.select(index + 1)
							}
						}
					}

					ToolButton {
						id: resetButton;
						objectName: "ResetColumnsButton";
						enabled: true;
						width: buttonRow.height;
						height: width;
						tooltipText: qsTr("Reset to default columns");
						iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Restore", Icon.State.On, Icon.Mode.Normal):
											  "../../../" + Style.getIconPath("Icons/Restore", Icon.State.Off, Icon.Mode.Disabled)
						onClicked: {
							ModalDialogManager.openDialog(resetDialog, {});
						}
					}


				}

				ToolButton {
					id: fitToWidthButton;
					objectName: "FitToWidthButton";

					anchors.verticalCenter: buttonRow.verticalCenter;
					anchors.right: buttonRow.visible ? parent.right : undefined;
					anchors.rightMargin: buttonRow.visible ? Style.marginM : 0;
					anchors.horizontalCenter: !buttonRow.visible ? parent.horizontalCenter : undefined;

					visible: !dialog.tableItem ? false : dialog.tableItem.isFlickable;

					enabled: visible;
					width: buttonRow.height;
					height: width;
					tooltipText: qsTr("Fit columns to available width");

					iconSource: enabled ? "../../../" + Style.getIconPath("Icons/FitToWidth", Icon.State.On, Icon.Mode.Normal):
										  "../../../" + Style.getIconPath("Icons/FitToWidth", Icon.State.Off, Icon.Mode.Disabled)

					onClicked: {
						if(dialog.tableItem){
							dialog.tableItem.fitToWidth();
							dialog.accepted();
						}
					}
				}
			}

			TreeItemModel {
				id: availableHeadersModel;

				Component.onCompleted: {
					let index = availableHeadersModel.insertNewItem();
					availableHeadersModel.setData("id", "name", index)
					availableHeadersModel.setData("name", qsTr("Available Columns"), index)

					item.updateModel()
				}
			}
		}
	}

	Component.onCompleted: {
		addButton(Enums.apply, qsTr("Apply"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	onFinished: {
		if (buttonId === Enums.apply){
			if (contentItem){
				dialog.tableItem.tableViewParams.copyFrom(dialog.tableViewParamsCopied);

				dialog.tableItem.updateWidthFromViewParams();
				dialog.tableItem.fitToWidthOptional();
				dialog.tableItem.tableViewParamsAccepted();
			}
		}
	}

	Component {
		id: resetDialog;
		MessageDialog {
			width: Style.sizeHintM;
			title: qsTr("Confirm reset headers");
			message: qsTr("Reset header settings to default ?");
			onFinished: {
				if (buttonId == Enums.yes){
					dialog.tableItem.resetViewParams();

					dialog.accepted();
				}
			}
		}
	}
}
