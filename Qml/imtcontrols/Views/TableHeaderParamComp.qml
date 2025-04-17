import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Dialog {
	id: dialog;

	width: 300;

	title: qsTr("Table configuration");
	property TableViewParams tableViewParamsCopied: TableViewParams {};
	property Item tableItem: null;
	contentComp: Component {
		Column {
			id: item;

			width: 300;

			property var checkedIndexes: [];
			property bool block: true;

			spacing: Style.sizeMainMargin;

			function updateGui(){
				item.block = true;
				leftTable.uncheckAll();
				if (leftTable.elements){
					for (let i = 0; i < leftTable.elements.getItemsCount(); i++){
						let id = leftTable.elements.getData("id", i)

						if (dialog.tableViewParamsCopied.headerIsVisible(id)){
							leftTable.checkItem(i);
						}
					}
				}
				item.block = false;
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
					let id = leftTable.elements.getData("id", i)
					dialog.tableViewParamsCopied.setHeaderOrder(id, i);
				}
			}

			Table {
				id: leftTable;

				anchors.left: parent.left;

				width: parent.width;
				height: Math.min(ModalDialogManager.activeView.height - 200, itemHeight * elementsCount + headerHeight);

				checkable: true;
				onCheckedItemsChanged: {
					item.checkedIndexes = getCheckedItems();
					if (item.block){
						return;
					}

					dialog.setButtonEnabled(Enums.apply, item.checkedIndexes.length > 0)
					errorText.visible = item.checkedIndexes.length < 1;

					for (let i = 0; i < leftTable.elements.getItemsCount(); i++){
						let id = leftTable.elements.getData("id", i)
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
						upButton.enabled = selection[0] > 0;
						downButton.enabled = selection[selection.length - 1] < leftTable.elementsCount - 1;
					}
				}
			}

			BaseText {
				id: errorText;
				anchors.horizontalCenter: parent.horizontalCenter;
				color: Style.errorTextColor;
				text: qsTr("Select at least one column");
				visible: false;
			}

			Item{
				height: 25;
				width: parent.width;

				Row {
					id: buttonRow;

					anchors.left: parent.left;
					anchors.leftMargin: Style.sizeMainMargin;

					height: 25;
					spacing: Style.sizeSmallMargin

					visible: !dialog.tableItem ? false : dialog.tableItem.canSwapColumns;

					ToolButton {
						id: upButton;
						enabled: false;
						width: 25;
						height: 25;
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
						enabled: false;
						width: 25;
						height: 25;
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
						enabled: true;
						width: 25;
						height: 25;
						iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Restore", Icon.State.On, Icon.Mode.Normal):
											  "../../../" + Style.getIconPath("Icons/Restore", Icon.State.Off, Icon.Mode.Disabled)
						onClicked: {
							ModalDialogManager.openDialog(resetDialog, {});
						}
					}


				}

				ToolButton {
					id: fitToWidthButton;

					anchors.verticalCenter: buttonRow.verticalCenter;
					anchors.right: buttonRow.visible ? parent.right : undefined;
					anchors.rightMargin: buttonRow.visible ? Style.sizeMainMargin : 0;
					anchors.horizontalCenter: !buttonRow.visible ? parent.horizontalCenter : undefined;

					visible: !dialog.tableItem ? false : dialog.tableItem.isFlickable;

					enabled: visible;
					width: 25;
					height: 25;

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
			width: 400;
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
