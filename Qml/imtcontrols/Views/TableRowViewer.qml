import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Row {
	id: dataList;

	property TableRowDelegateBase rowDelegate: null;
	property bool compl: false;
	property alias model: repeater.model
	property bool loadDefaultCellDelegate: true;

	Repeater {
		id: repeater

		Component.onCompleted: {
			dataList.compl = true;
		}

		delegate: Item {
			id: cell
			property bool compl: false;
			property bool complCompl: dataList.compl && dataList && dataList.rowDelegate && dataList.rowDelegate.tableItem && dataList.rowDelegate.tableItem.columnCount;
			width: loader.item ? loader.item.width :20
			height: dataList.height;

			clip: true;

			Component.onCompleted: {
				cell.compl = true;
			}

			onComplComplChanged: {
				if(cell.complCompl && dataList && dataList.rowDelegate && dataList.rowDelegate.tableItem){
					let headerId = dataList.rowDelegate.tableItem.headers.getData("id", model.index)
					let contents = dataList.rowDelegate.tableItem.columnContentComps;
					let contentComp = undefined;

					if (dataList.loadDefaultCellDelegate){
						contentComp = dataList.rowDelegate.tableItem.cellDelegate;
					}

					if (Object.keys(contents).includes(headerId)){
						if (contents[headerId]){
							contentComp = contents[headerId];
						}
					}

					if (contentComp === dataList.rowDelegate.tableItem.cellDelegate){
						// console.log("TableRowView error!!!!")
					}

					loader.sourceComponent = contentComp;

				}
			}

			Loader {
				id: loader;
				anchors.fill: parent
				property bool ready: item && dataList.rowDelegate;
				onReadyChanged: {
					if(ready){
						item.columnIndex = model.index
						item.rowDelegate = dataList.rowDelegate
						if(typeof item.setCellWidth === 'function'){
							item.setCellWidth()
						}
					}
				}

			}
		}
	}
}


