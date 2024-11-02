import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0


Dialog {
	id: dialog;

	width: 300;
	height: 400

	title: qsTr("Table configuration");
	property TableViewParams tableViewParamsCopied: TableViewParams {};
	property Item tableItem: null;
	contentComp: Component {
		Column {
			id: item;

			width: 300;

			property var checkedIndexes: [];
			property bool block: true;
			spacing: Style.size_mainMargin;

			function updateGui(){
				item.block = true;
				for (let i = 0; i < leftTable.elements.getItemsCount(); i++){
					let id = leftTable.elements.getData("Id", i)

					if (dialog.tableViewParamsCopied.headerIsVisible(id)){
						leftTable.checkItem(i);
					}
				}
				item.block = false;
			}

			function updateModel(){
				if(dialog.tableItem){
					dialog.tableViewParamsCopied.copyFrom(dialog.tableItem.tableViewParams)
					leftTable.headers = availableHeadersModel;
					leftTable.elements = dialog.tableItem.headers;
				}
			}

			Table {
				id: leftTable;
				anchors.left: parent.left;
				width: parent.width;
				height: itemHeight * elementsCount + headerHeight;
				checkable: true;
				onCheckedItemsChanged: {
					item.checkedIndexes = getCheckedItems();
					if (item.block){
						return;
					}

					dialog.buttonsModel.setProperty(0, 'Enabled', item.checkedIndexes.length > 0)
					errorText.visible = item.checkedIndexes.length < 1;

					for (let i = 0; i < leftTable.elements.getItemsCount(); i++){
						let id = leftTable.elements.getData("Id", i)
						let visible = item.checkedIndexes.includes(i);
						dialog.tableViewParamsCopied.setHeaderVisible(id, visible);
						dialog.tableViewParamsCopied.setHeaderSize(id, visible ? -1 : 0);
					}
				}

				onElementsChanged: {
					item.updateGui();
				}
			}

			BaseText {
				id: errorText;
				anchors.horizontalCenter: parent.horizontalCenter;
				color: Style.errorTextColor;
				text: qsTr("Select at least one column");
				visible: false;
			}

			TreeItemModel {
				id: availableHeadersModel;

				Component.onCompleted: {
					let index = availableHeadersModel.insertNewItem();
					availableHeadersModel.setData("Id", "Name", index)
					availableHeadersModel.setData("Name", qsTr("Available Columns"), index)

					item.updateModel()
				}
			}
		}
	}

	Component.onCompleted: {
		buttonsModel.append({Id: Enums.apply, Name:qsTr("Apply"), Enabled: false})
		buttonsModel.append({Id: Enums.cancel, Name:qsTr("Cancel"), Enabled: true})
	}

	onFinished: {
		if (buttonId === Enums.apply){
			if (contentItem){
				dialog.tableItem.tableViewParams.copyFrom(dialog.tableViewParamsCopied);

				dialog.tableItem.updateWidthFromViewParams();
				dialog.tableItem.tableViewParamsAccepted();
			}
		}
	}
}
